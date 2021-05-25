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


//  ******************************************************************************************//
//  module           :   false_data_dependency
//  description      :   solve false data dependency problem 
//  improve performance
//  input            :   code with += false data dependency code
//  output           :   code with channel to work around false data dependency
//  limitation       :   1. the index of += access is the same with all the
//  other usage in the same for loop
//                      2. only have one += respond to the same variable in one
//                      top level for loop
//                      3. Do not have unroll extend
//                      4. when user don't have data initialization before +=,
//                      may have problems
//  author           :   Han
//  ******************************************************************************************//
//  //////////////////////////////////////////  /
//  assign_communication_type()
//  input:  MarsIR
//  output: MarsIR with updated type information annotated for the ports of
//  MarsIR nodes
//  Communication Types:
//   a) find all top for loop
//   b) find all expression, detect += in each expression
//   c) get left reference of +=, detect it is iterator varient
//   d) if (! n,i==0) read_out =  read();
//   e) if (! n,i=N)  write(result);
//   f) data[j][k]  =  (is_init ? init_data: read_out ) + tmp;
//   g) define is_init out if for-loop, add is_init = 0 at the beginning of for
//   loop,
//      add is_init = 1 after each write
#include <tuple>
#include "comm_opt.h"
#include "mars_ir_v2.h"
#include "mars_opt.h"
#include "program_analysis.h"
#include "rose.h"

static int count_self_dep = 0;
static int count_iter = 0;

using MarsProgramAnalysis::ArrayRangeAnalysis;
using MarsProgramAnalysis::CMarsAccess;
using MarsProgramAnalysis::CMarsExpression;
using MarsProgramAnalysis::CMarsRangeExpr;
using MarsProgramAnalysis::CMarsVariable;
using std::tuple;
namespace MarsCommOpt {

void insert_function(CSageCodeGen *codegen, void *call_scope,
                     void *array_pntr) {
  //  insert function named __merlin_ivdep_***
  void *ref_array;
  vector<void *> ref_indexes;
  codegen->parse_pntr_ref(array_pntr, &ref_array, &ref_indexes);

  string return_type = "int";
  string func_name = IVDEP_SPEC_PREFIX + codegen->UnparseToString(ref_array);

  void *array_type = codegen->GetTypebyVar(ref_array);
  //  printf("array_type = %s\n", codegen->UnparseToString(array_type).c_str());
  vector<void *> vec_param_decl;
  void *param_decl = codegen->CreateVariable(array_type, "t");
  vec_param_decl.push_back(param_decl);
  void *func_decl = codegen->GetFuncDeclByName(func_name, 0);
  if (func_decl == nullptr) {
    func_decl =
        codegen->CreateFuncDecl(return_type, func_name, vec_param_decl,
                                codegen->GetGlobal(array_pntr), false, nullptr);
    codegen->PrependChild(codegen->GetGlobal(array_pntr), func_decl);
  }
  void *func_return_type = codegen->GetFuncReturnType(func_decl);
  vector<void *> vec_param_call;
  void *param_call =
      codegen->CreateVariableRef(codegen->GetVariableDecl(ref_array));
  vec_param_call.push_back(param_call);
  void *func_call =
      codegen->CreateFuncCall(func_name, func_return_type, vec_param_call,
                              codegen->GetGlobal(array_pntr));
  void *func_call_stmt = codegen->CreateStmt(V_SgExprStatement, func_call);
  codegen->AppendChild(call_scope, func_call_stmt);
}

int get_loop_bound_of_pntr(CSageCodeGen *codegen, void *array_pntr,
                           vector<void *> *vec_lb, vector<void *> *vec_ub,
                           void *outmost_loop) {
  void *ref_array;
  vector<void *> ref_indexes;
  codegen->parse_pntr_ref(array_pntr, &ref_array, &ref_indexes);
  for (size_t i = 0; i < ref_indexes.size(); i++) {
    void *ref_iter = ref_indexes[i];
    void *parent_loop = codegen->TraceUpToForStatement(array_pntr);
    while (true) {
      void *loop_iter = codegen->GetLoopIterator(parent_loop);

      if (codegen->UnparseToString(loop_iter) ==
          codegen->UnparseToString(ref_iter)) {
        CMarsVariable mars_var(parent_loop, codegen);
        CMarsRangeExpr mars_range_exp = mars_var.get_range();
        CMarsExpression lb;
        CMarsExpression ub;
        int ret = mars_range_exp.get_simple_bound(&lb, &ub);
        if (ret == 0) {
          return 0;
        }

        //  get lower bound and upper bound information
        vec_lb->push_back(lb.get_expr_from_coeff());
        vec_ub->push_back(ub.get_expr_from_coeff());
        //  printf("    lb_exp = %s",
        //  codegen->UnparseToString(lb.get_expr_from_coeff()).c_str());
        //  printf(" ub_exp = %s\n",
        //  codegen->UnparseToString(ub.get_expr_from_coeff()).c_str());
        break;
      }

      if (parent_loop == outmost_loop) {
        break;
      }

      void *parent_stmt = codegen->GetParent(parent_loop);
      parent_loop = codegen->TraceUpToForStatement(parent_stmt);
    }
  }
  return 1;
}

int check_read_write(CSageCodeGen *codegen, void *scope, void *array) {
  int has_read = 0;
  int has_write = 0;
  ArrayRangeAnalysis ris(array, codegen, scope, scope, false, false);
  has_read = ris.has_read();
  has_write = ris.has_write();
  if ((has_read != 0) && (has_write != 0)) {
    return 1;
  }
  return 0;
}

int check_false_data_dep(CSageCodeGen *codegen, vector<void *> loops,
                         void *array_pntr) {
  CMarsAccess ma(array_pntr, codegen, nullptr);
  if (!ma.is_simple_type_v1()) {
    return 0;
  }
  vector<tuple<int, int64_t, int64_t>> vec_access =
      ma.simple_type_v1_get_var_to_dim(loops, 0);
  for (size_t t = 0; t < vec_access.size(); t++) {
    int index = static_cast<int>(vec_access.size() - t - 1);
    if (t == 0 && std::get<0>(vec_access[index]) != -1) {
      //  printf("Detect false data dependency!\n    %s\n",
      //  codegen->UnparseToString(array_pntr).c_str());
      return 1;
    }
    if (t == 0) {
      //  printf("Detect true dependency!\n    %s\n",
      //  codegen->UnparseToString(array_pntr).c_str());
      return 0;
    }
  }
  return 1;
}

int check_if_in_innermost(CSageCodeGen *codegen, void *array,
                          void *innermost_loop) {
  void *var_decl = codegen->GetVariableDecl(array);
  //  printf("var = %s = %s\n", codegen->UnparseToString(array).c_str(),
  //  codegen->UnparseToString(var_decl).c_str());
  if (codegen->IsInScope(var_decl, innermost_loop) != 0) {
    //  printf("in innermost forloop\n");
    return 1;
  }
  return 0;
}

int check_same_pntr(CSageCodeGen *codegen, void *scope, void *outmost_loop,
                    void *array_pntr) {
  int flag = 0;
  vector<void *> lb_ref;
  vector<void *> ub_ref;
  flag = get_loop_bound_of_pntr(codegen, array_pntr, &lb_ref, &ub_ref,
                                outmost_loop);
  void *array_ref;
  codegen->parse_array_ref_from_pntr(array_pntr, &array_ref);
  if (flag == 0) {
    return 0;
  }

  vector<void *> vec_exp;
  codegen->GetNodesByType(scope, "preorder", "SgPntrArrRefExp", &vec_exp);
  int add_step = 0;
  for (size_t i = 0; i < vec_exp.size(); i = i + add_step) {
    void *array_new;
    codegen->parse_array_ref_from_pntr(vec_exp[i], &array_new);
    if (codegen->UnparseToString(array_ref) ==
        codegen->UnparseToString(array_new)) {
      //  printf("array_ref = %s\n",
      //  codegen->UnparseToString(array_ref).c_str()); printf("array_new =
      //  %s\n", codegen->UnparseToString(array_new).c_str());
      vector<void *> lb_new;
      vector<void *> ub_new;
      flag = get_loop_bound_of_pntr(codegen, vec_exp[i], &lb_new, &ub_new,
                                    outmost_loop);
      if (flag == 0) {
        return 0;
      }
      if (lb_ref.size() != lb_new.size()) {
        return 0;
      }
      for (size_t j = 0; j < lb_ref.size(); j++) {
        if (codegen->UnparseToString(lb_ref[j]) !=
                codegen->UnparseToString(lb_new[j]) ||
            codegen->UnparseToString(ub_ref[j]) !=
                codegen->UnparseToString(ub_new[j])) {
          //  printf("lb_ref = %s, ub_ref = %s\n",
          //  codegen->UnparseToString(lb_ref[j]).c_str(),
          //  codegen->UnparseToString(ub_ref[j]).c_str()); printf("lb_new = %s,
          //  ub_new = %s\n", codegen->UnparseToString(lb_new[j]).c_str(),
          //  codegen->UnparseToString(ub_new[j]).c_str()); printf("faild
          //  compare\n");
          return 0;
        }
      }
    }
    void *array;
    vector<void *> ref_indexes;
    codegen->parse_pntr_ref(vec_exp[i], &array, &ref_indexes);
    add_step = ref_indexes.size();
  }
  return 1;
}

#if 0
void check_legal(CSageCodeGen *codegen, CMarsIrV2 *mars_ir) {
  int node_num = mars_ir->size();
  for (int i = 0; i != node_num; i++) {
    CMarsNode *node = mars_ir->get_node(i);
    vector<void *> loops = node->get_loops();
    int loop_depth = node->get_loop_depth();
    //  printf("loop_depth = %d\n",loop_depth);
    if (loop_depth == 0)
      continue;
    if (loop_depth == 1)
      continue;
    void *innermost_loop_body =
        codegen->GetLoopBody(node->get_loop(loop_depth - 1));

    vector<void *> vec_exp;
    codegen->GetNodesByType(innermost_loop_body, "preorder", "SgPlusAssignOp",
                           &vec_exp);
    for (size_t j = 0; j < vec_exp.size(); j++) {
      void *left_exp = codegen->GetExpLeftOperand(vec_exp[j]);
      vector<void *> vec_ref_exp;
      codegen->GetNodesByType(innermost_loop_body, "preorder",
                              "SgPntrArrRefExp", &vec_ref_exp);

      //  ---------------------------------------------------------------------------------------------------//
      //  1. check if all variable ref index is the same
      //  ---------------------------------------------------------------------------------------------------//
      void *array_ref;
      codegen->parse_array_ref_from_pntr(left_exp, &array_ref);
      void *init_name = codegen->GetVariableInitializedName(array_ref);
      vector<void *> all_ref =
          codegen->GetAllRefInScope(init_name, innermost_loop_body);
      int is_same_pntr = 1;
      for (size_t k = 0; k < all_ref.size(); k++) {
        void *ref_array;
        void *ref_pntr;
        int ref_pointer_dim;
        vector<void *> ref_indexes;
        codegen->parse_pntr_ref_by_array_ref(all_ref[k], &ref_array, &ref_pntr,
                                            &ref_indexes, &ref_pointer_dim);
        is_same_pntr = (codegen->UnparseToString(ref_pntr) ==
                        codegen->UnparseToString(left_exp));
        if (!is_same_pntr)
          continue;
        //  printf("all_ref[k] = %s\n",
        //  codegen->UnparseToString(all_ref[k]).c_str());
      }
      if (!is_same_pntr)
        continue;

      //  ---------------------------------------------------------------------------------------------------//
      //  2. check if have one += for the same variable ref
      //  ---------------------------------------------------------------------------------------------------//
      int count_plusassign = 0;
      for (size_t k = 0; k < vec_ref_exp.size(); k++) {
        void *parent_node = codegen->GetParent(vec_ref_exp[k]);
        int is_same_exp = (codegen->UnparseToString(vec_ref_exp[k]) ==
                           codegen->UnparseToString(left_exp));
        if (codegen->IsPlusAssignOp(parent_node) && is_same_exp) {
          count_plusassign++;
          //  printf("child_node = %s",
          //  codegen->UnparseToString(vec_ref_exp[k]).c_str());
          //  printf("    parent_node = %s\n",
          //  codegen->UnparseToString(parent_node).c_str());
        }
        if (count_plusassign > 1) {
          printf("WARNING : Do not support more than one pluss assign on one "
                 "variable\n");
        }
        if (count_plusassign <= 1)
          continue;
      }
      if (count_plusassign <= 1)
        continue;
    }
  }
}
#endif

int check_legal(CSageCodeGen *codegen, CMarsNode *node) {
  vector<void *> loops = node->get_loops();
  int loop_depth = node->get_loop_depth();
  //  printf("loop_depth = %d\n",loop_depth);
  if (loop_depth <= 1) {
    return 0;
  }
  void *innermost_loop_body =
      codegen->GetLoopBody(node->get_loop(loop_depth - 1));

  vector<void *> vec_exp;
  codegen->GetNodesByType(innermost_loop_body, "preorder", "SgPlusAssignOp",
                          &vec_exp);
  if (vec_exp.empty()) {
    return 0;
  }
  return 1;
}

int check_legal(CSageCodeGen *codegen, CMarsNode *node, void *accum_exp) {
  vector<void *> loops = node->get_loops();
  int loop_depth = node->get_loop_depth();
  //  printf("loop_depth = %d\n",loop_depth);
  if (loop_depth <= 1) {
    return 0;
  }
  void *innermost_loop_body =
      codegen->GetLoopBody(node->get_loop(loop_depth - 1));

  void *left_exp = codegen->GetExpLeftOperand(accum_exp);
  vector<void *> vec_ref_exp;
  codegen->GetNodesByType(innermost_loop_body, "preorder", "SgPntrArrRefExp",
                          &vec_ref_exp);

  //  ---------------------------------------------------------------------------------------------------//
  //  1. check if all variable ref index is the same
  //  ---------------------------------------------------------------------------------------------------//
  void *array_ref;
  codegen->parse_array_ref_from_pntr(left_exp, &array_ref);
  //  printf("array ref = %s\n", codegen->UnparseToString(array_ref).c_str());
  void *init_name = codegen->GetVariableInitializedName(array_ref);
  //  printf("init name = %s\n", codegen->UnparseToString(init_name).c_str());
  vector<void *> all_ref =
      codegen->GetAllRefInScope(init_name, innermost_loop_body);
  for (size_t k = 0; k < all_ref.size(); k++) {
    void *ref_array;
    void *ref_pntr;
    int ref_pointer_dim;
    vector<void *> ref_indexes;
    codegen->parse_pntr_ref_by_array_ref(all_ref[k], &ref_array, &ref_pntr,
                                         &ref_indexes, &ref_pointer_dim);
    int is_same_pntr = static_cast<int>(codegen->UnparseToString(ref_pntr) ==
                                        codegen->UnparseToString(left_exp));
    //  assert(is_same_pntr);
    if (is_same_pntr == 0) {
      return 0;
    }
    //  printf("all_ref[k] = %s\n",
    //  codegen->UnparseToString(all_ref[k]).c_str());
  }

  //  ---------------------------------------------------------------------------------------------------//
  //  2. check if have one += for the same variable ref
  //  ---------------------------------------------------------------------------------------------------//
  int count_plusassign = 0;
  for (size_t k = 0; k < vec_ref_exp.size(); k++) {
    void *parent_node = codegen->GetParent(vec_ref_exp[k]);
    int is_same_exp =
        static_cast<int>(codegen->UnparseToString(vec_ref_exp[k]) ==
                         codegen->UnparseToString(left_exp));
    if ((codegen->IsPlusAssignOp(parent_node) != 0) && (is_same_exp != 0)) {
      count_plusassign++;
      //  printf("child_node = %s",
      //  codegen->UnparseToString(vec_ref_exp[k]).c_str());
      //  printf("    parent_node = %s\n",
      //  codegen->UnparseToString(parent_node).c_str());
    }
    if (count_plusassign > 1) {
      printf("ERROR : Do not support more than one pluss assign on one "
             "variable\n");
      return 0;
    }
  }
  return 1;
}

void *create_loop_for_exp(CMarsAST_IF *codegen, void *stmt,
                          const vector<void *> &iteration_unroll,
                          const vector<void *> &iteration_lb_unroll,
                          const vector<void *> &iteration_ub_unroll) {
  string string_unroll = "\n#pragma ACCEL PARALLEL COMPLETE";
  void *new_loop = nullptr;
  for (size_t t = 0; t < iteration_unroll.size(); t++) {
    void *body = codegen->CreateBasicBlock();
    codegen->AppendChild(body, stmt);
    new_loop = codegen->CreateStmt(
        V_SgForStatement,
        codegen->GetVariableInitializedName(iteration_unroll[t]),
        codegen->CopyExp(iteration_lb_unroll[t]),
        codegen->CreateExp(V_SgAddOp, codegen->CopyExp(iteration_ub_unroll[t]),
                           codegen->CreateConst(1)),
        body, nullptr);
    codegen->AddDirectives(string_unroll, stmt);
    stmt = new_loop;
  }
  return new_loop;
}

int channel_gen_one_edge(CMarsAST_IF *codegen, CMarsNode *node,
                         vector<int> &vec_condition  //  unrelated for loops
                         ,
                         vector<void *> &iteration  //  all the for loops
                         ,
                         vector<void *> &iteration_lb  //  loop iteration bounds
                         ,
                         vector<void *> &iteration_ub  //
                         ,
                         void *channel_type, void *basic_channel_type,
                         int channel_depth, int channel_width,
                         void *innermost_loop_body, void *outmost_loop_body,
                         void *ch_read_exp, void *new_ch_read_exp,
                         void *left_exp, string channel_name, int flag_unroll,
                         const vector<void *> &iteration_unroll,
                         const vector<void *> &iteration_lb_unroll,
                         const vector<void *> &iteration_ub_unroll,
                         void *channel_wr_array_ref) {
  int parallel_bit = 0;
  void *dummy_array_ref_w;
  void *dummy_array_ref_r;
  //  void *dummy_array_w_exp;
  //  void *dummy_array_r_exp;
  if (flag_unroll != 0) {
    for (size_t t = 0; t < iteration_unroll.size(); t++) {
      parallel_bit = 2 * parallel_bit + 1;
    }
    string name_w = "merlin_dummy_w_" + my_itoa(count_self_dep);
    string name_r = "merlin_dummy_r_" + my_itoa(count_self_dep);
    void *dummy_array_decl_w = codegen->CreateVariableDecl(
        channel_type, name_w, nullptr, outmost_loop_body);
    void *dummy_array_decl_r = codegen->CreateVariableDecl(
        channel_type, name_r, nullptr, outmost_loop_body);
    codegen->PrependChild(outmost_loop_body, dummy_array_decl_w);
    codegen->PrependChild(outmost_loop_body, dummy_array_decl_r);
    void *var_ref_for_array_w = codegen->CreateVariableRef(dummy_array_decl_w);
    void *var_ref_for_array_r = codegen->CreateVariableRef(dummy_array_decl_r);
    dummy_array_ref_w =
        codegen->CreateArrayRef(var_ref_for_array_w, iteration_unroll);
    dummy_array_ref_r =
        codegen->CreateArrayRef(var_ref_for_array_r, iteration_unroll);
  }
  //  ---------------------------------------------------------------------------------------------------//
  //  6. create channel read if statement
  //  ---------------------------------------------------------------------------------------------------//
  vector<void *> rd_condition_exp;
  //  create condition exp
  void *rd_exp = codegen->CreateConst(1);
  for (size_t t = 0; t < vec_condition.size(); t++) {
    void *condition_exp =
        codegen->CreateExp(V_SgGreaterThanOp, iteration[t], iteration_lb[t]);
    if (t == 0) {
      rd_exp = condition_exp;
    } else {
      rd_exp = codegen->CreateExp(V_SgOrOp, rd_exp, condition_exp);
    }
    //  printf("rd_cond_exp : %s %s %s\n",
    //  codegen->UnparseToString(iteration[t]).c_str(),
    //  codegen->UnparseToString(iteration_lb[t]).c_str(),
    //  codegen->UnparseToString(rd_exp).c_str());
  }
  //  create if statement true body
  void *dummy_exp_r = codegen->CopyExp(rd_exp);
  if (flag_unroll != 0) {
    dummy_exp_r = codegen->CopyExp(dummy_array_ref_r);
  }
  void *func_rd_stmt =
      codegen->CreateStmt(V_SgAssignStatement, new_ch_read_exp, dummy_exp_r);
  void *rd_if_stmt;
  if (flag_unroll != 0) {
    void *new_loop =
        create_loop_for_exp(codegen, func_rd_stmt, iteration_unroll,
                            iteration_lb_unroll, iteration_ub_unroll);
    void *basic_block = codegen->CreateBasicBlock();
    codegen->AppendChild(basic_block, new_loop);
    rd_if_stmt =
        codegen->CreateIfStmt(codegen->CopyExp(rd_exp), basic_block, nullptr);
  } else {
    rd_if_stmt =
        codegen->CreateIfStmt(codegen->CopyExp(rd_exp), func_rd_stmt, nullptr);
  }
  codegen->PrependChild(innermost_loop_body, rd_if_stmt);

  //  ---------------------------------------------------------------------------------------------------//
  //  7. create channel write if statement
  //  ---------------------------------------------------------------------------------------------------//
  vector<void *> wr_condition_exp;
  //  create condition exp
  void *wr_exp = codegen->CreateConst(1);
  for (size_t t = 0; t < vec_condition.size(); t++) {
    void *condition_exp =
        codegen->CreateExp(V_SgLessThanOp, iteration[t], iteration_ub[t]);
    if (t == 0) {
      wr_exp = condition_exp;
    } else {
      wr_exp = codegen->CreateExp(V_SgOrOp, wr_exp, condition_exp);
    }
    //  printf("wr_cond_exp : %s %s %s\n",
    //  codegen->UnparseToString(iteration[t]).c_str(),
    //  codegen->UnparseToString(iteration_lb[t]).c_str(),
    //  codegen->UnparseToString(wr_exp).c_str());
  }
  //  create if statement true body
  void *dummy_exp_w = codegen->CopyExp(wr_exp);
  if (flag_unroll != 0) {
    dummy_exp_w = codegen->CopyExp(dummy_array_ref_w);
  }
  void *func_wr_stmt = codegen->CreateStmt(V_SgExprStatement, dummy_exp_w);
  void *wr_if_stmt;
  if (flag_unroll != 0) {
    void *new_loop =
        create_loop_for_exp(codegen, func_wr_stmt, iteration_unroll,
                            iteration_lb_unroll, iteration_ub_unroll);
    void *basic_block = codegen->CreateBasicBlock();
    codegen->AppendChild(basic_block, new_loop);
    wr_if_stmt =
        codegen->CreateIfStmt(codegen->CopyExp(wr_exp), basic_block, nullptr);
  } else {
    wr_if_stmt =
        codegen->CreateIfStmt(codegen->CopyExp(wr_exp), func_wr_stmt, nullptr);
  }
  codegen->AppendChild(innermost_loop_body, wr_if_stmt);

  //  generate new dummy_exp_w and dummy_exp_r---->merlin_dummy_w[][]
  if (flag_unroll != 0) {
    gen_one_channel(codegen, dummy_exp_w, dummy_exp_r, channel_name,
                    channel_depth, 0, basic_channel_type, channel_wr_array_ref,
                    parallel_bit);
  } else {
    gen_one_channel(codegen, dummy_exp_w, dummy_exp_r, channel_name,
                    channel_depth, 0, basic_channel_type, left_exp);
  }

  return 1;
}

void gen_one_channel(CMarsAST_IF *codegen, void *ref_w, void *ref_r,
                     string channel_name, int channel_depth, int channel_width,
                     void *channel_base_type, void *write_value,
                     int parallel_bit) {
  void *right_exp;
  void *access_scope_rd = codegen->GetScope(ref_r);
  void *access_scope_wr = codegen->GetScope(ref_w);
  void *define_scope = codegen->GetFirstFuncDeclInGlobal(ref_r);
  void *global_scope = codegen->GetGlobal(ref_r);
  if (write_value != nullptr) {
    right_exp = write_value;
  } else {
    assert(codegen->IsAssignOp(codegen->GetParent(ref_w)));
    assert(ref_w == codegen->GetExpLeftOperand(codegen->GetParent(ref_w)));
    right_exp = codegen->GetExpRightOperand(codegen->GetParent(ref_w));
  }

  //  add channel false definition
  string channel_typedef = "__merlin_channel_typedef";
  codegen->RegisterType(channel_typedef);
  vector<size_t> channel_dims;
  vector<size_t> dims;
  if (parallel_bit != 0) {
    CMarsAccess ac_w(ref_w, codegen, nullptr);
    void *array = ac_w.GetArray();
    void *base_type;
    codegen->get_type_dimension(codegen->GetTypebyVar(array), &base_type, &dims,
                                array);

    for (size_t i = 0; i < dims.size(); ++i) {
      if (((1 << i) & parallel_bit) != 0) {
        channel_dims.push_back(dims[dims.size() - 1 - i]);
      }
    }
  }
  void *channel_type = codegen->GetTypeByString(channel_typedef);
  if (!channel_dims.empty()) {
    channel_type = codegen->CreateArrayType(channel_type, channel_dims);
  }

  codegen->get_valid_local_name(global_scope, &channel_name);

  void *channel_name_decl = codegen->CreateVariableDecl(
      channel_type, channel_name, nullptr, global_scope);
  //  void * channel_name_decl = codegen->CreateVariableDecl("int",
  //  channel_name, nullptr, define_scope);
  codegen->InsertStmt(channel_name_decl, define_scope);
  //  void * channel_name_exp = codegen->CreateVariableRef(channel_name_decl);

  //  add include
  string include_def = "\n#include \"channel_def.h\"\n";
  codegen->AddDirectives(include_def, channel_name_decl);

  //  add channel definition
  string def_type = codegen->UnparseToString(channel_base_type);
  string definition_channel =
      "\n#pragma channel " + def_type + " " + channel_name;
  for (auto &dim : channel_dims) {
    definition_channel += "[" + my_itoa(dim) + "]";
  }

  definition_channel +=
      " __attribute__((depth(" + my_itoa(channel_depth) + ")));\n";
  codegen->AddDirectives(definition_channel, channel_name_decl);

  //  add typedef
  //  string type_def = "typedef " + def_type + " __merlin_channel_typedef;\n";
  string type_def = "typedef int __merlin_channel_typedef;\n";
  codegen->AddDirectives(type_def, channel_name_decl);

  //  gen channel read
  string rd_channel_func =
      "read_channel_hidden_" + channel_name + "__merlinhiddenfuncend";
  vector<void *> rd_param_list;
  void *channel_rd_exp = codegen->CreateVariableRef(channel_name_decl);
  if (parallel_bit != 0) {
    CMarsAccess ac_r(ref_r, codegen, nullptr);
    for (size_t i = 0; i < dims.size(); ++i) {
      if (((1 << i) & parallel_bit) != 0) {
        channel_rd_exp =
            codegen->CreateExp(V_SgPntrArrRefExp, channel_rd_exp,
                               ac_r.GetIndex(i).get_expr_from_coeff());
      }
    }
  }
  rd_param_list.push_back(channel_rd_exp);
  void *func_rd_call = codegen->CreateFuncCall(
      rd_channel_func, channel_base_type, rd_param_list, access_scope_rd);
  string old_stmt_rd =
      codegen->UnparseToString(codegen->TraceUpToStatement(ref_r));
  //  void * xxx = codegen->CreateExp(V_SgNotOp,codegen->CreateConst(1));
  //  void * xxxs = codegen->CreateStmt(V_SgExprStatement, xxx);
  //  codegen->AppendChild(access_scope_rd, xxxs);
  //  codegen->ReplaceExp(ref_r, xxx);
  //  codegen->ReplaceExp(xxx, func_rd_call);
  codegen->ReplaceExp(ref_r, func_rd_call);
  //  youxiang 20161215 work around aoc type deduction system
  codegen->AddCastToExp(func_rd_call, channel_base_type);
#define COMMENT_ON_CHANGE 0
#if COMMENT_ON_CHANGE
  codegen->AddComment(old_stmt_rd, codegen->TraceUpToStatement(func_rd_call));
#endif

  //  gen channel write
  string wr_channel_func =
      "write_channel_hidden_" + channel_name + "__merlinhiddenfuncend";
  vector<void *> wr_param_list;
  void *channel_wr_exp = codegen->CreateVariableRef(channel_name_decl);
  if (parallel_bit != 0) {
    CMarsAccess ac_w(ref_w, codegen, nullptr);
    for (size_t i = 0; i < dims.size(); ++i) {
      if (((1 << i) & parallel_bit) != 0) {
        channel_wr_exp =
            codegen->CreateExp(V_SgPntrArrRefExp, channel_wr_exp,
                               ac_w.GetIndex(i).get_expr_from_coeff());
      }
    }
  }

  wr_param_list.push_back(channel_wr_exp);
  wr_param_list.push_back(codegen->CopyExp(right_exp));
  void *func_wr_call = codegen->CreateFuncCall(
      wr_channel_func, channel_base_type, wr_param_list, access_scope_wr);
  string old_stmt_wr =
      codegen->UnparseToString(codegen->TraceUpToStatement(ref_w));
  if (write_value != nullptr) {
    codegen->ReplaceExp(ref_w, func_wr_call);
  } else {
    codegen->ReplaceExp(codegen->GetParent(ref_w), func_wr_call);
  }

  //  generate channel function definition
  FILE *fp;
  fp = fopen("channel_def.h", "a+");
  string def1 = "#ifndef __MERLIN_ALTERA_CHANNEL_DEF_H_INCLUDED_" +
                wr_channel_func + "\n";
  fprintf(fp, "%s", def1.c_str());
  string def2 = "#define __MERLIN_ALTERA_CHANNEL_DEF_H_INCLUDED_" +
                wr_channel_func + "\n";
  fprintf(fp, "%s", def2.c_str());

  string rd_func_string = def_type + " " + rd_channel_func + "(int a);\n";
  string wr_func_string =
      def_type + " " + wr_channel_func + "(int a, " + def_type + " b);\n";
  fprintf(fp, "%s", rd_func_string.c_str());
  fprintf(fp, "%s", wr_func_string.c_str());

  string def3 = "#endif\n";
  fprintf(fp, "%s", def3.c_str());

  fclose(fp);
  //  string test_file_name = "channel_func_def.h";
  //  string rd_func_string = def_type + " " + rd_channel_func + "(int a);\n";
  //  string wr_func_string = def_type + " " + wr_channel_func + "(int a, " +
  //  def_type + " b);\n";
  //  void * file_scope = codegen->CreateSourceFile(test_file_name);
  //  codegen->AddDirectives(rd_func_string, file_scope);
  //  codegen->AddDirectives(wr_func_string, file_scope);
  //  codegen->AddDirectives("#include \"" + test_file_name + "\"\n",
  //  channel_name_decl);

#if COMMENT_ON_CHANGE
  codegen->AddComment(old_stmt_wr, codegen->TraceUpToStatement(func_wr_call));
#endif
}

int false_data_dep_top(CMarsIrV2 *mars_ir, int ivdep_flag) {
  printf("Hello False Data Dependency ... \n");

  CSageCodeGen *codegen = mars_ir->get_ast();
  //  check_legal(codegen, mars_ir);
  int node_num = mars_ir->size();
  for (int i = 0; i != node_num; i++) {
    CMarsNode *node = mars_ir->get_node(i);
    vector<void *> loops = node->get_loops();
    int loop_depth = node->get_loop_depth();
    if (loop_depth <= 0) {
      continue;
    }
    if (loop_depth == 1) {
      continue;
    }
    //  printf("\nget for loop depth = %d\n", loop_depth);
    void *outmost_loop_body = codegen->GetLoopBody(node->get_loop(0));
    void *innermost_loop_body =
        codegen->GetLoopBody(node->get_loop(loop_depth - 1));

    //  get kernel for loop iteration and lb and ub vector
    vector<void *> iteration;
    vector<CMarsExpression> cmarsexp_lb;
    vector<CMarsExpression> cmarsexp_ub;
    vector<void *> iteration_lb;
    vector<void *> iteration_ub;
    void *innermost_loop;
    void *outermost_loop;
    bool valid = true;
    for (int j = 0; j < loop_depth; j++) {
      void *loop_iter = codegen->GetLoopIterator(node->get_loop(j));
      CMarsVariable mars_var(node->get_loop(j), codegen);
      CMarsRangeExpr mars_range_exp = mars_var.get_range();
      CMarsExpression lb;
      CMarsExpression ub;
      int ret = mars_range_exp.get_simple_bound(&lb, &ub);
      if (ret == 0) {
        valid = false;
        break;
      }
      void *lb_exp = lb.get_expr_from_coeff();
      void *ub_exp = ub.get_expr_from_coeff();
      string string_lb = codegen->UnparseToString(lb_exp);
      string string_ub = codegen->UnparseToString(ub_exp);
      iteration.push_back(loop_iter);
      cmarsexp_lb.push_back(lb);
      cmarsexp_ub.push_back(ub);
      iteration_lb.push_back(lb_exp);
      iteration_ub.push_back(ub_exp);
      //  printf("\niter_exp = %s",
      //  codegen->UnparseToString(loop_iter).c_str()); printf("    lb_exp =
      //  %s",   codegen->UnparseToString(lb_exp).c_str()); printf("    ub_exp =
      //  %s\n", codegen->UnparseToString(ub_exp).c_str());
      if (j == loop_depth - 1) {
        innermost_loop = node->get_loop(j);
      }
      if (j == 0) {
        outermost_loop = node->get_loop(j);
      }
    }
    if (!valid) {
      continue;
    }

    if (ivdep_flag == 0) {
      if (check_legal(codegen, node) == 0) {
        continue;
      }
      vector<void *> vec_exp;
      //  codegen->GetNodesByType(innermost_loop_body, "preorder",
      //  "SgExprStatement", &vec_exp_stmt);
      codegen->GetNodesByType(innermost_loop_body, "preorder", "SgPlusAssignOp",
                              &vec_exp);
      for (size_t j = 0; j < vec_exp.size(); j++) {
        if (check_legal(codegen, node, vec_exp[j]) == 0) {
          continue;
        }

        //  ---------------------------------------------------------------------------------------------------//
        //  parse += statement
        //  ---------------------------------------------------------------------------------------------------//
        void *left_exp = codegen->GetExpLeftOperand(vec_exp[j]);
        string left_str = codegen->UnparseToString(left_exp);
        void *right_exp = codegen->GetExpRightOperand(vec_exp[j]);
        void *plusassign_stmt = codegen->TraceUpToStatement(left_exp);
        string array_name;
        array_name = codegen->GetSgPntrArrName(left_exp);
        void *array_ref;
        codegen->parse_array_ref_from_pntr(left_exp, &array_ref);
        void *array_type = codegen->GetTypebyVarRef(array_ref);
        void *array_base_type = codegen->GetBaseType(array_type, false);
        void *ref_array;
        void *ref_pntr;
        int ref_pointer_dim;
        vector<void *> ref_indexes;
        codegen->parse_pntr_ref_by_array_ref(array_ref, &ref_array, &ref_pntr,
                                             &ref_indexes, &ref_pointer_dim);

        //  check if kernel port
        void *init_name = codegen->GetVariableInitializedName(array_ref);
        if (mars_ir->is_shared_port(init_name) ||
            mars_ir->is_kernel_port(init_name)) {
          continue;
        }

        //  ---------------------------------------------------------------------------------------------------//
        //  extract parallel false data dependency information
        //  ---------------------------------------------------------------------------------------------------//
        vector<void *> iteration_unroll;
        vector<void *> new_iteration_unroll;
        vector<CMarsExpression> cmarsexp_lb_unroll;
        vector<CMarsExpression> cmarsexp_ub_unroll;
        vector<void *> iteration_lb_unroll;
        vector<void *> iteration_ub_unroll;
        vector<size_t> vec_dim_unroll;
        int basic_unroll_depth = 1;
        int flag_unroll = 0;
        int flag_unroll_legal = 1;
        void *parent_loop = codegen->TraceUpToForStatement(vec_exp[j]);
        while (true) {
          if (innermost_loop == parent_loop) {
            break;
          }
          //  printf("Unroll for-loop =
          //  %s\n",codegen->UnparseToString(parent_loop).c_str());
          CMarsLoop *loop_info = mars_ir->get_loop_info(parent_loop);
          if (loop_info->is_complete_unroll() != 0) {
            //  printf("+= parent for loop is unrolled!\n
            //  %s\n",codegen->UnparseToString(vec_exp[j]).c_str());
            //  continue;
            void *loop_iter = codegen->GetLoopIterator(parent_loop);
            int match_ref = 0;
            for (size_t i = 0; i < ref_indexes.size(); i++) {
              //  printf("  ref_indexes : %s;\n",
              //  codegen->UnparseToString(ref_indexes[i]).c_str());
              if (codegen->UnparseToString(ref_indexes[i]) ==
                  codegen->UnparseToString(loop_iter)) {
                match_ref = 1;
              }
            }
            if (match_ref == 0) {
              flag_unroll_legal = 0;
              break;
            }
            CMarsVariable mars_var(parent_loop, codegen);
            CMarsRangeExpr mars_range_exp = mars_var.get_range();
            CMarsExpression lb;
            CMarsExpression ub;
            int ret = mars_range_exp.get_simple_bound(&lb, &ub);
            if (ret == 0) {
              flag_unroll_legal = 0;
              break;
            }
            //  get lower bound and upper bound information
            void *lb_exp = lb.get_expr_from_coeff();
            void *ub_exp = ub.get_expr_from_coeff();
            string string_lb = codegen->UnparseToString(lb_exp);
            string string_ub = codegen->UnparseToString(ub_exp);
            iteration_unroll.push_back(loop_iter);
            cmarsexp_lb_unroll.push_back(lb);
            cmarsexp_ub_unroll.push_back(ub);
            iteration_lb_unroll.push_back(codegen->CopyExp(lb_exp));
            iteration_ub_unroll.push_back(codegen->CopyExp(ub_exp));
            //  printf("\niter_exp = %s",
            //  codegen->UnparseToString(loop_iter).c_str());
            //  printf("    lb_exp = %s",
            //  codegen->UnparseToString(lb_exp).c_str());
            //  printf("    ub_exp = %s\n",
            //  codegen->UnparseToString(ub_exp).c_str());
            //  assert(lb.IsConstant());
            //  assert(ub.IsConstant());
            if ((lb.IsConstant() == 0) || (ub.IsConstant() == 0)) {
              flag_unroll_legal = 0;
              break;
            }
            int int_lb = lb.get_const();
            int int_ub = ub.get_const();
            vec_dim_unroll.push_back(int_ub + 1);
            basic_unroll_depth *= (int_ub - int_lb + 1);
            //  printf("    depth  = %d\n", basic_unroll_depth);

            //  create new iterator for condition
            string string_iter = "__merlin_iteration_" + my_itoa(count_iter);
            void *new_var_decl = codegen->CreateVariableDecl(
                "int", string_iter, nullptr, outmost_loop_body);
            codegen->InsertStmt(new_var_decl, node->get_loop(0));
            void *new_var_ref = codegen->CreateVariableRef(new_var_decl);
            new_iteration_unroll.push_back(new_var_ref);
            count_iter += 1;

            void *parent_stmt = codegen->GetParent(parent_loop);
            parent_loop = codegen->TraceUpToForStatement(parent_stmt);
            flag_unroll = 1;
          } else {
            flag_unroll_legal = 0;
            break;
          }
        }
        if (flag_unroll_legal == 0) {
          continue;
        }

        //  channel attribution generated
        int channel_depth = 1;
        string channel_name;
        int channel_flag = 0;

        //  ---------------------------------------------------------------------------------------------------//
        //  analyze array and get channel depth : get iterator which is not in
        //  loops variables, check from the innermost loop iterator to outmost
        //  for loop, search until the first iterator which is not in array
        //  iterator, get array[in loop interator before first not in] channel
        //  depth = (ub[0]-lb[0])*(ub[1]-lb[1])... if channel_flag=1, need to
        //  use channel to slove self dependency
        //  ---------------------------------------------------------------------------------------------------//
        CMarsAccess ma(left_exp, codegen, nullptr);
        if (!ma.is_simple_type_v1()) {
          continue;
        }
        vector<tuple<int, int64_t, int64_t>> vec_access =
            ma.simple_type_v1_get_var_to_dim(loops, 0);
        for (size_t t = 0; t < vec_access.size(); t++) {
          //  cout << "Loop dim " << codegen->_p(loops[t]) << " : " <<
          //  vec_access[t]
          //  << endl;
          int index = static_cast<int>(vec_access.size() - t - 1);
          if (t == 0 && std::get<0>(vec_access[index]) != -1) {
            channel_flag = 1;
#if PROJDEBUG
            printf("Detect false data dependency!\n    %s in %s\n",
                   codegen->UnparseToString(left_exp).c_str(),
                   codegen->UnparseToString(vec_exp[j]).c_str());
#endif
          } else if (t == 0) {
#if PROJDEBUG
            printf("Detect true dependency!\n    %s in %s\n",
                   codegen->UnparseToString(left_exp).c_str(),
                   codegen->UnparseToString(vec_exp[j]).c_str());
#endif
            break;
          }
          if (std::get<0>(vec_access[index]) != -1) {
            //  assert(cmarsexp_lb[index].IsConstant());
            //  assert(cmarsexp_ub[index].IsConstant());
            if ((cmarsexp_lb[index].IsConstant() == 0) ||
                (cmarsexp_ub[index].IsConstant() == 0)) {
              channel_flag = 0;
              break;
            }
            int int_lb = cmarsexp_lb[index].get_const();
            int int_ub = cmarsexp_ub[index].get_const();
            channel_depth *= (int_ub - int_lb + 1);
            //  channel_flag = 1;
            //  printf("vec_access[%d]=%d\n", index,
            //  std::get<0>(vec_access[index]));
          } else {
            break;
          }
        }
        //  printf("    channel depth  = %d\n", channel_depth);

        //  get parallel bit
        //  int parallel_bit = 0;
        //  vector<tuple<void*, int64_t, int64_t> > loop_parallel =
        //  ma.simple_type_v1_get_dim_to_var();
        //  for (size_t t = 0; t < loop_parallel.size(); t++) {
        //    int index = t;
        //    void * loop = std::get<0>(loop_parallel[index]);
        //    CMarsLoop* loop_info = mars_ir->get_loop_info(loop);
        //    if(loop_info->is_complete_unroll()) {
        //        parallel_bit = (parallel_bit<<1) + 1;
        //        printf("1parallel bit = %d\n",parallel_bit);
        //    } else {
        //        parallel_bit = (parallel_bit<<1);
        //        printf("2parallel bit = %d\n",parallel_bit);
        //    }
        //  }

        void *channel_type;
        if (flag_unroll == 1) {
          channel_type =
              codegen->CreateArrayType(array_base_type, vec_dim_unroll);
        } else {
          channel_type = array_base_type;
        }
        void *basic_channel_type = array_base_type;
        //  printf("  channel type : %s;\n",
        //  codegen->UnparseToString(channel_type).c_str());

        if (channel_flag == 1) {
          count_self_dep++;

          //  ---------------------------------------------------------------------------------------------------//
          //  prepare channel_read variable, data_init variable, is_init flag,
          //  analyze array reference
          //  ---------------------------------------------------------------------------------------------------//
          void *top_scope = codegen->GetParent(node->get_loop(0));
          void *const_0 = codegen->CreateConst(0);
          void *const_1 = codegen->CreateConst(1);

          //  ---------------------------------------------------------------------------------------------------//
          //  generate channel read and write variable
          //  ---------------------------------------------------------------------------------------------------//
          string ch_read =
              "__ch_read_" + array_name + "_" + my_itoa(count_self_dep);
          void *ch_read_decl;
          void *ch_read_exp;
          void *new_ch_read_exp;  //  used for channel read and write for loop
          if (flag_unroll == 1) {
            void *ch_array_decl = codegen->CreateVariableDecl(
                channel_type, ch_read, nullptr, top_scope);
            codegen->InsertStmt(ch_array_decl, node->get_loop(0));
            ch_read_exp = codegen->CreateArrayRef(
                codegen->CreateVariableRef(ch_array_decl), iteration_unroll);
            new_ch_read_exp = codegen->CreateArrayRef(
                codegen->CreateVariableRef(ch_array_decl),
                new_iteration_unroll);
          } else {
            ch_read_decl = codegen->CreateVariableDecl(channel_type, ch_read,
                                                       nullptr, top_scope);
            codegen->InsertStmt(ch_read_decl, node->get_loop(0));
            ch_read_exp = codegen->CreateVariableRef(ch_read_decl);
            new_ch_read_exp = ch_read_exp;
          }
          //  printf("  channel read exp : %s;\n",
          //  codegen->UnparseToString(ch_read_exp).c_str());
          //  printf("  new channel read exp : %s;\n",
          //  codegen->UnparseToString(new_ch_read_exp).c_str());

          //  ---------------------------------------------------------------------------------------------------//
          //  generate is init flag
          //  ---------------------------------------------------------------------------------------------------//
          string is_init =
              "__ch_is_" + array_name + "_init_" + my_itoa(count_self_dep);
          void *is_init_decl =
              codegen->CreateVariableDecl("int", is_init, nullptr, top_scope);
          codegen->InsertStmt(is_init_decl, node->get_loop(0));
          void *is_init_exp = codegen->CreateVariableRef(is_init_decl);
          void *is_init_first_stmt = codegen->CreateStmt(
              V_SgAssignStatement, codegen->CopyExp(is_init_exp),
              codegen->CopyExp(const_0));
          void *is_init_mid_stmt = codegen->CreateStmt(
              V_SgAssignStatement, codegen->CopyExp(is_init_exp),
              codegen->CopyExp(const_1));

          void *ref_array;
          void *ref_pntr;
          int ref_pointer_dim;
          vector<void *> ref_indexes;
          void *array_ref = nullptr;
          codegen->parse_array_ref_from_pntr(left_exp, &array_ref);
          codegen->parse_pntr_ref_by_array_ref(array_ref, &ref_array, &ref_pntr,
                                               &ref_indexes, &ref_pointer_dim);
          void *base_type;
          vector<size_t> ref_dims;
          codegen->get_type_dimension(codegen->GetTypebyVar(ref_array),
                                      &base_type, &ref_dims, ref_array);
          //  for(size_t i=0; i<ref_dims.size(); i++) {
          //    printf("  ref_dim : %d;\n", ref_dims[i]);
          //  }

          //  ---------------------------------------------------------------------------------------------------//
          //  generate new reference of array write and variable index which
          //  unrolled
          //  ---------------------------------------------------------------------------------------------------//
          int count_match = 0;
          void *ch_array_ref_w;
          vector<void *> new_ref_indexes;
          vector<int> ref_keep_index;
          vector<int> ref_keep_index_dim;
          if (flag_unroll == 1) {
            for (size_t i = 0; i < ref_indexes.size(); i++) {
              //  printf("  ref_indexes : %s;\n",
              //  codegen->UnparseToString(ref_indexes[i]).c_str());
              void *parent_loop = codegen->TraceUpToForStatement(left_exp);
              count_match = 0;
              int flag = 0;
              while (true) {
                if (innermost_loop == parent_loop) {
                  break;
                }
                void *loop_iter = codegen->GetLoopIterator(parent_loop);
                CMarsLoop *loop_info = mars_ir->get_loop_info(parent_loop);
                if (loop_info->is_complete_unroll() != 0) {
                  if (codegen->UnparseToString(ref_indexes[i]) ==
                      codegen->UnparseToString(loop_iter)) {
                    new_ref_indexes.push_back(
                        new_iteration_unroll[count_match]);
                    ref_keep_index.push_back(i);
                    ref_keep_index_dim.push_back(ref_dims[i]);
                    flag = 1;
                    //  printf("  finial ref_indexes : %s;\n",
                    //  codegen->UnparseToString(new_iteration_unroll[count_match]).c_str());
                  }
                  count_match++;
                }
                void *parent_stmt = codegen->GetParent(parent_loop);
                parent_loop = codegen->TraceUpToForStatement(parent_stmt);
              }
              if (flag == 0) {
                new_ref_indexes.push_back(codegen->CopyExp(ref_indexes[i]));
                //  printf("  finial ref_indexes : %s;\n",
                //  codegen->UnparseToString(ref_indexes[i]).c_str());
              }
            }
            ch_array_ref_w = codegen->CreateArrayRef(
                codegen->CopyExp(array_ref), new_ref_indexes);
            //  printf("  ch_array_ref_w : %s;\n",
            //  codegen->UnparseToString(ch_array_ref_w).c_str());
          }

          //  ---------------------------------------------------------------------------------------------------//
          //  generate new variable init value
          //  ---------------------------------------------------------------------------------------------------//
          string init_name =
              "__ch_" + array_name + "_init_" + my_itoa(count_self_dep);
          void *init_type = channel_type;
          void *init_exp;
          void *var_ref_for_array_init;
          if (flag_unroll == 1) {
            vector<size_t> new_index_dim;
            for (size_t t = 0; t < ref_keep_index_dim.size(); t++) {
              new_index_dim.push_back(ref_dims[t]);
            }
            void *init_array_decl = codegen->CreateVariableDecl(
                codegen->CreateArrayType(basic_channel_type, new_index_dim),
                init_name, nullptr, top_scope);
            //  void * init_array_decl =
            //  codegen->CreateVariableDecl(channel_type, init_name, nullptr,
            //  top_scope);
            codegen->InsertStmt(init_array_decl, node->get_loop(0));
            var_ref_for_array_init =
                codegen->CreateVariableRef(init_array_decl);
            vector<void *> new_index;
            for (size_t t = 0; t < ref_keep_index.size(); t++) {
              new_index.push_back(iteration_unroll[t]);
            }
            //  init_exp = codegen->CreateArrayRef(var_ref_for_array_init,
            //  iteration_unroll);
            init_exp =
                codegen->CreateArrayRef(var_ref_for_array_init, new_index);
          } else {
            void *init_decl = codegen->CreateVariableDecl(init_type, init_name,
                                                          nullptr, top_scope);
            codegen->InsertStmt(init_decl, node->get_loop(0));
            init_exp = codegen->CreateVariableRef(init_decl);
          }
          //  printf("  channel init exp : %s;\n",
          //  codegen->UnparseToString(init_exp).c_str());
          //  void * new_array_ref =
          //  codegen->CreateArrayRef(codegen->CopyExp(init_exp), ref_indexes);
          void *new_array_ref = codegen->CopyExp(init_exp);

          //  ---------------------------------------------------------------------------------------------------//
          //  get channel condition iterator index
          //  ---------------------------------------------------------------------------------------------------//
          vector<int> vec_condition;
          int is_condition = 0;
          for (size_t t = 0; t < vec_access.size(); t++) {
            if (std::get<0>(vec_access[vec_access.size() - t - 1]) == -1) {
              is_condition = 1;
            }
            if (is_condition != 0) {
              vec_condition.push_back(vec_access.size() - t - 1);
              //  printf("\ncondition = %d", vec_access.size() - t - 1);
            }
          }

          channel_name =
              "__ch_" + array_name + "_dep_" + my_itoa(count_self_dep);
#if PROJDEBUG
          printf("\nchannel_type = %s, channel_name = %s, channel depth = %d\n",
                 codegen->UnparseToString(basic_channel_type).c_str(),
                 channel_name.c_str(), channel_depth);
#endif
          //  printf("channel_exp : %s\n",
          //  codegen->UnparseToString(channel_name_exp).c_str());

          channel_gen_one_edge(
              codegen, node, vec_condition  //  unrelated for loops
              ,
              iteration  //  all the for loops
              ,
              iteration_lb  //  loop iteration bounds
              ,
              iteration_ub  //
              ,
              channel_type, basic_channel_type, channel_depth,
              basic_unroll_depth, innermost_loop_body, outmost_loop_body,
              ch_read_exp, new_ch_read_exp, left_exp, channel_name, flag_unroll,
              new_iteration_unroll, iteration_lb_unroll, iteration_ub_unroll,
              ch_array_ref_w);

          //  ---------------------------------------------------------------------------------------------------//
          //  create a new += statement and replace with old one
          //  left_exp = first_exp ? second_exp : third_exp + right_exp
          //  ---------------------------------------------------------------------------------------------------//
          void *first_exp = codegen->CopyExp(is_init_exp);
          void *second_exp = codegen->CopyExp(new_array_ref);
          void *third_exp = codegen->CopyExp(ch_read_exp);
          void *condition_exp =
              codegen->CreateConditionExp(first_exp, second_exp, third_exp);
          void *condition_add_exp = codegen->CreateExp(
              V_SgAddOp, condition_exp, codegen->CopyExp(right_exp));
          void *new_left_exp = codegen->CopyExp(left_exp);
          void *new_plusassign_stmt = codegen->CreateStmt(
              V_SgAssignStatement, new_left_exp, condition_add_exp);
          codegen->ReplaceStmt(plusassign_stmt, new_plusassign_stmt);

          //  ---------------------------------------------------------------------------------------------------//
          //  add init_condition=1 after each array write; add init_condition=0
          //  at the beginning of body
          //  ---------------------------------------------------------------------------------------------------//
          vector<void *> vec_ref_exp;
          codegen->GetNodesByType(innermost_loop_body, "preorder",
                                  "SgPntrArrRefExp", &vec_ref_exp);
          vector<void *> vec_ref_pntr;
          for (size_t k = 0; k < vec_ref_exp.size(); k++) {
            void *ref_pntr = vec_ref_exp[k];
            void *parent_node = codegen->GetParent(ref_pntr);
            if (codegen->IsPntrArrRefExp(parent_node) != 0) {
              continue;
            }
            if (codegen->IsAssignOp(parent_node) != 0) {
              void *left_op = codegen->GetExpLeftOperand(parent_node);
              if (left_op == ref_pntr) {
                vec_ref_pntr.push_back(ref_pntr);
              }
            }
          }

          int write_count = 0;
          for (size_t k = 0; k < vec_ref_pntr.size(); k++) {
            void *ref_pntr = vec_ref_pntr[k];
            void *parent_node = codegen->GetParent(ref_pntr);
            int is_same_exp = static_cast<int>(
                codegen->UnparseToString(ref_pntr) == left_str);
            if ((codegen->IsAssignOp(parent_node) != 0) && (is_same_exp != 0) &&
                ref_pntr != new_left_exp) {
              void *left_op = codegen->GetExpLeftOperand(parent_node);
              void *right_op = codegen->GetExpRightOperand(parent_node);
              if (left_op == ref_pntr) {
#if PROJDEBUG
                printf("child_node : %s",
                       codegen->UnparseToString(ref_pntr).c_str());
                printf("    parent_node : %s\n",
                       codegen->UnparseToString(parent_node).c_str());
#endif
                void *ref_array;
                vector<void *> ref_indexes;
                codegen->parse_pntr_ref_new(ref_pntr, &ref_array, &ref_indexes);
                if (flag_unroll == 1) {
                  vector<void *> new_index;
                  for (size_t t = 0; t < ref_keep_index.size(); t++) {
                    new_index.push_back(codegen->CopyExp(ref_indexes[t]));
                  }
                  std::reverse(ref_indexes.begin(), ref_indexes.end());
                  new_array_ref = codegen->CreateArrayRef(
                      codegen->CopyExp(var_ref_for_array_init), new_index);
                }
                void *new_init_stmt = codegen->CreateStmt(
                    V_SgAssignStatement, codegen->CopyExp(new_array_ref),
                    codegen->CopyExp(right_op));
                void *ref_statement = codegen->TraceUpToStatement(parent_node);
                codegen->InsertAfterStmt(is_init_mid_stmt, ref_statement);
                codegen->ReplaceStmt(ref_statement, new_init_stmt);
                write_count++;
              }
            }
          }

          //  create default condition exp
          //  void * default_exp;
          //  for(size_t t = 0; t < vec_condition.size(); t++) {
          //    void * condition_exp = codegen->CreateExp(V_SgEqualityOp,
          //    iteration[t], iteration_lb[t]);
          //    if(t == 0) {
          //        default_exp = condition_exp;
          //    } else {
          //        default_exp = codegen->CreateExp(V_SgAndOp, default_exp,
          //        condition_exp);
          //    }
          //    //  printf("default_cond_exp : %s %s %s\n",
          //    codegen->UnparseToString(iteration[t]).c_str(),
          //    codegen->UnparseToString(iteration_lb[t]).c_str(),
          //    codegen->UnparseToString(default_exp).c_str());
          //  }

          //  if has not only write statement, create a default write statement,
          //  write it with 0 as default
          //  if(write_count == 0) {
          //    //  create if statement true body
          //    void * default_wr_stmt =
          //    codegen->CreateStmt(V_SgAssignStatement,
          //    codegen->CopyExp(new_array_ref), codegen->CopyExp(const_0));
          //    void * default_if_stmt =
          //    codegen->CreateIfStmt(codegen->CopyExp(default_exp),
          //    default_wr_stmt, nullptr);
          //    codegen->InsertStmt(default_if_stmt, new_plusassign_stmt);
          //    codegen->InsertAfterStmt(is_init_mid_stmt, default_wr_stmt);
          //  }

          //  init in the for loop body
          codegen->PrependChild(innermost_loop_body, is_init_first_stmt);
        }
      }
    } else if (ivdep_flag == 1) {
      vector<void *> vec_exp;
      codegen->GetNodesByType(innermost_loop_body, "preorder",
                              "SgPntrArrRefExp", &vec_exp);
      //  codegen->GetNodesByType(innermost_loop_body, "preorder",
      //  "SgVarRefExp", &vec_exp); int ref_pointer_dim = 0;
      int add_step = 0;
      vector<void *> already_done;
      for (size_t j = 0; j < vec_exp.size(); j = j + add_step) {
        void *ref_array = nullptr;
        //  void *ref_pntr;
        add_step = 1;
        vector<void *> ref_indexes;
        //  youxiang 20170310
        void *func_call = codegen->TraceUpToFuncCall(vec_exp[j]);
        if ((func_call != nullptr) &&
            codegen->IsMerlinInternalIntrinsic(
                codegen->GetFuncNameByCall(func_call, false))) {
          continue;
        }
        //  codegen->parse_pntr_ref_by_array_ref(vec_exp[j], &ref_array,
        //  &ref_pntr, &ref_indexes, &ref_pointer_dim);
        codegen->parse_pntr_ref(vec_exp[j], &ref_array, &ref_indexes);
        add_step = ref_indexes.size();

        if (check_if_in_innermost(codegen, ref_array, innermost_loop_body) !=
            0) {
          //  printf("check innermost\n");
          continue;
        }

        //  printf("SgPntrArrRefExp =
        //  %s\n",codegen->UnparseToString(vec_exp[j]).c_str());
        if (std::find(already_done.begin(), already_done.end(), ref_array) !=
            already_done.end()) {
          //  printf("already checked!\n");
          continue;
        }
        already_done.push_back(ref_array);
        if (ref_array == nullptr) {
          continue;
        }
        //  void *array_ref;
        //  codegen->parse_array_ref_from_pntr(vec_exp[j], &array_ref);
        //  void * init_name = codegen->GetVariableInitializedName(array_ref);
        void *init_name = ref_array;
        if (mars_ir->is_shared_port(init_name) ||
            mars_ir->is_kernel_port(init_name)) {
          continue;
        }

        if (check_same_pntr(codegen, innermost_loop_body, outermost_loop,
                            vec_exp[j]) == 0) {
          //  printf("pntr check failed!\n");
          continue;
        }

        if (check_read_write(codegen, innermost_loop_body, ref_array) == 0) {
          //  printf("read and write check failed!\n");
          continue;
        }

        if (check_false_data_dep(codegen, loops, vec_exp[j]) == 0) {
          //  printf("false data dep check failed!\n");
          continue;
        }

        string msg = "Applying the false data dependence on the variable " +
                     codegen->UnparseToString(vec_exp[j]) + "\n";
        dump_critical_message("IVDEP", "INFO", msg, 101);
        //  printf("Add ivdep on variable =
        //  %s\n",codegen->UnparseToString(vec_exp[j]).c_str());

        insert_function(codegen, innermost_loop_body, vec_exp[j]);
      }
    } else {
    }
  }
  printf("End False Data Dependency ... \n");
  return 1;
}
}  //  namespace MarsCommOpt
