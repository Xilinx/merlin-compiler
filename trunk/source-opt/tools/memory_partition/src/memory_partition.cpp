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


#include <iostream>
#include <string>

#include "memory_partition.h"

#define INVALID_DIM (-1)
#define CONST_ITER nullptr
#define DefaultHeuriThresh 64

using MarsProgramAnalysis::CMarsAccess;
using MarsProgramAnalysis::CMarsExpression;
using std::unordered_map;
using std::unordered_set;

void MemoryPartition::init() {
  if ("aocl" == mOptions.get_option_key_value("-a", "impl_tool")) {
    mAltera_flow = true;
    cout << "[MARS-PARALLEL-MSG] Intel flow mode.\n";
  }
  if ("sdaccel" == mOptions.get_option_key_value("-a", "impl_tool")) {
    mXilinx_flow = true;
    cout << "[MARS-PARALLEL-MSG] Xilinx flow mode.\n";
  }
  m_transform_level = "low";
  if ("medium" == mOptions.get_option_key_value("-a", "index_transform")) {
    m_transform_level = "medium";
  }

  // bits
  m_reshape_threshold = 4096;
  if (!mOptions.get_option_key_value("-a", "auto_reshape_threshold").empty()) {
    m_reshape_threshold =
        my_atoi(mOptions.get_option_key_value("-a", "auto_reshape_threshold"));
  }
  if (m_reshape_threshold == 0) {
    m_reshape_threshold = 4096;
  }

  //  Yuxin: Jun 27 2018
  //  dual port partitioning is turned on by default
  m_dual_option = true;
  if ("off" == mOptions.get_option_key_value("-a", "dual_port_mem")) {
    m_dual_option = false;
  }

  m_heuristic_threshold = DefaultHeuriThresh;
  if (!mOptions.get_option_key_value("-a", "partition_heuristic_threshold")
           .empty()) {
    m_heuristic_threshold = my_atoi(
        mOptions.get_option_key_value("-a", "partition_heuristic_threshold"));
  }
  if (m_heuristic_threshold == 0) {
    m_heuristic_threshold = DefaultHeuriThresh;
  }
  m_partitions.clear();
  m_actions.clear();
  m_registers.clear();
  has_out_of_bound = false;
}

bool MemoryPartition::run() {
  cout << "===============================================" << endl;
  cout << "=========>  Memory Partition Optimization Start\n";

  if (mAltera_flow) {
    insert_partition_intel();
  }
  if (mXilinx_flow) {
    insert_partition_xilinx();
  }

  cout << "==============================================" << endl;
  return true;
}

void MemoryPartition::insert_partition_xilinx() {
  mMars_ir->check_hls_partition(m_ast, mTopFunc, false);
  check_hls_resource();
#if PROJDEBUG
  mMars_ir->print_hls_partition(m_ast);
#endif

  partition_analysis();
  partition_merge();
  partition_transform_xilinx();
  reportPartitionMessage();
  delete_existing_pragma();
}

void MemoryPartition::partition_analysis() {
  cout << "\n\n ###### Partition analysis...\n";
  loop_partition_builder = new LoopPartitionBuilder(m_ast);
  loop_partition_builder->run();

  vector<CMirNode *> vec_nodes;
  mMars_ir->get_topological_order_nodes(&vec_nodes);
  for (size_t j = 0; j < vec_nodes.size(); j++) {
    CMirNode *new_node = vec_nodes[j];
    if (!new_node->is_fine_grain && mAltera_flow) {
      continue;
    }
    if (new_node->is_while && new_node->has_parallel() &&
        new_node->is_complete_unroll()) {
      continue;
    }

    m_actions[new_node] = true;
    bool turn_on = true;
    if (new_node->has_partition(&turn_on) != 0) {
      if (!turn_on) {
        continue;
      }
    }
    int ret = partition_analysis_node(new_node);
    if (!ret) {
      m_actions[new_node] = false;
    } else {
      tmp_factors[new_node] = loop_partition_builder->GetPartitions(new_node);
    }
    has_out_of_bound |= loop_partition_builder->loop_info_map[new_node]
                            ->has_out_of_bound_access;
  }
  if (has_out_of_bound)
    throw std::exception();
}

int LoopPartitionBuilder::run() {
  loop_info_map.clear();
  return 0;
}

int LoopPartitionBuilder::SetLoopOpt(CMirNode *loop_node) {
  if (loop_node->has_parallel() != 0 || loop_node->has_hls_unroll() != 0) {
    void *sg_loop = m_ast->TraceUpToLoopScope(loop_node->ref);
    int64_t l_range = 0;
    auto ret = m_ast->get_loop_trip_count_simple(sg_loop, &l_range);
    if (loop_node->is_complete_unroll()) {
      if (ret == 0) {
        return 0;
      }
      if (l_range == 1)
        loop_info_map[loop_node]->opt = 1;
      else
        loop_info_map[loop_node]->opt = 0;
    } else {
      string unroll_factor = loop_node->get_attribute(CMOST_parallel_factor);
      if (loop_node->is_partial_unroll(&loop_info_map[loop_node]->opt) != 0) {
        if ((ret != 0) && loop_info_map[loop_node]->opt > l_range) {
          loop_info_map[loop_node]->opt = 0;
        }
      } else {
        if (loop_info_map[loop_node]->opt <= 0) {
          return false;
        }
      }
    }
  }
  if ((loop_node->has_pipeline() == 0) && (loop_node->has_parallel() == 0) &&
      loop_node->has_hls_unroll() == 0) {
    loop_info_map[loop_node]->opt = -2;
  }
  if (!loop_node->is_fine_grain) {
    loop_info_map[loop_node]->opt = -2;
  }
  return 1;
}

int MemoryPartition::partition_analysis_node(CMirNode *loop_node) {
  cout << "\n\nPartition analysis node...\n";
  MerlinStreamModel::CStreamIR stream_ir(m_ast, mOptions, mMars_ir,
                                         mMars_ir_v2);
  stream_ir.ParseStreamModel();

  //  '-1'- pipeline; '0'- complete unroll; "x"- x>0, unroll factor
  loop_partition_builder->loop_info_map[loop_node] = new LoopInfo(loop_node);
  int ret = loop_partition_builder->SetLoopOpt(loop_node);
  if (ret == 0)
    return false;
  int opt = loop_partition_builder->GetLoopOpt(loop_node);
  cout << "Partition opt is: " << opt << endl;

  loop_node->set_full_access_table_v2(m_ast);
  for (auto pList : loop_node->full_access_table_v2) {
    void *arr_init = pList.first;
    if (mAltera_flow && m_registers.count(arr_init) > 0) {
      continue;
    }
    if (stream_ir.is_fifo(arr_init)) {
      continue;
    }
    // Yuxin: Dec/16/2019, dont partition union members
    // we havent find a corret way to insert partition pragma for them
    if (m_ast->IsUnionMemberVariable(arr_init) != 0) {
      continue;
    }
    void *func_decl = m_ast->GetEnclosingNode("Function", arr_init);
    map<void *, bool> res;
    if (mMars_ir->any_trace_is_bus(m_ast, func_decl, arr_init, &res,
                                   loop_node->ref)) {
      continue;
    }
    cache_trace_res[arr_init] = res;

    void *base_type;
    vector<size_t> arr_size;
    m_ast->get_type_dimension(m_ast->GetTypebyVar(arr_init), &base_type,
                              &arr_size);
    std::reverse(arr_size.begin(), arr_size.end());
    cache_var_size[arr_init] = arr_size;
    cache_var_dim[arr_init] = arr_size.size();
    cache_var_base_type[arr_init] = base_type;

    AccessInfo *curr_access_info = new AccessInfo(arr_init);
    curr_access_info->dual_option = m_dual_option;
    check_resource_port_setting(arr_init, &curr_access_info->dual_option);

    cout << "\n==>Array: " << m_ast->_up(arr_init) << endl;
    bool succeed = true;
    vector<pair<int, int>> priority_sorting;
    AnalyzeAccess(loop_node, curr_access_info, &priority_sorting);
    for (auto p_pair : priority_sorting) {
      auto dim = p_pair.second;
      int factor = 1;
      bool ret = true;
      // reset
      curr_access_info->heuristic_on = false;
      ret = partition_factor_anal(loop_node, curr_access_info, dim, &factor);
      if (ret) {
        if (factor == 0) {
          if (cache_var_size[arr_init][dim] <= XilinxThresh) {
            curr_access_info->local_pt_factor[dim] = factor;
          } else {
            curr_access_info->local_pt_factor[dim] = XilinxThresh;
            reportThresholdFactor(loop_node, arr_init, dim);
          }
        } else if (factor > 1) {
          if (factor <= XilinxThresh) {
            curr_access_info->local_pt_factor[dim] = factor;
          } else {
            curr_access_info->local_pt_factor[dim] = XilinxThresh;
            reportThresholdFactor(loop_node, arr_init, dim);
          }
        }

        if (factor == 0) {
          cout << "         ==> dimension " << dim + 1;
          cout << " ==> complete partition \n";
        } else if (factor > 1) {
          cout << "         ==> dimension " << dim + 1;
          cout << "==> partition factor: " << factor << endl;
        } else {
          cout << "         ==> dimension " << dim + 1;
          cout << "==> factor partition factor: " << factor << endl;
        }
      } else {
        succeed = false;
        cout << "[warning] No conflict-free partitioning solution for array "
             << m_ast->_p(arr_init) << ", dimension " << dim + 1 << endl;
      }
    }
    // Only apply to one dimension
    if (curr_access_info->dual_option) {
      dual_port_adjust(arr_init, &curr_access_info->local_pt_factor);
      curr_access_info->dual_option = false;
    }

    if (!succeed && opt == -1) {
      if (!loop_node->is_autoFG()) {
        reportSuboptimalFGPIP(loop_node, arr_init);
      }
    }
    if (!curr_access_info->local_pt_factor.empty()) {
      loop_partition_builder->SetPartitions(loop_node, arr_init,
                                            curr_access_info->local_pt_factor);
    }
  }

  // print_partition(m_ast, *var2partition);
  return true;
}

void MemoryPartition::partition_merge() {
  cout << "\n\n ######Partition merge...\n";

  //  1. Reorganize the partitioning factors
  map<void *, vector<map<int, int>>> v_factors;
  int ret = static_cast<int>(reorganize_factor(&v_factors));
  if (ret == 0) {
    return;
  }

  //  2. Merge the factors
  for (auto v : v_factors) {
    void *var = v.first;
    cout << "==> Array merge: " << m_ast->_p(var) << endl;
    map<int, int> m_factors;
    if (cache_var_size.count(var) <= 0) {
      void *base_type;
      vector<size_t> arr_size;
      m_ast->get_type_dimension(m_ast->GetTypebyVar(var), &base_type,
                                &arr_size);
      std::reverse(arr_size.begin(), arr_size.end());
      cache_var_size[var] = arr_size;
      cache_var_dim[var] = arr_size.size();
    }
    if (v.second.size() == 1) {
      m_partitions[var] = *v.second.begin();
      continue;
    }

    int ret = choose_factor(cache_var_size[var], v.second, &m_factors,
                            &prior_factors[var]);
    if ((ret == 0) && mAltera_flow) {
      reportSuboptimalMemIntel(var);
    }
    m_partitions[var] = m_factors;
  }

  //  3. Disable the node which has no partitioning applied
  for (auto p : tmp_factors) {
    bool action = true;
    void *arr_report;
    for (auto pp : p.second) {
      void *arr_init = pp.first;
      cout << "Check init: " << m_ast->_up(arr_init) << endl;
      if (m_partitions.find(arr_init) != m_partitions.end()) {
        map<int, int> local_factor = pp.second;
        for (auto itt : m_partitions[arr_init]) {
          int dim = itt.first;
          if (local_factor.find(dim) != local_factor.end()) {
            action &= check_action(local_factor[dim], itt.second);
          }
        }
      } else {
        void *func_decl = m_ast->GetEnclosingNode("Function", arr_init);
        map<void *, bool> res;
        if (cache_trace_res.count(arr_init) == 0) {
          mMars_ir->trace_to_bus(m_ast, func_decl, arr_init, &res,
                                 p.first->ref);
          cache_trace_res[arr_init] = res;
        } else {
          res = cache_trace_res[arr_init];
        }

        for (auto r : res) {
          if (m_partitions.find(r.first) != m_partitions.end()) {
            int partial_dim = m_ast->get_dim_num_from_var(r.first) -
                              m_ast->get_dim_num_from_var(arr_init);

            map<int, int> local_factor = pp.second;
            for (auto itt : m_partitions[r.first]) {
              int dim = itt.first;
              if (local_factor.find(dim - partial_dim) != local_factor.end()) {
                action &=
                    check_action(local_factor[dim - partial_dim], itt.second);
              }
            }
          }
        }
      }
      if (!action) {
        arr_report = arr_init;
        break;
      }
    }
    if (!action && (p.first->has_pipeline() != 0) && !p.first->is_autoFG()) {
      reportSuboptimalFGPIP(p.first, arr_report);
    }
    m_actions[p.first] &= action;
  }
}

void MemoryPartition::delete_existing_pragma() {
  map<void *, vector<void *>> existing_table = mMars_ir->get_partition_table();
  vector<pair<string, void *>> vec_pragma;
  for (auto it : existing_table) {
    for (auto p_it : it.second) {
      SgPragmaDeclaration *decl = static_cast<SgPragmaDeclaration *>(p_it);
      string pragmaString = decl->get_pragma()->get_pragma();
      vec_pragma.emplace_back(pragmaString, decl);
    }
  }
  sort(vec_pragma.begin(), vec_pragma.end());
  set<void *> removed;
  for (auto one_pair : vec_pragma) {
    void *decl = one_pair.second;
    string pragmaString = one_pair.first;
    m_ast->AddComment_v1(decl,
                         "Existing HLS partition: #pragma " + pragmaString);
    // Yuxin: Dec/16/2019, a pragma maybe has multiple variables
    if (removed.count(decl) == 0) {
      m_ast->RemoveStmt(decl);
      removed.insert(decl);
    }
  }
}

void MemoryPartition::partition_transform_xilinx() {
  cout << "\n\n ######Partition transform...\n";
  for (auto it : m_partitions) {
    void *arr_init = it.first;
    bool succeed = false;
    if (m_ast->IsArgumentInitName(arr_init) != 0) {
      continue;
    }
    bool fully_partition = true;
    fully_partition &= (it.second.size() == cache_var_dim[arr_init]);
    for (auto ps : it.second) {
      if (prior_factors.find(arr_init) != prior_factors.end()) {
        if (prior_factors[arr_init].find(ps.first) !=
            prior_factors[arr_init].end()) {
          cout << "Existing pragmas maybe in header files.\n";
          continue;
        }
      }
      int factor = ps.second;
      if (ps.second != 0 && fully_partition)
        fully_partition = false;
      succeed |= partition_pragma_gen_xilinx(arr_init, ps.first, factor);
    }
    if (succeed && fully_partition) {
      if (hls_resource_pragmas.count(arr_init) > 0) {
        for (size_t i = 0; i < hls_resource_pragmas[arr_init].size(); i++) {
          m_ast->RemoveStmt(hls_resource_pragmas[arr_init][i]);
        }
      }
    }
  }
#if 0
    array_index_transform(m_ast, mMars_ir, m_actions, m_partitions);
#endif
}

bool MemoryPartition::partition_pragma_gen_xilinx(void *arr_init, int dim,
                                                  int factor) {
  void *sg_scope = nullptr;
  void *arr_decl = m_ast->GetVariableDecl(arr_init);
  string arr_for_pragma = m_ast->_up(arr_init);

  static bool has_collect_all_local_variables = false;
  static vector<void *> vec_decls;
  if (!has_collect_all_local_variables) {
    unordered_set<void *> visited_funcs;
    for (auto kernel : mMars_ir_v2->get_top_kernels()) {
      collect_local_variables(kernel, &vec_decls, &visited_funcs);
    }
    has_collect_all_local_variables = true;
  }
  vector<void *> pragma_insert_scope;
  vector<string> pragma_insert_name;

  if (m_ast->IsGlobalInitName(arr_init) != 0) {
    vector<void *> vec_tmp;
    m_ast->GetNodesByType_compatible(m_ast->GetProject(), "SgVarRef", &vec_tmp);
    for (size_t i = 0; i < vec_tmp.size(); i++) {
      void *ref = vec_tmp[i];
      void *target_init = m_ast->GetVariableInitializedName(ref);
      if (target_init == arr_init) {
        void *sg_decl = m_ast->GetEnclosingNode("Function", ref);
        sg_scope = m_ast->GetFuncBody(sg_decl);
        if (sg_scope != nullptr) {
          break;
        }
      }
    }
    pragma_insert_name.push_back(arr_for_pragma);
    pragma_insert_scope.push_back(sg_scope);
  } else if (m_ast->IsMemberVariable(arr_init)) {
    for (auto decl : vec_decls) {
      vector<void *> vec_field_exp = m_ast->GetDataMemberExp(decl, arr_init);
      for (auto field_exp : vec_field_exp) {
        pragma_insert_name.push_back(m_ast->_up(field_exp));
        pragma_insert_scope.push_back(decl);
      }
    }
  } else if (m_ast->IsArgumentInitName(arr_init) != 0) {
    void *func_decl = m_ast->GetEnclosingNode("Function", arr_init);
    if (func_decl == nullptr) {
      cout << "Couldnt generate new array_partition pragma for variable '"
           << arr_for_pragma << "': cannot find the related function\n";
      return false;
    }
    sg_scope = m_ast->GetFuncBody(func_decl);

    pragma_insert_name.push_back(arr_for_pragma);
    pragma_insert_scope.push_back(sg_scope);
  } else {
    pragma_insert_name.push_back(arr_for_pragma);
    pragma_insert_scope.push_back(arr_decl);
  }

  for (size_t i = 0; i < pragma_insert_name.size(); i++) {
    string arr_str = pragma_insert_name[i];
    m_ast->removeSpaces(&arr_str);
    void *sg_scope = pragma_insert_scope[i];
    string pragma_str;
    string DIM_str;
    DIM_str += std::string(CMOST_dim_low) + "=" + my_itoa(dim + 1);
    if (factor == 1) {
      return false;
    }
    if (factor == 0) {
      pragma_str = std::string(HLS_PRAGMA) + " " + CMOST_ARR_PARTITION_low +
                   " " + CMOST_variable_low + "=" + arr_str + " complete " +
                   DIM_str;
    } else {
      pragma_str = std::string(HLS_PRAGMA) + " " + CMOST_ARR_PARTITION_low +
                   " " + CMOST_variable_low + "=" + arr_str + " cyclic " +
                   CMOST_factor_low + "=" + my_itoa(factor) + " " + DIM_str;
    }

    cout << "[pragma gen] " << pragma_str << endl;
    if (sg_scope == nullptr) {
      cout << "Couldnt generate new array_partition pragma for variable '"
           << arr_str << "': cannot find the related scope\n";
      return false;
    }
    void *new_pragma = m_ast->CreatePragma(pragma_str, sg_scope);
    if (new_pragma == nullptr) {
      cout << "Couldnt generate new array_partition pragma for variable '"
           << arr_str << "'\n";
      return false;
    }

    if (m_ast->IsBasicBlock(sg_scope) != 0) {
      m_ast->PrependChild(sg_scope, new_pragma);
    } else if (m_ast->IsVariableDecl(sg_scope) != 0) {
      m_ast->InsertAfterStmt(new_pragma, sg_scope);
    } else {
      m_ast->InsertStmt(new_pragma, sg_scope);
    }
  }
  return true;
}

bool MemoryPartition::reorganize_factor(
    map<void *, vector<map<int, int>>> *v_factors) {
  // 1. Reorganize the factors calculated from different nodes
  for (auto nd : tmp_factors) {
    for (auto arr : nd.second) {
      map<int, int> new_temp;
      copy_factors(arr.second, &new_temp);
      if (v_factors->find(arr.first) != v_factors->end()) {
        (*v_factors)[arr.first].push_back(new_temp);
      } else {
        vector<map<int, int>> new_vec;
        new_vec.push_back(new_temp);
        (*v_factors)[arr.first] = new_vec;
      }
    }
  }

  //  2. Add the exsiting pragma factors
  map<void *, vector<void *>> existing_table = mMars_ir->get_partition_table();
  for (auto it : existing_table) {
    void *arr = it.first;
    cout << "Get partition table for " << m_ast->_up(arr) << endl;
    map<int, int> temp;
    for (auto p_it : it.second) {
      map<int, int> hls_factors;
      int ret = static_cast<int>(
          factor_extract(m_ast, mMars_ir, p_it, &hls_factors, arr));
      if (ret == 0) {
        cout << "[warning] Invalid input partitioning pragma for "
             << m_ast->_p(arr) << endl;
        continue;
      }
      cout << "Pragma extract: " << m_ast->_p(p_it) << endl;
      for (auto info : hls_factors) {
        cout << "[INFO] dim-" << info.first << ", factor-" << info.second
             << endl;
      }

      if (m_ast->IsFromInputFile(p_it) == 0) {
        cout << "Not in the input file. Pragma in high priority\n";
        for (auto h_it : hls_factors) {
          if (temp.find(h_it.first) == temp.end()) {
            temp[h_it.first] = h_it.second;
          } else {
            if (temp[h_it.first] != h_it.second) {
              cout << "Conflict from the input header files\n";
              string msg = "Conflict array partition pragmas in user input "
                           "file on variable '";
              msg += m_ast->_up(arr) + "'";
              msg += " dimension " + my_itoa(h_it.first + 1) + "\n";
              dump_critical_message("FGPIP", "ERROR", msg, 301);
              exit(-1);
            }
          }
        }
      }
      if (v_factors->find(arr) != v_factors->end()) {
        (*v_factors)[arr].push_back(hls_factors);
      } else {
        vector<map<int, int>> new_vec;
        new_vec.push_back(hls_factors);
        (*v_factors)[arr] = new_vec;
      }
    }
    prior_factors[arr] = temp;
  }

  //  3. Pass the vectors between arguments
  map<void *, bool> pass_factor;
  for (auto v : *v_factors) {
    void *pos = nullptr;
    if (m_ast->IsArgumentInitName(v.first) == 0) {
      pos = m_ast->GetVariableDecl(v.first);
    }

    void *base_type;
    vector<size_t> arr_size;  //  The size is stored reversely
    m_ast->get_type_dimension(m_ast->GetTypebyVar(v.first), &base_type,
                              &arr_size);
    std::reverse(arr_size.begin(), arr_size.end());
    void *func_decl = m_ast->GetEnclosingNode("Function", v.first);
    map<void *, bool> res;
    if (cache_trace_res.count(v.first) == 0) {
      mMars_ir->trace_to_bus(m_ast, func_decl, v.first, &res, pos);
      cache_trace_res[v.first] = res;
    } else {
      res = cache_trace_res[v.first];
    }

    vector<map<int, int>> arg_factors = v.second;
    if (arg_factors.empty()) {
      continue;
    }
    for (auto r : res) {
      void *var_trace = r.first;
      if (r.second) {
        continue;
      }
      if (var_trace == v.first) {
        continue;
      }
      void *base_type_r;
      vector<size_t> arr_size_r;  //  The size is stored reversely
      m_ast->get_type_dimension(m_ast->GetTypebyVar(var_trace), &base_type_r,
                                &arr_size_r);
      if (arr_size_r.size() == 0) {
        continue;
      }
      std::reverse(arr_size_r.begin(), arr_size_r.end());
      if (cache_var_size.count(var_trace) <= 0) {
        cache_var_size[var_trace] = arr_size_r;
        cache_var_dim[var_trace] = arr_size_r.size();
        cache_var_base_type[var_trace] = base_type_r;
      }
      int partial_dim = m_ast->get_dim_num_from_var(var_trace) -
                        m_ast->get_dim_num_from_var(v.first);
      if (partial_dim > 0) {
        cout << "Partial accesses\n";
      }

      if (v_factors->find(var_trace) != v_factors->end()) {
        copy_factors(arg_factors, &((*v_factors)[var_trace]), arr_size,
                     arr_size_r, partial_dim);
      } else {
        vector<map<int, int>> arr_factors;
        copy_factors(arg_factors, &arr_factors, arr_size, arr_size_r,
                     partial_dim);
        (*v_factors)[var_trace] = arr_factors;
      }
      pass_factor[v.first] = true;
      cout << "Pass factors from " << m_ast->_up(v.first) << " to "
           << m_ast->_up(var_trace) << endl;
    }
  }

  //  4. Clean the partition factors of the function arguements
  for (auto ps : pass_factor) {
    cout << "Erase: " << m_ast->_up(ps.first) << endl;
    v_factors->erase(ps.first);
    for (auto p : tmp_factors) {
      map<void *, map<int, int>> arr_factors = p.second;
      arr_factors.erase(ps.first);
      tmp_factors[p.first] = arr_factors;
    }
  }

  return true;
}

void MemoryPartition::reportPartitionMessage() {
  if (g_debug_mode != 0) {
    for (auto it : m_partitions) {
      void *arr_decl = it.first;
      map<int, int> local_factor = it.second;
      for (auto itt : local_factor) {
        if (itt.second == 1) {
          continue;
        }
        string var_str = m_ast->_up(arr_decl);
        string dim_info = my_itoa(itt.first + 1);
        string factor_info = " ";
        if (itt.second != 0) {
          factor_info += "with factor " + my_itoa(itt.second) + "\n";
        } else {
          factor_info += "completely\n";
        }
        dump_critical_message(FGPIP_INFO_1(var_str, dim_info, factor_info), 1);
      }
    }
  }
}

// Yuxin: July 10 2018
// In dual-port memory partitioning, the partitioning factor of
// the rightmost dimension is half as single port partitioning
void MemoryPartition::dual_port_adjust(void *arr_init,
                                       map<int, int> *local_partitions) {
  // Yuxin 07-Apr-19
  // For multidimensional arrays, when there are multiple dimensions are
  // partitioned dual port partition should not be applied
  if (cache_var_dim[arr_init] > 1 && local_partitions->size() > 1) {
    return;
  }
  for (auto &itt : *local_partitions) {
    int dim = itt.first;
    int factor = itt.second;
    // Only apply to the rightmost dimension
    cout << "dual: " << m_ast->_up(arr_init) << "," << dim << ", " << factor
         << endl;
    if (factor >= 2) {
      int local_factor;
      if ((factor % 2) != 0) {
        local_factor = factor / 2 + 1;
      } else {
        local_factor = factor / 2;
      }
      // Yuxin: 07/08/2019
      // Dual port aligned to power of two
      int factor_po2 = local_factor;
      while (!isPo2(factor_po2)) {
        factor_po2++;
      }
      if (factor_po2 < factor) {
        local_factor = factor_po2;
      }
      (*local_partitions)[dim] = local_factor;
    }
  }
}

void MemoryPartition::array_index_transform() {
  for (auto node : m_actions) {
    auto bNode = node.first;
    if (bNode->has_pipeline() == 0 && bNode->has_parallel() == 0 &&
        bNode->has_hls_fg_opt() == 0)
      continue;

    if (bNode->is_while || bNode->is_function)
      continue;
    index_transform(bNode);
  }
}

//  Yuxin: Jun 20 2018
//  Index transformation will be applied
//  By default: 1) the indices only have variables,
//             2) the coefficient of the variables cannot be divided by the
//             partition factor
//  option "index_transform=medium"
//             1) In addition to the conditions by default, the indices can
//             have iterators
void MemoryPartition::index_transform(CMirNode *bNode) {
  cout << "====>  index transform: \n";
  void *trans_for = m_ast->GetEnclosingNode("ForLoop", bNode->ref);
  cout << m_ast->_p(trans_for) << endl;

  // Only apply to the accesses in the innermost for loop
  if (m_ast->is_innermost_for_loop(trans_for) == 0) {
    return;
  }

  bool Changed = true;
  set<void *> transformed;
  while (Changed) {
    Changed = false;

    for (auto pList : bNode->full_access_table_v2) {
      void *arr_init = pList.first;
      if (transformed.find(arr_init) != transformed.end()) {
        continue;
      }

      void *base_type;
      vector<size_t> arr_size;
      m_ast->get_type_dimension(m_ast->GetTypebyVar(arr_init), &base_type,
                                &arr_size);

      bool check_rest = false;
      vector<int> vec_factor;
      if (m_ast->IsArgumentInitName(arr_init) != 0) {
        check_rest = true;
      }

      if (!check_rest) {
        if (m_partitions.find(arr_init) != m_partitions.end()) {
          if (!m_partitions[arr_init].empty() != 0u) {
            for (size_t i = 0; i < arr_size.size(); i++) {
              if (m_partitions[arr_init].find(i) !=
                  m_partitions[arr_init].end()) {
                vec_factor.push_back(m_partitions[arr_init][i]);
              } else {
                vec_factor.push_back(1);
              }
            }
          } else {
            check_rest = true;
          }
        }
      }

      if (check_rest) {
        void *func_decl = m_ast->GetEnclosingNode("Function", arr_init);
        map<void *, bool> res;
        if (cache_trace_res.count(arr_init) == 0) {
          mMars_ir->trace_to_bus(m_ast, func_decl, arr_init, &res, bNode->ref);
          cache_trace_res[arr_init] = res;
        } else {
          res = cache_trace_res[arr_init];
        }
        for (auto r : res) {
          if (m_partitions.find(r.first) != m_partitions.end()) {
            int partial_dim = m_ast->get_dim_num_from_var(r.first) -
                              m_ast->get_dim_num_from_var(arr_init);

            for (size_t i = 0; i < arr_size.size(); i++) {
              if (m_partitions[r.first].find(i + partial_dim) !=
                  m_partitions[r.first].end()) {
                vec_factor.push_back(m_partitions[r.first][i + partial_dim]);
              } else {
                vec_factor.push_back(1);
              }
            }
            break;
          }
        }
      }
      if (vec_factor.empty()) {
        continue;
      }

      bool trans_tag = false;
      for (auto ref : pList.second) {
        CMarsAccess access1(ref, m_ast, bNode->ref);
        vector<CMarsExpression> indexes1 = access1.GetIndexes();
        for (size_t i = 0; i < indexes1.size(); i++) {
          if (vec_factor[i] == 1 || vec_factor[i] == 0) {
            continue;
          }
          map<void *, CMarsExpression> *m_coeff_1 = indexes1[i].get_coeff();
          map<void *, CMarsExpression>::iterator it;

          for (it = m_coeff_1->begin(); it != m_coeff_1->end(); it++) {
            bool execute = false;
            if (m_ast->IsForStatement(it->first) == 0) {
              execute = true;
            } else {
              if (m_transform_level == "low") {
                execute = false;
              } else if (m_transform_level == "medium") {
                execute = true;
              }
            }
            if (execute &&
                (isSgModOp(static_cast<SgNode *>(it->first)) == nullptr) &&
                ((it->second.GetConstant() % vec_factor[i]) != 0)) {
              trans_tag = true;
              break;
            }
          }
        }
        if (trans_tag) {
          break;
        }
      }
      if (trans_tag) {
        cout << "==> Array:" << m_ast->_up(arr_init) << endl;
        cout << "        Apply array index transform: level "
             << m_transform_level << endl;
        bool read_only = true;
        if (m_transform_level == "medium") {
          read_only = false;
        }

        int ret = index_switch_transform(m_ast, arr_init, trans_for, vec_factor,
                                         read_only);
        if (ret == 0) {
          cout << "        Tansform aborted\n";
        } else {
          cout << "        Tansform applied\n";
          Changed = true;
          transformed.insert(arr_init);
          bNode->set_full_access_table_v2(m_ast);
          break;
        }
      }
    }
  }
}

void MemoryPartition::insert_partition_intel() {
  partition_analysis();
  partition_merge();
  partition_evaluate_intel();
  partition_transform_intel();
}

// Yuxin: 20190605 explore the array implement design space for Intel flow
// evaluate according to partition patterns
void MemoryPartition::partition_evaluate_intel() {
  cout << "\n\n =====> Intel partition evaluation...\n";
  for (auto it : m_partitions) {
    void *var = it.first;
    if (m_registers.count(var) > 0) {
      cout << "==> variable " << m_ast->_up(var)
           << " is implemented in registers\n";
      continue;
    }

    cout << "++ variable attributes: " << m_ast->_up(var) << endl;
    int type_bit =
        m_ast->get_bitwidth_from_type(cache_var_base_type[var], false);
    map<int, int> result_factor = it.second;
    vector<int> local_factor;
    map<string, string> local_attribute;
    vector<size_t> local_size = cache_var_size[var];
    int local_dim = cache_var_dim[var];
    int consecutive_dim = local_dim - 1;
    int reshape_dim = local_dim - 1;

    for (int i = 0; i < local_dim; i++) {
      if (result_factor.count(i) > 0) {
        local_factor.push_back(result_factor[i]);
      } else {
        local_factor.push_back(1);
      }
      cout << "dim: " << i << ",factor:" << local_factor[i] << endl;
    }

    // Step 1: check consecutive partitioning
    for (int i = local_dim - 1; i >= 0; i--) {
      if (local_factor[i] == 0) {
        consecutive_dim--;
      } else {
        break;
      }
    }

    // Step 2: set the attribute bankwidth
    {
      size_t t_bankwidth = type_bit;
      bool exceed_threshold = false;
      for (int i = local_dim - 1; i >= 0; i--) {
        size_t curr_bankwidth = 1;
        if (local_factor[i] == 0) {
          curr_bankwidth = local_size[i];
        } else {
          curr_bankwidth = local_factor[i];
        }
        if (t_bankwidth * curr_bankwidth > m_reshape_threshold) {
          reshape_dim = i + 1;
          exceed_threshold = true;
          reportSuboptimalMemIntel(var);
          break;
        }
        t_bankwidth *= curr_bankwidth;
        if (consecutive_dim == i) {
          reshape_dim = i;
          break;
        }
      }
      if (exceed_threshold) {
        reportSuboptimalMemIntel(var);
      } else {
        if (!isPo2(t_bankwidth)) {
          reportSuboptimalMemIntel(var);
          cout << "     Third party tool decision!!\n";
          continue;
        }
        // The bankwidth attribute is in byte
        local_attribute["bankwidth"] = my_itoa(t_bankwidth / 8);
      }
    }

    // Step 3: set the attribute bank_bits
    {
      // Count the rest partition factors
      int count = 0;
      int partition_dim;
      for (int i = reshape_dim - 1; i >= 0; i--) {
        if (local_factor[i] != 1) {
          count++;
          partition_dim = i;
        }
      }
      if (count == 0) {
        cout << "No other partitions\n";
      } else if (count == 1) {
        int t_dim = partition_dim;
        int t_bank = local_factor[t_dim];
        if (t_bank == 0) {
          t_bank = local_size[t_dim];
        }
        while (!isPo2(t_bank)) {
          t_bank++;
        }
        size_t low_bits = 1;
        for (int i = t_dim + 1; i < local_dim; i++) {
          low_bits *= local_size[i];
        }
        if (!isPo2(low_bits)) {
          reportSuboptimalMemIntel(var);
        } else {
          int base_bits = 0;
          while (low_bits > 1) {
            low_bits = low_bits >> 1;
            base_bits++;
          }
          int base_step = 0;
          while (t_bank > 1) {
            t_bank = t_bank >> 1;
            base_step++;
          }
          vector<string> bits;
          string bits_str;
          for (int i = 0; i < base_step; i++) {
            bits.push_back(my_itoa(base_bits));
            base_bits++;
          }
          for (int i = base_step - 1; i >= 0; i--) {
            bits_str += bits[i];
            if (i != 0) {
              bits_str += ",";
            }
          }
          local_attribute["bank_bits"] = bits_str;
        }
      } else {
        reportSuboptimalMemIntel(var);
      }
    }
    m_attributes[var] = local_attribute;
    for (auto att : local_attribute) {
      cout << "     " << att.first << " : " << att.second << endl;
    }
  }
}

void MemoryPartition::partition_transform_intel() {
  cout << "\n\n ######Partition transform...\n";
  for (auto it : m_attributes) {
    void *var = it.first;
    if (m_ast->IsArgumentInitName(var) != 0) {
      continue;
    }
    map<string, string> local_attribute = it.second;
    if (!local_attribute.empty()) {
      string pragma_str = "ACCEL attribute variable=" + m_ast->_up(var);
      void *var_decl = m_ast->GetVariableDecl(var);
      string att_str;
      for (auto pair : local_attribute) {
        string name_str = pair.first;
        string val_str = pair.second;
        if (name_str == "singlepump") {
          att_str += " " + name_str;
        } else {
          att_str += " " + name_str + "=" + val_str;
        }
      }
      pragma_str += att_str;
      void *pragma =
          m_ast->CreatePragma(pragma_str, m_ast->GetParent(var_decl));
      m_ast->InsertStmt(pragma, var_decl);
    }
  }
}

int choose_factor(std::vector<size_t> arr_size,
                  const std::vector<std::map<int, int>> &o_factors,
                  std::map<int, int> *m_factors,
                  std::map<int, int> *prior_factors) {
  cout << "Choose factors\n";
  int opt = 1;
  // Yuxin: 07/15/2019
  // Deal with dim=0 array_partition pragma
  for (auto candidate : o_factors) {
    if (candidate.count(-1) > 0) {
      cout << "Fully partition the array into registers\n";
      cout << "dim-" << -1 << ", factor-" << candidate[-1] << endl;
      (*m_factors)[-1] = 0;
      return opt;
    }
  }
  int dim_size = arr_size.size();
  for (int i = 0; i < dim_size; i++) {
    if (prior_factors->find(i) != prior_factors->end()) {
      (*m_factors)[i] = (*prior_factors)[i];
      cout << "input'' dim-" << i << ", size-" << arr_size[i] << ", choose-"
           << (*prior_factors)[i] << endl;
      continue;
    }

    vector<int> factors;
    for (auto candidate : o_factors) {
      if (candidate.count(i) <= 0) {
        continue;
      }
      cout << "dim-" << i << ", factor-" << candidate[i] << endl;
      factors.push_back(candidate[i]);
    }
    if (factors.empty()) {
      continue;
    }
    // Yuxin: Jun.13.2019
    // Merge the partition factors, choose the biggest one
    // For Intel flow, we will report warning if the chosen factor cannot be
    // divded by any other factors
    int a = factors[0];
    if (a > 0) {
      for (size_t ii = 1; ii < factors.size(); ii++) {
        if (factors[ii] == 0) {
          a = 0;
          break;
        }
        if (factors[ii] > a) {
          if (factors[ii] % a != 0) {
            opt = 0;
          }
          a = factors[ii];
        } else if (factors[ii] < a) {
          if (a % factors[ii] != 0) {
            opt = 0;
          }
        }
      }
    }

    if (arr_size[i] > 0 && arr_size[i] < XilinxThresh && a > arr_size[i] / 2) {
      a = 0;
    }

    //  FIXME: set the max factor as the partition factor (or
    //  complete unroll)
    (*m_factors)[i] = a;
    cout << "dim-" << i << ", size-" << arr_size[i] << ", choose-" << a << endl;
  }
  return opt;
}

void copy_factors(const vector<map<int, int>> &original,
                  vector<map<int, int>> *target,
                  const vector<size_t> &arr_size_org,
                  const vector<size_t> &arr_size_target, int partial_dim) {
  for (auto it : original) {
    map<int, int> new_temp;
    for (auto itt : it) {
      int factor = itt.second;
      if (arr_size_org.size() && arr_size_target.size()) {
        int o_size = arr_size_org[itt.first];
        int t_size = arr_size_target[itt.first + partial_dim];
        if (o_size < t_size && itt.second == 0) {
          factor = o_size;
        }
      }
      new_temp[itt.first + partial_dim] = factor;
    }
    target->push_back(new_temp);
  }
}

void copy_factors(const map<int, int> &original, map<int, int> *target) {
  for (auto itt : original) {
    (*target)[itt.first] = itt.second;
  }
}

bool check_action(int f1, int f2) {
  bool action = false;
  cout << "check  (" << f1 << " , " << f2 << ")" << endl;
  if (f1 == f2) {
    action = true;
  } else if (f2 == 0 || f1 == 0) {
    action = true;
  } else if (f1 < f2 && ((f2 % f1) == 0)) {
    action = true;
  } else if (f1 < f2 && isPo2(f2)) {
    action = true;
  } else {
    cout << "factor doesnt match " << endl;
  }

  if (!action) {
    cout << "==> Action FALSE!!! \n";
  }
  return action;
}

void MemoryPartition::reportSuboptimalFGPIP(CMirNode *node, void *arr) {
  void *sg_loop = m_ast->GetParent(node->ref);
  string str_label = m_ast->get_internal_loop_label(sg_loop);
  void *iter = m_ast->GetLoopIterator(sg_loop);
  if (iter) {
    str_label = m_ast->UnparseToString(iter);
  }
  string sFileName = getUserCodeFileLocation(m_ast, sg_loop);
  string array_info = "'" + m_ast->UnparseToString(arr) + "'";
  string loop_info = "\'" + str_label + "\' " + " (" + sFileName + ")";
  dump_critical_message(FGPIP_WARNING_13(loop_info, array_info), 0, m_ast,
                        sg_loop);
}

void MemoryPartition::reportSuboptimalMemIntel(void *arr_init) {
  string var_str = m_ast->_up(arr_init);
  string var_info =
      "\'" + var_str + "\' " + getUserCodeFileLocation(m_ast, arr_init, true);
  dump_critical_message(FGPIP_WARNING_20(var_info));
}

void MemoryPartition::print_partition(
    const map<void *, map<int, int>> &partitions) {
  cout << "    ==Print Partition==\n";
  for (auto it : partitions) {
    cout << "Array: " << m_ast->_up(it.first);
    for (auto itt : it.second) {
      cout << " dim : " << itt.first << " factor : " << itt.second << endl;
    }
  }
}

bool factor_extract(CSageCodeGen *codegen, CMarsIr *mars_ir, void *sg_pragma,
                    map<int, int> *hls_factors, void *arr_init) {
  string dim_str;
  bool errorOut = false;
  CAnalPragma ana_pragma(codegen);
  if (ana_pragma.PragmasFrontendProcessing(sg_pragma, &errorOut, false, true)) {
    dim_str = ana_pragma.get_attribute(CMOST_dim);
  }
  string pragma_str = codegen->GetPragmaString(sg_pragma);
  int var_dim = codegen->get_dim_num_from_var(arr_init);
  int hls_d;
  if (dim_str.empty()) {
    if (var_dim == 1)
      // If array is one dimensional, even if without dim info in the pragma,
      // we suppose the pragma is valid, and the partition dim is 1
      hls_d = 1;
    else
      hls_d = INVALID_DIM;
  } else {
    hls_d = std::atoi(dim_str.c_str());
  }

  // Yuxin: Jun 2 2018, user inser invalid pragma
  // Yuxin: 07/15/2019, dim=0 is valid in HLS array_parition pragma
  // It means fully partition the whole array into registers
  if (hls_d == INVALID_DIM) {
    cout << "Existing pragma: illegal dimension information\n";
    dump_critical_message(FGPIP_WARNING_18(pragma_str, dim_str));
    return true;
  }
  int tmp_dim = hls_d - 1;

  bool exsit = false;
  if (hls_factors->find(tmp_dim) != hls_factors->end()) {
    exsit = true;
  }
  if (mars_ir->getCompleteString(
          static_cast<SgPragmaDeclaration *>(sg_pragma))) {
    if (exsit) {
      if ((*hls_factors)[tmp_dim] != 0) {
        cout << "Multiple pragmas on same arrays on same dimension existed.\n";
        return false;
      }
    } else {
      // User input pragma dim=0
      if (hls_d == 0) {
        for (size_t i = 0; i < var_dim; i++)
          (*hls_factors)[i] = 0;
      } else {
        (*hls_factors)[tmp_dim] = 0;
      }
    }
  } else {
    string factor_str =
        mars_ir->getFactorString(static_cast<SgPragmaDeclaration *>(sg_pragma));
    if (factor_str.empty()) {
      cout << "Either no complete or factor argument, reconganized as "
              "complete "
              "partition\n";
      if (exsit) {
        if ((*hls_factors)[tmp_dim] != 0) {
          cout << "Multiple pragmas on same arrays on same dimension "
                  "existed.\n";
          return false;
        }
      } else {
        if (hls_d == 0) {
          for (size_t i = 0; i < var_dim; i++)
            (*hls_factors)[i] = 0;
        } else {
          (*hls_factors)[tmp_dim] = 0;
        }
      }
      return true;
    }
    int factor = std::atoi(factor_str.c_str());
    if (factor == 0) {
      dump_critical_message(FGPIP_WARNING_21(pragma_str, factor_str));
      return false;
    }
    if (exsit) {
      if ((*hls_factors)[tmp_dim] != factor) {
        cout << "Multiple pragmas on same arrays on same dimension existed.\n";
        return false;
      }
    } else {
      if (hls_d == 0) {
        for (size_t i = 0; i < var_dim; i++)
          (*hls_factors)[i] = factor;
      } else {
        (*hls_factors)[tmp_dim] = factor;
      }
    }
  }
  return true;
}

void enumerate_point_space(const vector<int> &dim_size,
                           vector<vector<int>> *point_set) {
  vector<vector<int>> ret;
  ret.clear();
  if (dim_size.empty()) {
    // return empty set
  } else if (dim_size.size() == 1) {
    for (int i = 0; i < dim_size[0]; i++) {
      vector<int> point_1d;
      point_1d.push_back(i);
      ret.push_back(point_1d);
    }
  } else {
    int dim_minus_1 = static_cast<int>(dim_size.size() - 1);
    vector<int> dim_size_minus_1;
    for (int i = 0; i < dim_minus_1; i++) {
      dim_size_minus_1.push_back(dim_size[i]);
    }
    vector<vector<int>> point_set_minus_1;
    enumerate_point_space(dim_size_minus_1, &point_set_minus_1);

    for (size_t i = 0; i < point_set_minus_1.size(); i++) {
      vector<int> curr_point = point_set_minus_1[i];
      for (int j = 0; j < dim_size[dim_minus_1]; j++) {
        vector<int> one_new_point = curr_point;
        one_new_point.push_back(j);
        ret.push_back(one_new_point);
      }
    }
  }

  *point_set = ret;
}

//
// Notes:
// 1. Only handle full dimension read accesses (ignore other access)
// 2. Use new variable xxx_r to replace original references
// 3. The access index must be continuous
// 4. Only handle the innermost loop ( assume single scope )
int index_switch_transform(CSageCodeGen *codegen, void *input_array,
                           void *input_loop, vector<int> switch_factor_in,
                           bool read_only) {
  // 1. Find the feasible accesses
  // 2. Check and assign the offsets
  // 3. Generate code

  // 0. basic information
  vector<int> switch_factor;
  for (int j = 0; j < static_cast<int>(switch_factor_in.size()); j++) {
    if (switch_factor_in[j] == 0) {
      switch_factor.push_back(1);
    } else {
      switch_factor.push_back(switch_factor_in[j]);
    }
  }

  int input_dim = 0;
  void *input_base_type;
  {
    void *sg_type = codegen->GetTypebyVar(input_array);
    vector<size_t> vec_sizes;
    input_dim = codegen->get_type_dimension(sg_type, &input_base_type,
                                            &vec_sizes, input_loop);
    if (codegen->IsModifierType(input_base_type) != 0) {
      input_base_type = codegen->GetElementType(input_base_type);
    }
  }

  // 1. Find the feasible accesses
  //    a) read access
  //    b) full dimension
  vector<int> global_is_read;
  vector<CMarsAccess> org_access;
  vector<CMarsAccess> org_global_access;
  vector<void *> org_pntr;
  vector<void *> org_global_pntr;
  vector<CMarsAccess> org_write_access;
  vector<void *> org_write_pntr;
  {
    vector<void *> vecNodes;
    codegen->GetNodesByType_int(input_loop, "preorder", V_SgVarRefExp,
                                &vecNodes);

    for (size_t i = 0; i < vecNodes.size(); i++) {
      void *array_ref = vecNodes[i];

      if (codegen->GetVariableInitializedName(array_ref) != input_array) {
        continue;
      }

      void *sg_pntr;
      int pointer_dim = 0;
      {
        void *sg_array;
        vector<void *> sg_indexes;
        codegen->parse_pntr_ref_by_array_ref(array_ref, &sg_array, &sg_pntr,
                                             &sg_indexes, &pointer_dim);
        assert(sg_array == input_array);
      }

      bool has_write = false;
      if (codegen->has_write_conservative(sg_pntr) != 0) {
        has_write = true;
      }

      if (pointer_dim == input_dim) {
        CMarsAccess ma(sg_pntr, codegen, nullptr);
        if ((codegen->has_read_conservative(sg_pntr) != 0) && !has_write) {
          org_access.push_back(ma);
          org_global_access.push_back(ma);
          global_is_read.push_back(1);
          org_pntr.push_back(sg_pntr);
          org_global_pntr.push_back(sg_pntr);
        } else if (!read_only && has_write) {
          org_write_access.push_back(ma);
          org_global_access.push_back(ma);
          global_is_read.push_back(0);
          org_write_pntr.push_back(sg_pntr);
          org_global_pntr.push_back(sg_pntr);
        }
      }
    }
  }

  if (org_global_access.size() < 2) {
    return 0;
  }
  CMarsAccess base_access = org_global_access[0];

  // 2. Check and assign the offsets
  vector<vector<int>> vec_offsets;        // [access][dim]-> offset
  vector<vector<int>> vec_write_offsets;  // [access][dim]-> offset
  {
    for (size_t i = 0; i < org_global_access.size(); i++) {
      vector<int> one_offset;
      one_offset.resize(input_dim);
      CMarsAccess one_access = org_global_access[i];

      for (int j = 0; j < input_dim; j++) {
        int n_switch_factor = switch_factor[j];
        if (n_switch_factor == 0) {
          n_switch_factor = 1;
        }

        if (n_switch_factor > 1) {
          CMarsExpression offset =
              one_access.GetIndex(j) - base_access.GetIndex(j);

          if (offset.IsConstant() == 0) {
            return 0;
          }

          one_offset[j] = offset.GetConstant();
        } else {
          one_offset[j] = 0;
        }
      }
      if (global_is_read[i] != 0) {
        vec_offsets.push_back(one_offset);
      } else {
        vec_write_offsets.push_back(one_offset);
      }
    }
  }

  // 3. Generate code
  bool local_changed = false;
  {
    void *curr_scope = codegen->GetLoopBody(input_loop);
    void *curr_pos = nullptr;
    if (read_only) {
      curr_pos = codegen->TraceUpToStatement(org_pntr[0]);
    } else {
      curr_pos = codegen->TraceUpToStatement(org_global_pntr[0]);
    }

    void *curr_bb = codegen->CreateBasicBlock();
    codegen->InsertStmt(curr_bb, curr_pos);
    string s_prefix = "_" + codegen->UnparseToString(input_array);
    string var_name;
    void *decl;
    void *initer;

    // 3.1 create the declaration of the index variables
    vector<void *> addr_decl;
    vector<void *> bank_decl;
    {
      for (int i = 0; i < input_dim; i++) {
        int n_switch_factor = switch_factor[i];
        if (n_switch_factor == 0) {
          n_switch_factor = 1;
        }

        void *base_idx = base_access.GetIndex(i).get_expr_from_coeff();

        if (n_switch_factor > 1) {
          var_name = s_prefix + "_addr" + my_itoa(i);
          initer = codegen->CreateExp(V_SgDivideOp, base_idx,
                                      codegen->CreateConst(n_switch_factor));
          decl = codegen->CreateVariableDecl(codegen->GetTypeByString("int"),
                                             var_name, initer, curr_bb);
          codegen->AppendChild(curr_bb, decl);
        } else {
          decl = nullptr;
        }
        addr_decl.push_back(decl);

        if (n_switch_factor > 1) {
          var_name = s_prefix + "_bank" + my_itoa(i);
          initer = codegen->CreateExp(V_SgModOp, codegen->CopyExp(base_idx),
                                      codegen->CreateConst(n_switch_factor));
          decl = codegen->CreateVariableDecl(codegen->GetTypeByString("int"),
                                             var_name, initer, curr_bb);
          codegen->AppendChild(curr_bb, decl);
        } else {
          decl = nullptr;
        }
        bank_decl.push_back(decl);
      }
    }
    // 3.2 create the declaration of the access variables
    vector<void *> ref_decl;
    {
      int num_ref = org_access.size();
      ref_decl.resize(num_ref);
      for (size_t i = 0; i < static_cast<int>(org_access.size()); i++) {
        int ii = static_cast<int>(num_ref - 1 - i);
        var_name = s_prefix + "_r" + my_itoa(ii);
        decl = codegen->CreateVariableDecl(input_base_type, var_name, nullptr,
                                           curr_scope);
        codegen->PrependChild(curr_scope, decl, true);
        ref_decl[ii] = decl;
      }
    }

    // 3.3 create the assignment of the access variables
    vector<void *> vec_if_cond;
    vector<void *> vec_if_body;
    vector<vector<int>> bank_cond_combination;  // [cond_idx][dim] = #bank
    {
      // 3.3.1 enumerate conditions
      enumerate_point_space(switch_factor, &bank_cond_combination);

      // 3.3.2 Get If-statement conditions
      for (size_t i = 0; i < bank_cond_combination.size(); i++) {
        // Condition:
        // if (_bank0 == 0 && _bank1 == 1 && ...
        vector<int> one_case = bank_cond_combination[i];
        assert(one_case.size() == (size_t)input_dim);
        // cout << one_case[0] << " " << one_case[1] << " " << one_case[2] <<
        // endl;

        void *curr_cond = nullptr;
        for (int j = 0; j < input_dim; j++) {
          int n_switch_factor = switch_factor[j];
          if (n_switch_factor == 0) {
            n_switch_factor = 1;
          }

          if (n_switch_factor > 1) {
            void *one_cond = codegen->CreateExp(
                V_SgEqualityOp, codegen->CreateVariableRef(bank_decl[j]),
                codegen->CreateConst(one_case[j]));
            if (curr_cond == nullptr) {
              curr_cond = one_cond;
            } else {
              curr_cond = codegen->CreateExp(V_SgAndOp, curr_cond, one_cond);
            }
          }
        }
        vec_if_cond.push_back(curr_cond);
      }

      // 3.3.3 Get if-statement bodies
      for (size_t i = 0; i < bank_cond_combination.size(); i++) {
        // Body Assignement
        // _a_ref0 = a[factor*addr0 + bank0][...];
        // _a_ref1 = a[factor*addr0 + bank0+1][...];
        vector<int> one_case = bank_cond_combination[i];
        assert(one_case.size() == (size_t)input_dim);

        void *curr_if_body = codegen->CreateBasicBlock();

        // vec_offset [access][dim] -> offset
        for (size_t j = 0; j < vec_offsets.size(); j++) {
          vector<int> offset_of_one_ref = vec_offsets[j];
          assert(offset_of_one_ref.size() == (size_t)input_dim);

          vector<void *> new_ref_index;
          for (int k = 0; k < input_dim; k++) {
            int n_switch_factor = switch_factor[k];
            if (n_switch_factor == 0) {
              n_switch_factor = 1;
            }

            void *new_index;
            if (n_switch_factor > 1) {
              new_index = codegen->CreateExp(
                  V_SgAddOp,
                  codegen->CreateExp(V_SgMultiplyOp,
                                     codegen->CreateVariableRef(addr_decl[k]),
                                     codegen->CreateConst(n_switch_factor)),
                  codegen->CreateConst(offset_of_one_ref[k] + one_case[k]));
            } else {
              new_index = codegen->CopyExp(
                  org_access[j].GetIndex(k).get_expr_from_coeff());
            }
            new_ref_index.push_back(new_index);
          }
          void *one_assign = codegen->CreateExp(
              V_SgAssignOp, codegen->CreateVariableRef(ref_decl[j]),
              codegen->CreateArrayRef(codegen->CreateVariableRef(input_array),
                                      new_ref_index));
          void *stmt = codegen->CreateStmt(V_SgExprStatement, one_assign);
          codegen->AppendChild(curr_if_body, stmt);
          local_changed |= true;
        }

        vec_if_body.push_back(curr_if_body);
      }

      // 3.3.4 Build if-statement
      {
        assert(vec_if_cond.size() == vec_if_body.size());

        if (!vec_if_cond.empty()) {
          void *curr_else_stmt = vec_if_body[vec_if_cond.size() - 1];
          for (size_t i = 1; i < static_cast<int>(vec_if_cond.size()); i++) {
            int ii = static_cast<int>(vec_if_cond.size() - 1 - i);
            void *one_cond = vec_if_cond[ii];
            void *one_body = vec_if_body[ii];
            void *curr_if_stmt =
                codegen->CreateIfStmt(one_cond, one_body, curr_else_stmt);
            curr_else_stmt = curr_if_stmt;
          }
          codegen->AppendChild(curr_bb, curr_else_stmt);
        }
      }
    }

    // 3.4 Replace original references
    for (size_t i = 0; i < org_pntr.size(); i++) {
      void *org_ref = org_pntr[i];
      void *new_ref = codegen->CreateVariableRef(ref_decl[i]);
      codegen->ReplaceExp(org_ref, new_ref);
    }

    // 3.5 Deal with write references
    if (!read_only) {
      // create the declaration of the access variables
      vector<void *> write_ref_decl;
      {
        int num_ref = org_write_access.size();
        write_ref_decl.resize(num_ref);
        for (size_t i = 0; i < static_cast<int>(org_write_access.size()); i++) {
          int ii = static_cast<int>(num_ref - 1 - i);
          var_name = s_prefix + "_w" + my_itoa(ii);
          decl = codegen->CreateVariableDecl(input_base_type, var_name, nullptr,
                                             curr_scope);
          codegen->PrependChild(curr_scope, decl, true);
          write_ref_decl[ii] = decl;
        }
      }
      vector<void *> write_pos_stmts;
      vector<int> write_changed;
      for (size_t ii = 0; ii < org_write_pntr.size(); ii++) {
        bool changed = false;
        void *write_ref = org_write_pntr[ii];
        void *write_pos = codegen->TraceUpToStatement(org_write_pntr[ii]);
        write_pos_stmts.push_back(write_pos);
        void *sg_assign = codegen->GetParent(write_ref);

        if (sg_assign != nullptr) {
          void *right = codegen->GetExpRightOperand(sg_assign);

          if (codegen->is_movable_test(right, curr_pos, write_pos) != 0) {
            void *new_write_ref =
                codegen->CreateVariableRef(write_ref_decl[ii]);
            codegen->ReplaceExp(write_ref, new_write_ref);

            for (size_t i = 0; i < bank_cond_combination.size(); i++) {
              // Body Assignement
              // a[factor*addr0 + bank0][...] = ...;
              vector<int> one_case = bank_cond_combination[i];
              assert(one_case.size() == (size_t)input_dim);

              void *curr_if_body = vec_if_body[i];
              void *write_stmt = codegen->CopyStmt(write_pos);
              codegen->AppendChild(curr_if_body, write_stmt);
              local_changed |= true;
              changed |= true;

              // vec_offset [access][dim] -> offset
              //             for (size_t j = 0; j < vec_write_offsets.size();
              //             j++) {
              //  vector<int> offset_of_one_ref = vec_write_offsets[j];
              vector<int> offset_of_one_ref = vec_write_offsets[ii];
              assert(offset_of_one_ref.size() == (size_t)input_dim);

              vector<void *> new_ref_index;
              for (int k = 0; k < input_dim; k++) {
                int n_switch_factor = switch_factor[k];
                if (n_switch_factor == 0) {
                  n_switch_factor = 1;
                }

                void *new_index;
                if (n_switch_factor > 1) {
                  new_index = codegen->CreateExp(
                      V_SgAddOp,
                      codegen->CreateExp(
                          V_SgMultiplyOp,
                          codegen->CreateVariableRef(addr_decl[k]),
                          codegen->CreateConst(n_switch_factor)),
                      codegen->CreateConst(offset_of_one_ref[k] + one_case[k]));
                } else {
                  new_index = codegen->CopyExp(
                      org_write_access[ii].GetIndex(k).get_expr_from_coeff());
                }
                new_ref_index.push_back(new_index);
              }
              void *one_assign = codegen->CreateExp(
                  V_SgAssignOp,
                  codegen->CreateArrayRef(
                      codegen->CreateVariableRef(input_array), new_ref_index),
                  codegen->CreateVariableRef(write_ref_decl[ii]));
              void *stmt = codegen->CreateStmt(V_SgExprStatement, one_assign);
              codegen->AppendChild(curr_if_body, stmt);
              //             }
            }
          }
        }
        write_changed.push_back(
            static_cast<
                std::vector<int, class std::allocator<int>>::value_type>(
                changed));
      }

      for (size_t ii = 0; ii < write_pos_stmts.size(); ii++) {
        if (write_changed[ii] != 0) {
          codegen->RemoveStmt(write_pos_stmts[ii]);
        }
      }
    }
    if (!local_changed) {
      codegen->RemoveStmt(curr_bb);
    }
  }
  return 1;
}

void MemoryPartition::check_hls_resource() {
  cout << "CHECK HLS resource pragma\n";
  vector<void *> vec_pragmas;
  m_ast->GetNodesByType_int(mTopFunc, "preorder", V_SgPragmaDeclaration,
                            &vec_pragmas);
  for (auto pragma : vec_pragmas) {
    string str_pragma = m_ast->GetPragmaString(pragma);
    bool errorOut = false;
    CAnalPragma ana_pragma(m_ast);
    if (ana_pragma.PragmasFrontendProcessing(pragma, &errorOut, false, true)) {
      if (ana_pragma.get_vendor_type() != "HLS")
        continue;
      if (ana_pragma.get_pragma_type() != HLS_RESOURCE)
        continue;
      // Yuxin: Sep/18/2020, keep FIFO resource pragmas
      string s_core = ana_pragma.get_attribute(HLS_CORE);
      if (s_core.find("FIFO") == 0)
        continue;
      vector<void *> find_vars;
      string s_var = ana_pragma.get_attribute(CMOST_variable);
      auto var_init = m_ast->getInitializedNameFromName(pragma, s_var, true);
      if (var_init != nullptr) {
        find_vars.push_back(var_init);
      } else {
        vector<string> var_tokens;
        m_ast->GetTokens(&var_tokens, s_var);
        if (var_tokens.size() == 1) {
          // Didnt find the variable
          continue;
        } else {
          if (s_var.find(",") != string::npos) {
            for (auto v_token : var_tokens) {
              if (v_token == ",")
                continue;
              SgInitializedName *var_init =
                  m_ast->getInitializedNameFromName(pragma, v_token, true);
              if (var_init != nullptr) {
                find_vars.push_back(var_init);
              }
            }
          }
        }
      }
      if (find_vars.size() > 0) {
        for (auto var_init : find_vars) {
          void *sg_type = m_ast->GetTypebyVar(var_init);
          void *base_type = m_ast->GetBaseType(sg_type, true);
          if (m_ast->IsXilinxStreamType(base_type))
            continue;
          hls_resource_pragmas[var_init].push_back(pragma);
          if (hls_resource_table.count(var_init) > 0 &&
              hls_resource_table[var_init] != s_core) {
            continue;
          }
          hls_resource_table[var_init] = s_core;
#if PROJDEBUG
          cout << "found arr: " << m_ast->_up(var_init) << endl;
          cout << "   Add HLS resource pragma: " << str_pragma << endl;
#endif
        }
      }
    }
  }
}

void MemoryPartition::check_resource_port_setting(void *var_init,
                                                  bool *true_dual_port) {
  if (hls_resource_table.count(var_init) > 0) {
    string s_core = hls_resource_table[var_init];
    if (s_core.find("RAM_2P") == 0 || s_core.find("RAM_S2P") == 0 ||
        s_core.find("RAM_1P") == 0 || s_core.find("ROM_1P") == 0)
      *true_dual_port = false;
  }
}

static bool large_compare(const std::pair<int, int> &one,
                          const std::pair<int, int> &other) {
  return one.first > other.first;
}

void MemoryPartition::AnalyzeAccess(CMirNode *bNode, AccessInfo *access_info,
                                    vector<pair<int, int>> *priority_sorting) {
  auto *array = access_info->array;
  cout << "\n ==> choose dimension priority for variable: ";
  cout << m_ast->_p(array) << endl;

  auto vec_dim_size = cache_var_size[array];
  vector<void *> vec_pntrs;
  for (size_t dim = 0; dim < vec_dim_size.size(); dim++) {
    //  Yuxin: Jun 15, 2020
    //  Move the parsing access references here
    int mod_size = 1;
    map<void *, vector<map<void *, int>>> index_expr_full;
    int dim_priority = 100;
    ParseFullAccess(bNode, dim, array, &index_expr_full, &mod_size,
                    &dim_priority, &vec_pntrs);
    access_info->mod_vals.push_back(mod_size);
    if (!index_expr_full.empty()) {
      cout << "\n ==> parse access in dimension " << dim << endl;
      cout << "priority: " << dim_priority << endl;
      access_info->index_expr_array[dim] = index_expr_full;
      priority_sorting->emplace_back(dim_priority, dim);
    }
  }
  std::sort(priority_sorting->begin(), priority_sorting->end(), large_compare);

  //  Yuxin: Jul 10 2018
  //  The array has self assign references
  //  Jul 8 2020
  //  Add read/write analysis on the same index
  if (has_self_assign_ref(vec_pntrs) != 0 &&
      loop_partition_builder->GetLoopOpt(bNode) != -2) {
    access_info->has_self_assign = true;
    access_info->dual_option = false;
  }
}

bool MemoryPartition::ParseFullAccess(
    CMirNode *bNode, int dim, void *arr_init,
    map<void *, vector<map<void *, int>>> *index_expr_full, int *mod_size,
    int *dim_priority, vector<void *> *vec_pntrs) {
  int opt = loop_partition_builder->GetLoopOpt(bNode);
  auto vec_refs = bNode->full_access_table_v2[arr_init];

  auto dim_size = cache_var_size[arr_init][dim];
  if (dim_size > 0 && vec_refs.size() >= dim_size / 2)
    (*dim_priority)++;

  for (auto sg_ref : vec_refs) {
    void *func_decl = m_ast->TraceUpToFuncDecl(sg_ref);
    void *ref_loop = m_ast->TraceUpToLoopScope(sg_ref);
    void *node_loop = m_ast->TraceUpToLoopScope(bNode->ref);
    // Direct reference
    if (opt == -2) {
      if (bNode->is_function) {
        if (ref_loop)
          continue;
      } else {
        if (ref_loop != node_loop)
          continue;
      }
    }
    if (func_decl) {
      auto func_name = m_ast->GetFuncName(func_decl, false);
      if (func_name.find("memcpy_wide_bus_read") == 0 ||
          func_name.find("memcpy_wide_bus_write") == 0) {
        // Youxiang: skip wide bus analysis because we have inferred
        // partition pragma
        continue;
      }
    }
    if (m_ast->IsMemberVariable(arr_init)) {
      member_accesses[arr_init].push_back(sg_ref);
    }
    void *sg_array = nullptr;
    void *sg_pntr;
    vector<void *> sg_indexes;
    int pointer_dim;
    m_ast->parse_pntr_ref_by_array_ref(sg_ref, &sg_array, &sg_pntr, &sg_indexes,
                                       &pointer_dim, nullptr, true, -1, 1);
    int org_dim = m_ast->get_dim_num_from_var(arr_init);
    if (dim == 0) {
      if (sg_pntr != nullptr)
        vec_pntrs->push_back(sg_pntr);
      else
        vec_pntrs->push_back(sg_ref);
    }
#ifdef _DEBUG_PARTITION_
    cout << "==>access: " << m_ast->_p(sg_pntr) << endl;
    for (size_t j = 0; j != sg_indexes.size(); ++j)
      cout << m_ast->_p(sg_indexes[j]) << ",";
    cout << "pointer_dim: " << pointer_dim << endl;
    cout << "org_dim: " << org_dim << endl;
#endif
    if (m_ast->IsVarRefExp(sg_pntr) != 0) {
      continue;
    }

    void *sg_idx = nullptr;
    vector<map<void *, int>> idx_vec;
    //  If it is the direct access to the array
    if (sg_array == arr_init) {
      if (pointer_dim == org_dim) {
        sg_idx = sg_indexes[dim];
      } else if (pointer_dim < org_dim) {
        if (dim < pointer_dim) {
          sg_idx = sg_indexes[dim];
        } else if (dim == pointer_dim && (m_ast->IsAddressOfOp(sg_pntr) != 0)) {
          sg_idx = sg_indexes[dim];
        } else {
          continue;
        }
      }
    } else if (sg_array != arr_init) {
      int curr_dim = m_ast->get_dim_num_from_var(sg_array);
      int sub_dim = dim - (org_dim - curr_dim);
      if (sub_dim >= 0) {
        if (pointer_dim == curr_dim) {
          sg_idx = sg_indexes[sub_dim];
        } else if (pointer_dim < curr_dim) {
          if (sub_dim < pointer_dim) {
            sg_idx = sg_indexes[sub_dim];
          } else if (sub_dim == pointer_dim &&
                     (m_ast->IsAddressOfOp(sg_pntr) != 0)) {
            sg_idx = sg_indexes[sub_dim];
          } else {
            continue;
          }
        }
        //  Trace up using call path
        if (m_ast->IsVarRefExp(sg_idx) != 0) {
          index_trace_up(bNode->ref, &sg_idx);
        }
      } else {
        // Access in the lower dimensions
        continue;
      }
    }
    m_ast->remove_cast(&sg_idx);
#ifdef _DEBUG_PARTITION_
    cout << "[INFO] final index: " << m_ast->_p(sg_idx) << endl;
#endif

    CMarsExpression index1(sg_idx, m_ast, sg_pntr);
    map<void *, int> index_expr;
    map<void *, CMarsExpression> *m_coeff_1 = index1.get_coeff();
    map<void *, CMarsExpression>::iterator it;
    bool is_mod = false;
    bool is_var = false;

    for (it = m_coeff_1->begin(); it != m_coeff_1->end(); it++) {
      auto coeff_var = it->first;
      auto coeff_const = it->second;
      if (coeff_const.IsConstant() == 0) {
        return false;
      }
      if (m_ast->IsForStatement(coeff_var) != 0) {
        auto var = m_ast->GetLoopIterator(coeff_var);
        index_expr[var] = coeff_const.GetConstant();
        // If the related loop iterator will be fully unrolled
        // so that it will become constant in the index
        void *loop_body = m_ast->GetLoopBody(coeff_var);
        auto node = mMars_ir->get_node(loop_body);
        if (node) {
          if (node->has_parallel() && node->is_complete_unroll())
            (*dim_priority)++;
        }
        if (node_loop != nullptr && coeff_var == node_loop)
          (*dim_priority)++;
      } else {
        (*dim_priority)--;
        if (isSgModOp(static_cast<SgNode *>(coeff_var)) != nullptr) {
          void *left = m_ast->GetExpLeftOperand(coeff_var);
          void *right = m_ast->GetExpRightOperand(coeff_var);
          CMarsExpression idx_l(left, m_ast, sg_pntr);
          map<void *, CMarsExpression> *m_coeff_2 = idx_l.get_coeff();

          for (auto itt : *m_coeff_2) {
            if (m_ast->IsForStatement(itt.first) != 0) {
              auto var = m_ast->GetLoopIterator(itt.first);
              index_expr[var] = itt.second.GetConstant();
            } else {
              index_expr[itt.first] = itt.second.GetConstant();
            }
          }
          index_expr[CONST_ITER] = idx_l.GetConstant();
          CMarsExpression idx_r(right, m_ast, sg_pntr);
          if (idx_r.IsConstant() != 0) {
            *mod_size = idx_r.GetConstant();
          } else {
            return false;
          }
          is_mod = true;
        } else {
          if (m_ast->IsVarRefExp(coeff_var) != 0) {
            void *sg_iter = nullptr;
            trace_up_to_loop_iterator(coeff_var, &sg_iter);
            if (sg_iter != nullptr) {
              index_expr[sg_iter] = coeff_const.GetConstant();
              (*dim_priority)++;
              continue;
            }
          }
          if (coeff_const.IsConstant() == 0) {
            return false;
          }
          if (node_loop != nullptr &&
              m_ast->IsLoopInvariant(node_loop, coeff_var, sg_ref)) {
            (*dim_priority)++;
          }
          index_expr[coeff_var] = coeff_const.GetConstant();
          is_var = true;
        }
      }
    }
    if (!is_mod || is_var) {
      index_expr[CONST_ITER] = index1.GetConstant();
      (*dim_priority)++;
      if (index1.IsConstant())
        (*dim_priority)++;
    }
    if (index_expr.find(CONST_ITER) == index_expr.end()) {
      index_expr[CONST_ITER] = 0;
    }

    idx_vec.push_back(index_expr);
    (*index_expr_full)[sg_ref] = idx_vec;
  }
  return true;
}

//  Yuxin: Sep 3 2018
//  For both variable ref and pntr
//  Self assign refs includes:
//  1) a+=, a++
//  2) b=a; a=c;
//  3) a=a+b
int MemoryPartition::has_self_assign_ref(vector<void *> vec_pntrs) {
  for (auto sg_ref : vec_pntrs) {
    void *binary_op = m_ast->GetEnclosingNode("CompoundAssignOp", sg_ref);
    void *unary_op = m_ast->GetEnclosingNode("UnaryOp", sg_ref);
    void *call_op = m_ast->GetEnclosingNode("FunctionCallExp", sg_ref);
#ifdef _DEBUG_PARTITION_
    cout << "SELF check: " << m_ast->_p(m_ast->GetParent(sg_ref)) << endl;
#endif
    if (binary_op != nullptr) {
      void *left = m_ast->GetExpLeftOperand(binary_op);
      if (sg_ref == left) {
        return 1;
      }
    }
    if (unary_op != nullptr) {
      if ((m_ast->IsPlusPlusOp(unary_op) != 0) ||
          (m_ast->IsMinusMinusOp(unary_op) != 0)) {
        void *left = m_ast->GetExpUniOperand(unary_op);
        if (sg_ref == left) {
          return 1;
        }
      }
    }
    // Yuxin: Oct 10 2019
    // Check APint op
    if (call_op != nullptr) {
      void *left_exp;
      void *right_exp;
      int ap_op = 0;
      m_ast->IsAPIntOp(call_op, &left_exp, &right_exp, &ap_op);
      if (m_ast->IsCompoundAssignOp(ap_op) != 0) {
        if (sg_ref == left_exp) {
          return 1;
        }
      }
    }
  }

  if (vec_pntrs.size() < 2) {
    return 0;
  }
  // Check read/write
  for (size_t i = 0; i < vec_pntrs.size(); ++i) {
    for (size_t ii = i + 1; ii < vec_pntrs.size(); ++ii) {
      void *sg_pntr1 = vec_pntrs[i];
      void *sg_pntr2 = vec_pntrs[ii];
      void *sg_pos1 = m_ast->GetEnclosingNode("Statement", sg_pntr1);
      void *sg_pos2 = m_ast->GetEnclosingNode("Statement", sg_pntr2);

      //  Yuxin:Sep 3 Edit
      //  Includes a=a+b
      if (m_ast->is_identical_expr(sg_pntr1, sg_pntr2, sg_pos1, sg_pos2)) {
        if ((m_ast->has_write_conservative(sg_pntr1) != 0) ||
            (m_ast->has_write_conservative(sg_pntr2) != 0)) {
#ifdef _DEBUG_PARTITION_
          cout << "SELF assign on identical refs: " << m_ast->_p(sg_pntr1)
               << ", " << m_ast->_p(sg_pntr2) << endl;
#endif
          return 1;
        }
      }
    }
  }
  return 0;
}

void MemoryPartition::collect_local_variables(
    void *func, vector<void *> *vec_decls,
    unordered_set<void *> *visited_funcs) {
  if (visited_funcs->count(func) > 0) {
    return;
  }
  visited_funcs->insert(func);
  vector<void *> vec_vars;
  m_ast->GetNodesByType_int(func, "preorder", V_SgVariableDeclaration,
                            &vec_vars);
  vec_decls->insert(vec_decls->end(), vec_vars.begin(), vec_vars.end());
  vector<void *> vec_calls;
  m_ast->GetNodesByType_int(func, "preorder", V_SgFunctionCallExp, &vec_calls);
  for (auto call : vec_calls) {
    void *sub_func = m_ast->GetFuncDeclByCall(call);
    if (sub_func == nullptr)
      continue;
    collect_local_variables(sub_func, vec_decls, visited_funcs);
  }
  return;
}
