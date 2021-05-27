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


#ifndef TRUNK_SOURCE_OPT_TOOLS_INCLUDE_MEMORY_BURST_H_
#define TRUNK_SOURCE_OPT_TOOLS_INCLUDE_MEMORY_BURST_H_
#include <vector>
#include <map>
#include <utility>
#include <set>
#include <string>

#include "PolyModel.h"
#include "cmdline_parser.h"
#include "file_parser.h"
#include "mars_ir_v2.h"
#include "mars_ir.h"
#include "mars_opt.h"
#include "tldm_annotate.h"
#include "xml_parser.h"

#include "program_analysis.h"
class MemoryAccessInfo {
  size_t m_burst_number;
  size_t m_burst_length;

 public:
  MemoryAccessInfo(size_t burst_number, size_t burst_length)
      : m_burst_number(burst_number), m_burst_length(burst_length) {}
  size_t get_burst_number() const { return m_burst_number; }
  size_t get_burst_length() const { return m_burst_length; }
};
class MemoryBurst {
  CSageCodeGen *m_ast;
  void *mTopFunc;
  CInputOptions mOptions;
  CMarsIr mMars_ir;
  CMarsIrV2 mMars_ir_v2;
  size_t mSizeofBRAM;
  size_t m_current_total_usage;
  size_t mSingleBufferSize;
  size_t mLiftingThreshold;
  long double mLiftingRatio;
  bool mNaive_tag;
  bool mMemcpy_inline;
  bool m_report_out_of_resource;
  enum effort mEffort;
  size_t m_length_threshold;
  size_t m_wide_bus_length_lower_threshold;
  size_t m_partition_factor_upper_threshold;
  bool m_lift_fine_grained_cache;
  void *m_top_kernel;
  std::set<void *> mInsertedGlobal;
  //  key: <var, loop> -> basicblock
  std::map<std::pair<void *, void *>, void *> mMemcpyBlockBefore;
  std::map<std::pair<void *, void *>, void *> mMemcpyBlockAfter;
  std::map<void *, void *> mArray2TopArg;
  std::set<void *> m_assert_header_file;
  std::map<void *, std::set<void *>> m_cached_scope;
  enum ACCESS_FLAG {
    NO_ACCESS = 0,
    READ_FLAG = 1,
    WRITE_FLAG = 2,
    READ_WRITE_FLAG = 3
  };

  std::map<void *, std::map<void *, std::map<void *, void *>>>
      mParameter2CallAndOffset;
  std::map<void *, int> mParameter2Offset;

  std::map<void *, int> mArgOptBitwidth;

 public:
  MemoryBurst(CSageCodeGen *codegen, void *pTopFunc,
              const CInputOptions &options, bool report)
      : m_ast(codegen), mTopFunc(pTopFunc), mOptions(options),
        mNaive_tag(false), mMemcpy_inline(false),
        m_report_out_of_resource(!report), mEffort(STANDARD),
        m_length_threshold(1024),
        m_wide_bus_length_lower_threshold(WIDE_BUS_LENGTH_THRESHOLD),
        m_partition_factor_upper_threshold(PARTITION_FACTOR_UPPER_THRESHOLD),
        m_lift_fine_grained_cache(true), m_top_kernel(nullptr) {
    init();
  }

  bool run();

  void init();

  void clean_tiling_tag_and_cache();

  // Interface for auto coalescing
  bool auto_cache_for_coalescing();
  void set_arg_opt_bitwidth(void *arg, int opt_bitwidth);
  int get_arg_opt_bitwidth(void *arg) const;
  void set_top_kernel(void *func_decl) { m_top_kernel = func_decl; }
  void *get_top_kernel() const { return m_top_kernel; }

 public:
  enum BURST_DIAGNOSIS_INFO {
    NO_NEED_BURST = 0,
    BURST_APPLIED = 1,
    READ_WRITE_SYNC = 2,
    NON_SIMPLE_FORM = 3,
    NON_EXACT_ACCESS = 4,
    NEGATIVE_OFFSET = 5,
    NEGATIVE_BURST_LENGTH = 6,
    UNSUPPORTED_REFERENCE = 7,
    UNDETERMINED_LEN = 8,
    RESOURCE_OUT_OF_BOUND = 9,
    INFINITE_RANGE = 10,
    UNALIGNED_BURST_LENGTH = 11,
    UNALIGNED_BURST_OFFSET = 12,
    BREAK_DATAFLOW = 13,
    UNSUPPORTED_CONTROL_FLOW = 14,
    NON_EXACT_WRITE_ONLY_IN_DATAFLOW = 15,
  };

  void cg_check_range_availability(
      void *sg_scope, void *sg_array, int element_size,
      const MarsProgramAnalysis::CMarsRangeExpr &mr_merged,
      MarsProgramAnalysis::CMarsExpression *me_start,
      MarsProgramAnalysis::CMarsExpression *me_length, int *is_applied,
      int64_t *buff_size, int *read_write, bool *is_non_negative_len,
      bool *is_exact, bool report);

 private:
  bool insert_burst(CMirNode *bNode, CMirNode *fNode, int node_num,
                    int parallel_tag, int parent_pipeline_t);
  bool burst_anal(CMirNode *bNode, CMirNode *fNode, void *array,
                  const std::vector<void *> &refs, int64_t *buffer_size,
                  int *element_size, void **burst_scope, bool *intra_reuse,
                  int parent_pipeline_t, int offset = 0);
  int get_granularity(CMirNode *bNode, const std::vector<void *> &bAccess,
                      void *array, int *burst_level, int parent_pipeline_t);
  int get_location(CMirNode *bNode, const std::vector<void *> &bAccess,
                   void *array, int *gran, int burst_level, int element_size,
                   int offset, bool *intra_reuse);
  bool build_memcpy(CMirNode *bNode, CMirNode *fNode,
                    const std::vector<void *> &bAccess, void *array,
                    int64_t buffer_size, void *burst_scope, int node_num,
                    int buffer_count, int element_size);
  void insert_flatten(CMirNode *bNode, string tag_str);

  void add_header(void *pos);

  bool is_partial_access(void *ref, void *loop);

  bool checkDataDependence(CMirNode *scope, CMirNode *lnode, bool group,
                           void *array);
  void *create_local_buffer(CMirNode *lnode, CMirNode *fNode, void *arr,
                            int node_num, void *burst_scope, int buffer_count,
                            int64_t buffer_size);

  bool check_memory_resource(void *array, void *scope, int64_t buffer_size,
                             int element_size);

  bool check_resource_limit(void *array, int64_t orig_buffer_size,
                            int element_size);

  bool check_single_resource_limit(void *array, int64_t buffer_size,
                                   int element_size);

  bool check_whole_resource_limit(int64_t buffer_size, int element_size);

  bool check_lifting_resource_limit(void *array, void *scope,
                                    int64_t last_buffer_size,
                                    int64_t new_buffer_size, int element_size);

  bool check_lifting_efficiency(MemoryAccessInfo last_burst_info,
                                MemoryAccessInfo new_burst_info,
                                void *last_scope, void *scope);

  void *get_insert_loop(CMirNode *bNode, int burst_location);

  void reportUnboundAccess(void *array,
                           const MarsProgramAnalysis::CMarsRangeExpr &range,
                           void *scope);

  void reportPossibleNegativeAccessLength(
      void *scope, void *array,
      const MarsProgramAnalysis::CMarsExpression &len);

  void reportDataDependence(CMirNode *bNode, void *array);

  void reportOutofResourceLimit(void *array, void *scope,
                                int64_t size_in_bytes);

  void reportOutofSingleResourceLimit(void *array, void *scope,
                                      int64_t size_in_bytes);

  void reportMessage(CMirNode *bNode, void *array, int64_t buffer_size,
                     int element_bytes);

  void cg_reportMessage(void *array, void *scope, int64_t buffer_size,
                        int element_bytes, string kind,
                        bool aggressive_write_only = false);

  void reportPossibleNegativeAccessStart(
      const MarsProgramAnalysis::CMarsExpression &me_start, void *sg_array,
      void *sg_scope);

  void reportLocalBurstStartAddress(
      void *array, const MarsProgramAnalysis::CMarsExpression &start_address,
      void *scope);

  bool isUnsupported(void *ref);
  bool isUnsupportedAndReport(void *ref);
  bool is_cg_trace_up(int burst_diagnosis_res);

 protected:
  void cg_get_loops_in_topology_order(std::vector<void *> *cg_scopes);
  int cg_check_reference_availability(
      void *sg_scope, void *sg_array,
      const MarsProgramAnalysis::CMarsExpression &me_start);
  int cg_check_single_reference_availability(void *sg_scope, void *sg_array);
  bool cg_transform_burst(
      void *sg_scope, void *sg_array,
      const MarsProgramAnalysis::CMarsExpression &me_merged_start,
      const MarsProgramAnalysis::CMarsExpression &me_merged_length,
      bool is_non_negative_merged_len, bool is_merge_exact,
      const MarsProgramAnalysis::CMarsExpression &me_write_start,
      const MarsProgramAnalysis::CMarsExpression &me_write_length,
      bool is_non_negative_write_len, bool is_write_exact, int read_write,
      void **buf_decl, void **memcpy_r, void **memcpy_w, int64_t *buffer_size,
      bool check_constant_burst_len, bool report, int element_size,
      bool *aggressive_write_only);
  int cg_transform_refs_in_scope(void *sg_array, void *offset, void *insert_pos,
                                 void *sg_array_replace, void *scope,
                                 int check_only,
                                 const std::vector<void *> &vec_offset);
  void cg_lifting_burst(void *sg_loop,
                        const MarsProgramAnalysis::CMarsExpression &me_start,
                        const MarsProgramAnalysis::CMarsExpression &me_length,
                        void **buf_decl, void **memcpy_r, void **memcpy_w,
                        int read_only);
  bool
  cg_get_merged_access_range(void *sg_scope, void *sg_array,
                             MarsProgramAnalysis::CMarsRangeExpr *mr_merged,
                             MarsProgramAnalysis::CMarsRangeExpr *mr_write,
                             int *read_write, int64_t *access_size);

  bool process_coarse_grained_top();
  bool process_fine_grained_top();

  void cg_get_options_for_lifting(void *curr_scope, void *curr_array,
                                  std::vector<void *> *vec_upper_scope,
                                  std::vector<void *> *vec_array_in_upper_scope,
                                  bool mars_ir_v2 = false);

  void cg_get_cached_options(void *scope);

  int is_cg_burst_available(void *lift_array, void *lift_scope,
                            int element_size, int64_t *buff_size,
                            int64_t *access_size, bool fifo_check = false,
                            bool report = false,
                            bool auto_coalecing_check = false);

  int check_control_flow_availability(void *scope);

  int cg_check_delinearize_availability(
      void *sg_scope, void *sg_array,
      MarsProgramAnalysis::CMarsExpression me_start);

  bool cg_memory_burst_transform(void *scope_to_lift, void *array_to_lift,
                                 bool check_constant_burst_len, bool report);

  bool process_coarse_grained_top_v2();

  bool process_all_scopes(const std::vector<void *> &scopes);

  bool process_one_node(CMarsNode *node);

  bool check_identical_range_in_multiple_nodes(void *port, void *scope,
                                               bool report);

  bool check_identical_indices_within_single_node(void *port, void *scope,
                                                  bool report);

  void update_mars_ir(void *port, void *scope);

  bool cg_transform_burst_for_parallel(
      void *sg_loop, void *sg_array,
      const MarsProgramAnalysis::CMarsExpression &me_start,
      const MarsProgramAnalysis::CMarsExpression &me_length,
      bool is_non_negative_len, bool is_exact, int read_write, void **buf_decl,
      void **memcpy_r, void **memcpy_w, int64_t *buffer_size, int para_size,
      int64_t lb, bool continuous, int element_size);

  bool cg_memory_burst_transform_for_parallel(void *scope, void *array);

  bool cg_memory_burst_transform_for_data_reuse(void *scope, void *array);

  int is_cg_parallel_available(void *array, void *scope, std::string *message);

  int cg_get_access_size(void *array, void *scope);

  bool cg_transform_burst_for_data_reuse(
      void *sg_scope, void *sg_array,
      const MarsProgramAnalysis::CMarsExpression &me_start,
      const MarsProgramAnalysis::CMarsExpression &me_length,
      bool is_non_negative_len, bool is_exact, int read_write, void **buf_decl,
      void **memcpy_r, void **memcpy_w, int64_t *buffer_size,
      const std::vector<void *> &vec_loops, const std::vector<size_t> &vec_lens,
      const std::vector<void *> &vec_lbs, void *insert_pos, int element_size);

  bool is_burst_candidate(void *var_init, void *func, void *pos, bool force_on);

  void *get_top_arg(void *var_init);

  void *update_position(void *orig_pos);

  void *
  create_offset_expr(const MarsProgramAnalysis::CMarsExpression &start_index,
                     void **burst_len, bool *is_non_negative_len,
                     bool *is_non_negative_start, int64_t *buff_size,
                     bool refine_len, bool final_expr, void *pos);

  size_t get_single_threshold(void *array, int element_size);

  void detect_user_burst();

  bool detect_user_for_loop_burst(void *ref, void *pntr);

  void refine_burst_start_and_length(
      const MarsProgramAnalysis::CMarsExpression &me_length,
      const MarsProgramAnalysis::CMarsExpression &me_start, void *insert_pos,
      void *sg_scope, void *sg_array, bool refine_len, int64_t *buffer_size,
      void **burst_len, void **burst_start_expr, bool *is_non_negative_len,
      bool *is_non_negative_start);

  void *createTempVariableWithBoundAssertion(void *func_decl, void *sg_scope,
                                             void *initializer, string var_name,
                                             int64_t up_bound,
                                             bool is_non_negative_len,
                                             void **assert_stmt);

  //  YX: handle negative offset during burst generation
  void add_offset_later(void *decl, void *param, void *call, void *offset_exp);

  void add_offset();

  void update_array_index(void *param, void *scope, void *offset);

  bool check_auto_caching_for_coalescing(void *curr_array, void *curr_scope);

  int get_access_number(void *array, void *sg_scope, std::set<void *> *visited);

  void fg_get_loops_in_topology_order(std::vector<void *> *fg_scopes);
};
static const char VIVADO_str[] = "HLS";
#endif  // TRUNK_SOURCE_OPT_TOOLS_INCLUDE_MEMORY_BURST_H_
