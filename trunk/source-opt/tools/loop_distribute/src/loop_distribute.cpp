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


#include "loop_distribute.h"
#include "mars_opt.h"
int loop_distribute_top(CSageCodeGen *codegen, void *pTopFunc,
                        const CInputOptions &options) {
  LoopDistribute instance(codegen, pTopFunc, options);
  instance.run();
  return 0;
}

LoopDistribute::LoopDistribute(CSageCodeGen *codegen, void *pTopFunc,
                               const CInputOptions &options)
    : mCodegen(codegen) {
  mMars_ir.build_mars_ir(codegen, pTopFunc, false, true);
}

bool LoopDistribute::run() {
  //  1. move all variable declarations out of loops
  lift_local_variable_out_of_loop();
  //  2. build new mars ir
  mMars_ir.clear();
  mMars_ir.build_mars_ir(mCodegen, mCodegen.GetProject(), false, true);
  bool Changed = false;
  size_t i = 0;
  while (i < mMars_ir.size()) {
    CMarsNode *node = mMars_ir.get_node(i);
    void *kernel_top = node->get_user_kernel();
    vector<int> order_res;
    mMars_ir.get_order_of_sub_nodes(kernel_top, vector<int>(), &order_res);
    vector<void *> top_loops;
    for (size_t j = 0; j != order_res.size(); ++j) {
      void *loop =
          mMars_ir.get_loop_for_order(kernel_top, vector<int>(1, order_res[j]));
      if (loop != nullptr) {
        top_loops.push_back(loop);
      }
    }
    Changed |= distribute(top_loops);
    pair<int, int> range = mMars_ir.get_node_range(kernel_top);
    for (int j = range.first; j != range.second; ++j) {
      CMarsNode *node = mMars_ir.get_node(j);
      if (node->has_loops()) {
        continue;
      }
      if (node->is_dead()) {
        continue;
      }
      node->code_generation();
      node->remove_all_statements();
    }
    i = range.second;
  }
  return Changed;
}

int loop_flatten_top(CSageCodeGen *codegen, void *pTopFunc,
                     const CInputOptions &options) {
  LoopFlatten instance(codegen, pTopFunc, options);
  instance.run();
  return 0;
}

LoopFlatten::LoopFlatten(CSageCodeGen *codegen, void *pTopFunc,
                         const CInputOptions &options)
    : mCodegen(codegen), mOptions(options) {
  mMars_ir.build_mars_ir(codegen, pTopFunc, false, true);
}

bool LoopFlatten::run() {
  bool Changed = false;
  size_t i = 0;
  while (i < mMars_ir.size()) {
    CMarsNode *node = mMars_ir.get_node(i);
    void *kernel_top = node->get_user_kernel();
    vector<void *> all_loops;
    mCodegen.GetNodesByType(kernel_top, "preorder", "SgForStatement",
                            &all_loops);
    set<void *> unrolled_loops;
    for (auto &loop : all_loops) {
      CMarsLoop *loop_info = mMars_ir.get_loop_info(loop);
      if (loop_info->is_complete_unroll() != 0) {
        unrolled_loops.insert(loop);
      }
    }
    vector<int> order_res;
    mMars_ir.get_order_of_sub_nodes(kernel_top, vector<int>(), &order_res);
    vector<void *> top_loops;
    for (size_t j = 0; j != order_res.size(); ++j) {
      void *loop =
          mMars_ir.get_loop_for_order(kernel_top, vector<int>(1, order_res[j]));
      if (loop != nullptr) {
        top_loops.push_back(loop);
      }
    }
    Changed |= flatten(top_loops, unrolled_loops);
    pair<int, int> range = mMars_ir.get_node_range(kernel_top);
    i = range.second;
  }
  return Changed;
}

bool LoopDistribute::distribute(const vector<void *> &top_loops) {
  for (size_t i = 0; i != top_loops.size(); ++i) {
    void *top_loop = top_loops[i];
    vector<CMarsNode *> sub_nodes =
        mMars_ir.get_nodes_with_common_loop(top_loop);
    for (auto sub_node : sub_nodes) {
      if (sub_node->is_dead()) {
        continue;
      }
      sub_node->code_generation(top_loop);
    }
    mCodegen.RemoveStmt(top_loop);
  }
  return true;
}

bool LoopFlatten::flatten(const vector<void *> &top_loops,
                          const set<void *> &unrolled_loops) {
  bool Changed = false;
  for (size_t i = 0; i != top_loops.size(); ++i) {
    void *top_loop = top_loops[i];
    Changed |= loop_flatten_nested_loops(top_loop, unrolled_loops,
                                         /*no_final_reset*/
                                         true);
  }
  return Changed;
}

bool LoopDistribute::lift_local_variable_out_of_loop(void *sg_loop) {
  vector<void *> var_decls;
  mCodegen.GetNodesByType(sg_loop, "preorder", "SgVariableDeclaration",
                          &var_decls);
  if (var_decls.empty()) {
    return false;
  }
  void *func_decl = mCodegen.TraceUpToFuncDecl(sg_loop);
  if (func_decl == nullptr) {
    return false;
  }
  void *func_body = mCodegen.GetFuncBody(func_decl);
  if (func_body == nullptr) {
    return false;
  }
  map<void *, void *> oldVar2NewVar;
  map<void *, void *> oldDecl2NewAssign;
  for (size_t i = 0; i != var_decls.size(); ++i) {
    void *var_decl = var_decls[i];
    if (mMars_ir.iswithininnermostloop(var_decl)) {
      continue;
    }
    vector<void *> vec_vars;
    mCodegen.GetVariableInitializedName(var_decl, &vec_vars);
    vector<void *> expr_list;
    for (size_t j = 0; j != vec_vars.size(); ++j) {
      void *var = vec_vars[j];
      void *sg_type = mCodegen.GetTypebyVar(var);
      string old_name = mCodegen.GetVariableName(var);
      mCodegen.SetVariableName(var, "old_" + old_name);
      string new_name = old_name;
      mCodegen.get_valid_local_name(func_decl, &new_name);
      void *new_var =
          mCodegen.CreateVariableDecl(sg_type, new_name, NULL, func_body);
      mCodegen.PrependChild(func_body, new_var, true);
      oldVar2NewVar[var] = new_var;
      void *var_init = mCodegen.GetInitializerOperand(var);
      if (var_init != nullptr) {
        expr_list.push_back(mCodegen.CreateExp(V_SgAssignOp, new_var,
                                               mCodegen.CopyExp(var_init)));
      }
    }
    void *expr_stmt = NULL;
    if (!expr_list.empty()) {
      void *expr = expr_list[0];
      if (expr_list.size() > 1) {
        expr = mCodegen.CreateExpList(expr_list);
      }
      expr_stmt = mCodegen.CreateStmt(V_SgExprStatement, expr);
    }
    oldDecl2NewAssign[var_decl] = expr_stmt;
  }
  vector<void *> vec_refs;
  mCodegen.GetNodesByType(sg_loop, "preorder", "SgVarRefExp", &vec_refs);
  for (size_t i = 0; i != vec_refs.size(); ++i) {
    void *ref = vec_refs[i];
    void *var = mCodegen.GetVariableInitializedName(ref);
    if (oldVar2NewVar.count(var) <= 0) {
      continue;
    }
    void *new_var = oldVar2NewVar[var];
    mCodegen.ReplaceExp(ref, mCodegen.CreateVariableRef(new_var));
  }
  for (map<void *, void *>::iterator i = oldDecl2NewAssign.begin();
       i != oldDecl2NewAssign.end(); ++i) {
    void *var_decl = i->first;
    void *new_assign = i->second;
    if (new_assign != nullptr) {
      mCodegen.ReplaceStmt(var_decl, new_assign);
    } else {
      mCodegen.RemoveStmt(var_decl);
    }
  }

  return true;
}

bool LoopDistribute::lift_local_variable_out_of_loop() {
  bool Changed = false;
  size_t i = 0;
  while (i < mMars_ir.size()) {
    CMarsNode *node = mMars_ir.get_node(i);
    void *kernel_top = node->get_user_kernel();
    vector<int> order_res;
    mMars_ir.get_order_of_sub_nodes(kernel_top, vector<int>(), &order_res);
    vector<void *> top_loops;
    for (size_t j = 0; j != order_res.size(); ++j) {
      void *loop =
          mMars_ir.get_loop_for_order(kernel_top, vector<int>(1, order_res[j]));
      if (loop != nullptr) {
        top_loops.push_back(loop);
      }
    }
    Changed |= process_all_scopes(top_loops);
    pair<int, int> range = mMars_ir.get_node_range(kernel_top);
    i = range.second;
  }
  return Changed;
}

bool LoopDistribute::process_all_scopes(const vector<void *> &loops) {
  bool Changed = false;
  for (size_t i = 0; i != loops.size(); ++i) {
    void *loop = loops[i];
    Changed |= lift_local_variable_out_of_loop(loop);
  }
  return Changed;
}

int LoopFlatten::loop_flatten_nested_loops(void *sg_loop,
                                           const set<void *> &not_touched_loops,
                                           bool no_final_reset) {
  int keep_last = static_cast<int>(
      "keep_last" == mOptions.get_option_key_value("-a", "loop_flatten_flag"));

#define PUT_ALL_INIT_INTO_LOOP_INIT 1
  vector<void *> nested_loops;
  mCodegen.get_perfectly_nested_loops(sg_loop, nested_loops);
  if (nested_loops.size() < 2) {
    return 0;
  }

  //  ZP: 20170105 support of keeping last level
  if ((keep_last != 0) && nested_loops.size() < 3) {
    return 0;
  }

  int max_depth = 0;
  void *func_decl = mCodegen.TraceUpToFuncDecl(sg_loop);

  bool found_possible_negative_trip = false;
  for (size_t i = 0; i != nested_loops.size(); ++i, ++max_depth) {
    //  ZP: 20170105 support of keeping last level
    if ((keep_last != 0) && i == nested_loops.size() - 1) {
      break;
    }
    void *next_loop = nested_loops[i + 1];
    if ((keep_last != 0) && not_touched_loops.count(next_loop) > 0) {
      break;
    }

    //  check how many levels of loop can be flatten
    //  1. have detemined loop bound
    //  2. both low and upper bound is non-negative
    //  3. step is 1
    void *curr_loop = nested_loops[i];
    string file_name = mCodegen.GetFileInfo_filename(curr_loop, 1);
    string loop_name = mCodegen.get_internal_loop_label(curr_loop);
    int line_num = mCodegen.GetFileInfo_line(curr_loop);
    if (not_touched_loops.count(curr_loop) > 0) {
      break;
    }
    MarsProgramAnalysis::CMarsVariable mv(curr_loop, &mCodegen);
    MarsProgramAnalysis::CMarsRangeExpr mr = mv.get_range();
    if (mr.get_lb_set().size() != 1 || mr.get_ub_set().size() != 1) {
      string msg = "Stopping flattening loop \'" + loop_name + "\' (" +
                   file_name + ":" + my_itoa(line_num) + ") because its " +
                   "lower or upper bound is unknown\n";

      dump_critical_message("FLATN", "WARNING", msg, 201, 1);
      break;
    }
    MarsProgramAnalysis::CMarsExpression me_lb = mr.get_lb(0);
    MarsProgramAnalysis::CMarsExpression me_ub = mr.get_ub(0);
    if (!me_lb.IsLoopInvariant(sg_loop) || !me_ub.IsLoopInvariant(sg_loop)) {
      break;
    }
    if (found_possible_negative_trip && ((me_ub >= me_lb) == 0)) {
      string msg = "Stopping flattening loop \'" + loop_name + "\' (" +
                   file_name + ":" + my_itoa(line_num) + ") because its " +
                   "loop trip count may be negative\n";
      msg += "  Hint:   specify its bound of the loop using 'assert' \n";

      dump_critical_message("FLATN", "WARNING", msg, 202, 1);

      break;
    }
    if ((me_ub >= me_lb) == 0) {
      found_possible_negative_trip = true;
    }
    int step_val;
    {
      void *for_loop = curr_loop;
      void *iv;
      void *lb;
      void *ub;
      void *step;
      void *body;
      bool inc_dir;
      bool inclusive_bound;
      int ret = mCodegen.IsCanonicalForLoop(for_loop, &iv, &lb, &ub, &step,
                                            &body, &inc_dir, &inclusive_bound);
      ret &= mCodegen.GetLoopStepValueFromExpr(step, &step_val);
      if ((step != nullptr) && (mCodegen.GetParent(step) == nullptr)) {
        mCodegen.DeleteNode(step);
      }
      if (ret == 0) {
        string msg = "Stopping flattening the non-canonical loop \'" +
                     loop_name + "\' (" + file_name + ":" + my_itoa(line_num) +
                     ")";
        dump_critical_message("FLATN", "WARNING", msg, 204, 1);
        break;
      }
    }
    if (step_val != 1) {
      string msg = "Stopping flattening loop \'" + loop_name + "\' (" +
                   file_name + ":" + my_itoa(line_num) + ") because its " +
                   "step is not one\n";
      dump_critical_message("FLATN", "WARNING", msg, 204, 1);
      break;
    }
  }

  if (max_depth < 2) {
    return 0;
  }

  void *top_loop = nested_loops[0];
  void *bottom_loop = nested_loops[max_depth - 1];
  void *post_update;
  {
    post_update = mCodegen.CreateBasicBlock();
    for (int i = max_depth - 1; i > 0; --i) {
      void *curr_loop = nested_loops[i];
      MarsProgramAnalysis::CMarsVariable mv(curr_loop, &mCodegen);
      MarsProgramAnalysis::CMarsRangeExpr mr = mv.get_range();
      MarsProgramAnalysis::CMarsExpression me_ub = mr.get_ub(0);
      MarsProgramAnalysis::CMarsExpression me_lb = mr.get_lb(0);
      void *upper_loop = nested_loops[i - 1];
      void *iter_1 = mCodegen.GetLoopIterator(upper_loop);
      void *iter_2 = mCodegen.GetLoopIterator(curr_loop);

      //  i++; if (i>UBI) { j++; i = LBI; }
      if (i == max_depth - 1) {
        void *incr_2 = mCodegen.CreateStmt(
            V_SgExprStatement,
            mCodegen.CreateExp(V_SgPlusPlusOp,
                               mCodegen.CreateVariableRef(iter_2)));
        mCodegen.AppendChild(post_update, incr_2);
      }
      void *incr_1_cond = mCodegen.CreateExp(V_SgGreaterThanOp,
                                             mCodegen.CreateVariableRef(iter_2),
                                             me_ub.get_expr_from_coeff());
      void *if_body = mCodegen.CreateBasicBlock();
      void *incr_1 = mCodegen.CreateStmt(
          V_SgExprStatement,
          mCodegen.CreateExp(V_SgPlusPlusOp,
                             mCodegen.CreateVariableRef(iter_1)));
      mCodegen.AppendChild(if_body, incr_1);

      void *reset_2 = mCodegen.CreateStmt(
          V_SgExprStatement,
          mCodegen.CreateExp(V_SgAssignOp, mCodegen.CreateVariableRef(iter_2),
                             me_lb.get_expr_from_coeff()));
      mCodegen.AppendChild(if_body, reset_2);

      void *if_stmt = mCodegen.CreateIfStmt(incr_1_cond, if_body, NULL);
      mCodegen.AppendChild(post_update, if_stmt);
    }
  }
  mCodegen.AppendChild(mCodegen.GetLoopBody(bottom_loop), post_update);
#ifndef PUT_ALL_INIT_INTO_LOOP_INIT
  vector<void *> vec_initer;
#else
  vector<void *> vec_initer_exp;
#endif
  vector<void *> vec_finalizer;
  for (int i = 0; i != max_depth; ++i) {
    void *curr_loop = nested_loops[i];  //  ZP: 20170105
    MarsProgramAnalysis::CMarsVariable mv(curr_loop, &mCodegen);
    MarsProgramAnalysis::CMarsRangeExpr mr = mv.get_range();
    MarsProgramAnalysis::CMarsExpression me_lb = mr.get_lb(0);
    MarsProgramAnalysis::CMarsExpression me_ub = mr.get_ub(0) + 1;
    void *assign = mCodegen.CreateExp(
        V_SgAssignOp,
        mCodegen.CreateVariableRef(mCodegen.GetLoopIterator(curr_loop)),
        me_lb.get_expr_from_coeff());
#ifndef PUT_ALL_INIT_INTO_LOOP_INIT
    vec_initer.push_back(CreateStmt(V_SgExprStatement, assign));
#else
    vec_initer_exp.push_back(assign);
#endif
    void *assign_2 = mCodegen.CreateExp(
        V_SgAssignOp,
        mCodegen.CreateVariableRef(mCodegen.GetLoopIterator(curr_loop)),
        me_ub.get_expr_from_coeff());
    if (!no_final_reset) {
      vec_finalizer.push_back(mCodegen.CreateStmt(V_SgExprStatement, assign_2));
    }
  }

  void *new_iter_decl;
  void *new_for_loop;
  {
    string str_new_iter =
        "_" + mCodegen.UnparseToString(mCodegen.GetLoopIterator(top_loop));
    MarsProgramAnalysis::CMarsVariable mv_top(top_loop, &mCodegen);
    MarsProgramAnalysis::CMarsRangeExpr mr_top = mv_top.get_range();
    MarsProgramAnalysis::CMarsExpression me_lb_top = mr_top.get_lb(0);
    MarsProgramAnalysis::CMarsExpression me_ub_top = mr_top.get_ub(0) + 1;
    void *start_exp = me_lb_top.get_expr_from_coeff();
    void *end_exp = me_ub_top.get_expr_from_coeff();
    start_exp = mCodegen.CreateCastExp(start_exp, "long");
    end_exp = mCodegen.CreateCastExp(end_exp, "long");
    for (int i = 1; i != max_depth; ++i) {
      void *curr_loop = nested_loops[i];
      str_new_iter +=
          "_" + mCodegen.UnparseToString(mCodegen.GetLoopIterator(curr_loop));
      MarsProgramAnalysis::CMarsVariable mv(curr_loop, &mCodegen);
      MarsProgramAnalysis::CMarsRangeExpr mr = mv.get_range();
      MarsProgramAnalysis::CMarsExpression me_lb = mr.get_lb(0);
      MarsProgramAnalysis::CMarsExpression me_ub = mr.get_ub(0);
      MarsProgramAnalysis::CMarsExpression me_length = me_ub - me_lb + 1;
      start_exp = mCodegen.CreateExp(V_SgMultiplyOp, start_exp,
                                     me_length.get_expr_from_coeff());
      end_exp = mCodegen.CreateExp(V_SgMultiplyOp, end_exp,
                                   me_length.get_expr_from_coeff());
    }
    end_exp =
        mCodegen.CreateExp(V_SgSubtractOp, end_exp, mCodegen.CreateConst(1));
    mCodegen.get_valid_local_name(func_decl, &str_new_iter);
    new_iter_decl = mCodegen.CreateVariableDecl(
        mCodegen.GetTypeByString("long"), str_new_iter, NULL,
        mCodegen.GetScope(top_loop));
    void *new_iter = mCodegen.GetVariableInitializedName(new_iter_decl);

    void *new_body = mCodegen.CopyStmt(mCodegen.GetLoopBody(bottom_loop));

#ifndef PUT_ALL_INIT_INTO_LOOP_INIT
    int64_t new_step = 1;
    new_for_loop = mCodegen.CreateStmt(V_SgForStatement, new_iter, start_exp,
                                       end_exp, new_body, &new_step);
#else
    void *new_iter_init = mCodegen.CreateExp(
        V_SgAssignOp, mCodegen.CreateVariableRef(new_iter), start_exp);
    vec_initer_exp.insert(vec_initer_exp.begin(), new_iter_init);
    void *new_init_stmt = mCodegen.CreateStmt(
        V_SgExprStatement, mCodegen.CreateExpList(vec_initer_exp));
    void *new_test_stmt = mCodegen.CreateStmt(
        V_SgExprStatement,
        mCodegen.CreateExp(V_SgLessOrEqualOp,
                           mCodegen.CreateVariableRef(new_iter), end_exp));
    void *new_incr = mCodegen.CreateExp(V_SgPlusPlusOp,
                                        mCodegen.CreateVariableRef(new_iter));
    new_for_loop = SageBuilder::buildForStatement(
        isSgStatement(static_cast<SgNode *>(new_init_stmt), )
            isSgStatement(static_cast<SgNode *>(new_test_stmt), )
                isSgExpression(static_cast<SgNode *>(new_incr),
                               isSgStatement(static_cast<SgNode *>(new_body)));
#endif
  }

  void *sg_bb = mCodegen.CreateBasicBlock();
  if (true) {
    //  ///////////////////////////////  /
    //  Handling iterator declaration in the for statement
    {
      for (int i = 0; i != max_depth; ++i) {
        void *curr_loop = nested_loops[i];
        void *loop_iter = mCodegen.GetLoopIterator(curr_loop);
        vector<void *> init_var = mCodegen.get_var_decl_in_for_loop(curr_loop);
        if (init_var.size() > 1) {
          return 0;
        }
        if (init_var.size() == 1 &&
            mCodegen.GetVariableInitializedName(init_var[0]) == loop_iter) {
          mCodegen.AppendChild(sg_bb, mCodegen.CreateVariableDecl(
                                          mCodegen.GetTypebyVar(loop_iter),
                                          mCodegen.GetVariableName(loop_iter),
                                          NULL, sg_bb));
        } else if (init_var.size() == 1 && init_var[0] != NULL) {
          return 0;
        }
      }
    }
    //  ///////////////////////////////  /

    mCodegen.AppendChild(sg_bb, new_iter_decl);
#ifndef PUT_ALL_INIT_INTO_LOOP_INIT
    for (size_t i = 0; i != vec_initer.size(); ++i)
      mCodegen.AppendChild(sg_bb, vec_initer[i]);
#endif
    for (int i = 0; i != max_depth; ++i) {
      mCodegen.AddComment("Flattened " +
                              mCodegen.UnparseToString(nested_loops[i]),
                          new_for_loop);
    }

    mCodegen.AppendChild(sg_bb, mCodegen.CopyStmt(new_for_loop));
    if (!no_final_reset) {
      for (size_t i = 0; i != vec_finalizer.size(); ++i) {
        mCodegen.AppendChild(sg_bb, vec_finalizer[i]);
      }
    }
    void *orig_bb = mCodegen.GetParent(top_loop);
    if ((mCodegen.IsBasicBlock(orig_bb) != 0) &&
        mCodegen.GetChildStmtNum(orig_bb) == 1) {
      mCodegen.ReplaceStmt(orig_bb, sg_bb);
    } else {
      mCodegen.ReplaceStmt(top_loop, sg_bb);
    }
  }

  mCodegen.clean_empty_aggregate_initializer(sg_bb);

  return 1;
#undef PUT_ALL_INIT_INTO_LOOP_INIt
}
