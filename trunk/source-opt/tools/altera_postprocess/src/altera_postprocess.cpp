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


#include <iostream>
#include <string>

#include "altera_postprocess.h"
#include "id_update.h"
#include "loop_parallel.h"
#include "mars_message.h"
#include "program_analysis.h"

int is_host_function(CSageCodeGen *codegen, void *func) {
  //  check whether there is task pragma in the function body
  void *func_body = codegen->GetFuncBody(func);
  //  remove unused function declaration without body
  if (func_body == nullptr) {
    return 0;
  }
  vector<void *> vec_pragmas;
  codegen->GetNodesByType(func_body, "preorder", "SgPragmaDeclaration",
                          &vec_pragmas);
  for (auto pragma : vec_pragmas) {
    SgPragmaDeclaration *decl =
        isSgPragmaDeclaration(static_cast<SgNode *>(pragma));
    assert(decl);
    CAnalPragma ana_pragma(codegen);
    bool invalid;
    if (ana_pragma.PragmasFrontendProcessing(decl, &invalid, false)) {
      if (ana_pragma.is_task()) {
        return 1;
      }
    }
  }
  return 0;
}

int altera_postprocess_top(CSageCodeGen *codegen, void *pTopFunc,
                           const CInputOptions &options) {
  if ("on" ==
      options.get_option_key_value("-a", "test_remove_redundant_global")) {
    remove_redundancy(codegen, pTopFunc);
  }

  return 0;
}

//  remove redundant function and global variables
int remove_redundancy(CSageCodeGen *codegen, void *pTopFunc) {
  CMarsIrV2 mars_ir;
  mars_ir.build_mars_ir(codegen, pTopFunc);
  bool errorOut = false;
  for (auto sg_kernel : mars_ir.get_top_kernels()) {
    //  ////////////////////////////////////////  /
    //  remove redundant functions in the kernel file
    //  ////////////////////////////////////////  /
    //  void *sg_global = codegen->GetGlobal(sg_kernel);
    vector<void *> all_funcs;
    codegen->GetNodesByType(nullptr, "preorder", "SgFunctionDeclaration",
                            &all_funcs);
    set<void *> all_sub_funcs;
    for (auto decl : all_funcs) {
      if (sg_kernel == decl) {
        continue;
      }
      //  1. do not touch function declaration in header files or outside
      //  input files
      //  2. try to remove unused function declaration without body
      //      if (!codegen->IsFromInputFile(decl) ||
      //          codegen->isWithInHeaderFile(decl))
      //        continue;
      if (codegen->isWithInHeaderFile(decl)) {
        continue;
      }
      if (!codegen->IsTransformation(decl) &&
          (codegen->IsFromInputFile(decl) == 0) &&
          !codegen->IsCompilerGenerated(decl)) {
        continue;
      }

      all_sub_funcs.insert(decl);
    }

    //  youxiang: 20160927 bug678
    {
      bool LocalChanged;
      vector<void *> vec_calls;
      vector<void *> vec_func_refs;
      codegen->GetNodesByType(nullptr, "preorder", "SgFunctionCallExp",
                              &vec_calls);
      codegen->GetNodesByType(nullptr, "preorder", "SgFunctionRefExp",
                              &vec_func_refs);
      map<void *, set<void *>> call2func;
      map<void *, set<void *>> func2calls;
      map<void *, set<void *>> ref2func;
      map<void *, set<void *>> func2refs;
      set<void *> sub_funcs;
      //  FIXME: the following code has running time issue because the time
      //  complexity
      //  is about N_FUNCS * N_CALLS
      for (auto func : all_sub_funcs) {
        if (mars_ir.is_kernel(func) || codegen->GetFuncName(func) == "main" ||
            (is_host_function(codegen, func) != 0)) {
          continue;
        }
        sub_funcs.insert(func);
        for (auto call : vec_calls) {
          void *callee_decl = codegen->GetFuncDeclByCall(call, 0);
          if (callee_decl == nullptr) {
            call2func[call].insert(nullptr);
          } else if (codegen->isSameFunction(callee_decl, func)) {
            func2calls[func].insert(call);
            call2func[call].insert(func);
          }
        }
        for (auto func_ref : vec_func_refs) {
          void *callee_decl = codegen->GetFuncDeclByRef(func_ref);
          if (callee_decl == nullptr) {
            ref2func[func_ref].insert(nullptr);
          } else if (codegen->isSameFunction(callee_decl, func)) {
            func2refs[func].insert(func_ref);
            ref2func[func_ref].insert(func);
          }
        }
      }
      do {
        LocalChanged = false;
        for (auto func : sub_funcs) {
          string orig_func_name = codegen->GetFuncName(func);
          bool not_dead = !func2calls[func].empty() ||
                          !func2refs[func].empty() ||
                          orig_func_name.find("__merlin") == 0;
          if (not_dead) {
            continue;
          }
          LocalChanged = true;
          sub_funcs.erase(func);
          vector<void *> sub_calls;
          vector<void *> sub_func_refs;
          codegen->GetNodesByType(func, "preorder", "SgFunctionCallExp",
                                  &sub_calls);
          codegen->GetNodesByType(func, "preorder", "SgFunctionRefExp",
                                  &sub_func_refs);
          for (auto call : sub_calls) {
            for (auto sub_func : call2func[call]) {
              if (sub_func == nullptr) {
                continue;
              }
              func2calls[sub_func].erase(call);
            }
            call2func.erase(call);
          }
          for (auto func_ref : sub_func_refs) {
            for (auto sub_func : ref2func[func_ref]) {
              if (sub_func == nullptr) {
                continue;
              }
              func2refs[sub_func].erase(func_ref);
            }
            ref2func.erase(func_ref);
          }
          vector<void *> fp_list;
          void *dummy_decl = codegen->CreateFuncDecl(
              "void", "__merlin_dummy_" + orig_func_name, fp_list,
              codegen->GetGlobal(func), false, nullptr);
          codegen->SetStatic(dummy_decl);
          codegen->ReplaceStmt(func, dummy_decl);
          break;
        }
      } while (!sub_funcs.empty() && LocalChanged);
    }

    //  1. check whether the remained functions are used only by top kernel
    //  function
    for (auto func : all_sub_funcs) {
      if (codegen->get_file(func) != codegen->get_file(sg_kernel)) {
        continue;
      }
      set<void *> bound_set;
      mars_ir.get_function_bound(func, &bound_set);
      assert(!bound_set.empty());
      bool is_host = false;
      for (auto bound_func : bound_set) {
        if (codegen->GetFuncName(bound_func) == "main") {
          is_host = true;
        }
      }
      if (is_host) {
        string func_info = "  Host function: " + codegen->GetFuncName(func) +
                           " " + getUserCodeFileLocation(codegen, func, true) +
                           "\n";
        func_info += "  Kernel function: " + codegen->GetFuncName(sg_kernel) +
                     " " + getUserCodeFileLocation(codegen, func, true) + "\n";
        func_info += "  Common file name: " + codegen->get_file(func);
        string msg = "Host function and kernel function can not be ";
        msg += "defined in the same file, please separate them into";
        msg += "different files. \n" + func_info;
        dump_critical_message(FUCIN_ERROR_2(func_info));
        errorOut = true;
      }
    }
#if 0
    //  2. check whether all the noinline functions are in the same kernel
    //  files
    //
    set<void*> noinlined_func;
    set<void*> noinlined_call;
    codegen->GetSubFuncDeclsRecursively(sg_kernel, &noinlined_func,
        &noinlined_call);
    for (auto func : noinlined_func) {
      if (codegen->get_file(func) != codegen->get_file(sg_kernel)) {
        string callee_info = "\'" + codegen->GetFuncName(func) + "\' "
          + getUserCodeFileLocation(codegen, sg_kernel, true) + " ";
        string kernel_info = "\'"
          + codegen->GetFuncName(sg_kernel) + "\' " +
          getUserCodeFileLocation(codegen, sg_kernel, true);
        string msg = "Finding function " +
          callee_info + " used by kernel " +
          kernel_info + " but in the different file. Please merge it manually";
        dump_critical_message(FUCIN_ERROR_3(callee_info, kernel_info));
        errorOut = true;
      }
    }
#endif
  }
  //  ////////////////////////////////////////  /
  //  remove redundant global variable extern
  //  ZP: 20161028
  //  ////////////////////////////////////////  /
  {
    CSageCodeGen *ast = codegen;
    printf("Removing redundant global variables\n");
    set<void *> hit;
    for (auto kernel : mars_ir.get_top_kernels()) {
#if PROJDEBUG
      cout << "kernel - " << ast->_up(kernel) << endl;
#endif

      void *global = ast->GetGlobal(kernel);
      if (hit.count(global) == 0U) {
        auto v_decls = ast->GetChildStmts(global);
        for (auto decl : v_decls) {
          if ((ast->IsVariableDecl(decl) != 0) &&
              (ast->IsFromInputFile(decl) != 0)) {
            if (ast->IsExtern(decl)) {
#if PROJDEBUG
              cout << "Global extern: " << ast->_up(decl) << endl;
#endif

              void *init = ast->GetVariableInitializedName(decl);
              void *type = ast->GetTypebyVar(init);
              void *base_type = ast->GetBaseType(type, true);
              // TODO(youxiang): Add by Yuxin, temporarily for merlin_stream
              if ((ast->IsStructureType(base_type) != 0) ||
                  (ast->IsClassType(base_type) != 0)) {
                string str_name = ast->GetTypeNameByType(base_type);
                if (str_name.find("merlin_stream") != string::npos) {
                  continue;
                }
              }

              vector<void *> v_gv_refs;
              ast->get_ref_in_scope_recursive(init, kernel, &v_gv_refs);
              if (v_gv_refs.empty()) {
#if PROJDEBUG
                cout << "Remove: " << ast->_up(decl) << endl;
#endif
                vector<void *> fp_list;
                void *dummy_global = ast->CreateFuncDecl(
                    "void", "__merlin_dummy_" + ast->GetVariableName(init),
                    fp_list, ast->GetGlobal(init), false, nullptr);
                ast->SetStatic(dummy_global);
                ast->ReplaceStmt(decl, dummy_global);
              }
            }
          }
        }

        hit.insert(global);
      }
    }
  }

  if (errorOut) {
    throw std::exception();
  }
  return 0;
}
