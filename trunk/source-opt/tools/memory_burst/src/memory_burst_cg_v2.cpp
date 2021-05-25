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


#include "memory_burst.h"
#include <iostream>
#include <string>

//  #include "rose.h"
#include "math.h"
#include "codegen.h"
#include "mars_ir.h"
#include "mars_message.h"
#include "mars_opt.h"
#include "program_analysis.h"
using std::map;
using std::pair;
using std::set;
using std::vector;
//  using namespace SageInterface;
//  using namespace SageBuilder;
using MarsProgramAnalysis::CMarsAccess;
using MarsProgramAnalysis::CMarsExpression;
using MarsProgramAnalysis::CMarsRangeExpr;
#if 0
bool MemoryBurst::process_coarse_grained_top_v2() {
  bool Changed = false;
  int i = 0;
  while (i < mMars_ir_v2.size()) {
    CMarsNode *node = mMars_ir_v2.get_node(i);
    void *kernel_top = node->get_user_kernel();
    vector<int> order_res;
    mMars_ir_v2.get_order_of_sub_nodes(kernel_top, vector<int>(), &order_res);
    vector<void*> top_loops;
    for (int j = 0; j != order_res.size(); ++j) {
      void *loop = mMars_ir_v2.get_loop_for_order(kernel_top,
                               vector<int>(1, order_res[j]));
      if (loop)
        top_loops.push_back(loop);
    }
    Changed |= process_all_scopes(top_loops);
    pair<int, int> range = mMars_ir_v2.get_node_range(kernel_top);
    i = range.second;
  }
  return Changed;
}
#else
bool MemoryBurst::process_coarse_grained_top_v2() {
  mMars_ir_v2.build_mars_ir(m_ast, mTopFunc, false, true);
  bool Changed = false;
  for (size_t i = 0; i < mMars_ir_v2.size(); ++i) {
    CMarsNode *node = mMars_ir_v2.get_node(i);
    Changed |= process_one_node(node);
  }
  return Changed;
}
#endif

bool MemoryBurst::check_identical_range_in_multiple_nodes(void *port,
                                                          void *scope,
                                                          bool report) {
  void *loop = m_ast->TraceUpToForStatement(scope);
  vector<CMarsNode *> all_nodes;
  if (loop != nullptr) {
    all_nodes = mMars_ir_v2.get_nodes_with_common_loop(loop);
  } else {
    //  collect all the nodes
    all_nodes = mMars_ir_v2.get_all_nodes();
  }
  vector<CMarsNode *> access_nodes;
  for (auto &node : all_nodes) {
    auto ports = node->get_ports();
    if (ports.count(port) > 0) {
      access_nodes.push_back(node);
    }
  }
  vector<vector<CMarsRangeExpr>> all_vec_range;
  for (auto &node : access_nodes) {
    if (node->port_is_read(port)) {
      all_vec_range.push_back(node->get_port_access_union(port, scope, true));
    }
    if (node->port_is_write(port)) {
      all_vec_range.push_back(node->get_port_access_union(port, scope, false));
    }
  }
  if (all_vec_range.size() <= 1) {
    return true;
  }

  vector<CMarsRangeExpr> first_vec_range = all_vec_range[0];
  bool range_match = true;
  for (size_t i = 1; i < all_vec_range.size(); ++i) {
    vector<CMarsRangeExpr> next_vec_range = all_vec_range[i];
    if (first_vec_range.size() != next_vec_range.size()) {
      range_match = false;
      break;
    }
    for (size_t j = 0; j != first_vec_range.size(); ++j) {
      if ((first_vec_range[j].is_exact() == 0) ||
          (next_vec_range[j].is_exact() == 0)) {
        range_match = false;
        break;
      }
      CMarsExpression fl;
      CMarsExpression fu;
      CMarsExpression nl;
      CMarsExpression nu;
      if ((first_vec_range[j].get_simple_bound(&fl, &fu) == 0) ||
          (next_vec_range[j].get_simple_bound(&nl, &nu) == 0)) {
        range_match = false;
        break;
      }
      if (((fl - nl != 0) != 0) || ((fu - nu != 0) != 0)) {
        range_match = false;
        break;
      }
    }
    if (!range_match) {
      break;
    }
  }
  if (!range_match && report) {
    string var_info = "'" + m_ast->GetVariableName(port) + "' " +
                      getUserCodeFileLocation(m_ast, port, true);
    dump_critical_message(BURST_WARNING_9(var_info));
#if PROJDEBUG
    cout << "Cannot infer burst for port " << var_info
         << "because of unmatched access range" << endl;
#endif
  }
  return range_match;
}

bool MemoryBurst::check_identical_indices_within_single_node(void *port,
                                                             void *scope,
                                                             bool report) {
  void *loop = m_ast->TraceUpToForStatement(scope);
  string var_name = m_ast->GetVariableName(port);
  string var_info =
      "\'" + var_name + "\' " + getUserCodeFileLocation(m_ast, port, true);
  vector<CMarsNode *> all_nodes;
  if (loop != nullptr) {
    all_nodes = mMars_ir_v2.get_nodes_with_common_loop(loop);
  } else {
    //  collect all the nodes
    all_nodes = mMars_ir_v2.get_all_nodes();
  }
  vector<CMarsNode *> access_nodes;
  for (auto &node : all_nodes) {
    auto ports = node->get_ports();
    if (ports.count(port) > 0) {
      access_nodes.push_back(node);
    }
  }
  vector<vector<CMarsRangeExpr>> all_vec_range;
  for (auto &node : access_nodes) {
    //  check whether multiple references have the same indices
    auto refs = node->get_port_references(port);
    if (!mMars_ir_v2.check_identical_indices_without_simple_type(refs)) {
#if PROJDEBUG
      cout << "Cannot infer burst \'" << var_name << "\'"
           << " because of multiple references in a single node" << endl;
#endif
      if (report) {
        dump_critical_message(BURST_WARNING_8(var_info));
      }
      return false;
    }
  }
  return true;
}

bool MemoryBurst::check_coarse_grained_write_only(void *port, void *scope) {
  if (!mAltera_flow || mEffort <= STANDARD) {
    return false;
  }
  void *loop = m_ast->TraceUpToForStatement(scope);
  vector<CMarsNode *> all_nodes;
  if (loop != nullptr) {
    all_nodes = mMars_ir_v2.get_nodes_with_common_loop(loop);
  } else {
    //  collect all the nodes
    all_nodes = mMars_ir_v2.get_all_nodes();
  }
  vector<CMarsNode *> access_nodes;
  for (auto &node : all_nodes) {
    auto ports = node->get_ports();
    if (ports.count(port) <= 0 || !node->port_is_read(port)) {
      continue;
    }
    if (mMars_ir_v2.has_definite_coarse_grained_def(node, port)) {
      continue;
    }
    return false;
  }
  return true;
}

void MemoryBurst::update_mars_ir(void *port, void *scope) {
  //  remove all the ports in all the nodes under the same scope
  void *loop = m_ast->TraceUpToForStatement(scope);
  vector<CMarsNode *> all_nodes;
  if (loop != nullptr) {
    all_nodes = mMars_ir_v2.get_nodes_with_common_loop(loop);
  } else {
    //  collect all the nodes
    all_nodes = mMars_ir_v2.get_all_nodes();
  }
  for (auto &node : all_nodes) {
    auto ports = node->get_ports();
    if (ports.count(port) > 0) {
      node->remove_port(port);
    }
  }
}

bool MemoryBurst::process_one_node(CMarsNode *node) {
  auto loops = node->get_loops();
  auto ports = node->get_ports();
  if (loops.empty()) {
    return false;
  }
  bool Changed = false;
  void *curr_scope = loops.back();
  bool cg_pipeline_check = mEffort > STANDARD;
  for (auto &curr_array : ports) {
    if (!mMars_ir_v2.is_kernel_port(curr_array)) {
      continue;
    }
    void *type = m_ast->GetTypebyVar(curr_array);
    if ((m_ast->IsPointerType(type) == 0) && (m_ast->IsArrayType(type) == 0)) {
      continue;
    }
    if (!is_burst_candidate(curr_array, m_ast->TraceUpToFuncDecl(curr_array),
                            curr_array, false)) {
      continue;
    }
    void *element_type = m_ast->GetBaseType(type, true);
    int element_size = m_ast->get_bitwidth_from_type(element_type) / 8;
    //  2. Get the surrounding for-loops of the pipelined loop and the array
    //  init_name
    //    It is an across-function analysis, the init_name is updated when
    //    crossing functions upward
    //    Stop tracing upward at
    //    a) kernel top function arrived
    //    b) actual argument is not simple pointer (e.g. alias or expressions)
    //    c) multiple function calls (currently as is, to be enhanced)
    vector<void *> vec_upper_scope;  //  from inner to outer
    vector<void *> vec_array_in_upper_scope;
    cg_get_options_for_lifting(curr_scope, curr_array, &vec_upper_scope,
                               &vec_array_in_upper_scope, true);

    void *scope_to_lift = nullptr;
    void *array_to_lift = nullptr;
    {
      void *pre_scope = nullptr;
      void *pre_array = nullptr;
      bool report = false;
#if PROJDEBUG
      cout << "analyze port \'" << m_ast->GetVariableName(curr_array) << "\'"
           << endl;
#endif
      for (size_t k = 0; k < vec_upper_scope.size(); k++) {
        void *lift_array = vec_array_in_upper_scope[k];
        void *lift_scope = vec_upper_scope[k];
        int64_t new_buff_size = 0;
        //  check whether all the nodes under the same scope have
        //  the same range to avoid accesses in the multiple nodes
        report = (pre_scope == nullptr);
        if (cg_pipeline_check) {
          if (!check_identical_range_in_multiple_nodes(lift_array, lift_scope,
                                                       report)) {
            break;
          }

          if (!check_identical_indices_within_single_node(lift_array,
                                                          lift_scope, report)) {
            break;
          }
        }
        report = vec_upper_scope.size() - 1 == k && (pre_scope == nullptr);
        int is_cf_applied = is_cg_burst_available(
            lift_array, lift_scope, element_size, &new_buff_size, nullptr,
            cg_pipeline_check, report);
        //  if (!is_cf_applied) break;
        if (is_cf_applied != BURST_APPLIED) {
          continue;
        }
        pre_scope = lift_scope;
        pre_array = lift_array;
      }

      scope_to_lift = pre_scope;
      array_to_lift = pre_array;
    }
    if ((scope_to_lift == nullptr) || (array_to_lift == nullptr)) {
      continue;
    }
    //  3. Get the surrounding for-loops of the pipelined loop and the array
    //  init_name
    Changed |=
        cg_memory_burst_transform(scope_to_lift, array_to_lift, false, true);
    update_mars_ir(array_to_lift, scope_to_lift);
  }
  return Changed;
}
#if 0
bool MemoryBurst::process_all_scopes(vector<void*> &top_loops) {
  bool Changed = false;
  for (int i = 0; i < top_loops.size(); i++) {
    void *curr_scope = top_loops[i];

    //  1. Get all the initialized names of the interface arrays of the current
    //  function
    vector<void *> vec_arrays;
    {
      void *func = m_ast->TraceUpToFuncDecl(curr_scope);
      int arg_size = m_ast->GetFuncParamNum(func);
      for (int j = 0; j != arg_size; ++j) {
        void *arg = m_ast->GetFuncParam(func, j);
        void *type = m_ast->GetTypebyVar(arg);
        if (!m_ast->IsPointerType(type) && !m_ast->IsArrayType(type))
          continue;
        //  if (mMars_ir_v2.every_trace_is_bus(m_ast, arg))
        if (mMars_ir_v2.is_kernel_port(arg))
          vec_arrays.push_back(arg);
      }
    }

    for (int j = 0; j < vec_arrays.size(); j++) {
      void *curr_array = vec_arrays[j];
      void *element_type = m_ast->GetBaseType(
          m_ast->GetTypebyVar(curr_array), true);
      int element_size = m_ast->get_bitwidth_from_type(element_type) / 8;
      int buff_size;
      int is_cf_applied = is_cg_burst_available(curr_array, curr_scope,
                           element_size, buff_size, true);
      if (!is_cf_applied) continue;
      Changed |= cg_memory_burst_transform(curr_scope, curr_array);
    }
  }
  return Changed;
}
#endif
