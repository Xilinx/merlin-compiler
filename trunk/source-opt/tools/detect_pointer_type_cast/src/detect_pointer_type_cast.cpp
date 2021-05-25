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
#include "program_analysis.h"

using MarsProgramAnalysis::ArrayRangeAnalysis;
using std::cout;
using std::vector;

bool DetectPointerTypeCast(CMarsIrV2 *p_mars_ir, CSageCodeGen *codegen,
                           void *func_decl) {
  vector<void *> exprs;
  bool all_valid = true;
  codegen->GetNodesByType(func_decl, "preorder", "SgExpression", &exprs, true,
                          true);
  for (auto &expr : exprs) {
    if (codegen->IsCastExp(expr) == 0) {
      continue;
    }
    void *cast_type = codegen->GetTypeByExp(expr);
    void *ori_expr = codegen->GetExpUniOperand(expr);
    void *ori_type = codegen->GetTypeByExp(ori_expr);

    //  screen out non-pointer type, only accept array and pointer type
    if (((codegen->IsPointerType(ori_type) == 0) &&
         (codegen->IsArrayType(ori_type) == 0)) ||
        ((codegen->IsPointerType(cast_type) == 0) &&
         (codegen->IsArrayType(cast_type) == 0))) {
      continue;
    }

    // Disabled: investigate the relationship with interface val
    // vector<void *> var_refs;
    // bool bus_related = false;
    // codegen->GetNodesByType(expr, "preorder", "SgVarRef", &var_refs, true);
    // for (auto &var_ref : var_refs) {
    //   void *sg_init_name = codegen->GetVariableInitializedName(var_ref);
    //   if (p_mars_ir->any_trace_is_bus(sg_init_name, expr)) {
    //     bus_related = true;
    //   }
    // }
    // if (!bus_related) {
    //   continue;
    // }

    if ((codegen->IsVoidType(codegen->GetBaseTypeOneLayer(cast_type)) != 0) ||
        (codegen->IsVoidType(codegen->GetBaseTypeOneLayer(ori_type)) != 0)) {
      continue;
    }

    //  If the casting is from an array pointer, let it pass.
    if (codegen->IsPointerType(ori_type)) {
      void *ori_ty_one_layer_down = ori_type;
      while (ori_ty_one_layer_down != nullptr) {
        if (codegen->IsAddressOfOp(ori_expr)) {
          ori_expr = codegen->GetExpUniOperand(ori_expr);
          ori_ty_one_layer_down = codegen->GetTypeByExp(ori_expr);
        } else {
          ori_ty_one_layer_down =
              codegen->GetBaseTypeOneLayer(ori_ty_one_layer_down);
        }
        if (codegen->IsArrayType(ori_ty_one_layer_down) ||
            codegen->IsPntrArrRefExp(ori_expr)) {
          break;
        }
      }
      if (codegen->IsArrayType(ori_ty_one_layer_down) ||
          codegen->IsPntrArrRefExp(ori_expr)) {
        continue;
      }
    }

    ori_type = codegen->ConvertTypeStrToPtrChar(ori_type);
    cast_type = codegen->ConvertTypeStrToPtrChar(cast_type);
    //  //  let compatiable and big-width type to small-width type pass
    int ori_width = codegen->get_bitwidth_from_type(ori_type);
    int cast_width = codegen->get_bitwidth_from_type(cast_type);

    vector<void *> indicies;
    void *pntr_init_name;
    codegen->parse_pntr_ref_new(expr, &pntr_init_name, &indicies);
    if (codegen->IsArrayType(ori_type) && codegen->IsPointerType(cast_type)) {
      if (codegen->IsCompatibleType(codegen->GetBaseType(ori_type, false),
                                    codegen->GetBaseType(cast_type, false),
                                    expr, false) != 0) {
        if ((codegen->IsConstType(cast_type) == 0 &&
             codegen->IsConstType(ori_type) == 0)) {
          continue;
        }
        if (codegen->IsConstType(cast_type) == 1 ||
            codegen->IsConstType(ori_type) == 0 ||
            codegen->has_write_in_scope(pntr_init_name,
                                        codegen->GetScope(nullptr)) == 0) {
          continue;
        }
      }
    } else {
      if (codegen->IsCompatibleType(ori_type, cast_type, expr, false) != 0 &&
          ori_width >= cast_width) {
        if ((codegen->IsConstType(cast_type) == 0 &&
             codegen->IsConstType(ori_type) == 0)) {
          continue;
        }
        if (codegen->IsConstType(cast_type) == 1 ||
            codegen->IsConstType(ori_type) == 0 ||
            codegen->has_write_in_scope(pntr_init_name,
                                        codegen->GetScope(nullptr)) == 0) {
          continue;
        }
      }
    }

    //  screen out memcpy
    void *curr_node = codegen->_pa(expr);
    bool bypass_mem_call = false;
    while (curr_node != NULL) {
      if (codegen->IsFunctionCall(curr_node) != 0) {
        string func_name = codegen->GetFuncNameByCall(curr_node);
        if (func_name.find("memcpy") != string::npos ||
            func_name.find("memset") != string::npos) {
          bypass_mem_call = true;
        }
        break;
      }
      curr_node = codegen->_pa(curr_node);
    }
    if (bypass_mem_call) {
      continue;
    }
    //  cout << codegen->_p(expr) << "---ORI TYPE: " << codegen->_p(ori_type)
    //     << "---CAST TYPE: " << codegen->_p(cast_type) << endl;
    //  string err_info = "from " + codegen->_p(ori_type) + " to " +
    //                  codegen->_p(cast_type) + " in " +
    //                  getUserCodeFileLocation(codegen, expr, true);
    //  dump_critical_message(PROCS_ERROR_66(err_info));

    //  I found small type to large type asscociated with interface
    //  that can pass vendor tool, so for now this will be a warning only.
    //  Needs further investigation.
    string error_info = "from " + codegen->GetStringByType(ori_type) + " to " +
                        codegen->GetStringByType(cast_type) +
                        getUserCodeFileLocation(codegen, expr, true);
    dump_critical_message(SYNCHK_ERROR_37(error_info));
    all_valid = false;
  }
  return all_valid;
}

int DetectPointerTypeCastInterfaceTop(CSageCodeGen *codegen, void *pTopFunc,
                                      const CInputOptions &options) {
  CMarsIrV2 mars_ir;
  mars_ir.build_mars_ir(codegen, pTopFunc);

  auto kernels = mars_ir.get_top_kernels();
  bool all_valid = true;
  for (auto kernel : kernels) {
    all_valid &= DetectPointerTypeCast(&mars_ir, codegen, kernel);
  }
  if (!all_valid) {
    throw std::exception();
  }
  return 0;
}
