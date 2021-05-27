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


#pragma once
#include <map>
#include <set>
#include <vector>
#include <string>
#include <utility>
#include <list>
#include <unordered_set>
#include <unordered_map>

#include "rose.h"
#include "bsuNode.h"
#include "codegen.h"
#include "mars_ir.h"
#include "mars_opt.h"
#include "math_gcd.h"
#include "mars_message.h"
#include "program_analysis.h"
#include "stream_ir.h"
#include "id_update.h"

using std::cout;
using std::list;
using std::make_shared;
using std::map;
using std::pair;
using std::set;
using std::shared_ptr;
using std::string;
using std::unordered_map;
using std::unordered_set;
using std::vector;

//  Vivado_HLS partition factor threshold
#define XilinxThresh 1024
// #define _DEBUG_PARTITION_

struct AccessInfo {
  void *array;
  bool has_self_assign;
  bool dual_option;
  bool heuristic_on;
  bool access_is_exact;
  bool constant_index;
  vector<int> mod_vals;
  map<int, int> local_pt_factor;
  map<int, map<void *, vector<map<void *, int>>>> index_expr_array;
  unordered_map<int, map<int, int>> repeat_access_info;
  unordered_map<int, vector<map<void *, int>>> index_copy_info;
  explicit AccessInfo(void *array_) : array(array_) {
    access_is_exact = true;
    constant_index = true;
    has_self_assign = false;
    dual_option = true;
    heuristic_on = false;
  }
};

struct LoopInfo {
  CMirNode *loop_node;
  int opt;
  bool has_out_of_bound_access;
  map<void *, map<int, int>> partition_results;
  explicit LoopInfo(CMirNode *loop_) : loop_node(loop_) {
    opt = -1;
    has_out_of_bound_access = false;
  }
};

class LoopPartitionBuilder {
 public:
  explicit LoopPartitionBuilder(CSageCodeGen *codegen_) : m_ast(codegen_) {}
  int run();

  map<void *, map<int, int>> GetPartitions(CMirNode *bNode) {
    return loop_info_map[bNode]->partition_results;
  }
  void SetPartitions(CMirNode *bNode, void *array, map<int, int> local_factor) {
    loop_info_map[bNode]->partition_results[array] = local_factor;
  }
  int SetLoopOpt(CMirNode *loop_node);
  int GetLoopOpt(CMirNode *bNode) { return loop_info_map[bNode]->opt; }
  unordered_map<CMirNode *, LoopInfo *> loop_info_map;

 private:
  CSageCodeGen *m_ast;
};

class MemoryPartition {
 private:
  CSageCodeGen *m_ast;
  void *mTopFunc;
  CInputOptions mOptions;
  CMarsIr *mMars_ir;
  CMarsIrV2 *mMars_ir_v2;

  // Mode options
  bool mXilinx_flow;
  int m_reshape_threshold;
  int m_heuristic_threshold;
  bool m_dual_option;
  std::string m_transform_level;

  // Final decisions
  std::map<void *, std::map<int, int>> m_partitions;
  std::map<CMirNode *, bool> m_actions;  // Xilinx use only

  // Temp results and caching data
  std::map<CMirNode *, std::map<void *, std::map<int, int>>> tmp_factors;
  std::map<void *, std::map<int, int>> prior_factors;
  std::map<void *, std::set<int>> adjust_factors;
  std::map<void *, int> cache_var_dim;
  std::map<void *, std::vector<size_t>> cache_var_size;
  std::map<void *, void *> cache_var_base_type;
  unordered_map<void *, std::map<void *, bool>> cache_trace_res;
  map<void *, vector<void *>> hls_resource_pragmas;
  map<void *, string> hls_resource_table;
  unordered_map<AccessInfo *, map<void *, set<void *>>> addr_visited_loops;
  map<void *, vector<void *>> member_accesses;
  LoopPartitionBuilder *loop_partition_builder;
  bool has_out_of_bound;

  void dual_port_adjust(void *arr_init, map<int, int> *local_partitions);

 public:
  MemoryPartition(CSageCodeGen *codegen, void *pTopFunc,
                  const CInputOptions &options, CMarsIr *mars_ir,
                  CMarsIrV2 *mars_ir_v2)
      : m_ast(codegen), mTopFunc(pTopFunc), mOptions(options),
        mMars_ir(mars_ir), mMars_ir_v2(mars_ir_v2),
        mXilinx_flow(false) {
    init();
  }
  void init();
  bool run();

  std::map<CMirNode *, bool> get_node_actions() { return m_actions; }
  std::map<void *, std::map<int, int>> get_partitions() { return m_partitions; }
  void set_registers(std::set<void *> registers) {
  }

  void partition_analysis();
  int partition_analysis_node(CMirNode *loop_node);
  void partition_merge();

  // Xilinx flow
  void insert_partition_xilinx();
  void partition_transform_xilinx();

  void reportPartitionMessage();
  void delete_existing_pragma();

  bool reorganize_factor(
      std::map<void *, std::vector<std::map<int, int>>> *v_factors);
  void index_transform(CMirNode *bNode);
  void array_index_transform();
  bool partition_pragma_gen_xilinx(void *arr_init, int dim, int factor);
  void check_hls_resource();

 private:
  void AnalyzeAccess(CMirNode *bNode, AccessInfo *access_info,
                     vector<pair<int, int>> *priority_sorting);
  bool ParseFullAccess(CMirNode *bNode, int dim, void *arr_init,
                       map<void *, vector<map<void *, int>>> *index_expr_full,
                       int *mod_size, int *dim_priority,
                       vector<void *> *vec_pntrs);
  int has_self_assign_ref(vector<void *> vec_pntrs);
  bool unroll_ref_generate(AccessInfo *access_info, int opt, void *curr_loop,
                           void *curr_ref,
                           vector<map<void *, int>> *index_expr_full,
                           vector<map<void *, int>> *index_expr_copy);
  bool partition_factor_anal(CMirNode *bNode, AccessInfo *access_info,
                             int t_dim, int *factor);
  bool unroll_ref_generate_full(CMirNode *bNode, AccessInfo *access_info,
                                int t_dim,
                                vector<map<void *, int>> *index_expr_copy,
                                map<int, void *> *index_pos);

  void collect_local_variables(void *func, vector<void *> *vec_decls,
                               std::unordered_set<void *> *visited_funcs);
  void check_resource_port_setting(void *var_init, bool *true_dual_port);
  void remove_repeated_ref(const vector<map<void *, int>> &index_expr,
                           map<int, int> *repeat_tag,
                           map<int, void *> index_pos);

  bool conflict_detect_full(int bank_proc, vector<map<void *, int>> index_expr,
                            map<int, int> repeat_tag,
                            map<int, void *> index_pos, int mod_size = 1);
  bool check_invalid_access_bound(void *sg_scope,
                                  vector<map<void *, int>> *index_expr,
                                  map<int, int> *repeat_tag, void *arr_init,
                                  int dim_size, int dim_val, int mod_size);
  int check_enough_partition(AccessInfo *access_info, int target_dim,
                             map<int, int> *repeat_tag, int *factor,
                             bool UNROLLED);

  void bank_renew(string prior, int *bank_proc);

  bool check_loop_tripcount_pragma(CMarsIr *mars_ir, void *sg_loop,
                                   int64_t *l_range);

  int trace_up_to_loop_iterator(void *ref, void **sg_iter);

  void index_trace_up(void *scope, void **ref);

  // User message
  void reportThresholdFactor(CMirNode *node, void *arr, int dim);
  void reportSuboptimalFGPIP(CMirNode *node, void *arr);

  // Debug print
  void print_partition(const std::map<void *, std::map<int, int>> &partitions);
  void print_index(vector<map<void *, int>> *index_expr,
                   map<int, int> *repeat_tag);
  void print_index(vector<map<void *, int>> *index_expr);
};

// Xilinx flow
void enumerate_point_space(const std::vector<int> &dim_size,
                           std::vector<std::vector<int>> *point_set);
int index_switch_transform(CSageCodeGen *codegen, void *input_array,
                           void *input_loop, std::vector<int> switch_factor_in,
                           bool read_only);
int choose_factor(std::vector<size_t> arr_size,
                  const std::vector<std::map<int, int>> &o_factors,
                  std::map<int, int> *m_factors, std::map<int, int> *p_factors);
void copy_factors(const std::vector<std::map<int, int>> &original,
                  std::vector<std::map<int, int>> *target,
                  const std::vector<size_t> &arr_size_org,
                  const std::vector<size_t> &arr_size_target, int partial_dim);
void copy_factors(const std::map<int, int> &original,
                  std::map<int, int> *target);
bool check_action(int f1, int f2);

bool factor_extract(CSageCodeGen *codegen, CMarsIr *mars_ir, void *sg_pragma,
                    map<int, int> *hls_factors, void *arr_init);
