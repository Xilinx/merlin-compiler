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


#ifndef TRUNK_SOURCE_OPT_TOOLS_INCLUDE_STREAM_PREFETCH_XILINX_H_
#define TRUNK_SOURCE_OPT_TOOLS_INCLUDE_STREAM_PREFETCH_XILINX_H_
#include <map>
#include <set>
#include <string>
#include <vector>
#include <tuple>
#include <utility>
#include "codegen.h"
#include "program_analysis.h"
#include "mars_ir.h"
#include "mars_ir_v2.h"

class StreamPrefetchXilinx {
  CSageCodeGen *m_ast;
  void *mTopFunc;
  CInputOptions mOptions;
  bool m_analysis;
  CMarsIrV2 mars_ir_v2;
  CMarsIr mars_ir;
  std::map<void *, std::map<void *, void *>> m_stream_read_function;
  std::map<void *, std::map<void *, void *>> m_stream_write_function;
  std::map<void *, void *> m_stream_class;
  std::map<void *, std::tuple<int64_t, int64_t, int64_t, int64_t>> m_loop_info;
  size_t m_burst_single_threshold;

 public:
  StreamPrefetchXilinx(CSageCodeGen *codegen, void *pTopFunc,
                       const CInputOptions &options, bool analysis)
      : m_ast(codegen), mTopFunc(pTopFunc), mOptions(options),
        m_analysis(analysis) {
    init();
  }
  void init();
  int run();

 private:
  bool get_single_access(void *arg, void **single_access, void *scope);
  void *
  get_sequential_scope(const MarsProgramAnalysis::CMarsExpression &index,
                       void *pos, void *scope,
                       const std::vector<std::pair<void *, void *>> &vec_loops,
                       int64_t start_range, std::vector<void *> *vec_atoms,
                       MarsProgramAnalysis::CMarsRangeExpr *me_range);
  bool is_unconditional_access(void *single_access, void *scope);

  MarsProgramAnalysis::CMarsExpression
  parse_index(void *single_access, void *scope,
              const MarsProgramAnalysis::CMarsExpression &offset);

  bool check_parallel_factor_matched_with_wide_bus_factor(
      int total_parallel_factor, int *wide_bus_factor,
      bool specified_opt_bitwidth);

  void *createStreamingFunction(
      void *arg, const std::string &new_func_name,
      const MarsProgramAnalysis::CMarsExpression &me_start,
      const MarsProgramAnalysis::CMarsExpression &me_length, void *new_channel,
      int channel_size, const std::vector<int> &vec_parallel_factor,
      int wide_bus_factor, bool read_only, void *scope,
      const std::vector<std::tuple<void *, int64_t, int64_t, int64_t, bool>>
          &non_sequentil_loops);

  void generate_channel_access(void *inner_loop_body, void *channel_pntr,
                               void *arg_pntr, void *arg, void *scope,
                               bool read_only);
  void *get_stream_write_function(void *input_type, void *scope);
  void *get_stream_read_function(void *return_type, void *scope);

  void
  replaceSingleAccessWithChannel(void *single_access, void *assign_exp,
                                 void *assign_val, void *new_channel,
                                 const std::vector<void *> &vec_parallel_loops,
                                 int channel_size, bool read_only, void *scope);

  bool is_constant_and_aligned_address_and_length(
      const MarsProgramAnalysis::CMarsRangeExpr &mr_range,
      MarsProgramAnalysis::CMarsExpression *me_start,
      MarsProgramAnalysis::CMarsExpression *me_length, int wide_bus_factor);
  void clean_interface_pragma(void *computation_kernel);
  void remove_all_logic_but_keep_interface_pragma(void *kernel);
  void
  update_interface_pragma(void *arg, void *kernel, int bus_bitwidth,
                          const MarsProgramAnalysis::CMarsExpression &me_start,
                          const MarsProgramAnalysis::CMarsExpression &me_length,
                          void *top_sequential_scope);
  bool isHLSStreamMemberCall(void *exp, std::string api_name);
  bool check_streaming_access(
      void *arg, void *single_access, void *scope, bool report,
      MarsProgramAnalysis::CMarsRangeExpr *mr_range, bool *read_only,
      std::vector<void *> *vec_parallel_loops,
      std::vector<int> *vec_parallel_factor,
      std::vector<std::tuple<void *, int64_t, int64_t, int64_t, bool>>
          *non_sequentil_loops,
      void **top_sequential_scope);
  bool is_constant_bound_canonical_loop(void *loop);
  bool get_canonical_loop_info(void *loop, void **iter, int64_t *i_lb,
                               int64_t *i_ub, int64_t *c_step,
                               int64_t *trip_count);
  bool get_and_analyze_loop_info(void *loop, void *loop_pos, void *pos,
                                 void *scope, int64_t *c_lb, int64_t *c_ub,
                                 int64_t *c_step, int64_t *c_trip_count);

  void *pass_loop_iterator_cross_function(void *loop, void *loop_iter,
                                          void *single_access);
  void *get_streaming_access_info_from_intrinsic(
      void *var, void *scope, void *pos,
      MarsProgramAnalysis::CMarsExpression *c_start,
      MarsProgramAnalysis::CMarsExpression *c_length, void **var_ref);

  bool check_streaming_access_from_intrinsic(
      void *arg, void *single_access, void *scope, bool report,
      const MarsProgramAnalysis::CMarsExpression &start,
      const MarsProgramAnalysis::CMarsExpression &length, void *var_ref,
      MarsProgramAnalysis::CMarsRangeExpr *mr_range, bool *read_only,
      vector<void *> *vec_parallel_loops, vector<int> *vec_parallel_factor,
      std::vector<std::tuple<void *, int64_t, int64_t, int64_t, bool>>
          *non_sequentil_loops,
      void **top_sequential_scope);

  bool
  lift_start_address(const MarsProgramAnalysis::CMarsExpression &start_index,
                     void *scope, bool check,
                     std::map<void *, void *> *old_loop2new_var,
                     void **new_address, set<void *> *related_loops);
  bool is_short_streaming_length(
      const MarsProgramAnalysis::CMarsExpression &me_length,
      int wide_bus_factor);
};

#endif  // TRUNK_SOURCE_OPT_TOOLS_INCLUDE_STREAM_PREFETCH_XILINX_H_
