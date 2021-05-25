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
#include <cmath>
#include <fstream>

#include "mars_opt.h"
#include "codegen.h"
#include "program_analysis.h"
#include "mars_ir.h"
#include "dse_utils.h"

#define MIN_TILE_SIZE 4
#define MAX_TILE_SIZE 16384

// Loop code set for a loop nest
vector<string> g_vec_tile_code;

// Check if we need to explore tiling for this loop
bool isTilable(CSageCodeGen *m_ast, void *loop, int tc, bool isInnermost) {
  // Ignore if the TC is too small
  if (tc < MIN_TILE_SIZE) {
    return false;
  }

  // Ignore fine-grained loops
  if (isInnermost) {
    return false;
  }

  return true;
}

static void traverser(CSageCodeGen *m_ast, void *loop, string loop_code) {
  int64_t tc = 0;
  if (m_ast->IsForStatement(loop) == 0) {
    return;
  }
  if (m_ast->get_loop_trip_count(loop, &tc) == 0) {
    return;
  }

  bool isFineGrained =
      (isFineGrainedLoop(m_ast, loop) && !hasNonFlattenFuncCall(m_ast, loop));

  if (!isTilable(m_ast, loop, tc, isFineGrained)) {
    return;
  }

  g_vec_tile_code.push_back(loop_code);
}

static void inserter(CSageCodeGen *m_ast, void *loop, string loop_code) {
  cout << "processing loop " << m_ast->_p(loop) << "...";

  // Build a design space by analyzing loop tripcount
  int64_t tc = 0;
  if (m_ast->IsForStatement(loop) == 0) {
    cout << "skip non for-loop" << endl;
    return;
  }
  if (m_ast->get_loop_trip_count(loop, &tc) == 0) {
    cout << "skip due to fail to analyze loop tripcount" << endl;
    return;
  }

  bool isFineGrained =
      (isFineGrainedLoop(m_ast, loop) && !hasNonFlattenFuncCall(m_ast, loop));

  if (!isTilable(m_ast, loop, tc, isFineGrained)) {
    cout << "skip due to non-tilable" << endl;
    return;
  }

  tc = (tc > MAX_TILE_SIZE) ? MAX_TILE_SIZE : tc;

  // Setup tiling design space
  string DS = loop_code + "=[x for x in [1";
  for (int64_t i = 2; i < tc; i *= 2) {
    DS += "," + std::to_string(i);
  }
  if ((tc & (tc - 1)) != 0) {
    // The upper-bound is not power of two, cover it separately
    DS += "," + std::to_string(tc);
  }
  DS += "]";
  string pragmaStr = "ACCEL TILE FACTOR=auto{options:" + DS + " ";

  // Setup conditions
  string cond;
  bool first = true;

  // Condition 1: Only one tile pragma is activated (>1) in one loop nest
  if (g_vec_tile_code.size() > 1) {
    cond = "if x==1 or ";
    for (auto other : g_vec_tile_code) {
      if (other == loop_code) {
        continue;
      }
      if (!first) {
        cond += " and ";
      }
      first = false;
      cond += other + "==1";
    }
  }

  // Condition 2: Valid only when pipeline pragma at higher loop levels
  // are not set to flatten
  vector<string> shadows = getShadowedPipelineParams(m_ast, loop);
  if (!shadows.empty()) {
    if (first) {
      cond = "if x==0 or ";
    }
    for (auto label : shadows) {
      if (!first) {
        cond += " and ";
      }
      first = false;
      cond += label + "!=\"flatten\"";
    }
  }
  pragmaStr += cond + "]; default:1}";

  void *newPragma = m_ast->CreatePragma(pragmaStr, m_ast->TraceUpToScope(loop));

  // Find the pipeline pragma at the same level as the insert position
  void *stmt = m_ast->GetChildStmt(m_ast->GetLoopBody(loop), 0);
  bool hasPipe = false;
  while ((stmt != nullptr) && (m_ast->IsPragmaDecl(stmt) != 0)) {
    string ori_str = m_ast->GetPragmaString(stmt);
    string str = ori_str;
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    if (str.find("pipeline") != string::npos) {
      hasPipe = true;
      break;
    }
    stmt = m_ast->GetNextStmt(stmt);
  }

  // It seems like we cannot insert a new pragma and replace it in
  // the same pass, so make sure replacing the pragmas only from
  // previous passes.
  cout << "inserted:" << endl;
  cout << "  " << pragmaStr << endl;
  if (!hasPipe) {
    m_ast->PrependChild(m_ast->GetLoopBody(loop), newPragma);
  } else {
    m_ast->InsertAfterStmt(newPragma, stmt);
  }
}

int ds_tiling_top(CSageCodeGen *m_ast, void *pTopFunc,
                  const CInputOptions &options) {
  cout << "==============================================" << endl;
  cout << "--=# Design Space Builder for Tiling Start#=--\n";
  cout << "==============================================" << endl;

  CMarsIr mMars_ir;
  mMars_ir.get_mars_ir(m_ast, pTopFunc, options, true);

  vector<CMirNode *> res;
  mMars_ir.get_topological_order_nodes(&res);

  int loop_idx = 0;
  for (auto node : res) {
    if (node->is_function) {
      continue;
    }
    auto *loop = m_ast->GetParent(node->ref);
    if (!IsOutermostForLoop(m_ast, loop)) {
      continue;
    }

    // Check if we already have tiling pragmas added by users
    // FIXME Now we ignore an entire loop nest that contains one or
    // more tiling, but this should be more flexible.
    bool hasExist = false;
    vector<void *> vecPragmas;
    m_ast->GetNodesByType(loop, "preorder", "SgPragmaDeclaration", &vecPragmas);
    for (auto pragma : vecPragmas) {
      string str = m_ast->GetPragmaString(pragma);
      transform(str.begin(), str.end(), str.begin(), ::tolower);
      if (str.find("til") != string::npos) {
        // Skip the loop nest that already has tiling/tile pragmas
        hasExist = true;
        cout << "skip due to existing tile pragma:" << endl;
        cout << "  " << str << endl;
        break;
      }
    }
    if (hasExist) {
      loop_idx++;
      continue;
    }

    g_vec_tile_code.clear();

    // 1st pass: collect loop code
    insertLoopDesignSpace(m_ast, &traverser, loop,
                          "T" + std::to_string(loop_idx));

    // 2nd pass: insert design space pragmas
    insertLoopDesignSpace(m_ast, &inserter, loop,
                          "T" + std::to_string(loop_idx));
    loop_idx++;
  }

  cout << "============================================" << endl;
  cout << "--=# Design Space Builder for Tiling End#=--\n";
  cout << "============================================" << endl;

  return 1;
}
