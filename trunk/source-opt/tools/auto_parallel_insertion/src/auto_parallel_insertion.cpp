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


#include "auto_parallel_insertion.h"

#include <algorithm>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <iostream>
#include <exception>
#include <stack>

#include "interface_transform.h"
#include "mars_message.h"

#define PENALTY_FLT_ADD_MUL 8
#define PENALTY_FLT_GENERAL 128
#define PENALTY_HARD_FN 256
#define HARDWARE_LIMIT 8192
#define LATENCY_LIMIT 8192
#define ARRAY_PARALLEL_LIMIT 512
#define TRIPCOUNT_LIMIT 64
#define DEFAULT_TRIP_COUNT 2
#define CRITICAL_LOOP_GAP 10
#define _DEBUG_AUTOPARALLEL
using MarsProgramAnalysis::CMarsExpression;
using MarsProgramAnalysis::CMarsRangeExpr;
using MarsProgramAnalysis::CMarsVariable;
using std::stack;
using std::unordered_map;
using std::unordered_set;

extern int loop_tiling_top(CSageCodeGen *codegen, void *pTopFunc,
                           const CInputOptions &options, bool pragma_in_loop);

void LoopInfoBuilder::FilterNonTopLoop(vector<void *> *p_loops,
                                       void *top_loop) {
  int i = 0;
  while (i < p_loops->size()) {
    if (codegen->TraceUpToLoopScope(codegen->GetParent((*p_loops)[i])) !=
        top_loop) {
      p_loops->erase(p_loops->begin() + i);
    } else {
      i++;
    }
  }
}

/// This function collects all the loop frontier that fn directly connects.
/// It recursively traverse all the functions that not called
/// in the loop to collect all candidate loops in the call graph.
/// @param fn : function declarations
/// @output a vector that stores all top loops.
unordered_set<void *> LoopInfoBuilder::GetLoopFrontier(void *fn) {
  unordered_set<void *> ret;
  if (fn == nullptr)
    return ret;
  void *rec_func = nullptr, *fn_body = codegen->GetFuncBody(fn);
  if (codegen->IsRecursiveFunction(fn, &rec_func))
    return ret;
  // append loops in current fn since they are top loops.
  vector<void *> cur_lv_loops;
  codegen->GetNodesByType(fn_body, "preorder", "SgForStatement", &cur_lv_loops);
  void *filter = nullptr;
  FilterNonTopLoop(&cur_lv_loops, filter);
  ret.insert(cur_lv_loops.begin(), cur_lv_loops.end());
  cur_lv_loops.clear();
  codegen->GetNodesByType(fn_body, "preorder", "SgWhileStmt", &cur_lv_loops);
  filter = nullptr;
  FilterNonTopLoop(&cur_lv_loops, filter);
  ret.insert(cur_lv_loops.begin(), cur_lv_loops.end());
  cur_lv_loops.clear();
  codegen->GetNodesByType(fn_body, "preorder", "SgDoWhileStmt", &cur_lv_loops);
  filter = nullptr;
  FilterNonTopLoop(&cur_lv_loops, filter);
  ret.insert(cur_lv_loops.begin(), cur_lv_loops.end());

  unordered_set<void *> in_loop_calls;
  for (auto &loop : ret) {
    vector<void *> vec_loop_calls;
    codegen->GetNodesByType(loop, "preorder", "SgFunctionCallExp",
                            &vec_loop_calls);
    copy(vec_loop_calls.begin(), vec_loop_calls.end(),
         inserter(in_loop_calls, in_loop_calls.end()));
  }
  // traverse fns that not within the current level loops.
  vector<void *> sub_fn_calls;
  codegen->GetNodesByType(fn, "preorder", "SgFunctionCallExp", &sub_fn_calls);
  for (auto &call : sub_fn_calls) {
    if (in_loop_calls.count(call)) {
      continue;
    }
    void *sub_fn = codegen->GetFuncDeclByCall(call);
    unordered_set<void *> sub_loop_frontier = GetLoopFrontier(sub_fn);
    ret.insert(sub_loop_frontier.begin(), sub_loop_frontier.end());
  }
  return ret;
}

void LoopInfoBuilder::TraverseLoops(void *loop) {
  if (loop == nullptr)
    return;
  void *sg_body = codegen->GetLoopBody(loop);

  vector<void *> sub_loops, sub_for_loops, sub_while_loops, sub_dowhile_loops;
  codegen->GetNodesByType(sg_body, "preorder", "SgForStatement",
                          &sub_for_loops);
  FilterNonTopLoop(&sub_for_loops, loop);
  sub_loops.insert(sub_loops.end(), sub_for_loops.begin(), sub_for_loops.end());

  codegen->GetNodesByType(sg_body, "preorder", "SgWhileStmt", &sub_while_loops);
  FilterNonTopLoop(&sub_while_loops, loop);
  sub_loops.insert(sub_loops.end(), sub_while_loops.begin(),
                   sub_while_loops.end());

  codegen->GetNodesByType(sg_body, "preorder", "SgDoWhileStmt",
                          &sub_dowhile_loops);
  FilterNonTopLoop(&sub_dowhile_loops, loop);
  sub_loops.insert(sub_loops.end(), sub_dowhile_loops.begin(),
                   sub_dowhile_loops.end());

  vector<void *> sub_fn_calls;
  codegen->GetNodesByType(loop, "preorder", "SgFunctionCallExp", &sub_fn_calls);
  FilterNonTopLoop(&sub_fn_calls, loop);
  for (auto &call : sub_fn_calls) {
    void *sub_fn = codegen->GetFuncDeclByCall(call);
    unordered_set<void *> called_loops = GetLoopFrontier(sub_fn);
    sub_loops.insert(sub_loops.end(), called_loops.begin(), called_loops.end());
  }

  if (sub_loops.empty()) {
    terminal_loops.insert(loop);
  }

  loop_graph[loop] = {};
  for (auto &sub_loop : sub_loops) {
    loop_graph[loop].insert(sub_loop);
#ifdef _DEBUG_AUTOPARALLEL
    cout << "Traverse Loop: "
         << codegen->_up(loop) + getUserCodeFileLocation(codegen, loop, true)
         << endl;
    cout << "  ==> insert sub Loop: "
         << codegen->_up(sub_loop) +
                getUserCodeFileLocation(codegen, sub_loop, true)
         << endl;
#endif
    if (!loop_graph.count(sub_loop)) {
      TraverseLoops(sub_loop);
    }
  }
}

void *LoopInfoBuilder::LoopHasPragma(void *loop, PragmaEnum pragma_ty, bool hls,
                                     string add) {
  vector<void *> pragmas;
  //  int is_compatible = 1;
  codegen->GetNodesByType(loop, "preorder", "SgPragmaDeclaration", &pragmas);
  //                        is_compatible != 0);
  for (auto &pragma : pragmas) {
    if (codegen->TraceUpToLoopScope(pragma) != loop) {
      continue;
    }
    string key;
    switch (pragma_ty) {
    case PARALLEL: {
      key = hls ? "unroll" : "parallel";
      break;
    }
    case PIPELINE: {
      key = "pipeline";
      break;
    }
    case REDUCTION: {
      key = "reduction";
      break;
    }
    case TILING: {
      key = "tile";
      break;
    }
    case LINEBUFFER: {
      key = "line_buffer";
      break;
    }
    case TILED_TAG: {
      key = "tiled_tag";
      break;
    }
    case PARTIAL_PARALLEL_TAG: {
      key = "partial_parallel_tag";
      break;
    }

    default: { break; }
    }
    string pragma_str = codegen->GetPragmaString(pragma);
    string s_filter, s_cmd;
    map<string, pair<vector<string>, vector<string>>> map_param;
    tldm_pragma_parse_whole(pragma_str, &s_filter, &s_cmd, &map_param, true);
    transform(s_filter.begin(), s_filter.end(), s_filter.begin(), ::tolower);
    transform(s_cmd.begin(), s_cmd.end(), s_cmd.begin(), ::tolower);
    if (s_cmd.find(key) != 0)
      continue;
    if (!add.empty() && !map_param.count(add))
      continue;
    if (hls && s_filter == "hls") {
      return pragma;
    }
    if (!hls && s_filter == "accel") {
      return pragma;
    }
  }
  return nullptr;
}

// For now only consider fully unroll
int64_t LoopInfoBuilder::GetPragmaFactor(void *pragma) {
  int64_t ret = 0;
  string pragma_str = codegen->GetPragmaString(pragma);
  string s_filter, s_cmd;
  map<string, pair<vector<string>, vector<string>>> map_param;
  tldm_pragma_parse_whole(pragma_str, &s_filter, &s_cmd, &map_param, true);
  for (auto &kvp : map_param) {
    string key = kvp.first;
    if (key == CMOST_parallel_factor && map_param[key].first.size() > 0) {
      ret = static_cast<int64_t>(stoll(map_param[key].first[0]));
      return ret;
    }
  }
  return 0;
}

int64_t LoopInfoBuilder::AnalyzeLoopTripCount(void *loop) {
  if (loop == nullptr)
    return -1;

  int64_t trip_count = -1, trip_count_ub = -1;
  int64_t trip_count_final = 0;
  int ret = codegen->get_loop_trip_count(loop, &trip_count, &trip_count_ub);
  if (ret == 0) {
    loop_info_map[loop]->estimation_bound = true;
  }

  if (trip_count != -1) {
    trip_count_final = trip_count;
    loop_info_map[loop]->orig_tc = trip_count;
    loop_info_map[loop]->no_bound = false;

    void *func = codegen->GetEnclosingNode("Function", loop);
    CMarsRangeExpr mr = CMarsVariable(loop, codegen, func).get_range();
    CMarsExpression me_lb;
    CMarsExpression me_ub;
    mr.get_simple_bound(&me_lb, &me_ub);
    if (me_lb.IsConstant() != 0) {
      loop_info_map[loop]->range_lb = me_lb.GetConstant();
    }
  } else {
    loop_info_map[loop]->estimation_bound = true;
    if (trip_count_ub == -1) {
      string warning_info = "' " + codegen->_up(loop, 20) + " ' " +
                            getUserCodeFileLocation(codegen, loop, true);
      dump_critical_message(
          PROCS_WARNING_45(warning_info, my_itoa(DEFAULT_TRIP_COUNT)));
      trip_count_final = DEFAULT_TRIP_COUNT;
      loop_info_map[loop]->orig_tc = DEFAULT_TRIP_COUNT;
    } else {
      trip_count_final = trip_count_ub;
      loop_info_map[loop]->orig_tc = trip_count_ub;
    }
  }

  int64_t unroll_factor = -1;
  void *hls_unroll_pragma = LoopHasPragma(loop, PARALLEL, true, "");
  void *accel_unroll_pragma = LoopHasPragma(loop, PARALLEL, false, "");
  CAnalPragma ana_pragma(codegen);
  if (hls_unroll_pragma) {
    bool errorOut = false;
    ana_pragma.PragmasFrontendProcessing(hls_unroll_pragma, &errorOut, false);
    string str_unroll_factor = ana_pragma.get_attribute(CMOST_parallel_factor);
    try {
      unroll_factor = stoll(str_unroll_factor, nullptr, 0);
    } catch (std::exception &e) {
      return 1;
    }
  }
  if (accel_unroll_pragma) {
    bool errorOut = false;
    ana_pragma.PragmasFrontendProcessing(accel_unroll_pragma, &errorOut, false);
    string str_unroll_factor = ana_pragma.get_attribute(CMOST_parallel_factor);
    try {
      unroll_factor = stoll(str_unroll_factor, nullptr, 0);
    } catch (std::exception &e) {
      return 1;
    }
  }

  if (unroll_factor > 0) {
    // A unroll factor creates a new pipelined loop with tripcount of
    // unroll_factor. Thus should label current loop has pipelined.
    loop_info_map[loop]->unroll_factor = unroll_factor;
    if (loop_info_map[loop]->estimation_bound) {
      loop_info_map[loop]->cur_partial_unroll = true;
      loop_info_map[loop]->cur_parallel_pg = false;
      return trip_count_final;
    } else {
      if (unroll_factor < trip_count_final) {
        loop_info_map[loop]->cur_partial_unroll = true;
        loop_info_map[loop]->cur_parallel_pg = false;
        return trip_count_final / unroll_factor;
      }
    }
  }
  return trip_count_final;
}

void LoopInfoBuilder::AnalyzeAccess(void *loop) {
  void *loop_body = codegen->GetLoopBody(loop);
  CMirNode *node = mars_ir->get_node(loop_body);
  if (node == nullptr)
    return;

  node->set_full_access_table_v2(codegen);
  for (auto pList : node->full_access_table_v2) {
    void *arr_init = pList.first;
    int org_dim = codegen->get_dim_num_from_var(arr_init);

    // Collect the reference in the loop which is related to the
    // loop iterator
    vector<pair<void *, unordered_map<int, int64_t>>> access_tags;
    set<void *> parent_vars;
    for (auto ref : pList.second) {
      void *sg_array;
      void *sg_pntr;
      vector<void *> sg_indexes;
      int pointer_dim;
      codegen->parse_pntr_ref_by_array_ref(ref, &sg_array, &sg_pntr,
                                           &sg_indexes, &pointer_dim, nullptr,
                                           true, -1, 1);

      // TODO(Yuxin): Cross function and partial access
      // Current loop iterator will be passed by a subfunction argument
      if (sg_array != arr_init && org_dim != sg_indexes.size())
        continue;

      // Record all the related coefficients
      // They will be used to calculate the parallel range
      unordered_map<int, int64_t> local_dim_tags;
      for (size_t j = 0; j < sg_indexes.size(); ++j) {
        CMarsExpression index1(sg_indexes[j], codegen, sg_pntr);
        map<void *, CMarsExpression> *m_coeff = index1.get_coeff();
        if (m_coeff->count(loop) > 0) {
          auto coeff_const = (*m_coeff)[loop];
          local_dim_tags[j] = coeff_const.GetConstant();
        }
      }
      access_tags.push_back(std::make_pair(ref, local_dim_tags));
      if (codegen->IsMemberVariable(arr_init)) {
        void *sg_parent = codegen->GetParent(ref);
        if ((codegen->IsDotExp(sg_parent) != 0) ||
            (codegen->IsArrowExp(sg_parent) != 0)) {
          void *left_exp = codegen->GetExpLeftOperand(sg_parent);
          void *sg_var = codegen->GetVariableInitializedName(left_exp);
          if (sg_var != nullptr && parent_vars.count(sg_var) == 0) {
            parent_vars.insert(sg_var);
          }
        }
      }
    }

    // Propagate to the original variable declaration
    if (codegen->IsArgumentInitName(arr_init) != 0) {
      void *func_decl = codegen->GetEnclosingNode("Function", loop_body);
      map<void *, bool> res;
      mars_ir->trace_to_bus(codegen, func_decl, arr_init, &res, loop_body);
      for (auto r : res) {
        int res_dim = codegen->get_dim_num_from_var(r.first);
        if (res_dim == org_dim) {
          loop_info_map[loop]->access_info[r.first] = access_tags;
        }
      }
    } else if (codegen->IsMemberVariable(arr_init)) {
      for (auto v : parent_vars) {
        loop_info_map[loop]->access_info[v] = access_tags;
      }
    } else {
      loop_info_map[loop]->access_info[arr_init] = access_tags;
    }
  }
}

AccessMap LoopInfoBuilder::GetAccessInfo(void *loop) {
  return loop_info_map[loop]->access_info;
}

void LoopInfoBuilder::TopoSortLoops(void *loop,
                                    unordered_map<void *, bool> *p_visited,
                                    vector<void *> *p_topo_vec) {
  if ((*p_visited)[loop] == true)
    return;
  (*p_visited)[loop] = true;
  loop_info_map[loop] = new LoopInfo(loop);

  // update current loop intomation, specifically
  // tc, has user defined pragmas
  if (LoopHasPragma(loop, PIPELINE, false, "")) {
    loop_info_map[loop]->cur_pipeline_pg = true;
  }
  if (LoopHasPragma(loop, PARALLEL, false, "")) {
    loop_info_map[loop]->cur_parallel_pg = true;
  }
  if (LoopHasPragma(loop, PIPELINE, true, "") ||
      LoopHasPragma(loop, PIPELINE, false, "flatten")) {
    loop_info_map[loop]->hier_pipeline_pg = true;
    loop_info_map[loop]->cur_pipeline_pg = true;
  }
  if (LoopHasPragma(loop, PARALLEL, true, "")) {
    loop_info_map[loop]->hier_parallel_pg = true;
    loop_info_map[loop]->cur_parallel_pg = true;
  }
  if (LoopHasPragma(loop, REDUCTION, false, "") ||
      LoopHasPragma(loop, PARALLEL, false, "reduction")) {
    loop_info_map[loop]->cur_reduction = true;
  }
  if (LoopHasPragma(loop, TILING, false, "")) {
    loop_info_map[loop]->cur_tiling = true;
  }
  if (LoopHasPragma(loop, LINEBUFFER, false, "")) {
    loop_info_map[loop]->cur_linebuf = true;
  }
  if (LoopHasPragma(loop, TILED_TAG, false, "")) {
    loop_info_map[loop]->tiled_sub_loop = true;
  }
  void *tag_pragma = LoopHasPragma(loop, PARTIAL_PARALLEL_TAG, false, "");
  if (tag_pragma) {
    loop_info_map[loop]->partial_sub_loop = true;
    to_remove_pragmas.push_back(tag_pragma);
  }

  int64_t trip_count_final = AnalyzeLoopTripCount(loop);
  AnalyzeAccess(loop);
  loop_info_map[loop]->tc = trip_count_final;
  if (loop_info_map[loop]->orig_tc < 0)
    loop_info_map[loop]->invalid_bound = true;

  cout << "ANALYZE Loop: "
       << codegen->_up(loop) + getUserCodeFileLocation(codegen, loop, true)
       << endl;
  cout << "tc: " << trip_count_final << endl;

  // Set default number as the loop body statments number
  SetLoopBodyLatency(loop, codegen->GetChildStmtNum(loop));

  for (auto &neighbour : loop_graph[loop]) {
    loop_info_map[loop]->child_loop_count++;
    TopoSortLoops(neighbour, p_visited, p_topo_vec);
  }
  loop_info_map[loop]->orig_child_count = loop_info_map[loop]->child_loop_count;

  p_topo_vec->insert(p_topo_vec->begin(), loop);
}

void LoopInfoBuilder::TagLoopAttributes(const vector<void *> &topo_vec) {
  for (auto &loop : topo_vec) {
    vector<void *> parents;
    for (auto &kvp : loop_graph) {
      if (kvp.first != loop && kvp.second.count(loop)) {
        parents.push_back(kvp.first);
      }
    }

    int64_t max_par_tc = 0;
    int64_t max_par_factor = 1;
    for (auto &parent : parents) {
      //      if (loop_info_map[parent]->no_bound) {
      //        loop_info_map[loop]->no_bound = true;
      //      }
      if (loop_info_map[parent]->hier_pipeline_pg) {
        loop_info_map[loop]->hier_pipeline_pg = true;
      }
      //      if (loop_info_map[parent]->estimation_bound)
      //        loop_info_map[loop]->estimation_bound = true;
      loop_info_map[loop]->parent_loop = parent;
      int64_t cur_par_tc =
          (loop_info_map[parent]->parent_loop == nullptr
               ? loop_info_map[parent]->tc
               : loop_info_map[parent]->par_tc * loop_info_map[parent]->tc);
      int64_t parent_unroll_factor = loop_info_map[parent]->unroll_factor;
      if (loop_info_map[parent]->cur_parallel_pg) {
        parent_unroll_factor = loop_info_map[parent]->orig_tc;
      }
      int64_t cur_par_factor = (loop_info_map[parent]->parent_loop == nullptr
                                    ? parent_unroll_factor
                                    : loop_info_map[parent]->par_unroll_factor *
                                          parent_unroll_factor);
      if (cur_par_tc > max_par_tc) {
        max_par_tc = cur_par_tc;
      }
      if (cur_par_tc >= loop_info_map[loop]->par_tc) {
        loop_info_map[loop]->par_tc = cur_par_tc;
      }
      if (cur_par_factor > max_par_factor) {
        max_par_factor = cur_par_factor;
      }
      if (cur_par_factor > loop_info_map[loop]->par_unroll_factor) {
        loop_info_map[loop]->par_unroll_factor = cur_par_factor;
      }
    }
    if (max_par_tc == 0)
      loop_info_map[loop]->par_tc = 0;
  }
}

void LoopInfoBuilder::BuildLoopInfo() {
  // consider subloops in this function, and sub loops in a function
  // this loop calls.
  unordered_set<void *> all_loops;
  unordered_map<void *, bool> visited;
  for (auto &kvp : loop_graph) {
    all_loops.insert(kvp.first);
    visited[kvp.first] = false;
    for (auto &neighbour : kvp.second) {
      all_loops.insert(neighbour);
      visited[neighbour] = false;
    }
  }

  vector<void *> topo_vec;
  for (auto &loop : all_loops) {
    if (visited[loop] == false) {
      TopoSortLoops(loop, &visited, &topo_vec);
    }
  }
  TagLoopAttributes(topo_vec);
}

void LoopInfoBuilder::RemoveTagPragmas() {
  for (size_t j = 0; j < to_remove_pragmas.size(); ++j) {
    if (to_remove_pragmas[j]) {
      codegen->RemoveStmt(to_remove_pragmas[j]);
    }
  }
}

void LoopInfoBuilder::ScanLoop() {
  top_loops.clear();
  terminal_loops.clear();
  loop_info_map.clear();
  loop_graph.clear();
  top_loops = GetLoopFrontier(kernel);
  for (auto &top_loop : top_loops) {
    TraverseLoops(top_loop);
  }
  BuildLoopInfo();
}

int64_t LoopInfoBuilder::GetTripCount(void *loop) {
  if (loop == nullptr || !loop_info_map.count(loop))
    return -1;
  return loop_info_map[loop]->tc;
}

int64_t LoopInfoBuilder::GetOrigTripCount(void *loop) {
  if (loop == nullptr || !loop_info_map.count(loop))
    return -1;
  return loop_info_map[loop]->orig_tc;
}

int64_t LoopInfoBuilder::GetSubLoopOps(void *loop) {
  if (loop == nullptr || !loop_info_map.count(loop))
    return -1;
  return loop_info_map[loop]->child_ops;
}

unordered_set<void *> LoopInfoBuilder::GetTerminalLoops() {
  return terminal_loops;
}

int LoopInfoBuilder::run() {
  ScanLoop();
  return 0;
}

bool LatencyEstimator::CanPipeline(void *loop) { return false; }

bool LatencyEstimator::CanParallel(void *loop) { return false; }

// Yuxin: 2020 Aug 11, set pipeline II=1 by default
int64_t LatencyEstimator::GetLoopII(void *loop) { return 1; }

int64_t LoopInfoBuilder::GetLoopBodyLatency(void *loop) {
  if (loop == nullptr || !loop_info_map.count(loop))
    return -1;
  return loop_info_map[loop]->body_lat;
}

void LoopInfoBuilder::SetLoopBodyLatency(void *loop, int64_t lat) {
  if (loop == nullptr)
    return;
  loop_info_map[loop]->body_lat = lat;
}

int64_t LatencyEstimator::GetLoopLatency(void *loop, bool is_pipeline) {
  if (loop == nullptr)
    return 0;
  int64_t loop_tc = loop_info_builder->GetOrigTripCount(loop);
  int64_t body_lat = loop_info_builder->GetLoopBodyLatency(loop);
  int64_t ret = body_lat;

  if (is_pipeline) {
    ret += loop_tc * GetLoopII(loop);
  } else if (CanParallel(loop)) {
    ret += 0;
  } else {
    ret *= loop_tc;
  }
  ret = (loop_info_builder->loop_info_map[loop]->parent_loop == nullptr
             ? ret
             : ret * loop_info_builder->loop_info_map[loop]->par_tc);
  return ret;
}

bool ResourceEstimator::IsHeavyMathFunction(void *scope) {
  string name = codegen->GetFuncName(scope);
  return heavy_functions.count(name);
}

int64_t ResourceEstimator::GetHeavyOps(void *scope) {
  if (scope == nullptr)
    return 0;
  if (heavy_op_cache.count(scope))
    return heavy_op_cache[scope];

  int64_t ret = 0;
  vector<void *> stmts = codegen->GetChildStmts(scope);
  if (codegen->IsFunctionDeclaration(scope)) {
    // ret += stmts.size();
    // TODO(Min) : should have more concrete resource model
    if (IsHeavyMathFunction(scope)) {
      ret += PENALTY_HARD_FN;
    }
  }

  for (auto &stmt : stmts) {
    // If we continue to recursion on scope whose body is itself, algorithm
    // will not terminate. The only exception is if stmt/switch stmt.
    // For if statement, we can obtain its branch bodies to continue recursion.
    // Switch statement missing API support, to be implemented.
    if (stmt == scope && !codegen->IsIfStatement(stmt))
      return ret;
    if (codegen->IsExprStatement(stmt) || codegen->IsVariableDecl(stmt)) {
      vector<void *> call_exprs;
      codegen->GetNodesByType(stmt, "preorder", "SgFunctionCallExp",
                              &call_exprs);
      for (auto &expr : call_exprs) {
        ret += GetHeavyOps(codegen->GetFuncDeclByCall(expr, 0));
      }
      vector<void *> binop_exprs;
      codegen->GetNodesByType(stmt, "preorder", "SgBinaryOp", &binop_exprs,
                              true);
      for (auto &expr : binop_exprs) {
        // Not take [] into consideration
        if (codegen->IsPntrArrRefExp(expr) && !codegen->IsVariableDecl(stmt))
          continue;
        // Not take logic comparison into consideration
        if (codegen->IsLessThanOp(expr) || codegen->IsLessOrEqualOp(expr) ||
            codegen->IsGreaterThanOp(expr) ||
            codegen->IsGreaterOrEqualOp(expr) || codegen->IsEqualOp(expr) ||
            codegen->IsNonEqualOp(expr) || codegen->IsAssignOp(expr))
          continue;
        void *exp_type = codegen->GetTypeByExp(expr);
        if (codegen->IsFloatType(exp_type)) {
          if (codegen->IsAddOp(expr) || codegen->IsMultiplyOp(expr) ||
              codegen->IsPlusAssignOp(expr) || codegen->IsMultAssignOp(expr)) {
            ret += PENALTY_FLT_ADD_MUL;
          } else {
            ret += PENALTY_FLT_GENERAL;
          }
        }
      }
      vector<void *> uop_exprs;
      codegen->GetNodesByType(stmt, "preorder", "SgUnaryOp", &uop_exprs, true);
      for (auto &expr : uop_exprs) {
        if (codegen->IsCastExp(expr))
          continue;
        void *exp_type = codegen->GetTypeByExp(expr);
        if (codegen->IsFloatType(exp_type)) {
          ret += PENALTY_FLT_GENERAL;
        }
      }
    } else if (codegen->IsIfStatement(stmt)) {
      ret += max(GetHeavyOps(codegen->GetIfStmtTrueBody(stmt)),
                 GetHeavyOps(codegen->GetIfStmtFalseBody(stmt)));
    } else {
      // TODO(Min) : Switch stmt lack codegen apis
      ret += GetHeavyOps(stmt);
    }
  }
  heavy_op_cache[scope] = ret;
  return ret;
}

int64_t ResourceEstimator::GetResourceUsage(void *loop,
                                            bool accumulate_parent) {
  if (loop == nullptr)
    return 0;
  // The subloops should all be parallelized when counting the resource
  // of the current loop
  int64_t sub_loop_ops = loop_info_builder->GetSubLoopOps(loop);
  int64_t cur_loop_ops = codegen->GetChildStmtNum(loop);
  // Yuxin: Jul 31 2020, only when loop is fully parallelized,
  // the resource is op*loop_tc. Otherwise no need to multiply
  // loop trip count
  cur_loop_ops += GetHeavyOps(loop) + sub_loop_ops;
  if (accumulate_parent) {
    cur_loop_ops =
        (loop_info_builder->loop_info_map[loop]->parent_loop == nullptr)
            ? cur_loop_ops
            : cur_loop_ops *
                  loop_info_builder->loop_info_map[loop]->par_unroll_factor;
  }
  return cur_loop_ops;
}

bool AutoParallel::InsertParallelPragma(void *loop) {
  if (loop_info_builder->loop_info_map[loop]->cur_pipeline_pg ||
      loop_info_builder->loop_info_map[loop]->cur_parallel_pg)
    return false;
  void *new_pragma = codegen->CreatePragma("ACCEL PARALLEL COMPLETE", loop);
  codegen->PrependChild(loop, new_pragma);
  return true;
}

bool AutoParallel::InsertPipelinePragma(void *loop) {
  if (loop_info_builder->loop_info_map[loop]->cur_pipeline_pg ||
      loop_info_builder->loop_info_map[loop]->cur_parallel_pg)
    return false;
  void *new_pragma = codegen->CreatePragma("ACCEL PIPELINE AUTO", loop);
  codegen->PrependChild(loop, new_pragma);
  return true;
}

bool AutoParallel::InsertPragma() {
  bool Changed = false;
  for (auto &kvp : loop_tags) {
    if (kvp.second == PARALLEL) {
      Changed |= InsertParallelPragma(kvp.first);
    }
  }

  if (mXilinx_flow) {
    for (auto &kvp : loop_tags) {
      if (kvp.second == PIPELINE) {
        Changed |= InsertPipelinePragma(kvp.first);
      }
    }
  }
  return Changed;
}

void AutoParallel::ProcessingCriticalLoopsByLatency(int64_t total_lat) {
  cout << "\n\n START Critical loops analysis\n====\n";

  int64_t rest_lat = total_lat;
  int64_t prev_lat = 0;
  while (!parallel_frontiers.empty()) {
    cout << "CHECK reference latency: " << prev_lat << endl;
    cout << "CHECK rest latency: " << rest_lat << endl;

    if (prev_lat < 0 || rest_lat < 0)
      break;
    if (rest_lat * CRITICAL_LOOP_GAP < prev_lat)
      break;

    auto node = parallel_frontiers.top();
    void *loop = node.second;
    auto curr_lat = node.first;
    auto curr_res = res_estimator->GetResourceUsage(loop, true);
    auto curr_tc = loop_info_builder->GetTripCount(loop);
    auto loop_orig_tc = loop_info_builder->GetOrigTripCount(loop);
    parallel_frontiers.pop();

    if (loop_info_builder->loop_info_map[loop]->no_bound ||
        loop_info_builder->loop_info_map[loop]->invalid_bound ||
        loop_info_builder->loop_info_map[loop]->hier_pipeline_pg)
      continue;

#ifdef _DEBUG_AUTOPARALLEL
    cout << "Tagging Pragma For Loop: "
         << codegen->_up(loop) + getUserCodeFileLocation(codegen, loop, true)
         << endl;
    cout << "Loop latency: " << curr_lat << endl;
    cout << "total latency: " << prev_lat << endl;
    cout << "Loop TripCount: " << curr_tc << endl;
    cout << "curr resource: " << curr_res << endl;
#endif

#if 0
    if (loop_info_builder->GetTripCount(loop) > 1) {
      // TODO(youxiang): need to add more check for
      // possible sequential execution,
      // e.g. reduction operation, external memory access (w/wo coalescing)
      if (contains_channel_access(loop)) {
        // youxiang: disabling fine grained parallel may block fine grained
        // pipeline of its parent loop. Although the current loop cannot be
        // paralleled completely and will increase the II of parent loop,
        // it would be possible to get benifit if there is partial parallel
        // in the current loop
        loop_tags[loop] = PIPELINE;
        continue;
      }
    }
#endif
    // TODO(Min): Wrap as classes
    // TODO(Min): Should be a controllable param.
    bool user_input_parallel = false;
    if (loop_info_builder->loop_info_map[loop]->cur_parallel_pg) {
      user_input_parallel = true;
    }

    if (!user_input_parallel) {
      // The outter loop after partial unroll needs pipeline
      // The inner loop after tiling need pipeline
      if (loop_info_builder->loop_info_map[loop]->cur_partial_unroll ||
          loop_info_builder->loop_info_map[loop]->tiled_sub_loop ||
          loop_info_builder->loop_info_map[loop]->partial_parent_loop) {
        loop_tags[loop] = PIPELINE;
      }

      if (loop_info_builder->loop_info_map[loop]->cur_pipeline_pg ||
          loop_info_builder->loop_info_map[loop]->estimation_bound) {
        loop_tags[loop] = PIPELINE;
      }

      if (curr_tc > TRIPCOUNT_LIMIT) {
        loop_tags[loop] = PIPELINE;
      }

      if (curr_res * loop_orig_tc > HARDWARE_LIMIT) {
        loop_tags[loop] = PIPELINE;
      }
    }
    // if (parent_loop &&
    //     lty_estimator->GetLoopLatency(loop) * 10 <
    //     lty_estimator->GetLoopLatency(parent_loop)) {
    //   // if parent_loop is top loop dont do anything
    //   if (!loop_info_builder->top_loops.count(parent_loop)) {
    //     loop_tags[parent_loop] = PIPELINE;
    //   }
    // }
    //  Disable big jump on tripcount
    // if (loop_info_builder->GetTripCount(loop) * 10 <
    //     loop_info_builder->GetTripCount(parent_loop)) {
    //   if (!loop_info_builder->top_loops.count(parent_loop)) {
    //     loop_tags[parent_loop] = PIPELINE;
    //   }
    // }
    // a loop may be labelled at its child, so filter out that part
    if (loop_tags[loop] == PIPELINE) {
      auto new_lat = lty_estimator->GetLoopLatency(loop, true);
      prev_lat += new_lat;
      rest_lat -= curr_lat;
      continue;
    }

    // Yuxin: Sep/9/2020, check and update array parallel info
    // If the parallelism on the array exceed the threshold, stop
    // parallelize the loop, because it might lead to huge partition.
    bool ret = UpdateArrayInfo(loop, loop_orig_tc);
    if (!ret) {
      loop_tags[loop] = PIPELINE;
      auto new_lat = lty_estimator->GetLoopLatency(loop, true);
      prev_lat += new_lat;
      rest_lat -= curr_lat;
      continue;
    } else {
      loop_tags[loop] = PARALLEL;
      rest_lat -= curr_lat;
    }

    for (auto &kvp : loop_info_builder->loop_graph) {
      auto parent_loop = kvp.first;
      if (kvp.second.count(loop)) {
        // update resource
        auto curr_local_res = res_estimator->GetResourceUsage(loop, false);
        loop_info_builder->loop_info_map[parent_loop]->child_ops +=
            curr_local_res * loop_orig_tc;
        // "-1" is remove one loop statement
        auto parent_body_lat =
            loop_info_builder->GetLoopBodyLatency(parent_loop);
        parent_body_lat += loop_info_builder->GetLoopBodyLatency(loop) - 1;
        loop_info_builder->SetLoopBodyLatency(parent_loop, parent_body_lat);

        auto lat = lty_estimator->GetLoopLatency(parent_loop, false);
        loop_info_builder->loop_info_map[parent_loop]->child_loop_count--;
        if (loop_info_builder->loop_info_map[parent_loop]->child_loop_count ==
            0) {
          rest_lat += lat;
          if (loop_info_builder->loop_info_map[loop]->cur_parallel_pg &&
              loop_info_builder->loop_info_map[loop]->partial_sub_loop) {
            loop_info_builder->loop_info_map[parent_loop]->partial_parent_loop =
                true;
          }
          parallel_frontiers.emplace(lat, parent_loop);
        }
      }
    }
  }
  cout << "STOP reference latency: " << prev_lat << endl;
  cout << "STOP rest latency: " << rest_lat << endl;
}

bool AutoParallel::check_range_function(void *loop) {
  auto curr_res = res_estimator->GetResourceUsage(loop, true);
  auto loop_orig_tc = loop_info_builder->GetOrigTripCount(loop);
  if (curr_res * loop_orig_tc > HARDWARE_LIMIT)
    return false;
  void *loop_body = codegen->GetLoopBody(loop);
  vector<void *> funcalexps;
  codegen->GetNodesByType(loop_body, "preorder", "SgFunctionCallExp",
                          &funcalexps);
  for (auto funcalexp : funcalexps) {
    if ("range" == codegen->GetFuncNameByCall(funcalexp) &&
        is_apint_or_apfixed(funcalexp)) {
      //  get the paramlist of range function
      std::vector<void *> paramlist = codegen->GetFuncCallParamList(funcalexp);
      for (auto param : paramlist) {
        CMarsExpression me_param(param, codegen);
        if (me_param.IsConstant()) {
          continue;
        } else if (is_only_relate_to_loop(param, loop)) {
          return true;
        }
      }
    }
  }
  return false;
}

bool AutoParallel::is_only_relate_to_loop(void *range_param, void *loop) {
  CMarsExpression me_param(range_param, codegen);
  auto vars = me_param.get_vars();
  if (0 == vars.size())
    return false;
  for (auto var : vars) {
    if (codegen->IsForStatement(var) && var == loop) {
      continue;
    } else {
      std::vector<void *> var_ref_exps;
      codegen->GetNodesByType(var, "preorder", "SgVarRefExp", &var_ref_exps);
      for (auto vre : var_ref_exps) {
        auto var_init_name = codegen->GetVariableInitializedName(vre);
        auto loopfromvar =
            codegen->GetLoopFromIteratorByPos(var_init_name, range_param);
        if (loopfromvar && loopfromvar == loop) {
          continue;
        } else {
          return false;
        }
      }
    }
  }
  return true;
}

bool AutoParallel::is_apint_or_apfixed(void *funcalexp) {
  auto func_call_exp = isSgFunctionCallExp(static_cast<SgNode *>(funcalexp));
  if (!func_call_exp)
    return false;
  auto call_exp = func_call_exp->get_function();
  auto left_operand = codegen->GetExpLeftOperand(call_exp);
  auto left_operand_type = codegen->GetTypeByExp(left_operand);
  auto base_type = codegen->GetBaseType(left_operand_type, true);
  if (codegen->IsXilinxAPFixedType(base_type) ||
      codegen->IsXilinxAPIntType(base_type))
    return true;
  else
    return false;
}

void AutoParallel::ProcessingTrivialLoopsByPartition() {
  cout << "\n\n START Trivial loops analysis\n====\n";

  while (!rest_frontiers.empty()) {
    auto loop = rest_frontiers.front();
    auto loop_orig_tc = loop_info_builder->GetOrigTripCount(loop);
    auto curr_res = res_estimator->GetResourceUsage(loop, true);
    rest_frontiers.pop();

    if (loop_info_builder->loop_info_map[loop]->hier_pipeline_pg ||
        loop_info_builder->loop_info_map[loop]->cur_pipeline_pg ||
        loop_info_builder->loop_info_map[loop]->cur_tiling ||
        loop_info_builder->loop_info_map[loop]->invalid_bound)
      continue;

#ifdef _DEBUG_AUTOPARALLEL
    cout << "Tagging Pragma For Loop: "
         << codegen->_up(loop) + getUserCodeFileLocation(codegen, loop, true)
         << endl;
    cout << "Loop TripCount: " << loop_orig_tc << endl;
    cout << "Loop res: " << curr_res << endl;
#endif

    if (loop_info_builder->loop_info_map[loop]->cur_parallel_pg) {
      loop_tags[loop] = PARALLEL;
    }

    if (loop_tags[loop] != PARALLEL) {
      if (loop_info_builder->loop_info_map[loop]->cur_partial_unroll ||
          loop_info_builder->loop_info_map[loop]->estimation_bound ||
          loop_info_builder->loop_info_map[loop]->no_bound ||
          loop_info_builder->loop_info_map[loop]->tiled_sub_loop ||
          loop_info_builder->loop_info_map[loop]->partial_parent_loop) {
        loop_tags[loop] = PIPELINE;
        continue;
      }
      if (check_range_function(loop)) {
        loop_tags[loop] = PARALLEL;
      }
      // Still check resource usage in case there are
      // heavy ops
      if (curr_res * loop_orig_tc > HARDWARE_LIMIT) {
        loop_tags[loop] = PIPELINE;
        continue;
      }
      if (loop_orig_tc <= TRIPCOUNT_LIMIT) {
        if (loop_orig_tc == 1) {
          loop_tags[loop] = PARALLEL;
        } else {
          SetLoopByArrayInfo(loop, loop_orig_tc);
        }
      }
    }

    if (loop_tags[loop] == PARALLEL) {
      for (auto &kvp : loop_info_builder->loop_graph) {
        auto parent_loop = kvp.first;
        if (kvp.second.count(loop)) {
          // update resource
          loop_info_builder->loop_info_map[parent_loop]->child_ops +=
              curr_res * loop_orig_tc;
          loop_info_builder->loop_info_map[parent_loop]->child_loop_count--;
          if (loop_info_builder->loop_info_map[parent_loop]->child_loop_count ==
              0) {
            if (loop_info_builder->loop_info_map[loop]->cur_parallel_pg &&
                loop_info_builder->loop_info_map[loop]->partial_sub_loop) {
              loop_info_builder->loop_info_map[parent_loop]
                  ->partial_parent_loop = true;
            }
            rest_frontiers.push(parent_loop);
          }
        }
      }
      continue;
    }
    loop_tags[loop] = PIPELINE;
  }
}

bool AutoParallel::UpdateArrayInfo(void *loop, int64_t loop_tc) {
  int64_t parallel_factor =
      loop_info_builder->loop_info_map[loop]->range_lb + +loop_tc - 1;
  auto access_info = loop_info_builder->GetAccessInfo(loop);
  for (auto info : access_info) {
    auto arr_init = info.first;
    set<int> visited;
    for (auto access_pair : info.second) {
      auto ref = access_pair.first;
      for (auto A : access_pair.second) {
        auto dim_info = A.first;
        auto coeff = A.second;
        // Update the parallel range after a loop is paralellized
        // The range is equal to tripcount * coefficient
        if (access_parallel_tags[arr_init][ref].count(dim_info) == 0) {
          access_parallel_tags[arr_init][ref][dim_info] =
              parallel_factor * coeff;
          if (visited.count(dim_info) == 0) {
            array_parallelism[arr_init][dim_info] = parallel_factor;
            visited.insert(dim_info);
          }
        } else {
          // More levels of parallel apply on one dimension
          auto curr_range = access_parallel_tags[arr_init][ref][dim_info];
          access_parallel_tags[arr_init][ref][dim_info] =
              curr_range + parallel_factor * coeff;
          if (visited.count(dim_info) == 0) {
            auto curr_parallelism = array_parallelism[arr_init][dim_info];
            auto new_parallelism = curr_parallelism + parallel_factor;
            if (coeff > 1)
              new_parallelism = curr_parallelism * parallel_factor;
            if (new_parallelism > ARRAY_PARALLEL_LIMIT)
              return false;
            array_parallelism[arr_init][dim_info] = new_parallelism;
            visited.insert(dim_info);
          }
        }
        // Choose the max parallel range of the array
        // It will be used as a reference range for trivial loop parallel
        auto curr_range = access_parallel_tags[arr_init][ref][dim_info];
        if (array_parallel_range[arr_init].count(dim_info) == 0) {
          array_parallel_range[arr_init][dim_info] = curr_range;
        } else {
          if (array_parallel_range[arr_init][dim_info] < curr_range)
            array_parallel_range[arr_init][dim_info] = curr_range;
        }
      }
    }
  }
  return true;
}

void AutoParallel::SetLoopByArrayInfo(void *loop, int64_t loop_tc) {
  if (loop_tc < 0)
    return;

  int64_t parallel_factor =
      loop_info_builder->loop_info_map[loop]->range_lb + loop_tc - 1;
  auto access_info = loop_info_builder->GetAccessInfo(loop);
  bool parallel_it = true;
  bool find_relate = false;
  for (auto info : access_info) {
    auto arr_init = info.first;
    if (array_parallel_range.count(arr_init) == 0)
      continue;

    for (auto access_pair : info.second) {
      auto ref = access_pair.first;
      for (auto A : access_pair.second) {
        auto dim_info = A.first;
        auto coeff = A.second;
        if (array_parallel_range[arr_init].count(dim_info) == 0)
          continue;
        find_relate = true;

        // Check whether if the parallel range is larger than the reference
        // parallel range, which is from critical loops
        auto ref_factor = array_parallel_range[arr_init][dim_info];
        if (access_parallel_tags[arr_init][ref].count(dim_info) == 0) {
          if (parallel_factor * coeff > ref_factor) {
            parallel_it = false;
            break;
          }
        } else {
          // More levels of parallelsim apply on one dimension
          auto curr_factor = access_parallel_tags[arr_init][ref][dim_info];
          if (curr_factor + parallel_factor * coeff > ref_factor) {
            parallel_it = false;
            break;
          }
        }
      }
      if (!parallel_it)
        return;
    }
  }
  if (!find_relate)
    return;

  loop_tags[loop] = PARALLEL;
  cout << "  Parallelize loop: "
       << codegen->_up(loop) + getUserCodeFileLocation(codegen, loop, true)
       << endl;

  // Update parallel range of the refs
  for (auto info : access_info) {
    auto arr_init = info.first;
    if (array_parallel_range.count(arr_init) == 0)
      continue;

    for (auto access_pair : info.second) {
      auto ref = access_pair.first;
      for (auto A : access_pair.second) {
        auto dim_info = A.first;
        auto coeff = A.second;
        if (array_parallel_range[arr_init].count(dim_info) == 0)
          continue;

        // Update the access parallel range after deciding to parallel
        // a trivial loop
        if (access_parallel_tags[arr_init][ref].count(dim_info) == 0) {
          access_parallel_tags[arr_init][ref][dim_info] =
              parallel_factor * coeff;
        } else {
          // More levels of parallel apply on one dimension
          access_parallel_tags[arr_init][ref][dim_info] +=
              parallel_factor * coeff;
        }
      }
    }
  }
}

void AutoParallel::ScanCandidates() {
  cout << "\n\n START Candidate analysis\n====\n";
  // Obtain All sink nodes from DAG,
  auto terminal_loops = loop_info_builder->GetTerminalLoops();
  int64_t total_lat = 0;
  for (auto &loop : terminal_loops) {
    int64_t lat = lty_estimator->GetLoopLatency(loop, false);
    // DEBUG PRINT LOOP LOCATION AND LATENCY
    if (lat == 0)
      continue;
    total_lat += lat;
    parallel_frontiers.emplace(lat, loop);
  }

  //  Pipeline/parallel critical loops
  ProcessingCriticalLoopsByLatency(total_lat);

  //  Yuxin: Aug 27 2020
  //  Pop the trivial loops into a normal queue
  //  Since the rest loops are trivial loops
  //  No need to consider latency
  while (!parallel_frontiers.empty()) {
    auto node = parallel_frontiers.top();
    void *loop = node.second;
    parallel_frontiers.pop();
    rest_frontiers.push(loop);
  }

  //  Remaining loops
  ProcessingTrivialLoopsByPartition();
}

int AutoParallel::run() {
  bool Changed = false;
  auto kernels = mars_ir_v2.get_top_kernels();
  for (auto kernel : kernels) {
    loop_tags.clear();
    loop_info_builder =
        make_shared<LoopInfoBuilder>(codegen, kernel, &mars_ir_v2, &mars_ir);
    if (mXilinx_flow) {
      Changed |= insertPragmaBasedOnXilinxChannel(kernel);
    }
    loop_info_builder->run();
    lty_estimator =
        make_shared<LatencyEstimator>(codegen, kernel, loop_info_builder);
    res_estimator =
        make_shared<ResourceEstimator>(codegen, kernel, loop_info_builder);
    ScanCandidates();
    Changed |= InsertPragma();
    loop_info_builder->RemoveTagPragmas();
  }

  return Changed;
}

bool AutoParallel::insertPragmaBasedOnXilinxChannel(
    void *func, unordered_set<void *> *visited_funcs,
    unordered_set<void *> *processed_loops,
    std::stack<pair<void *, void *>> *call_stack) {
  if (visited_funcs->count(func) > 0) {
    return false;
  }
  visited_funcs->insert(func);
  vector<void *> vec_calls;
  codegen->GetNodesByType(func, "preorder", "SgFunctionCallExp", &vec_calls);
  bool Changed = false;
  for (auto call : vec_calls) {
    auto sub_func = codegen->GetFuncDeclByCall(call);
    if (sub_func != nullptr) {
      call_stack->push(std::make_pair(sub_func, call));
      Changed |= insertPragmaBasedOnXilinxChannel(sub_func, visited_funcs,
                                                  processed_loops, call_stack);
      call_stack->pop();
    } else {
      void *ref = nullptr;
      if (codegen->is_xilinx_channel_read(call, &ref) ||
          codegen->is_xilinx_channel_write(call, &ref)) {
        Changed |= insertParallelBasedOnXilinxChannelIndex(ref, processed_loops,
                                                           call_stack);
      }
    }
  }
  return Changed;
}

bool AutoParallel::insertPragmaBasedOnXilinxChannel(void *kernel) {
  unordered_set<void *> visited_funcs;
  unordered_set<void *> processed_loops;
  std::stack<pair<void *, void *>> call_stack;
  return insertPragmaBasedOnXilinxChannel(kernel, &visited_funcs,
                                          &processed_loops, &call_stack);
}

bool AutoParallel::insertParallelBasedOnXilinxChannelIndex(
    void *ref, unordered_set<void *> *processed_loops,
    stack<pair<void *, void *>> *call_stack) {
  void *array = nullptr;
  vector<void *> vec_indices;
  codegen->parse_pntr_ref_new(ref, &array, &vec_indices);
  bool Changed = false;
  for (auto index : vec_indices) {
    CMarsExpression me_index(index, codegen);
    for (auto atom : me_index.get_vars()) {
      if (codegen->IsForStatement(atom)) {
        if (processed_loops->count(atom) > 0)
          continue;
        if (loop_info_builder->LoopHasPragma(atom, PARALLEL, true, "") ||
            loop_info_builder->LoopHasPragma(atom, PARALLEL, false, "")) {
          processed_loops->insert(atom);
          continue;
        }
        void *loop_body = codegen->GetLoopBody(atom);
        codegen->PrependChild(
            loop_body, codegen->CreatePragma("ACCEL PARALLEL", loop_body));
        processed_loops->insert(atom);
        Changed = true;
      }
    }
  }
  if (codegen->IsArgumentInitName(array)) {
    if (call_stack->empty())
      return Changed;
    auto top = call_stack->top();
    auto call = top.second;
    auto caller = top.first;
    int param_index = -1;
    int i = 0;
    for (auto param : codegen->GetFuncParams(caller)) {
      if (param == array) {
        param_index = i;
        break;
      }
      i++;
    }
    void *argument = codegen->GetFuncCallParam(call, param_index);
    if (argument == nullptr)
      return Changed;

    call_stack->pop();
    Changed |= insertParallelBasedOnXilinxChannelIndex(
        argument, processed_loops, call_stack);
    call_stack->push(top);
  }
  return Changed;
}

bool AutoParallel::contains_channel_access(void *scope) {
  if (m_channel_access.count(scope) > 0) {
    return m_channel_access[scope];
  }
  vector<void *> vec_calls;
  codegen->GetNodesByType_int(scope, "preorder", V_SgFunctionCallExp,
                              &vec_calls);
  bool check_res = false;
  for (auto call : vec_calls) {
    if (codegen->is_xilinx_channel_read(call, nullptr) ||
        codegen->is_xilinx_channel_write(call, nullptr)) {
      check_res = true;
      break;
    }
    void *decl = codegen->GetFuncDeclByCall(call);
    if (decl != nullptr && contains_channel_access(decl)) {
      check_res = true;
      break;
    }
  }
  m_channel_access[scope] = check_res;
  return check_res;
}

bool remove_parallel_pipeline_off_pragma(CSageCodeGen *codegen, void *pTopFunc,
                                         const CInputOptions &options) {
  bool Changed = false;
  vector<void *> pragmaStmts;
  codegen->GetNodesByType(pTopFunc, "preorder", "SgPragmaDeclaration",
                          &pragmaStmts);
  for (auto pragma : pragmaStmts) {
    if (codegen->IsFromInputFile(pragma) == 0 ||
        codegen->isWithInHeaderFile(pragma)) {
      continue;
    }
    CAnalPragma ana_pragma(codegen);
    bool errorOut = false;
    if (ana_pragma.PragmasFrontendProcessing(pragma, &errorOut, false)) {
      if (ana_pragma.is_pipeline() || ana_pragma.is_parallel()) {
        if (!ana_pragma.get_attribute(CMOST_OFF).empty() ||
            (ana_pragma.is_parallel() &&
             ana_pragma.get_attribute(CMOST_parallel_factor) == "1" &&
             "sdaccel" == options.get_option_key_value("-a", "impl_tool"))) {
          codegen->RemoveStmt(pragma);
          Changed = true;
        }
      }
    }
  }
  return Changed;
}

int AutoParallelInsertionTop(CSageCodeGen *codegen, void *pTopFunc,
                             const CInputOptions &options) {
  // Yuxin: Aug/31/2020
  // keep loop tiling before auto_parallel
  // Add partial parallel tags
  loop_tiling_top(codegen, pTopFunc, options, true);

  AutoParallel auto_parallelizer(codegen, pTopFunc, options);
  auto_parallelizer.run();

  //  remove parallel/pipeline off pragma
  remove_parallel_pipeline_off_pragma(codegen, pTopFunc, options);

  return 0;
}
