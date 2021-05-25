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

bool DetectMultiDimPointer(CSageCodeGen *codegen, void *func_decl) {
  vector<void *> var_ref_exprs;
  bool all_valid = true;
  codegen->GetNodesByType(func_decl, "preorder", "SgVarRef", &var_ref_exprs,
                          true, true);
  for (auto &var_ref_expr : var_ref_exprs) {
    void *ref_type = codegen->GetTypeByExp(var_ref_expr);
    bool first_ptr_type = false, second_ptr_type = false;
    while (ref_type != nullptr) {
      if (codegen->IsPointerType(ref_type)) {
        if (first_ptr_type) {
          second_ptr_type = true;
          break;
        } else {
          first_ptr_type = true;
        }
      } else {
        first_ptr_type = false;
      }
      ref_type = codegen->GetBaseTypeOneLayer(ref_type);
    }
    void *var_init = codegen->GetVariableInitializedName(var_ref_expr);
    if (first_ptr_type && second_ptr_type &&
        codegen->IsLocalInitName(var_init)) {
      string error_info = "'" + codegen->GetVariableName(var_init) + "' " +
                          getUserCodeFileLocation(codegen, var_init, true);
      dump_critical_message(PROCS_ERROR_74(error_info));
      all_valid = false;
    }

    // void *basic_type = NULL;
    // vector<size_t> sizes;
    // codegen->get_type_dimension(ref_type, &basic_type, &sizes);
    // if (sizes.size() < 2) {
    //   continue;
    // }
    // bool has_zero = false;
    // for (auto &num : sizes) {
    //   if (num == 0) {
    //     has_zero = true;
    //   }
    // }
    // if (!has_zero) {
    //   continue;
    // }

    // void *ref_init_name = codegen->GetVariableInitializedName(var_ref_expr);
    //
    // vector<pair<void *, void *>> all_defs = codegen->
    //                                         GetAllDefbyPosition(ref_init_name);

    // vector<void *> all_refs =
    //     codegen->GetAllRefInScope(ref_init_name, func_decl);
    // //  check if the var_ref has been assigned
    // bool is_written = false;
    // for (auto &one_ref : all_refs) {
    //   if (codegen->is_write_ref(one_ref) != 0) {
    //     is_written = true;
    //     break;
    //   }
    // }
    // if (is_written) {
    //   continue;
    // }

    // string error_info = "' " + codegen->_up(var_ref_expr) + " ' " +
    //                     getUserCodeFileLocation(codegen, var_ref_expr, true);
    // dump_critical_message(PROCS_ERROR_74(error_info));
    // // all_valid = false;
  }
  return all_valid;
}

int DetectMultiDimPointerInterfaceTop(CSageCodeGen *codegen, void *pTopFunc,
                                      const CInputOptions &options) {
  if ("sdaccel" != options.get_option_key_value("-a", "impl_tool")) {
    return 0;
  }
  CMarsIrV2 mars_ir;
  mars_ir.build_mars_ir(codegen, pTopFunc);

  auto kernels = mars_ir.get_top_kernels();
  bool all_valid = true;
  for (auto kernel : kernels) {
    all_valid &= DetectMultiDimPointer(codegen, kernel);
  }

  if (!all_valid)
    throw std::exception();
  return 0;
}
