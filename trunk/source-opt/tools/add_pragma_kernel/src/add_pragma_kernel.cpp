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
//  module           :   add pragma to llvm test
//  description      :
//  input            :
//  output           :
//  limitation       :
//  author           :   Shan
//  ******************************************************************************************//
#include <string>

#include "cmdline_parser.h"
#include "codegen.h"
#include "rose.h"

// using namespace std;

int add_pragma_kernel(CSageCodeGen *codegen, void *pTopFunc,
                      const CInputOptions &options) {
  printf("hello add pragma kernel\n");
  vector<void *> funcDecls = codegen->GetAllFuncDecl();
  for (auto d : funcDecls) {
    if (codegen->IsFromInputFile(d) != 0 &&
        codegen->isWithInHeaderFile(d) == 0 &&
        codegen->GetScope(d) == codegen->GetGlobal(d)) {
      string name = codegen->GetFuncName(d);
      string pragma_kernel = "ACCEL kernel name=\"" + name + "\"";
      printf("add pragma kernel : %s\n", name.c_str());
      void *new_pragma =
          codegen->CreatePragma(pragma_kernel, codegen->GetScope(d));
      codegen->InsertStmt(new_pragma, d);
    }
  }
  return 0;
}
