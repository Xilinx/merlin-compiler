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


#ifndef TRUNK_SOURCE_OPT_TOOLS_INCLUDE_REPLACE_EXP_VISITOR_H_
#define TRUNK_SOURCE_OPT_TOOLS_INCLUDE_REPLACE_EXP_VISITOR_H_
#include <unordered_set>
#include <unordered_map>

#include "codegen.h"
#include "mars_ir_v2.h"

class ReplaceExpVisitor {
 public:
  enum VisitMode { Replace = 0, ConstCheck, DimCheck, PtrArrayConvert };

  ReplaceExpVisitor(CSageCodeGen *codegen, CMarsIrV2 *p_mars_ir_v2)
      : m_ast(codegen), mars_ir(p_mars_ir_v2) {}
  ~ReplaceExpVisitor() {}

  void EmbedConstInitializers();

 private:
  void ResetErrorFlags();

  void ReplacePtrToArrDecl(SgVariableDeclaration *);

  void CheckConstDecl(SgInitializedName *);

  //  Single purpose use only
  //  May consider promote to codegen, and use template class
  void VisitBinaryOp(SgBinaryOp *);

  void VisitAssignInitializer(SgAssignInitializer *);

  void VisitAggregateInitializer(SgAggregateInitializer *);

  void VisitPntrArrRefExp(SgPntrArrRefExp *);

  void VisitVarRefExp(SgVarRefExp *);

  void VisitValueExp(SgValueExp *);

  void VisitCastExp(SgCastExp *);

  void VisitMinusOp(SgMinusOp *);

  void Visit(SgExpression *exp);

  SgValueExp *ValBinOp(SgValueExp *op_l, SgValueExp *op_r, SgNode *op);

  bool IsConstType(SgNode *op);

  bool IsNumOp(SgNode *op);

  template <typename SgTy, typename ValTy>
  ValTy ValNumCalc(SgTy *op_l, SgTy *op_r, SgNode *op);

  template <typename SgTy, typename ValTy>
  ValTy ValGeneralCalc(SgTy *op_l, SgTy *op_r, SgNode *op);

  template <typename SgTy, typename ValTy>
  ValTy ValHandleNonInt(SgValueExp *op_l, SgValueExp *op_r, SgNode *op);

  template <typename SgTy, typename ValTy>
  ValTy ValHandleInt(SgValueExp *op_l, SgValueExp *op_r, SgNode *op);

  template <typename SgTy, typename ValTy>
  void InvertVal(SgTy *cast_val, int const_type);

  template <typename SgTy, typename ValTy>
  void CastVal(int const_type, SgValueExp **src_val);

  template <typename NodeTy> NodeTy *isa(SgNode *node);

 private:
  CSageCodeGen *m_ast;
  CMarsIrV2 *mars_ir;
  bool is_invalid;
  bool is_non_const;
  bool has_no_initializer;
  bool all_const;
  SgValueExp *val;
  SgExprListExp *arr_list;
  //  bool check_init_only;
  //  bool get_missing_bound;
  int missing_bound;

  VisitMode mode;

  std::unordered_map<void *, void *> ori_exp_map;
};
#endif  // TRUNK_SOURCE_OPT_TOOLS_INCLUDE_REPLACE_EXP_VISITOR_H_
