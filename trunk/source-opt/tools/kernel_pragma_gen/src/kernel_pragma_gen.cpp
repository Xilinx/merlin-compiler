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


//  ******************************************************************************************//
//  module           :   kernel wrapper
//  description      :   generate a wrapper for specified kernel
//  input            :
//  output           :
//  limitation       :
//  author           :   Han
//  ******************************************************************************************//
#include <iostream>
#include <string>
#include <boost/algorithm/string.hpp>
#include "math.h"
#include "kernel_pragma_gen.h"
#include "analPragmas.h"
#include "codegen.h"
#include "ir_types.h"
#include "mars_ir_v2.h"
#include "mars_message.h"
#include "mars_opt.h"
#include "rose.h"
using std::exception;
extern void GetTLDMInfo_withPointer4(void *sg_node, void *pArg);

//  -----------------------------------------------------------------------------
//  check task pragma lccation, must followed by afunction declaration
//  -----------------------------------------------------------------------------
void check_task_pragma_location(CSageCodeGen *codegen, void *pragma) {
  void *next_stmt = codegen->GetNextStmt(pragma, false);
  while (codegen->IsPragmaDecl(next_stmt) != 0) {
    next_stmt = codegen->GetNextStmt(next_stmt, false);
  }
  if (codegen->IsFunctionDeclaration(next_stmt) == 0) {
    string msg = "Task pragma : " + codegen->UnparseToString(pragma) + " " +
                 getUserCodeFileLocation(codegen, pragma, true) + "\n";
    msg += "    must followed by one function declaration.\n";
    dump_critical_message(PRAGM_ERROR_1(msg));
    throw std::exception();
    return;
  }
}

//  -----------------------------------------------------------------------------
//  check task pragma lccation, must followed by a function call
//  the funciton name can not be "kernel"
//  -----------------------------------------------------------------------------
void check_kernel_pragma_location(CSageCodeGen *codegen, void *pragma) {
  void *next_stmt = codegen->GetNextStmt(pragma, false);
  if (codegen->IsFunctionDeclaration(next_stmt) != 0) {
    string kernel_name = codegen->GetFuncName(next_stmt);
    if (kernel_name == "kernel") {
      string msg = "Find illegal kernel name \"" + kernel_name + "\".\n";
      msg += "Please change to another kernel name.";
      dump_critical_message(PRAGM_ERROR_7(msg));
      throw std::exception();
    }
  } else {
    void *call = codegen->GetFuncCallInStmt(next_stmt);
    if (call != nullptr) {
      string kernel_name = codegen->GetFuncNameByCall(call);
      if (kernel_name == "kernel") {
        string msg = "Find illegal kernel name \"" + kernel_name + "\".\n";
        msg += "Please change to another kernel name.";
        dump_critical_message(PRAGM_ERROR_7(msg));
        throw std::exception();
      }
    } else {
      string msg = "Kernel pragma : " + codegen->UnparseToString(pragma) + " " +
                   getUserCodeFileLocation(codegen, pragma, true) + "\n";
      msg +=
          "    must followed by one function declaration or function call.\n";
      dump_critical_message(PRAGM_ERROR_2(msg));
      throw std::exception();
    }
  }
}

//  -----------------------------------------------------------------------------
//  check repeat kernel pragmas, two kernel pragma can not be used on the same
//  function name
//  -----------------------------------------------------------------------------
void check_repeat_pragmas(CSageCodeGen *codegen, vector<void *> vec_pragmas) {
  vector<string> vec_func_names;
  for (size_t i = 0; i < vec_pragmas.size(); i++) {
    void *next_stmt = codegen->GetNextStmt(vec_pragmas[i], false);
    string func_name;
    if (codegen->IsFunctionDeclaration(next_stmt) != 0) {
      func_name = codegen->GetFuncName(next_stmt);
    } else {
      vector<void *> vec_expr;
      codegen->GetNodesByType(next_stmt, "preorder", "SgFunctionCallExp",
                              &vec_expr);
      void *expr = nullptr;
      if (vec_expr.size() == 1) {
        expr = vec_expr[0];
        func_name = codegen->GetFuncNameByCall(expr);
      }
    }
    void *pragma_re;
    if (vec_func_names.empty()) {
      vec_func_names.push_back(func_name);
    } else {
      int flag = 0;
      for (size_t j = 0; j < vec_func_names.size(); j++) {
        if (vec_func_names[j] == func_name) {
          flag = 1;
          pragma_re = vec_pragmas[j];
        }
      }
      if (flag == 0) {
        vec_func_names.push_back(func_name);
      } else {
        void *pragma1 = pragma_re;
        void *pragma2 = vec_pragmas[i];
        string msg = "Pragma : " + codegen->UnparseToString(pragma1) + " " +
                     getUserCodeFileLocation(codegen, pragma1, true) + "\n";
        msg += "    and pragma : " + codegen->UnparseToString(pragma2) + " " +
               getUserCodeFileLocation(codegen, pragma2, true) + "\n";
        msg += "    declared on the same function :" + func_name + "\n";
        dump_critical_message(PRAGM_ERROR_3(msg));
        throw std::exception();
      }
    }
  }
}

//  -----------------------------------------------------------------------------
//  check kernel and task pragma legality and  unify kernel and task pragmas
//  transfer task pragma to kernel pragma
//  -----------------------------------------------------------------------------

int kernel_pragma_gen(CSageCodeGen *codegen, void *pTopFunc,
                      const CInputOptions &options) {
  printf("Hello kernel_pragma_gen...\n");

  //  Youxiang: 20171222
  //  add_missing_kernel_pragma(codegen, pTopFunc);

  size_t i;
  int count_kernel = 0;
  vector<void *> vec_pragmas;
  vector<pair<void *, string>> vecTldmPragmas;
  codegen->TraverseSimple(pTopFunc, "preorder", GetTLDMInfo_withPointer4,
                          &vecTldmPragmas);
  for (i = 0; i < vecTldmPragmas.size(); i++) {
    string sFilter;
    string sCmd;
    map<string, pair<vector<string>, vector<string>>> mapParams;
    tldm_pragma_parse_whole(vecTldmPragmas[i].second, &sFilter, &sCmd,
                            &mapParams);
    boost::algorithm::to_upper(sFilter);
    boost::algorithm::to_upper(sCmd);

    if (CMOST_PRAGMA != sFilter && ACCEL_PRAGMA != sFilter &&
        TLDM_PRAGMA != sFilter) {
      continue;
    }
    if ("TASK" == sCmd) {  //  FIXME task pragma only defined only before call
                           //  or basic block
      check_task_pragma_location(codegen, vecTldmPragmas[i].first);
    }
    if ("KERNEL" == sCmd) {
      check_kernel_pragma_location(codegen, vecTldmPragmas[i].first);
      vec_pragmas.push_back(vecTldmPragmas[i].first);
      count_kernel++;
    }
  }
  if (count_kernel == 0) {
    string msg = "Did not find kernel pragma!";
    dump_critical_message(PRAGM_ERROR_5(msg));
    throw std::exception();
    return 0;
  }
  check_repeat_pragmas(codegen, vec_pragmas);
  //  kernel --> kernel
  //  if kernel pragma defined before function call, transfer to before function
  //  declaration
  for (i = 0; i < vecTldmPragmas.size(); i++) {
    string sFilter;
    string sCmd;
    map<string, pair<vector<string>, vector<string>>> mapParams;
    tldm_pragma_parse_whole(vecTldmPragmas[i].second, &sFilter, &sCmd,
                            &mapParams);
    boost::algorithm::to_upper(sFilter);
    boost::algorithm::to_upper(sCmd);

    if (CMOST_PRAGMA != sFilter && ACCEL_PRAGMA != sFilter &&
        TLDM_PRAGMA != sFilter) {
      continue;
    }
    if ("KERNEL" != sCmd) {
      continue;
    }
    string user_kernel_name;
#if PROJDEBUG
    printf("Find a kernel pragma : %s\n", vecTldmPragmas[i].second.c_str());
#endif
    string pragma_kernel = vecTldmPragmas[i].second;
    void *orig_task_decl = vecTldmPragmas[i].first;
    string kernel_name;
    if (!mapParams["name"].first.empty()) {
      kernel_name = mapParams["name"].first[0];
      string msg =
          "Ignored the unknown option 'name' in the following kernel pragma:\n";
      msg += "    #pragma " + pragma_kernel + " " +
             getUserCodeFileLocation(codegen, orig_task_decl, true);
      dump_critical_message(PRAGM_WARNING_1(msg));
    }
    //  youxiang 20161201 check whether the following is function declaration.
    void *next_stmt = codegen->GetNextStmt(orig_task_decl, false);
    if (codegen->IsFunctionDeclaration(next_stmt) != 0) {
      continue;
    }
  }
  return 0;
}
