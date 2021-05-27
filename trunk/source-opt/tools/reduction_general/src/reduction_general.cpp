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

#include "codegen.h"
#include "mars_message.h"
#include "mars_opt.h"
#include "program_analysis.h"
#include "reduction.h"
#include "reduction_general.h"
#include "loop_tiling.h"
#include "rose.h"

#define _DEBUG_REDUC 1
#define USED_CODE_IN_COVERAGE_TEST 0
#define MULTI_LEVEL 0
//  #define NOT_GEN_FUNC

using MarsProgramAnalysis::ArrayRangeAnalysis;
using MarsProgramAnalysis::CMarsAccess;
using MarsProgramAnalysis::CMarsExpression;
using MarsProgramAnalysis::CMarsRangeExpr;

using std::map;
using std::set;
using std::string;
using std::vector;

void add_loop_tripcount_pragma(CSageCodeGen *codegen, void *pTopFunc,
                               CMarsIr *mars_ir);

int reduction_general_top(CSageCodeGen *codegen, void *pTopFunc,
                          const CInputOptions &options) {
  reduction_general(codegen, pTopFunc, options);
  return 0;
}

//  /////////////////////////////////////////////  /
//  Yuxin: Aug 9 2018 edit
//  reduction is combined with parallel pragma
//  /////////////////////////////////////////////  /
void reduction_general(CSageCodeGen *codegen, void *pTopFunc,
                       const CInputOptions &options) {
  std::cout << "==============================================" << std::endl;
  std::cout << "-----=# Reduction General Optimization Start#=----\n";
  std::cout << "==============================================" << std::endl;

  if (!options.get_option_key_value("-a", "naive_hls").empty()) {
    cout << "[MARS-REDUCTION-MSG] Naive HLS mode.\n";
    return;
  }
  CMarsIr mars_ir;
  mars_ir.get_mars_ir(codegen, pTopFunc, options, true);
  mars_ir.update_loop_node_info(codegen);

  //  Preprocess
  add_loop_tripcount_pragma(codegen, pTopFunc, &mars_ir);

  reduction_analysis(codegen, pTopFunc, options, &mars_ir);

  reduction_postprocessing(codegen, pTopFunc, &mars_ir);

  std::cout << "============================================" << std::endl;
  std::cout << "-----=# Reduction General Optimization End#=----\n";
  std::cout << "============================================" << std::endl
            << std::endl;
}

void reduction_analysis(CSageCodeGen *codegen, void *pTopFunc,
                        const CInputOptions &options, CMarsIr *mars_ir) {
  vector<CMirNode *> vec_nodes;
  mars_ir->get_topological_order_nodes(&vec_nodes);
  map<void *, void *> checked_loop;
  string scheme_str = "auto";

  for (size_t j = 0; j < vec_nodes.size(); j++) {
    CMirNode *new_node = vec_nodes[j];
    if (new_node->is_while) {
      continue;
    }
    void *sg_loop = codegen->GetEnclosingNode("ForLoop", new_node->ref);
    //  Check syntax for reduction
    //  IF pipeline pragma is recursively inserted,
    //  only apply the reduction in the innermost level
    if (new_node->is_fine_grain && new_node->has_reduction()) {
      if (!new_node->has_pipeline() && !new_node->has_parallel())
        continue;
      vector<void *> reduc_vars;
      parsing_reduction_pragma(codegen, new_node, &reduc_vars, &scheme_str);
      bool is_tiling = true;
      if (new_node->has_parallel() && new_node->is_complete_unroll()) {
        is_tiling = false;
        auto parent_node = new_node->get_parent();
        if (parent_node != nullptr && !parent_node->has_parallel())
          is_tiling = true;
      }

      if (!is_tiling) {
        string str_label = codegen->get_internal_loop_label(sg_loop);
        string loop_info = "'" + str_label + "' (" +
                           getUserCodeFileLocation(codegen, sg_loop) + ")";
        dump_critical_message(REDUC_WARNING_10(loop_info));
        continue;
      }
      cout << "REDUC scheme: " << scheme_str << endl;
      for (auto target_var : reduc_vars) {
        cout << "REDUC var: " << codegen->_p(target_var) << endl;
        bool checked = false;
        for (auto pair : checked_loop) {
          if (pair.first == target_var) {
            if (codegen->IsInScope(new_node->ref, pair.second) != 0) {
              checked = true;
              break;
            }
          }
        }
        if (checked) {
          cout << "[warning] reduction has been checked in this scope\n";
          continue;
        }

        //  Check the reduction variables
        vector<void *> pntr_to_reduce;
        vector<void *> op_list;
        check_reduction_op_var(codegen, new_node->ref, target_var, &op_list,
                               &pntr_to_reduce);

        //  Yuxin: Aug 17 2018
        //  Limitation: one reduction in a loop scope
        if (op_list.size() > 1) {
          cout << "[warning] Only support one reduction op in a loop scope\n";
          continue;
        }

        for (size_t ii = 0; ii < op_list.size(); ii++) {
          void *arr_pntr_ref = pntr_to_reduce[ii];
          void *curr_op = op_list[ii];
          int scheme_1_enabled = 0;
          int lift_level = 0;
          vector<void *>
              vec_reduct_loops;  //  unrelated loops which need reduction
          vector<void *> vec_parallel_loops;  //  related loops which already
                                              //  have parallelism
          vector<void *> loop_nest;
          map<void *, int> loop_is_related;

          bool ret1 = anal_reduction(
              codegen, mars_ir, new_node, arr_pntr_ref, curr_op, target_var,
              scheme_str, &scheme_1_enabled, &lift_level, &vec_reduct_loops,
              &vec_parallel_loops, &loop_is_related, &loop_nest);

          if (!ret1) {
            cout << "[MARS-REDUCTION-MSG] Reduction is not applied to "
                 << codegen->_up(arr_pntr_ref) << endl;
            continue;
          }

          //  Yuxin: If there is reduction optimization, then comment all the
          //  label statements.
          remove_labels(codegen, pTopFunc);
          int ret = static_cast<int>(build_reduction_general(
              codegen, options, mars_ir, target_var, curr_op, lift_level,
              pntr_to_reduce, scheme_1_enabled, vec_reduct_loops,
              vec_parallel_loops, loop_is_related, loop_nest));

          if (ret != 0) {
            cout << "[MARS-REDUCTION-MSG] Reduction is automatically applied"
                 << endl;
            for (auto loop : loop_nest) {
              checked_loop[target_var] = loop;
            }
          }
        }
      }
    }
  }
}

//  /////////////////////////////////////////////  /
//  ZP: 20151203: Note:
//  There are two ways to handle the reduction loop inside the pipeline loop
//  Scheme0: cyclic mode, the reduction loop is parallelized by allocating
//           additional dimension in the reduction buffer; and then the
//           reduction
//           dimensions are reduced layer-by-layer in the postproc loop nests ?
//  Scheme1: block mode, additional local reduction buffers are allocated inside
//           the pipeline loop; local reduction buffers only contains
//           the parallel dimensions; and their value are reduced to the global
//           reduce buffer at each iteration of the pipeline loop; only one
//           postproc step is needed to reduce the iterations for the pipeline
//           loop
//  Comparison: from current finding, scheme1 is better than scheme0 in case
//           of massive parallelism: less BRAM and LUT utilization, and better
//           timing because of smaller fanout
//  Note: scheme 1 is disabled if the pipeline loop is not a reduction loop
//  #define REDUCE_SCHEME 0
//  #define REDUCE_SCHEME 1

//  ////////////////////////////////////////////////  /
//  ZP: Support the Multi-level reduction (Scheme 0)
//  1. Get the parallel loops and reduction loops
//  2. Create declearation and reference of multiple level for multiple
//  references
//  3. Initialize the reduction buffer with original init value or zero
//  4. Replace the references in the reduciton loop (add dependence false)
//  5. Create post processing loop nests
//  5.1 Each post loop reduce the partial results from level i to leven i+1

//  ////////////////////////////////////////////////  /
//  ZP: Support the block-based reduction (Scheme 1)
//  1. the same as Scheme 0
//  2. Create decleration and reference of local reduction buffers
//  3. Initialize the local reduction buffers using input values
//  4. Replace the references
//  5. simple post reduciton (only one layer)
//

bool build_reduction_general(
    CSageCodeGen *codegen, const CInputOptions &options, CMarsIr *mars_ir,
    void *target_var, void *curr_op, int lift_level,
    vector<void *> pntr_to_reduce, int scheme_1_enabled,
    vector<void *> vec_reduct_loops, vector<void *> vec_parallel_loops,
    const map<void *, int> &loop_is_related, const vector<void *> &loop_nest) {
  int i;
  map<void *, void *> all_func_call_map;
  //  Note: preparation for building
  void *outmost_loop = loop_nest[0];
  void *innermost_loop = loop_nest[loop_nest.size() - 1];
  map<void *, int> loop_is_reduct;  //  unrelated loops which need reduction
  map<void *, int>
      loop_is_parallel;  //  related loops which already have parallelism

  //  Note: find reduction insert location
  void *insertLoop = loop_nest[0];
  for (i = 0; i < lift_level; i++) {
    insertLoop = codegen->GetEnclosingNode("ForLoop", insertLoop);
  }
  // Han: comment because of line number limitation
  cout << "==>Target lifting loop: " << codegen->_p(insertLoop) << endl;
  cout << "==>Target in loop: " << codegen->_p(innermost_loop) << endl;
  map<string, string> msg_map;
  msg_map["node_priority"] = "major";

  int reduct_level = 1;
#if MULTI_LEVEL
  reduct_level = vec_reduct_loops.size();
#endif

  //  ////////////////////////////////////////////////////////////////////////////////
  //  / 2 Create the declaration for the reduction buffers
  //  ////////////////////////////////////////////////////////////////////////////////
  //  /

  vector<void *> new_decl;
  //  the multi-layer reduction variables (for scheme 0)
  vector<vector<void *>> new_decl_ln;
  //  the local reduction variables (for scheme 1)
  vector<void *> new_decl_local;
  void *insert_pos_local = nullptr;
  new_decl_ln.resize(reduct_level - 1);
  vector<void *> vec_new_refs;
  vector<void *> vec_new_refs_init;
  vector<void *> vec_old_refs;  //  make a copy for later use, after replacement
  vector<void *> vec_new_refs_local;
  vector<void *> vec_new_refs_local_init;
  vector<vector<void *>> vec_new_refs_ln;
  vector<vector<void *>> vec_new_refs_ln_init;
  vec_new_refs_ln.resize(reduct_level - 1);
  vec_new_refs_ln_init.resize(reduct_level - 1);

  bool ret = create_reduction_buf_and_refs(
      codegen, scheme_1_enabled, pntr_to_reduce, &new_decl, &new_decl_ln,
      &new_decl_local, &vec_new_refs_local, &vec_new_refs_local_init,
      &vec_old_refs, &vec_new_refs, &vec_new_refs_ln, &vec_new_refs_ln_init,
      vec_reduct_loops, vec_parallel_loops, loop_nest, loop_is_related,
      &loop_is_reduct, &loop_is_parallel, &insert_pos_local, insertLoop,
      target_var);
  if (!ret)
    return false;

  //  display
  display_scheme(codegen, scheme_1_enabled, curr_op);

  ///////////////////////////////////////////////////////////////////////////////////
  // 3 Create Initialization for reduction buffers
  ///////////////////////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////////////////////
  // 3.1 copy and create the for loops for init and post processing
  ///////////////////////////////////////////////////////////////////////////////////
  {
    map<void *, void *> for_init_map;
    void *init_loop_body = codegen->CreateBasicBlock();
    void *init_bb_t = nullptr;
    void *init_bb = codegen->CreateBasicBlock();
#ifdef NOT_GEN_FUNC
    void *init_bb_local = nullptr;
#endif

    // Note: build assignment from new reference to old reference
    for (size_t l = 0; l < vec_new_refs.size(); l++) {
      void *new_ref_init = codegen->CopyExp(vec_new_refs[l]);
      vec_new_refs_init.push_back(new_ref_init);
    }
    // 3.1.1 Copy pragmas from main loop to init loop
    {
      if (scheme_1_enabled == 0) {
        codegen->InsertStmt(init_bb, insertLoop);
        // the new loop which
        // contains the init, main
        // and post loops
        {
          // Build the copied for-loops (two copies: init and post)
          for (size_t t0 = 0; t0 < loop_nest.size(); t0++) {
            if (scheme_1_enabled == 0) {
              if (loop_is_parallel.count(loop_nest[t0]) > 0 ||
                  loop_is_related.count(loop_nest[t0]) > 0) {
                for_init_map[loop_nest[t0]] =
                    codegen->copy_loop_for_c_syntax(loop_nest[t0], init_bb);
              }
            } else {
              if (t0 == 0 || loop_is_reduct.count(loop_nest[t0]) <= 0) {
                for_init_map[loop_nest[t0]] =
                    codegen->copy_loop_for_c_syntax(loop_nest[t0], init_bb);
              }
            }
          }
        }

        int l = 0;
        string str_pragma = "ACCEL PARALLEL COMPLETE";
        string str_pragma0 = "ACCEL ARRAY_PARTITION OFF";
        for (auto for_it : for_init_map) {
          void *copyLoop = for_it.first;
          void *initLoop = for_it.second;

          if (l == 0) {
            codegen->AppendChild(init_bb, initLoop);
          } else {
            codegen->AppendChild(init_loop_body, initLoop);
          }

          insertMessage(codegen, initLoop, msg_map);
          init_loop_body = codegen->GetLoopBody(initLoop);
          void *sg_pragma = codegen->CreatePragma(str_pragma, init_loop_body);
          codegen->AppendChild(init_loop_body, sg_pragma);
          void *sg_pragma0 = codegen->CreatePragma(str_pragma0, init_loop_body);
          codegen->AppendChild(init_loop_body, sg_pragma0);
          void *old_iter = codegen->GetLoopIterator(copyLoop);
          void *new_iter = codegen->GetLoopIterator(initLoop);
          for (size_t ll = 0; ll < vec_new_refs_init.size(); ll++) {
            void *new_ref_init = vec_new_refs_init[ll];
            codegen->replace_variable_references_in_scope(old_iter, new_iter,
                                                          new_ref_init);
          }
#if MULTI_LEVEL
          for (size_t lv = 0; lv < vec_new_refs_ln_init.size(); lv++) {
            for (size_t ll = 0; ll < vec_new_refs_ln_init[lv].size(); ll++) {
              void *new_ref_init = vec_new_refs_ln_init[lv][ll];
              codegen->replace_variable_references_in_scope(old_iter, new_iter,
                                                            new_ref_init);
            }
          }
#endif
          l++;
        }
      }
    }

    ///////////////////////////////////////////////////////////////////////////////////
    // 3.2 Build initialization statement
    ///////////////////////////////////////////////////////////////////////////////////

    // 3.2.1 Build the loop nest for local initilization
    void *new_init_local_body = nullptr;
    {
      if (scheme_1_enabled != 0) {
        // a.1 get init insert position
        // a.2 create body
        void *sg_local_init = codegen->CreateBasicBlock();
        if (vec_new_refs_local_init.size() > 0)
          init_bb_t = codegen->CreateBasicBlock();

        // a.3 create loops
        void *for_loop_nest = nullptr;
        void *curr_loop = nullptr;
        string str_pragma;
        str_pragma = "ACCEL PARALLEL COMPLETE";
        string str_pragma0 = "ACCEL ARRAY_PARTITION OFF";
        for (size_t t = 0; t < loop_nest.size(); t++) {
          if (init_bb_t == nullptr)
            break;
          // only care parallel loops
          if (loop_is_parallel.count(loop_nest[t]) <= 0 ||
              loop_is_related.count(loop_nest[t]) <= 0) {
            continue;
          }

          void *loop_t =
              codegen->copy_loop_for_c_syntax(loop_nest[t], init_bb_t);
          void *old_iter = codegen->GetLoopIterator(loop_nest[t]);
          void *new_iter = codegen->GetLoopIterator(loop_t);
          if (curr_loop != nullptr) {
            void *bb_t = codegen->CreateBasicBlock();
            void *sg_pragma = codegen->CreatePragma(str_pragma, bb_t);
            codegen->AppendChild(bb_t, sg_pragma);
            void *sg_pragma0 = codegen->CreatePragma(str_pragma0, bb_t);
            codegen->AppendChild(bb_t, sg_pragma0);
            codegen->AppendChild(bb_t, loop_t);
            codegen->ReplaceChild(codegen->GetLoopBody(curr_loop), bb_t);
          } else {
            for_loop_nest = loop_t;
          }
          curr_loop = loop_t;
          insertMessage(codegen, loop_t, msg_map);

          for (size_t l = 0; l < vec_new_refs_local_init.size(); l++) {
            void *new_ref_init = vec_new_refs_local_init[l];
            replace_reference_by_var(codegen, old_iter, new_iter, new_ref_init);
          }
        }
        if (init_bb_t) {
          void *new_body = codegen->CreateBasicBlock();
          if (curr_loop != nullptr) {
            void *sg_pragma = codegen->CreatePragma(str_pragma, new_body);
            codegen->AppendChild(new_body, sg_pragma);
            void *sg_pragma0 = codegen->CreatePragma(str_pragma0, new_body);
            codegen->AppendChild(new_body, sg_pragma0);
            codegen->ReplaceChild(codegen->GetLoopBody(curr_loop), new_body);
            // a.4 insert into AST
            codegen->AppendChild(sg_local_init, for_loop_nest);
          } else {
            // Edit by Yuxin Jan 08, the nested {{...}} will triger crash in
            // LivenessAnalysis
            sg_local_init = new_body;
          }

          codegen->AppendChild(init_bb_t, sg_local_init);
          codegen->InsertAfterStmt(init_bb_t, insert_pos_local);
#ifdef NOT_GEN_FUNC
          init_bb_local = init_bb_t;
#endif
          new_init_local_body = new_body;
        }
      }
    }

    // init statement: a_rdc[i][j][k] = 0;          // for +=
    //                 a_rdc[i][j][k] = a[..][..];  // for reduction function
    int init_val = 0;
    if (codegen->IsMultAssignOp(curr_op) || codegen->IsDivAssignOp(curr_op))
      init_val = 1;

    for (size_t l = 0; l < vec_new_refs_init.size(); l++) {
      if (scheme_1_enabled != 0 && new_init_local_body) {
        void *init_value_exp = codegen->CreateConst(init_val);
        void *assign_stmt = codegen->CreateStmt(
            V_SgExprStatement,
            codegen->CreateExp(V_SgAssignOp,
                               codegen->CopyExp(vec_new_refs_local_init[l]),
                               codegen->CopyExp(init_value_exp)));
        codegen->AppendChild(new_init_local_body, assign_stmt);
      } else {
        void *init_expr = codegen->CreateStmt(
            V_SgAssignStatement, codegen->CopyExp(vec_new_refs_init[l]),
            codegen->CreateConst(init_val));
        codegen->AppendChild(init_loop_body, init_expr);
//        string str_pragma0 = "ACCEL ARRAY_PARTITION OFF";
//        void *sg_pragma0 = codegen->CreatePragma(str_pragma0, init_loop_body);
//        codegen->PrependChild(init_loop_body, sg_pragma0);
#if MULTI_LEVEL
        void *init_value_exp = codegen->CreateConst(init_val);
        for (int level = 0; level < reduct_level - 1; level++) {
          void *assign_stmt = codegen->CreateStmt(
              V_SgExprStatement,
              codegen->CreateExp(V_SgAssignOp, vec_new_refs_ln_init[level][l],
                                 codegen->CopyExp(init_value_exp)));
          codegen->AppendChild(init_loop_body, assign_stmt);
        }
#endif
      }
    }
#ifdef NOT_GEN_FUNC
    insert_reduc_function(codegen, "init", target_var, init_bb, insertLoop,
                          "_rdc", &all_func_call_map);
    if (init_bb)
      codegen->RemoveStmt(init_bb);
    if (scheme_1_enabled && init_bb_local) {
      insert_reduc_function(codegen, "init", target_var, init_bb_local,
                            init_bb_local, "_rdc_t", &all_func_call_map);
      codegen->RemoveStmt(init_bb_local);
    }
#endif
  }
  ///////////////////////////////////////////////////////////////////////////////////
  // 4 Build main loop with replacement
  ///////////////////////////////////////////////////////////////////////////////////
  {
    for (size_t l = 0; l < pntr_to_reduce.size(); l++) {
      void *old_pntr = pntr_to_reduce[l];
#ifdef NOT_GEN_FUNC
      void *stmt = codegen->GetEnclosingNode("Statement", old_pntr);
#endif
      void *new_ref = codegen->CopyExp(
          (scheme_1_enabled) != 0 ? vec_new_refs_local[l] : vec_new_refs[l]);
      void *new_arr_decl =
          (scheme_1_enabled) != 0 ? new_decl_local[l] : new_decl[l];
      void *array_ref = codegen->CreateVariableRef(new_arr_decl);
      string str_var_name = codegen->UnparseToString(array_ref);

      codegen->ReplaceExp(old_pntr, new_ref);
      // void *inner_loop_body = innermost_loop;
      // if (!scheme_1_enabled) {
      //   string str_pragma =
      //       "HLS dependence variable=" + str_var_name + " array inter false";
      //   void *sg_pragma = codegen->CreatePragma(str_pragma, inner_loop_body);
      //   codegen->PrependChild(inner_loop_body, sg_pragma);
      // }
#ifdef NOT_GEN_FUNC
      insert_reduc_function(codegen, "proc", target_var, stmt, stmt, "",
                            &all_func_call_map);
      codegen->RemoveStmt(stmt);
#endif
    }
  }

  //  ////////////////////////////////////////////////////////////////////////////////
  //  / 5 Build post process loop(s)
  //  ////////////////////////////////////////////////////////////////////////////////
  //  /

  //  Build post processing loop body: old_ref += new_ref;
  //    Note: build assignment for old reference
  map<void *, void *> loop_copies;
  map<void *, void *> loop_copies_t;

  assert(reduct_level >= 1);
  vector<void *> vec_postproc_bodies;
  {
    int num_post_steps = (scheme_1_enabled) != 0 ? 2 : reduct_level;
    for (int level = 0; level < num_post_steps; level++) {
      void *post_bb = codegen->CreateBasicBlock();
      vector<void *> vec_reduce_from;
      vector<void *> vec_reduce_to;

      if (scheme_1_enabled != 0) {
        if (level == 0) {
          vec_reduce_from = vec_new_refs_local;
          vec_reduce_to = vec_new_refs;
        } else {
          vec_reduce_from = vec_new_refs;
          vec_reduce_to = vec_old_refs;
        }
      } else {
        if (level == 0) {
          vec_reduce_from = vec_new_refs;
        } else {
          vec_reduce_from = vec_new_refs_ln[level - 1];
        }
        if (level == reduct_level - 1) {
          vec_reduce_to = vec_old_refs;
        } else {
          vec_reduce_to = vec_new_refs_ln[level];
        }
      }

      if (vec_reduce_from.size() != vec_reduce_to.size()) {
        return false;
      }
      if (vec_reduce_from.size() != 1) {
        return false;
      }

      //  post process statement: a[x][y] += a_rdc[i][j][k]
      {
        for (size_t l = 0; l < vec_reduce_from.size(); l++) {
          void *old_ref = codegen->CopyExp(vec_reduce_to[l]);
          void *assign_expr = nullptr;
          void *assign_expr_new = codegen->CreateStmt(
              V_SgExprStatement,
              codegen->CreateExp(V_SgAssignOp, old_ref,
                                 codegen->CopyExp(vec_reduce_from[l])));
          void *assign_expr_old = nullptr;
          if (codegen->IsPlusAssignOp(curr_op) ||
              codegen->IsMinusAssignOp(curr_op)) {
            assign_expr_old = codegen->CreateStmt(
                V_SgExprStatement,
                codegen->CreateExp(V_SgPlusAssignOp, old_ref,
                                   codegen->CopyExp(vec_reduce_from[l])));
          } else {
            // *= or /=
            assign_expr_old = codegen->CreateStmt(
                V_SgExprStatement,
                codegen->CreateExp(V_SgMultAssignOp, old_ref,
                                   codegen->CopyExp(vec_reduce_from[l])));
          }
          // Yuxin: Aug/20/2019
          // For block reduction, each partial sum just needs to be assign to
          // the reduction array directly. The size of the reduction array
          // is equal to the number of partial sum.
          if (scheme_1_enabled != 0) {
            if (level == 0) {
              assign_expr = assign_expr_new;
            } else {
              assign_expr = assign_expr_old;
            }
          } else {
            assign_expr = assign_expr_old;
          }
          codegen->AppendChild(post_bb, assign_expr);
        }
      }

      vec_postproc_bodies.push_back(post_bb);
    }
  }

  //  Note build post proc for loops
  vector<void *> vec_post_loops;
  void *post_bb_local = codegen->CreateBasicBlock();
  void *vec_post_bb = codegen->CreateBasicBlock();
  codegen->InsertAfterStmt(vec_post_bb, insertLoop);
  {
    vector<void *> vec_loop_reordered;
    {
      size_t t0;
      for (t0 = 0; t0 < vec_reduct_loops.size(); t0++) {
        vec_loop_reordered.push_back(vec_reduct_loops[t0]);
      }
      for (t0 = 0; t0 < vec_parallel_loops.size(); t0++) {
        vec_loop_reordered.push_back(vec_parallel_loops[t0]);
      }
    }

    if (scheme_1_enabled == 1) {
      //  two level of post reduction
      //  a.1 reduce from a_rdc_t -> a_rdc; only parallel loops
      //  a.2 reduce from a_rdc -> old_ref; pipeline + parallel loops
      for (int level = 0; level < 2; level++) {
        void *for_loop_nest =
            (level == 0)
                ? nullptr
                : codegen->copy_loop_for_c_syntax(loop_nest[0], vec_post_bb);
        if (level == 1) {
          loop_copies[loop_nest[0]] = for_loop_nest;
        }

        void *curr_loop = for_loop_nest;
        string str_pragma = "ACCEL PIPELINE";
        string str_pragma0 = "ACCEL ARRAY_PARTITION OFF";
        for (size_t t = level; t < loop_nest.size(); t++) {
          // only care parallel loops
          if (loop_is_parallel.count(loop_nest[t]) <= 0 ||
              loop_is_related.count(loop_nest[t]) <= 0) {
            continue;
          }
          void *loop_t = nullptr;
          if (level == 0) {
            loop_t = codegen->copy_loop_for_c_syntax(loop_nest[t]);
            loop_copies_t[loop_nest[t]] = loop_t;
          } else {
            loop_t = codegen->copy_loop_for_c_syntax(loop_nest[t], vec_post_bb);
            loop_copies[loop_nest[t]] = loop_t;
          }
          if (curr_loop != nullptr) {
            void *bb_t = codegen->CreateBasicBlock();
            void *sg_pragma = codegen->CreatePragma(str_pragma, bb_t);
            codegen->AppendChild(bb_t, sg_pragma);
            void *sg_pragma0 = codegen->CreatePragma(str_pragma0, bb_t);
            codegen->AppendChild(bb_t, sg_pragma0);
            codegen->AppendChild(bb_t, loop_t);
            codegen->ReplaceChild(codegen->GetLoopBody(curr_loop), bb_t);
          } else {
            for_loop_nest = loop_t;
          }
          curr_loop = loop_t;
          insertMessage(codegen, loop_t, msg_map);
        }
        str_pragma = "ACCEL PARALLEL COMPLETE";
        void *new_body = codegen->CreateBasicBlock();
        if (curr_loop != nullptr) {
          void *sg_pragma = codegen->CreatePragma(str_pragma, new_body);
          codegen->AppendChild(new_body, sg_pragma);
          void *sg_pragma0 = codegen->CreatePragma(str_pragma0, new_body);
          codegen->AppendChild(new_body, sg_pragma0);
        }

        // if (level == 0 &&
        //     "sdaccel" == options.get_option_key_value("-a", "impl_tool")) {
        //   for (size_t l = 0; l < new_decl.size(); l++) {
        //     string str_var_name = codegen->GetVariableName(
        //         codegen->GetVariableInitializedName(new_decl[l]));
        //     str_pragma = "HLS dependence variable=" + str_var_name +
        //                  " array inter false";
        //     void *sg_pragma = codegen->CreatePragma(str_pragma, new_body);
        //     codegen->AppendChild(new_body, sg_pragma);
        //   }
        // }
        int num_stmt = codegen->GetChildStmtNum(vec_postproc_bodies[level]);
        for (int i = 0; i < num_stmt; ++i) {
          void *stmt = codegen->GetChildStmt(vec_postproc_bodies[level], i);
          codegen->AppendChild(new_body, codegen->CopyStmt(stmt));
        }
        if (curr_loop != nullptr) {
          codegen->ReplaceChild(codegen->GetLoopBody(curr_loop), new_body);
        } else {
          for_loop_nest = new_body;  //  no loop, fake the body as the loop
        }

        if (level == 0) {
          codegen->AppendChild(codegen->GetLoopBody(outmost_loop),
                               for_loop_nest);
          post_bb_local = for_loop_nest;
        } else {
          vec_post_loops.push_back(for_loop_nest);
        }
      }
    } else {
      //  the last level reduction keeps the original loop order
      //  all the high-level reduction will use the optimized order (reduction
      //  loop outer)
      int level = 0;  //  used after the for loop
#if MULTI_LEVEL
      for (level = 0; level < reduct_level - 1; level++) {
        //  loop interchange: reduction outer and parallel inner
        void *for_loop_nest = codegen->copy_loop_for_c_syntax(
            vec_loop_reordered[level], vec_post_bb);
        loop_copies[vec_loop_reordered[level]] = for_loop_nest;
        void *curr_loop = for_loop_nest;
        string str_pragma = "ACCEL PIPELINE";
        for (size_t t = level + 1; t < vec_loop_reordered.size(); t++) {
          void *loop_t = codegen->copy_loop_for_c_syntax(vec_loop_reordered[t],
                                                         vec_post_bb);
          loop_copies[vec_loop_reordered[t]] = loop_t;
          void *bb_t = codegen->CreateBasicBlock();
          void *sg_pragma = codegen->CreatePragma(str_pragma, bb_t);
          codegen->AppendChild(bb_t, sg_pragma);
          codegen->AppendChild(bb_t, loop_t);
          codegen->ReplaceChild(codegen->GetLoopBody(curr_loop), bb_t);
          curr_loop = loop_t;
          insertMessage(codegen, loop_t, msg_map);
          str_pragma = "ACCEL PARALLEL COMPLETE";
        }
        void *new_body = codegen->CreateBasicBlock();
        void *sg_pragma = codegen->CreatePragma(str_pragma, new_body);
        codegen->AppendChild(new_body, sg_pragma);
        int num_stmt = codegen->GetChildStmtNum(vec_postproc_bodies[level]);
        for (int i = 0; i < num_stmt; ++i) {
          void *stmt = codegen->GetChildStmt(vec_postproc_bodies[level], i);
          codegen->AppendChild(new_body, codegen->CopyStmt(stmt));
        }
        codegen->ReplaceChild(codegen->GetLoopBody(curr_loop), new_body);

        vec_post_loops.push_back(for_loop_nest);
      }
#endif
      //  Last level reduction:
      {
        //  only the last reduction level is in the loop nest
        void *for_loop_nest = nullptr;
        void *curr_loop = nullptr;
        string str_pragma = "ACCEL PARALLEL COMPLETE";
        string str_pragma0 = "ACCEL ARRAY_PARTITION OFF";
        for (size_t t = 0; t < loop_nest.size(); t++) {
          void *one_loop = loop_nest[t];
          if (loop_is_parallel.count(one_loop) > 0 ||
              loop_is_related.count(one_loop) > 0) {
            void *loop_t =
                codegen->copy_loop_for_c_syntax(one_loop, vec_post_bb);
            loop_copies[one_loop] = loop_t;
            if (curr_loop == nullptr) {
              curr_loop = loop_t;
              for_loop_nest = curr_loop;
            } else {
              void *bb_t = codegen->CreateBasicBlock();
              void *sg_pragma = codegen->CreatePragma(str_pragma, bb_t);
              codegen->AppendChild(bb_t, sg_pragma);
              void *sg_pragma0 = codegen->CreatePragma(str_pragma0, bb_t);
              codegen->AppendChild(bb_t, sg_pragma0);
              codegen->AppendChild(bb_t, loop_t);
              codegen->ReplaceChild(codegen->GetLoopBody(curr_loop), bb_t);
              curr_loop = loop_t;
              insertMessage(codegen, loop_t, msg_map);
              str_pragma = "ACCEL PARALLEL COMPLETE";
            }
          }
        }

        if (curr_loop) {
          void *new_body = codegen->CreateBasicBlock();
          void *sg_pragma = codegen->CreatePragma(str_pragma, new_body);
          codegen->AppendChild(new_body, sg_pragma);
          int num_stmt = codegen->GetChildStmtNum(vec_postproc_bodies[level]);
          for (int i = 0; i < num_stmt; ++i) {
            void *stmt = codegen->GetChildStmt(vec_postproc_bodies[level], i);
            codegen->AppendChild(new_body, codegen->CopyStmt(stmt));
          }
          void *sg_pragma0 = codegen->CreatePragma(str_pragma0, new_body);
          codegen->PrependChild(new_body, sg_pragma0);
          codegen->ReplaceChild(codegen->GetLoopBody(curr_loop), new_body);
          vec_post_loops.push_back(for_loop_nest);
        }
      }
    }
  }

  for (size_t t = 0; t < vec_post_loops.size(); t++) {
    codegen->AppendChild(vec_post_bb, vec_post_loops[t]);
  }

  for (auto loop_tt : loop_copies) {
    void *old_iter = codegen->GetLoopIterator(loop_tt.first);
    void *new_iter = codegen->GetLoopIterator(loop_tt.second);
    replace_reference_by_var(codegen, old_iter, new_iter, vec_post_bb);
    insertMessage(codegen, loop_tt.second, msg_map);
  }

  for (auto loop_tt : loop_copies_t) {
    void *old_iter = codegen->GetLoopIterator(loop_tt.first);
    void *new_iter = codegen->GetLoopIterator(loop_tt.second);
    replace_reference_by_var(codegen, old_iter, new_iter, post_bb_local);
    insertMessage(codegen, loop_tt.second, msg_map);
  }

#ifdef NOT_GEN_FUNC
  if (scheme_1_enabled == 1) {
    insert_reduc_function(codegen, "post", target_var, post_bb_local,
                          post_bb_local, "_rdc_t", &all_func_call_map);
    codegen->RemoveStmt(post_bb_local);
  }
  insert_reduc_function(codegen, "post", target_var, vec_post_bb, insertLoop,
                        "_rdc", &all_func_call_map);
  codegen->RemoveStmt(vec_post_bb);

  for (auto func_bind : all_func_call_map)
    setFuncDeclUserCodeScopeId(codegen, func_bind.first, func_bind.second);
#endif

  return true;
}

#ifdef NOT_GEN_FUNC
void insert_reduc_function(CSageCodeGen *codegen, string func_type, void *var,
                           void *sg_copy, void *sg_location, string buf_type,
                           map<void *, void *> *all_func_call_map) {
  if (sg_copy == nullptr)
    return;
  string var_name = codegen->_up(var);
  string func_name = "_merlin_reduc_" + func_type + "_" + var_name + buf_type;
  codegen->get_valid_global_name(codegen->GetGlobal(sg_location), &func_name);
  void *func_decl = codegen->GetEnclosingNode("Function", sg_copy);
  vector<void *> call_param_list;
  map<void *, void *> init_map;

  vector<void *> vec_refs;
  codegen->GetNodesByType(sg_copy, "preorder", "SgVarRefExp", &vec_refs);
  void *param_list = codegen->buildFunctionParameterList();
  for (auto ref : vec_refs) {
    void *var_init = codegen->GetVariableInitializedName(ref);
    //  return 0 for read, 1 for write, 2 for read_write
    int wr_tag = codegen->array_ref_is_write_conservative(ref);
    if ((codegen->IsInScope(var_init, sg_copy) != 0) ||
        (codegen->IsGlobalInitName(var_init) != 0)) {
      continue;
    }

    if (init_map.find(var_init) == init_map.end()) {
      SgType *sg_type =
          (static_cast<SgInitializedName *>(var_init)->get_type());
      void *sg_param = codegen->CreateVariableRef(var_init);
      if ((wr_tag != 0) && (isSgPointerType(sg_type) == nullptr) &&
          (isSgArrayType(sg_type) == nullptr)) {
        sg_type = static_cast<SgType *>(codegen->CreatePointerType(sg_type));
        sg_param = codegen->CreateExp(V_SgAddressOfOp, sg_param);
      }
      void *sg_arg = codegen->CreateVariable(sg_type, codegen->_up(var_init));
      codegen->appendArg(param_list, sg_arg);
      call_param_list.push_back(sg_param);
      init_map[var_init] = sg_arg;
    }
  }

  void *init_func =
      codegen->CreateFuncDecl("void", func_name, param_list,
                              codegen->GetScope(func_decl), true, nullptr);
  void *insert_pos = func_decl;
  while (codegen->IsPragmaDecl(codegen->GetPreviousStmt(insert_pos)) != 0) {
    insert_pos = codegen->GetPreviousStmt(insert_pos);
  }
  codegen->InsertStmt(init_func, insert_pos);
  void *sg_func_body = codegen->GetFuncBody(init_func);
  if (codegen->IsBasicBlock(sg_copy) != 0) {
    int num_stmt = codegen->GetChildStmtNum(sg_copy);
    for (int i = 0; i < num_stmt; ++i) {
      void *stmt = codegen->GetChildStmt(sg_copy, i);
      codegen->AppendChild(sg_func_body, codegen->CopyStmt(stmt));
    }
  } else {
    codegen->AppendChild(sg_func_body, codegen->CopyStmt(sg_copy));
  }

  //  Yuxin Oct 30: inline the computation function
  string pragma_str = "HLS INLINE";
  void *new_pragma = codegen->CreatePragma(pragma_str, sg_func_body);
  codegen->PrependChild_v1(new_pragma, sg_func_body);

  vector<void *> vec_refs_n;
  codegen->GetNodesByType(sg_func_body, "preorder", "SgVarRefExp", &vec_refs_n);
  for (auto ref : vec_refs_n) {
    void *var_init = codegen->GetVariableInitializedName(ref);
    int wr_tag = codegen->array_ref_is_write_conservative(ref);
    void *new_init = nullptr;
    if (codegen->IsInScope(var_init, sg_func_body) != 0) {
      continue;
    }

    SgType *sg_type = (static_cast<SgInitializedName *>(var_init)->get_type());
    if (init_map.find(var_init) != init_map.end()) {
      new_init = init_map[var_init];
    }
    if (new_init != nullptr) {
      void *new_ref = codegen->CreateVariableRef(new_init);
      if ((wr_tag != 0) && (isSgPointerType(sg_type) == nullptr) &&
          (isSgArrayType(sg_type) == nullptr)) {
        new_ref = codegen->CreateExp(V_SgPointerDerefExp, new_ref);
      }
      codegen->ReplaceExp(ref, new_ref);
    } else {
      cout << "Didnt find the var\n";
    }
  }
  void *parameters = codegen->CreateExpList(call_param_list);
  void *init_call = codegen->CreateFuncCallStmt(init_func, parameters);
  (*all_func_call_map)[init_func] = init_call;
  if (func_type == "init" || func_type == "proc") {
    codegen->InsertStmt(init_call, sg_location);
  } else if (func_type == "post") {
    codegen->InsertAfterStmt(init_call, sg_location);
  }
}
#endif

void parsing_reduction_pragma(CSageCodeGen *codegen, CMirNode *bNode,
                              vector<void *> *reduc_vars, string *scheme_str) {
  //  Limitation: support one reduction
  //  variable in a FGPIP
  set<void *> all_vars;
  vector<CAnalPragma> vec_pragma = bNode->get_all_pragmas();
  for (auto ana_pragma : vec_pragma) {
    if (ana_pragma.is_reduction()) {
      void *decl = ana_pragma.get_ref();
      string var_str = ana_pragma.get_attribute(CMOST_variable);
      *scheme_str = ana_pragma.get_attribute(CMOST_scheme);
      void *target_var = nullptr;
      target_var = codegen->getInitializedNameFromName(codegen->GetScope(decl),
                                                       var_str, true);
      if ((target_var != nullptr) && all_vars.count(target_var) <= 0) {
        all_vars.insert(target_var);
        reduc_vars->push_back(target_var);
      }
    }
  }
}

void replace_reference_by_var(CSageCodeGen *codegen, void *old_init,
                              void *new_init, void *sg_scope) {
  vector<void *> vec_refs1;
  codegen->GetNodesByType(sg_scope, "preorder", "SgVarRefExp", &vec_refs1);
  for (auto ref : vec_refs1) {
    void *sg_init = codegen->GetVariableInitializedName(ref);
    if (sg_init == old_init) {
      void *new_ref = codegen->CreateVariableRef(new_init);
      codegen->ReplaceExp(ref, new_ref);
    }
  }
}

void check_reduction_op_var(CSageCodeGen *codegen, void *sg_scope,
                            void *target_var, vector<void *> *op_list,
                            vector<void *> *pntr_to_reduce) {
  void *forLoop = codegen->GetEnclosingNode("ForLoop", sg_scope);
  string var_str = codegen->_up(target_var);
  string str_label = codegen->get_internal_loop_label(forLoop);
  string loop_info = "\'" + str_label + "\' " +
                     getUserCodeFileLocation(codegen, forLoop, true);
  void *variable_decl = codegen->GetVariableDecl(target_var);
  if (codegen->IsInScope(variable_decl, sg_scope)) {
    dump_critical_message(REDUC_WARNING_11(var_str, loop_info));
    return;
  }
  int ref_num = get_ref_num(codegen, forLoop, target_var);
  if (ref_num > 1) {
#if _DEBUG_REDUC
    cout << "[MARS-REDUCTION-MARN] Multiple accesses to array '"
         << codegen->_up(target_var) << "'. Reduction optimization quit."
         << endl;
#endif
    dump_critical_message(REDUC_WARNING_6(var_str, loop_info));
    return;
  }

  vector<void *> op_list_tmp;
  vector<void *> vec_ops_plus;
  codegen->GetNodesByType_int(sg_scope, "preorder", V_SgExpression,
                              &vec_ops_plus, true);
  //  remove binary op in for loop increment
  {
    for (size_t i = 0; i < vec_ops_plus.size(); i++) {
      void *curr_op = vec_ops_plus[i];
      if (codegen->IsBinaryAssign(static_cast<SgBinaryOp *>(curr_op)) == 0)
        continue;
      void *curr_loop = codegen->GetEnclosingNode("ForLoop", curr_op);
      void *loop_body = codegen->GetLoopBody(curr_loop);
      if (codegen->IsInScope(curr_op, loop_body) == 0) {
        cout << "Binary assign operation " << codegen->_up(curr_op)
             << " is found in for init/test/increment expression.\n";
        continue;
      }
      cout << "push: " << codegen->_p(curr_op) << endl;
      op_list_tmp.push_back(curr_op);
    }
  }

  //  2. Get arrays to reduce
  for (auto curr_op : op_list_tmp) {
    void *var_ref = nullptr;
    void *arr_var = nullptr;
    void *arr_pntr_ref = codegen->GetExpLeftOperand(curr_op);

    if (codegen->IsVarRefExp(arr_pntr_ref) != 0) {
      var_ref = arr_pntr_ref;
    } else if (codegen->IsPntrArrRefExp(arr_pntr_ref) != 0) {
      codegen->get_var_from_pntr_ref(arr_pntr_ref, &var_ref);
    }

    if (var_ref == nullptr) {
      continue;
    }
    arr_var = codegen->GetVariableInitializedName(var_ref);

    // Yuxin: Oct 08 2019
    // Currently only support +=, -=, *=, /=
    if (arr_var == target_var) {
      if (codegen->IsPlusAssignOp(curr_op) ||
          codegen->IsMinusAssignOp(curr_op) ||
          codegen->IsMultAssignOp(curr_op) || codegen->IsDivAssignOp(curr_op)) {
        pntr_to_reduce->push_back(arr_pntr_ref);
        op_list->push_back(curr_op);
      } else {
        cout << "[MARS-REDUCTION-MARN] Unsupported reduction operation\n";
        string op_str = codegen->_up(curr_op);
        dump_critical_message(REDUC_WARNING_9(var_str, loop_info, op_str));
      }
    }
  }
}

void reduction_postprocessing(CSageCodeGen *codegen, void *pTopFunc,
                              CMarsIr *mars_ir) {
  //  Step1: remove_all_reduction_pragma
  Rose_STL_Container<SgNode *> nodeList = NodeQuery::querySubTree(
      static_cast<SgProject *>(pTopFunc), V_SgPragmaDeclaration);

  Rose_STL_Container<SgNode *>::iterator nodeListIterator = nodeList.begin();
  for (nodeListIterator = nodeList.begin(); nodeListIterator != nodeList.end();
       ++nodeListIterator) {
    SgPragmaDeclaration *decl = isSgPragmaDeclaration(*nodeListIterator);
    ROSE_ASSERT(decl);
    CAnalPragma ana_pragma(codegen);
    bool errorOut;
    ana_pragma.PragmasFrontendProcessing(decl, &errorOut);
    if (ana_pragma.is_reduction()) {
      codegen->RemoveStmt(decl);
    }
  }

  //  Step2: remove all empty aggregate initializer
  for (auto it : mars_ir->ir_list) {
    CMirNode *fNode = it.get_top_node();
    codegen->clean_empty_aggregate_initializer(fNode->ref);
  }
}

bool anal_reduction(CSageCodeGen *codegen, CMarsIr *mars_ir, CMirNode *bNode,
                    void *arr_pntr_ref, void *curr_op, void *target_var,
                    string scheme_str, int *scheme_1_enabled, int *lift_level,
                    vector<void *> *vec_reduct_loops,
                    vector<void *> *vec_parallel_loops,
                    map<void *, int> *loop_is_related,
                    vector<void *> *loop_nest) {
  cout << "Start reduction analysis.\n";

  void *innermost_loop = codegen->GetEnclosingNode("ForLoop", arr_pntr_ref);
  void *opt_loop = codegen->GetEnclosingNode("ForLoop", bNode->ref);
  auto parent_node = bNode->get_parent();
  if (parent_node != nullptr && parent_node->has_pipeline())
    opt_loop = codegen->GetEnclosingNode("ForLoop", opt_loop);

  map<void *, int> loop_is_reduct;
  map<void *, int> loop_is_parallel;
  //  ////////////////////////////////////////  /
  //  Reduction analysis
  //  1. Get Basic Loop Information
  //  ////////////////////////////////////////  /

  void *target_decl = codegen->GetVariableDecl(target_var);
  CMarsAccess ma(arr_pntr_ref, codegen, nullptr);
  vector<CMarsExpression> indexes = ma.GetIndexes();

  void *curr_loop = innermost_loop;
  {
    while (curr_loop != nullptr) {
      void *curr_body = codegen->GetLoopBody(curr_loop);
      if (codegen->IsInScope(target_decl, curr_loop) != 0) {
        break;
      }
      CMirNode *curr_node =
          mars_ir->get_node(static_cast<SgBasicBlock *>(curr_body));
      if (!curr_node->is_fine_grain ||
          (!curr_node->has_parallel() && !curr_node->has_pipeline())) {
        break;
      }
      void *iter = codegen->GetLoopIterator(curr_loop);
      if (iter == nullptr) {
        reportUnCanonicalLoop(codegen, curr_loop);
        return false;
      }

      //  Yuxin: 07/24/2019
      //  Check later
      //      int64_t dim_size = 0;
      //      int ret = codegen->get_loop_trip_count_simple(curr_loop,
      //      dim_size); if (!ret) {
      //        cout << "Cannot get the range of the loop. Quit reduction.\n";
      //        //  FIXME: report cannot get trip count
      //        return false;
      //      }

      // For each loop, check if the iterator is in the references
      int is_related = 0;
      for (size_t t = 0; t < indexes.size(); t++) {
        map<void *, CMarsExpression> *m_coeff_1 = indexes[t].get_coeff();
        if (m_coeff_1->find(curr_loop) != m_coeff_1->end()) {
          is_related = 1;
          break;
        }
        for (auto it : *m_coeff_1) {
          cout << "[print] "
               << "m_coeff: " << codegen->_p(it.first) << endl;
          if (codegen->IsIncludeVar(it.first, iter)) {
            is_related = 1;
            break;
          }
        }
      }
      if (is_related != 0) {
        (*loop_is_related)[curr_loop] = 1;
      }
      if (curr_node->has_parallel()) {
        loop_is_parallel[curr_loop] = 1;
        vec_parallel_loops->push_back(curr_loop);
        cout << "[parallel loop]: " << codegen->_p(curr_loop) << endl;
      }
      if (!is_related || !curr_node->has_parallel()) {
        loop_is_reduct[curr_loop] = 1;
        vec_reduct_loops->push_back(curr_loop);
        cout << "[reduc loop] " << codegen->_p(curr_loop) << endl;
      }

      loop_nest->push_back(curr_loop);
      cout << codegen->_p(curr_loop) << endl;
      if (curr_loop == opt_loop)
        break;
      curr_loop = codegen->GetEnclosingNode("ForLoop", curr_loop);
    }
    if (vec_parallel_loops->empty()) {
      reportWarnBenefit(codegen, target_var, opt_loop);
      return false;
    }
    curr_loop = codegen->GetEnclosingNode("ForLoop", curr_loop);
  }

  if (loop_is_related->size() == loop_nest->size()) {
    cout << "[MARS-REDUCTION-MSG] Reduction is not required." << endl;
    return false;
  }
  cout << "Need reduction.\n";

  //  from 0 to N is the loops from outside to inner
  std::reverse(vec_reduct_loops->begin(), vec_reduct_loops->end());
  std::reverse(vec_parallel_loops->begin(), vec_parallel_loops->end());
  std::reverse(loop_nest->begin(), loop_nest->end());

  //  ////////////////////////////////////////  /
  //  2. Check reduction lifting level
  //  ////////////////////////////////////////  /
  {
    while (curr_loop != nullptr) {
      if (codegen->IsInScope(target_decl, curr_loop) != 0) {
        break;
      }
      void *iter = codegen->GetLoopIterator(curr_loop);
      if (iter == nullptr) {
        reportUnCanonicalLoop(codegen, curr_loop);
        return false;
      }
      //  for each loop, check if the iterator is in the references
      int is_related = 0;
      for (size_t t = 0; t < indexes.size(); t++) {
        map<void *, CMarsExpression> *m_coeff_1 = indexes[t].get_coeff();
        if (m_coeff_1->find(curr_loop) != m_coeff_1->end()) {
          is_related = 1;
          break;
        }
        for (auto it : *m_coeff_1) {
          //         cout << "[print] "
          //              << "m_coeff: " << codegen->_p(it.first) << endl;
          if (codegen->IsIncludeVar(it.first, iter)) {
            is_related = 1;
            break;
          }
        }
      }
      if (is_related != 0) {
        break;
      }

      int count = 0;
      vector<void *> vec_refs;
      codegen->GetNodesByType_int(curr_loop, "preorder", V_SgVarRefExp,
                                  &vec_refs);
      for (auto ref : vec_refs) {
        void *arr_init = codegen->GetVariableInitializedName(ref);
        if (arr_init == target_var) {
          count++;
        }
      }
      if (count > 1) {
        break;
      }

      cout << "[lift loop] " << codegen->_p(curr_loop) << endl;
      (*lift_level)++;
      curr_loop = codegen->GetEnclosingNode("ForLoop", curr_loop);
    }
  }

  //  ////////////////////////////////////////  /
  //  2. Choose reduction scheme
  //  ////////////////////////////////////////  /
  int reduct_level = loop_is_reduct.size();
  cout << "REDUCT LEVEL: " << reduct_level << endl;
  cout << "LIFT LEVEL: " << *lift_level << endl;
  // Yuxin: 23 Sep 2019
  // Use cyclic reduction by default
  if (scheme_str == "auto" || scheme_str.empty()) {
#if 0
    *scheme_1_enabled = static_cast<int>(reduct_level > 1 &&
                                         loop_is_reduct.find((*loop_nest)[0]) !=
                                             loop_is_reduct.end());
#endif
    *scheme_1_enabled = 0;
    //  FIXME: Only apply for interleaved reduction, Jan 07 2016 Yuxin
    //  In interleaved reduction, when lifting level<1, reduction will stop
    if ((*scheme_1_enabled == 0) && *lift_level <= 0) {
      //     reportNoBenefit(codegen, curr_op);
      cout << "No benefit from reduction. Quit reduction.\n";
      //  return false;
    }
  } else if (scheme_str == "cyclic") {
    *scheme_1_enabled = 0;
    if (*lift_level <= 0) {
      reportWarnBenefit(codegen, target_var, innermost_loop);
      cout << "Maybe no benefit from reduction. Quit reduction.\n";
    }
  } else if (scheme_str == "block") {
    *scheme_1_enabled = 1;
  }
  return true;
}

void reportWarnBenefit(CSageCodeGen *codegen, void *target_var, void *sg_loop) {
  string str_var = codegen->UnparseToString(target_var);
  string str_label = codegen->get_internal_loop_label(sg_loop);
  string loop_info =
      "'" + str_label + "' " + getUserCodeFileLocation(codegen, sg_loop);
  dump_critical_message(REDUC_WARNING_7(str_var, loop_info));
}

void reportVariableTripCount(CSageCodeGen *codegen, void *target_var,
                             void *sg_loop) {
  string str_var = codegen->UnparseToString(target_var);
  string str_label = codegen->get_internal_loop_label(sg_loop);
  string loop_info =
      "'" + str_label + "' " + getUserCodeFileLocation(codegen, sg_loop);
  dump_critical_message(REDUC_WARNING_8(str_var, loop_info));
}

void display_scheme(CSageCodeGen *codegen, int scheme_1_enabled,
                    void *curr_op) {
  string str_scheme = "cyclic-based";
  if (scheme_1_enabled != 0) {
    str_scheme = "block-based";
  }

  string str_ref = codegen->UnparseToString(curr_op);
  if (str_ref.length() > 20) {
    str_ref = str_ref.substr(0, 20) + " ...";
  }
  string msg = "Applying " + str_scheme + " reduction on operation '" +
               str_ref + "' " +
               getUserCodeFileLocation(codegen, curr_op, true) + "\n";
  cout << msg << endl;
  string op_info =
      "'" + str_ref + "' " + getUserCodeFileLocation(codegen, curr_op, true);
  dump_critical_message(REDUC_INFO_1(str_scheme, op_info));
}

bool create_reduction_buf_and_refs(
    CSageCodeGen *codegen, int scheme_1_enabled,
    const vector<void *> &pntr_to_reduce, vector<void *> *new_decl,
    vector<vector<void *>> *new_decl_ln, vector<void *> *new_decl_local,
    vector<void *> *vec_new_refs_local, vector<void *> *vec_new_refs_local_init,
    vector<void *> *vec_old_refs, vector<void *> *vec_new_refs,
    vector<vector<void *>> *vec_new_refs_ln,
    vector<vector<void *>> *vec_new_refs_ln_init,
    const vector<void *> &vec_reduct_loops,
    const vector<void *> &vec_parallel_loops, const vector<void *> &loop_nest,
    const std::map<void *, int> &loop_is_related,
    map<void *, int> *loop_is_reduct, map<void *, int> *loop_is_parallel,
    void **insert_pos_local, void *insertLoop, void *target_var) {
  void *outmost_loop = loop_nest[0];
  void *func_decl = codegen->GetEnclosingNode("Function", outmost_loop);
  void *insertLoop_t = loop_nest[0];
  // void *insertLoop_t = vec_parallel_loops[0];

  for (auto loop : vec_reduct_loops) {
    (*loop_is_reduct)[loop] = 1;
  }
  for (auto loop : vec_parallel_loops) {
    (*loop_is_parallel)[loop] = 1;
  }
  //  Caching loop trip count
  //  from bnode (pipeline loop) to cnode (innermost loop)
  vector<size_t> loop_dim_sizes;
  {
    size_t t0;
    for (t0 = 0; t0 < loop_nest.size(); t0++) {
      int64_t dim_size = 0;
      int64_t range_ub = 0;
      int ret =
          codegen->get_loop_trip_count(loop_nest[t0], &dim_size, &range_ub);
      //  Yuxin: 07/24/2019
      //  Allow the outmost loop to be variable
      if ((ret == 0) && (range_ub == 0)) {
        cout << "Loop: " << t0 << ": " << codegen->_p(loop_nest[t0]) << endl;
        cout << "Cannot get the range of the loop. Quit reduction.\n";
        loop_dim_sizes.push_back(-1);
      } else {
        cout << "trip_count: " << dim_size << ", " << range_ub << endl;
        loop_dim_sizes.push_back(range_ub);
      }
    }
  }
  //  ////////////////////////////////////////////////////////////////////////////////
  //  / 2.1 Create the declaration for the reduction buffers
  //  ////////////////////////////////////////////////////////////////////////////////
  //  /

  //  Note: insert new buffer declaration
  //  FIXME: to support multiple reduction variables
  for (size_t l = 0; l < pntr_to_reduce.size(); l++) {
    void *arr_type = nullptr;
    void *sg_type = codegen->GetTypebyVar(target_var);
    void *arr_base_type = codegen->GetBaseType(sg_type, false);
    size_t t0;
    vector<size_t> dim_sizes;
    for (t0 = 0; t0 < loop_nest.size(); t0++) {
      if (loop_is_parallel->count(loop_nest[t0]) > 0 ||
          loop_is_related.count(loop_nest[t0]) > 0) {
        if (loop_dim_sizes[t0] == -1) {
          reportVariableTripCount(codegen, target_var, loop_nest[t0]);
          return false;
        }
        dim_sizes.push_back(loop_dim_sizes[t0]);
      }
    }
    arr_type = codegen->CreateArrayType(arr_base_type, dim_sizes);

    if (scheme_1_enabled == 1) {
      size_t t0;
      vector<size_t> dim_sizes;
      for (t0 = 0; t0 < loop_nest.size(); t0++) {
        if (t0 == 0 || loop_is_reduct->count(loop_nest[t0]) <= 0) {
          if (loop_dim_sizes[t0] == -1) {
            reportVariableTripCount(codegen, target_var, loop_nest[t0]);
            return false;
          }
          dim_sizes.push_back(loop_dim_sizes[t0]);
        }
      }
      arr_type = codegen->CreateArrayType(arr_base_type, dim_sizes);
    }

    void *array_type_local;  //  scheme 1
    {
      //  local buffer only has parallel dimensions
      size_t t0;
      vector<size_t> dim_sizes;
      for (t0 = 0; t0 < loop_nest.size(); t0++) {
        if (loop_is_parallel->count(loop_nest[t0]) > 0 &&
            loop_is_related.count(loop_nest[t0]) > 0) {
          if (loop_dim_sizes[t0] == -1) {
            reportVariableTripCount(codegen, target_var, loop_nest[t0]);
            return false;
          }
          dim_sizes.push_back(loop_dim_sizes[t0]);
        }
      }
      if (dim_sizes.empty()) {
        array_type_local = arr_base_type;
      } else {
        array_type_local = codegen->CreateArrayType(arr_base_type, dim_sizes);
      }
    }

#if MULTI_LEVEL
    vector<void *> array_type_ln;  //  for each level (scheme 0)
    array_type_ln.resize(reduct_level - 1);
    for (int level = 0; level < reduct_level - 1; level++) {
      void *array_type = arr_base_type;
      size_t t0;
      vector<size_t> dim_sizes;
      int skip = level + 1;
      for (t0 = 0; t0 < loop_nest.size(); t0++) {
        if (loop_dim_sizes[t0] == -1) {
          reportVariableTripCount(codegen, target_var, loop_nest[t0]);
          return false;
        }
        int dim_size = loop_dim_sizes[t0];

        if (loop_is_reduct->find(loop_nest[t0]) != loop_is_reduct->end()) {
          if (skip == 0) {
            dim_sizes.push_back(dim_size);
          } else {
            skip--;
          }
        }
      }
      for (t0 = 0; t0 < loop_nest.size(); t0++) {
        if (loop_dim_sizes[t0] == -1) {
          reportVariableTripCount(codegen, target_var, loop_nest[t0]);
          return false;
        }
        int dim_size = loop_dim_sizes[t0];

        if (loop_is_parallel->find(loop_nest[t0]) != loop_is_parallel->end()) {
          dim_sizes.push_back(dim_size);
        }
      }
      array_type = codegen->CreateArrayType(array_type, dim_sizes);
      array_type_ln[level] = array_type;
    }
#endif

    //  Note: create new reduction buffer
    string bufname_str = "_" + codegen->UnparseToString(target_var) + "_rdc";
    codegen->get_valid_local_name(func_decl, &bufname_str);
    void *indexScope = codegen->GetScope(insertLoop);
    void *sg_location = insertLoop;
    void *sg_prev = codegen->GetParent(insertLoop);
    if (codegen->IsLabelStatement(sg_prev) != 0) {
      sg_location = sg_prev;
    }

    void *new_buf =
        codegen->CreateVariableDecl(arr_type, bufname_str, nullptr, indexScope);
    codegen->InsertStmt(new_buf, sg_location);
    new_decl->push_back(new_buf);

    if (scheme_1_enabled == 1) {
      void *sg_body = codegen->GetLoopBody(insertLoop_t);
      string bufname_local = bufname_str + "_t";
      void *new_buf =
          codegen->CreateVariableDecl(array_type_local, bufname_local, nullptr,
                                      codegen->GetScope(insertLoop_t));
      if (*insert_pos_local != nullptr) {
        codegen->InsertAfterStmt(new_buf, *insert_pos_local);
        *insert_pos_local = new_buf;
      } else {
        codegen->PrependChild(sg_body, new_buf, true);
        // codegen->InsertStmt(new_buf, insertLoop_t);
        *insert_pos_local = new_buf;
      }
      new_decl_local->push_back(new_buf);
    } else {
#if MULTI_LEVEL
      for (int level = 0; level < reduct_level - 1; level++) {
        string bufname_ln = bufname_str + "_L" + my_itoa(level + 1);
        void *new_buf = codegen->CreateVariableDecl(
            array_type_ln[level], bufname_ln, nullptr, indexScope);
        codegen->InsertStmt(new_buf, sg_location);
        (*new_decl_ln)[level].push_back(new_buf);
      }
#endif
    }
  }

  //  ////////////////////////////////////////////////////////////////////////////////
  //  / 2.2 Create the references for the reduction buffers
  //     And replace the reference in the main loop
  //  ////////////////////////////////////////////////////////////////////////////////
  //  /
  //  Note: replace old reference with new reference
  //  Main Body: a[x][y] -> a_rdc[i][j][k]
  for (size_t l = 0; l < pntr_to_reduce.size(); l++) {
    void *arr_ref_c = codegen->rebuild_pntr_ref(pntr_to_reduce[l]);
    //  Revised by Yuxin, Jan 23 2016, arr_ref_c will be modified during code
    //  generation
    //  Should keep a copy of it, instead of the original node
    vec_old_refs->push_back(codegen->CopyExp(arr_ref_c));
    void *new_ref = codegen->CreateVariableRef((*new_decl)[l]);
    vector<void *> vec_index;
    for (size_t t0 = 0; t0 < loop_nest.size(); t0++) {
      //  for reduction buffer:
      //  1. Only skip loops if scheme_1 is applied
      //  2. Only skip internal reduction loops
      if (scheme_1_enabled == 0) {
        if (loop_is_parallel->count(loop_nest[t0]) > 0 ||
            loop_is_related.count(loop_nest[t0]) > 0) {
          vec_index.push_back(codegen->CreateVariableRef(
              codegen->GetLoopIterator(loop_nest[t0])));
        }
      } else {
        if (t0 == 0 || loop_is_reduct->count(loop_nest[t0]) <= 0) {
          vec_index.push_back(codegen->CreateVariableRef(
              codegen->GetLoopIterator(loop_nest[t0])));
        }
      }
    }
    new_ref = codegen->CreateArrayRef(new_ref, vec_index);
    vec_new_refs->push_back(new_ref);
    if (scheme_1_enabled == 1) {
      vector<void *> index;
      for (size_t t0 = 0; t0 < loop_nest.size(); t0++) {
        if (loop_is_parallel->count(loop_nest[t0]) > 0 &&
            loop_is_related.count(loop_nest[t0]) > 0) {
          index.push_back(codegen->CreateVariableRef(
              codegen->GetLoopIterator(loop_nest[t0])));
        }
      }

      void *array_ref = codegen->CreateVariableRef((*new_decl_local)[l]);
      void *new_ref_local = codegen->CreateArrayRef(array_ref, index);
      vec_new_refs_local->push_back(new_ref_local);
      vec_new_refs_local_init->push_back(codegen->CopyExp(new_ref_local));
    } else {
      //  build the partial sum references
      //  for level, skip the outer level+1 reduction loops
#if MULTI_LEVEL
      for (int level = 0; level < reduct_level - 1; level++) {
        vector<void *> index;
        size_t t0;
        for (t0 = level + 1; t0 < vec_reduct_loops.size(); t0++) {
          index.push_back(codegen->CreateVariableRef(
              codegen->GetLoopIterator(vec_reduct_loops[t0])));
        }
        for (t0 = 0; t0 < vec_parallel_loops.size(); t0++) {
          index.push_back(codegen->CreateVariableRef(
              codegen->GetLoopIterator(vec_parallel_loops[t0])));
        }
        void *arr_ref = codegen->CreateVariableRef((*new_decl_ln)[level]);
        void *new_ref = codegen->CreateArrayRef(arr_ref, index);
        (*vec_new_refs_ln)[level].push_back(new_ref);
        (*vec_new_refs_ln_init)[level].push_back(codegen->CopyExp(new_ref));
      }
#endif
    }
  }
  return true;
}
