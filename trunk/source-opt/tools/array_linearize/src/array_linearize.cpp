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


#include "array_linearize.h"
#include "cmdline_parser.h"
#include "file_parser.h"
#include "xml_parser.h"

#include "PolyModel.h"
#include "codegen.h"
#include "mars_opt.h"
#include "tldm_annotate.h"

#include "mars_ir.h"
#include "mars_ir_v2.h"

#include "interface_transform.h"
#include "mars_message.h"
#include "postwrap_process.h"
#include "input_checker.h"
#undef DEBUG
#define DISABLE_DEBUG 1
#if DISABLE_DEBUG
#define DEBUG(stmts)
#else
#define DEBUG(stmts)                                                           \
  do {                                                                         \
    stmts;                                                                     \
  } while (false)
#endif

using std::vector;

using MarsProgramAnalysis::ArrayRangeAnalysis;
using MarsProgramAnalysis::CMarsExpression;
using MarsProgramAnalysis::CMarsRangeExpr;

extern void GetTLDMInfo_withPointer4(void *sg_node, void *pArg);

int confirm_array_dimension(void *sg_init, const vector<size_t> &sizes,
                            const vector<size_t> &depths, void *kernel,
                            CSageCodeGen *ast, bool check_zero_dim) {
  int err = 0;
  size_t total = 1;
  int zero_dim = 0;
  static bool has_generate_opencl_host_library_generation_disabled_tag = false;
  for (size_t i = 0; i < sizes.size(); i++) {
    size_t s = sizes[i];
    if (static_cast<int64_t>(s) <= 0) {
      zero_dim = 1;
    }
    if (s >= MAX_SIZE_THRESHOLD || s * total >= MAX_SIZE_THRESHOLD) {
      string var_info =
          "variable=" + ast->UnparseToString(sg_init) + ", depth=";
      for (auto dim : sizes) {
        var_info += my_itoa(dim) + ", ";
      }
      var_info = var_info.substr(0, var_info.size() - 2);
      var_info += "\n    kernel=" + ast->_up(ast->TraceUpToFuncDecl(sg_init)) +
                  " " + getUserCodeFileLocation(ast, sg_init, true) + "\n";
      string msg =
          "Finding overflowed dimension size of the interface variable : \n" +
          var_info;
      dump_critical_message(MDARR_ERROR_3(var_info), 0);
#ifdef PROJDEBUG
      cout << "ERROR: " << msg << endl;
#endif
      err++;
      break;
    }
    total *= s;
  }

  string var_name = ast->GetVariableName(sg_init);
  string var_info =
      "'" + var_name + "' " + getUserCodeFileLocation(ast, sg_init, true);
  if (check_zero_dim) {
    if (zero_dim != 0) {
      string pragma_info =
          "#pragma ACCEL interface variable=" + var_name + " max_depth=";
      string type_info = ast->GetStringByType(ast->GetTypebyVar(sg_init));
      size_t i = 0;
      for (auto dim : sizes) {
        if (0 == dim) {
          pragma_info += "?";
        }
        if (i < sizes.size() - 1) {
          pragma_info += ",";
        }
        ++i;
      }
      string msg =
          "Cannot determine some of the dimension sizes of the "
          "interface variable \'" +
          var_name + "\' of type \'" + type_info +
          "\'. \n  Hint: please specify the missing dimension sizes in "
          "the following pragma " +
          "and add it to your kernel:\n" + pragma_info;
      dump_critical_message(MDARR_WARNING_1(var_info, type_info, pragma_info));
      if (!has_generate_opencl_host_library_generation_disabled_tag) {
        string cmd_generate_opencl_host_generation_disabled_tag =
            std::string("touch ") + OPENCL_HOST_GENERATION_DISABLED_TAG;
        system(cmd_generate_opencl_host_generation_disabled_tag.c_str());
        has_generate_opencl_host_library_generation_disabled_tag = true;
      }
    }
  }

  //  check out of bound of access
  if (!depths.empty()) {
    void *type = ast->GetTypebyVar(sg_init);
    if ((ast->IsArrayType(type) != 0) || (ast->IsPointerType(type) != 0)) {
      bool empty_read_access = true;
      bool empty_write_access = true;
      bool is_exact_w = true;
      bool is_exact_r = true;
      string r_msg = "";
      string w_msg = "";
      int OutOfBound_write = ast->check_access_write_out_of_bound(
          sg_init, kernel, depths, &empty_write_access, &is_exact_w, &w_msg);
      int OutOfBound_read = ast->check_access_read_out_of_bound(
          sg_init, kernel, depths, &empty_read_access, &is_exact_r, &r_msg);
      if (OutOfBound_write) {
        string port_range_info = var_info + " : " + w_msg;
        string msg = "Finding a possible out-of-bound write access on ";
        msg += "the argument " + port_range_info;
        if (is_exact_w) {
          dump_critical_message(SYNCHK_ERROR_1(port_range_info));
          err++;
        } else {
          dump_critical_message(SYNCHK_WARNING_1(port_range_info));
        }
      }
      if (OutOfBound_read) {
        string port_range_info = var_info + " : " + r_msg;
        string msg = "Finding a possible out-of-bound read access on ";
        msg += "the argument " + port_range_info;
        if (is_exact_r) {
          dump_critical_message(SYNCHK_ERROR_2(port_range_info));
          err++;
        } else {
          dump_critical_message(SYNCHK_WARNING_2(port_range_info));
        }
      }
      if (empty_read_access && empty_write_access) {
        string msg =
            "Finding a task interface variable unused in the kernel function ";
        msg += var_info;
        dump_critical_message(MDARR_WARNING_3(var_info));
      }
    }
  }
  return err;
}

void *create_one_dim_pntr(void *sg_array, void *sg_cast_type,
                          vector<void *> sg_indexes, vector<size_t> sg_dims,
                          vector<void *> offsets, int pointer_dim,
                          CSageCodeGen *ast, void *pos) {
  int dim = sg_dims.size();

  void *sg_type = ast->GetTypebyVar(sg_array);
  void *sg_base_type;
  vector<size_t> dummy_dims;
  ast->get_type_dimension(sg_type, &sg_base_type, &dummy_dims, sg_array);
  for (int i = sg_indexes.size(); i < dim; i++) {
    sg_indexes.push_back(nullptr);
  }
  for (int i = offsets.size(); i < dim; i++) {
    offsets.push_back(nullptr);
  }

  vector<size_t> strides;
  strides.resize(dim);
  size_t size = 1;
  for (size_t i = 0; i < sg_dims.size(); i++) {
    strides[dim - i - 1] = size;
    size *= sg_dims[dim - i - 1];
  }

  void *index = nullptr;
  CMarsExpression me_addr(ast, 0);
  for (int i = 0; i < dim; i++) {
    CMarsExpression me_idx = (sg_indexes[i]) != nullptr
                                 ? CMarsExpression(sg_indexes[i], ast, pos)
                                 : CMarsExpression(ast, 0);
    CMarsExpression me_offset = (offsets[i]) != nullptr
                                    ? CMarsExpression(offsets[i], ast, pos)
                                    : CMarsExpression(ast, 0);
    void *idx = nullptr;
    if (me_idx != 0) {
      idx = ast->CopyExp(sg_indexes[i]);
    }
    void *offset = nullptr;
    if (me_offset != 0) {
      offset = ast->CopyExp(offsets[i]);
    }
    if (idx == nullptr) {
      idx = offset;
    } else if (offset != nullptr) {
      idx = ast->CreateExp(V_SgAddOp, idx, offset);
    }
    if (idx != nullptr && (me_idx + me_offset) != 0) {
      if (strides[i] != 1)
        idx = ast->CreateExp(V_SgMultiplyOp, idx,
                             ast->CreateConst((int64_t)strides[i]));
      if (index == nullptr)
        index = idx;
      else
        index = ast->CreateExp(V_SgAddOp, index, idx);
    }
    me_addr = me_addr + (me_idx + me_offset) * strides[i];
  }
  if (index == nullptr) {
    index = ast->CreateConst(0);
  }

  vector<void *> sg_new_index;
  sg_new_index.push_back(index);
  void *new_pntr =
      ast->CreateArrayRef(ast->CreateVariableRef(sg_array), sg_new_index);

  //  for (int i = pointer_dim; i < dim; i++)
  if (pointer_dim != dim) {
    if ((me_addr == 0) != 0) {
      //  remove the rightmost index
      new_pntr = ast->GetExpLeftOperand(new_pntr);
    } else {
      new_pntr = ast->CreateExp(V_SgAddressOfOp, new_pntr);
    }
  }

  if (sg_base_type != sg_cast_type) {
    if (pointer_dim == dim) {
      //  a[0] -> *(static_cast<type *>( &a[0]))
      new_pntr = ast->CreateExp(V_SgAddressOfOp, new_pntr);
      new_pntr =
          ast->CreateCastExp(new_pntr, ast->CreatePointerType(sg_cast_type));
      new_pntr = ast->CreateExp(V_SgPointerDerefExp, new_pntr);
    } else {
      //  a -> static_cast<type *>( a)
      new_pntr =
          ast->CreateCastExp(new_pntr, ast->CreatePointerType(sg_cast_type));
    }
  }
  return new_pntr;
}

int check_one_dim_pntr(void *sg_array, vector<void *> sg_indexes,
                       vector<size_t> sg_dims, vector<void *> offsets,
                       int pointer_dim, CSageCodeGen *ast, void *pos) {
  int dim = sg_dims.size();

  void *sg_type = ast->GetTypebyVar(sg_array);
  void *sg_base_type;
  vector<size_t> dummy_dims;
  ast->get_type_dimension(sg_type, &sg_base_type, &dummy_dims, sg_array);
  for (int i = sg_indexes.size(); i < dim; i++) {
    sg_indexes.push_back(nullptr);
  }
  for (int i = offsets.size(); i < dim; i++) {
    offsets.push_back(nullptr);
  }

  vector<size_t> strides;
  strides.resize(dim);
  size_t size = 1;
  for (size_t i = 0; i < sg_dims.size(); i++) {
    strides[dim - i - 1] = size;
    size *= sg_dims[dim - i - 1];
  }

  CMarsExpression me_addr(ast, 0);
  for (int i = 0; i < dim; i++) {
    if (strides[i] == 0) {
      CMarsExpression idx = (sg_indexes[i]) != nullptr
                                ? CMarsExpression(sg_indexes[i], ast, pos)
                                : CMarsExpression(ast, 0);
      CMarsExpression offset = (offsets[i]) != nullptr
                                   ? CMarsExpression(offsets[i], ast, pos)
                                   : CMarsExpression(ast, 0);
      if ((idx + offset != 0) != 0) {
        return 0;
      }
    }
  }
  return 1;
}

string print_dim(vector<size_t> sizes) {
  string out;
  for (auto dim : sizes) {
    out += "[" + my_itoa(dim) + "]";
  }
  return out;
}

int preprocess_for_alias(void *sg_init, void *sg_scope, CSageCodeGen *ast,
                         int *err) {
  int ret = 0;
  vector<void *> refs = ast->GetAllRefInScope(sg_init, sg_scope);
  //  standarize pntr
  for (size_t i = refs.size(); i > 0; i--) {
    void *ref = refs[i - 1];
    void *sg_pntr;
    void *sg_array;
    void *sg_base_type;
    vector<void *> sg_indexes;
    int pointer_dim;
    if (ast->parse_standard_pntr_ref_from_array_ref(ref, &sg_array, &sg_pntr,
                                                    &sg_base_type, &sg_indexes,
                                                    &pointer_dim) != 0) {
      continue;
    }
    int valid = ast->standardize_pntr_ref_from_array_ref(ref);
    if (valid == 0) {
      void *func_scope = ast->TraceUpToFuncDecl(ref);
      string str_array_info =
          "    Array: " + ast->_up(ast->GetTypebyVar(sg_array)) + " " +
          ast->_up(sg_array) + " " +
          getUserCodeFileLocation(ast, sg_array, true) + "\n";
      str_array_info +=
          "    Reference: " + ast->_up(ast->TraceUpToStatement(ref)) + " " +
          getUserCodeFileLocation(ast, ref, true) + "\n";
      str_array_info += "    In function: " + ast->_up(func_scope) + " " +
                        getUserCodeFileLocation(ast, func_scope, true) + "\n";
      dump_critical_message(MDARR_ERROR_4(str_array_info));
      ++(*err);
    }
  }
  refs = ast->GetAllRefInScope(sg_init, sg_scope);
  void *sg_base_type;
  void *sg_type = ast->GetTypebyVar(sg_init);
  vector<size_t> dims;
  ast->get_type_dimension(sg_type, &sg_base_type, &dims, sg_init);
  for (auto one_ref : refs) {
    void *sg_pntr;
    void *sg_array;
    vector<void *> sg_indexes;
    int pointer_dim;
    ast->parse_pntr_ref_by_array_ref(one_ref, &sg_array, &sg_pntr, &sg_indexes,
                                     &pointer_dim, one_ref);
    //  youxiang 20170106 do not touch scalar computation
    if (static_cast<size_t>(pointer_dim) == dims.size()) {
      continue;
    }
    //  void *sg_op = ast->GetParent(sg_pntr);
    //  //  ++ => x=x+1
    //  if (ast->IsPlusPlusOp(sg_op)) {
    //  void *sg_new =
    //      ast->CreateExp(V_SgAssignOp, ast->CopyExp(sg_pntr),
    //                     ast->CreateExp(V_SgAddOp, ast->CopyExp(sg_pntr),
    //                                    ast->CreateConst(1)));
    //  int is_suffix = ast->GetUnaryOpMode(sg_op);

    //  if (is_suffix && !ast->IsStatement(ast->GetParent(sg_op))) {
    //    sg_new =
    //        ast->CreateExp(V_SgCommaOpExp, ast->CopyExp(sg_new),
    //                       ast->CreateExp(V_SgSubtractOp,
    //                       ast->CopyExp(sg_pntr),
    //                                      ast->CreateConst(1)));
    //  }
    //  ast->ReplaceExp(sg_op, sg_new);
    //  ret = 1;
    //  }
    //  //  -- => x=x-1
    //  if (ast->IsMinusMinusOp(sg_op)) {
    //  void *sg_new =
    //      ast->CreateExp(V_SgAssignOp, ast->CopyExp(sg_pntr),
    //                     ast->CreateExp(V_SgSubtractOp, ast->CopyExp(sg_pntr),
    //                                    ast->CreateConst(1)));
    //  int is_suffix = ast->GetUnaryOpMode(sg_op);

    //  if (is_suffix && !ast->IsStatement(ast->GetParent(sg_op))) {
    //    sg_new = ast->CreateExp(V_SgCommaOpExp, ast->CopyExp(sg_new),
    //                            ast->CreateExp(V_SgAddOp,
    //                            ast->CopyExp(sg_pntr),
    //                                           ast->CreateConst(1)));
    //  }
    //  ast->ReplaceExp(sg_op, sg_new);
    //  ret = 1;
    //  }

    //  //  += => x = x +
    //  if (ast->IsPlusAssignOp(sg_op) &&
    //    sg_pntr == ast->GetExpLeftOperand(sg_op)) {
    //  void *sg_new = ast->CreateExp(
    //      V_SgAssignOp, ast->CopyExp(sg_pntr),
    //      ast->CreateExp(V_SgAddOp, ast->CopyExp(sg_pntr),
    //                     ast->CopyExp(ast->GetExpRightOperand(sg_op))));

    //  ast->ReplaceExp(sg_op, sg_new);
    //  ret = 1;
    //  }

    //  //  -= => x = x -
    //  if (ast->IsMinusAssignOp(sg_op) &&
    //    sg_pntr == ast->GetExpLeftOperand(sg_op)) {
    //  void *sg_new = ast->CreateExp(
    //      V_SgAssignOp, ast->CopyExp(sg_pntr),
    //      ast->CreateExp(V_SgSubtractOp, ast->CopyExp(sg_pntr),
    //                     ast->CopyExp(ast->GetExpRightOperand(sg_op))));

    //  ast->ReplaceExp(sg_op, sg_new);
    //  ret = 1;
    //  }
  }
  return ret;
}

void update_access_range_intrinsic_after_linearize(CSageCodeGen *ast,
                                                   void *sg_init, void *scope) {
  vector<void *> vec_refs;
  ast->get_ref_in_scope(sg_init, scope, &vec_refs);
  for (auto ref : vec_refs) {
    int arg_idx = ast->GetFuncCallParamIndex(ref);
    if (arg_idx == -1) {
      continue;
    }

    void *func_call = ast->TraceUpToFuncCall(ref);

    string func_name = ast->GetFuncNameByCall(func_call);
    if (func_name.find("__merlin_access_range") != 0) {
      continue;
    }
    int call_args = ast->GetFuncCallParamNum(func_call);
    if (call_args <= 3) {
      continue;
    }
    assert(call_args % 2);
    void *block = ast->TraceUpToBasicBlock(ref);
    string access_range_intrinsic_name = "__merlin_access_range";
    vector<void *> arg_list;
    CMarsExpression flatten_size(ast, 1);
    for (int i = call_args / 2; i >= 1; i--) {
      void *low = ast->GetFuncCallParam(func_call, i * 2 - 1);
      void *high = ast->GetFuncCallParam(func_call, i * 2);
      CMarsExpression dim =
          CMarsExpression(high, ast) - CMarsExpression(low, ast) + 1;
      flatten_size = flatten_size * dim;
    }
    arg_list.push_back(ast->CreateVariableRef(sg_init));
    arg_list.push_back(ast->CreateConst(0));
    arg_list.push_back((flatten_size - 1).get_expr_from_coeff(true));
    void *new_access_range_intrinsic_call =
        ast->CreateFuncCall(access_range_intrinsic_name,
                            ast->GetTypeByString("void"), arg_list, block);
    ast->ReplaceExp(func_call, new_access_range_intrinsic_call);
  }
}

//  Note:
//  Process:
//  1. full access -> linearization
//  2. partial access -> propagation
//  Special Processing:
//  1. for sub_function: if multiple calls exist for a sub_func,
//     the dimension of the array to be linearized must match
//  2. for alias operation
//      ++, --, +=; -=  -> a = a + ...
//  3. ignore alias assignment -> not allowed (FIXME: not handled)
//      x_alias = ... -> x_alias_linearized = ...
//  4. alias selection -> not allowed (FIXME: not handled)
//      x = (cond) ? alias_a : alias_b;
int apply_array_linearize(void *sg_init, vector<size_t> sizes, int cont_flag,
                          vector<void *> offsets, CSageCodeGen *ast,
                          void *sg_alias_pos, set<void *> *changed_decls,
                          map<void *, void *> *changed_parameters, bool top) {
  DEBUG(cout << "\nTransform -- " << ast->_up(sg_init) << "("
             << ast->_up(ast->GetTypebyVar(sg_init)) << ") : ";
        for (auto s
             : sizes) { cout << "[" << my_itoa(s) << "]"; } cout
        << endl);

  static map<void *, vector<size_t>> s_prevous_trans;
  static map<void *, string> s_prevous_alias_pos;
  {
    if (s_prevous_trans.find(sg_init) != s_prevous_trans.end()) {
      vector<size_t> pre_sizes = s_prevous_trans[sg_init];
      int mismatch = 0;
      if (sizes.size() != pre_sizes.size()) {
        mismatch = 1;
      } else {
        //  Fix 1763  ignore the difference of highest dimension
        for (size_t i = 1; i < sizes.size(); i++) {
          if (sizes[i] != pre_sizes[i]) {
            mismatch = 1;
          }
        }
      }
      if (mismatch != 0) {
        string str_pre_position = s_prevous_alias_pos[sg_init];

        string var_info;
        var_info += "    (1): \"" + ast->_up(ast->GetTypebyVar(sg_init)) + " " +
                    ast->_up((sg_init)) + " " +
                    getUserCodeFileLocation(ast, sg_init, true) + +"\" at \"" +
                    str_pre_position + "\" : " + print_dim(pre_sizes) + "\n";
        var_info += "    (2): \"" + ast->_up(ast->GetTypebyVar(sg_init)) + " " +
                    ast->_up((sg_init)) + "\" " +
                    getUserCodeFileLocation(ast, sg_init, true) +
                    string(" at \"") + ast->_up(sg_alias_pos) + " " +
                    getUserCodeFileLocation(ast, sg_alias_pos, true) +
                    +" \" : " + print_dim(sizes) + "\n";
        string msg = "The dimensions of interface array "
                     "variables do not match in multiple "
                     "function calls or alias: \n" +
                     var_info;
        dump_critical_message(MDARR_ERROR_2(var_info), 0);
#ifdef PROJDEBUG
        cout << "ERROR: " << msg << endl;
#endif

        throw std::exception();
      }
      DEBUG(cout << "Variable -- " << ast->_up(sg_init)
                 << ": skip because of duplication" << endl);
      return 0;
    }

    s_prevous_trans[sg_init] = sizes;
    s_prevous_alias_pos[sg_init] =
        ast->_up(sg_alias_pos) + " " +
        getUserCodeFileLocation(ast, sg_alias_pos, true);
  }

  if (sizes.size() < 2 &&
      (ast->IsReferenceType(ast->GetTypebyVar(sg_init)) == 0)) {
    return 0;
  }

  void *sg_scope;
  if (ast->IsArgumentInitName(sg_init) != 0) {
    sg_scope = ast->GetFuncBody(ast->TraceUpToFuncDecl(sg_init));
  } else if (ast->IsLocalInitName(sg_init) != 0) {
    sg_scope = ast->TraceUpByTypeCompatible(sg_init, V_SgScopeStatement);
  } else if (ast->IsGlobalInitName(sg_init) != 0) {
    sg_scope = nullptr;
  } else {
    cout << "Unrecognized init name type in apply_array_linearize()" << endl;
    assert(0);
  }

  //  cout << "Scope" << ast->_p(scope) << endl;
  vector<void *> refs;
  ast->get_ref_in_scope(sg_init, sg_scope, &refs, true);
  int array_dim = sizes.size();

  //  1. Change type for the variable

  void *sg_new_type;
  {
    void *sg_type = ast->GetTypebyVar(sg_init);
    void *sg_base_type;
    vector<size_t> dummy_dims;
    ast->get_type_dimension(sg_type, &sg_base_type, &dummy_dims, sg_scope);
    size_t total_size = 1;
    for (auto size : sizes) {
      total_size *= size;
    }
    if (top && total_size > 0) {
      sg_new_type = ast->CreateArrayType(ast->GetBaseType(sg_base_type, false),
                                         vector<size_t>{total_size});
    } else {
      sg_new_type =
          ast->CreatePointerType(ast->GetBaseType(sg_base_type, false));
    }
    DEBUG(cout << "Variable -- " << ast->_up(sg_init) << ": "
               << ast->_up(sg_type) << " -> " << ast->_up(sg_new_type)
               << " continue=" << my_itoa(cont_flag) << endl);
  }

  //  2. Replace references
  for (size_t i = refs.size(); i > 0; --i) {
    void *one_ref = refs[i - 1];
    void *sg_pntr;
    void *sg_array;
    void *sg_cast_type;
    vector<void *> sg_indexes;
    int pointer_dim;
    int valid = ast->parse_standard_pntr_ref_from_array_ref(
        one_ref, &sg_array, &sg_pntr, &sg_cast_type, &sg_indexes, &pointer_dim,
        one_ref);
    assert(valid);
    void *sg_new_pntr =
        create_one_dim_pntr(sg_array, sg_cast_type, sg_indexes, sizes, offsets,
                            pointer_dim, ast, one_ref);

    if (pointer_dim != array_dim) {
      void *new_array = nullptr;
      void *sg_alias_pos = nullptr;
      vector<size_t> new_dims;
      for (int t = pointer_dim; t < array_dim; t++) {
        new_dims.push_back(sizes[t]);
      }

      //  case 1: int ** a_lias = pntr(a);
      {
        void *var_name;
        void *value_exp;
        if (ast->parse_assign(ast->GetParent(sg_pntr), &var_name, &value_exp) !=
            0) {
          if (value_exp == sg_pntr && (ast->IsInitName(var_name) != 0)) {
            new_array = var_name;
            sg_alias_pos = ast->TraceUpToStatement(sg_pntr);
          }
        }
      }

      //  case 2: func_call(pntr(a));
      {
        int arg_idx = ast->GetFuncCallParamIndex(sg_pntr);
        if (-1 != arg_idx) {
          void *func_call = ast->TraceUpToFuncCall(sg_pntr);
          assert(func_call);
          void *func_decl = ast->GetFuncDeclByCall(func_call);

          if (func_decl != nullptr) {
            new_array = ast->GetFuncParam(func_decl, arg_idx);
            sg_alias_pos = func_call;
          }
        }
      }

      vector<void *> dummy_offset;
      if (new_array != nullptr) {
        bool Changed =
            apply_array_linearize(new_array, new_dims, cont_flag, dummy_offset,
                                  ast, sg_alias_pos, changed_decls,
                                  changed_parameters, false) != 0;
        if (Changed) {
          int arg_idx = ast->GetFuncParamIndex(new_array);
          if (arg_idx != -1) {
            changed_decls->insert(ast->TraceUpToFuncDecl(new_array));
          }
        }
      } else {
        //  FIXME
        cerr << "assertion failed" << __FILE__ << ":" << __LINE__ << '\n';
      }
    }

    DEBUG(cout << "  Reference -- " << ast->_up(sg_pntr) << " -> "
               << ast->_up(sg_new_pntr) << endl);
    ast->ReplaceExp(sg_pntr, sg_new_pntr);
  }
  if (ast->GetFuncParamIndex(sg_init) == -1) {
    ast->SetTypetoVar(sg_init, sg_new_type);
  } else {
    (*changed_parameters)[sg_init] = sg_new_type;
  }

  update_access_range_intrinsic_after_linearize(ast, sg_init, sg_scope);
  return 1;
}

//  return true when find some variable can't be linearize
bool check_array_linearize(void *sg_init, vector<size_t> sizes,
                           set<void *> *visited, CSageCodeGen *ast) {
  if (visited->count(sg_init) > 0) {
    return false;
  }
  visited->insert(sg_init);

  DEBUG(cout << "\nCheck -- " << ast->_up(sg_init) << "("
             << ast->_up(ast->GetTypebyVar(sg_init)) << ") : ";
        for (auto s
             : sizes) { cout << "[" << my_itoa(s) << "]"; } cout
        << endl);

  if (sizes.size() < 2 &&
      (ast->IsReferenceType(ast->GetTypebyVar(sg_init)) == 0)) {
    return false;
  }

  void *sg_scope;
  if (ast->IsArgumentInitName(sg_init) != 0) {
    sg_scope = ast->GetFuncBody(ast->TraceUpToFuncDecl(sg_init));
  } else if (ast->IsLocalInitName(sg_init) != 0) {
    sg_scope = ast->TraceUpByTypeCompatible(sg_init, V_SgScopeStatement);
  } else if (ast->IsGlobalInitName(sg_init) != 0) {
    sg_scope = nullptr;
  } else {
    cout << "Unrecognized init name type in check_array_linearize()" << endl;
    assert(0);
  }

  //  get_ref_in_scope
  //  cout << "Scope" << ast->_p(scope) << endl;
  vector<void *> refs;
  ast->get_ref_in_scope(sg_init, sg_scope, &refs, true);
  int array_dim = sizes.size();

  bool contains_zero_dim = false;
  for (auto dim : sizes) {
    contains_zero_dim |= dim == 0;
  }
  //  1. Change type for the variable

  {
    void *sg_type = ast->GetTypebyVar(sg_init);
    void *sg_base_type;
    vector<size_t> dummy_dims;
    ast->get_type_dimension(sg_type, &sg_base_type, &dummy_dims, sg_scope);

#ifdef PROJDEBUG
    cout << "Variable -- " << ast->_up(sg_init) << ": " << ast->_up(sg_type)
         << endl;
#endif
  }

  bool ret = false;
  //  2. Replace references
  for (size_t i = refs.size(); i > 0; --i) {
    void *one_ref = refs[i - 1];
    void *sg_pntr = nullptr;
    void *sg_array = nullptr;
    void *sg_cast_type = nullptr;
    vector<void *> sg_indexes;
    int pointer_dim;
    int valid = ast->parse_standard_pntr_ref_from_array_ref(
        one_ref, &sg_array, &sg_pntr, &sg_cast_type, &sg_indexes, &pointer_dim,
        one_ref);
    void *func_scope = ast->TraceUpToFuncDecl(one_ref);
    string str_array_info =
        "    Array: " + ast->_up(ast->GetTypebyVar(sg_array)) + " " +
        ast->_up(sg_array) + " " +
        getUserCodeFileLocation(ast, sg_array, true) + "\n";
    str_array_info +=
        "    Reference: " + ast->_up(ast->TraceUpToStatement(one_ref)) + " " +
        getUserCodeFileLocation(ast, one_ref, true) + "\n";
    str_array_info += "    In function: " + ast->_up(func_scope) + +" " +
                      getUserCodeFileLocation(ast, func_scope, true) + "\n";

    if (valid == 0) {
      dump_critical_message(MDARR_ERROR_6(str_array_info));
      return true;
    }
    vector<void *> offsets;
    if (contains_zero_dim &&
        (check_one_dim_pntr(sg_array, sg_indexes, sizes, offsets, pointer_dim,
                            ast, one_ref) == 0)) {
      confirm_array_dimension(sg_array, sizes, vector<size_t>(), nullptr, ast,
                              true);
      return true;
    }
    if (pointer_dim != array_dim) {
      void *new_array = nullptr;
      vector<size_t> new_dims;
      for (int t = pointer_dim; t < array_dim; t++) {
        new_dims.push_back(sizes[t]);
      }

      //  case 1: int ** a_lias = pntr(a);
      {
        void *var_name;
        void *value_exp;
        if (ast->parse_assign(ast->GetParent(sg_pntr), &var_name, &value_exp) !=
            0) {
          if (value_exp == sg_pntr && (ast->IsInitName(var_name) != 0)) {
            new_array = var_name;
          }
        }
      }

      //  case 2: func_call(pntr(a));
      {
        int arg_idx = ast->GetFuncCallParamIndex(sg_pntr);
        if (-1 != arg_idx) {
          void *func_call = ast->TraceUpToFuncCall(sg_pntr);
          assert(func_call);
          void *func_decl = ast->GetFuncDeclByCall(func_call);

          if (func_decl != nullptr) {
            new_array = ast->GetFuncParam(func_decl, arg_idx);
          }
        }
      }

      //  vector<void *> dummy_offset;
      if (new_array != nullptr) {
        ret = ret || check_array_linearize(new_array, new_dims, visited, ast);
      } else if (array_dim - pointer_dim != 1) {
        dump_critical_message(MDARR_ERROR_6(str_array_info));
        ret = true;
      }
    }
  }
  return ret;
}

int preprocessing(void *sg_init, vector<size_t> sizes, CSageCodeGen *ast,
                  int *err) {
  if (sizes.size() < 2) {
    return 0;
  }

  void *sg_scope;
  if (ast->IsArgumentInitName(sg_init) != 0) {
    sg_scope = ast->GetFuncBody(ast->TraceUpToFuncDecl(sg_init));
  } else if (ast->IsLocalInitName(sg_init) != 0) {
    sg_scope = ast->TraceUpByTypeCompatible(sg_init, V_SgScopeStatement);
  } else if (ast->IsGlobalInitName(sg_init) != 0) {
    sg_scope = nullptr;
  } else {
    cout << "Unrecognized init name type in apply_array_linearize()" << endl;
    assert(0);
  }

  //  cout << "Scope" << ast->_p(scope) << endl;
  return preprocess_for_alias(sg_init, sg_scope, ast, err);
}

string get_dim_str(vector<size_t> size, int cont_flag) {
  string ret;
  size_t mult = 1;
  for (auto t : size) {
    ret += (t != 0U ? my_itoa(t) : "") + ",";
    mult *= t;
  }

  int n_size = ret.size();
  string str_non_cont = ret.substr(0, n_size - 1);

  return cont_flag != 0 ? my_itoa(mult) : str_non_cont;
}

//  Note:
//  1. If previous pragma exist, update the info (depth, max_depth)
//  2. If not exist, create a new one
void update_kernel_pragma_after_linearize(void *sg_init, vector<size_t> sizes,
                                          int cont_flag, vector<void *> offsets,
                                          CSageCodeGen *ast, CMarsIrV2 *mars_ir,
                                          void *sg_alias_pos) {
  //  if (sizes.size() < 1) return ;

  void *kernel = ast->TraceUpToFuncDecl(sg_init);

  if (!mars_ir->is_kernel(kernel)) {
    return;
  }

  void *body = ast->GetFuncBody(kernel);
  if (ast->IsBasicBlock(body) == 0) {
    return;
  }

  vector<void *> vec_pragmas;
  ast->GetNodesByType(kernel, "preorder", "SgPragmaDeclaration", &vec_pragmas);

  string str_max_depth = get_dim_str(sizes, 0);
  if (ast->IsScalarType(ast->GetTypebyVar(sg_init)) != 0) {
    str_max_depth = "";
  }

  //  if ("starting_node" == ast->_up(sg_init))
  //  {
  //    printf("HELLO\n");
  //    sg_init = sg_init;
  //  }

  int hit = 0;
  for (auto &pragma : vec_pragmas) {
    string s_var = mars_ir->get_pragma_attribute(pragma, CMOST_variable);
    string s_intf = mars_ir->get_pragma_attribute(pragma, CMOST_interface);
    boost::algorithm::to_upper(s_intf);
    if (s_var == ast->_up(sg_init) && s_intf == CMOST_interface &&
        !str_max_depth.empty()) {
      mars_ir->set_pragma_attribute(&pragma, "max_depth", str_max_depth);
      string str_depth = mars_ir->get_pragma_attribute(pragma, "depth");
      vector<string> vec_depth = str_split(str_depth, ',');
      vector<string> vec_max_depth = str_split(str_max_depth, ',');
      vec_depth.resize(vec_max_depth.size());
      for (size_t i = 0; i < vec_depth.size(); ++i) {
        if (vec_depth[i].empty() || vec_depth[i] == "0") {
          vec_depth[i] = vec_max_depth[i];
        }
      }
      str_depth = str_merge(',', vec_depth);
      mars_ir->set_pragma_attribute(&pragma, "depth", str_depth);
      hit = 1;
    }
  }
  if (hit == 0 && !sizes.empty() && !str_max_depth.empty()) {
    string str_pragma = "ACCEL interface variable=" + ast->_up(sg_init) +
                        " depth=" + str_max_depth +
                        " max_depth=" + str_max_depth;
    ast->PrependChild(body, ast->CreatePragma(str_pragma, body));
  }
}

int get_total_size_from_string(string s_depth) {
  vector<string> v_depth = str_split(s_depth, ",");
  if (v_depth.empty()) {
    return 0;
  }
  int ret = 1;
  for (auto d : v_depth) {
    int n_depth = my_atoi(d);
    if (n_depth == 0) {
      return 0;
    }
    ret *= n_depth;
  }
  return ret;
}

void *get_unique_wrapper_pragma(void *sg_init, CSageCodeGen *ast,
                                CMarsIrV2 *mars_ir) {
  void *kernel = ast->TraceUpToFuncDecl(sg_init);
  string func_name = ast->GetFuncName(kernel);
  if (!mars_ir->is_kernel(kernel)) {
    return nullptr;
  }

  //  func_name = get_wrapper_name_by_kernel(ast, kernel);
  func_name = ast->get_wrapper_name_by_kernel(kernel);
  void *wrapper = ast->GetFuncDeclByName(func_name);
  if (wrapper == nullptr) {
    return nullptr;
  }
  if (ast->GetFuncBody(wrapper) == nullptr) {
    return nullptr;
  }

  vector<void *> vec_pragmas;
  ast->GetNodesByType(wrapper, "preorder", "SgPragmaDeclaration", &vec_pragmas);

  int hit = 0;
  void *sg_pragma = nullptr;
  for (auto &pragma : vec_pragmas) {
    string s_var = mars_ir->get_pragma_attribute(pragma, "port");
    string s_intf = mars_ir->get_pragma_attribute(pragma, "wrapper");
    boost::algorithm::to_lower(s_intf);
    if (s_var == ast->_up(sg_init) && s_intf == "wrapper") {
      if (hit != 0) {
        return nullptr;
      }
      sg_pragma = pragma;
      hit++;
    }
  }

  return sg_pragma;
}

//  sg_init is the kernel argument
void update_wrapper_pragma_after_linearize(
    void *sg_init, vector<size_t> sizes, int cont_flag, vector<void *> offsets,
    CSageCodeGen *ast, CMarsIrV2 *mars_ir, void *sg_alias_pos,
    bool skip_io_analysis) {
  void *kernel = ast->TraceUpToFuncDecl(sg_init);
  vector<void *> vec_pragma = ast->GetUniqueInterfacePragma(sg_init, mars_ir);
  void *pragma = nullptr;
  if (!vec_pragma.empty()) {
    assert(vec_pragma.size() == 1);
    pragma = vec_pragma[0];
  }
  void *pragma1 = get_unique_wrapper_pragma(sg_init, ast, mars_ir);

  if ((pragma == nullptr) || (pragma1 == nullptr)) {
    return;
  }

  //  1. depth and max_depth

  string s_depth = mars_ir->get_pragma_attribute(pragma, "depth");
  //  string s_max_depth = get_dim_str(sizes, cont_flag);
  string s_max_depth = get_dim_str(sizes, 0);
  int n_low_dim_size = 0;
  {
    vector<string> v_depth = str_split(s_depth, ",");
    vector<string> v_max_depth = str_split(s_max_depth, ",");
    int dim = v_max_depth.size();
    for (size_t i = 0; i < v_max_depth.size(); i++) {
      if (i < v_depth.size() && (v_depth[i].empty() || v_depth[i] == "0")) {
        v_depth[i] = v_max_depth[i];
      } else if (i >= v_depth.size()) {
        v_depth.push_back(v_max_depth[i]);
      }
    }

    //  /////////////////////////////  /
    //  Peng Zhang: 20180210
    //  Fixing the issue on return/global types which has a GetAddressOp
    //  on the kenrel call in the wrapper function. In that situation,
    //  the wrapper function do not need to see the inserted leftmost dim
    //  with size of 1.
    {
      //  these two attributes are set in postwrap_proc pass
      string s_scope_type = mars_ir->get_pragma_attribute(pragma, "scope_type");
      boost::algorithm::to_lower(s_scope_type);
      int is_remove_first_one =
          static_cast<int>(s_scope_type == "global" && v_depth.size() >= 2);
      //  if v_depth.size() == 1, there is a special handling on the scalar
      //  already which is for the efficiency purpose

      //  - remove first 1 in v_depth
      if (is_remove_first_one != 0) {
        assert(!v_depth.empty() && v_depth[0] == "1");
        v_depth.erase(v_depth.begin());
        assert(!v_max_depth.empty() && v_max_depth[0] == "1");
        v_max_depth.erase(v_max_depth.begin());
        dim--;
      }
    }

    s_depth = str_merge(',', v_depth);
    s_max_depth = str_merge(',', v_max_depth);

    if (dim > 0) {
      n_low_dim_size = my_atoi(v_depth[dim - 1]);
      if (n_low_dim_size == 0) {
        n_low_dim_size = my_atoi(v_max_depth[dim - 1]);
      }
    }
  }
  if (!s_depth.empty()) {
    mars_ir->set_pragma_attribute(&pragma1, "depth", s_depth);
  }
  if (!s_max_depth.empty()) {
    mars_ir->set_pragma_attribute(&pragma1, "max_depth", s_max_depth);
  }

  //  2. data_type
  void *sg_type = ast->GetTypebyVar(sg_init);
  sg_type = ast->GetBaseType(sg_type, false);
  //  string s_type = ast->_up(sg_type);
  string s_type = ast->GetStringByType(sg_type);
  mars_ir->set_pragma_attribute(&pragma1, "data_type", s_type);

  //  3. io
  string s_io = mars_ir->get_pragma_attribute(pragma, "io");
  if (s_io.empty()) {
    void *type = ast->GetTypebyVar(sg_init);
    int is_scalar = ast->IsScalarType(type);
    int is_argument = ast->IsArgumentInitName(sg_init);
    //  youxiang 20161004
    //  if ((is_scalar && is_argument)) s_io = "RO";
    if (((is_scalar != 0) && (is_argument != 0)) ||
        (ast->IsConstType(type) != 0)) {
      s_io = "RO";
    } else if (skip_io_analysis) {
      s_io = "RW";
    } else {
      ArrayRangeAnalysis aris(sg_init, ast, kernel, ast->GetProject(), false,
                              false);
      int has_read = aris.has_read();
      int has_write = aris.has_write();
      int write_only = 1;
      if (has_read != 0) {
        write_only = 0;
      } else if ((has_write != 0) && aris.GetRangeExprWrite().size() > 1) {
        write_only = 0;
      } else if (has_write != 0) {
        CMarsRangeExpr range_w = aris.GetRangeExprWrite()[0];
        int size = get_total_size_from_string(s_max_depth);
        if (!range_w.is_full_access(size)) {
          write_only = 0;
        }
      }

      s_io = ((has_read == 0) && (has_write == 0))
                 ? "NO"
                 : (has_write == 0) ? "RO" : write_only != 0 ? "WO" : "RW";
    }
  }
  mars_ir->set_pragma_attribute(&pragma1, "io", s_io);

  //  3.1 continue flag
  string s_cont_flag = mars_ir->get_pragma_attribute(pragma, "continue");
  boost::algorithm::to_lower(s_cont_flag);
  if (s_cont_flag == "true" || s_cont_flag == "on") {
    s_cont_flag = "1";
  }
  if (s_cont_flag == "off" || s_cont_flag == "false") {
    s_cont_flag = "0";
  }
  if (s_cont_flag.empty()) {
    s_cont_flag = cont_flag != 0 ? "1" : "0";
  }

  //  ZP: 20170116
  {
    string s_var_wrapper =
        mars_ir->get_pragma_attribute(pragma1, CMOST_variable);
    if (string::npos != s_var_wrapper.find(".")) {
      s_cont_flag = "0";
    }
  }

  if (s_cont_flag == "1" && string::npos != s_depth.find(",")) {
    mars_ir->set_pragma_attribute(&pragma1, "continue", s_cont_flag);
  }

  //  4. copy
#define PCIE_CPU_COPY_THRD 1024
  string s_copy = n_low_dim_size < PCIE_CPU_COPY_THRD ? "true" : "false";
  mars_ir->set_pragma_attribute(&pragma1, "copy", s_copy);

  //  5. buffer_id
  string s_buf_id = mars_ir->get_pragma_attribute(pragma, "buffer_id");
  if (!s_buf_id.empty()) {
    mars_ir->set_pragma_attribute(&pragma1, "buffer_id", s_buf_id);
  }

  //  6. scope_type
  string s_scope_type = mars_ir->get_pragma_attribute(pragma, "scope_type");
  if (!s_scope_type.empty()) {
    mars_ir->set_pragma_attribute(&pragma1, "scope_type", s_scope_type);
  }

  //  additioal
  vector<string> v_attrs;
  v_attrs.push_back("migrate_mode");
  v_attrs.push_back("dummy");
  for (auto at : v_attrs) {
    string s_scope_type = mars_ir->get_pragma_attribute(pragma, at);
    if (!s_scope_type.empty()) {
      mars_ir->set_pragma_attribute(&pragma1, at, s_scope_type);
    }
  }
}

//  this function is defined in preprocess.cpp
// TODO(Min) : Promote to upper module
int function_uniquify_top(CSageCodeGen *codegen, void *pTopFunc,
                          const CInputOptions &options,
                          vector<void *> vec_kernels);

int array_linearize_top(CSageCodeGen *codegen, void *pTopFunc,
                        const CInputOptions &options) {
  printf("Hello Array Linearize ... \n");

  bool pure_kernel_flow =
      options.get_option_key_value("-a", "pure_kernel") == "on";
  string tool_version = options.get_option_key_value("-a", "tool_version");
  bool hls_flow = false;
  if ("vitis_hls" == tool_version || "vivado_hls" == tool_version) {
    hls_flow = true;
  }
  bool has_check_zero_dim =
      test_file_for_read(OPENCL_HOST_GENERATION_DISABLED_TAG) != 0;
  bool skip_io_analysis =
      options.get_option_key_value("-a", "skip_io_analysis") == "on";
  CMarsIr mars_ir_v1;
  mars_ir_v1.get_mars_ir(codegen, pTopFunc, options);

  CMarsIrV2 mars_ir;
  mars_ir.build_mars_ir(codegen, pTopFunc);

  InputChecker instance(codegen, pTopFunc, options);

  auto kernels = mars_ir.get_top_kernels();

  function_uniquify_top(codegen, pTopFunc, options, kernels);

  //  youxiang: 2016-09-22 collect all the pairs of identical declarations
  //  if one declaration has been changed, update other same declarations
  vector<void *> vec_func_decls;
  codegen->GetNodesByType(nullptr, "preorder", "SgFunctionDeclaration",
                          &vec_func_decls);
  map<void *, set<void *>> vec_same_decls;
  vector<void *> vec_decls_with_def;
  for (auto decl : vec_func_decls) {
    if (codegen->GetFuncBody(decl) != nullptr) {
      vec_decls_with_def.push_back(decl);
    }
  }
  for (auto decl : vec_func_decls) {
    if (codegen->GetFuncBody(decl) != nullptr) {
      continue;
    }
    for (auto decl_def : vec_decls_with_def) {
      if (!codegen->isSameFunction(decl, decl_def)) {
        continue;
      }
      vec_same_decls[decl_def].insert(decl);
    }
  }

  //  youxiang 20161010 preprocess
  {
    int any_err = 0;

    for (auto kernel : kernels) {
#ifdef PROJDEBUG
      // cout << "Kernel -- " << codegen->_p(kernel) << endl;
#endif

      vector<set<void *>> ports_alias(codegen->GetFuncParamNum(kernel));
      for (int i = 0; i < codegen->GetFuncParamNum(kernel); ++i) {
        set<void *> visited;
        auto port = codegen->GetFuncParam(kernel, i);
        set<void *> one_port_alias =
            instance.GetPortAlias(port, kernel, &visited);
        ports_alias[i] = one_port_alias;
      }

      for (int i = 0; i < codegen->GetFuncParamNum(kernel); i++) {
        void *sg_name = codegen->GetFuncParam(kernel, i);
        string port_info = "'" + codegen->GetVariableName(sg_name) + "' " +
                           getUserCodeFileLocation(codegen, sg_name, true) +
                           "\n";
        vector<void *> vec_pragma =
            codegen->GetUniqueInterfacePragma(sg_name, &mars_ir);
        string pragma_info;
        for (auto pragma : vec_pragma) {
          pragma_info += codegen->UnparseToString(pragma) + " " +
                         getUserCodeFileLocation(codegen, pragma, true) + "\n";
        }
        void *sg_type = codegen->GetTypebyVar(sg_name);
        void *sg_base_type;
        vector<size_t> dummy_dims;
        codegen->get_type_dimension(sg_type, &sg_base_type, &dummy_dims,
                                    sg_name);

        //  printf("sg_name = %s\n", codegen->UnparseToString(sg_name).c_str());
        vector<string> depths =
            codegen->GetUniqueInterfaceAttribute(sg_name, "depth", &mars_ir);
        vector<string> max_depths = codegen->GetUniqueInterfaceAttribute(
            sg_name, "max_depth", &mars_ir);
        string dim_info = my_itoa(dummy_dims.size());
        if (depths.size() > dummy_dims.size()) {
          string depth_info = str_merge(',', depths);
          //  if (dummy_dims.size() == 0) {  //  if scalar, do not need to set
          //  depth pragma
          //  dump_critical_message(INFTF_WARNING_7(port_info, depth_info,
          //  dim_info));
          //  //  for (auto pragma : vec_pragma) {
          //  //    codegen->RemoveStmt(pragma);
          //  //  }
          //  continue;
          //  }
          dump_critical_message(INFTF_ERROR_7(port_info, depth_info, dim_info));
          any_err++;
        }
        //  Han: now miss some original struct information, not easy to check
        //  depth releate to original kernel argument if(!check_depths(depths,
        //  kernel, codegen)) {
        //    any_err++;
        //  }
        if (max_depths.size() > dummy_dims.size()) {
          string max_depth_info = str_merge(',', max_depths);
          dump_critical_message(
              INFTF_ERROR_8(port_info, max_depth_info, dim_info));
          any_err++;
        }
        if (instance.CheckMaxDepths(max_depths) == 0) {
          any_err++;
        }
        if (vec_pragma.size() > 1) {
          dump_critical_message(INFTF_ERROR_6(port_info, pragma_info));
          any_err++;
        }

        int flag_interface = instance.IsInterfaceRelated(depths, kernel);
        if (flag_interface != 0) {
          if (max_depths.empty()) {
            string str_example = "?";
            for (size_t j = 0; j < depths.size() - 1; j++) {
              str_example += ",?";
            }
            string sg_name_s = codegen->UnparseToString(sg_name);
            void *pragma = vec_pragma[0];
            string str_depth = mars_ir.get_pragma_attribute(pragma, "depth");
            string msg =
                "Incorrect pragma : " + codegen->UnparseToString(pragma) + " " +
                getUserCodeFileLocation(codegen, pragma, true) + "\n";
            msg +=
                "    depth defined as variable, so the max_depth must be set\n";
            msg += "Example : " + codegen->UnparseToString(pragma) +
                   " max_depth=" + str_example + "\n";
            dump_critical_message(INFTF_ERROR_4(msg));
            any_err++;  //  throw std::exception();
          }
          //  check_max_depth(depths, kernel, codegen);
        }
        {
          int cont_flag = 0;
          vector<size_t> sizes =
              mars_ir.get_array_dimension(sg_name, &cont_flag);
          vector<size_t> depths = mars_ir.get_array_depth(sg_name);
          int err = 0;

          if ("on" != options.get_option_key_value(
                          "-a", "skip_out_of_address_checker")) {
            printf(" -- Checking out-of-range access ... \n");
            err |= confirm_array_dimension(
                sg_name, sizes, depths, kernel, codegen,
                !pure_kernel_flow && !hls_flow && !has_check_zero_dim);
            err |= static_cast<int>(
                static_cast<int>((instance.CheckAddressOfOnePort(
                    sg_name, kernel, ports_alias[i], true))) == 0);
          }
          any_err |= err;

          if (err == 0) {
            if (preprocessing(sg_name, sizes, codegen, &any_err) != 0) {
              codegen->init_defuse_range_analysis();
            }
          }
        }
      }
    }
    if (any_err != 0) {
      throw std::exception();
    }
  }

  codegen->reset_func_call_cache();
  codegen->reset_func_decl_cache();
  set<void *> changed_decls;
  map<void *, void *> changed_parameters;
  bool has_error = false;
  for (auto kernel : kernels) {
    for (int i = 0; i < codegen->GetFuncParamNum(kernel); i++) {
      void *sg_name = codegen->GetFuncParam(kernel, i);
      int cont_flag = 0;
      vector<size_t> sizes = mars_ir.get_array_dimension(sg_name, &cont_flag);
      if (sizes.size() <= 1)
        continue;
      if (!mars_ir_v1.every_trace_is_bus(codegen, kernel, sg_name, sg_name))
        continue;
      set<void *> visited;
      has_error =
          has_error || check_array_linearize(sg_name, sizes, &visited, codegen);
    }
  }
  if (has_error) {
    throw std::exception();
  }

  for (auto kernel : kernels) {
    DEBUG(cout << "Kernel -- " << codegen->_p(kernel) << endl);
    bool Changed = false;
    for (int i = 0; i < codegen->GetFuncParamNum(kernel); i++) {
      void *sg_name = codegen->GetFuncParam(kernel, i);
      if (!mars_ir_v1.every_trace_is_bus(codegen, kernel, sg_name, sg_name))
        continue;
      int cont_flag = 0;
      vector<size_t> sizes = mars_ir.get_array_dimension(sg_name, &cont_flag);
      vector<void *> empty_offsets;
      if (sizes.size() > 1) {
        bool LocalChanged =
            apply_array_linearize(sg_name, sizes, cont_flag, empty_offsets,
                                  codegen, kernel, &changed_decls,
                                  &changed_parameters, true /*top*/) != 0;

        Changed |= LocalChanged;
        if (LocalChanged) {
          //  ZP: 20170122
          codegen->init_defuse_range_analysis();
        }
      }
      update_kernel_pragma_after_linearize(
          sg_name, sizes, cont_flag, empty_offsets, codegen, &mars_ir, kernel);
      update_wrapper_pragma_after_linearize(sg_name, sizes, cont_flag,
                                            empty_offsets, codegen, &mars_ir,
                                            kernel, skip_io_analysis);
    }
    if (Changed) {
      changed_decls.insert(kernel);
    }
    //  if (!Changed) continue;
  }

  bool error_out = false;
  for (auto decl_def : changed_decls) {
    auto set_decls = vec_same_decls[decl_def];
    vector<void *> vec_decls(set_decls.begin(), set_decls.end());
    for (auto decl : vec_decls) {
      if (codegen->isWithInHeaderFile(decl)) {
        string msg = "Kernel subfunction can not be declared in the header "
                     "file, if it contains mutiple dimension parameters\n";
        msg += "    Function name  : " + codegen->_up(decl) + "\n";
        msg += "    File location  : " +
               getUserCodeFileLocation(codegen, decl, false);
        string decl_info = codegen->_up(decl);
        string file_info = getUserCodeFileLocation(codegen, decl, true);

        dump_critical_message(INFTF_ERROR_2(decl_info, file_info), 0);
#ifdef PROJDEBUG
        cout << "ERROR: " << msg << endl;
#endif
        error_out = true;
        continue;
      }
    }
    for (auto arg : codegen->GetFuncParams(decl_def)) {
      if (changed_parameters.count(arg) > 0) {
        void *param = arg;
        map<void *, bool> res;
        if (mars_ir_v1.trace_to_bus_available(codegen, decl_def, param,
                                              nullptr) &&
            !mars_ir_v1.every_trace_is_bus(codegen, decl_def, param, nullptr) &&
            mars_ir_v1.any_trace_is_bus(codegen, decl_def, param, &res)) {
          string decl_info = codegen->_up(decl_def);
          string para_info = codegen->_up(param);
          string file_info = getUserCodeFileLocation(codegen, decl_def, true);
          dump_critical_message(INFTF_ERROR_10(decl_info, para_info, file_info),
                                0);
          error_out = true;
          continue;
        }
        codegen->SetTypetoVar(arg, changed_parameters[arg]);
      }
    }
  }
  if (error_out) {
    throw std::exception();
  }
  for (auto decl_def : changed_decls) {
    auto set_decls = vec_same_decls[decl_def];
    vector<void *> vec_decls(set_decls.begin(), set_decls.end());
    for (auto decl : vec_decls) {
      void *new_decl =
          codegen->CloneFuncDecl(decl_def, codegen->GetGlobal(decl), false);
      codegen->ReplaceStmt(decl, new_decl);
    }
  }
  return 1;
}
