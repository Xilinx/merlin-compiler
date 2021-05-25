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

#include "mars_opt.h"
#include "codegen.h"
#include "program_analysis.h"
#include "mars_ir.h"
#include "dse_utils.h"

static void inserter(CSageCodeGen *m_ast, void *loop, string loop_code) {
  cout << "processing " << m_ast->_p(loop) << "...";

  void *stmt = m_ast->GetChildStmt(m_ast->GetLoopBody(loop), 0);
  while ((stmt != nullptr) && (m_ast->IsPragmaDecl(stmt) != 0)) {
    string str = m_ast->GetPragmaString(stmt);
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    if (str.find("parallel") != string::npos) {
      // Skip the loop that already has a parallel pragma
      cout << "skip due to existing parallel pragma:" << endl;
      cout << "  " << str << endl;
      return;
    }
    stmt = m_ast->GetNextStmt(stmt);
  }

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

  cout << "tripcount " << tc << ", ";

  if (tc <= 1) {
    cout << "skip due to abnormal loop tripcount" << endl;
    return;
  }

  // Limit the parallel factor to 128 due to
  // resource and design complexity consieration.
  // Should use a more smart wat to determine it.
  int64_t max_factor = (tc <= 128) ? tc : 128;

  // Setup design space
  string DS = loop_code + "=[x for x in ";
  if (max_factor < 9) {
    DS += "range(1," + std::to_string(max_factor - 1) + ")";
  } else {
    DS += "range(1,10)+[";
    bool first = true;
    for (int64_t i = 16; i <= max_factor; i *= 2) {
      if (!first) {
        DS += ",";
      }
      first = false;
      DS += std::to_string(i);
    }

    // Add tripcount to cover fully unroll
    if ((max_factor & (max_factor - 1)) != 0) {
      if (!first) {
        DS += ",";
      }
      DS += std::to_string(max_factor);
    }
    DS += "]";
  }

  // Setup conditions by looking for the corresponding tiling pragmas
  // Condition 1: tiling factor * parallel factor <= tc
  string tile_ds_name;
  stmt = m_ast->GetChildStmt(m_ast->GetLoopBody(loop), 0);
  bool hasTile = false;
  while ((stmt != nullptr) && (m_ast->IsPragmaDecl(stmt) != 0)) {
    string ori_str = m_ast->GetPragmaString(stmt);
    string str = ori_str;
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    if (str.find("tile") != string::npos) {
      tile_ds_name = getDesignSpaceParamName(ori_str);
      DS += " if x*" + tile_ds_name + "<=" + std::to_string(tc);
      hasTile = true;
      break;
    }
    stmt = m_ast->GetNextStmt(stmt);
  }

  // Condition 2: valid only when pipeline pragma at higher loop levels
  // are not set to flatten
  vector<string> shadows = getShadowedPipelineParams(m_ast, loop);
  if (!shadows.empty()) {
    if (!hasTile) {
      DS += " if x==1 or ";
    } else {
      DS += " and (x==1 or ";
    }
    bool first = true;
    for (auto label : shadows) {
      if (!first) {
        DS += " and ";
      }
      first = false;
      DS += label + "!=\"flatten\"";
    }
    if (hasTile) {
      DS += ")";
    }
  }

  // Condition 3: valid only when the activate tiling pragma is at the
  // higher loop level (TODO)

  // Set up the default value and close
  DS += "]; default:1";

  string newPragma = "ACCEL PARALLEL FACTOR=auto{options:" + DS + "}";
  void *newPragmaStmt =
      m_ast->CreatePragma(newPragma, m_ast->TraceUpToScope(loop));

  // It seems like we cannot insert a new pragma and replace it in
  // the same pass, so make sure replacing the pragmas only from
  // previous passes.
  cout << "inserted:" << endl;
  cout << "  " << newPragma << endl;
  if (!hasTile) {
    m_ast->PrependChild(m_ast->GetLoopBody(loop), newPragmaStmt);
  } else {
    m_ast->InsertAfterStmt(newPragmaStmt, stmt);
  }
}

void insertParallelComplete(CSageCodeGen *m_ast, void *loop) {
  cout << "try to fully unroll " << m_ast->_p(loop) << "...";

  void *stmt = m_ast->GetChildStmt(m_ast->GetLoopBody(loop), 0);
  while ((stmt != nullptr) && (m_ast->IsPragmaDecl(stmt) != 0)) {
    string ori_str = m_ast->GetPragmaString(stmt);
    string str = ori_str;
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    if (str.find("parallel") != string::npos) {
      if (str.find("auto") != string::npos) {
        // Take user specified parallel pragma with auto
        cout << "skip due to user-specified auto pragma:" << endl;
        cout << "  " << str << endl;
        return;
      }
      // Skip the loop that already has a parallel pragma
      cout << "skip due to existing parallel pragma:" << endl;
      cout << "  " << str << endl;
      return;
    }
    stmt = m_ast->GetNextStmt(stmt);
  }

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

  cout << "tripcount " << tc << ", ";

  if (tc > 9) {
    cout << "skip due to large tc" << endl;
    return;
  }

  void *newPragmaStmt =
      m_ast->CreatePragma("ACCEL PARALLEL", m_ast->TraceUpToScope(loop));

  // It seems like we cannot insert a new pragma and replace it in
  // the same pass, so make sure replacing the pragmas only from
  // previous passes.
  cout << "inserted:" << endl;
  cout << "  ACCEL PARALLEL" << endl;
  m_ast->PrependChild(m_ast->GetLoopBody(loop), newPragmaStmt);
}

int ds_loop_parallel_top(CSageCodeGen *m_ast, void *pTopFunc,
                         const CInputOptions &options) {
  cout << "=====================================================" << endl;
  cout << "--=# Design Space Builder for Loop Parallel Start#=--\n";
  cout << "=====================================================" << endl;

  CMarsIr mMars_ir;
  mMars_ir.get_mars_ir(m_ast, pTopFunc, options, true);

  vector<CMirNode *> res;
  mMars_ir.get_topological_order_nodes(&res);

  vector<void *> vecCGLoops;

  for (auto node : res) {
    if (node->is_function) {
      continue;
    }
    auto *loop = m_ast->GetParent(node->ref);

    bool isFineGrained =
        (isFineGrainedLoop(m_ast, loop) && !hasNonFlattenFuncCall(m_ast, loop));

    if (isFineGrained) {  // Try to flat innermost loops
      insertParallelComplete(m_ast, loop);
    }
  }

  // Insert design space
  int loop_idx = 0;
  for (auto node : res) {
    if (node->is_function) {
      continue;
    }
    auto *loop = m_ast->GetParent(node->ref);

    if (!IsOutermostForLoop(m_ast, loop)) {
      continue;
    }
    insertLoopDesignSpace(m_ast, &inserter, loop,
                          "L" + std::to_string(loop_idx++));
  }

  cout << "=====================================================" << endl;
  cout << "--=# Design Space Builder for Loop Parallel End#=--\n";
  cout << "=====================================================" << endl;

  return 1;
}
