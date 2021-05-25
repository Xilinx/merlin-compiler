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
#include <unordered_map>
void update_wrapper_pragma_after_rename(void *sg_init, CSageCodeGen *ast,
                                        CMarsIrV2 *mars_ir,
                                        const string &new_name) {
  void *pragma = get_unique_wrapper_pragma(sg_init, ast, mars_ir);

  if (pragma == nullptr) {
    return;
  }
  string port_name = mars_ir->get_pragma_attribute(pragma, "port");
  if (!port_name.empty()) {
    mars_ir->set_pragma_attribute(&pragma, "port", new_name);
  }
}

//  to work around HLS tool limitation:
//  for each kernel
//  1. rename keywords (in, out, input, output) with merlin_ prefix
//  2. replace double '_' with single '_'
int rename_interface_name_top(CSageCodeGen *codegen, void *pTopFunc,
                              const CInputOptions &options) {
  cout << "Interface port name renaming" << endl;

  CMarsIrV2 mars_ir;
  mars_ir.build_mars_ir(codegen, pTopFunc);

  auto kernels = mars_ir.get_top_kernels();
  bool Changed = false;
  const vector<string> reserve_keyword{"in",  "out",     "input", "output",
                                       "bit", "pos",     "label", "value",
                                       "val", "feature", "image", "parameter"};
  for (auto kernel : kernels) {
    int num_parameter = codegen->GetFuncParamNum(kernel);
    vector<void *> vec_pragmas;
    codegen->GetNodesByType(kernel, "preorder", "SgPragmaDeclaration",
                            &vec_pragmas);
    bool LocalChanged = false;
    map<void *, string> old_name2new_name;
    std::unordered_map<string, string> name_remap;
    map<void *, void *> old_para2new_para;
    for (int i = 0; i < num_parameter; ++i) {
      void *param = codegen->GetFuncParam(kernel, i);
      string old_name = codegen->GetVariableName(param);
      string new_name = codegen->legalizeName(old_name);
      old_name2new_name[param] = new_name;
      for (auto keyword : reserve_keyword) {
        if (new_name == keyword) {
          new_name = "merlin_" + keyword;
          break;
        }
      }
      if (new_name == old_name) {
        continue;
      }
      Changed = true;
      LocalChanged = true;
      codegen->get_valid_local_name(kernel, &new_name);
      old_name2new_name[param] = new_name;
      name_remap[old_name] = new_name;
      //  1. update wrapper pragma
      update_wrapper_pragma_after_rename(param, codegen, &mars_ir, new_name);
    }
    //  2. update kernel pragma
    codegen->ReplacePragmaTokens(kernel, name_remap);

    if (!LocalChanged) {
      continue;
    }
    vector<void *> fp_list;
    for (int i = 0; i < num_parameter; ++i) {
      void *old_param = codegen->GetFuncParam(kernel, i);
      void *new_param =
          codegen->CreateVariable(codegen->GetTypebyVar(old_param),
                                  old_name2new_name[old_param], old_param);
      old_para2new_para[old_param] = new_param;
      fp_list.push_back(new_param);
    }
    void *new_kernel = codegen->CreateFuncDecl(
        codegen->GetFuncReturnType(kernel), codegen->GetFuncName(kernel),
        fp_list, codegen->GetGlobal(kernel), true, kernel);

    void *old_kernel_body = codegen->CopyStmt(codegen->GetFuncBody(kernel));
    for (int i = 0; i < num_parameter; ++i) {
      void *old_param = codegen->GetFuncParam(kernel, i);
      void *new_param = codegen->GetFuncParam(new_kernel, i);
      codegen->replace_variable_references_in_scope(old_param, new_param,
                                                    old_kernel_body);
    }
    codegen->SetFuncBody(new_kernel, old_kernel_body);
    codegen->InsertStmt(new_kernel, kernel);
    vector<void *> vec_calls;
    codegen->GetFuncCallsFromDecl(kernel, nullptr, &vec_calls);
    for (auto call : vec_calls) {
      vector<void *> param_list;
      for (size_t i = 0; i < codegen->GetFuncCallParamNum(call); ++i) {
        param_list.push_back(
            codegen->CopyExp(codegen->GetFuncCallParam(call, i)));
      }
      auto new_call = codegen->CreateFuncCall(new_kernel, param_list, call);
      codegen->ReplaceExp(call, new_call);
    }
    codegen->RemoveStmt(kernel);
  }
  return static_cast<int>(Changed);
}
