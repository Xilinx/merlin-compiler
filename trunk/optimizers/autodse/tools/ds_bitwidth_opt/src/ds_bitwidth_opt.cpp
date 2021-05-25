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


#include <time.h>
#include <sys/time.h>
#include <iterator>
#include <vector>
#include <set>
#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>

#include "xml_parser.h"
#include "cmdline_parser.h"
#include "file_parser.h"

#include "mars_opt.h"
#include "PolyModel.h"
#include "tldm_annotate.h"
#include "codegen.h"
#include "program_analysis.h"

#include "mars_ir_v2.h"
#include "dse_utils.h"

int64_t GetTotalDepth(string depth) {
  int64_t ret = 1;
  replace(depth.begin(), depth.end(), ',', ' ');
  std::istringstream iss(depth);
  vector<string> tokens{std::istream_iterator<string>{iss},
                        std::istream_iterator<string>{}};
  for (auto d : tokens) {
    ret *= stoi(d);
  }
  return ret;
}

void InsertBitwidthPragma(CSageCodeGen *m_ast, void *kernel) {
  set<string> setIgnored;
  vector<void *> vecPragmas;
  m_ast->GetNodesByType(kernel, "preorder", "SgPragmaDeclaration", &vecPragmas);
  for (auto pragma : vecPragmas) {
    string s = m_ast->GetPragmaString(pragma);
    string s_lower = s;
    transform(s_lower.begin(), s_lower.end(), s_lower.begin(), ::tolower);

    if (s_lower.find("bus_bitwidth") == string::npos &&
        s_lower.find("accel interface") == string::npos) {
      continue;
    }
    size_t l = s.find(" ", s.find("variable=")) - s.find("variable=") - 9;
    string varName = s.substr(s.find("variable=") + 9, l);

    // Ignore predefined fixed value
    if (s_lower.find("bus_bitwidth") != string::npos) {
      setIgnored.insert(varName);
      cout << "Ignore " << varName << " due to user-specified bitwidth";
      cout << endl;
      continue;
    }

    // Ignore array size < 1024
    int64_t depth = 0;
    if (s_lower.find("max_depth") != string::npos) {
      size_t l = s.find(" ", s.find("max_depth=")) - s.find("max_depth=") - 10;
      depth = GetTotalDepth(s.substr(s.find("max_depth=") + 10, l));
    } else if (s_lower.find("depth") != string::npos) {
      size_t l = s.find(" ", s.find("depth=")) - s.find("depth=") - 6;
      depth = GetTotalDepth(s.substr(s.find("depth=") + 6, l));
    } else {
      cout << "Both max_depth and depth are missing for " << varName;
      cout << endl;
      return;
    }

    if (depth < 1024) {
      setIgnored.insert(varName);
      cout << "Ignore " << varName << " due to array length < 1024";
      cout << endl;
    }
  }

  // Insert pragmas for function interface
  int idx = 0;
  for (int i = 0; i < m_ast->GetFuncParamNum(kernel); i++) {
    void *param = m_ast->GetFuncParam(kernel, i);
    void *ptype = m_ast->GetTypebyVar(param);
    string argName = m_ast->GetVariableName(param);

    // Ignore scalar
    if ((m_ast->IsPointerType(ptype) == 0) &&
        (m_ast->IsArrayType(ptype) == 0)) {
      cout << "Ignore " << argName << " due to scalar type";
      cout << endl;
      continue;
    }

    // Ignore other types
    void *ptypeBase = m_ast->get_array_base_type(param);
    if ((m_ast->IsIntType(ptypeBase) == 0) &&
        (m_ast->IsFloatType(ptypeBase) == 0) &&
        (m_ast->IsCharType(ptypeBase) == 0) &&
        (m_ast->IsLongLongType(ptypeBase) == 0)) {
      cout << "Ignore " << argName << " due to non-primitive types";
      cout << endl;
      continue;
    }

    if (setIgnored.find(argName) != setIgnored.end()) {
      continue;
    }

    cout << "Inserting bitwidth pragma of " << argName << endl;

    // Only explore the min (do nothing) and the max (coalescing)
    string DS = "auto{options:bw" + std::to_string(idx++) + "=" + "[32,512]; " +
                "default:32}";
    void *newPragma = m_ast->CreatePragma(
        "ACCEL interface variable=" + argName + " bus_bitwidth=" + DS,
        m_ast->GetGlobal(kernel));
    m_ast->InsertStmt(newPragma,
                      m_ast->GetChildStmt(m_ast->GetFuncBody(kernel), 0));
  }
}

int ds_bitwidth_opt_top(CSageCodeGen *m_ast, void *pTopFunc,
                        const CInputOptions &options) {
  cout << "====================================================" << endl;
  cout << "--=# Design Space Builder for Bitwidth Opt Start#=--\n";
  cout << "====================================================" << endl;

  CMarsIrV2 marsIR;
  marsIR.build_mars_ir(m_ast, pTopFunc);

  vector<void *> vecKernels = marsIR.get_top_kernels();
  for (auto kernel : vecKernels) {
    InsertBitwidthPragma(m_ast, kernel);
  }

  cout << "====================================================" << endl;
  cout << "--=# Design Space Builder for Bitwidth Opt End#=--\n";
  cout << "====================================================" << endl;

  return 1;
}
