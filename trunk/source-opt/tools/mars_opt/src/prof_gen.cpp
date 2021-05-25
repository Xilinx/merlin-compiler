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


#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <map>

#include "codegen.h"
#define USED_CODE_IN_COVERAGE_TEST 0

//  extern void GetTLDMInfo_withPointer1(void * sg_node, void * pArg);
void GetTLDMInfo_withPointer2(void *sg_node, void *pArg) {
  CSageCodeGen sggen;
  string str = sggen.GetPragmaString(sg_node);

  if (!str.empty()) {
    void *sg_pragma_decl = sg_node;  //  sggen.GetParent(sg_node);
    vector<pair<void *, string>> *pPragmaList =
        static_cast<vector<pair<void *, string>> *>(pArg);
    pPragmaList->push_back(pair<void *, string>(sg_pragma_decl, str));
  }
}
//  extern int  header_gen(CSageCodeGen & codegen, void * sg_scope, int
//  debug_gen);
int header_gen(CSageCodeGen *codegen, void *sg_scope, string func_name) {
  string header_name = "\n#include \"x" + func_name + "_core_slv.h\"";
  codegen->AddHeader(header_name, sg_scope);
  return 1;
}

void prof_gen(CSageCodeGen *codegen, void *pTopFunc) {
  //  find "graph_end" pragma & insert "cl_flush()"
  size_t i;
  vector<pair<void *, string>> vecTldmPragmas;
  codegen->TraverseSimple(pTopFunc, "preorder", GetTLDMInfo_withPointer2,
                          &vecTldmPragmas);

  for (i = 0; i < vecTldmPragmas.size(); i++) {
    printf("Pragma: %s\n", vecTldmPragmas[i].second.c_str());
    string sFilter;
    string sCmd;
    map<string, pair<vector<string>, vector<string>>> mapParams;
    tldm_pragma_parse_whole(vecTldmPragmas[i].second, &sFilter, &sCmd,
                            &mapParams);
    if (("tldm" != sFilter && "cmost" != sFilter) ||
        (sCmd.find("graph_block") == string::npos)) {
      continue;
    }
    string pragma_str = vecTldmPragmas[i].second;

    if (pragma_str.find('"') == string::npos) {
      cerr << "Cannot extract graph_block name" << endl;
      exit(1);
    }

    int pos1 = static_cast<int>(pragma_str.find('"'));
    int pos2 = static_cast<int>(pragma_str.rfind('"'));
    string block_name = pragma_str.substr(pos1 + 1, pos2 - pos1 - 1);

    string prof_call = block_name + "_prof";

    void *sg_scope = vecTldmPragmas[i].first;
    void *blockStmt = codegen->GetNextStmt(sg_scope);
    if (codegen->IsBasicBlock(blockStmt) == 0) {
      cerr << "Pragama graph_block followed by non-basicblock" << endl;
      exit(1);
    }
    while (true) {
      if (codegen->IsScopeStatement(sg_scope) != 0) {
        break;
      }
      { sg_scope = codegen->GetParent(sg_scope); }
    }

    vector<void *> sg_init_list;

    void *profCallExp = codegen->CreateFuncCall(
        prof_call, codegen->GetTypeByString("void"), sg_init_list, sg_scope);
    void *profCallStmt = codegen->CreateStmt(V_SgExprStatement, profCallExp);

    //  codegen->InsertStmt(flushCallStmt, vecTldmPragmas[i].first);
    //  to check
    codegen->InsertStmt(profCallStmt, blockStmt);
    //  create function declaration
    vector<void *> fp_list;

    //  To check: sg_scope is sg_global
    void *sg_global = codegen->GetGlobal(sg_scope);

    void *prof_func_decl = codegen->CreateFuncDecl("void", prof_call, fp_list,
                                                   sg_global, true, nullptr);
    codegen->PrependChild(sg_global, prof_func_decl);

    //  remove the original statement
    codegen->RemoveChild(blockStmt);
  }
}

void cpu_timer_gen(CSageCodeGen *codegen, void *pTopFunc, int debug_gen) {
  //  find "graph_end" pragma & insert "cl_flush()"
  size_t i;
  vector<pair<void *, string>> vecTldmPragmas;
  codegen->TraverseSimple(pTopFunc, "preorder", GetTLDMInfo_withPointer2,
                          &vecTldmPragmas);
  codegen->RegisterType("__CMOST_OPENCL_TYPE__");

  set<void *> scope;

  for (i = 0; i < vecTldmPragmas.size(); i++) {
    //    printf("Pragma: %s\n", vecTldmPragmas[i].second.c_str());
    string sFilter;
    string sCmd;
    map<string, pair<vector<string>, vector<string>>> mapParams;
    tldm_pragma_parse_whole(vecTldmPragmas[i].second, &sFilter, &sCmd,
                            &mapParams);
    if (("tldm" != sFilter && "cmost" != sFilter) ||
        ((sCmd.find("graph_begin") == string::npos) &&
         (sCmd.find("graph_end") == string::npos))) {
      continue;
    }
    string pragma_str = vecTldmPragmas[i].second;

    void *sg_scope = vecTldmPragmas[i].first;
    //  void* blockStmt = codegen->GetNextStmt(sg_scope);
    void *blockStmt = sg_scope;  //  codegen->GetNextStmt(sg_scope);
    while (true) {
      if (codegen->IsScopeStatement(sg_scope) != 0) {
        break;
      }
      { sg_scope = codegen->GetParent(sg_scope); }
    }

    void *timer_decl = nullptr;
    if (sCmd.find("graph_begin") != string::npos) {
      timer_decl = codegen->CreateVariableDecl(
          "__CMOST_OPENCL_TYPE__", "__CMOST_CPU_TIMER_START__", 0, sg_scope);
    } else if (sCmd.find("graph_end") != string::npos) {
      timer_decl = codegen->CreateVariableDecl(
          "__CMOST_OPENCL_TYPE__", "__CMOST_CPU_TIMER_END__", 0, sg_scope);
    }
    if (timer_decl != nullptr) {
      codegen->InsertStmt(timer_decl, blockStmt);
    }
    void *curr_scope = codegen->GetGlobal(sg_scope);

    if (scope.count(curr_scope) == 0) {
      scope.insert(curr_scope);
#if USED_CODE_IN_COVERAGE_TEST
      header_gen(codegen, curr_scope, debug_gen);
#endif
    }
  }
}
