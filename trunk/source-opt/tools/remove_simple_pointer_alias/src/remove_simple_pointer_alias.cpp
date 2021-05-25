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

int remove_simple_pointer_alias(CSageCodeGen *codegen, void *func,
                                set<void *> *visited) {
  if (visited->count(func) > 0) {
    return 0;
  }
  visited->insert(func);
  vector<void *> vec_refs;
  codegen->GetNodesByType(func, "postorder", "SgVarRefExp", &vec_refs);
  int ret = 0;
  for (auto &ref : vec_refs) {
    void *var_init = codegen->GetVariableInitializedName(ref);
    if (var_init == nullptr) {
      continue;
    }
    if (codegen->IsArgumentInitName(var_init) != 0) {
      continue;
    }
    void *var_type = codegen->GetTypebyVar(var_init);
    if (codegen->IsPointerType(var_type) == 0) {
      continue;
    }
    if (codegen->is_lvalue(ref) != 0) {
      continue;
    }
    vector<void *> defs = codegen->GetVarDefbyPosition(var_init, ref);
    if (defs.size() != 1) {
      continue;
    }
    void *def_exp = defs[0];
    void *val_exp = nullptr;
    if (codegen->GetInitializer(var_init) == def_exp) {
      val_exp = codegen->GetInitializerOperand(var_init);
    } else if (codegen->IsAssignOp(def_exp) != 0) {
      val_exp = codegen->GetExpRightOperand(def_exp);
    }
    if (nullptr == val_exp) {
      continue;
    }

    if (codegen->IsVarRefExp(val_exp) != 0) {
      if (codegen->GetVariableInitializedName(val_exp) == var_init) {
        continue;
      }
    }
    // do not replace non-const pointer with const pointer
    if (codegen->IsCompatibleType(codegen->GetTypeByExp(val_exp),
                                  codegen->GetTypeByExp(ref), val_exp,
                                  true) == 0)
      continue;

    if (codegen->is_movable_test(val_exp, ref) != 0) {
      codegen->ReplaceExp(ref, codegen->CopyExp(val_exp));
      ret = 1;
    }
  }
  if (ret != 0) {
    codegen->dead_stmts_removal(func);
  }
  vector<void *> vec_calls;
  codegen->GetNodesByType(func, "preorder", "SgFunctionCallExp", &vec_calls);
  for (auto &call : vec_calls) {
    void *func_decl = codegen->GetFuncDeclByCall(call);
    if (func_decl == nullptr) {
      continue;
    }
    ret |= remove_simple_pointer_alias(codegen, func_decl, visited);
  }
  return ret;
}

int remove_simple_pointer_alias_top(CSageCodeGen *codegen, void *pTopFunc,
                                    const CInputOptions &option) {
  printf("    Enter pointer alias removal...\n");
  CMarsIrV2 mars_ir;
  mars_ir.build_mars_ir(codegen, pTopFunc);
  vector<void *> vecTopKernels = mars_ir.get_top_kernels();
  bool LocalChanged = false;
  bool Changed = codegen->simplify_pointer_access(nullptr);
  set<void *> visited_funcs;
  codegen->init_defuse();

  while (true) {
    LocalChanged = false;
    visited_funcs.clear();
    for (auto func : vecTopKernels) {
      LocalChanged |=
          remove_simple_pointer_alias(codegen, func, &visited_funcs);
    }
    if (LocalChanged) {
      codegen->init_defuse();
    }
    Changed |= LocalChanged;
    if (!LocalChanged) {
      break;
    }
  }
  return static_cast<int>(Changed);
}
