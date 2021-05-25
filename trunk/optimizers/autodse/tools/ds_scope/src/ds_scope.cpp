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
#include <map>
#include <set>
#include <algorithm>
#include <fstream>
#include <unordered_map>

#include "cmdline_parser.h"
#include "file_parser.h"

#include "mars_opt.h"
#include "PolyModel.h"
#include "tldm_annotate.h"
#include "codegen.h"
#include "program_analysis.h"
#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/rapidjson.h"

#include "mars_ir.h"
#include "dse_utils.h"
using std::unordered_map;

#define DS_SCOPE_DEBUG 0
#if DS_SCOPE_DEBUG
#define DEBUG_PRINT(x)                                                         \
  do {                                                                         \
    std::cerr << x << std::endl;                                               \
  } while (0)
#else
#define DEBUG_PRINT(x)
#endif

unordered_map<string, string> fetch_auto_pragmas(CSageCodeGen *m_ast,
                                                 void *pTopFunc) {
  vector<void *> vecPragmas;
  unordered_map<string, string> autoPragmas;
  m_ast->GetNodesByType(pTopFunc, "preorder", "SgPragmaDeclaration",
                        &vecPragmas);
  for (auto pragma : vecPragmas) {
    string str = m_ast->GetPragmaString(pragma);
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    // One pragma may contain multiple "auto", but we do not need to
    // warry about that here
    if (str.find("accel") == string::npos ||
        str.find("auto{") == string::npos) {
      continue;
    }
    str = m_ast->GetPragmaString(pragma);

    auto scope = m_ast->TraceUpToLoopScope(pragma);
    if (scope == nullptr) {
      scope = m_ast->TraceUpToFuncDecl(pragma);
    }
    if (scope == nullptr) {
      cerr << "[Error] Cannot identify the scope of " << str << endl;
      continue;
    }
    string id = getTopoLocation(m_ast, static_cast<SgNode *>(scope));
    DEBUG_PRINT("topo_id: " + id);
    DEBUG_PRINT("scope: " + m_ast->_p(scope));
    DEBUG_PRINT("pragma: " + str);
    autoPragmas[str] = id;
  }
  return autoPragmas;
}

int ds_scope_top(CSageCodeGen *m_ast, void *pTopFunc,
                 const CInputOptions &options) {
  cout << "==============================================" << endl;
  cout << "--=# Design Space Scope Identification Start#=--\n";
  cout << "==============================================" << endl;

  CMarsIr mMars_ir;
  mMars_ir.get_mars_ir(m_ast, pTopFunc, options, true);
  vector<CMirNode *> res;
  mMars_ir.get_topological_order_nodes(&res);

  rapidjson::Document doc(rapidjson::kObjectType);
  auto &alloc = doc.GetAllocator();

  auto pragmas = fetch_auto_pragmas(m_ast, pTopFunc);
  for (auto pragma : pragmas) {
    rapidjson::Value key(pragma.first.c_str(), alloc);
    rapidjson::Value val(pragma.second.c_str(), alloc);
    doc.AddMember(key, val, alloc);
  }

  // Write to a file
  std::ofstream filep("auto_pragmas.json");
  rapidjson::OStreamWrapper osw(filep);
  rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
  doc.Accept(writer);

  cout << "============================================" << endl;
  cout << "--=# Design Space Scope Identification End#=--\n";
  cout << "============================================" << endl;

  return 1;
}
