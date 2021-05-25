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


#include <vector>
#include <iostream>

#include "interface_transform.h"
#include "mars_message.h"

using std::cout;
using std::vector;

void *GetParentStatement(CSageCodeGen *codegen, void *node) {
  void *ret = NULL;
  node = codegen->_pa(node);
  while (node != NULL) {
    if (codegen->IsStatement(node) != 0) {
      ret = node;
      break;
    }
    node = codegen->_pa(node);
  }
  return ret;
}

bool DetectPointerCompare(CSageCodeGen *codegen, void *func_decl) {
  vector<void *> exprs;
  bool all_valid = true;
  codegen->GetNodesByType(func_decl, "preorder", "SgExpression", &exprs, true,
                          true);
  for (auto &expr : exprs) {
    bool valid = true;
    if (codegen->IsBinaryOp(expr)) {
      if (!codegen->IsLessThanOp(expr) && !codegen->IsLessOrEqualOp(expr) &&
          !codegen->IsGreaterThanOp(expr) &&
          !codegen->IsGreaterOrEqualOp(expr) && !codegen->IsEqualOp(expr) &&
          !codegen->IsNonEqualOp(expr) && !codegen->IsAssignOp(expr)) {
        continue;
      }
      void *op_ty = codegen->GetTypeByExp(codegen->GetExpLeftOperand(expr));
      if (codegen->IsPointerType(op_ty) || codegen->IsArrayType(op_ty)) {
        valid = false;
      }
      if (codegen->IsAssignOp(expr)) {
        void *op_l = codegen->GetExpLeftOperand(expr);
        void *op_r = codegen->GetExpRightOperand(expr);
        void *l_type = codegen->GetTypeByExp(op_l);
        if (!codegen->IsBoolType(l_type)) {
          continue;
        }
        if (!codegen->IsCastExp(op_r)) {
          continue;
        }
        void *cast_var = codegen->GetExpUniOperand(op_r);
        if (!codegen->IsVarRefExp(cast_var)) {
          continue;
        }
        void *cast_var_ty = codegen->GetTypeByExp(cast_var);
        if (codegen->IsPointerType(cast_var_ty)) {
          valid = false;
        }
      }
    } else if (codegen->IsUnaryOp(expr)) {
      if (!codegen->IsCastExp(expr))
        continue;
      void *expr_ty = codegen->GetTypeByExp(expr);
      void *uni_operand = codegen->GetExpUniOperand(expr);
      void *uni_op_ty = codegen->GetTypeByExp(uni_operand);
      if (((codegen->IsPointerType(expr_ty) || codegen->IsArrayType(expr_ty)) ^
           (codegen->IsPointerType(uni_op_ty) ||
            codegen->IsArrayType(uni_op_ty))) == 0)
        continue;
      if (codegen->IsValueExp(uni_operand))
        continue;
      // Towards end if an unary exp casts from point_ty to bool
      // Then pointer comparison happens.
      if ((codegen->IsPointerType(uni_op_ty) ||
           codegen->IsArrayType(uni_op_ty)) &&
          codegen->IsBoolType(expr_ty)) {
        valid = false;
      }
    } else if (codegen->IsVarRefExp(expr)) {
      // if (arr) {foo;}. Rose do not insert cast for array inside if stmt
      // comparing to if (ptr) {foo;}.
      // So special handling error out when a varref of array type exists
      // in conditional stmts.
      void *expr_ty = codegen->GetTypeByExp(expr);
      if (!codegen->IsArrayType(expr_ty))
        continue;
      void *parent_expr =
          codegen->TraceUpToExpression(codegen->GetParent(expr));
      void *cond_exp, *true_exp, *false_exp;
      // handle ?: as it is an exp, and can reside anywhere.
      // Not relevant to any statement constraints.
      if (codegen->IsConditionalExp(parent_expr, &cond_exp, &true_exp,
                                    &false_exp)) {
        valid = false;
      } else {
        if (codegen->TraceUpToRootExpression(expr) != expr)
          continue;
        void *parent_stmt = codegen->TraceUpToStatement(expr);
        if (parent_stmt == nullptr || parent_stmt == expr ||
            !codegen->IsExprStatement(parent_stmt) ||
            !codegen->IsUnderConditionStmt(parent_stmt))
          continue;
        // parent_stmt = codegen->GetParent(parent_stmt);
        // void *grand_parent_stmt = codegen->TraceUpToStatement(parent_stmt);
        // if (grand_parent_stmt == nullptr ||
        //     !codegen->IsControlStatement(grand_parent_stmt))
        //   continue;
        valid = false;
      }
    }

    if (!valid) {
      string err_info = "'" + codegen->_up(expr) + "' " +
                        getUserCodeFileLocation(codegen, expr, true);
      dump_critical_message(SYNCHK_ERROR_36(err_info));
      all_valid = false;
    }
  }
  return all_valid;
}

int DetectPointerCompareInterfaceTop(CSageCodeGen *codegen, void *pTopFunc,
                                     const CInputOptions &options) {
  //  if ("sdaccel" != options.get_option_key_value("-a", "impl_tool"))
  //  return 0;
  CMarsIrV2 mars_ir;
  mars_ir.build_mars_ir(codegen, pTopFunc);

  auto kernels = mars_ir.get_top_kernels();
  bool all_valid = true;
  for (auto kernel : kernels) {
    all_valid &= DetectPointerCompare(codegen, kernel);
  }
  if (!all_valid)
    throw std::exception();
  return 0;
}
