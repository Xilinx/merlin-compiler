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


#include "loop_flatten.h"
#include "program_analysis.h"
#include "ir_types.h"
#include "history_message.h"

using MarsProgramAnalysis::CMarsExpression;
using MarsProgramAnalysis::CMarsRangeExpr;
using MarsProgramAnalysis::CMarsVariable;

int loop_flatten_one_level(CSageCodeGen *codegen, void *sg_loop,
                           void **new_loop);

int loop_flatten_for_pipeline(CSageCodeGen *codegen, void *pTopFunc,
                              const CInputOptions &options, CMarsIr *mars_ir) {
  if (!options.get_option_key_value("-a", "naive_hls").empty()) {
    return 0;
  }
  cout << "Start loop flattening\n";

  //  1. get the arrays to transform
  //  Revised by Yuxin, 23-Feb-2016
  //  Change the order of applying CGPIP, using mars ir topological order
  vector<CMirNode *> vec_nodes;
  mars_ir->get_topological_order_nodes(&vec_nodes);

  //  Yuxin: 07/11/2019
  //  Conver all the Merlin loop_flatten off pragma to HLS
  vector<void *> new_decls;
  codegen->GetNodesByType(codegen->GetProject(), "preorder",
                          "SgPragmaDeclaration", &new_decls);
  set<void *> auto_flatten_off;
  string pragma_str = HLS_PRAGMA + std::string(" LOOP_FLATTEN OFF");
  for (size_t ii = 0; ii < new_decls.size(); ii++) {
    void *pragma = new_decls[ii];
    bool errorOut = false;
    CAnalPragma ana_pragma(codegen);
    if (ana_pragma.PragmasFrontendProcessing(pragma, &errorOut, false)) {
      if (ana_pragma.is_loop_flatten() &&
          !ana_pragma.get_attribute(CMOST_OFF).empty()) {
        if (!ana_pragma.get_attribute(CMOST_auto).empty()) {
          auto_flatten_off.insert(codegen->GetScope(pragma));
          string pragmaString = codegen->_up(pragma);
          codegen->AddComment_v1(codegen->GetScope(pragma),
                                 "Original: #pragma " + pragmaString);
          codegen->RemoveStmt(pragma);
          continue;
        }
        cout << "Find Merlin flatten off directive.\n";
        void *new_pragma =
            codegen->CreatePragma(pragma_str, codegen->GetScope(pragma));
        codegen->ReplaceStmt(pragma, new_pragma);
      }
    }
  }

  for (size_t i = 0; i < vec_nodes.size(); i++) {
    CMirNode *new_node = vec_nodes[i];
    if (new_node->is_function) {
      continue;
    }
    if (new_node->is_while) {
      continue;
    }

    void *scope = new_node->ref;
    void *sg_loop = codegen->GetParent(scope);
    void *sg_upper_loop =
        codegen->TraceUpToLoopScope(codegen->GetParent(sg_loop));
    if (auto_flatten_off.count(scope) > 0) {
      continue;
    }
    if (codegen->IsForStatement(sg_upper_loop) == 0) {
      continue;
    }

    //  ZP: 20151125: fix the problem if sg_loop is non-canonical
    if (codegen->GetLoopIterator(sg_loop) == nullptr) {
      continue;
    }
    if (sg_upper_loop != nullptr) {
      if (codegen->GetLoopIterator(sg_upper_loop) == nullptr) {
        continue;
      }
    }

    vector<void *> new_decls;
    codegen->GetNodesByType(sg_upper_loop, "preorder", "SgPragmaDeclaration",
                            &new_decls);
    bool hls_flatten = false;
    for (size_t ii = 0; ii < new_decls.size(); ii++) {
      SgPragmaDeclaration *decl =
          isSgPragmaDeclaration(static_cast<SgNode *>(new_decls[ii]));
      CAnalPragma ana_pragma(codegen);
      string pragmaString = decl->get_pragma()->get_pragma();
      vector<string> constructs;
      ana_pragma.splitString(pragmaString, " ", &constructs);
      if (constructs.size() != 3) {
        continue;
      }
      bool flatten1 = true;
      for (int i = 0; i < 2; i++) {
        string tokenConstruct = constructs[i];
        boost::algorithm::to_upper(tokenConstruct);
        if ((i == 0 && tokenConstruct != "HLS") ||
            (i == 1 && tokenConstruct != "LOOP_FLATTEN") ||
            (i == 2 && tokenConstruct != "OFF")) {
          flatten1 = false;
          break;
        }
      }
      if (flatten1) {
        hls_flatten = true;
        break;
      }
    }
    if (hls_flatten) {
      cout << "Find HLS flatten off directive. Stop flattening this loop.\n";
      continue;
    }
    if (new_node->has_pipeline() == 0) {
      continue;
    }
    if ((new_node->has_pipeline() != 0) && (new_node->has_parallel() != 0)) {
      continue;
    }

    string str_upper_label;
    string str_label;
    string str_upper_id;
    if (sg_upper_loop != nullptr) {
      str_upper_label = codegen->get_internal_loop_label(sg_upper_loop);
      str_label = codegen->get_internal_loop_label(sg_loop);
      str_upper_id = getUserCodeId(codegen, sg_upper_loop);
    }

    cout << "flatten: " << codegen->_p(sg_loop) << endl;
    void *new_loop = nullptr;
    int ret = loop_flatten_one_level(codegen, sg_loop, &new_loop);
    if (ret != 0) {
      string msg = "Flattening the loop '" + str_upper_label + "' and  '" +
                   str_label + "' " +
                   getUserCodeFileLocation(codegen, sg_loop, true);
#if PROJDEBUG
      cout << msg << endl;
#endif
      dump_critical_message("FLATN", "INFO", msg, 101);
      map<string, string> msg_map;
      msg_map["loop_flatten_removed"] = str_upper_id;
      insertMessage(codegen, new_loop, msg_map);
    }
  }
  return 1;
}

//  Notes:
//  1. flatten current loop with the upper one
//  2. Only flatten if the loop is the only statement in the upper loop
//  3. Only flatten if the loop has a constant bound less than 64
//  4. No declaration in the flattend loops
//  5. The increment steps are one for both loop
int loop_flatten_one_level(CSageCodeGen *codegen, void *sg_loop,
                           void **new_loop) {
  if (codegen->is_perfectly_nested(sg_loop) == 0) {
    return 0;
  }

  CMarsVariable mv(sg_loop, codegen);
  CMarsRangeExpr mr = mv.get_range();

  if (mr.get_lb_set().size() != 1) {
    return 0;
  }
  if (mr.get_ub_set().size() != 1) {
    return 0;
  }
  CMarsExpression me_lb = mr.get_lb(0);
  CMarsExpression me_ub = mr.get_ub(0);
  if (me_lb.IsConstant() == 0) {
    return 0;
  }
  if (me_ub.IsConstant() == 0) {
    return 0;
  }

  void *sg_upper_loop =
      codegen->TraceUpToForStatement(codegen->GetParent(sg_loop));
  CMarsVariable mv_upper(sg_upper_loop, codegen);
  mr = mv_upper.get_range();
  if (mr.get_lb_set().size() != 1) {
    return 0;
  }
  if (mr.get_ub_set().size() != 1) {
    return 0;
  }
  CMarsExpression me_lb_upper = mr.get_lb(0);
  CMarsExpression me_ub_upper = mr.get_ub(0);

  int64_t step_val;
  {
    void *for_loop = sg_upper_loop;
    void *iv;
    void *lb;
    void *ub;
    void *step;
    void *body;
    bool inc_dir;
    bool inclusive_bound;
    int ret = codegen->IsCanonicalForLoop(for_loop, &iv, &lb, &ub, &step, &body,
                                          &inc_dir, &inclusive_bound);
    ret &= codegen->GetLoopStepValueFromExpr(step, &step_val);
    if ((step != nullptr) && (codegen->GetParent(step) == nullptr)) {
      codegen->DeleteNode(step);
    }
    if (ret == 0) {
      return 0;
    }
  }
  if (step_val != 1) {
    return 0;
  }

  int loop_lb = me_lb.GetConstant();
  int loop_ub = me_ub.GetConstant();
  int loop_length = loop_ub - loop_lb + 1;

  //  ////////////////////////////////////////////  /
  if (loop_length > 64) {
    return 0;
  }
  //  ////////////////////////////////////////////  /

  void *sg_bb = codegen->CreateBasicBlock();
  void *iter_upper = codegen->GetLoopIterator(sg_upper_loop);
  void *iter = codegen->GetLoopIterator(sg_loop);
  string str_iter_upper = codegen->_up(iter_upper);
  string str_iter = codegen->_up(iter);
  void *org_iter = iter;
  void *org_iter_upper = iter_upper;

  //  ///////////////////////////////  /
  //  Step1: Handling iterator declaration in the original for loops
  {
    vector<void *> init_var = codegen->get_var_decl_in_for_loop(sg_upper_loop);
    if (init_var.size() > 1) {
      return 0;
    }
    if (init_var.size() == 1 &&
        codegen->UnparseToString(init_var[0]) == str_iter_upper) {
      void *new_decl_upper = codegen->CreateVariableDecl(
          codegen->GetTypeByString("int"), str_iter_upper, nullptr, sg_bb);
      codegen->AppendChild(sg_bb, new_decl_upper);
      org_iter_upper = codegen->GetVariableInitializedName(new_decl_upper);
    } else if (init_var.size() == 1 && init_var[0] != nullptr) {
      return 0;
    }

    init_var.clear();
    init_var = codegen->get_var_decl_in_for_loop(sg_loop);
    if (init_var.size() > 1) {
      return 0;
    }
    if (init_var.size() == 1 &&
        codegen->UnparseToString(init_var[0]) == str_iter) {
      void *new_decl = codegen->CreateVariableDecl(
          codegen->GetTypeByString("int"), str_iter, nullptr, sg_bb);
      codegen->AppendChild(sg_bb, new_decl);
      org_iter = codegen->GetVariableInitializedName(new_decl);
    } else if (init_var.size() == 1 && init_var[0] != nullptr) {
      return 0;
    }
  }

  //  ///////////////////////////////  /
  //  Step2: Create new flattened loop
  void *new_iter_decl;
  void *new_for_loop;
  void *new_body;
  {
    string str_new_iter = "_" + str_iter_upper + "_" + str_iter;

    void *start_exp =
        codegen->CreateExp(V_SgMultiplyOp, me_lb_upper.get_expr_from_coeff(),
                           codegen->CreateConst(loop_length));
    CMarsExpression me_ub_upper_1 = me_ub_upper + 1;
    void *end_exp =
        codegen->CreateExp(V_SgMultiplyOp, me_ub_upper_1.get_expr_from_coeff(),
                           codegen->CreateConst(loop_length));
    end_exp =
        codegen->CreateExp(V_SgSubtractOp, end_exp, codegen->CreateConst(1));

    new_iter_decl = codegen->CreateVariableDecl(
        codegen->GetTypeByString("long"), str_new_iter, nullptr, sg_bb);
    codegen->AppendChild(sg_bb, new_iter_decl);
    void *new_iter = codegen->GetVariableInitializedName(new_iter_decl);
    new_body = codegen->CopyStmt(codegen->GetLoopBody(sg_loop));

    int64_t new_step = 1;

    if (iter_upper != org_iter_upper) {
      codegen->replace_variable_references_in_scope(iter_upper, org_iter_upper,
                                                    new_body);
    }
    if (iter != org_iter) {
      codegen->replace_variable_references_in_scope(iter, org_iter, new_body);
    }

    new_for_loop =
        codegen->CreateStmt(V_SgForStatement, new_iter, start_exp, end_exp,
                            new_body, &new_step, nullptr, sg_loop);
  }

  //  ///////////////////////////////  /
  //  Step3: postprocessing in flattened loop
  void *post_update;
  {
    post_update = codegen->CreateBasicBlock();

    void *incr_2 = codegen->CreateStmt(
        V_SgExprStatement,
        codegen->CreateExp(V_SgPlusPlusOp,
                           codegen->CreateVariableRef(org_iter)));
    codegen->AppendChild(post_update, incr_2);
    void *incr_1_cond = codegen->CreateExp(V_SgGreaterThanOp,
                                           codegen->CreateVariableRef(org_iter),
                                           codegen->CreateConst(loop_ub));
    void *if_body = codegen->CreateBasicBlock();
    void *incr_1 = codegen->CreateStmt(
        V_SgExprStatement,
        codegen->CreateExp(V_SgPlusPlusOp,
                           codegen->CreateVariableRef(org_iter_upper)));
    codegen->AppendChild(if_body, incr_1);

    void *reset_2 = codegen->CreateStmt(
        V_SgExprStatement,
        codegen->CreateExp(V_SgAssignOp, codegen->CreateVariableRef(org_iter),
                           codegen->CreateConst(loop_lb)));
    void *if_reset_2 = codegen->CreateIfStmt(
        codegen->CreateExp(V_SgLessOrEqualOp,
                           codegen->CreateVariableRef(org_iter_upper),
                           me_ub_upper.get_expr_from_coeff()),
        reset_2, nullptr);
    codegen->AppendChild(if_body, if_reset_2);

    void *if_stmt = codegen->CreateIfStmt(incr_1_cond, if_body, nullptr);
    codegen->AppendChild(post_update, if_stmt);
  }
  codegen->AppendChild(new_body, post_update);

  void *initer_upper;
  void *initer;
  {
    void *assign_1 = codegen->CreateExp(
        V_SgAssignOp, codegen->CreateVariableRef(org_iter_upper),
        me_lb_upper.get_expr_from_coeff());
    initer_upper = codegen->CreateStmt(V_SgExprStatement, assign_1);
    void *assign_2 =
        codegen->CreateExp(V_SgAssignOp, codegen->CreateVariableRef(org_iter),
                           me_lb.get_expr_from_coeff());
    initer = codegen->CreateStmt(V_SgExprStatement, assign_2);

    codegen->AppendChild(sg_bb, initer_upper);
    codegen->AppendChild(sg_bb, initer);
  }

  string str_loop = codegen->UnparseToString(sg_loop);
  string str_upper_loop = codegen->UnparseToString(sg_upper_loop);
  cout << str_loop << endl;

  //  ///////////////////////////////  /

  codegen->AddComment("Flattened " + str_upper_loop, new_for_loop);
  codegen->AddComment("Flattened " + str_loop, new_for_loop);

  codegen->AppendChild(sg_bb, new_for_loop);
  *new_loop = new_for_loop;
  //   void *orig_bb = codegen->GetParent(sg_upper_loop);
  //    if (codegen->IsBasicBlock(orig_bb) && codegen->GetChildStmtNum(orig_bb)
  //    == 1)
  //      codegen->ReplaceStmt(orig_bb, sg_bb);
  //    else
  codegen->ReplaceStmt(sg_upper_loop, sg_bb);

  map<string, string> msg_map;
  msg_map[FLATTEN_LOOP] = "on";
  insertMessage(codegen, *new_loop, msg_map);

  return 1;
}
