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


#include <tuple>
#include "cmdline_parser.h"
#include "comm_opt.h"
#include "file_parser.h"
#include "mars_ir_v2.h"
#include "xml_parser.h"

#define _DEBUG_CHOPT_OUTPUT_ 1
using MarsProgramAnalysis::CMarsAccess;
using MarsProgramAnalysis::CMarsExpression;
using MarsProgramAnalysis::CMarsRangeExpr;
using MarsProgramAnalysis::CMarsVariable;

using std::cout;
using std::endl;
using std::map;
using std::set;
using std::tuple;
using std::vector;
namespace MarsCommOpt {

int comm_refine_top(CMarsIrV2 *mars_ir) {
  cout << "Communication refinement" << endl;

  CSageCodeGen *codegen = mars_ir->get_ast();
  int count = 0;
  map<CMarsNode *, map<void *, int>> changed_port;
  vector<CMarsEdge *> edges_of_array;

  for (auto &edge : mars_ir->get_all_edges()) {
    void *port = edge->GetPort();
    if (!edge->is_ssst()) {
#if PROJDEBUG
      cout << "Var " << codegen->_p(port) << ": not ssst, continue\n";
#endif
      continue;
    }

#if PROJDEBUG
    cout << "Var " << codegen->_p(port) << endl;
#endif
    CMarsNode *node1 = edge->GetNode(1);
    CMarsNode *node0 = edge->GetNode(0);

    vector<void *> common_loops;
    common_loops = node0->get_common_loops(node1);
    int comm_level = common_loops.size();

    for (int j = 0; j < 2; ++j) {
      CMarsNode *node = edge->GetNode(j);
      auto loops = node->get_loops();
      //            cout << "[info] common_level: " << comm_level << " in total
      //  level " <<loops.size() <<endl;
      vector<void *> inner_loops = node->get_inner_loops();
      if (loops.empty() && inner_loops.empty()) {
        continue;
      }

      if (changed_port.find(node) != changed_port.end()) {
        map<void *, int> c_port = changed_port[node];
        if (c_port.find(port) != c_port.end()) {
          continue;
        }
      }

      if (mars_ir->is_kernel_port(port)) {
        continue;
      }

      map<void *, vector<void *>> refine_iter_map;
      set<void *> refs0 = node->get_port_references(port);
      for (set<void *>::iterator r0 = refs0.begin(); r0 != refs0.end(); r0++) {
        void *ref = *r0;
        vector<void *> refine_iter_vec;
#if PROJDEBUG
        cout << "\nref: " << codegen->_p(ref) << endl;
#endif
        //    cout << "==>scope " <<codegen->_p(codegen->GetParent(ref)) <<
        //    endl;
        bool p_tag = test_parallel(codegen, mars_ir, node, ref);
        //        if(!isSgPntrArrRefExp((SgNode*)ref)) continue;
        CMarsAccess acc(ref, codegen, nullptr);
        if ((codegen->IsPntrArrRefExp(ref) != 0) && !acc.is_simple_type_v1()) {
          continue;
        }

        auto acc_ref_vec = acc.simple_type_v1_get_dim_to_var();
        int c = comm_level;

        for (auto &loop : loops) {
          if (c > 0) {
            c--;
            continue;
          }
          void *loop_iter = codegen->GetLoopIterator(loop);
          void *loop_test = codegen->GetLoopTest(loop);
          void *loop_init = codegen->GetLoopInit(loop);
          void *loop_incre = codegen->GetLoopIncrementExpr(loop);
          if ((codegen->IsInScope(ref, loop_test) != 0) ||
              (codegen->IsInScope(ref, loop_init) != 0) ||
              (codegen->IsInScope(ref, loop_incre) != 0)) {
            continue;
          }

          //                    cout << "Normal Iterator " <<
          //  codegen->_p(loop_iter) << endl;
          bool find_tag = false;
          for (size_t i = 0; i != acc_ref_vec.size(); ++i) {
            void *curr_loop = std::get<0>(acc_ref_vec[i]);
            if (curr_loop == nullptr) {
              continue;
            }
            void *curr_iter = codegen->GetLoopIterator(curr_loop);
            if (curr_iter == nullptr) {
              continue;
            }
            //                            cout << "Curr iterator " <<
            //  codegen->_p(curr_iter)
            //  << endl;
            if (loop_iter == curr_iter) {
              find_tag = true;
            }
          }
          if (!find_tag) {
#if PROJDEBUG
            cout << "refine iterator " << codegen->_p(loop_iter) << endl;
#endif
            refine_iter_vec.push_back(loop);
          } else if ((codegen->IsVarRefExp(ref) != 0) &&
                     (codegen->IsPntrArrRefExp(ref) == 0)) {
#if PROJDEBUG
            cout << "refine iterator " << codegen->_p(loop_iter) << endl;
#endif
            refine_iter_vec.push_back(loop);
          }
        }

        if (p_tag) {
          for (auto &loop : inner_loops) {
            if (codegen->IsInScope(ref, loop) == 0) {
              continue;
            }

            void *loop_iter = codegen->GetLoopIterator(loop);
            //                cout << "Parallel Iterator " <<
            //  codegen->_p(loop_iter)
            //  << endl;

            bool find_tag = false;
            for (size_t i = 0; i != acc_ref_vec.size(); ++i) {
              void *curr_loop = std::get<0>(acc_ref_vec[i]);
              if (curr_loop == nullptr) {
                continue;
              }
              void *curr_iter = codegen->GetLoopIterator(curr_loop);
              if (curr_iter == nullptr) {
                continue;
              }
              if (loop_iter == curr_iter) {
                find_tag = true;
              }
            }
            if (!find_tag) {
#if PROJDEBUG
              cout << "refine iterator " << codegen->_p(loop_iter) << endl;
#endif
              refine_iter_vec.push_back(loop);
            }
          }
        }
        if (!refine_iter_vec.empty()) {
          refine_iter_map[ref] = refine_iter_vec;
        }
      }

      int refine_type = 0;

      //  Note: refine_type, 1-refine for the iterators;2-refine for multiple
      //  refs If it has both the reasons above, refine_type=1
      if (!refine_iter_map.empty()) {
        refine_type = 1;
      } else if (refs0.size() > 1 && refine_iter_map.empty()) {
        refine_type = 2;

        for (set<void *>::iterator r0 = refs0.begin(); r0 != refs0.end();
             r0++) {
          vector<void *> n_loop;
          void *ref = *r0;
          refine_iter_map[ref] = n_loop;
        }
      } else if (codegen->IsScalarType(port) != 0) {
        //                cout <<"==> scalar: " <<codegen->_p(port) <<endl;
        refine_type = 2;

        for (set<void *>::iterator r0 = refs0.begin(); r0 != refs0.end();
             r0++) {
          vector<void *> n_loop;
          void *ref = *r0;
          refine_iter_map[ref] = n_loop;
        }
      }

      //                cout <<"refine_type: " <<refine_type <<",
      //  size:"<<refine_iter_map.size() <<endl;                 cout <<"[NOTE]
      //  refine_type, 1-refine for the iterators;2-refine for multiple refs;"
      //                    <<"3-refine for single scalar\n\n";

      if (refine_type != 0) {
        changed_port[node][port] = 1;
        refine_transformation(codegen, node, port, refine_iter_map, count, j,
                              refine_type);
        count++;
      }
    }
  }
  return 0;
}

void refine_transformation(CSageCodeGen *codegen, CMarsNode *node, void *port,
                           const map<void *, vector<void *>> &refine_iter_map,
                           int port_count, int edge_loc, int refine_type) {
  string arr_str = my_itoa(port_count);
  void *new_var = nullptr;
  void *final_w = nullptr;
  void *first_r = nullptr;
  set<void *> refs0 = node->get_port_references(port);
  //    cout << "refs:" <<refs0.size() <<endl;
  void *parent_scope = nullptr;
  if (refs0.size() > 1) {
    parent_scope = get_public_scope(codegen, refs0);
  }

  for (auto r_iter = refine_iter_map.rbegin(); r_iter != refine_iter_map.rend();
       r_iter++) {
    void *sg_pntr = r_iter->first;
    if (codegen->has_write_conservative(sg_pntr) != 0) {
      final_w = sg_pntr;
      break;
    }
  }

  for (auto iter : refine_iter_map) {
    void *sg_pntr = iter.first;
#if PROJDEBUG
    cout << "Transformation for: " << codegen->_p(sg_pntr) << endl;
#endif
    if (codegen->has_read_conservative(sg_pntr) != 0) {
      first_r = sg_pntr;
      break;
    }
  }
  bool write_out = false;
  bool read_in = false;
  int count = 0;
  for (auto refine_iter : refine_iter_map) {
    if (refine_iter.second.size() > 1) {
      cout << "[info] Multiple iterators for refinement. \n";
    }
    void *sg_pntr = refine_iter.first;
    if (refs0.size() == 1) {
      parent_scope = codegen->GetParent(sg_pntr);
    }

    if (count == 0) {
      void *sg_func_decl = codegen->GetEnclosingNode("Function", sg_pntr);
      void *sg_func_body = codegen->GetFuncBody(sg_func_decl);
      void *sg_type = codegen->GetTypebyVar(port);
      string refine_buf_name = codegen->_up(port) + "_rf" + arr_str;
      codegen->get_valid_local_name(sg_func_decl, &refine_buf_name);
      new_var = codegen->CreateVariableDecl(sg_type, refine_buf_name, nullptr,
                                            sg_func_body);
      codegen->PrependChild(sg_func_body, new_var);
    }
    ROSE_ASSERT(new_var);
    count++;

    if (edge_loc == 0) {
#if PROJDEBUG
      cout << "[write ]: " << codegen->_p(refine_iter.first) << ":"
           << refine_iter.second.size() << endl;
#endif
      //  FIXME: Very simple dominate analysis
      write_out = sg_pntr == final_w;

      read_in = sg_pntr == first_r;

      refine_write_ref(codegen, node, port, sg_pntr, refine_iter.second,
                       new_var, write_out, read_in, refine_type, parent_scope);
    } else {
#if PROJDEBUG
      cout << "[read ]: " << codegen->_p(refine_iter.first) << ":"
           << refine_iter.second.size() << endl;
#endif
      read_in = sg_pntr == first_r;
      refine_read_ref(codegen, node, port, sg_pntr, refine_iter.second, new_var,
                      read_in, refine_type, parent_scope);
    }
  }

  for (set<void *>::iterator r0 = refs0.begin(); r0 != refs0.end(); r0++) {
    void *ref = *r0;
    if (refine_iter_map.find(ref) == refine_iter_map.end()) {
      //            cout << "Replace the other related refs: "
      //                << codegen->_p(ref) << endl;
      SgExpression *old_ref = static_cast<SgExpression *>(ref);
      SgInitializedName *new_port = static_cast<SgInitializedName *>(
          codegen->GetVariableInitializedName(new_var));
      vector<void *> vec_old_refs;
      vector<void *> vec_new_refs;
      void *orig_port = nullptr;
      codegen->parse_pntr_ref_new(ref, &orig_port, &vec_old_refs);
      std::reverse(vec_old_refs.begin(), vec_old_refs.end());
      assert(orig_port == port);
      for (size_t i = 0; i < vec_old_refs.size(); i++) {
        //            vec_new_refs.push_back(
        //                        (SgExpression*)codegen->CopyExp(isSgExpression((SgNode
        //  *)vec_old_refs[i])));
        CMarsExpression me_expr(vec_old_refs[i], codegen);
        vec_new_refs.push_back(static_cast<SgExpression *>(
            codegen->CopyExp(me_expr.get_expr_from_coeff())));
      }

      SgExpression *new_ref =
          static_cast<SgExpression *>(codegen->CreateVariableRef(new_port));
#if 1  //  Only build scalar for each access?
      new_ref = static_cast<SgExpression *>(
          codegen->CreateArrayRef(new_ref, vec_new_refs));
#endif
      codegen->ReplaceExp(old_ref, new_ref);
    }
  }
}

void refine_write_ref(CSageCodeGen *codegen, CMarsNode *node, void *port,
                      void *ref, vector<void *> loops, void *new_var,
                      bool write_out, bool read_in, int refine_type,
                      void *parent_scope) {
#if PROJDEBUG
  cout << "Write refinement transformation for " << codegen->_p(ref) << endl;
#endif

  //  Preparation
  bool read_flag = codegen->has_read_conservative(ref) != 0;
  SgExpression *old_ref = static_cast<SgExpression *>(ref);
  //    SgExpression *old_ref_cp = (SgExpression*)codegen->CopyExp(old_ref);
  //  SgBasicBlock * scope_bb =
  //  SageInterface::getEnclosingNode<SgBasicBlock>(static_cast<SgNode
  //  *>(parent_scope));
  SgStatement *ref_stmt =
      SageInterface::getEnclosingNode<SgStatement>(static_cast<SgNode *>(ref));
  auto m_loops = node->get_loops();
  auto inner_loops = node->get_inner_loops(ref);
  map<void *, int> unrelated_parallel_loops;  //  parallelized loops which are
                                              //  unrelated to the index
  map<void *, int> related_parallel_loops;    //  parallelized loops which are
                                              //  related to the index
  map<void *, int> related_unparallel_loops;  //  unparallelized loops which are
                                              //  related to the index

  loop_classification(codegen, node, ref, loops, &unrelated_parallel_loops,
                      &related_parallel_loops, &related_unparallel_loops);

  //  Note: To test wether if refinement transformation is already applied
  //  manually
  bool exec = false;
  if (refine_type == 1) {
    bool refine_exist = true;
    for (size_t j = 0; j < loops.size(); j++) {
      void *loop = loops[j];
      CMarsRangeExpr mr =
          CMarsVariable(loop, codegen, parent_scope).get_range();
      CMarsExpression me_lb;
      CMarsExpression me_ub;
      int ret = mr.get_simple_bound(&me_lb, &me_ub);
      if (unrelated_parallel_loops.find(loop) ==
          unrelated_parallel_loops.end()) {
        if (ret == 0) {
#if PROJDEBUG
          cout << "loop: " << codegen->_p(loop)
               << " has no simple bound, so stop refining "
               << codegen->_p(old_ref) << endl;
#endif
          return;
        }
      }
      CMarsExpression me_range = me_ub - me_lb;
      if (me_range.get_const() != 0) {
        refine_exist = false;
      }
    }

    if (refine_exist) {
      cout << "[info] Fully refinement probably already exists on this "
              "variable. Quit transformation.\n";
      if (!unrelated_parallel_loops.empty()) {
        exec = true;
      } else {
        return;
      }
    }
  }

  //    if(inner_loops.size() && refine_type==1)
  //        scope_bb =
  //  SageInterface::getEnclosingNode<SgBasicBlock>((SgNode*)inner_loops[0]);

  SgInitializedName *new_port = static_cast<SgInitializedName *>(
      codegen->GetVariableInitializedName(new_var));
  vector<void *> vec_old_refs;
  vector<void *> vec_new_refs;
  void *orig_port = nullptr;
  codegen->parse_pntr_ref_new(ref, &orig_port, &vec_old_refs);
  std::reverse(vec_old_refs.begin(), vec_old_refs.end());
  assert(orig_port == port);
  for (size_t i = 0; i < vec_old_refs.size(); i++) {
    //    vec_new_refs.push_back(
    //            (SgExpression*)codegen->CopyExp(isSgExpression((SgNode
    //  *)vec_old_refs[i])));
    CMarsExpression me_expr(vec_old_refs[i], codegen);
    vec_new_refs.push_back(static_cast<SgExpression *>(
        codegen->CopyExp(me_expr.get_expr_from_coeff())));
  }

  SgExpression *new_ref =
      static_cast<SgExpression *>(codegen->CreateVariableRef(new_port));
  SgExpression *old_ref_cp =
      static_cast<SgExpression *>(codegen->CreateVariableRef(port));
#if 1  //  Only build scalar for each access?
  new_ref = static_cast<SgExpression *>(
      codegen->CreateArrayRef(new_ref, vec_new_refs));
  old_ref_cp = static_cast<SgExpression *>(
      codegen->CreateArrayRef(old_ref_cp, vec_new_refs));
#endif
#if PROJDEBUG
  cout << codegen->_p(new_ref) << endl;
#endif
  codegen->ReplaceExp(old_ref, new_ref);

  SgExpression *lb_cond = nullptr;
  SgExpression *ub_cond = nullptr;

  bool start_cond = false;
  //  Note: build refinement if condition for the unrelated unparallel loops
  for (size_t j = 0; j < loops.size(); j++) {
    void *loop = loops[j];

    if (unrelated_parallel_loops.find(loop) == unrelated_parallel_loops.end()) {
      CMarsRangeExpr mr =
          CMarsVariable(loop, codegen, parent_scope).get_range();
      CMarsExpression me_lb;
      CMarsExpression me_ub;
      int ret = mr.get_simple_bound(&me_lb, &me_ub);
      if (ret == 0) {
        cout << "loop: " << codegen->_p(loop) << " has no simple bound\n";
        continue;
      }
      CMarsExpression me_range = me_ub - me_lb;
      if (me_range.get_const() == 0) {
        cout << "[info] Partial refinement probably exist for this loop on the "
                "variable\n";
        //  continue;
      }
#if PROJDEBUG
      cout << "[range ]" << me_lb.print_s() << "," << me_ub.print_s() << endl;
#endif
      void *iter = codegen->GetLoopIterator(loop);
      if (!start_cond) {
        lb_cond = SageBuilder::buildEqualityOp(
            static_cast<SgVarRefExp *>(codegen->CreateVariableRef(iter)),
            isSgExpression(static_cast<SgNode *>(me_lb.get_expr_from_coeff())));
        ub_cond = SageBuilder::buildEqualityOp(
            static_cast<SgVarRefExp *>(codegen->CreateVariableRef(iter)),
            isSgExpression(static_cast<SgNode *>(me_ub.get_expr_from_coeff())));
        start_cond = true;
      } else {
        SgExpression *lb_exp = SageBuilder::buildEqualityOp(
            static_cast<SgVarRefExp *>(codegen->CreateVariableRef(iter)),
            isSgExpression(static_cast<SgNode *>(me_lb.get_expr_from_coeff())));
        SgExpression *ub_exp = SageBuilder::buildEqualityOp(
            static_cast<SgVarRefExp *>(codegen->CreateVariableRef(iter)),
            isSgExpression(static_cast<SgNode *>(me_ub.get_expr_from_coeff())));

        lb_cond = SageBuilder::buildAndOp(lb_cond, lb_exp);
        ub_cond = SageBuilder::buildAndOp(ub_cond, ub_exp);
      }
    }
  }

  //  Note: build refinement if condition for the related unparallel loops
  //        if(lb_cond && ub_cond && refine_type==1)
  {
    for (auto &up_loop : related_unparallel_loops) {
      void *loop = up_loop.first;

      CMarsRangeExpr mr_scope =
          CMarsVariable(loop, codegen, parent_scope).get_range();
      CMarsRangeExpr mr = CMarsVariable(loop, codegen).get_range();
      CMarsExpression me_lb;
      CMarsExpression me_scope_lb;
      CMarsExpression me_ub;
      CMarsExpression me_scope_ub;
      int ret = mr_scope.get_simple_bound(&me_scope_lb, &me_scope_ub);
      ret &= mr.get_simple_bound(&me_lb, &me_ub);
      if (ret == 0) {
        cout << "loop: " << codegen->_p(loop) << " has no simple bound\n";
        continue;
      }
      CMarsExpression me_lb_range = me_scope_lb - me_lb;
      CMarsExpression me_ub_range = me_ub - me_scope_ub;
      if (me_lb_range.get_const() == 0 && me_ub_range.get_const() == 0) {
        continue;
      }
#if PROJDEBUG
      cout << "[range difference: ] loop(" << me_lb.print_s() << ","
           << me_ub.print_s() << "), scope(" << me_scope_lb.print_s() << ","
           << me_scope_ub.print_s() << ")" << endl;
#endif
      void *iter = codegen->GetLoopIterator(loop);
      if (me_lb_range.get_const() > 0) {
        if (!start_cond) {
          lb_cond = SageBuilder::buildGreaterOrEqualOp(
              static_cast<SgVarRefExp *>(codegen->CreateVariableRef(iter)),
              isSgExpression(
                  static_cast<SgNode *>(me_scope_lb.get_expr_from_coeff())));
          ub_cond = SageBuilder::buildGreaterOrEqualOp(
              static_cast<SgVarRefExp *>(codegen->CreateVariableRef(iter)),
              isSgExpression(
                  static_cast<SgNode *>(me_scope_lb.get_expr_from_coeff())));

          start_cond = true;
        } else {
          SgExpression *lb_exp = SageBuilder::buildGreaterOrEqualOp(
              static_cast<SgVarRefExp *>(codegen->CreateVariableRef(iter)),
              isSgExpression(
                  static_cast<SgNode *>(me_scope_lb.get_expr_from_coeff())));

          lb_cond = SageBuilder::buildAndOp(lb_cond, lb_exp);
          ub_cond = SageBuilder::buildAndOp(ub_cond, lb_exp);
        }
      }
      if (me_ub_range.get_const() > 0) {
        if (!start_cond) {
          lb_cond = SageBuilder::buildLessOrEqualOp(
              static_cast<SgVarRefExp *>(codegen->CreateVariableRef(iter)),
              isSgExpression(
                  static_cast<SgNode *>(me_scope_ub.get_expr_from_coeff())));
          ub_cond = SageBuilder::buildLessOrEqualOp(
              static_cast<SgVarRefExp *>(codegen->CreateVariableRef(iter)),
              isSgExpression(
                  static_cast<SgNode *>(me_scope_ub.get_expr_from_coeff())));

          start_cond = true;
        } else {
          SgExpression *ub_exp = SageBuilder::buildLessOrEqualOp(
              static_cast<SgVarRefExp *>(codegen->CreateVariableRef(iter)),
              isSgExpression(
                  static_cast<SgNode *>(me_scope_ub.get_expr_from_coeff())));

          lb_cond = SageBuilder::buildAndOp(lb_cond, ub_exp);
          ub_cond = SageBuilder::buildAndOp(ub_cond, ub_exp);
        }
      }
    }
  }
  /*        if(lb_cond && exist_cond)
              lb_cond = buildAndOp(lb_cond, exist_cond);

              if(ub_cond && exist_cond)
              ub_cond = buildAndOp(ub_cond, exist_cond);
              */
  //  Note: build refinement statement
  SgStatement *r_init = SageBuilder::buildAssignStatement(
      static_cast<SgExpression *>(codegen->CopyExp(new_ref)),
      static_cast<SgExpression *>(codegen->CopyExp(old_ref_cp)));
  if (refine_type == 1 && (lb_cond != nullptr) && inner_loops.empty() &&
      !exec) {
    //            cout << "[info] read & write\n";
    SgStatement *init_stmt = SageBuilder::buildAssignStatement(
        static_cast<SgExpression *>(codegen->CopyExp(new_ref)),
        static_cast<SgExpression *>(codegen->CopyExp(old_ref_cp)));
    SgBasicBlock *if_body =
        static_cast<SgBasicBlock *>(codegen->CreateBasicBlock(init_stmt));
    SgStatement *node_if_cond_stmt = SageBuilder::buildExprStatement(lb_cond);
    SgIfStmt *node_if_stmt =
        SageBuilder::buildIfStmt(node_if_cond_stmt, if_body, nullptr);
    r_init = node_if_stmt;
    //        } else if(refine_type==2 || (!lb_cond && p_tag)) {
    //    } else if(refine_type==2 || inner_loops.size() > 0 ) {
  }

  SgStatement *w_init = SageBuilder::buildAssignStatement(
      static_cast<SgExpression *>(codegen->CopyExp(old_ref_cp)),
      static_cast<SgExpression *>(codegen->CopyExp(new_ref)));
  if (refine_type == 1 && (ub_cond != nullptr) && inner_loops.empty() &&
      !exec) {
    SgStatement *post = SageBuilder::buildAssignStatement(
        static_cast<SgExpression *>(codegen->CopyExp(old_ref_cp)),
        static_cast<SgExpression *>(codegen->CopyExp(new_ref)));
    SgBasicBlock *if_body = SageBuilder::buildBasicBlock(post);
    SgStatement *node_if_cond_stmt = SageBuilder::buildExprStatement(ub_cond);
    SgIfStmt *node_if_stmt =
        SageBuilder::buildIfStmt(node_if_cond_stmt, if_body, nullptr);
    w_init = node_if_stmt;
  }

  //  Note: build copied for-statement for refinement preprocess
  //  int np_level=unrelated_parallel_loops.size();

  if (/*np_level > 0 && */ !inner_loops.empty() && (r_init != nullptr) &&
      (w_init != nullptr) /* && refine_type==1*/) {
    //    cout << "Build for loop for refinement preprocess\n";
    for (size_t j = 0; j < inner_loops.size(); j++) {
      void *loop = inner_loops[j];
      //            if(unrelated_parallel_loops.find(loop) ==
      //  unrelated_parallel_loops.end() &&
      if (related_parallel_loops.find(loop) != related_parallel_loops.end()) {
        if (codegen->IsInScope(ref_stmt, loop) != 0) {
#if PROJDEBUG
          cout << "[inner iter] " << codegen->_p(loop) << endl;
#endif
          SgForStatement *copyLoop = static_cast<SgForStatement *>(loop);
          SgStatement *init_stmt = static_cast<SgStatement *>(
              codegen->CopyStmt(copyLoop->get_init_stmt()[0]));
          ROSE_ASSERT(init_stmt);
          SgStatement *cond =
              static_cast<SgStatement *>(static_cast<SgStatement *>(
                  codegen->CopyStmt(isSgExprStatement(copyLoop->get_test()))));
          ROSE_ASSERT(cond);
          SgExpression *incr = static_cast<SgExpression *>(
              codegen->CopyExp(copyLoop->get_increment()));
          ROSE_ASSERT(incr);
          SgBasicBlock *init_loop_body = SageBuilder::buildBasicBlock();
          SgBasicBlock *w_init_loop_body = SageBuilder::buildBasicBlock();
          SgForStatement *r_initLoop = SageBuilder::buildForStatement(
              init_stmt, cond, incr, init_loop_body);
          SgForStatement *w_initLoop = SageBuilder::buildForStatement(
              static_cast<SgStatement *>(codegen->CopyStmt(init_stmt)),
              static_cast<SgStatement *>(codegen->CopyStmt(cond)),
              static_cast<SgExpression *>(codegen->CopyExp(incr)),
              w_init_loop_body);

          std::ostringstream oss;
          oss << "ACCEL parallel";
          void *sg_pragma = codegen->CreatePragma(oss.str(), init_loop_body);
          void *sg_pragma1 = codegen->CreatePragma(oss.str(), w_init_loop_body);
          codegen->PrependChild_v1(r_init, init_loop_body);
          codegen->PrependChild(init_loop_body, sg_pragma);
          codegen->PrependChild_v1(w_init, w_init_loop_body);
          codegen->PrependChild(w_init_loop_body, sg_pragma1);

          r_init = r_initLoop;
          w_init = w_initLoop;
        }
      }
    }
    if (lb_cond != nullptr) {
      SgBasicBlock *if_body =
          static_cast<SgBasicBlock *>(codegen->CreateBasicBlock(r_init));
      SgStatement *node_if_cond_stmt = SageBuilder::buildExprStatement(lb_cond);
      SgIfStmt *node_if_stmt =
          SageBuilder::buildIfStmt(node_if_cond_stmt, if_body, nullptr);
      r_init = node_if_stmt;
    }
    if (ub_cond != nullptr) {
      SgBasicBlock *if_body = SageBuilder::buildBasicBlock(w_init);
      SgStatement *node_if_cond_stmt = SageBuilder::buildExprStatement(ub_cond);
      SgIfStmt *node_if_stmt =
          SageBuilder::buildIfStmt(node_if_cond_stmt, if_body, nullptr);
      w_init = node_if_stmt;
    }
  }

  //  Note: select the correct insertion location
  //  SgBasicBlock *sg_loop_body =
  //  (SgBasicBlock*)codegen->GetLoopBody(m_loops[m_loops.size()-1]);
  /*    void *w_insert_loc;
          void *r_insert_loc;
          bool w_in_middle=false, r_in_middle=false;
  //  if(sg_loop_body == scope_bb) {
  vector<void*> m_stmts = node->get_stmts();
  int num = codegen->GetChildStmtNum(scope_bb);
  void *w_loc=m_stmts[m_stmts.size()-1];
  int w_idx = codegen->GetChildStmtIdx(scope_bb, w_loc);
  int num_p = codegen->GetChildStmtNum(parent_scope);

  if(inner_loops.size() && refine_type==2 && !ub_cond && num_p > 0) {
  w_insert_loc=codegen->GetChildStmt(parent_scope,num_p-1);
  } else
  w_insert_loc=w_loc;
  */
  vector<void *> m_stmts = node->get_stmts();
  void *w_insert_loc = m_stmts[m_stmts.size() - 1];
  void *r_insert_loc = m_stmts[0];
  //    void *r_loc=m_stmts[0];
  /*    int r_idx = codegen->GetChildStmtIdx(scope_bb, r_loc);
          if(r_idx > 0) {
          r_insert_loc=r_loc;
  //        r_in_middle=true;
  } else if(r_idx < 0 ) {
  if(codegen->IsInScope(r_loc, scope_bb)) {
  cout << "Possible inside a if-statement of the loop body\n";
  //        SgIfStmt *stmt_if =
  SageInterface::getEnclosingNode<SgIfStmt>((SgNode
  *)r_loc);
  //            if(stmt_if && codegen->IsInScope(stmt_if, scope_bb)) {
  SgBasicBlock *if_bb =
  SageInterface::getEnclosingNode<SgBasicBlock>((SgNode*)r_loc); int rr_idx =
  codegen->GetChildStmtIdx(if_bb, r_loc);
  //            if(rr_idx > 0) {
  r_insert_loc=r_loc;
  //                    r_in_middle=true;
  //                }
  //            }
  } else {
  //        if(inner_loops.size() && refine_type==1)
  //            scope_bb =
  SageInterface::getEnclosingNode<SgBasicBlock>((SgNode*)inner_loops[0]);
  if(inner_loops.size() && refine_type==2 && !lb_cond && num_p > 0) {
  r_insert_loc=codegen->GetChildStmt(parent_scope,0);
  } else
  r_insert_loc=r_loc;
  }
  } else {
  r_insert_loc = codegen->GetChildStmt(scope_bb, 0);
  }
  */
  //    }

  if (read_flag && read_in && (r_init != nullptr)) {
    //        if(r_in_middle) {
    //            cout <<"read in middle\n" <<endl;
    codegen->InsertStmt(r_init, r_insert_loc);
    //    }
    //        else
    //            codegen->PrependChild_v1(r_init, scope_bb);
  }
  if (write_out && (w_init != nullptr)) {
    //        if(w_in_middle) {
    //            cout <<"write in middle\n" <<endl;
    codegen->InsertAfterStmt(w_init, w_insert_loc);
    //    }
    //        else {
    //            cout <<"write in other\n" <<endl;
    //            appendStatement(w_init, scope_bb);
    //        }

#if PROJDEBUG
    cout << codegen->_p(w_init) << endl;
#endif
  }
}

void refine_read_ref(CSageCodeGen *codegen, CMarsNode *node, void *port,
                     void *ref, vector<void *> loops, void *new_var,
                     bool read_in, int refine_type, void *parent_scope) {
#if PROJDEBUG
  cout << "Read refinement transformation for " << codegen->_p(ref) << endl;
#endif
  SgExpression *old_ref = static_cast<SgExpression *>(ref);
  //    SgForStatement *forLoop =
  //  SageInterface::getEnclosingNode<SgForStatement>(static_cast<SgNode
  //  *>(ref)); SgBasicBlock
  //  * scope_bb = SageInterface::getEnclosingNode<SgBasicBlock>((SgNode
  //  *)parent_scope);
  SgStatement *ref_stmt =
      SageInterface::getEnclosingNode<SgStatement>(static_cast<SgNode *>(ref));

  //  Note: node loops analysis
  auto m_loops = node->get_loops();
  auto inner_loops = node->get_inner_loops(ref);
  map<void *, int> unrelated_parallel_loops;  //  parallelized loops which are
                                              //  unrelated to the index
  map<void *, int> related_parallel_loops;    //  parallelized loops which are
                                              //  related to the index
  map<void *, int> related_unparallel_loops;  //  unparallelized loops which are
                                              //  related to the index

  loop_classification(codegen, node, ref, loops, &unrelated_parallel_loops,
                      &related_parallel_loops, &related_unparallel_loops);

  /*
       void* outter_loop=forLoop;
       if(unrelated_parallel_loops.size()) {
       for (size_t j = 0; j < inner_loops.size(); j++) {
       void *loop = inner_loops[j];
       if (codegen->IsInScope(outter_loop, loop)) {
       outter_loop=loop;
       cout <<"outter:" << codegen->_p(outter_loop) <<endl;
       }
       }
       }*/

  //  Note: To test wether if refinement transformation is already applied
  //  manually
  bool exec = false;
  if (refine_type == 1) {
    bool refine_exist = true;
    for (size_t j = 0; j < loops.size(); j++) {
      void *loop = loops[j];
      CMarsRangeExpr mr =
          CMarsVariable(loop, codegen, parent_scope).get_range();
      CMarsExpression me_lb;
      CMarsExpression me_ub;
      int ret = mr.get_simple_bound(&me_lb, &me_ub);
      if (unrelated_parallel_loops.find(loop) ==
          unrelated_parallel_loops.end()) {
        if (ret == 0) {
#if PROJDEBUG
          cout << "loop: " << codegen->_p(loop)
               << " has no simple bound, so stop refining "
               << codegen->_p(old_ref) << endl;
#endif
          return;
        }
      }
      CMarsExpression me_range = me_ub - me_lb;
      if (me_range.get_const() != 0) {
        refine_exist = false;
      }
    }

    if (refine_exist) {
      //    cout <<"[info] refinement probably doesnt exist for this variable.
      //  Quit transformation.\n";
      cout << "[info] Fully refinement probably already exists on this "
              "variable. Quit transformation.\n";
      if (!unrelated_parallel_loops.empty()) {
        exec = true;
      } else {
        return;
      }
    }
  }

  SgInitializedName *new_port = static_cast<SgInitializedName *>(
      codegen->GetVariableInitializedName(new_var));

  vector<void *> vec_old_refs;
  vector<void *> vec_new_refs;
  void *orig_port = nullptr;
  codegen->parse_pntr_ref_new(ref, &orig_port, &vec_old_refs);
  std::reverse(vec_old_refs.begin(), vec_old_refs.end());
  assert(orig_port == port);
  for (size_t i = 0; i < vec_old_refs.size(); i++) {
    //    vec_new_refs.push_back(
    //            (SgExpression*)codegen->CopyExp(isSgExpression((SgNode
    //  *)vec_old_refs[i])));
    CMarsExpression me_expr(vec_old_refs[i], codegen);
    vec_new_refs.push_back(static_cast<SgExpression *>(
        codegen->CopyExp(me_expr.get_expr_from_coeff())));
  }

  SgExpression *new_ref =
      static_cast<SgExpression *>(codegen->CreateVariableRef(new_port));
  SgExpression *old_ref_cp =
      static_cast<SgExpression *>(codegen->CreateVariableRef(port));
#if 1
  new_ref = static_cast<SgExpression *>(
      codegen->CreateArrayRef(new_ref, vec_new_refs));
  old_ref_cp = static_cast<SgExpression *>(
      codegen->CreateArrayRef(old_ref_cp, vec_new_refs));
#endif
#if PROJDEBUG
  cout << "old ref: " << codegen->_p(old_ref) << endl;
  cout << "new ref: " << codegen->_p(new_ref) << endl;
#endif
  codegen->ReplaceExp(old_ref, new_ref);

  //  Note: build if-condition for the loop iterators which dont appear in the
  //  index
  SgExpression *lb_cond = nullptr;
  bool start_cond = false;
  for (size_t j = 0; j < loops.size(); j++) {
    void *loop = loops[j];
    if (unrelated_parallel_loops.find(loop) == unrelated_parallel_loops.end()) {
      SgExpression *cond =
          isSgExprStatement((static_cast<SgForStatement *>(loop)->get_test()))
              ->get_expression();
      ROSE_ASSERT(cond);
      CMarsRangeExpr mr =
          CMarsVariable(loop, codegen, parent_scope).get_range();
      CMarsExpression me_lb;
      CMarsExpression me_ub;
      int ret = mr.get_simple_bound(&me_lb, &me_ub);
      if (ret == 0) {
        cout << "loop: " << codegen->_p(loop) << " has no simple bound\n";
        continue;
      }
      CMarsExpression me_range = me_ub - me_lb;
      if (me_range.get_const() == 0) {
        cout << "[info] Partial refinement probably exist for this loop on the "
                "variable\n";
        //        continue;
      }
#if PROJDEBUG
      cout << "[range ]" << me_lb.print_s() << "," << me_ub.print_s() << endl;
#endif
      void *iter = codegen->GetLoopIterator(loop);
      if (!start_cond) {
        lb_cond = SageBuilder::buildEqualityOp(
            static_cast<SgVarRefExp *>(codegen->CreateVariableRef(iter)),
            isSgExpression(static_cast<SgNode *>(me_lb.get_expr_from_coeff())));
        start_cond = true;
      } else {
        SgExpression *lb_exp = SageBuilder::buildEqualityOp(
            static_cast<SgVarRefExp *>(codegen->CreateVariableRef(iter)),
            isSgExpression(static_cast<SgNode *>(me_lb.get_expr_from_coeff())));
        lb_cond = SageBuilder::buildAndOp(lb_cond, lb_exp);
      }
    }
  }

  //  Note: build if-condition for the loop iterators which appear in the index
  //  but range dont match
  //    if(lb_cond  && refine_type==1)
  {
    for (auto &up_loop : related_unparallel_loops) {
      void *loop = up_loop.first;

      CMarsRangeExpr mr_scope =
          CMarsVariable(loop, codegen, parent_scope).get_range();
      CMarsRangeExpr mr = CMarsVariable(loop, codegen).get_range();
      CMarsExpression me_lb;
      CMarsExpression me_scope_lb;
      CMarsExpression me_ub;
      CMarsExpression me_scope_ub;
      int ret = mr_scope.get_simple_bound(&me_scope_lb, &me_scope_ub);
      ret &= mr.get_simple_bound(&me_lb, &me_ub);
      if (ret == 0) {
        cout << "loop: " << codegen->_p(loop) << " has no simple bound\n";
        continue;
      }
      CMarsExpression me_lb_range = me_scope_lb - me_lb;
      CMarsExpression me_ub_range = me_ub - me_scope_ub;
      if (me_lb_range.get_const() == 0 && me_ub_range.get_const() == 0) {
        continue;
      }
#if PROJDEBUG
      cout << "[range difference: ] loop(" << me_lb.print_s() << ","
           << me_ub.print_s() << "), scope(" << me_scope_lb.print_s() << ","
           << me_scope_ub.print_s() << ")" << endl;
#endif
      void *iter = codegen->GetLoopIterator(loop);
      if (me_lb_range.get_const() > 0) {
        if (!start_cond) {
          lb_cond = SageBuilder::buildGreaterOrEqualOp(
              static_cast<SgVarRefExp *>(codegen->CreateVariableRef(iter)),
              isSgExpression(
                  static_cast<SgNode *>(me_scope_lb.get_expr_from_coeff())));
          start_cond = true;
        } else {
          SgExpression *lb_exp = SageBuilder::buildGreaterOrEqualOp(
              static_cast<SgVarRefExp *>(codegen->CreateVariableRef(iter)),
              isSgExpression(
                  static_cast<SgNode *>(me_scope_lb.get_expr_from_coeff())));

          lb_cond = SageBuilder::buildAndOp(lb_cond, lb_exp);
        }
      }
      if (me_ub_range.get_const() > 0) {
        if (!start_cond) {
          lb_cond = SageBuilder::buildLessOrEqualOp(
              static_cast<SgVarRefExp *>(codegen->CreateVariableRef(iter)),
              isSgExpression(
                  static_cast<SgNode *>(me_scope_ub.get_expr_from_coeff())));
          start_cond = true;
        } else {
          SgExpression *ub_exp = SageBuilder::buildLessOrEqualOp(
              static_cast<SgVarRefExp *>(codegen->CreateVariableRef(iter)),
              isSgExpression(
                  static_cast<SgNode *>(me_scope_ub.get_expr_from_coeff())));

          lb_cond = SageBuilder::buildAndOp(lb_cond, ub_exp);
        }
      }
    }
  }

  //    if(lb_cond && exist_cond)
  //        lb_cond = SageBuilder::buildAndOp(lb_cond, exist_cond);

  //  Note: build refinement statement
  SgStatement *init = SageBuilder::buildAssignStatement(
      static_cast<SgExpression *>(codegen->CopyExp(new_ref)),
      static_cast<SgExpression *>(codegen->CopyExp(old_ref_cp)));
  if (refine_type == 1 && (lb_cond != nullptr) && inner_loops.empty() &&
      !exec) {
    SgBasicBlock *if_body = SageBuilder::buildBasicBlock(init);
    SgStatement *node_if_cond_stmt = SageBuilder::buildExprStatement(lb_cond);
    SgIfStmt *node_if_stmt =
        SageBuilder::buildIfStmt(node_if_cond_stmt, if_body, nullptr);
    init = node_if_stmt;
  }

  //  Note: build copied for-statement for refinement preprocess
  //  int np_level=unrelated_parallel_loops.size();

  if (/*np_level > 0 && */ !inner_loops.empty() &&
      (init != nullptr) /* && refine_type==1*/) {
    cout << "Build for loop for refinement preprocess\n";
    for (size_t j = 0; j < inner_loops.size(); j++) {
      void *loop = inner_loops[j];
      //        if(unrelated_parallel_loops.find(loop) ==
      //  unrelated_parallel_loops.end() ||
      if (related_parallel_loops.find(loop) != related_parallel_loops.end()) {
        if (codegen->IsInScope(ref_stmt, loop) != 0) {
          SgForStatement *copyLoop = static_cast<SgForStatement *>(loop);
          SgStatement *init_stmt = static_cast<SgStatement *>(
              codegen->CopyStmt(copyLoop->get_init_stmt()[0]));
          ROSE_ASSERT(init_stmt);
          SgStatement *cond = static_cast<SgStatement *>(
              codegen->CopyStmt(isSgExprStatement(copyLoop->get_test())));
          ROSE_ASSERT(cond);
          SgExpression *incr = static_cast<SgExpression *>(
              codegen->CopyExp(copyLoop->get_increment()));
          ROSE_ASSERT(incr);
          SgBasicBlock *init_loop_body = SageBuilder::buildBasicBlock();
          SgForStatement *initLoop = SageBuilder::buildForStatement(
              init_stmt, cond, incr, init_loop_body);

          std::ostringstream oss;
          oss << "ACCEL parallel";
          void *sg_pragma = codegen->CreatePragma(oss.str(), init_loop_body);
#if PROJDEBUG
          cout << "[inner iter] " << codegen->_p(loop) << endl;
#endif
          codegen->PrependChild(init_loop_body, init);
          codegen->PrependChild(init_loop_body, sg_pragma);
          init = initLoop;
        }
      }
    }
    if (lb_cond != nullptr) {
      SgBasicBlock *if_body = SageBuilder::buildBasicBlock(init);
      SgStatement *node_if_cond_stmt = SageBuilder::buildExprStatement(lb_cond);
      SgIfStmt *node_if_stmt =
          SageBuilder::buildIfStmt(node_if_cond_stmt, if_body, nullptr);
      init = node_if_stmt;
    }
  }

  //  Note: select the correct insertion location
  //    SgBasicBlock *sg_loop_body =
  //  (SgBasicBlock*)codegen->GetLoopBody(m_loops[m_loops.size()-1]);

  //    void *r_insert_loc = 0;
  vector<void *> m_stmts = node->get_stmts();
  void *r_insert_loc = m_stmts[0];
  //        int num = codegen->GetChildStmtNum(scope_bb);
  //        void *r_loc=m_stmts[0];
  //        r_insert_loc=r_loc;
  //        int r_idx = codegen->GetChildStmtIdx(scope_bb, r_loc);
  //        int num_p = codegen->GetChildStmtNum(parent_scope);
  /*        if(r_idx > 0) {
              r_insert_loc=r_loc;
  //        r_in_middle=true;
  } else if(r_idx < 0 ) {
  if(codegen->IsInScope(r_loc, scope_bb)) {
  cout << "Possible inside a basic block of the loop body\n";
  //            SgIfStmt *stmt_if =
  SageInterface::getEnclosingNode<SgIfStmt>((SgNode
  *)r_loc);
  //                if(stmt_if && codegen->IsInScope(stmt_if, scope_bb)) {
  SgBasicBlock *if_bb =
  SageInterface::getEnclosingNode<SgBasicBlock>((SgNode*)r_loc); int rr_idx =
  codegen->GetChildStmtIdx(if_bb, r_loc);
  //            cout << "child if stmt index: " <<rr_idx <<endl;
  //                if(rr_idx > 0) {
  r_insert_loc=r_loc;
  //                }
  //            }
  }
  } else {
  r_insert_loc = codegen->GetChildStmt(scope_bb, 0);
  }
  */
  if (read_in && (init != nullptr)) {
    //    codegen->PrependChild(sg_loop_body, init);
    //  cout <<"read in middle\n" <<endl;
    codegen->InsertStmt(init, r_insert_loc);
    //    }
    /*        else {
                codegen->PrependChild(scope_bb, init);} */
#if PROJDEBUG
    cout << codegen->_p(init) << endl;
#endif
  }
}

void loop_classification(CSageCodeGen *codegen, CMarsNode *node, void *ref,
                         vector<void *> loops,
                         map<void *, int> *unrelated_parallel_loops,
                         map<void *, int> *related_parallel_loops,
                         map<void *, int> *related_unparallel_loops) {
  auto m_loops = node->get_loops();
  auto inner_loops = node->get_inner_loops(ref);

  //  Note: parallelized loops which are unrelated to the index
  for (size_t j = 0; j < loops.size(); j++) {
    void *loop = loops[j];
    bool find = false;
    for (size_t i = 0; i < m_loops.size(); i++) {
      void *m_loop = m_loops[i];
      if (m_loop == loop) {
        find = true;
      }
    }
    if (!find) {
#if PROJDEBUG
      cout << "[Unrelated parallel loop:] " << codegen->_p(loop) << endl;
#endif
      (*unrelated_parallel_loops)[loop] = 1;
    }
  }

  //  Note: parallelized loops which are related to the index
  for (size_t i = 0; i < inner_loops.size(); i++) {
    void *inner_loop = inner_loops[i];
    bool find = false;

    for (size_t i = 0; i < loops.size(); i++) {
      void *loop = loops[i];
      if (loop == inner_loop) {
        find = true;
      }
    }

    if (!find) {
#if PROJDEBUG
      cout << "[Related parallel loop:] " << codegen->_p(inner_loop) << endl;
#endif
      (*related_parallel_loops)[inner_loop] = 1;
    }
  }

  //  Note: unparallelized loops which are related to the index
  for (size_t i = 0; i < m_loops.size(); i++) {
    void *m_loop = m_loops[i];
    bool find = false;

    for (size_t i = 0; i < loops.size(); i++) {
      void *loop = loops[i];
      if (loop == m_loop) {
        find = true;
      }
    }

    if (!find) {
#if PROJDEBUG
      cout << "[Related unparallel loop:] " << codegen->_p(m_loop) << endl;
#endif
      (*related_unparallel_loops)[m_loop] = 1;
    }
  }
}

#if 0
int test_common_loop(CMarsIrV2 *mars_ir) {
  int node_num = mars_ir->size();
  CMarsNode *node0 = mars_ir->get_node(0);
  CMarsNode *node1 = mars_ir->get_node(node_num - 1);
  vector<void *> common_loops;
  common_loops = node0->get_common_loops(node1);
  return common_loops.size();
}

bool test_identical_index(CSageCodeGen *codegen,
                          const map<void *, vector<void *>> &refine_iter_map) {
  for (map<void *, vector<void *>>::iterator it = refine_iter_map.begin();
       it != refine_iter_map.end(); it++) {
    for (map<void *, vector<void *>>::iterator itt = it;
         itt != refine_iter_map.end(); itt++) {
      CMarsAccess access1(it->first, codegen, nullptr);
      CMarsAccess access2(itt->first, codegen, nullptr);
      vector<CMarsExpression> indexes1 = access1.GetIndexes();
      vector<CMarsExpression> indexes2 = access2.GetIndexes();

      int dim = indexes1.size();
      if (indexes2.size() != (size_t)dim)
        return false;

      for (int i = 0; i < dim; i++) {
        if (indexes1[i] - indexes2[i] != 0) {
          cout << "The refs dont satisfy the indentical index limitation.\n";
          return false;
        }
      }
    }
  }
  return true;
}

bool test_identical_index(CSageCodeGen *codegen, const set<void *> &refs) {
  for (set<void *>::iterator it = refs.begin(); it != refs.end(); it++) {
    for (set<void *>::iterator itt = it; itt != refs.end(); itt++) {
      CMarsAccess access1(*it, codegen, nullptr);
      CMarsAccess access2(*itt, codegen, nullptr);
      vector<CMarsExpression> indexes1 = access1.GetIndexes();
      vector<CMarsExpression> indexes2 = access2.GetIndexes();

      int dim = indexes1.size();
      if (indexes2.size() != (size_t)dim)
        return false;

      for (int i = 0; i < dim; i++) {
        if (indexes1[i] - indexes2[i] != 0) {
          cout << "The refs dont satisfy the indentical index limitation.\n";
          return false;
        }
      }
    }
  }
  return true;
}
#endif

bool test_parallel(CSageCodeGen *codegen, CMarsIrV2 *mars_ir, CMarsNode *node,
                   void *ref) {
  bool PARALLEL = false;
  vector<void *> inner_loops = node->get_inner_loops();
  if (inner_loops.empty()) {
    return false;
  }

  for (size_t i = 0; i < inner_loops.size(); i++) {
    void *inner_loop = inner_loops[i];
    if (codegen->IsInScope(ref, inner_loop) == 0) {
      continue;
    }
    CMarsLoop *loop_info = mars_ir->get_loop_info(inner_loop);
    if (loop_info->is_complete_unroll() != 0) {
      PARALLEL = true;
    }
  }
  return PARALLEL;
}

void *get_public_scope(CSageCodeGen *codegen, set<void *> refs0) {
  if (refs0.size() <= 1) {
    return nullptr;
  }

  set<void *>::iterator r0 = refs0.begin();
  void *scope = *r0;
  ++r0;
  while (r0 != refs0.end()) {
    void *ref0 = *r0++;
    scope = codegen->GetCommonPosition(scope, ref0);
  }
#if PROJDEBUG
  cout << "[public scope] refs: " << refs0.size() << endl
       << codegen->_p(scope) << endl;
#endif
  return scope;
}
}  //  namespace MarsCommOpt

int comm_refine_top(CSageCodeGen *codegen, void *pTopFunc,
                    const CInputOptions &options) {
  printf("Hello Communication Optimization (Refinement)... \n");

  CMarsIrV2 mars_ir;
  mars_ir.build_mars_ir(codegen, pTopFunc, false, true);
  return MarsCommOpt::comm_refine_top(&mars_ir);
}
