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
#include <unordered_set>

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

#define DS_SKELETON_EXTRACT_DEBUG 0
#if DS_SKELETON_EXTRACT_DEBUG
#define DEBUG_PRINT(x)                                                         \
  do {                                                                         \
    std::cerr << x << std::endl;                                               \
  } while (0)
#else
#define DEBUG_PRINT(x)
#endif

// Skeleton types
// Component label -> (callee label -> (trip-count, unroll factor))
typedef pair<int, int> SkeletonCalleeType;
typedef unordered_map<string, SkeletonCalleeType> SkeletonCallerType;
typedef unordered_map<string, SkeletonCallerType> SkeletonType;

// Topo ID type
typedef unordered_map<string, string> TopoIDType;

int explore_stmts(CSageCodeGen *m_ast, void *stmt_block, SkeletonType *skeleton,
                  TopoIDType *topo_id_map, string component_name,
                  int component_idx) {
  // Generate topo ID
  if (skeleton->count(component_name) == 0) {
    (*skeleton)[component_name] = SkeletonCallerType();
    void *parent = m_ast->GetParent(stmt_block);

    // We traverse basic blocks so we only find Function definitions,
    // but the topo ID is based on function declaration
    if (m_ast->IsFunctionDefinition(parent) != 0) {
      parent = m_ast->GetFuncDeclByDefinition(parent);
    }

    if ((m_ast->IsForStatement(parent) != 0) ||
        (m_ast->IsFunctionDeclaration(parent) != 0)) {
      string id = getTopoLocation(m_ast, static_cast<SgNode *>(parent));
      DEBUG_PRINT("topo_id: " + id);
      DEBUG_PRINT("node src: " + m_ast->_p(parent));
      if (!id.empty()) {
        (*topo_id_map)[component_name] = id;
      } else {
        cout << "topo ID not found for node " << m_ast->_p(parent) << endl;
      }
    } else {
      cout << "unknown node: " << m_ast->_p(parent) << endl;
    }
  }
  if (stmt_block == NULL || m_ast->GetChildStmtNum(stmt_block) <= 0) {
    return component_idx;
  }

  for (size_t i = 0; i < m_ast->GetChildStmtNum(stmt_block); i++) {
    void *stmt = m_ast->GetChildStmt(stmt_block, i);
    if (m_ast->IsLabelStatement(stmt) != 0) {
      stmt = m_ast->GetStmtFromLabel(stmt);
    }

    if (m_ast->IsFunctionCall(stmt) != 0) {
    } else if (m_ast->IsForStatement(stmt) != 0) {
      string loop_name =
          component_name + "@L@" + std::to_string(component_idx++);
      int64_t dummy = 1;
      int64_t trip_count = 1;
      if ((m_ast->get_loop_trip_count(stmt, &dummy, &trip_count) == 0) ||
          trip_count < 1) {
        trip_count = 1;
      }
      int unroll_factor = 1;
      // MER-1048: enabling pragma declared before loop statement &
      // record unroll factors in the skeleton
      void *pre_stmt = m_ast->GetPreviousStmt(stmt);
      while (m_ast->IsPragmaDecl(pre_stmt) != 0) {
        string pragma_str = m_ast->GetPragmaString(pre_stmt);
        // transform the pragma string to its lower-case equivalent
        transform(pragma_str.begin(), pragma_str.end(), pragma_str.begin(),
                  ::tolower);
        int found = pragma_str.find("unroll");

        if (found != string::npos) {
          int found_factor = pragma_str.find(" ");
          if (found_factor != string::npos) {
            string token =
                pragma_str.substr(pragma_str.find(" "), pragma_str.size() - 1);
            unroll_factor = my_atoi(token);
          } else {
            unroll_factor = trip_count;
          }
          break;
        }
        pre_stmt = m_ast->GetPreviousStmt(pre_stmt);
      }

      void *loop_body = m_ast->GetLoopBody(stmt);
      for (size_t i = 0; i < m_ast->GetChildStmtNum(loop_body); i++) {
        void *body_stmt = m_ast->GetChildStmt(loop_body, i);
        if (m_ast->IsPragmaDecl(body_stmt) != 0) {
          string pragma_str = m_ast->GetPragmaString(body_stmt);
          // transform the pragma string to its lower-case equivalent
          transform(pragma_str.begin(), pragma_str.end(), pragma_str.begin(),
                    ::tolower);
          if (pragma_str.find(" unroll") != string::npos) {
            unroll_factor = trip_count;
            break;
          }
        }
      }
      (*skeleton)[component_name][loop_name] = {unroll_factor, trip_count};
      DEBUG_PRINT("exploring loop body of " + m_ast->_p(stmt));
      explore_stmts(m_ast, m_ast->GetLoopBody(stmt), skeleton, topo_id_map,
                    loop_name, 0);
    } else if (m_ast->IsLoopStatement(stmt) != 0) {
      component_idx = explore_stmts(m_ast, m_ast->GetLoopBody(stmt), skeleton,
                                    topo_id_map, component_name, component_idx);
    } else if (m_ast->IsBasicBlock(stmt) != 0) {
      component_idx = explore_stmts(m_ast, stmt, skeleton, topo_id_map,
                                    component_name, component_idx);
    } else if (m_ast->IsIfStatement(stmt) != 0) {
      component_idx =
          explore_stmts(m_ast, m_ast->GetIfStmtTrueBody(stmt), skeleton,
                        topo_id_map, component_name, component_idx);
      component_idx =
          explore_stmts(m_ast, m_ast->GetIfStmtFalseBody(stmt), skeleton,
                        topo_id_map, component_name, component_idx);
    } else {
      vector<void *> func_calls;
      m_ast->GetNodesByType(stmt, "preorder", "SgFunctionCallExp", &func_calls);
      for (auto func_call : func_calls) {
        string callee_name = m_ast->GetFuncNameByCall(func_call) + "@F@" +
                             std::to_string(component_idx++);
        (*skeleton)[component_name][callee_name] = {1, 1};
      }
    }
  }
  return component_idx;
}

void extract_skeleton(CSageCodeGen *m_ast, void *pTopFunc,
                      SkeletonType *skeleton, TopoIDType *topo_id_map) {
  vector<void *> func_decls;
  m_ast->GetNodesByType(pTopFunc, "preorder", "SgFunctionDeclaration",
                        &func_decls);
  for (auto func_decl : func_decls) {
    void *func_body = m_ast->GetFuncBody(func_decl);
    // omit empty functions since they do not affect resource usage
    if (func_body == NULL || m_ast->GetChildStmtNum(func_body) == 0) {
      continue;
    }
    if (m_ast->IsSystemFunction(func_decl) != 0) {
      continue;
    }
    string func_name = m_ast->GetFuncName(func_decl);
    if (skeleton->count(func_name) == 0) {
      explore_stmts(m_ast, func_body, skeleton, topo_id_map, func_name, 0);
    } else {
      cerr << "[Warning] multiple definitions of Function " << func_name
           << " are found." << endl;
    }
  }
}

rapidjson::Value gen_skeleton_obj(rapidjson::Document::AllocatorType *alloc,
                                  const SkeletonType &skeleton) {
  rapidjson::Value skeleton_val(rapidjson::kObjectType);
  for (auto caller : skeleton) {
    rapidjson::Value caller_item(rapidjson::kObjectType);
    for (auto callee : caller.second) {
      rapidjson::Value callee_item(rapidjson::kObjectType);
      auto callee_info = callee.second;
      callee_item.AddMember("unroll-factor",
                            rapidjson::Value().SetInt(callee_info.first),
                            *alloc);
      callee_item.AddMember(
          "trip-count", rapidjson::Value().SetInt(callee_info.second), *alloc);

      rapidjson::Value key(callee.first.c_str(), *alloc);
      caller_item.AddMember(key, callee_item, *alloc);
    }
    rapidjson::Value key(caller.first.c_str(), *alloc);
    skeleton_val.AddMember(key, caller_item, *alloc);
  }
  return skeleton_val;
}

rapidjson::Value gen_id_map_obj(rapidjson::Document::AllocatorType *alloc,
                                const TopoIDType &topo_id_map) {
  rapidjson::Value obj(rapidjson::kObjectType);
  for (auto pair : topo_id_map) {
    rapidjson::Value key(pair.first.c_str(), *alloc);
    rapidjson::Value val(pair.second.c_str(), *alloc);
    obj.AddMember(key, val, *alloc);
  }
  return obj;
}

int ds_skeleton_extract_top(CSageCodeGen *m_ast, void *pTopFunc,
                            const CInputOptions &options) {
  cout << "==============================================" << endl;
  cout << "--------=# Skeleton Extraction Start#=--------\n";
  cout << "==============================================" << endl;

  SkeletonType skeleton;
  TopoIDType topo_id_map;
  extract_skeleton(m_ast, pTopFunc, &skeleton, &topo_id_map);

  cout << "[Info] skeleton extracted." << endl;

  string output_name = "skeleton.json";
  rapidjson::Document doc(rapidjson::kObjectType);
  auto &alloc = doc.GetAllocator();

  doc.AddMember("skeleton", gen_skeleton_obj(&alloc, skeleton), alloc);
  doc.AddMember("topo_id_map", gen_id_map_obj(&alloc, topo_id_map), alloc);

  std::ofstream output_file(output_name);
  rapidjson::OStreamWrapper osw(output_file);
  rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
  doc.Accept(writer);

  cout << "[Info] skeleton written into skeleton.json." << endl;

  cout << "============================================" << endl;
  cout << "--------=# Skeleton Extraction End#=--------\n";
  cout << "============================================" << endl;

  return 1;
}
