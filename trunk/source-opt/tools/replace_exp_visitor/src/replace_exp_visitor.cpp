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


#include "replace_exp_visitor.h"
#include "mars_message.h"
using std::unordered_set;
#define IS_INVALID(X)                                                          \
  if (X) {                                                                     \
    is_invalid = true;                                                         \
    return;                                                                    \
  }
#define IS_NON_CONST(X)                                                        \
  if (X) {                                                                     \
    is_non_const = true;                                                       \
    return;                                                                    \
  }
#define HAS_NO_INITIALIZER(X)                                                  \
  if (X) {                                                                     \
    has_no_initializer = true;                                                 \
    return;                                                                    \
  }
#define SHOULD_RET                                                             \
  is_invalid == true || is_non_const == true || has_no_initializer == true

void ReplaceExpVisitor::ResetErrorFlags() {
  has_no_initializer = false;
  is_non_const = false;
  is_invalid = false;
}

void ReplaceExpVisitor::EmbedConstInitializers() {
  auto kernels = mars_ir->get_top_kernels();
  vector<void *> globals = m_ast->GetGlobals();
  vector<void *> all_decls;
  vector<void *> all_expressions;

  //  replace const
  for (auto &global : globals) {
    vector<void *> exprs;
    m_ast->GetNodesByType(global, "postorder", "SgExpression", &exprs, true);
    all_expressions.insert(all_expressions.end(), exprs.begin(), exprs.end());
  }

  //  check non-const in global decls
  for (auto &expr : all_expressions) {
    //  has_no_initializer = false;
    //  is_non_const = false;
    //  is_invalid = false;
    //  get_missing_bound = false;
    //  check_init_only = false;
    ResetErrorFlags();
    mode = Replace;
    if (m_ast->IsInitializer(expr) == 0) {
      continue;
    }
    Visit(static_cast<SgExpression *>(expr));
  }

  //  replace int *arr[a][b] to int arr[a][b][c]
  for (auto &global : globals) {
    vector<void *> decls;
    m_ast->GetNodesByType(global, "preorder", "SgVariableDeclaration", &decls,
                          true);
    all_decls.insert(all_decls.end(), decls.begin(), decls.end());
  }
  for (auto &decl : all_decls) {
    //  has_no_initializer = false;
    //  is_non_const = false;
    //  is_invalid = false;
    //  get_missing_bound = true;
    //  check_init_only = false;
    missing_bound = -1;
    ResetErrorFlags();
    mode = PtrArrayConvert;
    ReplacePtrToArrDecl(static_cast<SgVariableDeclaration *>(decl));
  }

  vector<void *> all_init_names;
  for (auto &global : globals) {
    vector<void *> names;
    m_ast->GetNodesByType(global, "postorder", "SgInitializedName", &names,
                          true);
    all_init_names.insert(all_init_names.end(), names.begin(), names.end());
  }
  for (auto &name : all_init_names) {
    //  has_no_initializer = false;
    //  is_non_const = false;
    //  is_invalid = false;
    //  get_missing_bound = false;
    //  check_init_only = true;
    ResetErrorFlags();
    mode = ConstCheck;
    CheckConstDecl(static_cast<SgInitializedName *>(name));
  }
}

void ReplaceExpVisitor::ReplacePtrToArrDecl(SgVariableDeclaration *decl) {
  SgInitializedNamePtrList &init_name_list = decl->get_variables();
  for (auto iter = init_name_list.begin(); iter != init_name_list.end();
       ++iter) {
    SgInitializedName *init_name = *iter;
    auto *initzer = init_name->get_initializer();
    string var_name = init_name->get_name().getString();
    if ((initzer == nullptr) ||
        (isa<SgAggregateInitializer>(initzer) == nullptr)) {
      continue;
    }
    SgType *var_type = init_name->get_type();
    if (m_ast->IsArrayType(var_type) == 0) {
      continue;
    }

    void *basic_type = NULL;
    vector<size_t> sizes;
    //  what you guys return on []? -1?
    m_ast->get_type_dimension(var_type, &basic_type, &sizes);
    //  only want to handle *[][][][]
    bool skip_flag = false;
    int num_leading_zero = 0;
    int n_sizes = sizes.size();
    for (int i = 0; i < n_sizes - 1; i++) {
      if (sizes[i] == 0) {
        num_leading_zero++;
      }
      if (sizes[i] != 0 && sizes[i + 1] == 0) {
        skip_flag = true;
        break;
      }
    }
    num_leading_zero =
        (sizes[n_sizes - 1] == 0 ? num_leading_zero + 1 : num_leading_zero);
    //  cant think of any valid usage of doing int **arr[4] = {....};
    if (skip_flag || num_leading_zero != 1) {
      continue;
    }

    //  fill right size to the back
    Visit(initzer);
    if (missing_bound == -1) {
      continue;
    }
    reverse(sizes.begin(), sizes.end());
    sizes.pop_back();
    sizes.push_back(missing_bound);
    //  Create new type
    void *new_arr_type = m_ast->CreateArrayType(basic_type, sizes);
    void *new_initzer = m_ast->CopyExp(initzer);
    void *scope = m_ast->GetGlobal(decl);
    void *new_var_decl =
        m_ast->CreateVariableDecl(new_arr_type, var_name, new_initzer, scope);
    //  FIXME : set static brings in extra brackets
    //  example : static const float a_test_5[2][3] = {({(1.00000F), (2.00000F),
    //  (3.1)}), ({(1.00000F), (2.00000F), (3.1)})};
    //  if (m_ast->IsStatic(decl))
    //  m_ast->SetStatic(new_var_decl);
    //  if (m_ast->IsExtern(decl))
    //  m_ast->SetExtern(new_var_decl);
    m_ast->ReplaceStmt(decl, new_var_decl);
  }
}

//  Though I have no idea why this step is necessary
void ReplaceExpVisitor::CheckConstDecl(SgInitializedName *init_name) {
  if (m_ast->IsFromInputFile(init_name) == 0) {
    if (!m_ast->IsTransformation(init_name)) {
      return;
    }
  }
  if (m_ast->IsGlobalInitName(init_name) == 0) {
    return;
  }
  auto *initzer = init_name->get_initializer();
  if (initzer == nullptr) {
    return;
  }
  all_const = true;
  Visit(initzer);
  if (all_const) {
    return;
  }
  string warning_info = " '" + m_ast->_up(init_name) + "' " +
                        getUserCodeFileLocation(m_ast, init_name, true);
  dump_critical_message(PROCS_WARNING_38(warning_info));
}

void ReplaceExpVisitor::VisitBinaryOp(SgBinaryOp *bin_op) {
  if (mode == PtrArrayConvert) {
    missing_bound = -1;
    return;
  }
  if (isSgPntrArrRefExp(static_cast<SgNode *>(bin_op)) != nullptr) {
    VisitPntrArrRefExp(isSgPntrArrRefExp(static_cast<SgNode *>(bin_op)));
    return;
  }
  //  As binary Op should only show up in indexing, we assume it is integer
  //  No logic operation support, don't want to mess things up
  auto *op_l = bin_op->get_lhs_operand();
  auto *op_r = bin_op->get_rhs_operand();
  Visit(op_l);
  SgValueExp *l_val = val;
  Visit(op_r);
  SgValueExp *r_val = val;
  if (SHOULD_RET) {
    return;
  }
  SgValueExp *bin_res = ValBinOp(l_val, r_val, bin_op);
  val = bin_res;
  if (SHOULD_RET) {
    return;
  }
  if (mode == Replace) {
    //  auto *bin_op_ty = m_ast->GetTypeByExp(bin_op);
    //  auto *to_replace = m_ast->CreateCastExp(m_ast->CopyExp(bin_res),
    //  bin_op_ty);
    auto *init_name = m_ast->TraceUpToInitializedName(bin_op);
    IS_INVALID(init_name == NULL);
    auto *org_initzer = m_ast->CopyExp(m_ast->GetInitializer(init_name));
    if (ori_exp_map.count(init_name) == 0U) {
      ori_exp_map[init_name] = org_initzer;
    }
    auto *to_replace = m_ast->CopyExp(bin_res);
    m_ast->ReplaceExp(bin_op, to_replace);
  }
}

void ReplaceExpVisitor::VisitAssignInitializer(SgAssignInitializer *initzer) {
  auto *op = initzer->get_operand();
  Visit(op);
}

void ReplaceExpVisitor::VisitAggregateInitializer(
    SgAggregateInitializer *initzer) {
  SgExprListExp *expr_list = initzer->get_initializers();
  SgExpressionPtrList &expr_ptr_list = expr_list->get_expressions();
  if (mode == PtrArrayConvert) {
    missing_bound = expr_ptr_list.size();
  }
  unordered_set<int> bounds;
  for (auto iter = expr_ptr_list.begin(); iter != expr_ptr_list.end(); ++iter) {
    Visit(*iter);

    if (mode == PtrArrayConvert) {
      if (bounds.empty()) {
        bounds.insert(missing_bound);
      } else if (bounds.count(missing_bound) == 0U) {
        missing_bound = -1;
        //  change back to original
        auto *init_name = m_ast->TraceUpToInitializedName(initzer);
        IS_INVALID(init_name == NULL);
        auto *to_replace = m_ast->CopyExp(ori_exp_map[init_name]);
        m_ast->ReplaceExp(initzer, to_replace);

        return;
      }
    }
  }
  arr_list = expr_list;
}

void ReplaceExpVisitor::VisitPntrArrRefExp(SgPntrArrRefExp *pntr_arr_ref_exp) {
  if (mode == PtrArrayConvert) {
    missing_bound = -1;
    return;
  }
  //  Get VarRef, Get Index, And assemble
  auto *op_l = pntr_arr_ref_exp->get_lhs_operand();
  auto *op_r = pntr_arr_ref_exp->get_rhs_operand();
  Visit(op_r);
  if (SHOULD_RET) {
    return;
  }
  int64_t idx_r;
  IS_INVALID(!m_ast->IsConstantInt(val, &idx_r, true, nullptr));
  Visit(op_l);
  if (SHOULD_RET) {
    return;
  }
  if (mode == Replace) {
    //  Get Value inside of the array
    SgExpressionPtrList arr_contents = arr_list->get_expressions();
    SgExpression *content = arr_contents[idx_r];
    Visit(content);
    //  copy
    //  auto *pntr_arr_ref_exp_ty = m_ast->GetTypeByExp(pntr_arr_ref_exp);
    //  auto *to_replace =
    //     m_ast->CreateCastExp(m_ast->CopyExp(content), pntr_arr_ref_exp_ty);
    auto *init_name = m_ast->TraceUpToInitializedName(pntr_arr_ref_exp);
    IS_INVALID(init_name == NULL);
    auto *org_initzer = m_ast->CopyExp(m_ast->GetInitializer(init_name));
    if (ori_exp_map.count(init_name) == 0U) {
      ori_exp_map[init_name] = org_initzer;
    }
    auto *to_replace = m_ast->CopyExp(content);
    m_ast->ReplaceExp(pntr_arr_ref_exp, to_replace);
  }
}

void ReplaceExpVisitor::VisitVarRefExp(SgVarRefExp *var_ref_exp) {
  if (mode == PtrArrayConvert) {
    missing_bound = -1;
    return;
  }
  IS_INVALID(m_ast->is_write_ref(var_ref_exp));
  //  Check for const modifier
  auto *var_ref_exp_ty = m_ast->GetTypebyVarRef(var_ref_exp);
  void *var_decl = m_ast->GetVariableDecl(var_ref_exp);
  void *init_name = m_ast->GetVariableInitializedName(var_decl);
  void *initzer = m_ast->GetInitializer(init_name);
  if (mode == Replace) {
    IS_NON_CONST(!m_ast->IsConstType(var_ref_exp_ty));
  }
  if (mode == ConstCheck) {
    all_const &= IsConstType(var_ref_exp);
  }

  if ((m_ast->IsAggregateInitializer(initzer) == 0) &&
      (m_ast->IsAssignInitializer(initzer) == 0)) {
    HAS_NO_INITIALIZER(true);
  }
  Visit(static_cast<SgExpression *>(initzer));
  if (SHOULD_RET) {
    return;
  }
  if (mode == Replace) {
    //  Replace aggregate as well, as a bottom up travesal replaces lower level
    //  auto *to_replace =
    //     m_ast->CreateCastExp(m_ast->CopyExp(initzer), var_ref_exp_ty);
    auto *init_name = m_ast->TraceUpToInitializedName(var_ref_exp);
    IS_INVALID(init_name == NULL);
    auto *org_initzer = m_ast->CopyExp(m_ast->GetInitializer(init_name));
    if (ori_exp_map.count(init_name) == 0U) {
      ori_exp_map[init_name] = org_initzer;
    }
    auto *to_replace = m_ast->CopyExp(initzer);
    m_ast->ReplaceExp(var_ref_exp, to_replace);
  }
}

void ReplaceExpVisitor::VisitValueExp(SgValueExp *val_exp) {
  IS_INVALID(!IsConstType(val_exp));
  if (mode == PtrArrayConvert) {
    if (isa<SgStringVal>(val_exp) != nullptr) {
      missing_bound = -1;
      return;
    }
  }
  val = val_exp;
}

//  To play safe, we will not use VisitUnary for now.
//  If we are sure every unary op can use the template below,
//  Promotion to VisitUnary is advisible.
void ReplaceExpVisitor::VisitCastExp(SgCastExp *cast_exp) {
  if (mode == PtrArrayConvert) {
    missing_bound = -1;
    return;
  }
  auto *uni_op = m_ast->GetExpUniOperand(cast_exp);
  Visit(static_cast<SgExpression *>(uni_op));
  //  perform type cast of val
  if (SHOULD_RET) {
    return;
  }
  auto *cast_ty = static_cast<SgNode *>(m_ast->GetTypeByExp(cast_exp));
  if (isa<SgTypeInt>(cast_ty) != nullptr) {
    CastVal<SgIntVal, int>(V_SgIntVal, &val);
  } else if (isa<SgTypeLong>(cast_ty) != nullptr) {
    CastVal<SgLongIntVal, long>(V_SgLongIntVal, &val);
  } else if (isa<SgTypeLongLong>(cast_ty) != nullptr) {
    CastVal<SgLongLongIntVal, int64_t>(V_SgLongLongIntVal, &val);
  } else if (isa<SgTypeShort>(cast_ty) != nullptr) {
    CastVal<SgShortVal, short>(V_SgShortVal, &val);
  } else if (isa<SgTypeUnsignedInt>(cast_ty) != nullptr) {
    CastVal<SgUnsignedIntVal, unsigned int>(V_SgUnsignedIntVal, &val);
  } else if (isa<SgTypeUnsignedLong>(cast_ty) != nullptr) {
    CastVal<SgUnsignedLongVal, unsigned long>(V_SgUnsignedLongVal, &val);
  } else if (isa<SgTypeUnsignedLongLong>(cast_ty) != nullptr) {
    CastVal<SgUnsignedLongLongIntVal, uint64_t>(V_SgUnsignedLongLongIntVal,
                                                &val);
  } else if (isa<SgTypeUnsignedShort>(cast_ty) != nullptr) {
    CastVal<SgUnsignedShortVal, unsigned short>(V_SgUnsignedShortVal, &val);
  } else if (isa<SgTypeFloat>(cast_ty) != nullptr) {
    CastVal<SgFloatVal, float>(V_SgFloatVal, &val);
  } else if (isa<SgTypeDouble>(cast_ty) != nullptr) {
    CastVal<SgDoubleVal, double>(V_SgDoubleVal, &val);
  }
}

void ReplaceExpVisitor::VisitMinusOp(SgMinusOp *minus_op) {
  auto *uni_op = m_ast->GetExpUniOperand(minus_op);
  Visit(static_cast<SgExpression *>(uni_op));
  //  update value stored in val
  if (SHOULD_RET) {
    return;
  }
  if (auto *cast_val = isa<SgIntVal>(val)) {
    InvertVal<SgIntVal, int>(cast_val, V_SgIntVal);
  } else if (auto *cast_val = isa<SgLongIntVal>(val)) {
    InvertVal<SgLongIntVal, long>(cast_val, V_SgLongIntVal);
  } else if (auto *cast_val = isa<SgLongLongIntVal>(val)) {
    InvertVal<SgLongLongIntVal, int64_t>(cast_val, V_SgLongLongIntVal);
    //  ROSE converts to int
    //  else if (auto *cast_val = isa<SgShortVal>(val)) {
    //  InvertVal<SgShortVal, short>(cast_val, V_SgShortVal);
    //  }
  } else if (auto *cast_val = isa<SgUnsignedIntVal>(val)) {
    InvertVal<SgUnsignedIntVal, unsigned int>(cast_val, V_SgUnsignedIntVal);
  } else if (auto *cast_val = isa<SgUnsignedLongVal>(val)) {
    InvertVal<SgUnsignedLongVal, unsigned long>(cast_val, V_SgUnsignedLongVal);
  } else if (auto *cast_val = isa<SgUnsignedLongLongIntVal>(val)) {
    InvertVal<SgUnsignedLongLongIntVal, uint64_t>(cast_val,
                                                  V_SgUnsignedLongLongIntVal);
    //  ROSE converts to int
    //  else if (auto *cast_val = isa<SgUnsignedShortVal>(val)) {
    //  InvertVal<SgUnsignedShortVal, unsigned short>(cast_val,
    //  V_SgUnsignedShortVal);
    //  }
  } else if (auto *cast_val = isa<SgFloatVal>(val)) {
    InvertVal<SgFloatVal, float>(cast_val, V_SgFloatVal);
  } else if (auto *cast_val = isa<SgDoubleVal>(val)) {
    InvertVal<SgDoubleVal, double>(cast_val, V_SgDoubleVal);
  }
}

void ReplaceExpVisitor::Visit(SgExpression *exp) {
  if (auto *bin_op = isSgBinaryOp(static_cast<SgNode *>(exp))) {
    VisitBinaryOp(bin_op);
  } else if (auto *initzer = isa<SgAssignInitializer>(exp)) {
    VisitAssignInitializer(initzer);
  } else if (auto *initzer = isa<SgAggregateInitializer>(exp)) {
    VisitAggregateInitializer(initzer);
  } else if (auto *var_ref_exp = isa<SgVarRefExp>(exp)) {
    VisitVarRefExp(var_ref_exp);
  } else if (auto *val_exp = isa<SgValueExp>(exp)) {
    VisitValueExp(val_exp);
  } else if (auto *cast_exp = isa<SgCastExp>(exp)) {
    VisitCastExp(cast_exp);
  } else if (auto *minus_op = isa<SgMinusOp>(exp)) {
    VisitMinusOp(minus_op);
  } else {
    IS_INVALID(true);
  }
}

SgValueExp *ReplaceExpVisitor::ValBinOp(SgValueExp *op_l, SgValueExp *op_r,
                                        SgNode *op) {
  auto *binop_ty = static_cast<SgType *>(m_ast->GetTypeByExp(op));
  if (isa<SgTypeInt>(binop_ty) != nullptr) {
    CastVal<SgIntVal, int>(V_SgIntVal, &op_l);
    CastVal<SgIntVal, int>(V_SgIntVal, &op_r);
    int res = ValHandleInt<SgIntVal, int>(op_l, op_r, op);
    return static_cast<SgValueExp *>(m_ast->CreateConst(&res, V_SgIntVal));
  }
  if (isa<SgTypeLong>(binop_ty) != nullptr) {
    CastVal<SgLongIntVal, long>(V_SgLongIntVal, &op_l);
    CastVal<SgLongIntVal, long>(V_SgLongIntVal, &op_r);
    long res = ValHandleInt<SgLongIntVal, long>(op_l, op_r, op);
    return static_cast<SgValueExp *>(m_ast->CreateConst(&res, V_SgLongIntVal));
    //  uint64_t c = a + b, ROSE told me binop_ty is SgTypeLong.
    //  suppose to be a bug
    //  } else if (isa<SgTypeLongLong>(binop_ty)) {
    //  CastVal<SgLongLongIntVal, int64_t>(V_SgLongLongIntVal, &op_l);
    //  CastVal<SgLongLongIntVal, int64_t>(V_SgLongLongIntVal, &op_r);
    //  int64_t res = ValHandleInt<SgLongLongIntVal, long long>(op_l, op_r,
    //  op); return static_cast<SgValueExp *>(m_ast->CreateConst(&res,
    //  V_SgLongLongIntVal));
    //  ROSE converts short to int
    //  } else if (isa<SgTypeShort>(binop_ty)) {
    //  CastVal<SgShortVal, short>(V_SgShortVal, &op_l);
    //  CastVal<SgShortVal, short>(V_SgShortVal, &op_r);
    //  short res = ValHandleInt<SgShortVal, short>(op_l, op_r, op);
    //  return static_cast<SgValueExp *>(m_ast->CreateConst(&res,
    //  V_SgShortVal));
  }
  if (isa<SgTypeUnsignedInt>(binop_ty) != nullptr) {
    CastVal<SgUnsignedIntVal, unsigned int>(V_SgUnsignedIntVal, &op_l);
    CastVal<SgUnsignedIntVal, unsigned int>(V_SgUnsignedIntVal, &op_r);
    unsigned int res =
        ValHandleInt<SgUnsignedIntVal, unsigned int>(op_l, op_r, op);
    return static_cast<SgValueExp *>(
        m_ast->CreateConst(&res, V_SgUnsignedIntVal));
  }
  if (isa<SgTypeUnsignedLong>(binop_ty) != nullptr) {
    CastVal<SgUnsignedLongVal, unsigned long>(V_SgUnsignedLongVal, &op_l);
    CastVal<SgUnsignedLongVal, unsigned long>(V_SgUnsignedLongVal, &op_r);
    unsigned long res =
        ValHandleInt<SgUnsignedLongVal, unsigned long>(op_l, op_r, op);
    return static_cast<SgValueExp *>(
        m_ast->CreateConst(&res, V_SgUnsignedLongVal));
  }
  if (isa<SgTypeUnsignedLongLong>(binop_ty) != nullptr) {
    CastVal<SgUnsignedLongLongIntVal, uint64_t>(V_SgUnsignedLongLongIntVal,
                                                &op_l);
    CastVal<SgUnsignedLongLongIntVal, uint64_t>(V_SgUnsignedLongLongIntVal,
                                                &op_r);
    uint64_t res =
        ValHandleInt<SgUnsignedLongLongIntVal, uint64_t>(op_l, op_r, op);
    return static_cast<SgValueExp *>(
        m_ast->CreateConst(&res, V_SgUnsignedLongLongIntVal));
    //  ROSE converts short to int
    //  } else if (isa<SgTypeUnsignedShort>(binop_ty)) {
    //  CastVal<SgUnsignedShortVal, unsigned short>(V_SgUnsignedShortVal,
    //  &op_l); CastVal<SgUnsignedShortVal, unsigned
    //  short>(V_SgUnsignedShortVal, &op_r); unsigned short res =
    //      ValHandleInt<SgUnsignedShortVal, unsigned short>(op_l, op_r, op);
    //  return static_cast<SgValueExp *>(m_ast->CreateConst(&res,
    //  V_SgUnsignedShortVal));
    //  Do not calculate float values
    //  } else if (isa<SgTypeFloat>(binop_ty)) {
    //  CastVal<SgFloatVal, float>(V_SgFloatVal, &op_l);
    //  CastVal<SgFloatVal, float>(V_SgFloatVal, &op_r);
    //  float res = ValHandleNonInt<SgFloatVal, float>(op_l, op_r, op);
    //  return (is_invalid ? NULL
    //                     : static_cast<SgValueExp *>(m_ast->CreateConst(&res,)
    //                     V_SgFloatVal));
    //  } else if (isa<SgTypeDouble>(binop_ty)) {
    //  CastVal<SgDoubleVal, double>(V_SgDoubleVal, &op_l);
    //  CastVal<SgDoubleVal, double>(V_SgDoubleVal, &op_r);
    //  double res = ValHandleNonInt<SgDoubleVal, double>(op_l, op_r, op);
    //  return (is_invalid ? NULL
    //                     : static_cast<SgValueExp *>(m_ast->CreateConst(&res,)
    //                     V_SgDoubleVal));
  }
  is_invalid = true;
  return NULL;
}

bool ReplaceExpVisitor::IsConstType(SgNode *op) {
  return isa<SgChar16Val>(op) != NULL || isa<SgChar32Val>(op) != NULL ||
         isa<SgCharVal>(op) != NULL || isa<SgDoubleVal>(op) != NULL ||
         isa<SgFloatVal>(op) != NULL || isa<SgIntVal>(op) != NULL ||
         isa<SgLongDoubleVal>(op) != NULL || isa<SgLongIntVal>(op) != NULL ||
         isa<SgLongLongIntVal>(op) != NULL || isa<SgShortVal>(op) != NULL ||
         isa<SgStringVal>(op) != NULL || isa<SgUnsignedCharVal>(op) != NULL ||
         isa<SgUnsignedIntVal>(op) != NULL ||
         isa<SgUnsignedLongLongIntVal>(op) != NULL ||
         isa<SgUnsignedLongVal>(op) != NULL ||
         isa<SgUnsignedShortVal>(op) != NULL;
}

bool ReplaceExpVisitor::IsNumOp(SgNode *op) {
  return (isa<SgAddOp>(op) != nullptr) || (isa<SgSubtractOp>(op) != nullptr) ||
         (isa<SgMultiplyOp>(op) != nullptr) || (isa<SgDivideOp>(op) != nullptr);
}

template <typename SgTy, typename ValTy>
ValTy ReplaceExpVisitor::ValNumCalc(SgTy *op_l, SgTy *op_r, SgNode *op) {
  if (isa<SgAddOp>(op)) {
    return op_l->get_value() + op_r->get_value();
  }
  if (isa<SgDivideOp>(op)) {
    return op_l->get_value() / op_r->get_value();
  }
  if (isa<SgMultiplyOp>(op)) {
    return op_l->get_value() * op_r->get_value();
  }
  if (isa<SgSubtractOp>(op)) {
    return op_l->get_value() - op_r->get_value();
  }
  assert(0 && "Unreachable");
}

template <typename SgTy, typename ValTy>
ValTy ReplaceExpVisitor::ValGeneralCalc(SgTy *op_l, SgTy *op_r, SgNode *op) {
  if (isa<SgBitAndOp>(op)) {
    return op_l->get_value() & op_r->get_value();
  }
  if (isa<SgBitOrOp>(op)) {
    return op_l->get_value() | op_r->get_value();
  }
  if (isa<SgBitXorOp>(op)) {
    return op_l->get_value() ^ op_r->get_value();
  }
  if (isa<SgLshiftOp>(op)) {
    return op_l->get_value() << op_r->get_value();
  }
  if (isa<SgModOp>(op)) {
    return op_l->get_value() % op_r->get_value();
  }
  if (isa<SgRshiftOp>(op)) {
    return op_l->get_value() >> op_r->get_value();
  } else {
    is_invalid = true;
    return op_l->get_value();
  }
}

template <typename SgTy, typename ValTy>
ValTy ReplaceExpVisitor::ValHandleNonInt(SgValueExp *op_l, SgValueExp *op_r,
                                         SgNode *op) {
  auto *l = isa<SgTy>(op_l);
  auto *r = isa<SgTy>(op_r);
  if (!IsNumOp(op)) {
    is_invalid = true;
    return -1;
  }
  return ValNumCalc<SgTy, ValTy>(l, r, op);
}

template <typename SgTy, typename ValTy>
ValTy ReplaceExpVisitor::ValHandleInt(SgValueExp *op_l, SgValueExp *op_r,
                                      SgNode *op) {
  auto *l = isa<SgTy>(op_l);
  auto *r = isa<SgTy>(op_r);
  return (IsNumOp(op) ? ValNumCalc<SgTy, ValTy>(l, r, op)
                      : ValGeneralCalc<SgTy, ValTy>(l, r, op));
}

template <typename SgTy, typename ValTy>
void ReplaceExpVisitor::InvertVal(SgTy *cast_val, int const_type) {
  ValTy new_val = -(cast_val->get_value());
  val = static_cast<SgTy *>(m_ast->CreateConst(&new_val, const_type));
}

template <typename SgTy, typename ValTy>
void ReplaceExpVisitor::CastVal(int const_type, SgValueExp **src_val) {
  if (auto *ori_val = isa<SgIntVal>(*src_val)) {
    ValTy res = ori_val->get_value();
    *src_val = static_cast<SgValueExp *>(m_ast->CreateConst(&res, const_type));
  } else if (auto *ori_val = isa<SgLongIntVal>(*src_val)) {
    ValTy res = ori_val->get_value();
    *src_val = static_cast<SgValueExp *>(m_ast->CreateConst(&res, const_type));
  } else if (auto *ori_val = isa<SgLongLongIntVal>(*src_val)) {
    ValTy res = ori_val->get_value();
    *src_val = static_cast<SgValueExp *>(m_ast->CreateConst(&res, const_type));
  } else if (auto *ori_val = isa<SgShortVal>(*src_val)) {
    ValTy res = ori_val->get_value();
    *src_val = static_cast<SgValueExp *>(m_ast->CreateConst(&res, const_type));
  } else if (auto *ori_val = isa<SgUnsignedIntVal>(*src_val)) {
    ValTy res = ori_val->get_value();
    *src_val = static_cast<SgValueExp *>(m_ast->CreateConst(&res, const_type));
  } else if (auto *ori_val = isa<SgUnsignedLongVal>(*src_val)) {
    ValTy res = ori_val->get_value();
    *src_val = static_cast<SgValueExp *>(m_ast->CreateConst(&res, const_type));
  } else if (auto *ori_val = isa<SgUnsignedLongLongIntVal>(*src_val)) {
    ValTy res = ori_val->get_value();
    *src_val = static_cast<SgValueExp *>(m_ast->CreateConst(&res, const_type));
  } else if (auto *ori_val = isa<SgUnsignedShortVal>(*src_val)) {
    ValTy res = ori_val->get_value();
    *src_val = static_cast<SgValueExp *>(m_ast->CreateConst(&res, const_type));
  } else if (auto *ori_val = isa<SgFloatVal>(*src_val)) {
    ValTy res = ori_val->get_value();
    *src_val = static_cast<SgValueExp *>(m_ast->CreateConst(&res, const_type));
  } else if (auto *ori_val = isa<SgDoubleVal>(*src_val)) {
    ValTy res = ori_val->get_value();
    *src_val = static_cast<SgValueExp *>(m_ast->CreateConst(&res, const_type));
  }
}

template <typename NodeTy> NodeTy *ReplaceExpVisitor::isa(SgNode *node) {
  assert(node && "Use isa_or_null for possibly nullptrs.");
  if (rose_ClassHierarchyCastTable[node->variantT()]
                                  [NodeTy::static_variant >> 3] &
      (1 << (NodeTy::static_variant & 7))) {
    return static_cast<NodeTy *>(node);
  }
  return NULL;
}
