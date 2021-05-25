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

//  sg_init is the kernel argument
void update_wrapper_pragma_after_bool2char(void *sg_init, CSageCodeGen *ast,
                                           CMarsIrV2 *mars_ir) {
  void *pragma = get_unique_wrapper_pragma(sg_init, ast, mars_ir);

  if (pragma == nullptr) {
    return;
  }

  mars_ir->set_pragma_attribute(&pragma, "data_type", "char");
}

bool bool2char(CSageCodeGen *codegen, void *var_init, CMarsIrV2 *mars_ir_v2,
               void *func_decl) {
  bool update_function = false;
  void *sg_type = codegen->GetTypebyVar(var_init);
  void *initializer = codegen->GetInitializer(var_init);
  if (initializer != nullptr) {
    sg_type = codegen->GetTypeByExp(initializer);
  }
  void *new_type = nullptr;
  if (codegen->ConvertBool2Char(sg_type, var_init, &new_type) == 0) {
    return false;
  }
  codegen->SetTypetoVar(var_init, new_type);
  int arg_idx = -1;
  if (codegen->IsArgumentInitName(var_init) != 0) {
    arg_idx = codegen->GetFuncParamIndex(var_init);
  }
  if (arg_idx >= 0) {
    update_function = true;
  }
  if (update_function) {
    update_wrapper_pragma_after_bool2char(var_init, codegen, mars_ir_v2);
  }

  //  add type cast for scalar access
  vector<void *> vec_refs;
  codegen->get_ref_in_scope(var_init, func_decl, &vec_refs, true, true);
  vector<void *> vec_assign;
  for (size_t i = vec_refs.size(); i > 0; --i) {
    void *ref = vec_refs[i - 1];
    void *pntr = codegen->get_pntr_from_var_ref(ref);
    if (pntr == nullptr) {
      continue;
    }
    void *sg_type = codegen->GetTypeByExp(pntr);
    if ((codegen->IsPointerType(sg_type) != 0) ||
        (codegen->IsArrayType(sg_type) != 0)) {
      continue;
    }
    void *parent = codegen->GetParent(pntr);
    if (codegen->IsAssignOp(parent) &&
        codegen->GetExpLeftOperand(parent) == pntr) {
      vec_assign.push_back(parent);
    }
    if (codegen->is_lvalue(pntr) != 0) {
      continue;
    }
    void *org_type = codegen->GetTypeByString("bool");
    codegen->AddCastToExp(pntr, org_type);
  }
  for (auto assign : vec_assign) {
    // replace implicit cast with explicit transformation to keep cross
    // reference consistent with any transformation
    void *char_type = codegen->GetTypeByString("char");
    codegen->AddCastToExp(codegen->GetExpRightOperand(assign), char_type);
  }
  return update_function;
}

void bool2char_top(CSageCodeGen *codegen, void *pTopFunc,
                   const CInputOptions &options) {
  printf("\n\nEnter bool2char...\n");
  string tool_type = options.get_option_key_value("-a", "impl_tool");
  if (tool_type == "sdaccel") {
    string tool_version = options.get_option_key_value("-a", "tool_version");
    if (tool_version == "vivado_hls" || tool_version == "vitis_hls") {
      cout << "disable bool conversion for hls flow because simulation may fail"
           << '\n';
      return;
    }
  }
  vector<CMirNode *> kernels;
  set<CMirNode *> all_nodes;
  vector<CMirNode *> all_nodes_vec;
  CMarsIr mars_ir;
  mars_ir.get_mars_ir(codegen, pTopFunc, options, true);
  mars_ir.get_all_kernel_nodes(&kernels, &all_nodes, &all_nodes_vec);
  CMarsIrV2 mars_ir_v2;
  mars_ir_v2.build_mars_ir(codegen, pTopFunc);

  bool any_update_function = false;
  for (auto &node : all_nodes_vec) {
    if (!node->is_function) {
      continue;
    }
    bool update_function = false;
    void *func_decl = codegen->TraceUpToFuncDecl(node->ref);
    vector<void *> vec_calls;
    codegen->GetFuncCallsFromDecl(func_decl, nullptr, &vec_calls);
    vector<void *> vec_decls = codegen->GetAllFuncDecl(func_decl);
    vector<void *> local_ref;
    codegen->GetNodesByType(node->ref, "preorder", "SgVarRefExp", &local_ref);
    set<void *> set_init;
    map<void *, int> arg_index;
    set<int> changed_arg_index;
    for (auto var_ref : local_ref) {
      void *var_init = codegen->GetVariableInitializedName(var_ref);
      if (!codegen->IsMemberVariable(var_init)) {
        set_init.insert(var_init);
      }
    }
    int index = 0;
    for (auto arg : codegen->GetFuncParams(func_decl)) {
      set_init.insert(arg);
      arg_index[arg] = index++;
    }

    for (auto var_init : set_init) {
      bool changed_arg = bool2char(codegen, var_init, &mars_ir_v2, func_decl);
      if (changed_arg) {
        update_function = true;
        if (arg_index.count(var_init) > 0) {
          changed_arg_index.insert(arg_index[var_init]);
        }
      }
    }
    if (update_function) {
      SgFunctionDeclaration *sg_decl =
          isSgFunctionDeclaration(static_cast<SgNode *>(func_decl));
      SgFunctionSymbol *sg_func_symbol =
          isSgFunctionSymbol(sg_decl->search_for_symbol_from_symbol_table());
      if (sg_func_symbol != nullptr) {
        //  FIXME: should set the new first non defininition declaration
        sg_func_symbol->set_declaration(sg_decl);
      }
      for (auto call : vec_calls) {
        vector<void *> new_args;
        index = 0;
        for (auto arg : codegen->GetFuncCallParamList(call)) {
          if (changed_arg_index.count(index) <= 0) {
            index++;
            new_args.push_back(codegen->CopyExp(arg));
            continue;
          }
          index++;
          //  try to skip original bool pointer cast
          void *arg_type = codegen->GetTypeByExp(arg);
          if ((codegen->IsArrayType(arg_type) != 0) ||
              (codegen->IsPointerType(arg_type) != 0)) {
            if ((codegen->IsCastExp(arg) != 0) &&
                codegen->UnparseToString(arg_type).find("bool") !=
                    string::npos) {
              arg = codegen->GetExpUniOperand(arg);
            }
          }
          new_args.push_back(codegen->CopyExp(arg));
        }
        void *new_call = codegen->CreateFuncCall(func_decl, new_args, call);
        codegen->ReplaceExp(call, new_call);
        any_update_function = true;
      }
      for (auto decl : vec_decls) {
        if (decl == func_decl) {
          continue;
        }
        void *new_decl =
            codegen->CloneFuncDecl(func_decl, codegen->GetGlobal(decl), false);
        codegen->ReplaceStmt(decl, new_decl);
      }
    }
  }
  if (any_update_function) {
    codegen->reset_func_call_cache();
    codegen->reset_func_decl_cache();
    for (auto kernel : mars_ir_v2.get_top_kernels()) {
      string func_name = codegen->get_wrapper_name_by_kernel(kernel);
      void *wrapper = codegen->GetFuncDeclByName(func_name);
      if (wrapper == nullptr) {
        continue;
      }
      void *body = codegen->GetFuncBody(wrapper);
      if (body == nullptr) {
        continue;
      }
      vector<void *> vec_inits;
      codegen->GetNodesByType(body, "preorder", "SgInitializedName",
                              &vec_inits);
      for (auto init : vec_inits) {
        bool2char(codegen, init, &mars_ir_v2, wrapper);
      }
    }
  }
}
