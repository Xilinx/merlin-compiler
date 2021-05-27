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
#include <unordered_set>
#include <utility>
#include "bsuGroup.h"
#include "ir_types.h"

typedef std::map<SgInitializedName *, vector<SgPragmaDeclaration *>>
    node_pragma_map;

class CMarsIr {
 public:
  enum flow_type { XILINX_HLS = 1, XILINX_SDX = 2};

 private:
  set<void *> kernel_decl;
  map<void *, vector<std::unordered_set<string>>> kernel2interface;
  set<void *> reduction_decl;
  set<string> task_decl;
  bool mErrorOut;
  // pair<var, pos>, map<traced_var, is_bus>>
  map<pair<void *, void *>, map<void *, bool>> m_port_is_bus;
  enum flow_type m_flow;

 public:
  void clear_port_is_bus_cache() { m_port_is_bus.clear(); }

 public:
  CMirNodeFuncSet ir_list;

 public:
  CMarsIr() : mErrorOut(false) {}
  void clear();
  typedef CMirNodeFuncSet::iterator iterator;
  void get_mars_ir(CSageCodeGen *codegen, void *pTopFunc,
                   const CInputOptions &options, bool build_node = false,
                   bool report = false, bool pragma_in_loop = true,
                   bool while_support = false);
  void build_mars_node(CSageCodeGen *codegen, void *pTopFunc,
                       const SetVector<void *> &all_kernel_funcs,
                       bool pragma_in_loop, bool report, bool while_support);
  void build_fine_grain(CSageCodeGen *codegen, bool while_support);
  void update_loop_node_info(CSageCodeGen *codegen);
  bool is_kernel(CSageCodeGen *codegen, void *func_decl);
  bool is_task(CSageCodeGen *codegen, void *func_decl);
  bool is_bus_port(CSageCodeGen *codegen, void *func_decl, size_t arg_index);
  bool is_reduction(CSageCodeGen *codegen, void *func_decl);

  bool trace_to_bus(CSageCodeGen *codegen, void *kernel_decl, void *target_arr,
                    map<void *, bool> *res, void *pos);

  bool trace_to_bus_available(CSageCodeGen *codegen, void *kernel_decl,
                              void *target_arr, void *pos = nullptr);
  bool any_trace_is_bus(CSageCodeGen *codegen, void *kernel_decl,
                        void *target_arr, map<void *, bool> *res,
                        void *pos = nullptr);
  bool every_trace_is_bus(CSageCodeGen *codegen, void *kernel_decl,
                          void *target_arr, void *pos = nullptr);
  void trace_to_bus_ports(CSageCodeGen *codegen, void *kernel_decl,
                          void *target_arr, set<void *> *top_args, void *pos);
  map<void *, vector<void *>> hls_partition_table;

  map<void *, vector<void *>> get_partition_table() {
    return hls_partition_table;
  }

  bool is_fine_grained_scope(CSageCodeGen *codegen, void *pos);

  void print_hls_partition(CSageCodeGen *codegen);

  void set_partition_table(SgInitializedName *sg_arr,
                           SgPragmaDeclaration *sg_pragma) {
    hls_partition_table[sg_arr].push_back(sg_pragma);
  }

  void set_partition_table(void *sg_arr, vector<void *> pragma_vec) {
    hls_partition_table[sg_arr] = pragma_vec;
  }

  void check_hls_partition(CSageCodeGen *codegen, void *pTopFunc,
                           bool include_interface = true);
  string getFactorString(SgPragmaDeclaration *decl);
  string getDimString(SgPragmaDeclaration *decl);
  bool getCompleteString(SgPragmaDeclaration *decl);
  bool isHLSPartition(void *sg_decl);
  bool isHLSTripCount(void *sg_decl);

  CMirNode *get_node(void *body_ref);

  ~CMarsIr();

  bool get_topological_order_nodes(vector<CMirNode *> *res);

  void get_all_kernel_nodes(vector<CMirNode *> *kernels,
                            set<CMirNode *> *all_nodes_set,
                            vector<CMirNode *> *all_nodes_vec,
                            bool exclude_task = false);

  void get_all_while_nodes(vector<CMirNode *> *all_nodes_vec);

  void get_related_declaration(CSageCodeGen *codegen, CMirNode *fNode);

  bool isErrorOut() { return mErrorOut; }

  bool HasLink(CSageCodeGen *codegen, void *sg_func_decl, void *sg_arg,
               bool report);

  void checkKernelFunctionInterface(CSageCodeGen *codegen, void *func_decl,
                                    void *sg_call);

  void splitString(string str, string delim, vector<string> *results);

 private:
  bool trace_to_bus(CSageCodeGen *codegen, void *kernel_decl, void *target_arr,
                    map<void *, bool> *res, void *pos, set<void *> *visited);

  bool dfs_traverse(CMirNode *curr, set<CMirNode *> *visited,
                    set<CMirNode *> *updated, const set<CMirNode *> &all_nodes,
                    vector<CMirNode *> *res);
  bool check_valid_reduction(CSageCodeGen *codegen, void *func_decl);
};
