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


#ifndef TRUNK_SOURCE_OPT_TOOLS_INCLUDE_LINE_BUFFER_H_
#define TRUNK_SOURCE_OPT_TOOLS_INCLUDE_LINE_BUFFER_H_
#include <map>
#include <vector>

#include "ir_types.h"
#include "mars_ir.h"
#include "mars_ir_v2.h"
#include "mars_message.h"

using MarsProgramAnalysis::CMarsExpression;
using MarsProgramAnalysis::CMarsRangeExpr;
using MarsProgramAnalysis::CMarsVariable;

int array_linearize_top(CSageCodeGen *codegen, void *pTopFunc,
                        const CInputOptions &options);

class LineBuffer {
 private:
  CSageCodeGen *m_ast;
  void *mTopFunc;
  CInputOptions mOptions;
  CMarsIr mMars_ir;
  CMarsIrV2 mMars_ir_v2;

  size_t mSingleBufferSize;  //  option from memory burst
  size_t mWindowSize;
  bool mAltera_flow;
  bool mXilinx_flow;
  bool m_analysis;
  map<void *, int64_t> loop_ub_cache;
  map<void *, int64_t> loop_lb_cache;
  map<void *, map<void *, size_t>> access_ub;
  map<void *, map<void *, size_t>> access_lb;

 public:
  LineBuffer(CSageCodeGen *codegen, void *pTopFunc,
             const CInputOptions &options, bool analysis_only)
      : m_ast(codegen), mTopFunc(pTopFunc), mOptions(options),
        mAltera_flow(false), mXilinx_flow(false), m_analysis(analysis_only) {
    init();
  }
  void init();
  bool run();
  //  top function
  void line_buffer_processing();
  //  Check wether if the accesses are stencil
  int access_analysis(
      CMirNode *bNode, void *arr_init, std::vector<void *> *stencil_loops,
      std::vector<size_t> *cache_wind,
      std::vector<std::vector<size_t>> *reuse_winds,
      std::map<int, int> *reuse_pntr2wind,
      std::map<int, std::vector<int64_t>> *base_distance,
      std::vector<int64_t> *base_cache_address,
      map<void *, vector<map<void *, int64_t>>> *pntr_to_index_expr,
      std::vector<void *> *index_copy2pntr, int is_delinear,
      std::map<void *, std::vector<MarsProgramAnalysis::CMarsExpression>>
          *mapIndex2Delinear,
      std::vector<map<void *, int64_t>> *first_index_expr,
      const std::vector<size_t> &new_arr_size);
  // Parse the references
  bool parse_full_accesses(
      CMirNode *bNode, void *arr_init, vector<void *> *index_expr_pntr,
      map<void *, vector<map<void *, int64_t>>> *pntr_index_expr,
      std::map<void *, std::vector<MarsProgramAnalysis::CMarsExpression>>
          *mapIndex2Delinear,
      int is_delinear);

  bool unroll_ref_generate(int opt, void *curr_loop, void *curr_ref,
                           const vector<map<void *, int64_t>> &index_expr_full,
                           vector<map<void *, int64_t>> *index_expr_copy,
                           int dim_size);
  void unroll_ref_generate_full(
      CMirNode *bNode,
      const map<void *, vector<map<void *, int64_t>>> &pntr_index_expr,
      vector<vector<map<void *, int64_t>>> *index_expr_copy,
      vector<void *> *index_copy_map_pntr,
      const std::vector<size_t> &new_arr_size);
  bool check_stencil(
      const vector<vector<map<void *, int64_t>>> &index_expr_full_copy,
      std::vector<void *> *stencil_loops, map<int, vector<int64_t>> *base_const,
      int *partial_dim);
  //  Create the line buffers and sliding windows
  //  Replace the original accesses
  //  Update the line buffers and sliding windows
  void code_transform(
      CMirNode *bNode, void *arr_init, const std::vector<void *> &stencil_loops,
      const std::vector<size_t> &cache_wind,
      const std::vector<std::vector<size_t>> &reuse_winds,
      const std::map<int, int> &reuse_pntr2wind,
      std::map<int, std::vector<int64_t>> *base_distance,
      const std::vector<int64_t> &base_cache_address,
      const std::vector<void *> &index_copy2pntr, int is_delinear,
      const std::vector<size_t> &dim_split_steps,
      const std::vector<size_t> &new_arr_size,
      const map<void *, vector<map<void *, int64_t>>> &pntr_index_expr,
      const vector<map<void *, int64_t>> &first_index_expr, bool local_buffer);
  //  Check if the interface variable is delinearizable
  int check_delinearize_availability(
      void *sg_scope, void *sg_array, std::vector<size_t> *dim_split_steps,
      std::map<void *, std::vector<MarsProgramAnalysis::CMarsExpression>>
          *mapIndex2Delinear,
      std::vector<size_t> *delinear_arr_size);
  //  Get the delinearze info
  bool update_array_size_after_delinearize(void *sg_init,
                                           std::vector<size_t> *size,
                                           void **if_pragma,
                                           vector<size_t> *new_arr_size,
                                           void *kernel_decl);
  void generate_conditions(vector<void *> from_refs, vector<void *> to_refs,
                           void *condition_var, void *location);

 protected:
  //  Remove the pragmas
  void remove_line_buffer_pragmas();
  void generate_linearize_ref(std::vector<void *> *index_vector,
                              const std::vector<size_t> &dim_split_steps);
  void *generate_linearized_address(const vector<void *> &index_vector,
                                    const vector<size_t> &dim_split_steps,
                                    int start);
  void cache_loop_tripcount(const std::vector<void *> &stencil_loops);
  // Create line buffer variable declarations
  void create_line_buffers(std::vector<void *> *line_buffers, void *arr_init,
                           const std::vector<size_t> &cache_wind,
                           void *slide_scope, void *outside_loop,
                           const vector<vector<size_t>> &reuse_winds,
                           const vector<void *> &stencil_loops, int start_dim,
                           const vector<size_t> &bank_wind);
  // Create sliding window variable declarations
  void create_sliding_windows(std::vector<void *> *vec_winds,
                              void **tmp_buf_decl, void *arr_init,
                              const std::vector<vector<size_t>> &reuse_winds,
                              const std::vector<void *> &stencil_loops,
                              void *slide_scope, void *outside_loop,
                              const vector<size_t> &new_arr_size,
                              vector<size_t> *size_tmp_wind,
                              vector<size_t> *size_tmp_buf, int start_dim);
  void *move_stencil_in_condition(void *arr_init,
                                  const std::vector<void *> &stencil_loops,
                                  void *slide_scope, int start_dim);
  void loop_padding(const vector<void *> &stencil_loops,
                    const vector<vector<size_t>> &reuse_winds,
                    const vector<size_t> &new_arr_size, int start_dim);
  void loop_shifting(const vector<void *> &stencil_loops,
                     const vector<int64_t> &base_cache_address,
                     int caching_start_dim);
  void print_index(vector<map<void *, int64_t>> *index_expr);
  void print_index(vector<vector<map<void *, int64_t>>> *index_expr_full_copy);

  void slide_line_buffer_and_load_new_data(
      void *arr_init, const std::vector<void *> &line_buffers,
      const vector<vector<size_t>> &reuse_winds,
      const vector<size_t> &cache_wind, void *slide_bb,
      const std::vector<void *> &stencil_loops,
      const std::vector<int64_t> &base_cache_address,
      const std::vector<size_t> &size_tmp_buf, int caching_start_dim,
      const std::vector<size_t> &new_arr_size, void *arr_base_type,
      void *slide_loop, void *func_decl, void *tmp_buf_init);
  void *shift_and_update_sliding_windows(
      void *arr_init, const vector<size_t> &dim_split_steps,
      const std::vector<void *> &line_buffer,
      const std::vector<std::vector<size_t>> &reuse_winds,
      const std::vector<void *> &vec_winds, void *slide_loop,
      const std::vector<std::map<void *, int64_t>> &index_expr_copy,
      const std::vector<size_t> &cache_wind,
      const std::vector<void *> &stencil_loops,
      const std::vector<int64_t> &base_cache_address,
      const std::vector<size_t> &new_arr_size, int caching_start_dim,
      int is_delinear, void *slide_bb, int dim_size, void *stencil_stmt,
      void *func_decl, int parallel_opt, void *tmp_buf_decl,
      void **tmp_buf_init, void **caching_base_address);
  void replace_references_with_sliding_windows(
      const std::map<int, int> &reuse_pntr2wind,
      const std::vector<void *> &index_copy2pntr,
      const std::vector<void *> &vec_winds,
      const map<void *, vector<map<void *, int64_t>>> &pntr_index_expr,
      const vector<int64_t> &base_cache_address,
      const std::vector<size_t> &new_arr_size,
      const vector<void *> &stencil_loops, int caching_start_dim);
  void update_interface_pragma(void *if_pragma, void *arr_init, void *kernel);
};
#endif  // TRUNK_SOURCE_OPT_TOOLS_INCLUDE_LINE_BUFFER_H_
