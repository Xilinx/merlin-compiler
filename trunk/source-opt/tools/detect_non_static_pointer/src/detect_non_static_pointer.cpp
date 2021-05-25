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
#include "mars_message.h"
bool check_non_static_pointer_interface(CSageCodeGen *codegen, void *func_decl,
                                        set<void *> *p_visited) {
  if (p_visited->count(func_decl) > 0) {
    return true;
  }
  p_visited->insert(func_decl);
  vector<void *> vec_calls;
  bool all_valid = true;
  codegen->GetNodesByType(func_decl, "preorder", "SgFunctionCallExp",
                          &vec_calls);
  for (auto call : vec_calls) {
    void *sub_func = codegen->GetFuncDeclByCall(call, 1);
    if (sub_func == nullptr) {
      continue;
    }
    // TODO(youxiang): check whether function 'sub_func' will be inlined or not
    int min_arg_num = min(codegen->GetFuncParamNum(sub_func),
                          codegen->GetFuncCallParamNum(call));
    for (int i = 0; i < min_arg_num; ++i) {
      void *param = codegen->GetFuncParam(sub_func, i);
      void *param_type = codegen->GetTypebyVar(param);
      if ((codegen->IsPointerType(param_type) == 0) &&
          (codegen->IsArrayType(param_type) == 0)) {
        continue;
      }
      void *arg = codegen->GetFuncCallParam(call, i);
      void *real_array = codegen->get_array_from_pntr(arg);
      if ((real_array == nullptr) ||
          (codegen->IsArrayType(codegen->GetTypebyVar(real_array)) != 0)) {
        continue;
      }
      vector<void *> vec_defs = codegen->GetVarDefbyPosition(real_array, call);

      set<void *> set_sources;
      //  get the real local/global variable or parameter
      for (auto def : vec_defs) {
        //  ignore self assign def
        if ((codegen->IsPlusPlusOp(def) != 0) ||
            (codegen->IsMinusMinusOp(def) != 0) ||
            (codegen->IsPlusAssignOp(def) != 0) ||
            (codegen->IsMinusAssignOp(def) != 0)) {
          continue;
        }
        if (codegen->IsAssignOp(def) != 0) {
          void *new_val = codegen->GetExpRightOperand(def);
          if ((codegen->IsAddOp(new_val) != 0) ||
              (codegen->IsSubtractOp(new_val) != 0)) {
            void *rhs;
            void *lhs;
            rhs = lhs = nullptr;
            codegen->GetExpOperand(new_val, &rhs, &lhs);
            if ((rhs != nullptr) &&
                ((codegen->IsPointerType(codegen->GetTypeByExp(rhs))) != 0) &&
                (codegen->get_array_from_pntr(rhs) == real_array)) {
              continue;
            }
            if ((lhs != nullptr) &&
                ((codegen->IsPointerType(codegen->GetTypeByExp(lhs))) != 0) &&
                (codegen->get_array_from_pntr(lhs) == real_array)) {
              continue;
            }
          }
        }
        void *exp = def;
        if (codegen->IsInitName(def) != 0) {
          set_sources.insert(def);
          continue;
        }
        if (codegen->IsInitializer(def) != 0) {
          exp = codegen->GetInitializerOperand(def);
        } else if (codegen->IsAssignOp(def) != 0) {
          exp = codegen->GetExpRightOperand(def);
        }
        void *var_init = codegen->get_array_from_pntr(exp);
        if (var_init == nullptr) {
          set_sources.insert(def);
        } else {
          set_sources.insert(var_init);
        }
      }
      if (set_sources.size() > 1) {
        string arg_info = "'" + codegen->GetVariableName(real_array) + "' " +
                          getUserCodeFileLocation(codegen, call, true);
        dump_critical_message(INFTF_ERROR_9(arg_info));
        all_valid = false;
      }
    }
    all_valid &=
        check_non_static_pointer_interface(codegen, sub_func, p_visited);
  }
  return all_valid;
}

int check_non_static_pointer_interface_top(CSageCodeGen *codegen,
                                           void *pTopFunc,
                                           const CInputOptions &options) {
  if ("sdaccel" != options.get_option_key_value("-a", "impl_tool")) {
    return 0;
  }
  CMarsIrV2 mars_ir;
  mars_ir.build_mars_ir(codegen, pTopFunc);

  auto kernels = mars_ir.get_top_kernels();
  bool all_valid = true;
  set<void *> visited;
  for (auto kernel : kernels) {
    all_valid &= check_non_static_pointer_interface(codegen, kernel, &visited);
  }
  if (!all_valid) {
    throw std::exception();
  }
  return 0;
}
