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



#ifndef TRUNK_SOURCE_OPT_TOOLS_INCLUDE_MARS_IR_V2_H_
#define TRUNK_SOURCE_OPT_TOOLS_INCLUDE_MARS_IR_V2_H_
#include <unordered_set>
#include <unordered_map>
#include <limits>
#include <utility>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <tuple>

#include "bsuNode_v2.h"

enum effort { LOW = 0, STANDARD = 1, MEDIUM = 2, HIGH = 3 };
enum tool_type {
  XILINX_SDX = 0,
};
class CMarsIrV2 {
  std::set<void *> mKernelTops;  //  top level user kernel function declaration
  std::map<void *, void *>
      mKernel2Pragma;  //  the task pragma associated with the kernel if any
  std::set<void *>
      mAllKernelFuncs;  //  all the kernel functions and kernel subfunctions
  std::set<void *> mSharedPorts;
  std::vector<CMarsNode *> mIrNodes;  //  MarsIR nodes
  std::map<pair<void *, bool>, map<void *, bool>> m_port_is_bus;

  std::vector<CMarsEdge *> mIrEdgesList;  //  MarsIR nodes
  std::map<CMarsNode *, std::map<CMarsNode *, std::map<void *, CMarsEdge *>>>
      mIrEdges;  //  MarsIR edges

  std::map<void *, pair<int, int>>
      mKernel2IrNodeRange;  //  MarsIR nodes range for each kernel
  std::map<void *, std::vector<std::vector<int>>>
      mLoop2Orders;  //  map from loop to its order
  std::map<void *, std::map<std::vector<int>, void *>>
      mOrder2Loop;  //  map from order to its loop

  std::map<void *, void *> mCall2Func;              //  intermediate variable
  std::map<void *, std::set<void *>> mFunc2Caller;  //  intermediate variable
  std::map<void *, std::set<void *>> mFunc2Callee;  //  intermediate variable
  std::map<void *, bool> mFunc2Loop;                //  intermediate variable
  std::map<void *, bool> mFlattenLoop;              //  intermediate variable
  std::map<void *, CMarsLoop *> mLoopInfo;  //  map from loop to its info
  std::set<void *> mPragmas;
  CMarsAST_IF *m_ast;

  bool mValid;       //  check whether the progam can be distributed
                     //  according to for-loop
                     //  structures
  bool mValidCheck;  //  whether to do the above check

  bool mErrorOut;

  std::map<void *, std::vector<size_t>>
      mDimensionPragma;  //  the allocated size, constant
  std::map<void *, std::vector<void *>>
      mDepthPragma;  //  the size for date transfer, 0: equal to dimension,
                     //  sg_init: the variable
  std::map<void *, std::string>
      mContinueFlag;  //  the size for date transfer, 0: equal
                      //  to dimension, sg_init: the variable
  std::map<void *, bool>
      mBurstFlag;  //  true: infer burst; false: not infer burst
  std::map<std::pair<void *, void *>, bool>
      mWriteOnlyFlag;  //  true: do not infer read burst from host
                       //  even if the write is non-exact
  std::map<void *, bool> mCoalescingFlag;  //  true: not apply delinearization;
                                           //  false: apply delinearization
  std::map<void *, bool>
      mFalseDepFlag;  //  true: no data dependence; false: data dependence
  std::map<void *, size_t> mBurstMaxSize;  //  max threshold for each port
  std::set<void *> mNoSynchronizedPorts;

 public:
  explicit CMarsIrV2(CSageCodeGen *ast = nullptr)
      : m_ast(ast), mValid(true), mValidCheck(false), mErrorOut(false) {}
  void build_mars_ir(CSageCodeGen *codegen, void *pTopFunc,
                     bool check_valid = false, bool build_node = false);
  bool is_kernel(void *func_decl);

  bool is_valid() { return mValid; }

  void clear_port_is_bus_cache() { m_port_is_bus.clear(); }

 public:
  bool is_kernel_port(void *sg_init_name);

  bool is_tiled_loop(void *scope);

  bool is_shared_port(void *sg_init_name);


  std::set<void *> get_all_shared_ports() { return mSharedPorts; }

  bool iswithininnermostloop(void *stmt);

  bool skip_synchronization(void *port) {
    return mNoSynchronizedPorts.count(port) > 0;
  }

  bool is_undistributed_statement(void *stmt);

  bool get_burst_flag(void *sg_name);

  bool get_write_only_flag(void *sg_name, void *sg_scope);

  bool get_coalescing_flag(void *sg_name);

  bool get_false_dep_flag(void *arg);

  size_t get_burst_max_size(void *sg_name);

 protected:
  bool trace_to_bus(void *target_arr, std::map<void *, bool> *res, void *pos);

 public:
  bool any_trace_is_bus(void *target_arr, void *pos);
  bool trace_to_bus_available(void *target_arr, void *pos);

 protected:
  bool every_trace_is_bus(void *target_arr, void *pos);

 public:
  CMarsNode *get_node(size_t i);
  CMarsAST_IF *get_ast() { return m_ast; }
  std::vector<CMarsNode *> &get_all_nodes() { return mIrNodes; }
  std::vector<CMarsEdge *> &get_all_edges() { return mIrEdgesList; }
  std::set<void *> get_shared_ports(CMarsNode *node);
  ~CMarsIrV2();

  size_t size() const { return mIrNodes.size(); }

  bool isErrorOut() const { return mErrorOut; }

  void clear();

  void update_ports_for_all_nodes();

  void update_all_edges();
  void update_dependence_for_one_edge();
  void update_dependence_for_all_edges();
  void update_ssst_flag_for_all_edges();
  int get_number_of_all_sub_nodes(void *kernel_top,
                                  const std::vector<int> &order_prefix);
  void get_order_of_sub_nodes(void *kernel_top,
                              const std::vector<int> &order_prefix,
                              std::vector<int> *orders);

  std::vector<std::vector<int>> get_order_for_loop(void *loop);

  void *get_loop_for_order(void *kernel_top, const std::vector<int> &order);

  pair<int, int> get_node_range(void *kernel_top) {
    pair<int, int> res(std::numeric_limits<int>::max(),
                       std::numeric_limits<int>::max());
    std::map<void *, pair<int, int>>::iterator ret =
        mKernel2IrNodeRange.find(kernel_top);
    if (ret == mKernel2IrNodeRange.end())
      return res;
    return ret->second;
  }

  std::vector<CMarsNode *> get_nodes_with_common_loop(/*void *kernel_top, */
                                                      void *loop);

  CMarsEdge *get_edge_by_node(CMarsNode *node0, CMarsNode *node1, void *port) {
    return GetEdgeByNode(node0, node1, port);
  }

  void
  get_top_loop_info(std::map<void *, std::map<void *, bool>> *top_loop_2_ports,
                    std::map<void *, std::vector<void *>> *top_loops);

  std::vector<void *> get_top_kernels() {
    return std::vector<void *>(mKernelTops.begin(), mKernelTops.end());
  }
  void *get_kernel_task_pragma(void *kernel) {
    if (mKernel2Pragma.find(kernel) == mKernel2Pragma.end())
      return nullptr;
    else
      return mKernel2Pragma[kernel];
  }
  void *TraceUpToOneKernelByPos(void *pos);

  std::vector<void *> get_common_loops(std::vector<CMarsNode *> *nodes);

  CMarsLoop *get_loop_info(void *loop);

  int get_node_idx(CMarsNode *node);

  void remove_all_pragmas();

  bool remove_writeonly_shared_local_ports();

  //  ZP: 20160910
  std::vector<size_t> get_array_dimension(void *sg_name, int *cont_flag,
                                          void *pos = nullptr);
  std::vector<size_t> get_array_depth(void *sg_name, void *pos = nullptr);
  std::string get_depth_string(std::vector<size_t> size, int cont_flag);

  void build_node_info(const std::set<void *> &unrolled_loops,
                       const std::vector<void *> &topo_funcs);

 private:
  bool is_known_type(void *type);
  void collect_type_decl(void *decl, std::unordered_set<void *> *visited);
  void process_typedef(void *type, std::unordered_set<void *> *visited);
  void process_base_type(void *type, std::unordered_set<void *> *visited);
  bool isEmptyStmtSequence(const std::vector<void *> &stmts);

  void createNode(
      std::vector<void *> *stmts, const std::vector<void *> &loops,
      const std::map<void *, std::vector<pair<void *, bool>>> &loop2conds,
      const std::vector<pair<void *, bool>> &conds, std::vector<int> *orders,
      const t_func_call_path &call_path, void *kernel);

  bool checkSubLoops(void *sg_scope, const std::set<void *> &unrolled_loops);

  bool get_topological_order_funcs(std::vector<void *> *res);

  bool dfs_traverse(void *curr, std::set<void *> *visited,
                    std::set<void *> *updated, std::vector<void *> *res);

  void buildNode(void *body, std::vector<void *> *loops,
                 std::map<void *, std::vector<pair<void *, bool>>> *loop2cond,
                 std::vector<pair<void *, bool>> *conds,
                 std::vector<int> *orders, t_func_call_path *call_path,
                 void *kernel, const std::set<void *> &unrolled_loops);

  CMarsEdge *GetEdgeByNode(CMarsNode *node0, CMarsNode *node1, void *port) {
    if (mIrEdges.end() == mIrEdges.find(node0)) {
      std::map<CMarsNode *, std::map<void *, CMarsEdge *>> empty;
      mIrEdges.insert(
          pair<CMarsNode *,
               std::map<CMarsNode *, std::map<void *, CMarsEdge *>>>(node0,
                                                                     empty));
    }
    if (mIrEdges[node0].end() == mIrEdges[node0].find(node1)) {
      std::map<void *, CMarsEdge *> empty;
      mIrEdges[node0].insert(
          pair<CMarsNode *, std::map<void *, CMarsEdge *>>(node1, empty));
    }
    if (mIrEdges[node0][node1].end() == mIrEdges[node0][node1].find(port)) {
      CMarsEdge *edge = new CMarsEdge(m_ast, node0, node1, port);
      mIrEdges[node0][node1].insert(pair<void *, CMarsEdge *>(port, edge));
    }

    return mIrEdges[node0][node1][port];
  }


  bool check_identical_indices(const std::set<void *> &refs);

  void init_variable_info_pragma();
  size_t get_dimension_by_pragma(void *sg_name, int i, int *cont_flag);
  size_t get_depth_by_pragma(void *sg_name, int i, void *pos);

  bool is_flatten_loop(void *loop, const std::set<void *> &unrolled_loops);

  bool trace_to_bus(void *target_arr, std::map<void *, bool> *res, void *pos,
                    std::set<void *> *visited);

 public:
  void *get_unique_wrapper_pragma(void *sg_init);
  void *get_unique_interface_pragma(void *sg_init);

  std::string get_pragma_attribute(void *sg_pragma, std::string key,
                                   bool allow_hls = false);
  std::string get_all_pragma_attribute(void *sg_pragma, std::string key,
                                       bool *is_cmost);
  std::string get_pragma_command(void *sg_pragma, bool allow_hls = false);
  void set_pragma_attribute(void **sg_pragma, std::string key,
                            std::string value, bool third_party = false);

  //  s_fileter: the field after ACCEL, e.g. "interface", "wrapper", etc.
  void *get_pragma_by_variable(std::string s_var, void *scope,
                               std::string s_filter = "");

  void get_function_bound(void *func, std::set<void *> *bound_set);

  bool
  check_identical_indices_without_simple_type(const std::set<void *> &refs);

  std::vector<std::tuple<CMarsNode *, bool, int>>
  get_coarse_grained_def(CMarsNode *node, void *port);

  bool has_definite_coarse_grained_def(CMarsNode *node, void *port);

  void reset_kernel_node_analysis() { m_valid_kernel_node_analysis = false; }

  bool is_used_by_kernel(void *sg_node);

 private:
  bool is_used_by_kernel_sub(void *sg_node);
  bool m_valid_kernel_node_analysis;
  std::unordered_map<void *, bool> m_used_by_kernel_cached;
  std::unordered_set<void *> m_var_used_by_kernel;
  std::unordered_set<void *> m_func_used_by_kernel;
  std::unordered_set<void *> m_type_used_by_kernel;
  void collect_all_kernel_nodes();
  void collect_all_kernel_nodes(void *scope,
                                std::unordered_set<void *> *visited);
};

#endif  // TRUNK_SOURCE_OPT_TOOLS_INCLUDE_MARS_IR_V2_H_
