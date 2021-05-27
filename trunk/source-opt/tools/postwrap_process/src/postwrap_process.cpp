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

#include "array_linearize.h"
#include "mars_message.h"
#include "postwrap_process.h"

#define GLOBAL_VAR_PREFIX ""
#define ALTERA_ARGUMENTS_LIMITATION 32
int global_variable_conversion_top(CSageCodeGen *codegen, void *pTopFunc,
                                   const CInputOptions &options);

extern vector<void *> get_top_kernels(CSageCodeGen *codegen, void *pTopFunc);

int postwrap_process_top(CSageCodeGen *codegen, void *pTopFunc,
                         const CInputOptions &options) {
  if ("test_typedef_spread" ==
      options.get_option_key_value("-a", "test_typedef_spread")) {
    typedef_spread_top(codegen, pTopFunc, options);
    return 1;
  }

  if ("test_reference_replace" ==
      options.get_option_key_value("-a", "test_reference_replace")) {
    return reference_replace_top(codegen, pTopFunc, options);
  }

  if ("test_return_replace" ==
      options.get_option_key_value("-a", "test_return_replace")) {
    return_replace_top(codegen, pTopFunc, options);
    return 1;
  }

  if ("test_global_convert" ==
      options.get_option_key_value("-a", "test_global_convert")) {
    return global_variable_conversion_top(codegen, pTopFunc, options);
  }

  if ("test_global_checker" ==
      options.get_option_key_value("-a", "test_global_checker")) {
    return global_variable_conversion_checker(codegen, pTopFunc, options);
  }

  typedef_spread_top(codegen, pTopFunc, options);
  string tool_type = options.get_option_key_value("-a", "impl_tool");
 
  if (options.get_option_key_value("-a", "global_convert").empty()) {
    return_replace_top(codegen, pTopFunc, options);
  }

  //  ZP : 20160920
  if ("true" == options.get_option_key_value("-a", "global_convert")) {
    global_variable_conversion_top(codegen, pTopFunc, options);
  }

  //  youxiang: 20160922
  //  please do not add transformation after this
  //  because the current AST is incomplete.
  return 0;
}

//  at id_update.cpp

void return_replace_top(CSageCodeGen *codegen, void *pTopFunc,
                        const CInputOptions &options) {
  string tool_version = options.get_option_key_value("-a", "tool_version");
  bool hls_flow = false;
  if ("vitis_hls" == tool_version || "vivado_hls" == tool_version) {
    hls_flow = true;
  }
  if (hls_flow == true) {
    cout
        << "\n====== Skip kernel return value replace for HLS flow ========>\n";
    return;
  } else {
    cout << "\n====== Kernel return value replace ========>\n";
  }
  vector<void *> kernels = get_top_kernels(codegen, pTopFunc);
  int kernel_num = kernels.size();

  for (int j = 0; j < kernel_num; ++j) {
    SgFunctionDeclaration *kernel_decl =
        static_cast<SgFunctionDeclaration *>(kernels[j]);
    void *func_body = codegen->GetFuncBody(kernel_decl);
    if (func_body == nullptr) {
      continue;
    }
    SgFunctionType *func_type = kernel_decl->get_type();
    SgType *return_type = func_type->get_return_type();
    SgType *new_return_type = isSgType(static_cast<SgNode *>(
        codegen->RemoveConstVolatileType(return_type, kernel_decl)));
    string sFuncName = codegen->GetFuncName(kernel_decl);
    //  Get all kernel function calls
    vector<void *> all_calls;
    codegen->GetFuncCallsFromDecl(kernel_decl, nullptr, &all_calls);

    if (codegen->IsReferenceType(return_type) != 0) {
      string stype = codegen->_up(return_type);
      string func_info = "'" + sFuncName + "' " +
                         getUserCodeFileLocation(codegen, kernel_decl, true);
      dump_critical_message(POSTW_ERROR_1(func_info, stype));
      throw std::exception();
    }

    if (isSgTypeVoid(return_type) == nullptr) {
      //  ZP: 20180215 "C++11 Extended Initializer List" is not supported to
      //  be in an assignment in ROSE, while it can be as a return type in
      //  ROSE Workaround:
      //   Error out when detecting AggregateInitialize used as return type
      //   (an more restrict limitation)
      vector<void *> vec_stmts;
      codegen->GetNodesByType(func_body, "preorder", "SgReturnStmt",
                              &vec_stmts);
      for (size_t k = 0; k < vec_stmts.size(); k++) {
        SgReturnStmt *sg_return =
            isSgReturnStmt(static_cast<SgNode *>(vec_stmts[k]));
        SgExpression *rt_value = sg_return->get_expression();
        {
          if (codegen->IsAggregateInitializer(rt_value) != 0) {
            string msg =
                "Kernel return expresion can not be aggregate initilizer \n";

            string global_info;
            global_info += "    Return expression : " + codegen->_up(sg_return);
            global_info +=
                " " + getUserCodeFileLocation(codegen, sg_return, true) + "\n";
            dump_critical_message(POSTW_ERROR_2(global_info), 0);

            msg += global_info + "\n";
#if PROJDEBUG
            cout << "ERROR: " << msg << endl;
#endif
            throw std::exception();
          }
        }
      }

      //  Set return type, add new argument for use of pass return value
      SgType *sg_void_type = SageBuilder::buildVoidType();
      codegen->SetFuncReturnType(kernel_decl, sg_void_type);
      vector<size_t> dims;
      dims.push_back(1);
      void *arg_new_type = codegen->CreateArrayType(new_return_type, dims);
      void *new_arg = codegen->CreateVariable(arg_new_type, RETURN_VAR);
      codegen->insert_param_to_decl(kernel_decl, new_arg);

      for (size_t k = 0; k < vec_stmts.size(); k++) {
        void *rt_value = codegen->GetExprFromStmt(vec_stmts[k]);
        void *sg_new_ref = codegen->CreateExp(
            V_SgPntrArrRefExp, codegen->CreateVariableRef(new_arg),
            codegen->CreateConst(0));
        void *rt_assign = codegen->CreateStmt(V_SgAssignStatement, sg_new_ref,
                                              codegen->CopyExp(rt_value));
        codegen->ReplaceStmt(vec_stmts[k], rt_assign);
        void *new_sg_return = codegen->CreateStmt(
            V_SgReturnStmt, codegen->CreateExp(V_SgNullExpression));
        codegen->InsertAfterStmt(new_sg_return, rt_assign);
      }

      //  ZP: 20160924 add scope_type pragma
      //  #pragma ACCEL variable=RETURN_VAR scope_type=return
      void *new_func_body = codegen->GetFuncBody(kernel_decl);
      {
        CSageCodeGen *ast = codegen;
        string new_arg_str = RETURN_VAR;
        string s_pragma = "ACCEL interface variable=" + new_arg_str +
                          " io=WO scope_type=return \n";
        ast->PrependChild(new_func_body,
                          ast->CreatePragma(s_pragma, new_func_body));
      }
      //  Add the argument in the kernel function calls
      for (size_t i = 0; i < all_calls.size(); i++) {
        void *sg_call = all_calls[i];
        void *sg_return_stmt = codegen->GetEnclosingNode("Statement", sg_call);
        void *sg_func = codegen->GetEnclosingNode("Function", sg_call);
        void *sg_scope =
            codegen->GetEnclosingNode("BasicBlock", sg_return_stmt);
        string new_return_name = RETURN_VAR;
        codegen->get_valid_local_name(sg_func, &new_return_name);
        void *new_decl = codegen->CreateVariableDecl(
            new_return_type, new_return_name, nullptr, sg_scope, sg_call);
        void *sg_loc = sg_return_stmt;
        void *sg_pragma = codegen->GetPreviousStmt(sg_return_stmt);
        if ((sg_pragma != nullptr) && (codegen->IsPragmaDecl(sg_pragma) != 0)) {
          CAnalPragma ana_pragma(codegen);
          bool errorOut = false;
          if (ana_pragma.PragmasFrontendProcessing(sg_pragma, &errorOut,
                                                   false)) {
            if (ana_pragma.is_kernel() || ana_pragma.is_task()) {
              sg_loc = sg_pragma;
            }
          }
        }
        codegen->InsertStmt(new_decl, sg_loc);
        void *sg_init_name = codegen->GetVariableInitializedName(new_decl);
        void *new_return_arg = codegen->CreateExp(
            V_SgAddressOfOp, codegen->CreateVariableRef(sg_init_name));
        codegen->insert_argument_to_call(sg_call, new_return_arg);
        void *new_call_stmt =
            codegen->CreateStmt(V_SgExprStatement, codegen->CopyExp(sg_call));
        codegen->InsertStmt(new_call_stmt, sg_return_stmt);
        void *call_ref = codegen->CreateVariableRef(sg_init_name);
        codegen->ReplaceExp(sg_call, call_ref);
      }
    }
  }
}

void structure_typedef_spread(CSageCodeGen *codegen, void *sg_type,
                              set<void *> *visited, bool skip_builtin,
                              void *scope) {
  void *sg_base_type = codegen->GetBaseType(sg_type, true);
  if ((codegen->IsStructureType(sg_base_type) != 0) ||
      (codegen->IsClassType(sg_base_type) != 0) ||
      (codegen->IsUnionType(sg_base_type) != 0)) {
    if (visited->count(sg_base_type) > 0) {
      return;
    }
    visited->insert(sg_base_type);
    vector<void *> data_members;
    codegen->GetClassDataMembers(sg_base_type, &data_members);
    for (auto m : data_members) {
      SgInitializedName *var_init = static_cast<SgInitializedName *>(
          codegen->GetVariableInitializedName(m));
      if (((scope == nullptr) && ((codegen->IsFromInputFile(var_init) != 0) ||
                                  codegen->IsTransformation(var_init))) ||
          ((scope != nullptr) && (codegen->IsInScope(var_init, scope) != 0))) {
        void *var_type = codegen->GetTypebyVar(var_init);
        void *var_new_type = var_type;
        string msg = codegen->GetTypeNameByType(var_type);
        msg += getUserCodeFileLocation(codegen, var_init, true);
        int ret = codegen->RemoveTypedef(var_type, &var_new_type, var_init,
                                         skip_builtin, msg);
        if (ret != 0) {
#ifdef PROJDEBUG
          cout << "=> structure REPLACE: " << codegen->_p(var_new_type) << endl;
          cout << "=> structure Orig: " << codegen->_p(var_type) << endl;
#endif
          var_init->set_type(static_cast<SgType *>(var_new_type));
        }
        structure_typedef_spread(codegen, var_new_type, visited, skip_builtin,
                                 scope);
      }
    }
  }
}

void cast_typedef_spread(CSageCodeGen *codegen, void *scope,
                         bool skip_builtin) {
  vector<void *> vec_casts;
  codegen->GetNodesByType(scope, "preorder", "SgCastExp", &vec_casts);
  for (size_t k = 0; k < vec_casts.size(); k++) {
    SgCastExp *sg_cast = static_cast<SgCastExp *>(vec_casts[k]);
    void *sg_type = sg_cast->get_type();
    void *sg_new_type = sg_type;
    string msg = codegen->GetTypeNameByType(sg_type);
    msg += getUserCodeFileLocation(codegen, sg_cast, true);
    int ret = codegen->RemoveTypedef(sg_type, &sg_new_type, sg_cast,
                                     skip_builtin, msg);
    if (ret == 0) {
      continue;
    }

#if PROJDEBUG
    cout << "=> cast REPLACE: " << codegen->_p(sg_new_type) << endl;
    cout << "=> cast Orig: " << codegen->_p(sg_type) << endl;
#endif
    sg_cast->set_type(static_cast<SgType *>(sg_new_type));
  }
}

void castonly_typedef_spread(CSageCodeGen *codegen, void *func_decl,
                             set<void *> *visited_funcs, bool skip_builtin) {
  if (visited_funcs->count(func_decl) > 0) {
    return;
  }
  visited_funcs->insert(func_decl);
  cast_typedef_spread(codegen, func_decl, skip_builtin);

  SetVector<void *> sub_funcs;
  SetVector<void *> sub_calls;
  codegen->GetSubFuncDeclsRecursively(func_decl, &sub_funcs, &sub_calls);
  for (auto &sub_decl : sub_funcs) {
    castonly_typedef_spread(codegen, sub_decl, visited_funcs, skip_builtin);
  }
}

void compound_literal_typedef_spread(CSageCodeGen *codegen, void *scope,
                                     bool skip_builtin) {
  vector<void *> vec_stmts;
  codegen->GetNodesByType_int(scope, "preorder", V_SgCompoundLiteralExp,
                              &vec_stmts);
  for (size_t k = 0; k < vec_stmts.size(); k++) {
    if (isSgCompoundLiteralExp(static_cast<SgNode *>(vec_stmts[k])) !=
        nullptr) {
      SgCompoundLiteralExp *sg_compound =
          static_cast<SgCompoundLiteralExp *>(vec_stmts[k]);
      SgAggregateInitializer *aggr_init = isSgAggregateInitializer(
          static_cast<SgNode *>(codegen->GetInitializerOperand(sg_compound)));
      if (aggr_init == nullptr) {
        continue;
      }
      void *sg_type = aggr_init->get_expression_type();
      void *sg_new_type = sg_type;
      string msg = codegen->GetTypeNameByType(sg_type);
      msg += getUserCodeFileLocation(codegen, aggr_init, true);
      int ret = codegen->RemoveTypedef(sg_type, &sg_new_type, sg_compound,
                                       skip_builtin, msg);
      if (ret == 0) {
        continue;
      }

#if PROJDEBUG
      cout << "=> compound REPLACE: " << codegen->_p(sg_new_type) << endl;
      cout << "=> compound Orig: " << codegen->_p(sg_type) << endl;
#endif
      aggr_init->set_expression_type(static_cast<SgType *>(sg_new_type));
    }
  }

  vector<void *> vec_whiles;
  codegen->GetNodesByType_int(scope, "preorder", V_SgWhileStmt, &vec_whiles);
  for (size_t k = 0; k < vec_whiles.size(); k++) {
    SgWhileStmt *sg_while = static_cast<SgWhileStmt *>(vec_whiles[k]);
    void *cond = sg_while->get_condition();
    cout << "while cond: " << codegen->_p(cond) << endl;
  }
}

void sizeof_typedef_spread(CSageCodeGen *codegen, void *scope,
                           void *input_scope, bool skip_builtin) {
  vector<void *> vec_sizeof;
  codegen->GetNodesByType_int(scope, "postorder", V_SgSizeOfOp, &vec_sizeof);
  for (size_t k = 0; k < vec_sizeof.size(); k++) {
    SgSizeOfOp *sizeof_op = static_cast<SgSizeOfOp *>(vec_sizeof[k]);
    void *sg_type = sizeof_op->get_operand_type();
    if (((input_scope == nullptr) &&
         ((codegen->IsFromInputFile(sizeof_op) != 0) ||
          codegen->IsTransformation(sizeof_op))) ||
        ((input_scope != nullptr) &&
         (codegen->IsInScope(sizeof_op, input_scope) != 0))) {
      if (sg_type == nullptr) {
        continue;
      }
      void *sg_new_type = sg_type;
      string msg = codegen->GetTypeNameByType(sg_type);
      msg += getUserCodeFileLocation(codegen, sizeof_op, true);
      int ret = codegen->RemoveTypedef(sg_type, &sg_new_type, sizeof_op,
                                       skip_builtin, msg);
      if (ret != 0) {
#if PROJDEBUG
        cout << "=> sizeof REPLACE: " << codegen->_up(sizeof_op) << " : "
             << codegen->_p(sg_new_type) << endl;
        cout << "=> sizeof Orig: " << codegen->_up(sizeof_op) << " : "
             << codegen->_p(sg_type) << endl;
#endif
        sizeof_op->set_operand_type(static_cast<SgType *>(sg_new_type));
      }
    }
  }
}

void intype_expr_typedef_spread(CSageCodeGen *codegen, void *sg_type,
                                void **sg_new_type, void *pos,
                                bool skip_builtin) {
  if (codegen->IsArrayType(sg_type) != 0) {
    SgArrayType *sg_type_array = isSgArrayType(static_cast<SgNode *>(sg_type));
    SgExpression *size_exp = sg_type_array->get_index();
    void *base_type = sg_type_array->get_base_type();
    void *new_base_type = base_type;
    sizeof_typedef_spread(codegen, size_exp, size_exp, skip_builtin);
    intype_expr_typedef_spread(codegen, base_type, &new_base_type, pos,
                               skip_builtin);
    *sg_new_type = SageBuilder::buildArrayType(
        isSgType(static_cast<SgNode *>(new_base_type)), size_exp);
    return;
  }
}

void init_typedef_spread(CSageCodeGen *codegen, void *sg_init,
                         set<void *> *visited_type, bool skip_builtin,
                         void *scope) {
  SgInitializedName *init = static_cast<SgInitializedName *>(sg_init);
  void *sg_type = codegen->GetTypebyVar(init);
  void *sg_mid_type = sg_type;
  void *sg_new_type = sg_type;
  intype_expr_typedef_spread(codegen, sg_type, &sg_mid_type, init,
                             skip_builtin);
  int ret;
  string msg = codegen->GetStringByType(sg_type);
  msg += getUserCodeFileLocation(codegen, sg_init, true);
  if (sg_type != sg_mid_type) {
    ret = codegen->RemoveTypedef(sg_mid_type, &sg_new_type, init, skip_builtin,
                                 msg);
  } else {
    ret =
        codegen->RemoveTypedef(sg_type, &sg_new_type, init, skip_builtin, msg);
  }

  //  Replace the typedef inside the structure
  structure_typedef_spread(codegen, sg_new_type, visited_type, skip_builtin,
                           scope);

  if (ret != 0) {
#if PROJDEBUG
    cout << "=> init REPLACE: " << codegen->_up(init) << " : "
         << codegen->_p(sg_new_type) << endl;
    cout << "=> init Orig: " << codegen->_up(init) << " : "
         << codegen->_p(sg_type) << endl;
#endif
    codegen->SetTypetoVar(init, sg_new_type);
  }
}

int remove_init_typedef(CSageCodeGen *codegen, void *sg_init,
                        void **sg_new_type, bool skip_builtin) {
  SgInitializedName *init = static_cast<SgInitializedName *>(sg_init);
  void *sg_type = codegen->GetTypebyVar(init);
  void *sg_mid_type = sg_type;
  *sg_new_type = sg_type;
  intype_expr_typedef_spread(codegen, sg_type, &sg_mid_type, init,
                             skip_builtin);
  string msg = codegen->GetStringByType(sg_type);
  msg += getUserCodeFileLocation(codegen, sg_init, true);
  int ret;
  if (sg_type != sg_mid_type) {
    ret = codegen->RemoveTypedef(sg_mid_type, sg_new_type, init, skip_builtin,
                                 msg);
  } else {
    ret = codegen->RemoveTypedef(sg_type, sg_new_type, init, skip_builtin, msg);
  }

  return ret;
}

void func_typedef_spread(CSageCodeGen *codegen, void *func_decl,
                         set<void *> *visited_type, bool skip_builtin,
                         void *scope) {
  //  DEFINE_START_END;
  //  Probably global memory access
  vector<void *> vec_refs;
  codegen->GetNodesByType(func_decl, "preorder", "SgVarRefExp", &vec_refs);
  for (size_t k = 0; k < vec_refs.size(); k++) {
    void *init = codegen->GetVariableInitializedName(vec_refs[k]);
    if (codegen->IsArgumentInitName(init) != 0) {
      continue;
    }
    init_typedef_spread(codegen, init, visited_type, skip_builtin, scope);
  }

  cast_typedef_spread(codegen, func_decl, skip_builtin);
  compound_literal_typedef_spread(codegen, func_decl, skip_builtin);

  //  Probably unaccessed variables
  vector<void *> vec_inits;
  codegen->GetNodesByType(func_decl, "preorder", "SgInitializedName",
                          &vec_inits);
  for (size_t k = 0; k < vec_inits.size(); k++) {
    void *init = vec_inits[k];
    if (codegen->IsArgumentInitName(init) != 0) {
      continue;
    }
    if (((scope == nullptr) && ((codegen->IsFromInputFile(init) != 0) ||
                                codegen->IsTransformation(init))) ||
        ((scope != nullptr) && (codegen->IsInScope(init, scope) != 0))) {
      init_typedef_spread(codegen, init, visited_type, skip_builtin, scope);
    }
  }
  //  Replace function return types
  void *sg_type = codegen->GetFuncReturnType(func_decl);
  void *sg_new_type = sg_type;
  string msg = codegen->GetFuncName(func_decl);
  msg += getUserCodeFileLocation(codegen, func_decl, true);
  int ret = codegen->RemoveTypedef(sg_type, &sg_new_type, func_decl,
                                   skip_builtin, msg);
  if (codegen->GetFuncBody(func_decl) != nullptr) {
    for (int i = 0; i < codegen->GetFuncParamNum(func_decl); i++) {
      void *arg = codegen->GetFuncParam(func_decl, i);
      init_typedef_spread(codegen, arg, visited_type, skip_builtin, scope);
    }
    if (ret != 0) {
#if PROJDEBUG
      cout << "=> func Orig: " << codegen->_p(func_decl) << endl;
#endif
      codegen->SetFuncReturnType(func_decl, sg_new_type);
#if PROJDEBUG
      cout << "=> func Orig: " << codegen->_p(func_decl) << endl;
#endif
    }
  } else {
    //  Rebuild the nondefine function, no matter if there is a related
    //  defined function
    //
    //  Yuxin: 20181212, skip the nondefine function decls in the include
    //  files anyway Will influence the whole file info
    if (codegen->isWithInHeaderFile(func_decl)) {
      return;
    }
    vector<void *> param_list;
    for (int i = 0; i < codegen->GetFuncParamNum(func_decl); i++) {
      void *arg = codegen->GetFuncParam(func_decl, i);
      void *initer = codegen->GetInitializer(arg);
      void *sg_arg_type = nullptr;
      remove_init_typedef(codegen, arg, &sg_arg_type, skip_builtin);
      void *copy_arg =
          codegen->CreateVariable(sg_arg_type, codegen->_up(arg), arg);
      if (initer != nullptr) {
        codegen->SetInitializer(copy_arg, codegen->CopyExp(initer));
      }
      param_list.push_back(copy_arg);
    }
#if PROJDEBUG
    cout << "=> nondefine func Orig: " << codegen->_p(func_decl) << endl;
#endif
    string sFuncName = codegen->GetFuncName(func_decl);
    void *new_func_decl =
        codegen->CreateFuncDecl(sg_new_type, sFuncName, param_list,
                                codegen->GetScope(func_decl), false, func_decl);
    if (ret != 0) {
      codegen->SetFuncReturnType(new_func_decl, sg_new_type);
    }
    if (codegen->IsStatic(func_decl)) {
      codegen->SetStatic(new_func_decl);
    }
    if (codegen->IsExtern(func_decl)) {
      codegen->SetExtern(new_func_decl);
    }
    codegen->ReplaceStmt(func_decl, new_func_decl);
#if PROJDEBUG
    cout << "=> nondefine func REPLACE: " << codegen->_p(new_func_decl) << endl;
#endif
  }
}

//  FIXME: typedef might be recursive
void typedef_spread_top(CSageCodeGen *codegen, void *pTopFunc,
                        const CInputOptions &options, bool castonly,
                        bool skip_builtin, void *scope, bool skip_include) {
  DEFINE_START_END;
  cout << "\n====== Typedef replace ========>\n";

  vector<void *> decls;
  vector<void *> all_decls;
  codegen->GetNodesByType(pTopFunc, "preorder", "SgFunctionDeclaration",
                          &all_decls);
  for (auto func_decl : all_decls) {
    //  Yuxin: 20181212, replace the typedef during lower_separate
    //  youxiang: 20161006
    if (skip_include && codegen->isWithInHeaderFile(func_decl)) {
      continue;
    }
    if (!codegen->IsTransformation(func_decl) &&
        (codegen->IsFromInputFile(func_decl) == 0) &&
        !codegen->IsCompilerGenerated(func_decl)) {
      continue;
    }
    //  youxiang: 20180220
    if (codegen->IsSupportedFunction(func_decl) == 0) {
      continue;
    }
    decls.push_back(func_decl);
  }
  //  bool Changed = false;
  //  set<void *> changed_func;
  set<void *> visited_funcs;
  if (castonly) {
    cout << "[Cast only typdef process] \n";
    for (size_t j = 0; j < decls.size(); ++j) {
      castonly_typedef_spread(codegen, decls[j], &visited_funcs, skip_builtin);
    }
    //    return Changed;
  }

  codegen->reset_func_call_cache();
  codegen->reset_func_decl_cache();
  set<void *> visited_type;
  for (size_t j = 0; j < decls.size(); ++j) {
    //  STEMP(start);
    func_typedef_spread(codegen, decls[j], &visited_type, skip_builtin, scope);
    //  ACCTM(func_typedef_spread_723, start, end);
  }

  vector<void *> vec_inits;
  codegen->GetNodesByType(pTopFunc, "preorder", "SgInitializedName",
                          &vec_inits);
  for (size_t k = 0; k < vec_inits.size(); k++) {
    SgInitializedName *init = static_cast<SgInitializedName *>(vec_inits[k]);
    if (codegen->IsGlobalInitName(init) == 0) {
      continue;
    }
    if (((scope == nullptr) && ((codegen->IsFromInputFile(init) != 0) ||
                                codegen->IsTransformation(init))) ||
        ((scope != nullptr) && (codegen->IsInScope(init, scope) != 0))) {
      init_typedef_spread(codegen, init, &visited_type, skip_builtin, scope);
      //  youxiang 20160930 fix cast in global variable initializer
      void *initializer = codegen->GetInitializer(init);
      if (initializer != nullptr) {
        cast_typedef_spread(codegen, initializer, skip_builtin);
      }
    }
  }

  sizeof_typedef_spread(codegen, pTopFunc, scope, skip_builtin);
}

void func_reference_replace(CSageCodeGen *codegen, void *func_decl) {
  //  /////////////////////////////////  /
  //  ZP: 20160920
  CSageCodeGen *ast = codegen;
  vector<void *> vec_func_decls;
  ast->GetNodesByType(nullptr, "preorder", "SgFunctionDeclaration",
                      &vec_func_decls);
  vector<void *> vec_same_decls;
  for (auto one_decl : vec_func_decls) {
    if (ast->GetFuncBody(one_decl) != nullptr) {
      continue;
    }
    if (!ast->isSameFunction(one_decl, func_decl)) {
      continue;
    }
    //  youxiang: 20160923 bug639: do not touch header file
    if (ast->isWithInHeaderFile(one_decl)) {
      continue;
    }
    vec_same_decls.push_back(one_decl);
  }
  //  /////////////////////////////////  /

  //  Preparation for the transformation
  vector<void *> all_calls;
  codegen->GetFuncCallsFromDecl(func_decl, nullptr, &all_calls);
  vector<void *> vec_refs;
  codegen->GetNodesByType(func_decl, "preorder", "SgVarRefExp", &vec_refs);
  set<int> arg_change;
  map<void *, void *> to_replace;

  //  Replace the function decalaration argument
  for (int i = 0; i < codegen->GetFuncParamNum(func_decl); i++) {
    void *arg = codegen->GetFuncParam(func_decl, i);
    SgInitializedName *sg_arg = isSgInitializedName(static_cast<SgNode *>(arg));
    SgType *arg_type = sg_arg->get_type();

    if (isSgReferenceType(arg_type) != nullptr) {
      //  FIXME: reference variable as loop iterator
      SgType *base_one_layer = isSgType(
          static_cast<SgType *>(codegen->GetBaseTypeOneLayer(arg_type)));
      if (SageInterface::isConstType(base_one_layer)) {
        sg_arg->set_type(base_one_layer);
        continue;
      }
      SgArrayType *arg_new_type = nullptr;
      arg_new_type = SageBuilder::buildArrayType(
          isSgReferenceType(arg_type)->get_base_type(),
          SageBuilder::buildIntVal(1));
      sg_arg->set_type(arg_new_type);
      arg_change.insert(i);

      //  Replace the variable references in the function
      for (size_t k = 0; k < vec_refs.size(); k++) {
        void *var_init = codegen->GetVariableInitializedName(vec_refs[k]);
        if (var_init == sg_arg) {
          void *sg_ref = vec_refs[k];
          void *sg_new_ref = codegen->CreateExp(
              V_SgPntrArrRefExp, codegen->CreateVariableRef(var_init),
              codegen->CreateConst(0));
          to_replace[sg_ref] = sg_new_ref;
        }
      }
    }
  }

  for (auto it : to_replace) {
    cout << "=> reference org: " << codegen->_p(it.first) << endl;
    codegen->ReplaceExp(it.first, it.second);
    cout << "=> reference new: " << codegen->_p(it.second) << endl;
  }

  //  Replace the argument in the kernel function calls

  for (size_t i = 0; i < all_calls.size(); i++) {
    void *sg_call = all_calls[i];
    for (int k = 0; k < codegen->GetFuncCallParamNum(sg_call); k++) {
      if (arg_change.find(k) != arg_change.end()) {
        void *sg_arg = codegen->GetFuncCallParam(sg_call, k);
        void *sg_new_arg =
            codegen->CreateExp(V_SgAddressOfOp, codegen->CopyExp(sg_arg));
        cout << "=> arg org: " << codegen->_p(sg_new_arg) << endl;
        codegen->ReplaceExp(sg_arg, sg_new_arg);
        cout << "=> arg new: " << codegen->_p(sg_new_arg) << endl;
      }
    }
  }

  //  ////////////////////////////////  /
  //  ZP: 20160920
  if (!arg_change.empty()) {
    for (auto one_decl : vec_same_decls) {
      void *new_decl =
          ast->CloneFuncDecl(func_decl, ast->GetGlobal(one_decl), false);
      codegen->ReplaceStmt(one_decl, new_decl);
    }
  }
  //  ////////////////////////////////  /
  /*
      SetVector<void *> sub_funcs;
      SetVector<void *> sub_calls;
      codegen->GetSubFuncDeclsRecursively(func_decl, &sub_funcs, &sub_calls);
      cout << "<<<<<<Subfunction substitute\n";
      for (auto &sub_decl : sub_funcs) {
          func_reference_replace(codegen, sub_decl);
      }*/
}

int reference_replace_top(CSageCodeGen *codegen, void *pTopFunc,
                          const CInputOptions &options) {
  cout << "\n====== Reference replace ========>\n";

  vector<void *> vec_func_decls;
  codegen->GetNodesByType(nullptr, "preorder", "SgFunctionDeclaration",
                          &vec_func_decls);
  for (auto one_decl : vec_func_decls) {
    if (codegen->IsFromInputFile(one_decl) == 0) {
      continue;
    }
    if (codegen->isWithInHeaderFile(one_decl)) {
      continue;
    }
    if (codegen->GetFuncBody(one_decl) == nullptr) {
      continue;
    }
    func_reference_replace(codegen, one_decl);
  }

  return 1;
}

void collect_used_global_variable(void *scope, set<void *> *s_global,
                                  map<void *, set<void *>> *var2refs,
                                  CSageCodeGen *ast, bool *errorOut,
                                  bool hls_flow) {
  vector<void *> v_call;
  ast->GetNodesByType(scope, "preorder", "SgFunctionCallExp", &v_call);

  for (auto call : v_call) {
    void *decl = ast->GetFuncDeclByCall(call);
    if ((decl != nullptr) && (ast->GetFuncBody(decl) != nullptr)) {
      if (ast->IsFromInputFile(decl) != 0) {
        collect_used_global_variable(ast->GetFuncBody(decl), s_global, var2refs,
                                     ast, errorOut, hls_flow);
      }
    }
  }

  vector<void *> v_ref;
  ast->GetNodesByType(scope, "preorder", "SgVarRefExp", &v_ref);

  for (auto ref : v_ref) {
    void *name = ast->GetVariableInitializedName(ref);
    if ((ast->IsGlobalInitName(name) != 0) &&
        (ast->IsFromInputFile(name) != 0)) {
      void *var_decl = ast->GetVariableDecl(name);
      // FIXME: for hls flow, we only treat external global variable as
      // interface
      if (hls_flow && !ast->IsExtern(var_decl))
        continue;
      void *type = ast->GetTypebyVar(name);
      void *base_type = ast->GetBaseType(type, true);
      // TODO(youxiang): Add by Yuxin, temporarily for merlin_stream
      if ((ast->IsStructureType(base_type) != 0) ||
          (ast->IsClassType(base_type) != 0)) {
        string str_name = ast->GetTypeNameByType(base_type);
        if (str_name.find("merlin_stream") != string::npos) {
          continue;
        }
      }
      if (ast->IsConstType(type) == 0 || ast->IsPointerType(type) == 1) {
        s_global->insert(name);
        (*var2refs)[name].insert(ref);
        if (ast->IsArrayType(type) != 0) {
          void *base_type;
          vector<size_t> nSizes;
          ast->get_type_dimension(type, &base_type, &nSizes, name);
          bool found_undertermined_size = false;
          for (auto size : nSizes) {
            if (!ast->IsEmptySize(size))
              continue;
            found_undertermined_size = true;
          }
          if (found_undertermined_size) {
            void *initializer = ast->GetInitializer(name);
            if (initializer != nullptr) {
              void *initializer_type = ast->GetTypeByExp(initializer);
              if (ast->IsArrayType(initializer_type)) {
                nSizes.clear();
                ast->get_type_dimension(initializer_type, &base_type, &nSizes,
                                        name);
                found_undertermined_size = false;
                for (auto size : nSizes) {
                  if (!ast->IsEmptySize(size))
                    continue;
                  found_undertermined_size = true;
                }
              }
            }
          }
          if (found_undertermined_size) {
            // MER-1758 check extern global array varibles with undetermined
            // size, such as extern int a[];
            string global_info =
                "\'" + ast->_up(ast->GetVariableDecl(name)) + "\' " +
                getUserCodeFileLocation(ast, name, true) + "\n";
            string msg =
                "Global array " + global_info + " with undetermined size";
            dump_critical_message(GLOBL_ERROR_6(global_info), 0);
#if PROJDEBUG
            cout << "ERROR: " << msg << endl;
#endif
            *errorOut = true;
          }
        }
      } else if (ast->IsExtern(ast->GetVariableDecl(name)) &&
                 ast->GetInitializer(name) == nullptr) {
        // MER-1678 check non-defined constant global varibles
        string global_info = "\'" + ast->_up(ast->GetVariableDecl(name)) +
                             "\' " + getUserCodeFileLocation(ast, name, true) +
                             "\n";
        string msg = "Non initialized constant " + global_info;
        dump_critical_message(GLOBL_ERROR_5(global_info), 0);
#if PROJDEBUG
        cout << "ERROR: " << msg << endl;
#endif
        *errorOut = true;
      }
    }
  }
}

int global_convert_checker(void *decl, vector<void *> g_var, CSageCodeGen *ast,
                           CMarsIrV2 *mars_ir, int is_top, bool *errorOut,
                           bool hls_flow) {
  static set<void *> s_set_record;
  if (s_set_record.find(decl) != s_set_record.end()) {
    return 1;
  }

  vector<string> vs_var;
  for (auto var : g_var) {
    vs_var.push_back(ast->_up(var));
  }

  //  0. get the missing variables on the interface
  vector<string> vs_miss;
  vector<void *> v_miss;
  int i = 0;
  set<void *> s_global;
  map<void *, set<void *>> var2refs;
  collect_used_global_variable(decl, &s_global, &var2refs, ast, errorOut,
                               hls_flow);

  set<string> records;
  for (auto s_var : vs_var) {
    if (s_global.find(g_var[i]) != s_global.end()) {
      if (records.find(s_var) == records.end()) {
        vs_miss.push_back(s_var);
        v_miss.push_back(g_var[i]);
        records.insert(s_var);
      }
    }
    i++;
  }

  //  2. insert the formal argument list for the function declerartion
  {
    vector<void *> vec_func_decls;
    ast->GetNodesByType(nullptr, "preorder", "SgFunctionDeclaration",
                        &vec_func_decls);
    vector<void *> vec_same_decls;
    for (auto one_decl : vec_func_decls) {
      //          if (ast->GetFuncBody(one_decl)) continue;
      if (!ast->isSameFunction(one_decl, decl)) {
        continue;
      }
      vec_same_decls.push_back(one_decl);
      cout << "function without body: " << ast->_p(one_decl) << endl;

      if (static_cast<unsigned int>(!v_miss.empty()) != 0U) {
        //  ZP: 20160921
        //  error out if the decl to be modified is in the header file
        string sfile = ast->GetFileInfo_filename(one_decl, 1);
        string sExt;
        {
          vector<string> sub = str_split(sfile, ".");
          sExt = sub[sub.size() - 1];
        }

#if PROJDEBUG
        cout << "---- DECL G_VAR --- " << ast->_up(one_decl) << " at " << sfile
             << endl;
#endif
        if (sExt != "c" && sExt != "cpp") {
          string global_info;
          global_info += "    Function \'" + ast->GetFuncName(one_decl) + "\' ";
          global_info += getUserCodeFileLocation(ast, one_decl, true) + "\n";
          global_info += "    Global variable: " +
                         ast->_up(ast->GetVariableDecl(v_miss[0])) + " " +
                         getUserCodeFileLocation(ast, v_miss[0], true) + "\n";
          string msg = "Kernel subfunction can not be declared in the header "
                       "file, if it contains global variable\n" +
                       global_info;
          dump_critical_message(GLOBL_ERROR_1(global_info), 0);
#if PROJDEBUG
          cout << "ERROR: " << msg << endl;
#endif
          *errorOut = true;
          //  exit(0);
        }
      }
    }
    //  youxiang 20161006 check whether the kernel function is shared by host
    if ((is_top == 0) && (static_cast<unsigned int>(!v_miss.empty()) != 0U)) {
      set<void *> bound_set;
      mars_ir->get_function_bound(decl, &bound_set);
      if (bound_set.size() > 1) {
        string global_info = "    Function \'" + ast->GetFuncName(decl) + "\' ";
        global_info += getUserCodeFileLocation(ast, decl, true) + "\n";
        global_info += "    Global variable: " +
                       ast->_up(ast->GetVariableDecl(v_miss[0])) + " " +
                       getUserCodeFileLocation(ast, v_miss[0], true) + "\n";
        string msg = "Kernel subfunction can not be shared by host, "
                     "if it contains global variable\n" +
                     global_info;
        dump_critical_message(GLOBL_ERROR_2(global_info), 0);
#if PROJDEBUG
        cout << "ERROR: " << msg << endl;
#endif
        *errorOut = true;
      }
    }

    //  youxiang 20161027 check whether the global variable name will conflict
    //  with argument and variable name
    for (auto g_var : v_miss) {
      string var_name = ast->GetVariableName(g_var);
      //  do not check the local static variables
      if (ast->TraceUpToFuncDecl(g_var) == decl) {
        continue;
      }
      if (ast->check_name_declared_within_function(decl, var_name)) {
        string global_info = "    Function \'" + ast->GetFuncName(decl) + "\' ";
        global_info += getUserCodeFileLocation(ast, decl, true) + "\n";
        global_info +=
            "    Global variable: " + ast->_up(ast->GetVariableDecl(g_var)) +
            getUserCodeFileLocation(ast, g_var, true) + "\n";
        string msg = "Global variable name conflicts with local variables or "
                     "arguments\n" +
                     global_info;
        dump_critical_message(GLOBL_ERROR_4(global_info), 0);
#if PROJDEBUG
        cout << "ERROR: " << msg << endl;
#endif
        *errorOut = true;
      }
    }
  }

  //  3. recursion for the sub-functions
  set<void *> s_subfunc;
  {
    vector<void *> v_call;
    ast->GetNodesByType(decl, "preorder", "SgFunctionCallExp", &v_call);

    for (auto call : v_call) {
      void *curr_decl = ast->GetFuncDeclByCall(call);
      if ((curr_decl != nullptr) && (ast->GetFuncBody(curr_decl) != nullptr)) {
        if (ast->IsFromInputFile(curr_decl) != 0) {
          s_subfunc.insert(curr_decl);
        }
      }
    }
  }
  for (auto func : s_subfunc) {
    global_convert_checker(func, g_var, ast, mars_ir, 0, errorOut, hls_flow);
  }

  s_set_record.insert(decl);
  return 0;
}

//  1. get all the calls, insert the last argument using the searched name
//  2. insert the formal argument list for the function declerartion
//  3. recursion
//  4. remove the extern declarations
void global_convert_all_variable(void *decl, vector<void *> g_var,
                                 CSageCodeGen *ast, int is_top,
                                 set<void *> *handled, bool hls_flow) {
  if (handled->count(decl) > 0) {
    return;
  }

  vector<void *> calls;
  ast->GetFuncCallsFromDecl(decl, nullptr, &calls);
  if (is_top == 0) {
    //  check whether all the caller functions have been handled
    bool not_all_handled = false;
    for (auto call : calls) {
      void *caller = ast->TraceUpToFuncDecl(call);
      if (handled->count(caller) <= 0) {
        not_all_handled = true;
        break;
      }
    }
    if (not_all_handled) {
      //  if all the caller functions have NOT been handled, delay to handle
      //  the function
      return;
    }
  }
  handled->insert(decl);

  vector<string> vs_var;
  for (auto var : g_var) {
    vs_var.push_back(ast->_up(var));
  }

  //  0. get the missing variables on the interface
  vector<string> vs_miss;
  vector<void *> v_miss;
  int i = 0;
  set<void *> s_global;
  map<void *, set<void *>> var2refs;
  bool errorOut = false;
  collect_used_global_variable(decl, &s_global, &var2refs, ast, &errorOut,
                               hls_flow);

  set<string> records;
  for (auto s_var : vs_var) {
    if (s_global.find(g_var[i]) != s_global.end()) {
      if (records.find(s_var) == records.end()) {
        vs_miss.push_back(s_var);
        v_miss.push_back(g_var[i]);
        records.insert(s_var);
      }
    }
    i++;
  }

  //  1. get all the calls, insert the last argument using the searched name
  {
    for (auto call : calls) {
      for (auto s_var : vs_miss) {
        if (is_top == 0) {
          s_var = GLOBAL_VAR_PREFIX + s_var;
        }
        void *var = ast->find_variable_by_name(s_var, call);
        if (var != nullptr) {
          //                cout<<"=> call Orig: "  << ast->_p(call)
          //  <<endl;
          void *ref = ast->CreateVariableRef(var);
          if (is_top != 0) {
            ref = ast->CreateExp(V_SgAddressOfOp, ref);
          }
          ast->insert_argument_to_call(call, ref);
          //  replaced_call.insert(call);
          //                cout<<"=> call NEW: "  << ast->_p(call)
          //  <<endl;
        }
      }
    }
  }

  //  2. insert the formal argument list for the function declerartion
  vector<void *> v_new_var;
  {
    for (auto var : v_miss) {
      string s_var = ast->_up(var);
      s_var = GLOBAL_VAR_PREFIX + s_var;
      void *sg_type = ast->GetTypebyVar(var);
      //  youxiang 2016-09-22 do not convert an array to a pointer to array
      sg_type = ast->GetOrigTypeByTypedef(sg_type);
      sg_type = ast->CreatePointerType(sg_type);
      void *new_var = ast->CreateVariable(sg_type, s_var, var);
      ast->insert_param_to_decl(decl, new_var);
      v_new_var.push_back(new_var);
    }
  }

  //  2.1 replace the original reference to the global variable with the new
  //  variable
  {
    int i = 0;
    for (auto var : v_miss) {
      void *new_var = v_new_var[i];
      //  FIXME: youxiang 2016-09-22 the new_var is traced to new extern
      //  global variable, so the following reference update is incomplete the
      //  root reason is that the new parameter inserted isnot in the symbol
      //  table ast->replace_variable_references_in_scope(var, new_var, decl);

      {
        void *scope = decl;
        void *sg_init_old = var;
        void *sg_init_new = new_var;
        vector<void *> v_ref;
        ast->GetNodesByType(scope, "preorder", "SgVarRefExp", &v_ref);

        vector<void *> v_ref1;
        for (auto ref : v_ref) {
          if (sg_init_old == ast->GetVariableInitializedName(ref)) {
            v_ref1.push_back(ref);
          }
        }

        for (auto &ref : v_ref1) {
          void *new_ref = ast->CreateVariableRef(sg_init_new);
          //  Yuxin: 29-Sep-2016, if it is a VarRef in call, it should be new
          //  inserted.
          //  Do not replace it.
          //  Youxiang: 20161003 we will handle each function according
          //  to function call topology
          new_ref = ast->CreateExp(V_SgPointerDerefExp, new_ref);
          ast->ReplaceExp(ref, new_ref);
        }
      }

      i++;
    }
  }

  //  3. recursion for the sub-functions
  set<void *> s_subfunc;
  {
    vector<void *> v_call;
    ast->GetNodesByType(decl, "preorder", "SgFunctionCallExp", &v_call);

    for (auto call : v_call) {
      void *curr_decl = ast->GetFuncDeclByCall(call);
      if ((curr_decl != nullptr) && (ast->GetFuncBody(curr_decl) != nullptr)) {
        if (ast->IsFromInputFile(curr_decl) != 0) {
          s_subfunc.insert(curr_decl);
        }
      }
    }
  }
  for (auto func : s_subfunc) {
    global_convert_all_variable(func, g_var, ast, 0, handled, hls_flow);
  }

  //  4. delete redundant extern declaration
  {
    void *global = ast->GetGlobal(decl);
    //  youxiang 20161004 delete all the dead global variables
    vector<void *> vec_curr_refs;
    set<string> set_curr_vars;
    ast->GetNodesByType(global, "preorder", "SgVarRefExp", &vec_curr_refs);
    for (auto ref : vec_curr_refs) {
      void *var = ast->GetVariableInitializedName(ref);
      if (ast->IsGlobalInitName(var) == 0) {
        continue;
      }
      set_curr_vars.insert(ast->GetVariableName(var));
    }
    vector<void *> vec_all_refs;
    set<string> set_all_vars;
    ast->GetNodesByType(nullptr, "preorder", "SgVarRefExp", &vec_all_refs);
    for (auto ref : vec_all_refs) {
      void *var = ast->GetVariableInitializedName(ref);
      if (ast->IsGlobalInitName(var) == 0) {
        continue;
      }
      set_all_vars.insert(ast->GetVariableName(var));
    }
    set<string> set_global_var_to_argument;
    for (auto var : s_global) {
      set_global_var_to_argument.insert(ast->GetVariableName(var));
    }

    for (size_t idx = 0; idx < ast->GetChildStmtNum(global); idx++) {
      void *child = ast->GetChildStmt(global, idx);
      if ((ast->IsVariableDecl(child) == 0) ||
          (ast->IsFromInputFile(child) == 0)) {
        continue;
      }
      void *name = ast->GetVariableInitializedName(child);
      void *sg_type = ast->GetTypebyVar(name);
      if (ast->IsConstType(sg_type) != 0) {
        continue;
      }
      string var_name = ast->GetVariableName(name);
      bool dead_global = ast->IsExtern(child)
                             ? set_curr_vars.count(var_name) <= 0
                             : set_all_vars.count(var_name) <= 0;
      if (!dead_global && ast->IsExtern(child)) {
        //  youxiang 20161003 we can only trace to variable definition by
        //  variable reference
        dead_global = set_global_var_to_argument.count(var_name) > 0;
      }
      if (dead_global) {
        //  youxiang 20160927 bug684
        vector<void *> fp_list;
        void *dummy_global = ast->CreateFuncDecl(
            "void", "__merlin_dummy_" + ast->GetVariableName(name), fp_list,
            ast->GetGlobal(name), false, nullptr);
        ast->SetStatic(dummy_global);
        ast->ReplaceStmt(child, dummy_global);
        //  ast->RemoveStmt(child);
        for (auto var : s_global) {
          if (ast->GetVariableName(var) == var_name)
            s_global.erase(var);
        }
      }
    }
    for (auto var : s_global) {
      void *var_decl = ast->GetVariableDecl(var);
      if (!var_decl || ast->is_floating_node(var_decl) ||
          ast->TraceUpToFuncDecl(var_decl) == nullptr)
        continue;

      void *null_expr_stmt = ast->CreateStmt(
          V_SgExprStatement, ast->CreateExp(V_SgNullExpression));
      ast->ReplaceStmt(var_decl, null_expr_stmt);
    }
  }

  //  s_set_record.insert(decl);
}

//  #define DEBUG_PRINT_LOCAL
//  ////////////////////////////////////////  /
//  ZP: 20160920
//  Process
//  1. find all the non-constant global variables used in kernel (because
//  constant global will be handled in function inline)
//  2. for each one, create an extern decl in wrapper
//  3. insert the variable in both call and decl for the kernel function
//  4. recursively apply step 3 for the sub functions
//  Assumption:
//  1. kernel and host do not share sub functions (FIXME: need checker)
//  2. different kernels do not share sub functions (FIXME: need checker)
int global_variable_conversion_top(CSageCodeGen *codegen, void *pTopFunc,
                                   const CInputOptions &options) {
  string tool_version = options.get_option_key_value("-a", "tool_version");
  bool hls_flow = false;
  if ("vitis_hls" == tool_version || "vivado_hls" == tool_version) {
    hls_flow = true;
  }
  cout << "\n====== Global2Port Conversion ========>\n";
  CMarsIrV2 mars_ir;
  mars_ir.build_mars_ir(codegen, pTopFunc);
  CSageCodeGen *ast = codegen;

  vector<void *> kernels = mars_ir.get_top_kernels();

  for (auto kernel : kernels) {
    string s_wrapper = ast->get_wrapper_name_by_kernel(kernel);
    void *wrapper = ast->GetFuncDeclByName(s_wrapper);
    if (wrapper == nullptr) {
      continue;
    }

    //  1. find all the non-constant global variables used in kernel (because
    //  constant global will be handled in function inline)
    set<void *> s_global;
    map<void *, set<void *>> var2refs;
    bool errorOut = false;
    collect_used_global_variable(kernel, &s_global, &var2refs, ast, &errorOut,
                                 hls_flow);

#ifdef DEBUG_PRINT_LOCAL
    for (auto name : s_global) {
      void *decl = ast->GetVariableDecl(name);
      cout << "Global var: " << ast->_up(decl) << endl;
    }
#endif  //  DEBUG_PRINT_LOCAL

    map<void *, void *> mapVar2Scope;
    //  for (auto name : s_global) {
    //  void * scope = ast->get_the_access_scope_of_global_variable(name);
    //  mapVar2Scope[name] = scope;
    //  }

    for (auto name : s_global) {
      //  2. for each one, create an extern decl in wrapper
      //  youxiang 20160924 extern delcaration should have no initializer
      void *new_decl = ast->CreateVariableDecl(
          ast->GetTypebyVar(name), ast->GetVariableName(name), nullptr,
          ast->GetGlobal(wrapper), name);
      ast->SetExtern(new_decl);
      ast->InsertStmt(new_decl, wrapper);
    }

    for (auto name : s_global) {
      void *pragma =
          mars_ir.get_pragma_by_variable(ast->_up(name), kernel, "interface");

      if (pragma == nullptr) {
        string str_pragma = "ACCEL interface variable=" + ast->_up(name);
        void *body = ast->GetFuncBody(kernel);
        pragma = ast->CreatePragma(str_pragma, body);
        ast->PrependChild(body, pragma);
      }

      mars_ir.set_pragma_attribute(&pragma, "scope_type", "global");
      string s_depth = mars_ir.get_pragma_attribute(pragma, "depth");
      if (s_depth.empty()) {
        s_depth = "1";
      } else {
        s_depth = "1," + s_depth;
      }
      mars_ir.set_pragma_attribute(&pragma, "depth", s_depth);
      string s_max_depth = mars_ir.get_pragma_attribute(pragma, "max_depth");
      if (s_max_depth.empty()) {
        s_max_depth = "1";
      } else {
        s_max_depth = "1," + s_max_depth;
      }
      mars_ir.set_pragma_attribute(&pragma, "max_depth", s_max_depth);
    }

    //  3. insert the variable in both call and decl for the kernel function
    //  4. recursively apply step 3 for the sub functions
    vector<void *> v_global;
    for (auto name : s_global) {
      v_global.push_back(name);
    }
    //  youxiang : 20161003
    set<void *> handled;
    global_convert_all_variable(kernel, v_global, ast, 1, &handled, hls_flow);

    //  func_reference_replace(*ast, kernel);
  }
  return 1;
}

int global_variable_conversion_checker(CSageCodeGen *codegen, void *pTopFunc,
                                       const CInputOptions &options) {
  string tool_version = options.get_option_key_value("-a", "tool_version");
  bool hls_flow = false;
  if ("vitis_hls" == tool_version || "vivado_hls" == tool_version) {
    hls_flow = true;
  }
  cout << "\n====== Global2Port Conversion ========>\n";
  CMarsIrV2 mars_ir;
  mars_ir.build_mars_ir(codegen, pTopFunc);
  CSageCodeGen *ast = codegen;
  bool errorOut = false;
  string tool_type = options.get_option_key_value("-a", "impl_tool");
  vector<void *> kernels = mars_ir.get_top_kernels();
  for (auto kernel : kernels) {
    //  1. find all the non-constant global variables used in kernel (because
    //  constant global will be handled in function inline)
    set<void *> s_global;
    map<void *, set<void *>> var2refs;
    collect_used_global_variable(kernel, &s_global, &var2refs, ast, &errorOut,
                                 hls_flow);

    //  3. insert the variable in both call and decl for the kernel function
    //  4. recursively apply step 3 for the sub functions
    vector<void *> v_global;
    for (auto name : s_global) {
      v_global.push_back(name);
    }
    global_convert_checker(kernel, v_global, ast, &mars_ir, 1, &errorOut,
                           hls_flow);
    //  5. check pointer operation on global variables
    for (auto g_var : s_global) {
      void *var_type = ast->GetTypebyVar(g_var);
      if (ast->IsPointerType(var_type) == 0) {
        continue;
      }
      auto &refs = var2refs[g_var];
      bool has_lvalue = false;
      for (auto ref : refs) {
        if (ast->is_write_conservative(ref, false) != 0) {
          has_lvalue = true;
          break;
        }
      }
      if (has_lvalue) {
        string msg = "Cannot support pointer operation on global variable\n";
        string global_info =
            "    File location  : " + ast->get_file(g_var) + "\n";
        global_info +=
            "    Global variable: " + ast->_up(ast->GetVariableDecl(g_var)) +
            "\n";
        dump_critical_message(GLOBL_ERROR_3(global_info), 0);
#if PROJDEBUG
        cout << "ERROR: " << msg << endl;
#endif
        errorOut = true;
      }
    }

    //  7. check whether global variables contain unsupported types
    //  Is this check redundant?
    for (auto g_var : s_global) {
      void *var_type = ast->GetTypebyVar(g_var);
      void *base_type;
      vector<size_t> nSizes;
      ast->get_type_dimension(var_type, &base_type, &nSizes, g_var);
      if ((ast->IsReferenceType(var_type) != 0) ||
          (ast->IsReferenceType(base_type) != 0)) {
        string reason;
        string msg = "Cannot support reference type the kernel interface: ";
        string detail = "global variable: '" + ast->UnparseToString(g_var) +
                        "' " + getUserCodeFileLocation(ast, g_var, true) +
                        " \n";
        dump_critical_message(SYNCHK_ERROR_12(detail));
        errorOut = true;
      }
      void *unsupported_type = nullptr;
      string reason;
      if (ast->ContainsUnSupportedType(var_type, &unsupported_type, &reason,
                                       false) != 0) {
        string msg = "Cannot support kernel interface port:";
        string detail = "global variable \'" + ast->UnparseToString(g_var) +
                        "' " + getUserCodeFileLocation(ast, g_var, true) +
                        " with type '" + ast->UnparseToString(var_type) +
                        "' which contains " + reason + " (" +
                        ast->UnparseToString(unsupported_type) + ")";
        dump_critical_message(SYNCHK_ERROR_13(detail));
        errorOut = true;
      }
    }
  }
  if (errorOut) {
    throw std::exception();
  }
  return 0;
}
#undef DEBUG_PRINT_LOCAL
