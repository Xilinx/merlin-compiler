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


#include "interface_transform.h"

static bool is_constant_condition(CSageCodeGen *codegen, void *expr, void *func,
                                  bool *res) {
  if (expr == nullptr) {
    return false;
  }
  if (codegen->IsCompareOp(expr)) {
    void *op0;
    void *op1;
    op0 = op1 = nullptr;
    codegen->GetExpOperand(expr, &op0, &op1);
    int64_t c_op0;
    int64_t c_op1;
    if (!codegen->IsConstantInt(op0, &c_op0, true, func) ||
        !codegen->IsConstantInt(op1, &c_op1, true, func)) {
      return false;
    }
    switch ((static_cast<SgNode *>(expr))->variantT()) {
    case V_SgLessThanOp:
      *res = c_op0 < c_op1;
      break;
    case V_SgLessOrEqualOp:
      *res = c_op0 <= c_op1;
      break;
    case V_SgGreaterThanOp:
      *res = c_op0 > c_op1;
      break;
    case V_SgGreaterOrEqualOp:
      *res = c_op0 >= c_op1;
      break;
    case V_SgEqualityOp:
      *res = c_op0 == c_op1;
      break;
    case V_SgNotEqualOp:
      *res = c_op0 != c_op1;
      break;
    default:
      return false;
      break;
    }
  }
  int64_t c_op;
  if (!codegen->IsConstantInt(expr, &c_op, true, func))
    return false;
  *res = c_op != 0;
  return true;
}

// Dead Code Elimination
int simple_dce(CSageCodeGen *codegen, void *func, set<void *> *visited,
               bool is_top) {
  if (visited->count(func) > 0) {
    return 0;
  }
  visited->insert(func);
  int ret = 0;
  vector<void *> vec_if_stmt;
  codegen->GetNodesByType_int(func, "postorder", V_SgIfStmt, &vec_if_stmt);
  for (auto ifstmt : vec_if_stmt) {
    void *true_body = codegen->GetIfStmtTrueBody(ifstmt);
    void *false_body = codegen->GetIfStmtFalseBody(ifstmt);
    void *condition = codegen->GetIfStmtCondition(ifstmt);
    void *expr = codegen->GetExprFromStmt(condition);
    bool res = false;
    if (!is_constant_condition(codegen, expr, func, &res))
      continue;
    if (res) {
      if (true_body != nullptr) {
        codegen->ReplaceStmt(ifstmt, codegen->CopyStmt(true_body));
      } else {
        codegen->RemoveStmt(ifstmt);
      }
    } else {
      if (false_body != nullptr) {
        codegen->ReplaceStmt(ifstmt, codegen->CopyStmt(false_body));
      } else {
        codegen->RemoveStmt(ifstmt);
      }
    }
    ret = 1;
  }
  vector<void *> vec_while_stmt;
  codegen->GetNodesByType_int(func, "postorder", V_SgWhileStmt,
                              &vec_while_stmt);
  for (auto whilestmt : vec_while_stmt) {
    void *condition = codegen->GetLoopTest(whilestmt);
    void *expr = codegen->GetExprFromStmt(condition);
    bool res = true;
    if (!is_constant_condition(codegen, expr, func, &res))
      continue;
    if (false == res) {
      codegen->RemoveStmt(whilestmt);
      ret = 1;
    }
  }
  vector<void *> vec_dowhile_stmt;
  codegen->GetNodesByType_int(func, "postorder", V_SgDoWhileStmt,
                              &vec_dowhile_stmt);
  for (auto dowhilestmt : vec_dowhile_stmt) {
    void *loop_body = codegen->GetLoopBody(dowhilestmt);
    void *condition = codegen->GetLoopTest(dowhilestmt);
    void *expr = codegen->GetExprFromStmt(condition);
    bool res = true;
    if (!is_constant_condition(codegen, expr, func, &res))
      continue;
    if (false == res) {
      codegen->ReplaceStmt(dowhilestmt, codegen->CopyStmt(loop_body));
      ret = 1;
    }
  }
  vector<void *> vec_for_stmt;
  codegen->GetNodesByType_int(func, "postorder", V_SgForStatement,
                              &vec_for_stmt);
  for (auto for_stmt : vec_for_stmt) {
    int64_t tripcount = 1;
    if (codegen->get_loop_trip_count_simple(for_stmt, &tripcount) &&
        tripcount <= 0) {
      // remove dead loop
      codegen->RemoveStmt(for_stmt);
      ret = 1;
    }
  }

  //  remove double brace to avoid defuse crash
  if (ret != 0) {
    codegen->remove_double_brace(func);
  }

  vector<void *> vec_calls;
  codegen->GetNodesByType(func, "preorder", "SgFunctionCallExp", &vec_calls);
  for (auto call : vec_calls) {
    void *func_decl = codegen->GetFuncDeclByCall(call);
    if (func_decl == nullptr) {
      continue;
    }
    ret |= simple_dce(codegen, func_decl, visited, false);
  }
  return ret;
}

int simple_dce_top(CSageCodeGen *codegen, void *pTopFunc,
                   const CInputOptions &option) {
  printf("    Enter simple dce...\n");
  CMarsIrV2 mars_ir;
  mars_ir.build_mars_ir(codegen, pTopFunc);
  vector<void *> vecTopKernels = mars_ir.get_top_kernels();
  bool Changed = true;
  set<void *> visited_funcs;
  codegen->init_defuse();

  for (auto func : vecTopKernels) {
    Changed |= simple_dce(codegen, func, &visited_funcs, true);
  }
  if (Changed) {
    codegen->init_defuse();
  }
  //  remove dead functions
  bool localChanged;
  do {
    localChanged = false;
    vector<void *> vec_funs;
    codegen->GetNodesByType_int(nullptr, "preorder", V_SgFunctionDeclaration,
                                &vec_funs);
    for (auto func : vec_funs) {
      //  skip the top kernel
      if (mars_ir.is_kernel(func)) {
        continue;
      }
      //  skip template/member/system functions
      if ((codegen->IsTemplateInstantiationFunctionDecl(func) != 0) ||
          (codegen->IsTemplateInstantiationMemberFunctionDecl(func) != 0) ||
          codegen->IsMemberFunction(func) ||
          (codegen->IsTemplateFunctionDecl(func) != 0) ||
          codegen->isWithInHeaderFile(func) ||
          (codegen->GetFuncBody(func) == nullptr)) {
        continue;
      }
      //  skip non-static function
      if (!codegen->IsStatic(func)) {
        continue;
      }
      vector<void *> vec_calls;
      codegen->GetFuncCallsByName(codegen->GetFuncName(func), nullptr,
                                  &vec_calls);
      if (vec_calls.empty()) {
#ifdef PROJDEBUG
        cout << "funciton: " << codegen->_p(func) << " is dead\n";
#endif
        codegen->replace_with_dummy_decl(func);
        Changed = true;
        localChanged = true;
      }
    }
  } while (localChanged);

  return static_cast<int>(Changed);
}
