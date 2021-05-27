// (C) Copyright 2016-2021 Xilinx, Inc.
// All Rights Reserved.
//
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.


#include "loop_tiling.h"

#include <limits>
#include <tuple>

#include "PolyModel.h"
#include "cmdline_parser.h"
#include "codegen.h"
#include "file_parser.h"
#include "mars_ir_v2.h"
#include "mars_message.h"
#include "mars_opt.h"
#include "tldm_annotate.h"
#include "xml_parser.h"

#include "analPragmas.h"
#include "bsuNode.h"
#include "ir_types.h"

using MarsProgramAnalysis::CMarsExpression;
using MarsProgramAnalysis::CMarsRangeExpr;
using MarsProgramAnalysis::CMarsVariable;
using std::cout;
using std::endl;
using std::map;
using std::set;
using std::string;
using std::tuple;
using std::vector;
//  Overall Processing Algorithms
//
//  1. find all the loops with partial parallelization pragma;
//    nested partial parallelized loop(with a divisible factor) will be grouped;
//    unsupported loop will be ignored.
//
//  2. for each nest
//  2.1 get the tiling factor by parsing the pragma
//  2.2 get the loop body
//  2.3 create the declaration of the original iterators, in the first statement
//  of body 2.4 create the new loop nest 2.5 replace the orignal iterator
//  references 2.6 update original loop bound and remove its pragma 2.7 apply
//  parallel pragma in the new sub loops

void loop_tiling_preprocessing(CSageCodeGen *codegen, void *pTopFunc) {
  SgProject *project = static_cast<SgProject *>(pTopFunc);
  for (size_t i = 0; i < project->get_fileList().size(); i++) {
    SgSourceFile *file = isSgSourceFile(project->get_fileList()[i]);
    if ((codegen->IsFromInputFile(file) == 0) ||
        codegen->isWithInHeaderFile(file)) {
      continue;
    }
    //  add missing brace
    codegen->add_missing_brace(file);
  }

  codegen->remove_double_brace();
}

static bool has_only_pragma_and_one_loop(CSageCodeGen *codegen,
                                         void *loop_body) {
  unsigned norm_stmt_num = 0;
  for (size_t i = 0; i < codegen->GetChildStmtNum(loop_body); ++i) {
    if (codegen->IsPragmaDecl(codegen->GetChildStmt(loop_body, i)) != 0) {
      continue;
    }
    norm_stmt_num++;
    if (norm_stmt_num > 1) {
      return false;
    }
  }
  return true;
}

static bool has_no_pragma(CSageCodeGen *codegen, void *loop_body) {
  for (size_t i = 0; i < codegen->GetChildStmtNum(loop_body); ++i) {
    if (codegen->IsPragmaDecl(codegen->GetChildStmt(loop_body, i)) != 0) {
      return true;
    }
  }
  return false;
}

static bool has_no_loop_tiled_tag(CSageCodeGen *codegen, void *loop_body) {
  for (size_t i = 0; i < codegen->GetChildStmtNum(loop_body); ++i) {
    void *stmt = codegen->GetChildStmt(loop_body, i);
    if (codegen->IsPragmaDecl(stmt) != 0) {
      CAnalPragma ana_pragma(codegen);
      bool errorOut = false;
      ana_pragma.PragmasFrontendProcessing(stmt, &errorOut, false);
      if (ana_pragma.is_loop_tiled_tag()) {
        return false;
      }
    }
  }
  return true;
}

void collect_tripcount_pragma(CSageCodeGen *codegen, CMarsIr *mars_ir,
                              void *scope, void **sg_bound_pragma,
                              int64_t *trip_count) {
  Rose_STL_Container<SgNode *> pragmaStmts = NodeQuery::querySubTree(
      static_cast<SgNode *>(scope), V_SgPragmaDeclaration);
  for (size_t j = 0; j != pragmaStmts.size(); ++j) {
    void *sg_pragma = pragmaStmts[j];
    void *loop_scope = codegen->TraceUpToLoopScope(sg_pragma);
    if (loop_scope != scope) {
      continue;
    }
    if (mars_ir->isHLSTripCount(sg_pragma)) {
      *sg_bound_pragma = sg_pragma;
      codegen->get_loop_trip_count_from_pragma(loop_scope, trip_count);
      cout << "Parse loop range from trip count pragma: " << *trip_count
           << endl;
      return;
    }
  }
}

bool collect_all_loop_info(
    CSageCodeGen *codegen, void *pTopFunc, CMarsIr *mars_ir,
    map<void *, vector<string>> *loop2remain_pragmas,
    map<void *, int64_t> *loop2c_len, map<void *, void *> *loop2len,
    map<void *, int> *loop2is_divisible, map<void *, int> *loop2is_zero_lb,
    map<void *, CMarsExpression> *loop2bound_offset,
    map<void *, CAnalPragma *> *loop2pragma, map<void *, int> *new_tripcount,
    vector<void *> *loop2stopfgpip,
    vector<vector<pair<void *, pair<int, bool>>>> *vec_loop_nest,
    bool pragma_in_loop, bool *pNeedDoAgain) {
  const string parallel_pragma = "ACCEL parallel";
  bool Changed = false;
  vector<void *> vec_loops;
  codegen->GetNodesByType(pTopFunc, "preorder", "SgForStatement", &vec_loops);
  //  vec_loops={Sg1,Sg2,...}
  //  for Lab1, vec_loops={"SgForStatement:for(j = 0;j < 10000;j++) c[j] =
  //  a[j]
  //  + b[j] + inc;"} for Lab1_nested_loop, vec_loops={ "SgForStatement:for(j
  //  = 0;j < 10000;j++) {...}" "SgForStatement:for(k = 0;k < 110;k++) {...}"
  //  }

  for (auto loop : vec_loops) {
    //  20170407 pragma parser
    void *loop_body = codegen->GetLoopBody(loop);
    CMirNode *node = mars_ir->get_node(loop_body);
    if (node == nullptr) {
      continue;
    }
    for (auto pragma_info : node->get_pragma()) {
      if (pragma_info.is_parallel() || pragma_info.is_loop_tiling()) {
        string s_factor = pragma_info.get_attribute(CMOST_parallel_factor);
        int i_factor = my_atoi(s_factor);
        string var_info = codegen->_up(loop) + " " +
                          getUserCodeFileLocation(codegen, loop, true) + "\n";

        //  Yuxin: Nov 2 2017, collecting user inserting HLS loop trip count
        //  pragmas
        int64_t orig_factor = 1;
        void *tripcount_pragma = nullptr;
        collect_tripcount_pragma(codegen, mars_ir, loop, &tripcount_pragma,
                                 &orig_factor);

        if (i_factor <= 1) {
          //  20170415 To avoid wrong warning message in the cases of "#pragma
          //  ACCEL parallel"
          if (i_factor == 0) {
            continue;
          }
          //  20170511 support parallel off
          if (i_factor == 1) {
            continue;
          }
          //  20170414
          //  Yuxin: Nov/06/2019, only report for tiling
          if (pragma_info.is_loop_tiling()) {
            string msg =
                "Ignoring a loop tiling pragma for a tiling factor <=1: \n" +
                var_info;
            dump_critical_message(TILE_WARNING_6(var_info), 0);
#if PROJDEBUG
            cout << "WARNING: " << msg << endl;
#endif
          }
          continue;
        }

        //  Tianyi 20170316 add the check of CanonicalForLoop
        void *iv;
        void *lb_tmp;
        void *ub_tmp;
        void *step;
        void *body_tmp;
        iv = lb_tmp = ub_tmp = step = body_tmp = nullptr;
        bool inc_dir;
        bool inclusive_bound;
        if (codegen->IsCanonicalForLoop(loop, &iv, &lb_tmp, &ub_tmp, &step,
                                        &body_tmp, &inc_dir,
                                        &inclusive_bound) == 0) {
          //  Yuxin: Nov/06/2019, only report for tiling
          if (pragma_info.is_loop_tiling()) {
            string msg =
                "Ignoring a loop tiling pragma for non-canonical loop: \n" +
                var_info;
            dump_critical_message(TILE_WARNING_1(var_info), 0);
#if PROJDEBUG
            cout << "WARNING: " << msg << endl;
#endif
          }
          continue;
        }
        if (!inc_dir) {
          if (pragma_info.is_loop_tiling()) {
            string msg = "Ignoring a loop tiling pragma for loop with "
                         "decremental iteration space: \n" +
                         var_info;
            dump_critical_message(TILE_WARNING_2(var_info), 0);
#if PROJDEBUG
            cout << "WARNING: " << msg << endl;
#endif
          }
          continue;
        }

        //  Tianyi 20170316 add the check of loop increment expression (only
        //  support step==1 now)
        int64_t istep = 0;
        if (codegen->GetLoopStepValueFromExpr(step, &istep) == 0) {
          if (pragma_info.is_loop_tiling()) {
            string msg = "Ignoring a loop tiling pragma for loop with "
                         "non-constant step: \n" +
                         var_info;
            dump_critical_message(TILE_WARNING_3(var_info), 0);
#if PROJDEBUG
            cout << "WARNING: " << msg << endl;
#endif
          }
          continue;
        }
        if (istep != 1) {
          if (pragma_info.is_loop_tiling()) {
            string msg =
                "Cannot tile a loop with loop increment not equal to 1: \n" +
                var_info;
            dump_critical_message(TILE_WARNING_4(var_info), 0);
#if PROJDEBUG
            cout << "WARNING: " << msg << endl;
#endif
          }
          continue;
        }

        //  auto subs = str_split(pragma_string, "=");
        //  if (subs.size() < 2) continue;//  20170315
        //  int factor_num = my_atoi(subs[1]);
        //  bool is_para = pragma_string.find("ACCEL parallel factor") == 0;

        //  20170407
        bool is_para = true;
        if (pragma_info.is_loop_tiling()) {
          is_para = false;
        }

        //  calculate factor_num: for loop is needed for multiple tiling
        int factor_num = 1;
        for (auto pragma : node->pragma_table) {
          CAnalPragma ana_pragma(codegen);
          bool errorOut;
          ana_pragma.PragmasFrontendProcessing(pragma, &errorOut, false);
          if ((ana_pragma.is_parallel()) || (ana_pragma.is_loop_tiling())) {
            int curFactor =
                my_atoi(ana_pragma.get_attribute(CMOST_parallel_factor));
            factor_num = curFactor > 1 ? factor_num * curFactor : factor_num;
            //  curFactor = my_atoi(ana_pragma.get_attribute(
            //    CMOST_parallel_multi_level_factor));
            //  factor_num =
            //    curFactor > 1 ? factor_num * curFactor : factor_num;
          }
        }

        //  Tianyi 20170313
        CMarsRangeExpr mr =
            CMarsVariable(loop, codegen, nullptr, loop).get_range();
        CMarsExpression lb;
        CMarsExpression ub;
        int ret = mr.get_simple_bound(&lb, &ub);
        assert(ret);
        CMarsExpression len = ub - lb + 1;
        int64_t c_len = len.GetConstant();
        int is_divisible = 1;
        if (lb.IsConstant() == 0) {
          if (pragma_info.is_loop_tiling()) {
            string msg = "Ignoring a loop tiling pragma for loop with "
                         "non-constant lower bound: \n" +
                         var_info;
            dump_critical_message(TILE_WARNING_5(var_info), 0);
#if PROJDEBUG
            cout << "WARNING: " << msg << endl;
#endif
          }
          continue;
        }

        if ((lb != 0) != 0) {
          (*loop2is_zero_lb)[loop] = 0;
          (*loop2bound_offset)[loop] = lb;
        } else {
          (*loop2is_zero_lb)[loop] = 1;
        }

        if (len.IsConstant() != 0) {
          is_divisible = static_cast<int>((c_len % factor_num) == 0);

          (*loop2c_len)[loop] = c_len;
          (*loop2is_divisible)[loop] = is_divisible;
        } else {
          if (divisible(len, factor_num, true) != 0) {
            is_divisible = 1;
            (*loop2len)[loop] = len.get_expr_from_coeff();
            (*loop2is_divisible)[loop] = is_divisible;
          } else {
            is_divisible = 0;
            (*loop2len)[loop] = len.get_expr_from_coeff();
            (*loop2is_divisible)[loop] = is_divisible;
          }
        }
        if (is_divisible == 0) {
          if (len.IsConstant() != 0 && factor_num <= c_len) {
            dump_critical_message(
                TILE_WARNING_10(var_info, my_itoa(factor_num)));
          }
          if (len.IsConstant() == 0) {
            dump_critical_message(
                TILE_WARNING_11(var_info, my_itoa(factor_num)));
          }
        }

        //  Yuxin: Nov 15 2017, enhance the label support
        if ((codegen->IsBasicBlock(loop_body) != 0) &&
            ((*loop2is_divisible)[loop] == 0)) {
          int label_in_body = 0;
          vector<void *> vec_labels;
          codegen->GetNodesByType(loop_body, "preorder", "SgLabelStatement",
                                  &vec_labels);

          void *sg_pre_ = codegen->GetParent(loop);
          if ((sg_pre_ != nullptr) &&
              (codegen->IsLabelStatement(sg_pre_) != 0)) {
            vec_labels.push_back(sg_pre_);
          }
          label_in_body = static_cast<int>(!vec_labels.empty());
          if (label_in_body != 0) {
            vector<void *> vec_gotos;
            set<void *> label_goto;
            codegen->GetNodesByType_int(pTopFunc, "preorder", V_SgGotoStatement,
                                        &vec_gotos);
            for (size_t i = 0; i < vec_gotos.size(); i++) {
              void *label0 = codegen->GetGotoLabel(vec_gotos[i]);
              if ((label0 != nullptr) &&
                  label_goto.find(label0) == label_goto.end()) {
                label_goto.insert(label0);
              }
            }

            for (size_t i = 0; i < vec_labels.size(); i++) {
              if (label_goto.find(vec_labels[i]) != label_goto.end()) {
                string label_info = codegen->GetLabelName(vec_labels[i]);
                string msg = "";
                if (is_para) {
                  msg = "Cannot parallel an indivisible loop with labels : " +
                        var_info +
                        ". The label is targeted by a goto statement.";
                  dump_critical_message(TILE_ERROR_4(var_info, label_info), 0);
                } else {
                  msg = "Cannot tile an indivisible loop with labels : " +
                        var_info +
                        ". The label is targeted by a goto statement.";
                  dump_critical_message(TILE_ERROR_3(var_info, label_info), 0);
                }
#if PROJDEBUG
                cout << "ERROR: " << msg << endl;
#endif
                throw std::exception();
              }
            }
          }
        }

        //  20170408Remove Pragma
        //  add remaining pragma to loop2remain_pragmas for multiple tiling
        {
          bool isFirstTilingPragma = true;
          vector<string> newPragmas;
          for (auto pragma : node->pragma_table) {
            CAnalPragma ana_pragma(codegen);
            bool errorOut;
            ana_pragma.PragmasFrontendProcessing(pragma, &errorOut, false);
            if ((ana_pragma.is_parallel() &&
                 !ana_pragma.get_attribute(CMOST_parallel_factor).empty()) ||
                (ana_pragma.is_loop_tiling())) {
              if (isFirstTilingPragma) {
                isFirstTilingPragma = false;

                if (ana_pragma.is_loop_tiling()) {
                  if (!ana_pragma
                           .get_attribute(CMOST_parallel_multi_level_factor)
                           .empty()) {
                    ana_pragma.remove_attribute(CMOST_parallel_factor);
                    ana_pragma.add_attribute(
                        CMOST_parallel_factor,
                        ana_pragma.get_attribute(
                            CMOST_parallel_multi_level_factor));
                    ana_pragma.remove_attribute(
                        CMOST_parallel_multi_level_factor);
                    string newPragmaStr =
                        "ACCEL PARALLEL " + ana_pragma.print_attribute();
                    newPragmas.push_back(newPragmaStr);

                    if (pNeedDoAgain != nullptr) {
                      *pNeedDoAgain = true;
                    }
                  }
                }
              } else {
                newPragmas.push_back(codegen->GetPragmaString(pragma));
                if (pNeedDoAgain != nullptr) {
                  *pNeedDoAgain = true;
                }
              }
              if (codegen->is_floating_node(pragma) == 0) {
                codegen->RemoveStmt(pragma);
              }
            }
          }

          (*loop2remain_pragmas)[loop] = newPragmas;
        }
        if (len.IsConstant() != 0) {
          //  Tianyi 20170512 When factor>=trip count, tiling is disabled.
          if (factor_num >= c_len) {
            if (is_para) {
              if (!pragma_in_loop) {
                codegen->InsertStmt(
                    codegen->CreatePragma(parallel_pragma,
                                          codegen->TraceUpToBasicBlock(loop)),
                    loop);
              } else {
                void *loop_body = codegen->GetLoopBody(loop);
                codegen->PrependChild(
                    loop_body,
                    codegen->CreatePragma(parallel_pragma, loop_body));
              }
            } else {
              string msg = "Loop tiling is disabled "
                           "because Factor >= Loop Trip Count: \n" +
                           var_info;
              dump_critical_message(TILE_WARNING_8(var_info), 0);
#if PROJDEBUG
              cout << "WARNING: " << msg << endl;
#endif
            }
            if (loop2remain_pragmas->find(loop) != loop2remain_pragmas->end()) {
              void *loopBody = codegen->GetLoopBody(loop);
              if (pragma_in_loop) {
                for (size_t i = (*loop2remain_pragmas)[loop].size(); i > 0;
                     --i) {
                  void *newPragma = codegen->CreatePragma(
                      (*loop2remain_pragmas)[loop][i - 1], loopBody);
                  codegen->PrependChild(loopBody, newPragma);
                }
              } else {
                for (auto pragmaStr : (*loop2remain_pragmas)[loop]) {
                  void *newPragma = codegen->CreatePragma(
                      pragmaStr, codegen->TraceUpToBasicBlock(loop));
                  codegen->InsertStmt(newPragma, loop);
                }
              }
            }
            Changed = true;
            break;
          }
        }
        cout << "Find a loop (" << factor_num
             << "):" << codegen->UnparseToString(loop) << endl;
        if (is_para) {
          loop2stopfgpip->push_back(loop);
        }

        CAnalPragma new_pragma_info = pragma_info;
        pragma_info.set_pragma_type(CMOST_PARALLEL);
        pragma_info.remove_attribute(CMOST_parallel_factor);
        CAnalPragma *ptr_pragma_info = new CAnalPragma(pragma_info);
        (*loop2pragma)[loop] = ptr_pragma_info;

        void *upper_loop = nullptr;
        if (is_divisible != 0) {  //  check divsiblity
          //  Youxiang: 20170409 how to check perfect nested loop

          void *parent_loop_body = codegen->GetParent(loop);
          void *parent_loop = codegen->GetParent(parent_loop_body);
          if ((!pragma_in_loop &&
               codegen->GetChildStmtNum(parent_loop_body) == 1) ||
              (pragma_in_loop &&
               has_only_pragma_and_one_loop(codegen, parent_loop_body) &&
               has_no_pragma(codegen, loop_body))) {
            if ((codegen->IsBasicBlock(parent_loop_body) != 0) &&
                (codegen->IsForStatement(parent_loop) != 0) &&
                ((*loop2is_divisible)[parent_loop] !=
                 0)) {  //  check divisiblity
              upper_loop = parent_loop;
            }
          }
        }  //  if perfect nested loop (with pragma) then set upper_loop as
        //  its parent loop

        int hit = 0;
        if (upper_loop != nullptr) {
          for (auto &nest : *vec_loop_nest) {
            if (nest[nest.size() - 1].first == upper_loop) {
              hit = 1;
              nest.push_back(pair<void *, pair<int, bool>>(
                  loop, pair<int, bool>(factor_num, is_para)));
            }
          }
        }

        if (hit == 0) {
          vector<pair<void *, pair<int, bool>>> new_nest;
          new_nest.push_back(pair<void *, pair<int, bool>>(
              loop, pair<int, bool>(factor_num, is_para)));
          vec_loop_nest->push_back(new_nest);
        }

        cout << "Nest: " << codegen->_p(upper_loop) << " -> "
             << codegen->_p(loop) << endl;

        if (tripcount_pragma != nullptr) {
          (*new_tripcount)[loop] = orig_factor / factor_num;
          codegen->RemoveStmt(tripcount_pragma);
        }

        break;
      }
    }
  }
  return Changed;
}

void remove_pragma_before_clone(CSageCodeGen *codegen, void *inner_body,
                                bool pragma_in_loop) {
  //  Youxiang: 20170423 remove all the pragmas related to loop in the cloned
  //  body
  vector<void *> vec_pragma;
  if (pragma_in_loop) {
    codegen->GetNodesByType(inner_body, "preorder", "SgPragmaDeclaration",
                            &vec_pragma);
    for (auto pragma : vec_pragma) {
      void *loop_scope = codegen->TraceUpToLoopScope(pragma);
      if (loop_scope != nullptr) {
        continue;
      }
      CAnalPragma ana_pragma(codegen);
      bool errorOut = false;
      ana_pragma.PragmasFrontendProcessing(pragma, &errorOut, false);
      if (!ana_pragma.is_loop_related()) {
        continue;
      }

      //  Yuxin: propagate the false dependence and reduction pragma
      if (ana_pragma.is_false_dep() || ana_pragma.is_reduction() ||
          ana_pragma.is_line_buffer()) {
        continue;
      }

      codegen->RemoveStmt(pragma);
    }
  }
  //  Youxiang: 20190117 remove all the cache pragmas related to scope in the
  //  cloned body
  {
    codegen->GetNodesByType(inner_body, "preorder", "SgPragmaDeclaration",
                            &vec_pragma);
    for (auto pragma : vec_pragma) {
      void *scope = codegen->TraceUpToScope(pragma);
      if (scope != inner_body) {
        continue;
      }
      CAnalPragma ana_pragma(codegen);
      bool errorOut = false;
      ana_pragma.PragmasFrontendProcessing(pragma, &errorOut, false);
      if (!ana_pragma.is_cache()) {
        continue;
      }
      codegen->RemoveStmt(pragma);
    }
  }
}

void apply_parallel_in_new_sub_loop(
    CSageCodeGen *codegen,
    const vector<tuple<void *, bool, CAnalPragma>> &vec_sub_loops,
    bool pragma_in_loop) {
  for (auto loop_info : vec_sub_loops) {
    void *loop = std::get<0>(loop_info);
    void *loop_body = codegen->GetLoopBody(loop);
    void *parent_scope = codegen->TraceUpToBasicBlock(loop);
    if (std::get<1>(loop_info)) {  //  parallel
      void *scope = pragma_in_loop ? loop_body : parent_scope;
      void *new_pragma =
          std::get<2>(loop_info).update_pragma(nullptr, false, scope);
      if (!pragma_in_loop) {
        codegen->InsertStmt(new_pragma, loop);
        void *tag_pragma = codegen->CreatePragma(
            std::string(ACCEL_PRAGMA) + " " + CMOST_PARTIAL_PARALLEL_TAG,
            parent_scope);
        codegen->InsertStmt(tag_pragma, loop);
      } else {
        codegen->PrependChild(loop_body, new_pragma);
        void *tag_pragma = codegen->CreatePragma(
            std::string(ACCEL_PRAGMA) + " " + CMOST_PARTIAL_PARALLEL_TAG,
            loop_body);
        codegen->PrependChild(loop_body, tag_pragma);
      }
    } else {  //  tiled
      //  YX: 20180307 add a tag for the tiled loop
      if (!pragma_in_loop) {
        void *tag_pragma = codegen->CreatePragma(std::string(ACCEL_PRAGMA) +
                                                     " " + CMOST_LOOP_TILED_TAG,
                                                 parent_scope);
        codegen->InsertStmt(tag_pragma, loop);
      } else {
        void *loop_body = codegen->GetLoopBody(loop);
        if (has_no_loop_tiled_tag(codegen, loop_body)) {
          void *tag_pragma = codegen->CreatePragma(
              std::string(ACCEL_PRAGMA) + " " + CMOST_LOOP_TILED_TAG,
              loop_body);
          codegen->PrependChild(loop_body, tag_pragma);
        }
      }
      map<string, string> message;
      message["tiling_loop"] = "yes";
      insertMessage(codegen, std::get<0>(loop_info), message);
    }
    setQoRMergeMode(codegen, std::get<0>(loop_info), "minor", true);
  }
}

void update_loop_tripcount_pragma(
    CSageCodeGen *codegen, const vector<pair<void *, pair<int, bool>>> &nest,
    const map<void *, int> &loop2is_divisible,
    const map<void *, void *> &loop2remain_loop,
    const map<void *, int> &loop2is_zero_lb,
    const map<void *, int64_t> &loop2c_len, const map<void *, void *> &loop2len,
    const map<void *, CMarsExpression> &loop2bound_offset,
    bool pragma_in_loop) {
  for (auto &loop_pair : nest) {
    void *loop = loop_pair.first;
    int factor = loop_pair.second.first;
    CMarsRangeExpr mr = CMarsVariable(loop, codegen, nullptr, loop).get_range();
    CMarsExpression lb;
    CMarsExpression ub;
    int ret = mr.get_simple_bound(&lb, &ub);
    assert(ret);
    CMarsExpression len = ub - lb + 1;  //  symbolic

    //  Tianyi 20170314 Update ub of main loop
    //  ZP: 20170119
    void *main_ub_tmp = nullptr;  //  main_loop_ub_tmp
    //  void * remain_loop_ub_tmp = nullptr;//  remain_loop_ub_tmp
    //  int is_divisible = 1;
    if (len.IsConstant() != 0) {
      assert(len.IsConstant());
      int64_t c_len = len.GetConstant();
      c_len = c_len / factor;

      //  is_divisible = (c_len % factor) == 0;
      if (c_len < std::numeric_limits<int>::max()) {
        main_ub_tmp = codegen->CreateConst(static_cast<int>(c_len));
      } else {
        main_ub_tmp = codegen->CreateConst(c_len);
      }
      //  remain_loop_ub_tmp = codegen->CreateConst(c_len % factor);
    } else {
      //  if ( divisible(len, factor))
      //  main_ub_tmp = (len / factor).get_expr_from_coeff();
      //  else
      //  main_ub_tmp = (len / factor).get_expr_from_coeff();  //  4*n vs 4
      main_ub_tmp = (len / factor).get_expr_from_coeff();
    }

    assert(main_ub_tmp);

    void *loop_iter = codegen->GetLoopIterator(loop);
    void *new_test_exp = codegen->CreateExp(
        V_SgLessThanOp, codegen->CreateVariableRef(loop_iter),
        main_ub_tmp);                              //  e.g. i<100 -> i<25
    void *loop_test = codegen->GetLoopTest(loop);  //  Stmt i<100;
    void *old_test_exp = codegen->GetExprFromStmt(loop_test);  //  Expr i<100
    codegen->ReplaceExp(old_test_exp, new_test_exp);  //  e.g. i<100 -> i<25

    //  Tianyi 20170314 Update lb of main loop
    void *lb_new = nullptr;
    void *iv;
    void *lb_tmp;
    void *ub_tmp;
    void *step;
    void *body_tmp;
    iv = lb_tmp = ub_tmp = step = body_tmp = nullptr;
    bool inc_dir;
    bool inclusive_bound;
    if (codegen->IsCanonicalForLoop(loop, &iv, &lb_tmp, &ub_tmp, &step,
                                    &body_tmp, &inc_dir,
                                    &inclusive_bound) != 0) {
      lb_new = codegen->CreateConst(0);  //  change the value of lb
      codegen->ReplaceExp(lb_tmp, lb_new);
      //  cout<<"main_loop lb has been updated"<<endl;
    }

    //  Tianyi 20170309
    if (loop2is_divisible.count(loop) <= 0 ||
        loop2is_divisible.find(loop)->second == 0) {
      void *remain_loop = loop2remain_loop.find(loop)->second;
      void *remain_loop_body = codegen->GetLoopBody(remain_loop);

      //  Yuxin 20171012: add tripcount pragma in "remain_loop" with variable
      //  loop tripcount trip count max = factor-1
      std::ostringstream oss;
      oss << HLS_PRAGMA << " " << HLS_TRIPCOUNT << " max=" << (factor - 1);
      //  Yuxin 20171115: remove the labels in "remain_loop"
      vector<void *> vec_labels;
      codegen->GetNodesByType(remain_loop_body, "preorder", "SgLabelStatement",
                              &vec_labels);
      for (size_t i = 0; i < vec_labels.size(); i++) {
        codegen->RemoveLabel(vec_labels[i]);
      }

      void *lb_new = nullptr;
      void *iv;
      void *lb_tmp;
      void *ub_tmp;
      void *step;
      void *body_tmp;
      iv = lb_tmp = ub_tmp = step = body_tmp = nullptr;
      bool inc_dir;
      bool inclusive_bound;
      //  update remain loop bound  8-10  -> 9-11    ub_tmp
      if (codegen->IsCanonicalForLoop(remain_loop, &iv, &lb_tmp, &ub_tmp, &step,
                                      &body_tmp, &inc_dir,
                                      &inclusive_bound) != 0) {
        if (loop2is_zero_lb.count(loop) > 0 &&
            loop2is_zero_lb.find(loop)->second != 0) {
          if (len.IsConstant() != 0) {
            int64_t c_len =
                loop2c_len.count(loop) > 0 ? loop2c_len.find(loop)->second : 0;
            int64_t remain_base = (c_len / factor) * factor;
            lb_new = codegen->CreateConst(remain_base);
          } else {  //  change the value of lb
            void *len = loop2len.count(loop) > 0 ? loop2len.find(loop)->second
                                                 : nullptr;
            lb_new = codegen->CreateExp(
                V_SgMultiplyOp,
                codegen->CreateExp(V_SgDivideOp, len,
                                   codegen->CreateConst(factor)),
                codegen->CreateConst(factor));
            codegen->PrependChild(
                remain_loop_body,
                codegen->CreatePragma(oss.str(), remain_loop_body));
          }  //  change the value of lb
        } else {
          if (len.IsConstant() != 0) {
            int64_t c_len =
                loop2c_len.count(loop) > 0 ? loop2c_len.find(loop)->second : 0;
            int64_t remain_base =
                (c_len / factor) * factor +
                (loop2bound_offset.count(loop) <= 0
                     ? 0
                     : loop2bound_offset.find(loop)->second.GetConstant());
            lb_new =
                codegen->CreateConst(remain_base);  //  change the value of lb
          } else {
            void *len = loop2len.count(loop) > 0 ? loop2len.find(loop)->second
                                                 : nullptr;
            int64_t offset =
                loop2bound_offset.count(loop) <= 0
                    ? 0
                    : loop2bound_offset.find(loop)->second.GetConstant();
            lb_new = codegen->CreateExp(
                V_SgAddOp,
                codegen->CreateExp(
                    V_SgMultiplyOp,
                    codegen->CreateExp(V_SgDivideOp, len,
                                       codegen->CreateConst(factor)),
                    codegen->CreateConst(factor)),
                codegen->CreateConst(offset));
            codegen->PrependChild(
                remain_loop_body,
                codegen->CreatePragma(oss.str(), remain_loop_body));
          }
        }
        codegen->ReplaceExp(lb_tmp, lb_new);
        //  cout<<"remain_loop lb&ub has been updated"<<endl;
      }
      //  Yuxin: 07/24/2019
      //  Remove copied line_buffer and reduction pragma
      if (pragma_in_loop) {
        vector<void *> vec_pragma;
        codegen->GetNodesByType(remain_loop_body, "preorder",
                                "SgPragmaDeclaration", &vec_pragma);
        for (size_t i = 0; i < vec_pragma.size(); i++) {
          void *pragma = vec_pragma[i];
          void *loop_scope = codegen->TraceUpToLoopScope(pragma);
          if (loop_scope != remain_loop) {
            continue;
          }
          CAnalPragma ana_pragma(codegen);
          bool errorOut = false;
          ana_pragma.PragmasFrontendProcessing(pragma, &errorOut, false);
          if (ana_pragma.is_reduction() || ana_pragma.is_line_buffer()) {
            codegen->RemoveStmt(pragma);
          }
        }
      }
    }
  }
}

int loop_tiling_top(CSageCodeGen *codegen, void *pTopFunc,
                    const CInputOptions &options) {
  string pragma_in_loop = options.get_option_key_value("-a", "pragma_in_loop");
  return loop_tiling_top(codegen, pTopFunc, options, pragma_in_loop == "yes");
}

//  assumptions: ( if any folloing condition is not met, the transformation will
//  be disabled)
//  1. lower bound is constant(not symbolic), and lower bound < upper bound, and
//  loop increment is one
//  2. Only the perfect nested canonical loop will be grouped
int loop_tiling_top(CSageCodeGen *codegen, void *pTopFunc,
                    const CInputOptions &options, bool pragma_in_loop,
                    bool *pNeedDoAgain /*= nullptr*/) {
  if (pNeedDoAgain != nullptr) {
    *pNeedDoAgain = false;
  }
  bool Changed = false;

  //  20170407
  loop_tiling_preprocessing(codegen, pTopFunc);
  string tool_type = options.get_option_key_value("-a", "impl_tool");

  printf("Hello Loop Tiling ... \n");

  //  1. find all the loops with partial parallelization pragma;
  //    nested partial parallelized loop(with a divisible factor) will be
  //    grouped; unsupported loop will be ignored.
  vector<vector<pair<void *, pair<int, bool>>>> vec_loop_nest;
  //  [nest][level](sg, (factor, is_parallel))

  //  Tianyi 20170309
  map<void *, vector<string>> loop2remain_pragmas;
  map<void *, int64_t> loop2c_len;
  map<void *, void *> loop2len;
  map<void *, int> loop2is_divisible;
  map<void *, int> loop2is_zero_lb;
  map<void *, CMarsExpression> loop2bound_offset;
  codegen->init_defuse_range_analysis();
  CMarsIr mars_ir;
  mars_ir.get_mars_ir(codegen, pTopFunc, options, true /* build node */,
                      false /* report */, pragma_in_loop /*pragma in loop */);
  map<void *, CAnalPragma *> loop2pragma;
  map<void *, int> new_tripcount;
  vector<void *> loop2stopfgpip;
  Changed |= collect_all_loop_info(
      codegen, pTopFunc, &mars_ir, &loop2remain_pragmas, &loop2c_len, &loop2len,
      &loop2is_divisible, &loop2is_zero_lb, &loop2bound_offset, &loop2pragma,
      &new_tripcount, &loop2stopfgpip, &vec_loop_nest, pragma_in_loop,
      pNeedDoAgain);

  //  2.1 get the tiling factor by parsing the pragma
  //  2.2 get the loop body
  //  2.3 create the declaration of the original iterators, in the first
  //  statement of body 2.4 create the new loop nest 2.5 replace the orignal
  //  iterator references 2.6 update original loop bound and remove its pragma
  //  2.7 apply parallel pragma in the new sub loops

  map<void *, void *> loop2remain_loop;
  //  for (auto nest : vec_loop_nest)
  for (size_t i = vec_loop_nest.size(); i > 0; --i) {
    //  2.1 get the tiling factor by parsing the pragma
    //  2.2 get the loop body
    auto nest = vec_loop_nest[i - 1];
    void *outer_loop = nest[0].first;
    void *inner_loop = nest[nest.size() - 1].first;
    void *inner_body = codegen->CopyStmt(codegen->GetLoopBody(inner_loop));
    remove_pragma_before_clone(codegen, inner_body, pragma_in_loop);
    //  "SgBasicBlock:{c[55 * j + k] = a[55 * j + k] + b[55 * j + k] + inc;}"
    //  void * scope = codegen->GetScope(outer_loop);
    void *func_decl = codegen->TraceUpToFuncDecl(outer_loop);
    //  "SgFunctionDeclaration:void vec_add_kernel(int * a,int * b,int * c,int
    //  inc) {...}"
    void *func_body = codegen->GetFuncBody(func_decl);
    map<void *, void *> loop_iter2loop;
    map<void *, void *> loop2subloop_iter;
    map<void *, int> loop2factor;

    vector<tuple<void *, bool, CAnalPragma>> vec_sub_loops;

    //  2.3 create the declaration of the original iterators, in the first
    //  statement of body
    for (auto &loop_pair : nest) {
      void *loop = loop_pair.first;
      void *loop_iter = codegen->GetLoopIterator(loop);
      assert(loop_iter);
      loop_iter2loop[loop_iter] = loop;
      loop2factor[loop] = loop_pair.second.first;
      string sub_iter_name = codegen->GetVariableName(loop_iter) + "_sub";
      codegen->get_valid_local_name(func_decl, &sub_iter_name);
      void *sub_iter = codegen->CreateVariableDecl(
          codegen->GetTypebyVar(loop_iter), sub_iter_name,
          codegen->CreateConst(0), func_body);
      loop2subloop_iter[loop] = sub_iter;

      //  Tianyi 20170307   insert int j_remain;
      /*if (!loop2is_divisible[loop]){
      //  void * remain_loop = codegen->CopyStmt(loop_pair.first);
      //  codegen->AppendChild(func_body, remain_loop);
      string remain_iter_name = codegen->GetVariableName(loop_iter) + "_remain";
      codegen->get_valid_local_name(func_decl, &remain_iter_name);
      void *remain_iter = codegen->CreateVariableDecl(
      codegen->GetTypebyVar(loop_iter), remain_iter_name, nullptr, func_body);
      codegen->PrependChild(func_body, remain_iter);
      loop2remainloop_iter[loop] = remain_iter;}*/
    }

    //  2.4 create the new loop nest
    void *body = inner_body;
    for (size_t i = nest.size(); i > 0; --i) {
      void *loop = nest[i - 1].first;
      int factor = nest[i - 1].second.first;
      void *sub_iter = loop2subloop_iter[loop];

      //  Tianyi 20170314
      void *sub_loop = codegen->CreateStmt(V_SgForStatement, sub_iter, nullptr,
                                           codegen->CreateConst(factor), body,
                                           nullptr, nullptr, loop);
      setQoRMergeMode(codegen, loop, "major", true);
      bool is_para = nest[i - 1].second.second;
      body = codegen->CreateBasicBlock();
      codegen->AppendChild(body, sub_loop);
      CAnalPragma new_pragma_info(codegen);
      if (loop2pragma.count(loop) > 0) {
        new_pragma_info = *loop2pragma[loop];
      }
      vec_sub_loops.push_back(
          tuple<void *, bool, CAnalPragma>(sub_loop, is_para, new_pragma_info));

      //  Tianyi 20170309
      if (loop2is_divisible[loop] == 0) {
        //  Tianyi 20170701 Insert braces for remainder loops
        void *remain_loop_bb = codegen->CreateBasicBlock();
        void *remain_loop = codegen->CopyStmt(nest[i - 1].first);
        codegen->PrependChild(remain_loop_bb, remain_loop);
        codegen->InsertAfterStmt(
            remain_loop_bb,
            loop);  //  insert a remainder loop after current main loop
                    //       loop2stopfgpip.push_back(remain_loop);
        map<string, string> message;
        message["loop_tile_tail"] = "true";
        insertMessage(codegen, remain_loop, message);
        loop2remain_loop[loop] = remain_loop;
        setQoRMergeMode(codegen, remain_loop, "tail", true);
      }

      //  added the pragmas in loop2remain_pragmas to the corresponding loop
      if (loop2remain_pragmas.find(loop) != loop2remain_pragmas.end()) {
        void *loopBody = codegen->GetLoopBody(sub_loop);
        if (pragma_in_loop) {
          for (size_t j = loop2remain_pragmas[loop].size(); j > 0; --j) {
            void *newPragma = codegen->CreatePragma(
                loop2remain_pragmas[loop][j - 1], loopBody);
            codegen->PrependChild(loopBody, newPragma);
          }
        } else {
          for (auto pragmaStr : loop2remain_pragmas[loop]) {
            void *newPragma = codegen->CreatePragma(
                pragmaStr, codegen->TraceUpToBasicBlock(sub_loop));
            codegen->InsertStmt(newPragma, sub_loop);
          }
        }
      }
    }
    void *old_inner_loop_body = codegen->GetLoopBody(inner_loop);
    //  copy all the pragmas related to loop of old inner loop
    if (pragma_in_loop) {
      vector<void *> vec_pragma;
      codegen->GetNodesByType(old_inner_loop_body, "preorder",
                              "SgPragmaDeclaration", &vec_pragma);
      for (auto pragma : vec_pragma) {
        void *loop_scope = codegen->TraceUpToLoopScope(pragma);
        if (loop_scope != inner_loop) {
          continue;
        }
        CAnalPragma ana_pragma(codegen);
        bool errorOut = false;
        ana_pragma.PragmasFrontendProcessing(pragma, &errorOut, false);
        if (!ana_pragma.is_loop_related()) {
          continue;
        }
        //  Yuxin: 05/08/2019
        //  Unecessary to spread out the line_buffer pragma
        if (ana_pragma.is_line_buffer()) {
          continue;
        }

        codegen->PrependChild(body, codegen->CopyStmt(pragma));
      }
    }
    //  copy all the pragms related to scope of old inner loop body
    //  currently, there is only cache pragma which needs to be handled
    {
      vector<void *> vec_pragma;
      codegen->GetNodesByType(old_inner_loop_body, "preorder",
                              "SgPragmaDeclaration", &vec_pragma);
      for (auto pragma : vec_pragma) {
        void *scope = codegen->TraceUpToScope(pragma);
        if (scope != old_inner_loop_body) {
          continue;
        }
        CAnalPragma ana_pragma(codegen);
        bool errorOut = false;
        ana_pragma.PragmasFrontendProcessing(pragma, &errorOut, false);
        if (!ana_pragma.is_cache()) {
          continue;
        }
        codegen->PrependChild(body, codegen->CopyStmt(pragma));
      }
    }
    //  replace the original loop body
    codegen->ReplaceStmt(old_inner_loop_body, body);
    if (new_tripcount.find(inner_loop) != new_tripcount.end()) {
      std::ostringstream oss;
      oss << HLS_PRAGMA << " " << HLS_TRIPCOUNT
          << " max=" << new_tripcount[inner_loop];
      void *new_pragma = codegen->CreatePragma(oss.str(), body);
      codegen->PrependChild(body, new_pragma);
    }

    //  2.5 replace the orignal iterator references
    vector<void *> vec_refs;
    codegen->GetNodesByType(inner_body, "preorder", "SgVarRefExp",
                            &vec_refs);  //  i,j->i0,j0
    //  vec_refs={c,j,k,a,j,k,b,j,k,inc}
    for (auto &ref : vec_refs) {
      void *var_init = codegen->GetVariableInitializedName(ref);
      if (loop_iter2loop.count(var_init) <= 0) {
        continue;
      }
      void *loop = loop_iter2loop[var_init];
      void *sub_iter = loop2subloop_iter[loop];
      int factor = loop2factor[loop];

      //  Tianyi 20170315

      void *new_ref = codegen->CreateExp(
          V_SgAddOp,
          codegen->CreateExp(V_SgMultiplyOp,
                             codegen->CreateVariableRef(var_init),
                             codegen->CreateConst(factor)),
          codegen->CreateVariableRef(sub_iter));
      if (loop2is_zero_lb[loop] == 0) {
        int offset_tmp = loop2bound_offset[loop].GetConstant();
        new_ref = codegen->CreateExp(V_SgAddOp, new_ref,
                                     codegen->CreateConst(offset_tmp));
      }
      codegen->ReplaceExp(ref, new_ref);
    }

    //  2.6 update original loop bound and remove its pragma
    update_loop_tripcount_pragma(codegen, nest, loop2is_divisible,
                                 loop2remain_loop, loop2is_zero_lb, loop2c_len,
                                 loop2len, loop2bound_offset, pragma_in_loop);
    //  2.7 apply parallel pragma in the new sub loops
    apply_parallel_in_new_sub_loop(codegen, vec_sub_loops, pragma_in_loop);

    Changed = true;
  }

  //  release memory
  for (auto loop_pragma : loop2pragma) {
    delete loop_pragma.second;
  }
  loop2pragma.clear();
  loop2stopfgpip.clear();
  //  cout<<"Finally, we finished"<<endl;
  return static_cast<int>(Changed);
}

int loop_tiling_top(CSageCodeGen *codegen, void *pTopFunc,
                    const CInputOptions &options, bool pragma_in_loop) {
  bool changed = false;
  bool needDoAgain = true;  //  it will be call repeatly for multiple tiling
  while (needDoAgain) {
    changed = (loop_tiling_top(codegen, pTopFunc, options, pragma_in_loop,
                               &needDoAgain) != 0) ||
              changed;
  }
  return static_cast<int>(changed);
}
