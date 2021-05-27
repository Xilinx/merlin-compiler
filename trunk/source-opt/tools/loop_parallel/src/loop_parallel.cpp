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


#include <iostream>
#include <string>

#include "id_update.h"
#include "loop_parallel.h"
#include "mars_message.h"
#include "history_message.h"
#include "memory_partition.h"
#include "program_analysis.h"
#include "rose.h"

#include "ir_types.h"
#include "mars_ir.h"
#include "mars_ir_v2.h"

using MarsProgramAnalysis::ArrayRangeAnalysis;
using MarsProgramAnalysis::CMarsExpression;
using MarsProgramAnalysis::CMarsRangeExpr;
using MarsProgramAnalysis::CMarsVariable;

#define _PRINT_DEBUG_ 0
// #define SELFDEBUG

// Loop flatten
int loop_flatten_for_pipeline(CSageCodeGen *codegen, void *pTopFunc,
                              const CInputOptions &options, CMarsIr *mars_ir);

int loop_parallel_top(CSageCodeGen *codegen, void *pTopFunc,
                      const CInputOptions &options) {

  LoopParallel instance(codegen, pTopFunc, options);
  instance.run();

  return 0;
}

void LoopParallel::init() {
  if ("sdaccel" == mOptions.get_option_key_value("-a", "impl_tool")) {
    mXilinx_flow = true;
    cout << "[MARS-PARALLEL-MSG] Xilinx flow mode.\n";
  }
  if ("naive_hls" == mOptions.get_option_key_value("-a", "naive_hls")) {
    mNaive = true;
    cout << "[MARS-PARALLEL-MSG] ====> Enable Naive HLS mode.\n";
  }
  if ("off" == mOptions.get_option_key_value("-a", "dependency_resolve")) {
    mDepResolve = false;
    cout << "[MARS-PARALLEL-MSG] ====> Disable dependency resolve mode.\n";
  }
  mLoopFlatten = false;
  if ("on" == mOptions.get_option_key_value("-a", "loop_flatten")) {
    mLoopFlatten = true;
    cout << "[MARS-PARALLEL-MSG] ====> enable loop flatten mode.\n";
  }
  if ("off" == mOptions.get_option_key_value("-a", "auto_memory_partition")) {
    mAutoPartition = false;
    cout << "[MARS-PARALLEL-MSG] ====> Disable auto memory partition mode.\n";
  }
  string str_II =
      mOptions.get_option_key_value("-a", "default_initiation_interval");
  int ii = my_atoi(str_II);
  if (ii != 0)
    mDefaultII = ii;
}

bool LoopParallel::run() {
  cout << "==============================================" << endl;
  cout << "-----=# Fine-grained Parallel Optimization Start#=----\n";
  cout << "==============================================" << endl;

  m_ast->BuildPragmaCache();
  mMars_ir.clear();
  mMars_ir.get_mars_ir(m_ast, mTopFunc, mOptions, true, false, true, true);
  mMars_ir_v2.clear();
  mMars_ir_v2.build_mars_ir(m_ast, mTopFunc, false, true);

  if (mXilinx_flow) {
    return loop_parallel_xilinx_top() != 0;
  }

  cout << "============================================" << std::endl;
  cout << "-----=# Fine-grained Parallel Optimization End#=----\n";
  cout << "============================================" << std::endl << endl;

  return true;
}

int LoopParallel::loop_parallel_xilinx_top() {
  cout << "==============================================" << endl;
  cout << "-----=# Loop Parallel Xilinx Optimization Start#=----\n";
  cout << "==============================================" << endl;

  if (mDepResolve) {
    dependency_resolve_top(m_ast, mTopFunc, mOptions);
  }

  bool Changed = preprocess();

  if (Changed) {
    mMars_ir.clear();
    mMars_ir.get_mars_ir(m_ast, mTopFunc, mOptions, true, false, true, true);
  }
  // Yuxin: Sep/10/2020, should be covered by out-of-bound
  // check in memory partition, since we turn on partition analysis
  // for non-pipeline scope
  //  check_out_of_bound_access();

  add_loop_tripcount_pragma(m_ast, mTopFunc, &mMars_ir);

  MemoryPartition mp_instance(m_ast, mTopFunc, mOptions, &mMars_ir,
                              &mMars_ir_v2);

  if (!mNaive) {
    mp_instance.run();
  }

  auto node_actions = mp_instance.get_node_actions();
  auto array_partitions = mp_instance.get_partitions();

  parse_pragma_xilinx(node_actions, array_partitions);

  if (!mNaive) {
    // Check and process false dependence pragma
    check_false_dependency_xilinx(m_ast, mTopFunc, mOptions);
    if ("off" != mOptions.get_option_key_value("-a", "index_transform"))
      mp_instance.array_index_transform();
  }

  remove_loop_pragmas();

  if (mLoopFlatten) {
    loop_flatten_for_pipeline(m_ast, mTopFunc, mOptions, &mMars_ir);
  } else {
    //  dump_critical_message(FGPIP_INFO_4);
  }

  return 1;
}

void LoopParallel::parse_pragma_xilinx(
    map<CMirNode *, bool> node_actions,
    map<void *, map<int, int>> array_partitions) {
  for (auto it : mMars_ir.ir_list) {
    CMirNodeSet new_MirNodeSet = it;
    int node_num = it.size();

#ifdef SELFDEBUG
    cout << "[TESTING] Total loop MirNode number in this function is: "
         << node_num << endl;
#endif

    for (int i = 0; i < node_num; i++) {
      CMirNode *new_node = new_MirNodeSet[i];
      void *func_decl = m_ast->GetEnclosingNode("Function", new_node->ref);
      if (m_ast->UnSupportedMemberFunction(func_decl)) {
        cout << "==> [warning] Unsupported function declaration\n";
        break;
      }

#ifdef SELFDEBUG
      cout << "[TESTING] BSU node is:" << new_node->is_fine_grain
           << " unroll:" << new_node->is_complete_unroll()
           << " has_loop: " << new_node->has_loop << " \n"
           << new_node->unparseToString() << endl;
      new_node->print_child();
#endif
      if (!new_node->is_fine_grain) {
        if (new_node->has_parallel() != 0) {
          reportCoarseGrainedParallel(m_ast, new_node);
        }
#ifdef SELFDEBUG
        cout << "Not fine-grained loop.\n";
#endif
        continue;
      }
      if ((new_node->has_parallel() != 0) && (new_node->has_pipeline() != 0)) {
        if (new_node->is_complete_unroll()) {
          continue;
        }
        cout << "\n==> INSERT PIPELINE and PARALLEL ANALYSIS\n";
        string II_factor = new_node->get_attribute(CMOST_ii);
        insert_unroll(new_node);
        insert_pipeline(new_node, array_partitions);

      } else if ((new_node->has_parallel() != 0) &&
                 (new_node->has_pipeline() == 0)) {
        cout << "\n==> INSERT PARALLEL ANALYSIS\n";
        insert_unroll(new_node);
      } else if ((new_node->has_pipeline() != 0) &&
                 (new_node->has_parallel() == 0)) {
        string II_factor = new_node->get_attribute(CMOST_ii);

        cout << "\n==> INSERT PIPELINE ANALYSIS\n";
        insert_pipeline(new_node, array_partitions);
      }
    }
    cout << "============================================" << endl;
  }

  cout << "============================================\n\n";
  cout << "-----=# Loop Parallel Optimization End#=----\n";
  cout << "============================================\n\n";
}

/*Fine grain loop pipeline*/
void LoopParallel::insert_pipeline(
    CMirNode *bNode, map<void *, map<int, int>> array_partitions) {
  string II_factor = bNode->get_attribute(CMOST_ii);
  string pragma_str = std::string(HLS_PRAGMA) + " " + HLS_PIPELINE_low;

  //  Yuxin: Nov/8/2017, if user doesnt spefiy II, we dont.
  if (!II_factor.empty()) {
    pragma_str += " II=" + II_factor;
  } else {
    if (mDefaultII != 0)
      pragma_str += " II=" + my_itoa(mDefaultII);
  }

  void *new_pragma = m_ast->CreatePragma(pragma_str, bNode->ref);
  void *sg_loop = m_ast->TraceUpToLoopScope(bNode->ref);
  if (sg_loop == nullptr) {
    return;
  }

  if (bNode->is_complete_unroll()) {
    std::cout << "[MARS-PARALLEL-MSG] Loop pipeline pragma is unnecessary "
                 "under this circumstance. Inner loop will be flattend.\n";
  } else {
    m_ast->PrependChild_v1(new_pragma, bNode->ref);
    string msg_dep;
    check_dependency_msg(bNode, &msg_dep, array_partitions);
    reportPipeline(m_ast, sg_loop, II_factor, msg_dep);
  }
}

void LoopParallel::insert_unroll(CMirNode *bNode) {
  void *sg_loop = m_ast->TraceUpToLoopScope(bNode->ref);
  int64_t factor_range = 0;
  m_ast->get_loop_trip_count_simple(sg_loop, &factor_range);

  //  Invalid pragmas are detected in preprocess and memory partition
  string pragma_str;
  if (bNode->is_complete_unroll()) {
    pragma_str = std::string(HLS_PRAGMA) + " " + HLS_UNROLL_low;
    reportCompleteUnroll(m_ast, sg_loop, factor_range);
  } else {
    int unroll = 0;
    string unroll_factor = bNode->get_attribute(CMOST_parallel_factor);
    bNode->is_partial_unroll(&unroll);
    if (unroll <= 0) {
      cout << "[MARS-PARALLEL-MSG] Incorrect parallel factor : "
           << unroll_factor << endl;
      reportInvalidFactor(m_ast, bNode, unroll_factor);
      return;
    }
    if (factor_range > 1 && unroll > factor_range) {
      cout << "[MARS-PARALLEL-MSG] Incorrect parallel factor : "
           << unroll_factor << endl;
      reportLargeFactor(m_ast, bNode, unroll, factor_range);
      pragma_str = std::string(HLS_PRAGMA) + " " + HLS_UNROLL_low;
    } else {
      pragma_str = std::string(HLS_PRAGMA) + " " + HLS_UNROLL_low + " " +
                   CMOST_factor_low + "=" + unroll_factor;
      reportUnrollFactor(m_ast, sg_loop, unroll);
    }
  }

  void *new_pragma = m_ast->CreatePragma(pragma_str, bNode->ref);
  m_ast->PrependChild_v1(new_pragma, bNode->ref);
}

void LoopParallel::removePragmaStatement(CMirNode *new_node) {
  vector<void *> vec_pragmas;
  m_ast->GetNodesByType(new_node->ref, "preorder", "SgPragmaDeclaration",
                        &vec_pragmas);
  for (auto pragma : vec_pragmas) {
    SgPragmaDeclaration *decl =
        isSgPragmaDeclaration(static_cast<SgNode *>(pragma));
    void *loop_scope = m_ast->TraceUpToLoopScope(decl);
    CAnalPragma curr_pragma(m_ast);
    bool errorOut;
    curr_pragma.PragmasFrontendProcessing(decl, &errorOut);
    if (curr_pragma.is_loop_related() && !curr_pragma.is_loop_flatten() &&
        !curr_pragma.is_hls_pragma()) {
      void *loop = m_ast->TraceUpToLoopScope(new_node->ref);
      if (loop_scope == loop) {
        string pragmaString = decl->get_pragma()->get_pragma();
        m_ast->AddComment_v1(new_node->ref,
                             "Original: #pragma " + pragmaString);
        m_ast->RemoveStmt(decl);
      }
    }
  }
}

void LoopParallel::remove_loop_pragmas() {
  for (auto it : mMars_ir.ir_list) {
    CMirNodeSet new_MirNodeSet = it;
    int node_num = it.size();

    for (int i = 0; i < node_num; i++) {
      CMirNode *new_node = new_MirNodeSet[i];
      removePragmaStatement(new_node);
    }
  }
}

bool LoopParallel::preprocess() {
  vector<CMirNode *> vec_nodes;
  set<CMirNode *> visited;
  mMars_ir.get_topological_order_nodes(&vec_nodes);
  bool Changed = false;
  for (size_t j = 0; j < vec_nodes.size(); j++) {
    CMirNode *node = vec_nodes[j];
    if (node->is_function) {
      continue;
    }

    if (node->is_fine_grain &&
        ((node->has_pipeline() != 0) || (node->has_parallel() != 0))) {
      vector<CMirNode *> childs = node->get_all_child(true);
      for (size_t i = 0; i < childs.size(); i++) {
        CMirNode *cnode = childs[i];
        if (visited.count(cnode) > 0) {
          continue;
        }
        void *loop = m_ast->TraceUpToLoopScope(cnode->ref);
        if ((cnode->has_parallel() != 0) && cnode->is_complete_unroll()) {
          // Yuxin: Feb/21/2020, cannot get simple bound doesnt mean
          // that the loop trip count is variable and cannot be parallelized
          // Only need to report: when the loop tripcount is variable and
          // the loop is completely unrolled
          CMarsRangeExpr mr = CMarsVariable(loop, m_ast).get_range();
          CMarsExpression me_lb, me_ub;
          if (mr.get_simple_bound(&me_lb, &me_ub) != 0) {
            CMarsExpression len = me_ub - me_lb + 1;
            if (len.IsConstant() == 0) {
              string str_label = m_ast->get_internal_loop_label(loop);
              void *iter = m_ast->GetLoopIterator(loop);
              if (iter) {
                str_label = m_ast->UnparseToString(iter);
              }
              string sFileName = getUserCodeFileLocation(m_ast, loop);
              string loop_info = "'" + str_label;
              string file_info = "  (" + sFileName + ")";
              dump_critical_message(FGPIP_WARNING_12(loop_info, file_info));
              visited.insert(cnode);
            }
          }
        }
      }
    }
  }
  return Changed;
}

<<<<<<< HEAD
int LoopParallel::parse_pragma_intel() {
  // Processing labels
  vector<void *> vec_for_stmt;
  m_ast->GetNodesByType_int(mTopFunc, "postorder", V_SgForStatement,
                            &vec_for_stmt);
  int ret = 0;
  for (auto for_loop : vec_for_stmt) {
    CMarsLoop *loop_info = mMars_ir_v2.get_loop_info(for_loop);
    if (loop_info->has_parallel() != 0) {
      ret |= m_ast->add_label_scope(for_loop);
    }
  }

  // Processing pragmas
  mMars_ir_v2.clear();
  mMars_ir_v2.build_mars_ir(m_ast, mTopFunc, false, true);
  int node_num = mMars_ir_v2.size();
  map<void *, int> loop2spread;
  bool Changed = false;
  for (int j = 0; j < node_num; ++j) {
    CMarsNode *node = mMars_ir_v2.get_node(j);
    vector<void *> inner_loops = node->get_inner_loops();
    cout << "Insert pragma in " << j << ", " << inner_loops.size() << endl;
    size_t i = 0;
    while (i < inner_loops.size()) {
      void *inner_loop = inner_loops[i++];
      if (loop2spread.find(inner_loop) != loop2spread.end()) {
        continue;
      }
      loop2spread[inner_loop] = 1;
      CMarsLoop *loop_info = mMars_ir_v2.get_loop_info(inner_loop);
      if (loop_info->is_complete_unroll() != 0) {
        int64_t l_range = 0;
        if (!check_legal_complete_unroll(m_ast, inner_loop, &l_range)) {
          continue;
        }
        void *scope = m_ast->GetParent(inner_loop);
        std::ostringstream oss;
        oss << "unroll";
        void *sg_pragma = m_ast->CreatePragma(oss.str(), scope);
        m_ast->InsertStmt(sg_pragma, inner_loop, false /*avoid pragma */,
                          false /*before label */);
        reportCompleteUnroll(m_ast, inner_loop, l_range);
        Changed = true;
        map<string, string> msg_map;
        msg_map["fine_grained_parallel"] = "on";
        msg_map["fine_grained_parallel_complete"] = "on";
        msg_map["fine_grained_parallel_factor"] = my_itoa(l_range);
        insertMessage(m_ast, inner_loop, msg_map);
        continue;
      }
      int factor = 1;
      if (loop_info->is_partial_unroll(&factor) != 0) {
        void *scope = m_ast->GetParent(inner_loop);
        std::ostringstream oss;
        oss << "unroll " << factor;
        void *sg_pragma = m_ast->CreatePragma(oss.str(), scope);
        m_ast->InsertStmt(sg_pragma, inner_loop, false /*avoid pragma */,
                          false /*before label */);
        if (factor > 1) {
          reportUnrollFactor(m_ast, inner_loop, factor);
        }
        Changed = true;
        map<string, string> msg_map;
        msg_map["fine_grained_parallel"] = "on";
        msg_map["fine_grained_parallel_factor"] = my_itoa(factor);
        insertMessage(m_ast, inner_loop, msg_map);
        continue;
      }
    }
    vector<void *> outer_loops = node->get_loops();
    cout << "Insert pragma " << outer_loops.size() << endl;
    for (auto outer_loop : outer_loops) {
      if (loop2spread.find(outer_loop) != loop2spread.end()) {
        continue;
      }
      loop2spread[outer_loop] = 1;
      CMarsLoop *loop_info = mMars_ir_v2.get_loop_info(outer_loop);
      if (loop_info->is_complete_unroll() != 0) {
        int64_t l_range = 0;
        if (!check_legal_complete_unroll(m_ast, outer_loop, &l_range)) {
          continue;
        }
        void *scope = m_ast->GetParent(outer_loop);
        std::ostringstream oss;
        oss << "unroll";
        void *sg_pragma = m_ast->CreatePragma(oss.str(), scope);
        m_ast->InsertStmt(sg_pragma, outer_loop, false /*avoid pragma */,
                          false /*before label */);
        reportCompleteUnroll(m_ast, outer_loop, l_range);
        Changed = true;
        map<string, string> msg_map;
        msg_map["fine_grained_parallel"] = "on";
        msg_map["fine_grained_parallel_complete"] = "on";
        msg_map["fine_grained_parallel_factor"] = my_itoa(l_range);
        insertMessage(m_ast, outer_loop, msg_map);
        continue;
      }
      int factor = 1;
      if (loop_info->is_partial_unroll(&factor) != 0) {
        void *scope = m_ast->GetParent(outer_loop);
        std::ostringstream oss;
        oss << "unroll " << factor;
        void *sg_pragma = m_ast->CreatePragma(oss.str(), scope);
        m_ast->InsertStmt(sg_pragma, outer_loop, false /*avoid pragma */,
                          false /*before label */);
        if (factor > 1) {
          reportUnrollFactor(m_ast, outer_loop, factor);
        }
        Changed = true;
        map<string, string> msg_map;
        msg_map["fine_grained_parallel"] = "on";
        msg_map["fine_grained_parallel_factor"] = my_itoa(factor);
        insertMessage(m_ast, outer_loop, msg_map);
        continue;
      }
    }
  }

  mMars_ir_v2.build_mars_ir(m_ast, mTopFunc, false, false);
  mMars_ir_v2.remove_all_pragmas();

  return static_cast<int>(Changed);
}

=======
>>>>>>> 0491c55dabf6bf5048b3d7a0a1365ed633eab9c3
//////////////////////////////////////////////////////////
// Yuxin: 20190604 false dependence checker
// nullptr: there is no loop carried dependency in any loop
// one_loop: innermost loop in which there is loop carried dependency
// ///////////////////////////////////////////////////////
void *test_false_dependency(CSageCodeGen *codegen, void *sg_loop,
                            void *sg_array, int *has_write) {
  *has_write = 1;
  if (codegen->IsForStatement(sg_loop) == 0) {
    return sg_loop;
  }
  void *loop_body = codegen->GetLoopBody(sg_loop);
  if (loop_body == nullptr) {
    return sg_loop;
  }
  void *sg_type = codegen->GetTypebyVar(sg_array);
  if ((codegen->IsPointerType(sg_type) == 0) &&
      (codegen->IsArrayType(sg_type) == 0)) {
    return sg_loop;
  }
  ArrayRangeAnalysis mr_range(sg_array, codegen, loop_body, loop_body, false,
                              false);
  if (mr_range.has_write() == 0) {
    *has_write = 0;
    return nullptr;
  }
  vector<void *> vec_loops;
  codegen->get_perfectly_nested_loops_from_bottom(sg_loop, &vec_loops);

  for (auto one_loop : vec_loops) {
    loop_body = codegen->GetLoopBody(one_loop);
    ArrayRangeAnalysis mr_range(sg_array, codegen, loop_body, loop_body, false,
                                true);
    vector<CMarsRangeExpr> mr_write = mr_range.GetRangeExprWrite();
    bool has_no_dep = false;
    for (auto one_range : mr_write) {
      CMarsExpression lb;
      CMarsExpression ub;
      CMarsExpression len;
      int ret = one_range.get_simple_bound(&lb, &ub);
      if (ret == 0) {
        continue;
      }
#if _PRINT_DEBUG_
      cout << lb.print() << " ----- " << ub.print() << endl;
#endif
      //  check whether lb and ub is linearize with one_loop
      if (((lb.get_coeff(one_loop) == 0) != 0) ||
          ((lb.get_coeff(one_loop) != ub.get_coeff(one_loop)) != 0)) {
        continue;
      }
      CMarsExpression coef = lb.get_coeff(one_loop);
      CMarsExpression new_lb = lb;
      new_lb.substitute(one_loop, 0);
      CMarsExpression new_ub = ub;
      new_ub.substitute(one_loop, 0);
      if (!new_lb.IsLoopInvariant(one_loop) ||
          !new_ub.IsLoopInvariant(one_loop)) {
        continue;
      }
      len = new_ub - new_lb + 1;
#if _PRINT_DEBUG_
      cout << coef.print() << " ----- " << len.print() << endl;
#endif
      //  FIXME: we assume the loop step is one
      //  CMarsExpression diff = coef - len;
      //  check whether the stride is no less than access length
      if ((coef >= len) != 0) {
        has_no_dep = true;
        break;
      }
    }
    if (!has_no_dep) {
#ifdef PROJDEBUG
#if _PRINT_DEBUG_
      cout << "there may be loop carried write after write dependency across "
              "loop \'"
           << codegen->_up(one_loop) << "\' on array \'"
           << codegen->_up(sg_array) << "\'" << endl;
#endif
#endif
      return one_loop;
    }
    if (mr_range.has_read() != 0) {
      has_no_dep = false;
      vector<CMarsRangeExpr> mr_read = mr_range.GetRangeExprRead();
      for (size_t i = 0; i < mr_write.size(); ++i) {
        CMarsRangeExpr one_read = mr_read[i];
        CMarsRangeExpr one_write = mr_write[i];
        CMarsRangeExpr one_range = one_read.Union(one_write);
        CMarsExpression lb;
        CMarsExpression ub;
        CMarsExpression len;
        int ret = one_range.get_simple_bound(&lb, &ub);
        if (ret == 0) {
          continue;
        }
        //  check whether lb and ub is linearize with one_loop
        if (((lb.get_coeff(one_loop) == 0) != 0) ||
            ((lb.get_coeff(one_loop) != ub.get_coeff(one_loop)) != 0)) {
          continue;
        }
        CMarsExpression coef = lb.get_coeff(one_loop);
        CMarsExpression new_lb = lb;
        new_lb.substitute(one_loop, 0);
        CMarsExpression new_ub = ub;
        new_ub.substitute(one_loop, 0);
        if (!new_lb.IsLoopInvariant(one_loop) ||
            !new_ub.IsLoopInvariant(one_loop)) {
          continue;
        }
        len = new_ub - new_lb + 1;
        //  FIXME: we assume the loop step is one
        //  check whether the stride is no less than access length
        if ((coef >= len) != 0) {
          has_no_dep = true;
          break;
        }
      }
      if (!has_no_dep) {
        ArrayRangeAnalysis mr_range(sg_array, codegen, one_loop, one_loop,
                                    false, false);
        vector<CMarsRangeExpr> mr_write = mr_range.GetRangeExprWrite();
        vector<CMarsRangeExpr> mr_read = mr_range.GetRangeExprRead();
        for (size_t i = 0; i < mr_write.size(); ++i) {
          CMarsRangeExpr one_read = mr_read[i];
          CMarsRangeExpr one_write = mr_write[i];
          CMarsRangeExpr one_range = one_read.Intersect(one_write);
          if (one_range.is_empty() != 0) {
            has_no_dep = true;
            break;
          }
        }
      }
      if (!has_no_dep) {
#ifdef PROJDEBUG
#if _PRINT_DEBUG_
        cout << "there may be loop carried read after write dependency across "
                "loop \'"
             << codegen->_up(one_loop) << "\' on array \'"
             << codegen->_up(sg_array) << "\'" << endl;
#endif
#endif
        return one_loop;
      }
    }
  }
  return nullptr;
}

void check_false_dependency_xilinx(CSageCodeGen *codegen, void *sg_pragma,
                                   void *target_var, bool force_on) {
  bool false_dep_tag = true;
  void *loop = codegen->GetEnclosingNode("ForLoop", sg_pragma);
  void *loop_body = codegen->GetLoopBody(loop);

  //  test_false_dependency return:
  //  current bNode loop: there is dependency
  //  nullptr: false dependency, and no need to add loop flatten off directive
  //  outter loop x: need to add loop_flatten off directive in outter loop x
  int has_write;
  void *dep_loop = test_false_dependency(codegen, loop, target_var, &has_write);
  if (dep_loop == loop) {
    if (!force_on) {
      reportIgnorePragmaDependence(codegen, sg_pragma, target_var);
      return;
    }
    reportRiskyFalseDependence(codegen, loop, target_var);
  }

#ifdef PROJDEBUG
  cout << "[MARS-PARALLEL-CODEGEN] Insert dependency false "
          "pragma for Array '"
       << codegen->_up(target_var) << "'" << endl;
#endif

  if (false_dep_tag == true) {
    string pragma_depend_str = std::string(HLS_PRAGMA) + " dependence " +
                               CMOST_variable_low + "=" +
                               codegen->_up(target_var) + " array inter false ";
    void *new_pragma = codegen->CreatePragma(pragma_depend_str, loop_body);
    codegen->InsertStmt(new_pragma, sg_pragma);
    reportApplyPragmaDependence(codegen, target_var, loop);
    return;
  }
}

void check_false_dependency_xilinx(CSageCodeGen *codegen, void *pTopFunc,
                                   const CInputOptions &options) {
  cout << "Check false dependency pragma for Xilinx flow\n";
  vector<void *> vecPragmas;
  codegen->GetNodesByType_int(pTopFunc, "preorder", V_SgPragmaDeclaration,
                              &vecPragmas);

  for (int i = static_cast<int>(vecPragmas.size() - 1); i >= 0; --i) {
    SgPragmaDeclaration *decl =
        isSgPragmaDeclaration(static_cast<SgNode *>(vecPragmas[i]));
    void *scope = codegen->GetScope(decl);
    CAnalPragma curr_pragma(codegen);
    bool errorOut;
    curr_pragma.PragmasFrontendProcessing(decl, &errorOut);
    if (curr_pragma.is_false_dep()) {
      string str_pragma = codegen->UnparseToString(decl);
      string pragma_info = "\'" + str_pragma + "' " +
                           getUserCodeFileLocation(codegen, decl, true);
      string var_str = curr_pragma.get_attribute(CMOST_variable);
      // Yuxin: Nov/21/2019
      // False dependence only use the variables inside the function scope
      // (what about global variable?)
      void *target_var =
          codegen->getInitializedNameFromName(scope, var_str, true);
      if (var_str.empty()) {
        dump_critical_message(PROCS_WARNING_34(pragma_info));

      } else if (!var_str.empty() && (target_var == nullptr)) {
        // TODO(youxiang): probably because the variable has been changed, e.g.
        // by
        //  memory burst/reduction We should trace the variable, or we could
        //  change the variable's name in pragma when doing transformations
        dump_critical_message(PROCS_WARNING_35(pragma_info));

      } else {
        string force_str = curr_pragma.get_attribute(CMOST_force);
        //  Yuxin: 07/04/2019
        //  Set false dependence pragma force on by default
        bool force_on = true;
        if (force_str == "off" || force_str == "OFF") {
          force_on = false;
        }
        check_false_dependency_xilinx(codegen, decl, target_var, force_on);
      }
    }
  }
}

void LoopParallel::check_dependency_msg(
    CMirNode *bNode, string *msg_dep,
    map<void *, map<int, int>> array_partitions) {
  if (bNode->has_pipeline() == 0) {
    return;
  }

  void *loop = m_ast->TraceUpToLoopScope(bNode->ref);
  if (loop == nullptr || m_ast->IsForStatement(loop) == 0) {
    return;
  }

  bNode->set_full_access_table_v2(m_ast);
  //  Turn off merlin auto loop flatten: risky
  bool flatten_off_auto_tag = false;
  // bool flatten_off_auto_vendor = false;
  //  Add vendor flatten off
  bool flatten_off_tag = true;
  void *flatten_off_loop = nullptr;
  for (auto pList : bNode->full_access_table_v2) {
    bool false_dep_tag = true;
    void *arr_init = pList.first;
    void *orig_arr_decl = m_ast->GetVariableDecl(arr_init);
    if (orig_arr_decl) {
      if (m_ast->IsInScope(orig_arr_decl, bNode->ref) != 0)
        continue;
    }
    cout << "Check: " << m_ast->_p(arr_init) << endl;
    if (m_ast->IsXilinxStreamType(
            m_ast->GetBaseType(m_ast->GetTypebyVar(arr_init), false))) {
      cout << "Skip xilinx channel checking: " << m_ast->_p(arr_init) << endl;
      continue;
    }
    //  This function return:
    //  current bNode loop: there is dependency
    //  nullptr: false dependency, and no need to add loop flatten off directive
    //  outter loop x: need to add loop_flatten off directive in outter loop x
    int has_write;
    void *dep_loop = test_false_dependency(m_ast, loop, arr_init, &has_write);
    if (dep_loop == loop) {
      cout << "dep on loop " << m_ast->_p(loop) << endl;
      false_dep_tag = false;
    } else if (dep_loop == nullptr) {
      cout << "outside can be flatten" << endl;
      false_dep_tag = true;
      flatten_off_tag = false;
    } else {
      cout << "outside cannot flatten" << endl;
      false_dep_tag = true;
      flatten_off_auto_tag = true;
      if (flatten_off_loop == nullptr) {
        flatten_off_loop = dep_loop;
      } else {
        //  Yuxin: 07/11/2019
        //  Check whether if the variables has dependency on the same outter
        //  loop
        if (flatten_off_loop != dep_loop) {
          flatten_off_tag = false;
        }
      }
    }

    // Yuxin: Mar/10/2020
    // Turn on automated false dependency insertion
    if (false_dep_tag && has_write == 1) {
      (*msg_dep) += m_ast->_up(arr_init) + ",";
#ifdef PROJDEBUG
      cout << "[MARS-PARALLEL-CODEGEN] no loop-carried dependency "
              "on Array '"
           << m_ast->_up(arr_init) << "'" << endl;
#endif
      string pragma_depend_str = std::string(HLS_PRAGMA) + " dependence " +
                                 CMOST_variable_low + "=" +
                                 m_ast->_up(arr_init) + " array inter false ";
      void *new_pragma = m_ast->CreatePragma(pragma_depend_str, bNode->ref);
      m_ast->PrependChild(bNode->ref, new_pragma);
      //       if (flatten_off_auto_tag)
      //         flatten_off_auto_vendor = true;
    }
  }

  if (flatten_off_tag && flatten_off_loop != nullptr) {
    string p_str = std::string(HLS_PRAGMA) + " LOOP_FLATTEN OFF";
    void *loop_body = m_ast->GetLoopBody(flatten_off_loop);
    void *new_pragma = m_ast->CreatePragma(p_str, loop_body);
    m_ast->PrependChild(loop_body, new_pragma);
    cout << "[MARS-PARALLEL-CODEGEN] Insert loop flatten off pragma\n";
  }

  if (flatten_off_auto_tag && mLoopFlatten) {
    string p_str = std::string(ACCEL_PRAGMA) + " LOOP_FLATTEN OFF AUTO";
    //    if (flatten_off_auto_vendor)
    //      p_str = std::string(HLS_PRAGMA) + " LOOP_FLATTEN OFF";
    void *new_pragma = m_ast->CreatePragma(p_str, bNode->ref);
    m_ast->PrependChild(bNode->ref, new_pragma);
    cout << "[MARS-PARALLEL-CODEGEN] Insert merlin auto loop flatten off "
            "pragma\n";
  }
}

void has_register_off_var(CMarsAST_IF *ast, CMarsIrV2 *mars_ir,
                          const vector<void *> &pragmas, set<void *> *vars) {
  for (auto one_pragma : pragmas) {
    if ("REGISTER" != mars_ir->get_pragma_command(one_pragma)) {
      continue;
    }
    string var_str = mars_ir->get_pragma_attribute(one_pragma, "variable");
    void *one_array = ast->find_variable_by_name(var_str, one_pragma);

    string var_off = mars_ir->get_pragma_attribute(one_pragma, "off");
    boost::algorithm::to_lower(var_off);

    if ("off" == var_off) {
      vars->insert(one_array);
    }
  }
}

void add_loop_tripcount_pragma(CSageCodeGen *codegen, void *pTopFunc,
                               CMarsIr *mars_ir) {
  vector<CMirNode *> kernels;
  set<CMirNode *> all_nodes;
  vector<CMirNode *> all_nodes_vec;
  mars_ir->get_all_kernel_nodes(&kernels, &all_nodes, &all_nodes_vec);
  codegen->init_defuse_range_analysis();
  for (size_t i = 0; i != all_nodes_vec.size(); ++i) {
    CMirNode *node = all_nodes_vec[i];
    if (node->is_function) {
      continue;
    }
    void *loop_body = node->ref;
    void *sg_loop = codegen->GetParent(loop_body);
    bool has_loop_bound_spec = false;
    Rose_STL_Container<SgNode *> pragmaStmts = NodeQuery::querySubTree(
        static_cast<SgNode *>(sg_loop), V_SgPragmaDeclaration);
    for (size_t j = 0; j != pragmaStmts.size(); ++j) {
      void *sg_pragma = pragmaStmts[j];
      void *loop_scope = codegen->TraceUpToLoopScope(sg_pragma);
      if (loop_scope != sg_loop) {
        continue;
      }
      if (mars_ir->isHLSTripCount(sg_pragma)) {
        has_loop_bound_spec = true;
        break;
      }
    }
    if (has_loop_bound_spec) {
      continue;
    }
    void *iv;
    void *lb;
    void *ub;
    void *step;
    void *body;
    bool inc_dir;
    bool inclusive_bound;
    bool ret = codegen->IsCanonicalForLoop(sg_loop, &iv, &lb, &ub, &step, &body,
                                           &inc_dir, &inclusive_bound) != 0;
    int64_t nStep = 0;
    if (ret) {
      ret = (codegen->GetLoopStepValueFromExpr(step, &nStep) != 0);
    }
    if (!ret || (nStep == 0)) {
      continue;
    }
    CMarsRangeExpr var_range =
        CMarsVariable(sg_loop, codegen, nullptr,
                      codegen->TraceUpToFuncDecl(sg_loop))
            .get_range();
    // do not add trip count spec for constant trip count loop
    vector<CMarsExpression> lbs = var_range.get_lb_set();
    vector<CMarsExpression> ubs = var_range.get_ub_set();
    if (lbs.size() == 1 && (lbs[0].IsConstant() != 0) && ubs.size() == 1 &&
        (ubs[0].IsConstant() != 0)) {
      continue;
    }
    if ((var_range.is_const_ub() == 0) || (var_range.is_const_lb() == 0)) {
      CMirNode *parent_node = node->get_parent();
      while ((parent_node != nullptr) && !parent_node->is_kernel) {
        if (parent_node->get_parent() != nullptr) {
          parent_node = parent_node->get_parent();
        } else {
          vector<CMirNode *> preds = parent_node->get_all_predecessor();
          if (!preds.empty()) {
            parent_node = preds[0];
          } else {
            parent_node = nullptr;
          }
        }
      }
      var_range.refine_range_in_scope(
          parent_node != nullptr ? parent_node->ref : nullptr, sg_loop);
    }
    if ((var_range.is_const_ub() == 0) || (var_range.is_const_lb() == 0)) {
      continue;
    }
    int64_t length =
        (var_range.get_const_ub() - var_range.get_const_lb()) / nStep + 1;
    cout << "ub: " << var_range.get_const_ub() << endl;
    cout << "lb: " << var_range.get_const_lb() << endl;
    std::ostringstream oss;
    oss << HLS_PRAGMA << " " << HLS_TRIPCOUNT << " max=" << length;
    void *sg_pragma = codegen->CreatePragma(oss.str(), loop_body);
    codegen->PrependChild(loop_body, sg_pragma);
  }
}

//  the function is to be called in the last stage since the output is not
//  C-compatible
int convert_attribute_pragma_to_type(CSageCodeGen *codegen, void *pTopFunc,
                                     const CInputOptions &options) {
  CMarsIrV2 mars_ir;
  mars_ir.build_mars_ir(codegen, pTopFunc);

  CMarsAST_IF *ast = codegen;

  vector<void *> pragmas;
  int is_compatible = 1;
  int is_cross_func = 1;
  ast->GetNodesByType(ast->GetProject(), "preorder", "SgPragmaDeclaration",
                      &pragmas, is_compatible != 0, is_cross_func != 0);
  //  get_global_pragmas(pragmas);
  set<void *> visited;
  set<void *> off_vars;
  has_register_off_var(ast, &mars_ir, pragmas, &off_vars);
  for (auto one_pragma : pragmas) {
    if ("ATTRIBUTE" != mars_ir.get_pragma_command(one_pragma)) {
      continue;
    }

    string var_str = mars_ir.get_pragma_attribute(one_pragma, "variable");
    void *array = ast->find_variable_by_name(var_str, one_pragma);

    if (array == nullptr) {
      continue;
    }

    if (visited.find(array) != visited.end()) {
      continue;
    }
    cout << " -- pragma: " << ast->_up(one_pragma) << " -> " << ast->_up(array)
         << endl;

    vector<string> attributes;
    if (!mars_ir.get_pragma_attribute(one_pragma, "register").empty()) {
      if (off_vars.count(array) > 0) {
        cout << " -- pragma: " << ast->_up(one_pragma) << " -> "
             << ast->_up(array) << endl;
        cout << "     -- Ignore due to user \"register off\" pragma " << endl;
      } else {
        attributes.push_back("register");
      }
    }
    if (!mars_ir.get_pragma_attribute(one_pragma, "singlepump").empty()) {
      attributes.push_back("singlepump");
    }
    if (!mars_ir.get_pragma_attribute(one_pragma, "bankwidth").empty()) {
      string one_attr = "bankwidth(" +
                        mars_ir.get_pragma_attribute(one_pragma, "bankwidth") +
                        ")";
      attributes.push_back(one_attr);
    }
    if (!mars_ir.get_pragma_attribute(one_pragma, "bank_bits").empty()) {
      string one_attr = "bank_bits(" +
                        mars_ir.get_pragma_attribute(one_pragma, "bank_bits") +
                        ")";
      attributes.push_back(one_attr);
    }

    if (attributes.empty()) {
      continue;
    }

    string new_attribute_str;
    for (size_t i = 0; i < attributes.size(); i++) {
      new_attribute_str += attributes[i];
      if (i != attributes.size() - 1) {
        new_attribute_str += ",";
      }
    }
    new_attribute_str = "(" + new_attribute_str + ")";

    //  1. type conversion
    vector<size_t> dim_org;  //  from left to right dim
    void *base_type = nullptr;
    ast->get_type_dimension_new(ast->GetTypebyVar(array), &base_type,
                                &dim_org);  //  left to right
    if (base_type == nullptr) {
      return 0;
    }
    if (dim_org.empty()) {
      return 0;
    }

    //  Update the type
    {
      string new_type_str = ast->_up(base_type);
      new_type_str += " __attribute__(" + new_attribute_str + ")";
      ast->RegisterType(new_type_str);

      void *new_type = ast->CreateArrayType(ast->GetTypeByString(new_type_str),
                                            dim_org);  //  left to right
      string str_old_type = ast->_up(ast->GetTypebyVar(array));
      ast->SetTypetoVar(array, new_type);
      string str_new_type = ast->_up(new_type);

      cout << "     -- Insert attributes in type " << ast->_up(array) << ": "
           << str_old_type << " -> " << str_new_type << endl;
    }

    //  update pragma
    string str_pragma = ast->_up(one_pragma);
    ast->AddComment(str_pragma, ast->TraceUpToStatement(array));
    ast->RemoveStmt(one_pragma, false);
    visited.insert(array);
  }

  return 1;
}

void reportLargeFactor(CSageCodeGen *codegen, CMirNode *node, int old_factor,
                       int range) {
  string factor_str = my_itoa(old_factor);
  void *sg_loop = codegen->TraceUpToLoopScope(node->ref);
  string str_label = codegen->get_internal_loop_label(sg_loop);
  string sFileName = getUserCodeFileLocation(codegen, sg_loop);
  string str_loop = codegen->UnparseToString(codegen->GetLoopIterator(sg_loop));
  string loop_info =
      "'" + str_label + "\' " + getUserCodeFileLocation(codegen, sg_loop, true);
  dump_critical_message(FGPIP_WARNING_9(loop_info, factor_str, my_itoa(range)),
                        0, codegen, sg_loop);

  map<string, string> msg_map;
  msg_map["fine_grained_parallel"] = "off";
  insertMessage(codegen, sg_loop, msg_map);
}

void reportInvalidFactor(CSageCodeGen *codegen, CMirNode *node,
                         string str_factor) {
  void *sg_loop = codegen->TraceUpToLoopScope(node->ref);
  string str_label = codegen->get_internal_loop_label(sg_loop);
  string str_loop = codegen->UnparseToString(codegen->GetLoopIterator(sg_loop));
  string msg = "Parallelization NOT applied: loop ";
  string loop_info = "\'" + str_label + "\' " +
                     getUserCodeFileLocation(codegen, sg_loop, true);
  string msg_r = "Invalid parallel factor: " + str_factor;
  msg += "  Reason: " + msg_r + " \n";
  msg += "  Hint:   parallel factor can only be constant positive integer \n";

  dump_critical_message(FGPIP_WARNING_4(loop_info, str_factor), 0, codegen,
                        sg_loop);

  map<string, string> msg_map;
  msg_map["fine_grained_parallel"] = "off";
  insertMessage(codegen, sg_loop, msg_map);
}

void reportCoarseGrainedParallel(CSageCodeGen *codegen, CMirNode *new_node) {
  void *sg_loop = codegen->GetParent(new_node->ref);
  string str_label = codegen->get_internal_loop_label(sg_loop);
  void *iter = codegen->GetLoopIterator(sg_loop);
  if (iter) {
    str_label = codegen->UnparseToString(iter);
  }
  string sFileName = getUserCodeFileLocation(codegen, sg_loop);
  string loop_info = "'" + str_label + "'\n";
  string file_info = "  (" + sFileName + "), ";
  dump_critical_message(FGPIP_WARNING_2(loop_info, file_info));
}

void reportCompleteUnroll(CSageCodeGen *codegen, void *sg_loop, int range) {
  std::ostringstream oss;
  oss << range;
  string str_label = codegen->get_internal_loop_label(sg_loop);
  void *iter = codegen->GetLoopIterator(sg_loop);
  if (iter) {
    str_label = codegen->UnparseToString(iter);
  }
  string loop_info = "'" + str_label + "'";
  string file_info = " " + getUserCodeFileLocation(codegen, sg_loop, true);
  dump_critical_message(FGPIP_INFO_2(loop_info, file_info));

  map<string, string> msg_map;
  msg_map[FGPAR] = "on";
  msg_map[FGPAR_COMPLETE] = "on";
  msg_map[FGPAR_FACTOR] = oss.str();
  insertMessage(codegen, sg_loop, msg_map);
}

void reportUnrollFactor(CSageCodeGen *codegen, void *sg_loop, int factor) {
  string str_label = codegen->get_internal_loop_label(sg_loop);
  void *iter = codegen->GetLoopIterator(sg_loop);
  if (iter) {
    str_label = codegen->UnparseToString(iter);
  }
  string loop_info =
      "'" + str_label + "' " + getUserCodeFileLocation(codegen, sg_loop, true);
  std::ostringstream oss;
  oss << factor;
  dump_critical_message(FGPIP_INFO_5(loop_info, oss.str()));

  map<string, string> msg_map;
  msg_map[FGPAR] = "on";
  msg_map[FGPAR_FACTOR] = oss.str();
  insertMessage(codegen, sg_loop, msg_map);
}

void reportPipeline(CSageCodeGen *codegen, void *sg_loop, string II_factor,
                    string msg_dep) {
  string str_label;
  string loop_info;
  if (codegen->IsForStatement(sg_loop) != 0) {
    str_label = codegen->get_internal_loop_label(sg_loop);
    void *iter = codegen->GetLoopIterator(sg_loop);
    if (iter) {
      str_label = codegen->UnparseToString(iter);
    }
    loop_info = "'" + str_label + "'";
  } else {
    string str_loop = codegen->UnparseToString(sg_loop).substr(0, 20);
    loop_info = "'" + str_loop + "'";
  }
  string sFileName = getUserCodeFileLocation(codegen, sg_loop);
  string file_info;
  if (!sFileName.empty()) {
    file_info = "  (" + sFileName + ")";
  }
  if (!II_factor.empty()) {
    dump_critical_message(FGPIP_INFO_8(loop_info, file_info, II_factor));
  } else {
    dump_critical_message(FGPIP_INFO_3(loop_info, file_info));
  }

  map<string, string> msg_map;
  msg_map[FGPIP] = "on";
  msg_map[FGPIP_II] = II_factor;
  msg_map[FALSE_DEPENDENCE] = msg_dep;
  insertMessage(codegen, sg_loop, msg_map);
}
void reportRiskyFalseDependence(CSageCodeGen *codegen, void *sg_loop,
                                void *var) {
  string str_label = codegen->get_internal_loop_label(sg_loop);
  void *iter = codegen->GetLoopIterator(sg_loop);
  if (iter) {
    str_label = codegen->UnparseToString(iter);
  }
  string loop_info =
      "'" + str_label + "' " + getUserCodeFileLocation(codegen, sg_loop, true);
  string var_str = codegen->UnparseToString(var);
  string var_info =
      "'" + var_str + "' " + getUserCodeFileLocation(codegen, var, true);

  dump_critical_message(FGPIP_WARNING_17(var_info, loop_info));
}

void reportIgnorePragmaDependence(CSageCodeGen *codegen, void *sg_pragma,
                                  void *var) {
  //    void *sg_loop = codegen->GetEnclosingNode("ForLoop", sg_pragma);
  string var_str = codegen->UnparseToString(var);
  string str_pragma = codegen->UnparseToString(sg_pragma);
  string pragma_info = "\'" + str_pragma + "' " +
                       getUserCodeFileLocation(codegen, sg_pragma, true);
  string var_info =
      "'" + var_str + "' " + getUserCodeFileLocation(codegen, var, true);
  string msg = "Ignoring a false dependence pragma because loop-carried "
               "dependence exists on variable " +
               var_info + "\n";

  dump_critical_message(FGPIP_WARNING_8(pragma_info, var_info));
}

void reportApplyPragmaDependence(CSageCodeGen *codegen, void *var,
                                 void *sg_loop, bool check_all) {
  string str_label = codegen->get_internal_loop_label(sg_loop);
  void *iter = codegen->GetLoopIterator(sg_loop);
  if (iter) {
    str_label = codegen->UnparseToString(iter);
  }
  string loop_info =
      "'" + str_label + "' " + getUserCodeFileLocation(codegen, sg_loop, true);
  string msg = "False dependence pragma applied on loop " + loop_info + "\n";

  if (!check_all) {
    string var_str =
        isSgInitializedName(static_cast<SgNode *>(var))->unparseToString();
    string var_info =
        "'" + var_str + "' " + getUserCodeFileLocation(codegen, var, true);
    dump_critical_message(FGPIP_INFO_6(var_info, loop_info));
  } else {
    dump_critical_message(FGPIP_INFO_7(loop_info));
  }
}

bool check_legal_complete_unroll(CSageCodeGen *codegen, void *sg_loop,
                                 int64_t *l_range) {
  int ret =
      static_cast<int>(codegen->get_loop_trip_count_simple(sg_loop, l_range));
  if (ret == 0) {
    string str_label = codegen->get_internal_loop_label(sg_loop);
    string sFileName = getUserCodeFileLocation(codegen, sg_loop);
    void *iter = codegen->GetLoopIterator(sg_loop);
    if (iter) {
      str_label = codegen->UnparseToString(iter);
    }
    string loop_info = "'" + str_label + "'";
    string file_info = "  (" + sFileName + "), ";
    dump_critical_message(FGPIP_WARNING_12(loop_info, file_info));

    map<string, string> msg_map;
    msg_map["fine_grained_parallel"] = "off";
    insertMessage(codegen, sg_loop, msg_map);
    return false;
  }
  return true;
}

// Yuxin: Dec/19/2019, check out-of-bound access
void LoopParallel::check_out_of_bound_access() {
  set<void *> visited;
  vector<CMirNode *> vec_nodes;
  mMars_ir.get_topological_order_nodes(&vec_nodes);
  bool OutOfBound = false;
  for (size_t j = 0; j < vec_nodes.size(); j++) {
    CMirNode *node = vec_nodes[j];
    if (!node->is_function)
      continue;

    node->set_full_access_table_v2(m_ast);
    for (auto pList : node->full_access_table_v2) {
      void *arr_init = pList.first;
      cout << "check : " << m_ast->_up(arr_init) << endl;
      cout << "refs number: " << pList.second.size();
      // Yuxin: Jan/14/2020
      // Deal with cross function accesses from actual parameter
      if (m_ast->IsArgumentInitName(arr_init) != 0)
        continue;
      void *base_type;
      vector<size_t> arr_size;
      m_ast->get_type_dimension(m_ast->GetTypebyVar(arr_init), &base_type,
                                &arr_size);
      std::reverse(arr_size.begin(), arr_size.end());

      // FIXME: Yuxin, Jan/03/2020, turn off temporarily
      // because of 1) long running time; 2) will report outofbound by mistake
      if (0) {
        string msg = "";
        bool is_exact = true;
        int ret = m_ast->check_access_out_of_bound(arr_init, node->ref,
                                                   arr_size, &is_exact, &msg);
        if (ret) {
          visited.insert(arr_init);
          void *sg_loop = m_ast->GetEnclosingNode("ForLoop", node->ref);
          string array_info = "'" + m_ast->UnparseToString(arr_init) + "' ";
          array_info += getUserCodeFileLocation(m_ast, arr_init, true);
          string location_info;
          if (sg_loop != nullptr) {
            string str_label = m_ast->get_internal_loop_label(sg_loop);
            void *iter = m_ast->GetLoopIterator(sg_loop);
            if (iter) {
              str_label = m_ast->UnparseToString(iter);
            }
            location_info = "loop \'" + str_label + "\' " +
                            getUserCodeFileLocation(m_ast, sg_loop, true);
          } else {
            location_info =
                "function " + getUserCodeFileLocation(m_ast, node->ref, true);
          }
          dump_critical_message(FGPIP_ERROR_3(array_info, location_info, msg));
          OutOfBound = true;
        }
      }

      // Check access with constant index
      if (visited.count(arr_init) == 0) {
        // Only deal with access not in loop scope
        for (auto ref : pList.second) {
          if (m_ast->IsArrayType(m_ast->GetTypebyVar(arr_init)) == 0)
            continue;

          void *sg_array = nullptr;
          void *sg_pntr;
          vector<void *> sg_indexes;
          int pointer_dim;
          m_ast->parse_pntr_ref_by_array_ref(ref, &sg_array, &sg_pntr,
                                             &sg_indexes, &pointer_dim, nullptr,
                                             true, -1, 1);
          // Not deal with partial accesss
          if (pointer_dim != arr_size.size())
            continue;

          for (size_t i = 0; i < arr_size.size(); i++) {
            void *sg_idx = sg_indexes[i];
            int64_t value = 0;
            if (m_ast->IsConstantInt(sg_idx, &value, true, nullptr)) {
              cout << "constant pntr: " << m_ast->_up(sg_pntr) << endl;
              if (value < 0 || value >= arr_size[i]) {
                string type_info = m_ast->_up(m_ast->GetTypebyVar(arr_init));
                string pntr_info = "'" + m_ast->_up(sg_pntr) + "'";
                string array_info =
                    "'" + m_ast->_up(arr_init) + "' " +
                    getUserCodeFileLocation(m_ast, arr_init, true) +
                    " with type '" + type_info + "'";
                string location_info =
                    getUserCodeFileLocation(m_ast, ref, true);
                dump_critical_message(
                    FGPIP_ERROR_4(pntr_info, array_info, location_info));
                OutOfBound = true;
              }
            }
          }
        }
      }
    }
  }
  if (OutOfBound)
    throw std::exception();
}
