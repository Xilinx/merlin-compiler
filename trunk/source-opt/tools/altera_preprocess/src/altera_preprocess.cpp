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
//  module           :   altera_preprocess
//  description      :   do some basic preprocess
//                      a. memcpy replace (done)
//                      b. split variable initialization and declaration (not
//                      done)
//  input            :
//  output           :
//  limitation       :
//  author           :   Han
//  ******************************************************************************************//
#include "altera_preprocess.h"
#include <iostream>
#include <string>
#include <tuple>
#include <boost/algorithm/string.hpp>

#include "math.h"
#include "codegen.h"
#include "ir_types.h"
#include "mars_ir_v2.h"
#include "mars_message.h"
#include "mars_opt.h"
#include "postwrap_process.h"
#include "program_analysis.h"
#include "rose.h"
#include "replace_exp_visitor.h"
#include "final_code_gen.h"

using MarsProgramAnalysis::CMarsAccess;
using MarsProgramAnalysis::CMarsExpression;
using MarsProgramAnalysis::CMarsRangeExpr;
using std::tuple;

#define USED_CODE_IN_COVERAGE_TEST 0
extern void GetTLDMInfo_withPointer4(void *sg_node, void *pArg);
//  extern bool doInline(SgFunctionCallExp* funcall, bool allowRecursion);

int altera_simplify_scalar_top(CSageCodeGen *codegen, void *func,
                               set<void *> *visited);

extern vector<void *> get_top_kernels(CSageCodeGen *codegen, void *pTopFunc);

static int count_memcpy = 0;

int generate_nested_loops(CSageCodeGen *codegen, void *sg_array1,
                          const vector<void *> &sg_idx1,
                          const vector<size_t> &dim1, void *sg_array2,
                          const vector<void *> &sg_idx2,
                          const vector<size_t> &dim2, void *sg_length,
                          int type_size, void *func_call, CMarsIrV2 *mars_ir) {
  //  only support one is 1D array and the other is multiple dimension array
  if (dim1.size() > 1 && dim2.size() > 1) {
    return 0;
  }
  if (dim1.size() == 1 && dim2.size() == 1) {
    return 0;
  }
  CMarsExpression length(sg_length, codegen, func_call);
  if (length.IsConstant() == 0) {
    return 0;
  }
  int64_t c_length = length.GetConstant() / type_size;
  int choose_dim1 = 1;
  void *choose_array = sg_array1;
  vector<size_t> choose_dim = dim1;
  if (dim2.size() > 1) {
    choose_dim1 = 0;
    choose_array = sg_array2;
    choose_dim = dim2;
  }

  int leftmost_dim = 0;
  int total_size = 1;
  bool aligned = false;
  while (static_cast<size_t>(leftmost_dim) < choose_dim.size()) {
    total_size *= choose_dim[leftmost_dim];
    if (c_length == total_size) {
      aligned = true;
    } else if (c_length < total_size) {
      //  handle dimension size equal to 1
      leftmost_dim--;
      break;
    }
    leftmost_dim++;
  }
  int rightfirst_dim = static_cast<int>(choose_dim.size()) - leftmost_dim;
  if (static_cast<size_t>(leftmost_dim) == choose_dim.size()) {
    leftmost_dim--;
  }

  if (!aligned) {
    return 0;
  }

  vector<void *> idxs = choose_dim1 != 0 ? sg_idx1 : sg_idx2;
  for (int i = static_cast<int>(idxs.size() - 1), j = 0; j <= leftmost_dim;
       --i, ++j) {
    CMarsExpression idx_exp(idxs[i], codegen, func_call);
    if ((idx_exp != 0) != 0) {
      return 0;
    }
  }

  map<int, int> reorder;
  for (int i = 0; i <= leftmost_dim; ++i) {
    reorder[i] = i;
  }
  {
    bool order_analysis = true;
    bool need_reorder = false;
    vector<int> orders;
    //  analysis the original access order
    void *func_decl = codegen->TraceUpToFuncDecl(func_call);
    vector<void *> all_refs;
    codegen->get_ref_in_scope(choose_array, func_decl, &all_refs);
    bool first = true;
    for (auto ref : all_refs) {
      CMarsAccess ac(ref, codegen, func_decl);
      if (!ac.is_simple_type_v1()) {
        order_analysis = false;
        break;
      }
      auto index_info = ac.simple_type_v1_get_dim_to_var();
      if (index_info.size() != choose_dim.size()) {
        order_analysis = false;
        break;
      }
      vector<int> curr_order(choose_dim.size(), -1);
      set<int> parallel_loop_level;
      void *pos = ref;
      int level = 0;
      while (pos != nullptr) {
        void *curr_loop = codegen->TraceUpToForStatement(pos);
        if (curr_loop == nullptr) {
          break;
        }
        for (size_t i = rightfirst_dim; i != choose_dim.size(); ++i) {
          if (std::get<0>(index_info[i]) == curr_loop) {
            curr_order[i] = level;
            CMarsLoop *loop_info = mars_ir->get_loop_info(curr_loop);
            if (loop_info->is_complete_unroll() != 0) {
              parallel_loop_level.insert(choose_dim.size() - 1 - i);
            }
            level++;
            break;
          }
        }
        pos = codegen->GetParent(curr_loop);
      }

      bool is_all_const = true;
      for (size_t i = rightfirst_dim; i != choose_dim.size(); ++i) {
        if (curr_order[i] != -1) {
          is_all_const = false;
          break;
        }
      }
      if (is_all_const) {
        continue;
      }

      //  need to reorder only if a parallel loop level is in coarse grained
      //  level
      need_reorder |= !parallel_loop_level.empty() &&
                      static_cast<size_t>(*parallel_loop_level.rbegin()) >=
                          parallel_loop_level.size();
      for (size_t i = rightfirst_dim; i != choose_dim.size(); ++i) {
        if (curr_order[i] == -1) {
          order_analysis = false;
          break;
        }
      }
      if (first) {
        orders = curr_order;
        first = false;
      } else if (orders != curr_order) {
        order_analysis = false;
      }

      if (!order_analysis) {
        break;
      }
    }
    if (order_analysis && !first && need_reorder) {
      for (size_t i = rightfirst_dim; i != choose_dim.size(); ++i) {
        reorder[orders[i]] = choose_dim.size() - i - 1;
      }
    }
  }
  void *func_decl = codegen->TraceUpToFuncDecl(func_call);
  void *func_body = codegen->GetFuncBody(func_decl);
  vector<void *> loop_iters;
  vector<void *> loop_uppers;
  string order = my_itoa(count_memcpy);
  for (int i = 0; i <= leftmost_dim; ++i) {
    loop_uppers.push_back(
        codegen->CreateConst(static_cast<int>(choose_dim[i])));
    string iter_name = "_memcpy_i" + order + "_" + my_itoa(i);
    codegen->get_valid_local_name(func_decl, &iter_name);
    void *iter_decl =
        codegen->CreateVariableDecl("int", iter_name, nullptr, func_body);
    codegen->PrependChild(func_body, iter_decl);
    loop_iters.push_back(iter_decl);
  }
  void *new_pntr1 = choose_dim1 != 0 ? sg_array1 : sg_array2;
  void *new_pntr2 = choose_dim1 != 0 ? sg_array2 : sg_array1;
  void *new_index2 =
      codegen->CopyExp(choose_dim1 != 0 ? sg_idx2[0] : sg_idx1[0]);
  void *new_index2_offset = codegen->CreateConst(0);
  for (size_t i = 0; i < choose_dim.size() - 1 - leftmost_dim; ++i) {
    new_pntr1 = codegen->CreateExp(
        V_SgPntrArrRefExp, new_pntr1,
        codegen->CopyExp(choose_dim1 != 0 ? sg_idx1[i] : sg_idx2[i]));
  }
  for (int j = leftmost_dim; j >= 0; --j) {
    new_pntr1 = codegen->CreateExp(V_SgPntrArrRefExp, new_pntr1,
                                   codegen->CreateVariableRef(loop_iters[j]));
    new_index2_offset = codegen->CreateExp(
        V_SgMultiplyOp, new_index2_offset,
        codegen->CreateConst(static_cast<int>(choose_dim[j])));
    new_index2_offset =
        codegen->CreateExp(V_SgAddOp, new_index2_offset,
                           codegen->CreateVariableRef(loop_iters[j]));
  }
  new_index2 = codegen->CreateExp(V_SgAddOp, new_index2, new_index2_offset);
  new_pntr2 = codegen->CreateExp(V_SgPntrArrRefExp, new_pntr2, new_index2);
  void *assign_stmt = codegen->CreateStmt(
      V_SgAssignStatement, choose_dim1 != 0 ? new_pntr1 : new_pntr2,
      choose_dim1 != 0 ? new_pntr2 : new_pntr1);
  void *body = codegen->CreateBasicBlock();
  codegen->AppendChild(body, assign_stmt);
  void *outer_loop = nullptr;
  int loop_level = loop_iters.size();
  for (int i = 0; i != loop_level; ++i) {
    void *iter = loop_iters[reorder[i]];
    void *upper = loop_uppers[reorder[i]];
    void *new_loop = codegen->CreateStmt(
        V_SgForStatement, codegen->GetVariableInitializedName(iter),
        codegen->CreateConst(0), upper, body, nullptr);
    if (static_cast<size_t>(i) + 1 != loop_iters.size()) {
      body = codegen->CreateBasicBlock();
      codegen->AppendChild(body, new_loop);
    }
    outer_loop = new_loop;
  }
  void *parentStmt = codegen->TraceUpByTypeCompatible(func_call, V_SgStatement);
  codegen->ReplaceStmt(parentStmt, outer_loop);
  count_memcpy++;
  return 1;
}

int handle_single_assignment(CSageCodeGen *codegen, void *sg_array1,
                             const vector<void *> &sg_idx1, void *sg_array2,
                             const vector<void *> &sg_idx2, void *sg_length,
                             int type_size, void *func_call) {
  CMarsExpression length(sg_length, codegen, func_call);
  if (length.IsConstant() == 0) {
    return 0;
  }
  int64_t c_length = length.GetConstant() / type_size;
  if (c_length != 1) {
    return 0;
  }
  void *pntr1 = codegen->CreateVariableRef(sg_array1);
  for (auto sg_idx : sg_idx1) {
    pntr1 =
        codegen->CreateExp(V_SgPntrArrRefExp, pntr1, codegen->CopyExp(sg_idx));
  }

  void *pntr2 = codegen->CreateVariableRef(sg_array2);
  for (auto sg_idx : sg_idx2) {
    pntr2 =
        codegen->CreateExp(V_SgPntrArrRefExp, pntr2, codegen->CopyExp(sg_idx));
  }

  void *single_assign_expr = codegen->CreateExp(V_SgAssignOp, pntr1, pntr2);
  codegen->ReplaceExp(func_call, single_assign_expr);
  count_memcpy++;
  return 1;
}

int memcpy_replace(CSageCodeGen *codegen, void *func_call, CMarsIrV2 *mars_ir) {
  void *func_decl = codegen->TraceUpToFuncDecl(func_call);
  void *parentStmt = codegen->TraceUpByTypeCompatible(func_call, V_SgStatement);
  //  void * parentStmt = codegen->TraceUpByTypeCompatible(pTopFunc,
  //  V_SgStatement); printf("[2Han Debug][opencl_gen.cpp]find memcpy function
  //  call\n");
  void *sg_array1 = nullptr;
  void *sg_pntr1 = nullptr;
  vector<void *> sg_idx1;
  int pointer_dim1;
  void *sg_array2 = nullptr;
  void *sg_pntr2 = nullptr;
  vector<void *> sg_idx2;
  int pointer_dim2;
  void *sg_length;

  int ret = codegen->parse_memcpy(func_call, &sg_length, &sg_array1, &sg_pntr1,
                                  &sg_idx1, &pointer_dim1, &sg_array2,
                                  &sg_pntr2, &sg_idx2, &pointer_dim2);
  if (ret == 0) {
    ReplaceMemcpyByBruteForce(codegen, func_call);
#if 0
#if PROJDEBUG
    cout << "cannot inline memcpy " << codegen->_p(func_call) << endl;
#endif
    string info = "    Memcpy: " + codegen->_up(func_call) + " " +
                  getUserCodeFileLocation(
                      &codegen, codegen->TraceUpToStatement(func_call), true) +
                  "\n";
    info += "    In function: " +
            codegen->_up(codegen->TraceUpToFuncDecl(func_call)) + "\n";
    dump_critical_message(FUCIN_ERROR_4(info));
#endif
    return 1;
  }

  /*******************************************************/
  /************check memcpy arguments value***************/
  /*******************************************************/
  //  read destination array
  //  printf("[2Han Debug]dim1 = %d, dim2 = %d\n",pointer_dim1, pointer_dim2);
  //  string sg_length_string = codegen->UnparseToString(sg_length);
  //  printf("[2Han Debug]sg_length = %s\n",sg_length_string.c_str());
  //  string array1_name = codegen->GetVariableName(sg_array1);
  //  string array1_type = codegen->GetVariableTypeName(sg_array1);
  void *array1_type_var = codegen->GetTypebyVar(sg_array1);
  int type_size1 = codegen->get_type_unit_size(array1_type_var, sg_array1);
  //  printf("[2Han Debug]array1_name = %s, array1_type =
  //  %s\n",array1_name.c_str(),array1_type.c_str());
  void *array1_basic_type = nullptr;
  vector<size_t> array1_basic_size;
  codegen->get_type_dimension(array1_type_var, &array1_basic_type,
                              &array1_basic_size, sg_array1);

  void *array2_type_var = codegen->GetTypebyVar(sg_array2);

  void *array2_basic_type = nullptr;
  vector<size_t> array2_basic_size;
  codegen->get_type_dimension(array2_type_var, &array2_basic_type,
                              &array2_basic_size, sg_array2);
  if ((array1_basic_type == nullptr) ||
      codegen->GetBaseType(array1_basic_type, true) !=
          codegen->GetBaseType(array2_basic_type, true)) {
    //    //  FIXME: support general memcpy
    //  #if PROJDEBUG
    //    cout << "cannot inline memcpy " << codegen->_p(func_call)
    //         << " because of different types" << endl;
    //  #endif
    //    string info = "    Memcpy: " + codegen->_up(func_call) + " " +
    //                  getUserCodeFileLocation(
    //                      &codegen, codegen->TraceUpToStatement(func_call),
    //                      true) +
    //                  "\n";
    //    info += "    In function: " +
    //            codegen->_up(codegen->TraceUpToFuncDecl(func_call)) + "\n";
    //    dump_critical_message(FUCIN_ERROR_5(info));
    //    return 0;
    ReplaceMemcpyByBruteForce(codegen, func_call);
    return 1;
  }

  int type_size = type_size1;
  //  For single element memcpy, for loop replacement is not required.
  if (handle_single_assignment(codegen, sg_array1, sg_idx1, sg_array2, sg_idx2,
                               sg_length, type_size, func_call) != 0) {
    return 1;
  }

  //  If the src and dst elements has multiple dimensions, for loops are
  //  replaced on dimension basis. (No guarantee, but try our best).
  if (generate_nested_loops(codegen, sg_array1, sg_idx1, array1_basic_size,
                            sg_array2, sg_idx2, array2_basic_size, sg_length,
                            type_size, func_call, mars_ir) != 0) {
    return 1;
  }

  //  Single for loop starting from this point.
  //  int a[10][20], b[10][20]
  //  memcpy(a[2], b[3], sizeof(int)*20);
  //  sg_idx1 {2,0} # exprs, can be variables
  //  sg_idx2 {3,0}
  //  me_total_offset(dst) = memcpy size(# 362)
  //                   + 2*20 + 0
  int dim1 = array1_basic_size.size();
  int opt_dim1 = dim1;
  //  get optimal dimensions which can be affected during this memcpy in order
  //  to reduce div/mod operation
  if (opt_dim1 > 1) {
    //  if offset + length is less than the lowest dimension,
    int curr_dim = 0;
    CMarsExpression me_total_offset =
        (CMarsExpression(sg_length, codegen, func_call) / type_size);
    int64_t curr_size = 1;
    while (curr_dim < opt_dim1) {
      void *curr_index = sg_idx1[opt_dim1 - 1 - curr_dim];
      CMarsExpression me_curr_offset =
          CMarsExpression(curr_index, codegen, func_call) * curr_size;
      me_total_offset = me_total_offset + me_curr_offset;
      CMarsRangeExpr mr_total_offset = me_total_offset.get_range();
      int64_t c_ub = mr_total_offset.get_const_ub();
      curr_size *= array1_basic_size[curr_dim];
      if (c_ub <= curr_size) {
        opt_dim1 = curr_dim + 1;
        break;
      }
      curr_dim++;
    }
  }
  int dim2 = array2_basic_size.size();
  int opt_dim2 = dim2;
  //  get optimal dimensions which can be affected during this memcpy in order
  //  to reduce div/mod operation
  if (opt_dim2 > 1) {
    //  if offset + length is less than the lowest dimension,
    int curr_dim = 0;
    CMarsExpression me_total_offset =
        (CMarsExpression(sg_length, codegen) / type_size);
    int64_t curr_size = 1;
    while (curr_dim < opt_dim2) {
      void *curr_index = sg_idx2[opt_dim2 - 1 - curr_dim];
      CMarsExpression me_curr_offset =
          CMarsExpression(curr_index, codegen) * curr_size;
      me_total_offset = me_total_offset + me_curr_offset;
      CMarsRangeExpr mr_total_offset = me_total_offset.get_range();
      int64_t c_ub = mr_total_offset.get_const_ub();
      curr_size *= array2_basic_size[curr_dim];
      if (c_ub <= curr_size) {
        opt_dim2 = curr_dim + 1;
        break;
      }
      curr_dim++;
    }
  }

  /****************************************************/
  /******************create for loop*******************/
  /****************************************************/
  //  Example
  //  for (int i = 0; i < 20; i++){
  //   a[((2*20)+i)/20][((2*20)+i)%20] = b[((3*20)+i)/20][((3*20)+i)%20];
  //  }
  assert((size_t)pointer_dim1 == sg_idx1.size() - 1);  //  check pointer_dim
  assert((size_t)pointer_dim2 == sg_idx2.size() - 1);
  string for_index = "__memcpy_i_" + my_itoa(count_memcpy);
  codegen->get_valid_local_name(func_decl, &for_index);
  void *sg_index_i = codegen->CreateVariableDecl(
      "long", for_index, nullptr, codegen->GetFuncBody(func_decl));
  codegen->InsertStmt(sg_index_i, parentStmt);
  void *body = codegen->CreateBasicBlock();
  void *index_name = codegen->GetVariableInitializedName(sg_index_i);

  //  generate leftside of "="
  vector<void *> array1_index;
  void *sg_total_offset1 = codegen->CreateConst(0);
  for (int i = dim1 - opt_dim1; i < dim1; i++) {
    sg_total_offset1 =
        codegen->CreateExp(V_SgMultiplyOp, sg_total_offset1,
                           codegen->CreateConst(static_cast<int>(
                               array1_basic_size[dim1 - 1 - i])));
    sg_total_offset1 = codegen->CreateExp(V_SgAddOp, sg_total_offset1,
                                          codegen->CopyExp(sg_idx1[i]));
  }
  string s_total_offset1 = "total_offset1";
  void *sg_total_offset_decl1 = codegen->CreateVariableDecl(
      "long", s_total_offset1, sg_total_offset1, body);
  codegen->AppendChild(body, sg_total_offset_decl1);
  for (int i = 0; i < dim1; i++) {
    if (i >= dim1 - opt_dim1) {
      array1_index.push_back(codegen->CreateExp(
          V_SgAddOp, codegen->CreateVariableRef(sg_total_offset_decl1),
          codegen->CreateVariableRef(sg_index_i)));
    } else {
      array1_index.push_back(codegen->CopyExp(sg_idx1[i]));
    }
  }

  if (opt_dim1 > 1) {
    for (int i = dim1 - opt_dim1; i < dim1; i++) {
      for (int j = 0; j < dim1 - i; j++) {
        if (0 == array1_basic_size[j]) {
          continue;
        }
        if (j == dim1 - i - 1) {
          if (i > dim1 - opt_dim1) {
            array1_index[i] = codegen->CreateExp(
                V_SgModOp, array1_index[i],
                codegen->CreateConst(static_cast<int>(array1_basic_size[j])));
          }
        } else {
          array1_index[i] = codegen->CreateExp(
              V_SgDivideOp, array1_index[i],
              codegen->CreateConst(static_cast<int>(array1_basic_size[j])));
        }
      }
    }
  }

  void *array1_ref = codegen->CreateArrayRef(
      codegen->CreateVariableRef(sg_array1), array1_index);
  //  string string_array1_exp = codegen->UnparseToString(array1_ref);
  //  printf("[2Han debug]test array1 exp string =
  //  %s\n",string_array1_exp.c_str());

  //  generate rightside of "="

  vector<void *> array2_index;
  void *sg_total_offset2 = codegen->CreateConst(0);
  for (int i = dim2 - opt_dim2; i < dim2; i++) {
    sg_total_offset2 =
        codegen->CreateExp(V_SgMultiplyOp, sg_total_offset2,
                           codegen->CreateConst(static_cast<int>(
                               array2_basic_size[dim2 - i - 1])));
    sg_total_offset2 = codegen->CreateExp(V_SgAddOp, sg_total_offset2,
                                          codegen->CopyExp(sg_idx2[i]));
  }
  string s_total_offset2 = "total_offset2";
  void *sg_total_offset_decl2 = codegen->CreateVariableDecl(
      "long", s_total_offset2, sg_total_offset2, body);
  codegen->AppendChild(body, sg_total_offset_decl2);
  for (int i = 0; i < dim2; i++) {
    if (i >= dim2 - opt_dim2) {
      array2_index.push_back(codegen->CreateExp(
          V_SgAddOp, codegen->CreateVariableRef(sg_total_offset_decl2),
          codegen->CreateVariableRef(sg_index_i)));
    } else {
      array2_index.push_back(codegen->CopyExp(sg_idx2[i]));
    }
  }

  if (opt_dim2 > 1) {
    for (int i = dim2 - opt_dim2; i < dim2; i++) {
      for (int j = 0; j < dim2 - i; j++) {
        if (0 == array2_basic_size[j]) {
          continue;
        }
        if (j == dim2 - i - 1) {
          if (i > dim2 - opt_dim2) {
            array2_index[i] = codegen->CreateExp(
                V_SgModOp, array2_index[i],
                codegen->CreateConst(static_cast<int>(array2_basic_size[j])));
          }
        } else {
          array2_index[i] = codegen->CreateExp(
              V_SgDivideOp, array2_index[i],
              codegen->CreateConst(static_cast<int>(array2_basic_size[j])));
        }
      }
    }
  }

  void *array2_ref = codegen->CreateArrayRef(
      codegen->CreateVariableRef(sg_array2), array2_index);

  //  generate "=" assignment expression
  void *transfer_exp = codegen->CreateExp(V_SgAssignOp, array1_ref, array2_ref);

  //  generate for loop and replace old memcpy function call
  void *sg_length_c =
      codegen->CreateExp(V_SgDivideOp, codegen->CopyExp(sg_length),
                         codegen->CreateConst(type_size));
  CMarsExpression me_length(sg_length, codegen, func_call);
  if (me_length.IsConstant() == 0) {
    int64_t total_dim1 = 1;
    for (int i = 0; i < opt_dim1; ++i) {
      total_dim1 *= array1_basic_size[i];
    }
    int64_t total_dim2 = 1;
    for (int i = 0; i < opt_dim2; ++i) {
      total_dim2 *= array2_basic_size[i];
    }
    int64_t upper = max(total_dim1, total_dim2);
    if (upper > 0) {
      upper =
          min(upper, static_cast<int64_t>(
                         (me_length / type_size).get_range().get_const_ub()));
      const string str_tripcount_pragma = std::string(HLS_PRAGMA) + " " +
                                          HLS_TRIPCOUNT + " " + HLS_MAX + "=" +
                                          my_itoa(upper);
      void *trip_pragma = codegen->CreatePragma(str_tripcount_pragma, body);
      codegen->AppendChild(body, trip_pragma);
    }
  }
  void *for_body_stmt = codegen->CreateStmt(V_SgExprStatement, transfer_exp);
  codegen->AppendChild(body, for_body_stmt);
  void *for_loop_stmt =
      codegen->CreateStmt(V_SgForStatement, index_name,  //  index_type,
                          codegen->CreateConst(0), sg_length_c, body, nullptr);
  codegen->ReplaceStmt(parentStmt, for_loop_stmt);
  count_memcpy++;
  return 1;
}

int altera_memcpy_replace_top(CSageCodeGen *codegen, void *func_call,
                              CMarsIrV2 *mars_ir) {
  string func_name = codegen->GetFuncNameByCall(func_call);
  void *func_decl = codegen->GetFuncDeclByCall(func_call);
  void *func_body = codegen->GetFuncBody(func_decl);
  int ret = 1;
  if (func_body == nullptr) {
    if (func_name == "memcpy") {
      printf("Memcpy transfer\n");
      ret &= memcpy_replace(codegen, func_call, mars_ir);
    }
  } else {
    vector<void *> vecCalls_2;
    codegen->GetNodesByType(func_decl, "preorder", "SgFunctionCallExp",
                            &vecCalls_2);
    for (size_t i = 0; i < vecCalls_2.size(); i++) {
      ret &= altera_memcpy_replace_top(codegen, vecCalls_2[i], mars_ir);
    }
  }
  return ret;
}

int memcpy_replace_top(CSageCodeGen *codegen, void *pTopFunc,
                       const CInputOptions &options) {
  printf("    Enter memcpy replace...\n");
  CMarsIrV2 mars_ir;
  mars_ir.build_mars_ir(codegen, pTopFunc, false, false);
  auto top_kernels = get_top_kernels(codegen, pTopFunc);
  int ret = 1;
  for (auto sg_kernel : top_kernels) {
    vector<void *> vecCalls;
    codegen->GetNodesByType(sg_kernel, "preorder", "SgFunctionCallExp",
                            &vecCalls);

    for (size_t i = 0; i < vecCalls.size(); i++) {
      ret &= altera_memcpy_replace_top(codegen, vecCalls[i], &mars_ir);
    }
  }
  if (ret == 0) {
    throw std::exception();
  }
  return ret;
}

void altera_const_extract_top(CSageCodeGen *codegen, void *scope,
                              set<void *> *visited_scope,
                              set<void *> *used_global_set,
                              vector<void *> *vec_global) {
  if (visited_scope->count(scope) > 0) {
    return;
  }
  visited_scope->insert(scope);
  vector<void *> vec_refs;
  codegen->GetNodesByType(scope, "preorder", "SgVarRefExp", &vec_refs);
  for (size_t i = 0; i < vec_refs.size(); ++i) {
    void *var_init = codegen->GetVariableInitializedName(vec_refs[i]);
    printf("find variable %s\n", codegen->UnparseToString(var_init).c_str());
    if (codegen->IsGlobalInitName(var_init) != 0) {
      void *var_def = codegen->GetVariableDefinitionByName(var_init);
      printf("is global variable %s\n",
             codegen->UnparseToString(var_def).c_str());
      if ((var_def != nullptr) && used_global_set->count(var_def) <= 0) {
        used_global_set->insert(var_def);
        void *initializer = codegen->GetInitializer(var_def);
        if (initializer != nullptr) {
          altera_const_extract_top(codegen, initializer, visited_scope,
                                   used_global_set, vec_global);
        }
        vec_global->push_back(var_def);
      }
    }
  }
  vector<void *> vecCalls_2;
  codegen->GetNodesByType(scope, "preorder", "SgFunctionCallExp", &vecCalls_2);
  for (auto call : vecCalls_2) {
    void *sub_func_decl = codegen->GetFuncDeclByCall(call);
    if (sub_func_decl == nullptr) {
      continue;
    }
    altera_const_extract_top(codegen, sub_func_decl, visited_scope,
                             used_global_set, vec_global);
  }
}

int const_extract(CSageCodeGen *codegen, void *pTopFunc,
                  set<void *> *used_global_set, vector<void *> *vec_globals) {
  printf("    Enter sub const extract...\n");
  vector<void *> vecTopKernels = get_top_kernels(codegen, pTopFunc);
  set<void *> visited_scope;
  for (auto sg_kernel : vecTopKernels) {
    printf("find kernel %s\n", codegen->UnparseToString(sg_kernel).c_str());
    altera_const_extract_top(codegen, sg_kernel, &visited_scope,
                             used_global_set, vec_globals);
  }
  return 0;
}

int const_extract_top(CSageCodeGen *codegen, void *pTopFunc,
                      const CInputOptions &options) {
  FILE *fp = fopen("altera_const.cl", "r");
  if (fp != nullptr) {
    fclose(fp);
    return 0;
  }
  printf("    Enter const extract...\n");

  set<void *> used_global_set;
  vector<void *> vec_globals;
  const_extract(codegen, pTopFunc, &used_global_set, &vec_globals);

  auto vecTopKernels = get_top_kernels(codegen, pTopFunc);
  bool is_cpp_design = false;
  for (auto kernel : vecTopKernels) {
    is_cpp_design |= codegen->isWithInCppFile(kernel);
  }

  string const_file_name = "__merlin_altera_const.";
  const_file_name += (is_cpp_design ? "cpp" : "c");
  {
    string cmd = "rm -rf " + const_file_name;
    system(cmd.c_str());
  }
  void *file_scope = codegen->CreateSourceFile(const_file_name);
  //  In the beginning
  codegen->AddDirectives("#ifndef __ALTERA_CONST_CL__\n", file_scope);
  codegen->AddDirectives("#define __ALTERA_CONST_CL__\n", file_scope);
  void *loc = file_scope;
  for (auto var_init : vec_globals) {
    if (codegen->IsGlobalInitName(var_init) != 0) {
      string file_name = codegen->get_file(var_init);
      printf("variable = %s, file_name = %s\n",
             codegen->UnparseToString(var_init).c_str(), file_name.c_str());
      if (codegen->IsFromInputFile(var_init) != 0) {
        //  printf("[altera_preprocess.cpp]init name =
        //  %s\n",codegen->UnparseToString(var_init).c_str());
#if 0
                string init_name_string = codegen->UnparseToString(var_init);
                void * init_type = codegen->GetTypebyVar(var_init);
                string init_type_string = codegen->GetStringByType(init_type);
                if (init_type_string.find("const") != string::npos) {
                    if (codegen->IsArrayType(init_type)) {
                        void *base_type;
                        vector<int> sizes;
                        codegen->get_type_dimension(init_type,
                                                    base_type, sizes, var_init);
                        string base_type_string =
                          codegen->UnparseToString(base_type);
                        base_type_string = codegen->GetStringByType(base_type);
                        base_type_string = "__constant " + base_type_string;
                        codegen->RegisterType(base_type_string);
                        void *new_array_type =
                          codegen->CreateArrayType(
                              codegen->GetTypeByString(
                                base_type_string), sizes);
                        codegen->SetTypetoVar(var_init, new_array_type);
                    } else {
                        string no_cast_type_string =
                          init_type_string.erase(0, 5);
                        string opencl_type =
                          "__constant " + no_cast_type_string;
                        codegen->RegisterType(opencl_type);
                        codegen->SetTypetoVar(
                            var_init, codegen->GetTypeByString(opencl_type));
                    }
                    void * new_decl = codegen->GetVariableDecl(var_init);
                    codegen->AddDirectives(
                        codegen->UnparseToString(new_decl) + "\n", file_scope);
                    codegen->SetTypetoVar(var_init, init_type);
                }
#else
        string init_name = codegen->GetVariableName(var_init);
        void *init_type = codegen->GetTypebyVar(var_init);
        void *initializer = codegen->GetInitializer(var_init);
        void *var_decl = codegen->GetVariableDecl(var_init);
        if ((codegen->IsConstType(init_type) != 0) &&
            !codegen->IsExtern(var_decl)) {
          void *new_decl = codegen->CreateVariableDecl(
              init_type, init_name,
              initializer != nullptr ? codegen->CopyExp(initializer) : nullptr,
              file_scope);
          codegen->AppendChild(file_scope, new_decl);
          codegen->AddDirectives("\n__constant ", new_decl);
          loc = new_decl;
        }

#endif
      }
    }
  }

  //  copy typedef to another file
  //    vector<void*> vec_def_decl;
  //    codegen->GetNodesByType(nullptr, "preorder",  "SgTypedefDeclaration",
  //    &vec_def_decl); for(auto &type_decl : vec_def_decl) {
  //        if(codegen->GetGlobal(type_decl) == codegen->GetParent(type_decl)) {
  //        //  printf("[altera_preprocess.cpp]type_def1 =
  //        %s\n",codegen->UnparseToString(type_decl).c_str());
  //            string file_name = codegen->get_file(type_decl);
  //            if(codegen->IsFromInputFile(type_decl)) {
  //                //  printf("[altera_preprocess.cpp]type_def2 =
  //                %s\n",codegen->UnparseToString(type_decl).c_str());
  //                codegen->AddDirectives(codegen->UnparseToString(type_decl) +
  //                "\n", file_scope);
  //            }
  //        }
  //    }

  //  In the end
  codegen->AddDirectives("\n#endif", loc, 0);
  return 1;
}

#if USED_CODE_IN_COVERAGE_TEST
void check_single_file(CSageCodeGen *codegen, void *func_call,
                       string top_file_name) {
  string func_name = codegen->GetFuncNameByCall(func_call);
  void *func_decl = codegen->GetFuncDeclByCall(func_call);
  void *func_body = codegen->GetFuncBody(func_decl);

  if (!func_body) {
    //  if no function body, skip
  } else {
    string sub_file_name = codegen->get_file(func_decl);
    if (sub_file_name != top_file_name) {
      printf("right\n");
      //  string msg = " Kernel function and it's sub functions should be in the
      //  same file.\nPlease move \'" + func_name
      //    + "\' in " + sub_file_name + " to " + top_file_name + "\n";
      //  dump_critical_message("PROCS", "ERROR", msg, 315);
    } else {
      printf("wrong\n");
    }
    vector<void *> vecCalls_2;
    codegen->GetNodesByType(func_decl, "preorder", "SgFunctionCallExp",
                            &vecCalls_2);
    for (size_t i = 0; i < vecCalls_2.size(); i++) {
      check_single_file(codegen, vecCalls_2[i], top_file_name);
    }
  }
}
#endif

int kernel_file_list_gen(CSageCodeGen *codegen, void *pTopFunc, int tool = 0) {
  printf("    Enter kernel_file_list_gen...\n");
  string kernel_list_file = "__merlin_kernel_list.h";
  if (test_file_for_read(kernel_list_file) != 0) {
#ifdef PROJ_DEBUG
    cerr << "file " << kernel_list_file << " has existed.\n";
#endif
    return 0;
  }
  {
    string cmd = "rm -rf " + kernel_list_file;
    system(cmd.c_str());
  }
  FILE *fp = fopen(kernel_list_file.c_str(), "w");
  size_t i;
  vector<pair<void *, string>> vecTldmPragmas;
  codegen->TraverseSimple(pTopFunc, "preorder", GetTLDMInfo_withPointer4,
                          &vecTldmPragmas);
  set<string> set_strings;
  //  printf("number = %d\n",vecTldmPragmas.size());
  for (i = 0; i < vecTldmPragmas.size(); i++) {
    string sFilter;
    string sCmd;
    map<string, pair<vector<string>, vector<string>>> mapParams;
    tldm_pragma_parse_whole(vecTldmPragmas[i].second, &sFilter, &sCmd,
                            &mapParams);
    //  printf("Enter func_inline_top sFilter %s\n\n\n",sFilter.c_str());
    //  printf("Enter func_inline_top sCmd %s\n\n\n",sCmd.c_str());
    boost::algorithm::to_upper(sFilter);
    boost::algorithm::to_upper(sCmd);

    if (CMOST_PRAGMA != sFilter && ACCEL_PRAGMA != sFilter &&
        TLDM_PRAGMA != sFilter) {
      continue;
    }
    if (CMOST_KERNEL != sCmd) {
      continue;
    }

    void *sg_kernel = nullptr;
    if (CMOST_TASK == sCmd) {
      void *sg_kernel_call = codegen->find_kernel_call(vecTldmPragmas[i].first);
      assert(sg_kernel_call);
      void *sg_task = sg_kernel_call;
      sg_kernel = codegen->GetFuncDeclByCall(sg_task);
    } else if (CMOST_KERNEL == sCmd) {
      void *next_stmt = codegen->GetNextStmt(vecTldmPragmas[i].first);
      if (codegen->IsFunctionDeclaration(next_stmt) != 0) {
        sg_kernel = next_stmt;
      } else if (codegen->IsExprStatement(next_stmt) != 0) {
        void *sg_call = codegen->GetFuncCallInStmt(next_stmt);
        sg_kernel = codegen->GetFuncDeclByCall(sg_call);
        //  } else {
        //    string msg = "Did not get a function declaration right after
        //    pragma:\n"; msg += "#pragma " + vecTldmPragmas[i].second;
        //    dump_critical_message("KWRAP", "ERROR", msg, 301, 1);
        //    throw std::exception();
      }
    }
    if (sg_kernel == nullptr) {
      continue;
    }

    string func_name = codegen->GetFuncName(sg_kernel);
    string user_kernel_name;
    if (!mapParams["name"].first.empty()) {
      user_kernel_name = mapParams["name"].first[0];
    } else {
      user_kernel_name = func_name;
    }
    string top_file_name = codegen->get_file(sg_kernel);
    string add_string = "//  " + top_file_name + "\n";
    set<string> set_files;
    if (tool == 1) {
      set_files.insert(top_file_name);
    } else {
      set_strings.insert(add_string);
    }
    SetVector<void *> sub_decls;
    SetVector<void *> sub_calls;
    codegen->GetSubFuncDeclsRecursively(sg_kernel, &sub_decls, &sub_calls);
    for (auto sub_decl : sub_decls) {
      if (codegen->GetFuncBody(sub_decl) == nullptr) {
        continue;
      }
      string sub_func_name = codegen->GetFuncName(sub_decl);
      string sub_file_name = codegen->get_file(sub_decl);
      string add_string = "//  " + sub_file_name + "\n";
      if (tool == 1) {
        set_files.insert(sub_file_name);
      } else {
        set_strings.insert(add_string);
      }
    }
    if (tool == 1) {
      string add_string = "//  " + func_name + "=";
      for (auto one_file : set_files) {
        add_string += one_file + " ";
      }
      add_string += "\n";
      set_strings.insert(add_string);
    }
  }
  for (auto add_string : set_strings) {
    //  printf("func_name = %s\n",top_file_name.c_str());
    fprintf(fp, "%s", add_string.c_str());
    //  printf("func call size = %d\n",vecCalls.size());

    //  for (size_t i=0; i < vecCalls.size(); i++) {
    //    check_single_file(codegen, vecCalls[i], top_file_name);
    //  }
  }
  fclose(fp);
  return 1;
}

int split_initializer(CSageCodeGen *codegen, void *func_decl,
                      set<void *> *visited_funcs) {
  if (visited_funcs->count(func_decl) > 0) {
    return 0;
  }
  visited_funcs->insert(func_decl);
  void *func_body = codegen->GetFuncBody(func_decl);
  if (func_body != nullptr) {
    vector<void *> vec_init;
    codegen->GetNodesByType(func_decl, "preorder", "SgInitializedName",
                            &vec_init);
    //  printf("vec_init_size = %d\n",vec_init.size());
    for (auto &var_init : vec_init) {
      if (codegen->IsArgumentInitName(var_init) != 0) {
        continue;
      }
      void *sg_type = codegen->GetTypebyVar(var_init);
      if (codegen->IsArrayType(sg_type) != 0) {
        continue;
      }
      if (!codegen->UnparseToString(codegen->GetInitializer(var_init))
               .empty()) {
#if PROJDEBUG
        printf("init_name = %s\n", codegen->UnparseToString(var_init).c_str());
        printf("initializer = %s\n",
               codegen->UnparseToString(codegen->GetInitializer(var_init))
                   .c_str());
#endif
        void *var_decl = codegen->GetVariableDecl(var_init);

        if (codegen->IsForInitStatement(codegen->GetParent(var_decl)) != 0) {
          continue;
        }
        if (codegen->IsConstType(sg_type) != 0) {
          string orig_type_str = codegen->GetStringByType(sg_type);
          if (orig_type_str.find("const ") == 0) {
            string new_type_str = orig_type_str.substr(strlen("const "));
            void *new_type = codegen->GetTypeByString(new_type_str);
            codegen->SetTypetoVar(var_init, new_type);
          }
        }
        void *initializer = codegen->GetInitializer(var_init);
        void *var_ref = codegen->CreateVariableRef(var_init);
        codegen->SetInitializer(var_init, nullptr);
        void *new_stmt =
            codegen->CreateStmt(V_SgAssignStatement, var_ref,
                                codegen->GetInitializerOperand(initializer));
        codegen->InsertAfterStmt(new_stmt, var_decl);
      }
    }
    vector<void *> vecCalls;
    codegen->GetNodesByType(func_decl, "preorder", "SgFunctionCallExp",
                            &vecCalls);
    for (size_t i = 0; i < vecCalls.size(); i++) {
      void *sub_func = codegen->GetFuncDeclByCall(vecCalls[i]);
      if (sub_func == nullptr) {
        continue;
      }
      split_initializer(codegen, sub_func, visited_funcs);
    }
  }
  return 1;
}

int split_initializer_top(CSageCodeGen *codegen, void *pTopFunc,
                          const CInputOptions &options) {
  printf("    Enter split initializer...\n");
  auto top_kernels = get_top_kernels(codegen, pTopFunc);
  set<void *> visited_funcs;
  for (auto sg_kernel : top_kernels) {
    split_initializer(codegen, sg_kernel, &visited_funcs);
  }
  return 0;
}

#if USED_CODE_IN_COVERAGE_TEST
int altera_value_propagation_top(CSageCodeGen *codegen, void *func,
                                 set<void *> *visited, CMarsIrV2 *mars_ir) {
  if (visited->count(func) > 0)
    return 0;
  visited->insert(func);
  vector<void *> vec_refs;
  codegen->GetNodesByType(func, "preorder", "SgVarRefExp", &vec_refs);
  int ret = 0;
  for (auto &ref : vec_refs) {
    void *var_init = codegen->GetVariableInitializedName(ref);
    if (!var_init)
      continue;
    if (codegen->IsArgumentInitName(var_init))
      continue;
    void *var_type = codegen->GetTypebyVar(var_init);
    if (codegen->IsArrayType(var_type))
      continue;
    if (codegen->is_lvalue(ref))
      continue;
    vector<void *> defs = codegen->GetVarDefbyPosition(var_init, ref);
    if (defs.size() != 1)
      continue;
    void *def_exp = defs[0];
    void *val_exp = nullptr;
    if (codegen->GetInitializer(var_init) == def_exp)
      val_exp = codegen->GetInitializerOperand(var_init);
    else if (codegen->IsAssignOp(def_exp))
      val_exp = codegen->GetExpRightOperand(def_exp);
    if (nullptr == val_exp)
      continue;
    CMarsExpression me(val_exp, codegen);
    if (me.IsConstant()) {
      codegen->ReplaceExp(ref, me.get_expr_from_coeff());
      ret = 1;
      continue;
    }
    if (!mars_ir->is_shared_port(var_init))
      continue;
#if 0
    if (codegen->TraceUpToLoopScope(val_exp) ==
        codegen->TraceUpToLoopScope(ref))
      continue;
    if (codegen->IsPntrArrRefExp(val_exp) ||
        codegen->IsPointerDerefExp(val_exp))
      continue;
#endif

    if (codegen->IsVarRefExp(val_exp)) {
      if (codegen->GetVariableInitializedName(val_exp) == var_init)
        continue;
    }
    if (codegen->is_movable_test(val_exp, ref)) {
      codegen->ReplaceExp(ref, codegen->CopyExp(val_exp));
      ret = 1;
    }
  }
  vector<void *> vec_calls;
  codegen->GetNodesByType(func, "preorder", "SgFunctionCallExp", &vec_calls);
  for (auto &call : vec_calls) {
    void *func_decl = codegen->GetFuncDeclByCall(call);
    if (!func_decl)
      continue;
    ret |= altera_value_propagation_top(codegen, func_decl, visited, mars_ir);
  }
  return ret;
}

int value_propagation_top(CSageCodeGen *codegen, void *pTopFunc,
                          const CInputOptions &options) {
  printf("    Enter value propagation...\n");
  vector<void *> vecTopKernels = get_top_kernels(codegen, pTopFunc);
  bool LocalChanged = false, Changed = true;
  set<void *> visited_funcs;
  codegen->init_defuse();
  CMarsIrV2 mars_ir;
  mars_ir.build_mars_ir(codegen, pTopFunc, false, true);

  while (true) {
    LocalChanged = false;
    visited_funcs.clear();
    for (auto func : vecTopKernels) {
      LocalChanged |=
          altera_value_propagation_top(codegen, func, &visited_funcs, &mars_ir);
    }
    visited_funcs.clear();
    for (auto func : vecTopKernels) {
      LocalChanged |= altera_simplify_scalar_top(codegen, func, &visited_funcs);
    }
    if (LocalChanged) {
      codegen->init_defuse();
      mars_ir.update_ports_for_all_nodes();
    }
    Changed |= LocalChanged;
    if (!LocalChanged)
      break;
  }
  return Changed;
}

int altera_simplify_condition_top(CSageCodeGen *codegen, void *func,
                                  set<void *> *visited) {
  if (visited->count(func) > 0)
    return 0;
  visited->insert(func);
  int ret = 0;
  bool local_changed;
  do {
    local_changed = false;
    vector<void *> vec_if_stmts;
    codegen->GetNodesByType(func, "preorder", "SgIfStmt", &vec_if_stmts);
    for (auto &if_stmt : vec_if_stmts) {
      if (codegen->is_floating_node(if_stmt))
        continue;
      void *condition = codegen->GetIfStmtCondition(if_stmt);
      void *condition_exp = codegen->GetExprFromStmt(condition);
      if (codegen->IsConstantTrue(condition_exp)) {
        void *true_body_copy =
            codegen->CopyStmt(codegen->GetIfStmtTrueBody(if_stmt));
        codegen->ReplaceStmt(if_stmt, true_body_copy);
        local_changed = true;
      } else if (codegen->IsConstantFalse(condition_exp)) {
        void *false_body = codegen->GetIfStmtFalseBody(if_stmt);
        if (!false_body) {
          codegen->RemoveStmt(if_stmt);
        } else {
          void *false_body_copy = codegen->CopyStmt(false_body);
          codegen->ReplaceStmt(if_stmt, false_body_copy);
        }
        local_changed = true;
      } else {
        vector<void *> vec_and_exp;
        codegen->GetExprListFromAndOp(condition_exp, &vec_and_exp);
        bool rewrite = false;
        vector<void *> new_vec_and_exp;
        for (auto &and_exp : vec_and_exp) {
          if (codegen->IsConstantTrue(and_exp)) {
            rewrite = true;
            continue;
          }
          new_vec_and_exp.push_back(and_exp);
        }
        if (rewrite) {
          vec_and_exp.clear();
          for (auto &and_exp : new_vec_and_exp)
            vec_and_exp.push_back(codegen->CopyExp(and_exp));
          codegen->ReplaceExp(condition_exp,
                              codegen->CreateAndOpFromExprList(vec_and_exp));
          local_changed = true;
        }
      }
    }
    if (local_changed)
      ret = 1;
  } while (local_changed);
  vector<void *> vec_calls;
  codegen->GetNodesByType(func, "preorder", "SgFunctionCallExp", &vec_calls);
  for (auto &call : vec_calls) {
    void *func_decl = codegen->GetFuncDeclByCall(call);
    if (!func_decl)
      continue;
    ret |= altera_simplify_condition_top(codegen, func_decl, visited);
  }
  return ret;
}

int simplify_condition_top(CSageCodeGen *codegen, void *pTopFunc,
                           const CInputOptions &options) {
  printf("    Enter condition simplify...\n");
  vector<void *> vecTopKernels = get_top_kernels(codegen, pTopFunc);

  bool Changed = true;
  set<void *> visited_funcs;

  visited_funcs.clear();
  for (auto &func : vecTopKernels) {
    Changed |= altera_simplify_condition_top(codegen, func, &visited_funcs);
  }
  return Changed;
}
#endif

int altera_simplify_scalar_top(CSageCodeGen *codegen, void *func,
                               set<void *> *visited) {
  if (visited->count(func) > 0) {
    return 0;
  }
  visited->insert(func);
  vector<void *> vec_address_ops;
  codegen->GetNodesByType(func, "preorder", "SgAddressOfOp", &vec_address_ops);
  int ret = 0;
  for (auto &address_op : vec_address_ops) {
    void *parent = codegen->GetParent(address_op);
    if (parent != nullptr) {
      if (codegen->IsPointerDerefExp(parent) != 0) {
        void *orig_exp = codegen->GetExpUniOperand(address_op);
        //  change *(&a) -> a
        codegen->ReplaceExp(parent, codegen->CopyExp(orig_exp));
        ret = 1;
      } else if (codegen->IsPntrArrRefExp(parent) != 0) {
        void *index = codegen->GetExpRightOperand(parent);
        CMarsExpression me(index, codegen, index);
        if ((me == 0) != 0) {
          void *orig_exp = codegen->GetExpUniOperand(address_op);
          codegen->ReplaceExp(parent, codegen->CopyExp(orig_exp));
          ret = 1;
        }
      }
    }
  }
  vector<void *> vec_calls;
  codegen->GetNodesByType(func, "preorder", "SgFunctionCallExp", &vec_calls);
  for (auto &call : vec_calls) {
    void *func_decl = codegen->GetFuncDeclByCall(call);
    if (func_decl == nullptr) {
      continue;
    }
    ret |= altera_simplify_scalar_top(codegen, func_decl, visited);
  }
  return ret;
}

int simplify_scalar_top(CSageCodeGen *codegen, void *pTopFunc,
                        const CInputOptions &options) {
  printf("    Enter simplifying scalar access...\n");
  vector<void *> vecTopKernels = get_top_kernels(codegen, pTopFunc);

  bool LocalChanged = false;
  bool Changed = true;
  set<void *> visited_funcs;

  while (true) {
    LocalChanged = false;
    visited_funcs.clear();
    for (auto &func : vecTopKernels) {
      LocalChanged |= altera_simplify_scalar_top(codegen, func, &visited_funcs);
    }
    Changed |= LocalChanged;
    if (!LocalChanged) {
      break;
    }
  }
  return static_cast<int>(Changed);
}

#if USED_CODE_IN_COVERAGE_TEST
int typedef_extract(CSageCodeGen *codegen, void *pTopFunc) {
  printf("    Enter typedef extract...\n");
  string file_name = "merlin_typedef.cpp";
  FILE *fp = fopen(file_name.c_str(), "r");
  if (fp != nullptr) {
    fclose(fp);
    return 0;
  }
  void *file_scope = codegen->CreateSourceFile(file_name);

  //  copy typedef to another file
  vector<void *> vec_def_decl;
  set<string> dumped_decls;
  codegen->GetNodesByType(nullptr, "preorder", "SgTypedefDeclaration",
                          &vec_def_decl);
  for (auto &type_decl : vec_def_decl) {
    if (codegen->GetGlobal(type_decl) == codegen->GetParent(type_decl)) {
      string file_name = codegen->get_file(type_decl);
      if (codegen->IsFromInputFile(type_decl)) {
        string typedef_str = codegen->UnparseToString(type_decl);
        if (dumped_decls.count(typedef_str) > 0)
          continue;
        dumped_decls.insert(typedef_str);
        //  printf("[altera_preprocess.cpp]type_def2 =
        //  %s\n",codegen->UnparseToString(type_decl).c_str());
        codegen->AddDirectives(typedef_str + "\n", file_scope);
      }
    }
  }
}
#endif

int altera_preprocess_top(CSageCodeGen *codegen, void *pTopFunc,
                          const CInputOptions &options) {
  CMarsIrV2 mars_ir;
  mars_ir.build_mars_ir(codegen, pTopFunc, false, true);
  ReplaceExpVisitor visitor(codegen, &mars_ir);
  visitor.EmbedConstInitializers();

  enum effort mEffort = STANDARD;
  string effort_level = options.get_option_key_value("-a", "effort");
  if (effort_level == "low") {
    mEffort = LOW;
  } else if (effort_level == "medium") {
    mEffort = MEDIUM;
  } else if (effort_level == "high") {
    mEffort = HIGH;
  }

  printf("Enter altera preprocess...\n");
  if (!options.get_option_key_value("-a", "naive_hls").empty()) {
    kernel_file_list_gen(codegen, pTopFunc);
  }
#if USED_CODE_IN_COVERAGE_TEST
  if (mEffort >= MEDIUM &&
      "" != options.get_option_key_value("-a", "value_prop")) {
    value_propagation_top(codegen, pTopFunc, options);
    return 1;
  }

  if (mEffort >= MEDIUM &&
      "" != options.get_option_key_value("-a", "condition_simplify")) {
    simplify_condition_top(codegen, pTopFunc, options);
    return 1;
  }
#endif

  if (options.get_option_key_value("-a", "naive_hls").empty()) {
    simplify_scalar_top(codegen, pTopFunc, options);
  } else {
  }

  if ("off" != options.get_option_key_value("-a", "inline_memcpy")) {
    memcpy_replace_top(codegen, pTopFunc, options);
  }

  const_extract_top(codegen, pTopFunc, options);
  //  youxiang 20161027 since we have replaced typedef, we do not need to
  //  extract typedef
  //  typedef_extract(codegen, pTopFunc);

  if (mEffort >= MEDIUM &&
      options.get_option_key_value("-a", "naive_hls").empty()) {
    split_initializer_top(codegen, pTopFunc, options);
  }

  //  YX: 20170908 move to final stage to avoid running time issue
  //  function_uniquify_top(codegen, pTopFunc, options);

  //  check_valid_top(codegen, pTopFunc);

  //    dump_critical_message("ATRCK", "ERROR", "HELLO", 303);
  //    exit(-1);

  return 0;
}
