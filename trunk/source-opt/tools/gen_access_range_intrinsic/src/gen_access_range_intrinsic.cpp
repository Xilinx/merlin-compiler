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

int access_range_gen_one_kernel(void *func_decl, CSageCodeGen *ast,
                                CMarsIrV2 *mars_ir,
                                set<void *> *global_with_header) {
  void *func_body = ast->GetFuncBody(func_decl);
  if (func_body == nullptr) {
    return 0;
  }
  void *global_scope = ast->GetGlobal(func_decl);
  for (int i = 0; i < ast->GetFuncParamNum(func_decl); ++i) {
    void *var_init = ast->GetFuncParam(func_decl, i);
    int continue_flag;
    vector<size_t> dims =
        mars_ir->get_array_dimension(var_init, &continue_flag);
    if (!dims.empty()) {
      bool all_dim_valid = true;
      for (auto dim : dims) {
        if (dim == 0U) {
          all_dim_valid = false;
          break;
        }
      }
      if (all_dim_valid) {
        string access_range_intrinsic_name = "__merlin_access_range";
        vector<void *> arg_list;
        arg_list.push_back(ast->CreateVariableRef(var_init));
        for (auto dim : dims) {
          arg_list.push_back(ast->CreateConst(0));
          arg_list.push_back(ast->CreateConst(dim - 1));
        }
        if (global_with_header->count(global_scope) <= 0) {
          ast->AddHeader("\n#include \"cmost.h\"\n", global_scope),
              global_with_header->insert(global_scope);
        }
        void *access_range_intrinsic_call = ast->CreateFuncCall(
            access_range_intrinsic_name, ast->GetTypeByString("void"), arg_list,
            func_body);

        void *call_stmt =
            ast->CreateStmt(V_SgExprStatement, access_range_intrinsic_call);
        ast->PrependChild(func_body, call_stmt);
      }
    }
  }
  return 1;
}

//  process:
//  for each kernel and its sub function,
//  generate a access range intrinsic for each array/pointer
//  parameter according to its type dimension or interface depth info
//  e.g.  __merlin_access_range_int_p(parameter, low, up, low, up);
int access_range_gen_top(CSageCodeGen *codegen, void *pTopFunc,
                         const CInputOptions &options) {
  cout << "Access range  generation" << endl;

  CMarsIrV2 mars_ir;
  mars_ir.build_mars_ir(codegen, pTopFunc);

  auto kernels = mars_ir.get_top_kernels();

  set<void *> visited;
  set<void *> global_with_header;
  for (auto kernel : kernels) {
    SetVector<void *> sub_funcs;
    SetVector<void *> sub_calls;
    codegen->GetSubFuncDeclsRecursively(kernel, &sub_funcs, &sub_calls);
    sub_funcs.insert(kernel);
    for (auto func : sub_funcs) {
      if (visited.count(func) > 0) {
        continue;
      }
      visited.insert(func);
      access_range_gen_one_kernel(func, codegen, &mars_ir, &global_with_header);
    }
  }
  return 0;
}
