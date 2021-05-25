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


#include "cmdline_parser.h"
#include "file_parser.h"
#include "xml_parser.h"

#include "PolyModel.h"
#include "codegen.h"
#include "mars_opt.h"
#include "tldm_annotate.h"

using std::string;
void access_ref_promote_to_bram(CSageCodeGen *codegen, void *access,
                                int ref_index, int start, int size) {
  static int s_memcpy_header_included = 0;

  //  1. get var_ref
  assert(codegen->IsPntrArrRefExp(access));
  void *var;
  codegen->get_var_from_pntr_ref(access, &var);
  int is_write = codegen->is_write_ref(access);

  //  2. get surrounding for loop;
  void *for_loop = codegen->GetEnclosingNode("ForLoop", access);
  void *bb = codegen->GetEnclosingNode("BasicBlock", for_loop);

  //  3. get base type
  string sType = codegen->get_var_ref_base_type(var);
  sType = sType + " [" + my_itoa(size) + "]";

  //  4. create and insert new buffer var decl
  string sName = codegen->UnparseToString(var) + "_buf" + my_itoa(ref_index);
  void *buf_var_decl = codegen->CreateVariableDecl(sType, sName, NULL, bb);
  codegen->PrependChild(bb, buf_var_decl, true);

  //  5. insert update for the buffer
  {
    if (s_memcpy_header_included != 0) {
      void *file_global = codegen->GetGlobal(access);
      codegen->AddHeader("\n#include <string.h>\n", file_global);
      s_memcpy_header_included = 1;
    }
    // TODO(youxiang): global pointer need to be refined
    vector<void *> vec_index;
    vec_index.push_back(codegen->CreateConst(0));
    vector<void *> vec_index1;
    vec_index1.push_back(codegen->CreateConst(0));
    void *global_ref = codegen->CreateExp(
        V_SgAddressOfOp,
        codegen->CreateArrayRef(codegen->CopyExp(var), vec_index));
    void *local_ref = codegen->CreateVariableRef(buf_var_decl);
    void *size_ref = codegen->CreateExp(V_SgMultiplyOp, codegen->CreateConst(4),
                                        codegen->CreateConst(size));

    vector<void *> param_list;
    if (is_write != 0) {
      param_list.push_back(global_ref);
      param_list.push_back(local_ref);
      param_list.push_back(size_ref);
    } else {
      param_list.push_back(local_ref);
      param_list.push_back(global_ref);
      param_list.push_back(size_ref);
    }
    void *memcpy_exp = codegen->CreateFuncCall(
        "memcpy", codegen->GetTypeByString("void"), param_list, bb);
    void *sg_stmt = codegen->CreateStmt(V_SgExprStatement, memcpy_exp);
    if (is_write != 0) {
      codegen->InsertAfterStmt(sg_stmt, for_loop);
    } else {
      codegen->InsertStmt(sg_stmt, for_loop);
    }
  }

  //  6. replace old access
  {
    void *it = codegen->GetLoopIterator(for_loop);
    vector<void *> vec_index;
    vec_index.push_back(codegen->CreateVariableRef(it));
    //  vec_index.push_back(codegen->CreateConst(1));
    void *new_ref = codegen->CreateArrayRef(
        codegen->CreateVariableRef(buf_var_decl), vec_index);
    codegen->ReplaceExp(access, new_ref);
  }
}

void access_ref_promote_to_reg(CSageCodeGen *codegen, void *access,
                               int ref_index) {
  //  1. get var_ref
  assert(codegen->IsPntrArrRefExp(access));
  void *var;
  codegen->get_var_from_pntr_ref(access, &var);
  int is_write = codegen->is_write_ref(access);
  void *sg_scope = codegen->GetScope(access);

  //  2. get surrounding bb;
  void *bb = codegen->GetEnclosingNode("BasicBlock", access);

  //  3. get base type
  string sType = codegen->get_var_ref_base_type(var);

  //  4. create and insert new buffer var decl
  string sName = codegen->UnparseToString(var) + "_" + my_itoa(ref_index);
  void *init_value = is_write != 0 ? NULL : codegen->CopyExp(access);
  void *buf_var_decl =
      codegen->CreateVariableDecl(sType, sName, init_value, sg_scope);
  codegen->PrependChild(bb, buf_var_decl, true);

  //  5. insert update for write ref
  if (is_write != 0) {
    void *sg_stmt =
        codegen->CreateStmt(V_SgAssignStatement, codegen->CopyExp(access),
                            codegen->CreateVariableRef(buf_var_decl));
    codegen->AppendChild(bb, sg_stmt);
  }

  //  6. replace old access
  void *new_ref = codegen->CreateVariableRef(buf_var_decl);
  codegen->ReplaceExp(access, new_ref);
}

int data_reuse_top(CSageCodeGen *codegen, void *pTopFunc,
                   const CInputOptions &options) {
  size_t i;
  size_t j;
  printf("Hello Data Reuse ... \n");

  //  1. get mars IR info

  vector<void *> vecFunc;
  //  codegen->GetNodesByType(pTopFunc, "preorder", "SgVarRefExp", &vecRefSet);
  codegen->GetNodesByType(pTopFunc, "preorder", "SgFunctionDeclaration",
                          &vecFunc);

  vector<string> vecDataSet;
  for (i = 0; i < vecFunc.size(); i++) {
    void *body = codegen->GetFuncBody(vecFunc[i]);

    if (body == nullptr) {
      continue;
    }

    vector<void *> vecRefSet;
    codegen->GetNodesByType(vecFunc[i], "preorder", "SgPntrArrRefExp",
                            &vecRefSet);

    for (j = 0; j < vecRefSet.size(); j++) {
      void *ref = vecRefSet[j];
      if (codegen->IsPntrArrRefExp(codegen->GetParent(ref)) != 0) {
        continue;
      }

      printf("EXP: %s\n", codegen->UnparseToString(ref).c_str());

      void *var;
      codegen->get_var_from_pntr_ref(ref, &var);

      if (-1 == str_vector_find(vecDataSet, codegen->UnparseToString(var))) {
        vecDataSet.push_back(codegen->UnparseToString(var));
      }
    }
  }

  //  2. merge reference

  //  3. calc reuse distance

  //  4. data reuse chain generation

  //  5. register-level data reuse gen
  {
    for (i = 0; i < vecDataSet.size(); i++) {
      printf("Var: %s\n", vecDataSet[i].c_str());

      string sVar = vecDataSet[i];

      vector<void *> vecRef;
      codegen->GetNodesByType(pTopFunc, "preorder", "SgPntrArrRefExp", &vecRef);
      int ref_idx = 0;
      for (j = 0; j < vecRef.size(); j++) {
        void *pntr = vecRef[j];
        if (codegen->IsPntrArrRefExp(codegen->GetParent(pntr)) != 0) {
          continue;
        }
        void *var;
        codegen->get_var_from_pntr_ref(pntr, &var);
        if (codegen->UnparseToString(var) != sVar) {
          continue;
        }

        int is_write = codegen->is_write_ref(pntr);
        string write_flag = is_write != 0 ? "w" : "r";

        printf("Exp: %s: %s\n", write_flag.c_str(),
               codegen->UnparseToString(pntr).c_str());
        if ("a" == sVar || "c" == sVar) {
          access_ref_promote_to_bram(codegen, pntr, ref_idx, 0, 100);
        } else {
          access_ref_promote_to_reg(codegen, pntr, ref_idx);
        }
        ref_idx++;
      }
    }
  }

  //  6. BRAM-level data reuse gen

  return 1;
}
