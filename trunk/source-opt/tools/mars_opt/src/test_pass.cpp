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


#include <fstream>
#include <iostream>

#include "cmdline_parser.h"
#include "file_parser.h"
#include "xml_parser.h"

#include "PolyModel.h"
#include "codegen.h"
#include "mars_opt.h"
#include "tldm_annotate.h"

string linearMap2Str(CSageCodeGen *codegen,
                     const map<string, int> &mapVar2Coeff) {
  string str;
  map<string, int>::const_iterator it;
  for (it = mapVar2Coeff.begin(); it != mapVar2Coeff.end(); it++) {
    if (it != mapVar2Coeff.begin()) {
      str += " ";
    }
    if (it->first != "NULL") {
      str += string(my_itoa(it->second)) + "*" + it->first;
    } else {
      str += "NULL";
    }
  }
  return str;
}

int test_linear_exp(CSageCodeGen *codegen, void *pTopFunc,
                    const CInputOptions &options) {
  size_t i;
  size_t j;
  vector<void *> vec_exps;
  codegen->GetNodesByType(pTopFunc, "preorder", "SgPntrArrRefExp", &vec_exps);

  for (i = 0; i < vec_exps.size(); i++) {
    void *exp = vec_exps[i];

    vector<void *> indexes;
    codegen->get_index_from_pntr_ref(exp, &indexes);

    string index_list;

    for (j = 0; j < indexes.size(); j++) {
      if (j != 0) {
        index_list += "\n";
      }

      map<string, int> mapVar2Coeff;
      int ret = codegen->analyze_linear_expression(indexes[j], &mapVar2Coeff);

      string linear_str = linearMap2Str(codegen, mapVar2Coeff);

      index_list += "     " + codegen->UnparseToString(indexes[j]) + " = " +
                    linear_str + (ret != 0 ? " linear" : " nonlinear");
    }

    printf("exp: %s\n%s\n", codegen->UnparseToString(exp).c_str(),
           index_list.c_str());
  }

  return 1;
}

int test_pass_top(CSageCodeGen *codegen, void *pTopFunc,
                  const CInputOptions &options) {
  return test_linear_exp(codegen, pTopFunc, options);
}
