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

#include "xml_parser.h"
#include "cmdline_parser.h"
#include "file_parser.h"

#include "mars_opt.h"
#include "PolyModel.h"
#include "tldm_annotate.h"
#include "codegen.h"
#include "program_analysis.h"

#include "mars_ir.h"
#include "dse_utils.h"

static void inserter(CSageCodeGen *m_ast, void *loop, string loop_code) {
  cout << "processing " << m_ast->_p(loop) << "...";

  bool isFineGrained =
      isFineGrainedLoop(m_ast, loop) && !hasNonFlattenFuncCall(m_ast, loop);

  if (isFineGrained) {
    cout << "skip due to fine-grained loop" << endl;
    return;
  }

  vector<string> shadows = getShadowedPipelineParams(m_ast, loop);

  // Build a design space for coarse-grained loops
  string DS = loop_code + "=[x for x in [\"off\",\"\",\"flatten\"]";
  if (!shadows.empty()) {
    DS += " if x==\"off\" or ";
    bool first = true;
    for (auto label : shadows) {
      if (!first) {
        DS += " and ";
      }
      first = false;
      DS += label + "!=\"flatten\"";
    }
  }
  DS += "]";
  string part = "0 if x!=\"flatten\" else 1";
  string pragmaStr = "ACCEL PIPELINE auto{options:" + DS + "; order:" + part +
                     "; default:\"off\"}";
  void *newPragma = m_ast->CreatePragma(pragmaStr, m_ast->TraceUpToScope(loop));

  m_ast->PrependChild(m_ast->GetLoopBody(loop), newPragma);
}

int ds_pipeline_top(CSageCodeGen *m_ast, void *pTopFunc,
                    const CInputOptions &options) {
  cout << "=====================================================" << endl;
  cout << "--=# Design Space Builder for Loop Pipeline Start#=--\n";
  cout << "=====================================================" << endl;

  CMarsIr mMars_ir;
  mMars_ir.get_mars_ir(m_ast, pTopFunc, options, true);

  int loop_idx = 0;
  vector<CMirNode *> res;
  mMars_ir.get_topological_order_nodes(&res);
  for (auto node : res) {
    if (node->is_function) {
      continue;
    }
    auto *loop = m_ast->GetParent(node->ref);

    if (!IsOutermostForLoop(m_ast, loop)) {
      continue;
    }
    insertLoopDesignSpace(m_ast, inserter, loop,
                          "I" + std::to_string(loop_idx++));
  }

  cout << "=====================================================" << endl;
  cout << "--=# Design Space Builder for Loop Pipeline End#=--\n";
  cout << "=====================================================" << endl;

  return 1;
}
