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
// FIXME: check loop step==1
//
#include "array_delinearize.h"
#include "id_update.h"
#include "line_buffer.h"
#include "program_analysis.h"
#include "rose.h"
#include "math_gcd.h"
#include "stream_prefetch_xilinx.h"
#define CONST_ITER nullptr
#define NOLOOP nullptr
#define _DEBUG_LINEBUF_ 1
#define NOREUSE_WIND 1
// #define _DEBUG_FULL_ 1
int line_buffer_top(CSageCodeGen *codegen, void *pTopFunc,
                    const CInputOptions &options) {
  string tool_flow = options.get_option_key_value("-a", "impl_tool");
  if ((tool_flow != "aocl") &&
      (options.get_option_key_value("-a", "stream_prefetch") != "off")) {
    // check whether some ports can be applied streaming. If so,
    // generate __merlin_streaming_range() before line buffer code generation
    StreamPrefetchXilinx stream_analysis(codegen, pTopFunc, options, true);
    stream_analysis.run();
  }

  LineBuffer instance(codegen, pTopFunc, options, false);
  instance.run();

  return 0;
}

void LineBuffer::init() {
  if ("aocl" == mOptions.get_option_key_value("-a", "impl_tool")) {
    mAltera_flow = true;
    cout << "[MARS-PARALLEL-MSG] Enable altera flow mode.\n";
  }
  if ("sdaccel" == mOptions.get_option_key_value("-a", "impl_tool")) {
    mXilinx_flow = true;
    cout << "[MARS-PARALLEL-MSG] Enable xilinx flow mode.\n";
  }

  //  Get the threshold option from memory burst
  //  the threshold for the single buffer size is 1M byte
  mSingleBufferSize = 1 * 1024 * 1024;
  string max_size_of_buffer_str =
      mOptions.get_option_key_value("-a", "burst_single_size_threshold");
  if (!max_size_of_buffer_str.empty()) {
    mSingleBufferSize = my_atoi(max_size_of_buffer_str);
  }

  //  the sliding window size by default
  mWindowSize = 16;
}

bool LineBuffer::run() {
  cout << "==============================================" << endl;
  cout << "-----=# Line Buffer Optimization Start#=----\n";
  cout << "==============================================" << endl;
  mMars_ir.clear();
  mMars_ir.get_mars_ir(m_ast, mTopFunc, mOptions, true, false, true, true);
  mMars_ir_v2.clear();
  mMars_ir_v2.build_mars_ir(m_ast, mTopFunc, false, false);

  line_buffer_processing();

  cout << "============================================" << std::endl;
  cout << "-----=# Line Buffer Optimization End#=----\n";
  cout << "============================================" << std::endl << endl;

  return true;
}

void LineBuffer::line_buffer_processing() {
  vector<CMirNode *> vec_nodes;
  mMars_ir.get_topological_order_nodes(&vec_nodes);

  for (size_t j = 0; j < vec_nodes.size(); j++) {
    CMirNode *new_node = vec_nodes[j];

    if (new_node->is_function) {
      continue;
    }
    // REVIEW: May 27 2020, remove the limitation of while loop
    // Also works for coarse-grained loop
    // See all the loops under line buffer scope as parallelized loops
    // No matter they have the pragma or not
    if (new_node->has_line_buffer() != 0) {
      vector<void *> vars;
      new_node->get_opt_vars(m_ast, "line_buffer", &vars);
      void *sg_loop = m_ast->GetEnclosingNode("ForLoop", new_node->ref);
      for (auto var : vars) {
        cout << "check : " << m_ast->_p(var) << endl;
        auto var_info = getUserCodeInfo(m_ast, var);
        string var_name = m_ast->GetVariableName(var);
        string var_str =
            var_info.name.empty() ? m_ast->GetVariableName(var) : var_info.name;
        string str_label = m_ast->get_internal_loop_label(sg_loop);
        string loop_info = "\'" + str_label + "\' " +
                           getUserCodeFileLocation(m_ast, sg_loop, true);

        // REVIEW: MAKE notes
        vector<void *> stencil_loops;
        vector<size_t> cache_wind;
        vector<vector<size_t>> reuse_winds;
        map<int, int> reuse_pntr2wind;
        map<int, vector<int64_t>>
            base_distance;  //  distance with the first index,
        //  for the use of sliding window
        vector<int64_t> base_cache_address;
        vector<size_t> dim_split_steps;
        vector<size_t> delinear_arr_size;
        vector<size_t> new_arr_size;
        vector<CMarsExpression> partial_base_address;
        map<void *, vector<CMarsExpression>> mapIndex2Delinear;
        map<void *, vector<map<void *, int64_t>>> pntr_index_expr;
        vector<void *> index_copy2pntr;
        vector<map<void *, int64_t>> first_index_expr;
        //  Global memory access is always linearized
        //  Check whether if it can be delinearized into multidimensions
        int is_delinear = check_delinearize_availability(
            new_node->ref, var, &dim_split_steps, &mapIndex2Delinear,
            &delinear_arr_size);

        // Get array information from interface pragma
        void *if_pragma = nullptr;
        bool local_buffer = false;
        void *kernel = m_ast->TraceUpToFuncDecl(var);
        void *sg_init = var;
        map<void *, bool> res;
        int ret0 = mMars_ir.trace_to_bus(m_ast, kernel, var, &res, var);
        if (ret0) {
          assert(res.size() == 1);
          sg_init = res.begin()->first;
          // assert(res.begin()->second);
          if (!mMars_ir.is_kernel(m_ast, kernel)) {
            kernel = m_ast->TraceUpToFuncDecl(sg_init);
          }
        } else {
          local_buffer = true;
        }
        int ret = update_array_size_after_delinearize(
            sg_init, &delinear_arr_size, &if_pragma, &new_arr_size, kernel);
        // REVIEW: message
        if (ret == 0) {
          cout << "Fail to get array size information\n";
          continue;
        }
        int ret1 = access_analysis(
            new_node, var, &stencil_loops, &cache_wind, &reuse_winds,
            &reuse_pntr2wind, &base_distance, &base_cache_address,
            &pntr_index_expr, &index_copy2pntr, is_delinear, &mapIndex2Delinear,
            &first_index_expr, new_arr_size);
        if (ret1 <= 0) {
          if (ret1 == 0) {
            if (!m_analysis)
              dump_critical_message(LINEBUF_WARNING_2(var_str, loop_info));
          }
          continue;
        }

        if (!local_buffer)
          update_interface_pragma(if_pragma, sg_init, kernel);
        if (m_analysis) {
          // REVIEW: give streaming prefetch
          // each dimension
          //    start_address + (window_size-1)
          // and nonstencil base_address
          continue;
        }
        string stencil_info = "";
        for (size_t i = 0; i < reuse_winds[0].size(); i++) {
          size_t wind_size = reuse_winds[0][i];
          if (wind_size != NOREUSE_WIND)
            stencil_info += my_itoa(wind_size);
          else
            stencil_info += "1";
          if (i < reuse_winds[0].size() - 1)
            stencil_info += "x";
        }
        dump_critical_message(LINEBUF_INFO_1(var_str, loop_info, stencil_info));
        // Prepare: caching useful information
        cache_loop_tripcount(stencil_loops);

        code_transform(new_node, var, stencil_loops, cache_wind, reuse_winds,
                       reuse_pntr2wind, &base_distance, base_cache_address,
                       index_copy2pntr, is_delinear, dim_split_steps,
                       new_arr_size, pntr_index_expr, first_index_expr,
                       local_buffer);
      }
    }
  }
  if (!m_analysis) {
    remove_line_buffer_pragmas();
  }
}

//  It has to be in the pattern of
//  a[i+const1][j]
//  a[i][j+const2]
int LineBuffer::access_analysis(
    CMirNode *bNode, void *arr_init, std::vector<void *> *stencil_loops,
    std::vector<size_t> *cache_wind,
    std::vector<std::vector<size_t>> *reuse_winds,
    std::map<int, int> *reuse_pntr2wind,
    std::map<int, std::vector<int64_t>> *base_distance,
    std::vector<int64_t> *base_cache_address,
    map<void *, vector<map<void *, int64_t>>> *pntr_index_expr,
    vector<void *> *index_copy2pntr, int is_delinear,
    std::map<void *, std::vector<MarsProgramAnalysis::CMarsExpression>>
        *mapIndex2Delinear,
    std::vector<map<void *, int64_t>> *first_index_expr,
    const std::vector<size_t> &new_arr_size) {
  cout << "\n ==> check stencil pattern, parse accesses of variable "
       << m_ast->_up(arr_init) << endl;

  bNode->set_full_access_table_v2(m_ast, false);
  if (bNode->full_access_table_v2.find(arr_init) ==
      bNode->full_access_table_v2.end()) {
    cout << "No direct accesses of variable " << m_ast->_up(arr_init)
         << "in the loop.\n";
    return 0;
  }
  vector<void *> index_expr_pntr;
  vector<vector<map<void *, int64_t>>> index_expr_copy;

  // Step 1: parse the accesses
  int ret1 =
      parse_full_accesses(bNode, arr_init, &index_expr_pntr, pntr_index_expr,
                          mapIndex2Delinear, is_delinear);
  if (!ret1) {
    cout << "[warning] Not stencil\n ";
    return 0;
  }
  // Step 1.2: Unroll the accesses, and remove the repeat accesses
  unroll_ref_generate_full(bNode, *pntr_index_expr, &index_expr_copy,
                           index_copy2pntr, new_arr_size);
  // Step 1.3: Check stencil after unrolling the references
  int org_dim = new_arr_size.size();
  int partial_dim = -1;
  bool s_ret = check_stencil(index_expr_copy, stencil_loops, base_distance,
                             &partial_dim);
  if (!s_ret) {
    cout << " FAILED \n";
    return 0;
  }
  *first_index_expr = index_expr_copy[0];
  int caching_start_dim = partial_dim + 1;
  cout << "Partial stencil dimension starts from dim=" << caching_start_dim
       << endl;

  cout << "[info] Step1 done...\n";

  // REVIEW: wrap into a function
  //  Review from YX: to reduce the complexity using 1 layer of loop
  //  Step2:
  //  2.1 check loop nest is constant from outside to inner
  //  is the same as the array index from left to right
  //  for(i..)
  //   for(j..)
  //     a[i][j]
  //  2.2 check the array size is consistency with loop tripcount
  map<void *, int64_t> stencil_loop_trip_count;
  {
    void *outer_loop = nullptr;
    void *outermost_loop = caching_start_dim < stencil_loops->size()
                               ? (*stencil_loops)[caching_start_dim]
                               : nullptr;
    for (size_t t = caching_start_dim; t < stencil_loops->size(); ++t) {
      void *curr_loop = (*stencil_loops)[t];
      cout << "Stencil loops: " << m_ast->_up(curr_loop) << endl;
      string var_str = m_ast->_up(arr_init);
      string str_label = m_ast->get_internal_loop_label(curr_loop);
      string loop_info = "\'" + str_label + "\' " +
                         getUserCodeFileLocation(m_ast, curr_loop, true);
      int64_t l_range = 0;
      int ret = static_cast<int>(
          m_ast->get_loop_trip_count_simple(curr_loop, &l_range));
      if (ret == 0) {
        //  FIXME: report cannot get trip count
        cout << "Cannot get the trip count\n";
        return 0;
      } else {
        stencil_loop_trip_count[curr_loop] = l_range;
      }
      if (outer_loop == nullptr) {
        outer_loop = curr_loop;
        continue;
      }
      //  Yuxin 8 Jan 2018: Is it necessary to be perfect nested loop?
      cout << "inner loops: " << m_ast->_up(curr_loop) << endl;
      if (m_ast->IsInScope(curr_loop, outer_loop) == 0) {
        //  FIXME: report loops sequential has to be same as index dimensions
        cout << "Sequential is not the same as index\n";
        return 0;
      }
      //       if loop b/c
      //      loop  a...
      //          loop  b...
      //            v[a][b]
      //          loop  c...
      //            b[a][c]
    }

    if (outermost_loop != nullptr &&
        m_ast->has_write_in_scope(arr_init, outermost_loop) != 0) {
      cout << "Cannot support write access\n";
      return 0;
    }
  }
  cout << "[info] Step2 done...\n";

  //  Step3: check stencil patterns
  //  Caculate the reuse distances
  //  base: i, j, k
  vector<int64_t> distance_lb(org_dim, std::numeric_limits<int64_t>::max());
  vector<int64_t> distance_ub(org_dim, std::numeric_limits<int64_t>::min());
  vector<int64_t> max_distance(org_dim, 0);
  base_cache_address->resize(org_dim);
  {
    // Calculate the distance to the first reference
    for (size_t k = 0; k < org_dim; k++) {
      for (auto &vec_const : *base_distance) {
        int64_t dist = vec_const.second[k];
        if (dist > distance_ub[k]) {
          distance_ub[k] = dist;
        }
        if (dist < distance_lb[k]) {
          distance_lb[k] = dist;
        }
      }
    }
    for (size_t k = 0; k < org_dim; k++) {
      max_distance[k] = std::abs(distance_ub[k] - distance_lb[k]);
      (*base_cache_address)[k] = distance_lb[k];
    }

    //  Adjust to the distance with a[i][j]
    for (auto &idx : *base_distance) {
      for (size_t k = caching_start_dim; k < org_dim; k++) {
        idx.second[k] -= distance_lb[k];
      }
      // FIXME: removable code, we only have 1 sliding window
      (*reuse_pntr2wind)[idx.first] = 0;
    }
    //  print info
    cout << "Max distance: ";
    for (size_t k = 0; k < org_dim; k++) {
      cout << max_distance[k] << ",";
    }
    cout << endl;
    // Adjust caching_start_dim
    for (size_t k = caching_start_dim; k < org_dim; k++) {
      if (max_distance[k] == 0) {
        caching_start_dim++;
      } else {
        break;
      }
    }

#ifdef _DEBUG_FULL_
    cout << "Base distance: " << endl;
    for (auto idx : *base_distance) {
      auto seq = idx.first;
      cout << "-- idx " << seq << ": ";
      for (size_t k = 0; k < org_dim; k++) {
        cout << idx.second[k] << ",";
      }
      cout << endl;
    }
#endif
  }
  cout << "[info] Step3 done...\n";
  // REVIEW: it should be stencil size
  // when stencil size > 2, return
  if (org_dim - caching_start_dim >= 3) {
    //  FIXME: report
    cout << "Only support 1d/2d stencils.\n";
    return 0;
  }
  cout << "\n ==> check line buffer size\n";
  //  mSingleBufferSize is the number of bytes
  void *basic_type = m_ast->GetTypebyVar(arr_init);
  void *element_type = m_ast->GetBaseType(basic_type, true);
  int element_size = m_ast->get_bitwidth_from_type(element_type) / 8;
  size_t cache_threshold = mSingleBufferSize;
  if (element_size > 0)
    cache_threshold = mSingleBufferSize / element_size;

#if 1
  cout << "THRE: " << cache_threshold << endl;
  cout << "Array size: ";
  for (auto dim : new_arr_size) {
    cout << dim << ",";
  }
  cout << endl;
#endif
  if (new_arr_size[org_dim - 1] > cache_threshold) {
    //  FIXME: report too large array
    //  no benefit from line_buffer
    //  Hint to user: using tiling
    cout << "lowest dimension exceed the cache threshold\n";
    return 0;
  }

  //  Step4: Calculste the base address of cache
  //  Estimate the minimum cache size
  {
    //  Dimension from right to left
    //  Find the optimal cache size
    for (size_t i = 0; i < max_distance.size(); i++) {
      if (i < caching_start_dim) {
        cache_wind->push_back(NOREUSE_WIND);
      } else {
        if (i == caching_start_dim) {
          cache_wind->push_back(max_distance[i] + 1);
        } else {
          cache_wind->push_back(stencil_loop_trip_count[(*stencil_loops)[i]] +
                                max_distance[i]);
        }
      }
    }
    bool no_caching = true;
    for (size_t i = 0; i < cache_wind->size(); i++) {
      if ((*cache_wind)[i] > NOREUSE_WIND && i != cache_wind->size() - 1)
        no_caching = false;
    }
    if (no_caching) {
      cache_wind->clear();
      for (size_t i = 0; i < org_dim; i++) {
        cache_wind->push_back(NOREUSE_WIND);
      }
    }

#ifdef _DEBUG_LINEBUF_
    cout << "L1 cache size: ";
    for (auto dim : *cache_wind) {
      cout << dim << ",";
    }
    cout << endl;
#endif
    size_t cache_estimate_size = 1;
    for (size_t i = 0; i < cache_wind->size(); i++) {
      cache_estimate_size *= (*cache_wind)[i];
    }
    if (cache_estimate_size > cache_threshold) {
      //  FIXME: report too large array
      //  no benefit from line_buffer
      //  Hint to user: using tiling
      cout << "estimate dimension exceed the cache threshold: "
           << cache_estimate_size << "," << cache_threshold << endl;
      return 0;
    }
  }

  //  Step5: Calculate the sliding windows sizes
  {
    int index = org_dim - 1;
    while (index >= 0) {
      vector<size_t> local_wind;
      for (size_t k = 0; k < org_dim; k++) {
        local_wind.push_back(NOREUSE_WIND);
      }
      for (size_t j = 0; j < org_dim; j++) {
        //  reversely
        while (max_distance[index] == 0 && index >= 0) {
          local_wind[index] = NOREUSE_WIND;
          index--;
        }
#if 1
        if (index == org_dim - 1) {
          int64_t lowest_window = max_distance[index] + 1;
          if (bNode->has_parallel() != 0) {
            //   lowest_window = mWindowSize;
            //   while (max_distance[index] + 1 > lowest_window) {
            //     lowest_window += mWindowSize;  //  Keep the window size as
            //     16x
            //   }
            int64_t factor_range = 0;
            if (bNode->is_complete_unroll()) {
              void *sg_loop = m_ast->GetEnclosingNode("ForLoop", bNode->ref);
              m_ast->get_loop_trip_count_simple(sg_loop, &factor_range);
            } else {
              int unroll = 0;
              if (bNode->is_partial_unroll(&unroll) != 0) {
                factor_range = unroll;
              }
            }
            cout << "lowest_window: " << lowest_window << endl;
          }
          if (lowest_window > new_arr_size[index]) {
            return 0;
          }
          local_wind[index] = lowest_window;
        } else {
#endif
          if (index >= 0) {
            local_wind[index] = max_distance[index] + 1;
          }
        }
        index--;
        if (index < 0) {
          break;
        }
      }
      reuse_winds->push_back(local_wind);
    }
  }
#if 1
  int max_reuse_dim = 0;
  for (size_t t = 0; t < reuse_winds->size(); t++) {
    cout << "Reuse window " << t << ": ";
    for (auto dim : (*reuse_winds)[t]) {
      cout << dim << ",";
      if (dim != NOREUSE_WIND)
        max_reuse_dim++;
    }
    cout << endl;
  }
  cout << "Max reuse dimension: " << max_reuse_dim << endl;
  cout << "Cache address: ";
  for (auto dim : *base_cache_address) {
    cout << dim << ",";
  }
  cout << endl;
#endif
#ifdef _DEBUG_FULL_
//  for (auto it : *reuse_pntr2wind) {
//    cout << "Map pntr: " << it.first << " to window " << it.second << endl;
//  }
#endif
  return 1;
}

void LineBuffer::code_transform(
    CMirNode *bNode, void *arr_init, const vector<void *> &stencil_loops,
    const vector<size_t> &line_buffer_wind,
    const vector<vector<size_t>> &reuse_winds,
    const std::map<int, int> &reuse_pntr2wind,
    map<int, vector<int64_t>> *base_distance,
    const vector<int64_t> &base_cache_address,
    const vector<void *> &index_copy2pntr, int is_delinear,
    const vector<size_t> &dim_split_steps, const vector<size_t> &new_arr_size,
    const map<void *, vector<map<void *, int64_t>>> &pntr_index_expr,
    const vector<map<void *, int64_t>> &first_index_expr, bool local_buffer) {
  cout << "\n ==> Start code transformation\n ";

  int parallel_opt = 1;
  void *apply_loop = m_ast->GetEnclosingNode("ForLoop", bNode->ref);
  void *apply_bb_org = m_ast->GetLoopBody(apply_loop);
  void *slide_bb = apply_bb_org;
  void *slide_loop = stencil_loops[stencil_loops.size() - 1];
  void *func_decl = m_ast->TraceUpToFuncDecl(apply_loop);
  int dim_size = new_arr_size.size();

  vector<size_t> arr_size_tmp;
  void *array_type = m_ast->GetTypebyVar(arr_init);
  void *arr_base_type;
  m_ast->get_type_dimension(array_type, &arr_base_type, &arr_size_tmp);
  if (m_ast->IsModifierType(arr_base_type))
    arr_base_type = m_ast->GetElementType(arr_base_type);

  //  Step1: Create the line buffer
  vector<void *> line_buffers;
  vector<size_t> cache_wind;
  vector<size_t> bank_wind;
  int caching_start_dim = 0;
  void *caching_base_address = nullptr;
  for (size_t i = 0; i < line_buffer_wind.size(); i++) {
    if (line_buffer_wind[i] > NOREUSE_WIND) {
      if (i == caching_start_dim) {
        // Only store reuse size-1 lines
        cache_wind.push_back(line_buffer_wind[i] - 1);
      } else {
        bank_wind.push_back(line_buffer_wind[i]);
        cache_wind.push_back(line_buffer_wind[i]);
      }
    } else {
      caching_start_dim++;
      cache_wind.push_back(line_buffer_wind[i]);
    }
  }
  // No need line_buffer
  if (caching_start_dim > new_arr_size.size() - 1) {
    cache_wind.clear();
    caching_start_dim--;
  }
  cout << "Reuse start from dimension " << caching_start_dim << endl;
  apply_loop = stencil_loops[caching_start_dim];
  create_line_buffers(&line_buffers, arr_init, cache_wind, apply_bb_org,
                      apply_loop, reuse_winds, stencil_loops, caching_start_dim,
                      bank_wind);
  //  Step2: Create the silding windows
  //  2.1 Create the windows according to analysis results: reuse_winds
  vector<void *> vec_winds;
  void *tmp_buf_decl = nullptr;
  void *tmp_buf_init = nullptr;
  vector<size_t> size_tmp_wind;
  vector<size_t> size_tmp_buf;
  create_sliding_windows(&vec_winds, &tmp_buf_decl, arr_init, reuse_winds,
                         stencil_loops, apply_bb_org, apply_loop, new_arr_size,
                         &size_tmp_wind, &size_tmp_buf, caching_start_dim);

  //  Step3: Replace the references
  //  3.1 Create the references to the sliding windows
  //  3.2 Replace the references according to reuse_pntr2wind, which is
  //  the window mapping analysis
  replace_references_with_sliding_windows(
      reuse_pntr2wind, index_copy2pntr, vec_winds, pntr_index_expr,
      base_cache_address, new_arr_size, stencil_loops, caching_start_dim);

  //  Step4: Move all the stencil computations into condition
  void *stencil_stmt = move_stencil_in_condition(
      arr_init, stencil_loops, apply_bb_org, caching_start_dim);

  //  Step5: shift and update the sliding windows
  slide_bb = shift_and_update_sliding_windows(
      arr_init, dim_split_steps, line_buffers, reuse_winds, vec_winds,
      slide_loop, first_index_expr, cache_wind, stencil_loops,
      base_cache_address, new_arr_size, caching_start_dim, is_delinear,
      slide_bb, dim_size, stencil_stmt, func_decl, parallel_opt, tmp_buf_decl,
      &tmp_buf_init, &caching_base_address);

  //  Step6: slide the line buffer and load new data from global memory
  slide_line_buffer_and_load_new_data(
      arr_init, line_buffers, reuse_winds, cache_wind, slide_bb, stencil_loops,
      base_cache_address, size_tmp_buf, caching_start_dim, new_arr_size,
      arr_base_type, slide_loop, func_decl, tmp_buf_init);

  //  Step7: Loop padding
  //  This can cover the line_buffer and sliding windows initialize.
  loop_padding(stencil_loops, reuse_winds, new_arr_size, caching_start_dim);

  // Step8: Loop shifting to avoid lower bound checking
  loop_shifting(stencil_loops, base_cache_address, caching_start_dim);
  cout << "\n   =====: Line_buffer is applied\n";
}

void LineBuffer::remove_line_buffer_pragmas() {
  //  remove_all_line_buffer_pragma
  vector<void *> vec_pragmas;
  m_ast->GetNodesByType(mTopFunc, "preorder", "SgPragmaDeclaration",
                        &vec_pragmas);
  for (size_t i = 0; i < vec_pragmas.size(); i++) {
    auto decl = vec_pragmas[i];
    CAnalPragma ana_pragma(m_ast);
    bool errorOut;
    ana_pragma.PragmasFrontendProcessing(decl, &errorOut);
    if (ana_pragma.is_line_buffer()) {
      m_ast->RemoveStmt(decl);
    }
  }
}

int LineBuffer::check_delinearize_availability(
    void *sg_scope, void *sg_array, std::vector<size_t> *dim_split_steps,
    map<void *, vector<CMarsExpression>> *mapIndex2Delinear,
    vector<size_t> *delinear_arr_size) {
  map<void *, size_t> mapAlias2BStep;
  int ret = analysis_delinearize(m_ast, sg_array, 0, dim_split_steps,
                                 &mapAlias2BStep);
  if (ret != 0) {
    check_only_delinearize(m_ast, sg_array, 0, *dim_split_steps,
                           &mapAlias2BStep, mapIndex2Delinear,
                           delinear_arr_size);
    cout << "Array " << m_ast->_up(sg_array) << " is delinearizable: " << ret
         << endl;
    for (auto index : (*mapIndex2Delinear)) {
      cout << "pntr: " << m_ast->_up(index.first) << ": ";
      for (auto expr : index.second) {
        cout << expr.print_s() << ",";
      }
      cout << endl;
    }
  }
  std::reverse(dim_split_steps->begin(), dim_split_steps->end());
  return mapIndex2Delinear->size();
}

bool LineBuffer::update_array_size_after_delinearize(
    void *sg_init, vector<size_t> *delinear_arr_size, void **if_pragma,
    vector<size_t> *new_arr_size, void *kernel_decl) {
  // Get array information from type
  vector<size_t> arr_size_tmp;
  void *array_type = m_ast->GetTypebyVar(sg_init);
  void *basic_type;
  m_ast->get_type_dimension(array_type, &basic_type, &arr_size_tmp);
  std::reverse(arr_size_tmp.begin(), arr_size_tmp.end());
  // REVIEW: if there is no interface pragma
  // We need to generate a new pragma to disable burst
  // need to check whether if Merlin has added if there is no
  // interface pragma in the input code, and for pure kernel mode
  vector<void *> vec_pragmas;
  m_ast->GetNodesByType(kernel_decl, "preorder", "SgPragmaDeclaration",
                        &vec_pragmas);
  vector<size_t> arr_size_if;
  for (auto pragma : vec_pragmas) {
    CAnalPragma curr_pragma(m_ast);
    bool errorOut;
    curr_pragma.PragmasFrontendProcessing(pragma, &errorOut);
    string s_var = curr_pragma.get_attribute(CMOST_variable);
    if (curr_pragma.is_interface() && s_var == m_ast->_up(sg_init)) {
      cout << "Analyze interface pragma: " << m_ast->_up(pragma) << endl;
      string str_depth = curr_pragma.get_attribute(CMOST_max_depth);
      if (str_depth.empty())
        str_depth = curr_pragma.get_attribute(CMOST_depth);
      cout << "Interface depth: " << str_depth << endl;
      vector<string> vec_depth = str_split(str_depth, ',');
      for (size_t s = 0; s < vec_depth.size(); s++) {
        size_t var_depth = my_atoi(vec_depth[s]);
        arr_size_if.push_back(var_depth);
      }
      *if_pragma = pragma;
      break;
    }
  }

  // Compare three array size information and choose one
  // Priority delinear->codegen analysis->interface
  if (!delinear_arr_size->empty()) {
    for (size_t t = 1; t < delinear_arr_size->size(); t++) {
      if ((*delinear_arr_size)[t] <= 0) {
        cout << "Invalid delinearize information" << endl;
        return false;
      }
    }
    if (delinear_arr_size->size() > 0) {
      if ((*delinear_arr_size)[0] == 0) {
        if (arr_size_if.size() == 1) {
          size_t total_depth = arr_size_if[0];
          size_t var_depth = 1;
          for (size_t t = 1; t < delinear_arr_size->size(); t++) {
            var_depth *= (*delinear_arr_size)[t];
          }
          if (var_depth != 0) {
            (*delinear_arr_size)[0] = total_depth / var_depth;
          }
        } else if (arr_size_if.size() == delinear_arr_size->size()) {
          for (size_t t = 0; t < delinear_arr_size->size(); t++) {
            size_t var_depth = arr_size_if[t];
            if (t > 0) {
              if (var_depth != (*delinear_arr_size)[t])
                return false;
            } else {
              if (var_depth != (*delinear_arr_size)[t]) {
                (*delinear_arr_size)[0] = var_depth;
              }
            }
          }
        }
      }
      *new_arr_size = *delinear_arr_size;
    }
  }
  if (new_arr_size->empty() && !arr_size_if.empty()) {
    if (arr_size_if.size() != 1 || arr_size_if[0] != 0) {
      *new_arr_size = arr_size_if;
    }
  }
  if (new_arr_size->empty() && !arr_size_tmp.empty()) {
    if (arr_size_tmp.size() != 1 || arr_size_tmp[0] != 0) {
      *new_arr_size = arr_size_tmp;
    }
  }
  if (new_arr_size->empty()) {
    return false;
  }
  return true;
}

void LineBuffer::generate_conditions(vector<void *> from_refs,
                                     vector<void *> to_refs,
                                     void *condition_loop, void *location) {
  cout << "Generate conditions\n";
  void *condition_var = m_ast->GetLoopIterator(condition_loop);
  void *bb_t = location;
  for (size_t i = 0; i < from_refs.size(); i++) {
    void *new_assign =
        m_ast->CreateStmt(V_SgAssignStatement, to_refs[i], from_refs[i]);
    void *true_bb = m_ast->CreateBasicBlock();
    void *false_bb = m_ast->CreateBasicBlock();
    if (i == from_refs.size() - 1) {
      false_bb = nullptr;
    }
    if (condition_var != nullptr) {
      void *cond_expr = m_ast->CreateExp(
          V_SgEqualityOp, m_ast->CreateVariableRef(condition_var),
          m_ast->CreateConst((int64_t)i));
      void *cond_stmt = m_ast->CreateStmt(V_SgExprStatement, cond_expr);
      void *sg_if_stmt = m_ast->CreateIfStmt(cond_stmt, true_bb, false_bb);
      m_ast->AppendChild(bb_t, sg_if_stmt);
      bb_t = true_bb;
    }
    m_ast->AppendChild(bb_t, new_assign);
    bb_t = false_bb;
  }
  cout << "Generate conditions finish\n";
}

void LineBuffer::generate_linearize_ref(vector<void *> *index_vector,
                                        const vector<size_t> &dim_split_steps) {
  void *from_global_ref = nullptr;
  for (size_t i = 0; i < index_vector->size(); i++) {
    cout << "interface access: " << m_ast->_up((*index_vector)[i]) << endl;
    void *from_ref =
        m_ast->CreateExp(V_SgMultiplyOp, m_ast->CopyExp((*index_vector)[i]),
                         m_ast->CreateConst((int64_t)dim_split_steps[i]));
    if (i == 0) {
      from_global_ref = from_ref;
    } else {
      from_global_ref = m_ast->CreateExp(V_SgAddOp, from_global_ref, from_ref);
    }
  }
  index_vector->clear();
  index_vector->push_back(from_global_ref);
}

void *
LineBuffer::generate_linearized_address(const vector<void *> &index_vector,
                                        const vector<size_t> &dim_split_steps,
                                        int start) {
  void *from_global_ref = nullptr;
  for (size_t i = 0; i < start; i++) {
    void *from_ref =
        m_ast->CreateExp(V_SgMultiplyOp, m_ast->CopyExp(index_vector[i]),
                         m_ast->CreateConst((int64_t)dim_split_steps[i]));
    if (i == 0) {
      from_global_ref = from_ref;
    } else {
      from_global_ref = m_ast->CreateExp(V_SgAddOp, from_global_ref, from_ref);
    }
  }
  if (from_global_ref == nullptr)
    from_global_ref = m_ast->CreateConst(0);

  return from_global_ref;
}
//  Cache the loops' tripcount
void LineBuffer::cache_loop_tripcount(const vector<void *> &stencil_loops) {
  for (size_t t = 0; t < stencil_loops.size(); t++) {
    void *loop_t = stencil_loops[t];
    if (loop_t == NOLOOP)
      continue;
    void *func_decl = m_ast->TraceUpToFuncDecl(loop_t);
    CMarsRangeExpr mr = CMarsVariable(loop_t, m_ast, func_decl).get_range();
    CMarsExpression me_lb;
    CMarsExpression me_ub;
    mr.get_simple_bound(&me_lb, &me_ub);
    loop_ub_cache[loop_t] = me_ub.GetConstant();
    loop_lb_cache[loop_t] = me_lb.GetConstant();
  }
}

// Create line buffers
void LineBuffer::create_line_buffers(vector<void *> *line_buffers,
                                     void *arr_init,
                                     const vector<size_t> &cache_wind,
                                     void *slide_scope, void *apply_loop,
                                     const vector<vector<size_t>> &reuse_winds,
                                     const vector<void *> &stencil_loops,
                                     int start_dim,
                                     const vector<size_t> &bank_wind) {
  if (cache_wind.empty())
    return;
  void *func_decl = m_ast->TraceUpToFuncDecl(apply_loop);
  string var_name = m_ast->GetVariableName(arr_init);
  vector<size_t> arr_size_tmp;
  void *array_type = m_ast->GetTypebyVar(arr_init);
  void *arr_base_type;
  m_ast->get_type_dimension(array_type, &arr_base_type, &arr_size_tmp);
  if (m_ast->IsModifierType(arr_base_type))
    arr_base_type = m_ast->GetElementType(arr_base_type);

  for (size_t i = 0; i < cache_wind[start_dim]; i++) {
    void *new_cache_decl = nullptr;
    void *new_cache_init = nullptr;
    void *sg_array_type = m_ast->CreateArrayType(arr_base_type, bank_wind);
    string var_name = m_ast->GetVariableName(arr_init);
    string cache_name = "LineBuffer_" + var_name + my_itoa(i);
    m_ast->get_valid_local_name(func_decl, &cache_name);
    new_cache_decl = m_ast->CreateVariableDecl(
        sg_array_type, cache_name, nullptr, m_ast->TraceUpToScope(apply_loop));
    m_ast->InsertStmt(new_cache_decl, apply_loop);
    new_cache_init = m_ast->GetVariableInitializedName(new_cache_decl);
    line_buffers->push_back(new_cache_init);
  }
}

// Create sliding windows
void LineBuffer::create_sliding_windows(
    vector<void *> *vec_winds, void **tmp_buf_decl, void *arr_init,
    const vector<vector<size_t>> &reuse_winds,
    const vector<void *> &stencil_loops, void *slide_scope, void *apply_loop,
    const vector<size_t> &new_arr_size, vector<size_t> *size_tmp_wind,
    vector<size_t> *size_tmp_buf, int start_dim) {
  void *func_decl = m_ast->TraceUpToFuncDecl(apply_loop);
  vector<size_t> arr_size_tmp;
  void *array_type = m_ast->GetTypebyVar(arr_init);
  void *arr_base_type;
  m_ast->get_type_dimension(array_type, &arr_base_type, &arr_size_tmp);
  if (m_ast->IsModifierType(arr_base_type))
    arr_base_type = m_ast->GetElementType(arr_base_type);

  for (size_t i = 0; i < reuse_winds.size(); i++) {
    for (size_t i0 = start_dim; i0 < new_arr_size.size(); i0++) {
      size_t wind_size;
      wind_size = reuse_winds[i][i0];
      //  if (wind_size > NOREUSE_WIND) {
      size_tmp_wind->push_back(wind_size);
      // FIXME: just for 2d?
      if (i0 != stencil_loops.size() - 1)
        size_tmp_buf->push_back(wind_size);
      //   } else {
      // when stencil is larger than 3d
      //     if (i0 >= start_dim)
      //       size_tmp_wind->push_back(1);
      //    }
    }
    void *sg_array_type = m_ast->CreateArrayType(arr_base_type, *size_tmp_wind);
    string wind_name = "slide_wind_" + my_itoa(i);
    m_ast->get_valid_local_name(func_decl, &wind_name);
    void *wind_decl = m_ast->CreateVariableDecl(
        sg_array_type, wind_name, nullptr, m_ast->TraceUpToScope(apply_loop));
    m_ast->InsertStmt(wind_decl, apply_loop);
    vec_winds->push_back(wind_decl);

    {
      void *sg_array_type =
          m_ast->CreateArrayType(arr_base_type, *size_tmp_buf);
      string buf_name = "tmp_wind_" + my_itoa(i);
      m_ast->get_valid_local_name(func_decl, &buf_name);
      *tmp_buf_decl = m_ast->CreateVariableDecl(
          sg_array_type, buf_name, nullptr,
          slide_scope);  //  Insert later in the loop body
    }
  }
}

void *LineBuffer::move_stencil_in_condition(void *arr_init,
                                            const vector<void *> &stencil_loops,
                                            void *slide_scope, int start_dim) {
  void *slide_cond_expr = nullptr;
  for (size_t t = start_dim; t < stencil_loops.size(); t++) {
    void *loop_t = stencil_loops[t];
    if (slide_cond_expr == nullptr) {
      slide_cond_expr = m_ast->CreateExp(
          V_SgGreaterOrEqualOp,
          m_ast->CreateVariableRef(m_ast->GetLoopIterator(loop_t)),
          m_ast->CreateConst((int64_t)access_lb[arr_init][loop_t]));
    } else {
      slide_cond_expr = m_ast->CreateExp(
          V_SgAndOp, slide_cond_expr,
          m_ast->CreateExp(
              V_SgGreaterOrEqualOp,
              m_ast->CreateVariableRef(m_ast->GetLoopIterator(loop_t)),
              m_ast->CreateConst((int64_t)access_lb[arr_init][loop_t])));
    }
    slide_cond_expr = m_ast->CreateExp(
        V_SgAndOp, slide_cond_expr,
        m_ast->CreateExp(
            V_SgLessOrEqualOp,
            m_ast->CreateVariableRef(m_ast->GetLoopIterator(loop_t)),
            m_ast->CreateConst((int64_t)access_ub[arr_init][loop_t])));
  }
  void *cond_stmt =
      m_ast->CreateStmt(V_SgExprStatement, m_ast->CopyExp(slide_cond_expr));
  void *insert_bb = m_ast->CreateBasicBlock();
  int num_stmt = m_ast->GetChildStmtNum(slide_scope);
  bool nontrival = false;
  vector<void *> local_decls, local_stmts;
  vector<void *> new_decls, new_pragmas;
  for (int i = 0; i < num_stmt; ++i) {
    void *stmt = m_ast->GetChildStmt(slide_scope, i);
    void *new_stmt = m_ast->CopyStmt(stmt);
    if (!nontrival) {
      if (m_ast->IsPragmaDecl(stmt) != 0) {
        new_pragmas.push_back(new_stmt);
      } else {
        nontrival = true;
      }
    }
    m_ast->AppendChild(insert_bb, new_stmt);
    if (m_ast->IsVariableDecl(stmt) != 0) {
      local_decls.push_back(stmt);
      new_decls.push_back(new_stmt);
    }
    if (nontrival) {
      local_stmts.push_back(stmt);
    }
  }
  void *sg_if_stmt = m_ast->CreateIfStmt(cond_stmt, insert_bb, nullptr);
  m_ast->AppendChild(slide_scope, sg_if_stmt);
  for (int i = 0; i < static_cast<int>(local_decls.size()); ++i)
    m_ast->replace_variable_references_in_scope(local_decls[i], new_decls[i],
                                                insert_bb);
  for (int i = 0; i < static_cast<int>(new_pragmas.size()); ++i)
    m_ast->RemoveStmt(new_pragmas[i]);
  for (int i = 0; i < static_cast<int>(local_stmts.size()); ++i)
    m_ast->RemoveStmt(local_stmts[i]);

  return sg_if_stmt;
}

void LineBuffer::loop_padding(const vector<void *> &stencil_loops,
                              const vector<vector<size_t>> &reuse_winds,
                              const vector<size_t> &new_arr_size,
                              int start_dim) {
  cout << "Start from dimension=" << start_dim << endl;
  for (int t = static_cast<int>(stencil_loops.size() - 1); t >= start_dim;
       t--) {
    void *loop_t = stencil_loops[t];
    if (loop_t == NOLOOP)
      continue;
    cout << "Padding: " << m_ast->_p(loop_t) << "," << loop_ub_cache[loop_t]
         << endl;
    void *iter_t = m_ast->GetLoopIterator(loop_t);
    //  Pad the lower bound
    if (t == start_dim || t == stencil_loops.size() - 1) {
      size_t wind_size = reuse_winds[0][t] - 1;
      //     if (t == start_dim)
      //       wind_size = wind_size - 1;
      int64_t new_lb = loop_lb_cache[loop_t] - wind_size;
      loop_lb_cache[loop_t] = new_lb;
      void *init_stmt = m_ast->GetLoopInit(loop_t);
      void *sg_init_stmt =
          (static_cast<SgForInitStatement *>(init_stmt)->get_init_stmt()[0]);

      if (m_ast->IsExprStatement(sg_init_stmt) != 0) {
        void *sg_init_expr = m_ast->GetExprFromStmt(sg_init_stmt);
        void *new_init_expr =
            m_ast->CreateExp(V_SgAssignOp, m_ast->CreateVariableRef(iter_t),
                             m_ast->CreateConst(new_lb), 0);
        m_ast->ReplaceExp(sg_init_expr, new_init_expr);
      } else if (m_ast->IsVariableDecl(sg_init_stmt) != 0) {
        m_ast->SetInitializer(
            iter_t, m_ast->CreateInitializer(m_ast->CreateConst(new_lb)));
      }
    } else {
      //  Pad the upper bound
      size_t tripcount = loop_ub_cache[loop_t] - loop_lb_cache[loop_t] + 1;
      int64_t new_ub = loop_ub_cache[loop_t] + (new_arr_size[t] - tripcount);
      loop_ub_cache[loop_t] = new_ub;
      void *cond_stmt = m_ast->GetLoopTest(loop_t);
      void *sg_cond_expr = m_ast->GetExprFromStmt(cond_stmt);
      void *new_test =
          m_ast->CreateExp(V_SgLessThanOp, m_ast->CreateVariableRef(iter_t),
                           m_ast->CreateConst(new_ub + 1), 0);
      m_ast->ReplaceExp(sg_cond_expr, new_test);
    }
  }
}

// Parse and unroll the references
// Map: index_expr_pntr => pntr_index_expr
// pntr_index_expr:
// map<sg_full_pntr, dim_vector<each dimension iter->coeff map>>
bool LineBuffer::parse_full_accesses(
    CMirNode *bNode, void *arr_init, vector<void *> *index_expr_pntr,
    map<void *, vector<map<void *, int64_t>>> *pntr_index_expr,
    std::map<void *, std::vector<MarsProgramAnalysis::CMarsExpression>>
        *mapIndex2Delinear,
    int is_delinear) {
  int org_dim = m_ast->get_dim_num_from_var(arr_init);
  for (auto pp : bNode->full_access_table_v2[arr_init]) {
    void *sg_array = nullptr;
    void *sg_pntr;
    vector<void *> sg_indexes;
    int pointer_dim;
    m_ast->parse_pntr_ref_by_array_ref(pp, &sg_array, &sg_pntr, &sg_indexes,
                                       &pointer_dim);

#if 1
    //  Print info
    cout << "==>access: " << m_ast->_p(sg_pntr) << endl;
    for (size_t j = 0; j < sg_indexes.size(); ++j)
      cout << m_ast->_up(sg_indexes[j]) << ",";
    cout << endl;
#endif
    if (pointer_dim != org_dim) {
      return false;
    }

    vector<CMarsExpression> index_analysis;
    if (is_delinear == 0) {
      for (size_t j = 0; j < sg_indexes.size(); ++j) {
        void *sg_idx = sg_indexes[j];
        CMarsExpression index1(sg_idx, m_ast, sg_pntr);
        index_analysis.push_back(index1);
      }
    } else {
      if (mapIndex2Delinear->find(sg_pntr) != mapIndex2Delinear->end()) {
        index_analysis = (*mapIndex2Delinear)[sg_pntr];
      } else {
        cout << "[warn] Didnt find the pntr\n";
        return false;
      }
    }

    cout << m_ast->_p(sg_pntr) << endl;
    for (auto index1 : index_analysis) {
      cout << "idx: " << index1.print() << endl;
      map<void *, int64_t> index_expr;
      map<void *, CMarsExpression> *m_coeff_1 = index1.get_coeff();
      map<void *, CMarsExpression>::iterator it;
      for (it = m_coeff_1->begin(); it != m_coeff_1->end(); it++) {
        if (it->second.IsConstant() == 0) {
          return false;
        }
        if (m_ast->IsForStatement(it->first) != 0) {
          auto check_loop = it->first;
          auto coeff = it->second;  //  CMarsExpression
          // Maybe negative coefficient on non-parallelized loops
          if (coeff.GetConstant() != 1 &&
              m_ast->IsInScope(check_loop, bNode->ref) == 0) {
            cout << "stencil loop iterators' coefficient needs to be positive "
                    "1\n";
            return false;
          }
          index_expr[check_loop] = coeff.GetConstant();
          CMarsRangeExpr mr =
              CMarsVariable(check_loop, m_ast, sg_pntr).get_range();
          CMarsExpression me_lb;
          CMarsExpression me_ub;
          mr.get_simple_bound(&me_lb, &me_ub);
          cout << "acccess ubound: " << me_ub.GetConstant() << endl;
          cout << "acccess lbound: " << me_lb.GetConstant() << endl;
          access_ub[arr_init][check_loop] = me_ub.GetConstant();
          access_lb[arr_init][check_loop] = me_lb.GetConstant();
        } else {
          // Need to support partial stencil access
          cout << "Has variable other than iteration variable\n";
          index_expr[it->first] = it->second.GetConstant();
        }
      }
      index_expr[CONST_ITER] = index1.GetConstant();
      (*pntr_index_expr)[sg_pntr].push_back(index_expr);
    }
    index_expr_pntr->push_back(sg_pntr);
  }
  return true;
}

// Add references introduced by loop unrolling
// opt: '-1'- pipeline; '0'- complete unroll; "x"- x>0, unroll factor
void LineBuffer::unroll_ref_generate_full(
    CMirNode *bNode,
    const map<void *, vector<map<void *, int64_t>>> &pntr_index_expr,
    vector<vector<map<void *, int64_t>>> *index_expr_copy,
    vector<void *> *index_copy2pntr, const std::vector<size_t> &new_arr_size) {
  void *curr_loop = m_ast->GetEnclosingNode("ForLoop", bNode->ref);
  map<void *, set<void *>> loop_trace;
  // <sg_pntr, dim_vector<copied_expr<copied index mapping of each dimension>>>>
  map<void *, vector<vector<map<void *, int64_t>>>> pntr_index_expr_copy;
  map<int, set<void *>> visited_loops;
  cout << "[print ==>] Unroll the references in " << m_ast->_p(curr_loop)
       << "\n original refs: " << pntr_index_expr.size() << endl;

  for (auto index : pntr_index_expr) {
    auto sg_pntr = index.first;
    set<void *> loops;
    m_ast->get_loop_nest_from_ref(curr_loop, sg_pntr, &loops);
    loop_trace[sg_pntr] = loops;
  }
  // FIXME: only consider the subloops as parallelized loops in the scope
  // REVIEW: use enum to express the meaning
  int opt = -1;

  for (auto index : pntr_index_expr) {
    auto sg_pntr = index.first;
    auto &index_expr = index.second;
    for (size_t i = 0; i < new_arr_size.size(); i++) {
      size_t dim_size = new_arr_size[i];
      vector<map<void *, int64_t>> index_expr_org;
      index_expr_org.push_back(index_expr[i]);
      vector<map<void *, int64_t>> index_expr_copy_local;
      unroll_ref_generate(opt, curr_loop, sg_pntr, index_expr_org,
                          &index_expr_copy_local, dim_size);
      pntr_index_expr_copy[sg_pntr].push_back(index_expr_copy_local);
    }
  }
  for (auto it : pntr_index_expr_copy) {
    auto sg_pntr = it.first;
    auto pntr_index_expr_dim = it.second;
    for (size_t i = 0; i < new_arr_size.size(); i++) {
      size_t dim_size = new_arr_size[i];
      auto index_expr_copy_local = pntr_index_expr_dim[i];
      for (auto loop : loop_trace[sg_pntr]) {
        int opt_1 = 0;
        // the inner loops of a FGPIP loop should be all
        //  complete parallelized
        if (loop != curr_loop) {
          vector<map<void *, int64_t>> index_expr_copy_tmp;
          int ret =
              unroll_ref_generate(opt_1, loop, sg_pntr, index_expr_copy_local,
                                  &index_expr_copy_tmp, dim_size);
          index_expr_copy_local = index_expr_copy_tmp;
          if (ret) {
            if (visited_loops[i].count(loop) == 0) {
              visited_loops[i].insert(loop);
              cout << "Unrolled loop: " << m_ast->_up(loop) << endl;
            }
          }
        }
      }
      pntr_index_expr_copy[sg_pntr][i] = index_expr_copy_local;
    }
  }

  // Reorganize the unroll references
  // copied_expr<dim_vector<copied index mapping of each dimension>>>
  for (auto index : pntr_index_expr_copy) {
    auto index_pntr = index.first;
    auto pntr_index_expr_dim = index.second;
    vector<vector<map<void *, int64_t>>> index_expr_reorganize;
    for (size_t i = 0; i < new_arr_size.size(); i++) {
      auto copy_local = pntr_index_expr_dim[i];
      if (i == 0) {
        for (auto index : copy_local) {
          vector<map<void *, int64_t>> copy_new;
          copy_new.push_back(index);
          index_expr_reorganize.push_back(copy_new);
        }
      } else {
        vector<vector<map<void *, int64_t>>> reorganize_new;
        for (auto index : copy_local) {
          for (auto copy : index_expr_reorganize) {
            auto copy_new = copy;
            copy_new.push_back(index);
            reorganize_new.push_back(copy_new);
          }
        }
        index_expr_reorganize = reorganize_new;
      }
    }
    for (auto copy : index_expr_reorganize) {
      index_expr_copy->push_back(copy);
      index_copy2pntr->push_back(index_pntr);
    }
  }
#ifdef _DEBUG_FULL_
  cout << "[print ==>] Unroll the references, TOTAL copies: \n";
  print_index(index_expr_copy);
#endif
}

//  opt: '-1'- pipeline; '0'- complete unroll; "x"- x>0, unroll factor
bool LineBuffer::unroll_ref_generate(
    int opt, void *curr_loop, void *curr_ref,
    const vector<map<void *, int64_t>> &pntr_index_expr,
    vector<map<void *, int64_t>> *index_expr_copy, int dim_size) {
  void *sg_pos = m_ast->GetEnclosingNode("Statement", curr_ref);
  int Changed = false;
  int64_t upper_bound = 1;
  int64_t lower_bound = 0;

  if (opt == 0) {
    //  complete unroll
    CMarsVariable m_loop(curr_loop, m_ast, sg_pos);
    CMarsRangeExpr mr = m_loop.get_range();
    CMarsExpression me_lb;
    CMarsExpression me_ub;
    int ret = mr.get_simple_bound(&me_lb, &me_ub);
    if (ret == 0) {
      CMarsVariable m_loop(curr_loop, m_ast, curr_loop);
      CMarsRangeExpr mr = m_loop.get_range();
      mr.get_simple_bound(&me_lb, &me_ub);
    }
    CMarsExpression me_range = me_ub - me_lb + 1;
    if ((me_lb.IsConstant() != 0) && (me_ub.IsConstant() != 0)) {
      lower_bound = me_lb.GetConstant();
      upper_bound = me_ub.GetConstant() + 1;
    }
#ifdef _DEBUG_FULL_
    cout << "For loop: " << m_ast->_p(curr_loop) << endl;
    cout << "lb: " << me_lb.print() << endl;
    cout << "ub: " << me_ub.print() << endl;
#endif
  }

#ifdef _DEBUG_FULL_
  cout << "For loop: " << m_ast->_p(curr_loop) << endl;
  cout << "unroll_bound: " << lower_bound << " to " << upper_bound << endl;
#endif

  for (auto idx : pntr_index_expr) {
#ifdef _DEBUG_FULL_
    for (auto pair_coeff : idx) {
      cout << "coeff: " << m_ast->_p(pair_coeff.first) << endl;
      cout << " * " << pair_coeff.second << endl;
    }
#endif
    if (idx.find(curr_loop) != idx.end()) {
      for (size_t j = lower_bound; j < upper_bound; j++) {
        map<void *, int64_t> idx_new;
        int const_value = 0;
        if (idx.find(CONST_ITER) != idx.end()) {
          const_value = idx.find(CONST_ITER)->second;
        }
        for (auto pair_coeff : idx) {
          if (pair_coeff.first == curr_loop) {
            const_value += static_cast<int>(idx.find(curr_loop)->second * j);
          }
          if (opt != 0 || (opt == 0 && pair_coeff.first != curr_loop)) {
            idx_new[pair_coeff.first] = pair_coeff.second;
          }
        }
        idx_new[CONST_ITER] = const_value;
        index_expr_copy->push_back(idx_new);
        Changed = true;
      }
    } else {
      index_expr_copy->push_back(idx);
    }
  }
#ifdef _DEBUG_FULL_
  print_index(index_expr_copy);
  cout << "[print ==>] Unroll the references, copied refs: "
       << index_expr_copy->size() << endl;
#endif
  return Changed;
}

#ifdef _DEBUG_FULL_
void LineBuffer::print_index(vector<map<void *, int64_t>> *index_expr) {
  for (size_t i = 0; i < index_expr->size(); i++) {
    for (auto iter : (*index_expr)[i]) {
      if (iter.first == CONST_ITER) {
        cout << iter.second << "+";
      } else {
        void *curr_iter = m_ast->GetLoopIterator(iter.first);
        cout << m_ast->_up(curr_iter) << "*" << iter.second << "+";
      }
    }
    cout << endl;
  }
}

void LineBuffer::print_index(
    vector<vector<map<void *, int64_t>>> *index_expr_full_copy) {
  for (auto index_expr : *index_expr_full_copy) {
    for (size_t i = 0; i < index_expr.size(); i++) {
      cout << "[";
      for (auto iter : index_expr[i]) {
        if (iter.first == CONST_ITER) {
          cout << iter.second << "+";
        } else {
          void *curr_iter = m_ast->GetLoopIterator(iter.first);
          cout << m_ast->_up(curr_iter) << "*" << iter.second << "+";
        }
      }
      cout << "]";
    }
    cout << endl;
  }
}
#endif

static bool is_same_expression(CSageCodeGen *ast,
                               const map<void *, int64_t> &one,
                               const map<void *, int64_t> &other) {
  for (auto atom : one) {
    auto var = atom.first;
    auto coeff = atom.second;
    auto it = other.find(var);
    if (it != other.end() && it->second == coeff)
      continue;
    bool matched = false;
    for (auto other_atom : other) {
      if (ast->is_identical_expr(var, other_atom.first, nullptr, nullptr) &&
          other_atom.second == coeff) {
        matched = true;
        break;
      }
    }
    if (!matched)
      return false;
  }
  return true;
}

// REVIEW: check stencil loops' step=1
// REVIEW: need to add detailed algorithm to describe conditioning and
// what is stencil
//    check: A[i][0], if no iterator in the index(or after unroll), it is not
//    stencil check: A[i][j] and A[n][m], not stencil, each dim has the same
//    iterator check: A[i+j] each dim has only 1 iterator check: A[i*j][k+1],
//    A[i-1][k-1], non-stencil part is the same
bool LineBuffer::check_stencil(
    const vector<vector<map<void *, int64_t>>> &index_expr_full_copy,
    std::vector<void *> *stencil_loops, map<int, vector<int64_t>> *base_const,
    int *partial_dim) {
  cout << " ==  start check stencil\n";
  auto &first_index_expr = index_expr_full_copy[0];
  // 1. check how many dimensions with stencil index from rightmost
  // stencil index is single iterator plus constant, such as i + 2, i - 1, i.
  // but neither 2 * i, i * j, i + j or 4 is
  for (size_t t = first_index_expr.size(); t > 0; --t) {
    auto &index = first_index_expr[t - 1];
    void *loop = nullptr;
    int64_t coeff;
    bool is_stencil = true;
    for (auto iter : index) {
      if (m_ast->IsForStatement(iter.first)) {
        if (loop != nullptr) {
          // multiple iterator
          is_stencil = false;
        }
        loop = iter.first;
        coeff = iter.second;
      } else if (iter.first != CONST_ITER) {
        // neither iterator nor constant
        is_stencil = false;
        break;
      }
    }
    if (loop == nullptr || coeff != 1 || !is_stencil)
      break;
    stencil_loops->push_back(loop);
    *partial_dim = static_cast<int>(t - 2);
  }
  if (stencil_loops->empty()) {
    return false;
  }
  size_t stencil_loop_size = stencil_loops->size();
  // refill nullptr for the remaining leftmost dimension
  stencil_loops->resize(first_index_expr.size());
  for (size_t i0 = 0; i0 < index_expr_full_copy.size(); i0++) {
    auto &index_expr = index_expr_full_copy[i0];
    if (index_expr.size() != first_index_expr.size()) {
      return false;
    }
    int leftmost_dim = 0;
    vector<int64_t> vec_const;
    for (size_t i = index_expr.size(); i > 0; i--) {
      auto &index = index_expr[i - 1];
      if (leftmost_dim < stencil_loop_size) {
        // stencil index with only constant difference
        void *loop = nullptr;
        int64_t coeff = 0;
        for (auto iter : index) {
          if (m_ast->IsForStatement(iter.first)) {
            if (loop != nullptr)
              return false;
            loop = iter.first;
            coeff = iter.second;
          } else if (iter.first == CONST_ITER) {
            vec_const.push_back(iter.second);
          } else {
            return false;
          }
        }
        if (loop != (*stencil_loops)[leftmost_dim] || coeff != 1)
          return false;
      } else {
        if (!is_same_expression(m_ast, index, first_index_expr[i - 1]))
          return false;
        vec_const.push_back(0);
      }
      leftmost_dim++;
    }
    std::reverse(vec_const.begin(), vec_const.end());
    (*base_const)[i0] = vec_const;
  }
  std::reverse(stencil_loops->begin(), stencil_loops->end());
  return true;
}

void *LineBuffer::shift_and_update_sliding_windows(
    void *arr_init, const vector<size_t> &dim_split_steps,
    const vector<void *> &line_buffers,
    const vector<vector<size_t>> &reuse_winds, const vector<void *> &vec_winds,
    void *slide_loop, const vector<map<void *, int64_t>> &first_index_expr,
    const vector<size_t> &cache_wind, const vector<void *> &stencil_loops,
    const vector<int64_t> &base_cache_address,
    const vector<size_t> &new_arr_size, int caching_start_dim, int is_delinear,
    void *slide_bb, int dim_size, void *stencil_stmt, void *func_decl,
    int parallel_opt, void *tmp_buf_decl, void **tmp_buf_init,
    void **caching_base_address) {
  string new_parallel_pragma =
      std::string(ACCEL_PRAGMA) + " " + CMOST_PARALLEL + " " + CMOST_complete;
  for (size_t i = 0; i < reuse_winds.size(); i++) {
    void *wind_init = m_ast->GetVariableInitializedName(vec_winds[i]);
    void *insert_bb = m_ast->CreateBasicBlock();
    m_ast->InsertStmt(insert_bb, stencil_stmt);
    //  Step4.1: shift the sliding window
    //  Moving forward the data so that the index to the sliding window
    //  can keep consistency. Otherwise we need to use % in the index, which
    //  will lead to more resource
    {
      vector<void *> v_idx, v_idx_from;
      int count = 0;
      void *bb_t = nullptr;
      for (size_t t = caching_start_dim; t < new_arr_size.size(); t++) {
        void *loop_t = stencil_loops[t];
        void *iter_init = m_ast->GetLoopIterator(loop_t);
        void *iter_type = m_ast->GetTypebyVar(iter_init);

        if (reuse_winds[i][t] != NOREUSE_WIND) {
          void *to_ref = nullptr;
          string iter_name = m_ast->GetVariableName(iter_init);
          m_ast->get_valid_local_name(func_decl, &iter_name);
          void *new_iter_decl = m_ast->CreateVariableDecl(
              iter_type, iter_name, nullptr, m_ast->TraceUpToScope(slide_loop));
          m_ast->PrependChild(insert_bb, new_iter_decl, true);
          void *new_iter_init =
              m_ast->GetVariableInitializedName(new_iter_decl);
          int64_t step = 1;
          void *lb_exp = nullptr;
          if (t == stencil_loops.size() - 1) {
            lb_exp = m_ast->CreateConst(static_cast<int64_t>(1));
          } else {
            lb_exp = m_ast->CreateConst(static_cast<int64_t>(0));
          }
          void *new_loop_t = m_ast->CreateStmt(
              V_SgForStatement, new_iter_init, lb_exp,
              m_ast->CreateConst((int64_t)reuse_winds[i][t] - 1),
              m_ast->CreateBasicBlock(), &step, nullptr, slide_loop);
          if (count == 0) {
            m_ast->AppendChild(insert_bb, new_loop_t);
          } else {
            m_ast->AppendChild(bb_t, new_loop_t);
          }
          bb_t = m_ast->GetLoopBody(new_loop_t);
          void *sg_pragma = m_ast->CreatePragma(new_parallel_pragma, bb_t);
          m_ast->PrependChild(bb_t, sg_pragma);
          if (t == stencil_loops.size() - 1) {
            to_ref = m_ast->CreateExp(
                V_SgAddOp,
                m_ast->CreateVariableRef(m_ast->GetLoopIterator(new_loop_t)),
                m_ast->CreateConst(-1));
          } else {
            to_ref =
                m_ast->CreateVariableRef(m_ast->GetLoopIterator(new_loop_t));
          }
          void *from_ref =
              m_ast->CreateVariableRef(m_ast->GetLoopIterator(new_loop_t));
          v_idx.push_back(to_ref);
          v_idx_from.push_back(from_ref);
          count++;
        } else {
          // Dimension with size '1'
          void *to_ref = m_ast->CreateConst(0);
          void *from_ref = m_ast->CreateConst(0);
          v_idx.push_back(to_ref);
          v_idx_from.push_back(from_ref);
        }
      }
      void *from_pntr = m_ast->CreateArrayRef(
          m_ast->CreateVariableRef(wind_init), v_idx_from);
      void *to_pntr =
          m_ast->CreateArrayRef(m_ast->CreateVariableRef(wind_init), v_idx);
      void *shift_assign =
          m_ast->CreateStmt(V_SgAssignStatement, to_pntr, from_pntr);
      if (bb_t != nullptr) {
        m_ast->AppendChild(bb_t, shift_assign);
      }
    }

    m_ast->PrependChild(insert_bb, tmp_buf_decl, true);
    *tmp_buf_init = m_ast->GetVariableInitializedName(tmp_buf_decl);
    cout << "tmp buffer: " << m_ast->_up(tmp_buf_decl) << endl;

    //  Step4.2: load the new data from line buffer and interface to temp buffer
    {
      int count = 0;
      vector<void *> v_idx1;
      vector<void *> v_idx_from1;
      vector<void *> v_idx_global;
      void *dim_loop = nullptr;
      void *bb_t = insert_bb;
      void *cond_exp = nullptr;
      // Create base address
      for (size_t t = 0; t < caching_start_dim; t++) {
        void *from_ref_g = nullptr;
        for (auto iter : first_index_expr[t]) {
          void *ref_exp = nullptr;
          if (iter.first == CONST_ITER) {
            ref_exp = m_ast->CreateConst((int64_t)iter.second);
          } else {
            void *var_exp = nullptr;
            if (m_ast->IsForStatement(iter.first)) {
              var_exp =
                  m_ast->CreateVariableRef(m_ast->GetLoopIterator(iter.first));
            } else {
              var_exp = m_ast->CopyExp(iter.first);
            }
            ref_exp =
                m_ast->CreateExp(V_SgMultiplyOp, var_exp,
                                 m_ast->CreateConst((int64_t)iter.second));
          }
          if (from_ref_g == nullptr) {
            from_ref_g = ref_exp;
          } else {
            from_ref_g = m_ast->CreateExp(V_SgAddOp, from_ref_g, ref_exp);
          }
        }
        v_idx_global.push_back(from_ref_g);
      }
      for (size_t t = caching_start_dim; t < new_arr_size.size(); t++) {
        void *to_ref = nullptr;
        void *loop_t = stencil_loops[t];
        void *iter_init = m_ast->GetLoopIterator(loop_t);
        void *from_ref_g = nullptr;
        // Currently stencil_loops.size == new_arr_size.size
        // We dont consider fully unrolled dimension as non-stencil
        if (t < stencil_loops.size()) {
          from_ref_g = m_ast->CreateExp(
              V_SgAddOp, m_ast->CreateVariableRef(iter_init),
              m_ast->CreateConst((int64_t)base_cache_address[t]));
          from_ref_g = m_ast->CreateExp(
              V_SgAddOp, from_ref_g,
              m_ast->CreateConst((int64_t)reuse_winds[i][t] - 1));
          int64_t ub_bound = base_cache_address[t] + reuse_winds[i][t] - 1 +
                             loop_ub_cache[loop_t];
          if (ub_bound >= new_arr_size[t]) {
            void *left_op = m_ast->CopyExp(from_ref_g);
            void *upper_cond_exp =
                m_ast->CreateExp(V_SgLessThanOp, left_op,
                                 m_ast->CreateConst((int64_t)new_arr_size[t]));
            if (cond_exp == nullptr) {
              cond_exp = upper_cond_exp;
            } else {
              cond_exp = m_ast->CreateExp(V_SgAndOp, cond_exp, upper_cond_exp);
            }
          }
          int64_t lb_bound = base_cache_address[t] + loop_lb_cache[loop_t];
          if (lb_bound < 0) {
            void *left_op = m_ast->CopyExp(from_ref_g);
            void *lower_cond_exp = m_ast->CreateExp(
                V_SgGreaterOrEqualOp, left_op, m_ast->CreateConst((int64_t)0));
            if (cond_exp == nullptr) {
              cond_exp = lower_cond_exp;
            } else {
              cond_exp = m_ast->CreateExp(V_SgAndOp, cond_exp, lower_cond_exp);
            }
          }
        }
        if (t != stencil_loops.size() - 1) {
          void *iter_type = m_ast->GetTypebyVar(iter_init);
          if (!line_buffers.empty()) {
            string iter_name = m_ast->GetVariableName(iter_init);
            m_ast->get_valid_local_name(func_decl, &iter_name);
            void *new_iter_decl =
                m_ast->CreateVariableDecl(iter_type, iter_name, nullptr,
                                          m_ast->TraceUpToScope(slide_loop));
            m_ast->PrependChild(insert_bb, new_iter_decl, true);
            void *new_iter_init =
                m_ast->GetVariableInitializedName(new_iter_decl);
            int64_t step = 1;
            void *lb_exp = nullptr;
            lb_exp = m_ast->CreateConst(0);

            void *new_loop_t = m_ast->CreateStmt(
                V_SgForStatement, new_iter_init, lb_exp,
                m_ast->CreateConst((int64_t)reuse_winds[i][t] - 1),
                m_ast->CreateBasicBlock(), &step, nullptr, slide_loop);
            if (count == 0) {
              m_ast->AppendChild(insert_bb, new_loop_t);
            } else {
              m_ast->AppendChild(bb_t, new_loop_t);
            }
            bb_t = m_ast->GetLoopBody(new_loop_t);
            count++;
            void *sg_pragma = m_ast->CreatePragma(new_parallel_pragma, bb_t);
            m_ast->PrependChild(bb_t, sg_pragma);
            to_ref = m_ast->CreateVariableRef(new_iter_init);
            void *from_ref = m_ast->CreateVariableRef(new_iter_init);
            if (t == caching_start_dim) {
              dim_loop = new_loop_t;
            } else {
              from_ref_g =
                  m_ast->CreateExp(V_SgAddOp, from_ref_g,
                                   m_ast->CreateVariableRef(new_iter_init));
              from_ref = m_ast->CreateExp(
                  V_SgAddOp, from_ref,
                  m_ast->CreateVariableRef(m_ast->GetLoopIterator(loop_t)));
#if 0
              //  May need in the future, if we support more than 3 dimensions
              else
                cond_exp = m_ast->CreateExp(
                    V_SgAndOp, cond_exp,
                    m_ast->CreateExp(V_SgLessThanOp, left_op,
                      m_ast->CreateConst((int64_t)new_arr_size[t])));
#endif
            }
            v_idx1.push_back(to_ref);
            if (t != caching_start_dim) {
              v_idx_from1.push_back(from_ref);
            }
          }
        } else {
          void *from_ref =
              m_ast->CreateVariableRef(m_ast->GetLoopIterator(loop_t));
          from_ref =
              m_ast->CreateExp(V_SgSubtractOp, from_ref,
                               m_ast->CreateConst(loop_lb_cache[loop_t]));
          if (reuse_winds[i][t] != NOREUSE_WIND) {
            from_ref = m_ast->CreateExp(
                V_SgAddOp, from_ref,
                m_ast->CreateConst((int64_t)reuse_winds[i][t] - 1));
          }
          v_idx_from1.push_back(from_ref);
        }
        v_idx_global.push_back(from_ref_g);
      }
      void *insert_tmp = bb_t;
      void *to_tmp_pntr = m_ast->CreateArrayRef(
          m_ast->CreateVariableRef(*tmp_buf_init), v_idx1);
      if (bb_t != nullptr) {
        vector<void *> from_vectors;
        vector<void *> to_vectors;
        if (!line_buffers.empty()) {
          for (size_t i = 0; i < line_buffers.size(); i++) {
            void *tmp_op = m_ast->CopyExp(to_tmp_pntr);
            to_vectors.push_back(tmp_op);
            vector<void *> tmp_refs;
            for (auto ref : v_idx_from1) {
              tmp_refs.push_back(m_ast->CopyExp(ref));
            }
            void *new_cache_init = line_buffers[i];
            void *from_tmp_pntr = m_ast->CreateArrayRef(
                m_ast->CreateVariableRef(new_cache_init), tmp_refs);
            from_vectors.push_back(from_tmp_pntr);
          }
        }
        // Load new data
        generate_conditions(from_vectors, to_vectors, dim_loop, insert_tmp);
        {
          // MOVE HERE
          if (is_delinear != 0) {
            *caching_base_address = generate_linearized_address(
                v_idx_global, dim_split_steps, caching_start_dim);
            generate_linearize_ref(&v_idx_global, dim_split_steps);
          }
          void *last_tmp_pntr = nullptr;
          if (!line_buffers.empty()) {
            last_tmp_pntr = m_ast->CreateArrayRef(
                m_ast->CreateVariableRef(*tmp_buf_init),
                vector<void *>{m_ast->CreateConst(line_buffers.size())});
          } else {
            last_tmp_pntr = m_ast->CreateVariableRef(*tmp_buf_init);
          }
          void *new_data_pntr = m_ast->CreateArrayRef(
              m_ast->CreateVariableRef(arr_init), v_idx_global);
          void *new_data_assign = m_ast->CreateStmt(
              V_SgAssignStatement, last_tmp_pntr, new_data_pntr);
          //  Bound checker, ensure that the access is within the array
          //  bound Need the checker because the loops are padded
          if (cond_exp != nullptr) {
            void *assign_bb = m_ast->CreateBasicBlock();
            m_ast->AppendChild(assign_bb, new_data_assign);
            new_data_assign = m_ast->CreateIfStmt(cond_exp, assign_bb, nullptr);
          }
          if (dim_loop != nullptr)
            m_ast->InsertAfterStmt(new_data_assign, dim_loop);
          else
            m_ast->AppendChild(insert_tmp, new_data_assign);
        }
      }
    }

    //  Step4.3: update the sliding window from the temp buffer
    //  The new data is stored in the temp buffer for each sliding window.
    //  Load the data from temp buffer after shifting the sliding window
    //  in the end of the loop.
    {
      int count = 0;
      vector<void *> v_idx1;
      vector<void *> v_idx_from1;
      void *bb_t = insert_bb;
      for (size_t t = 0; t < new_arr_size.size(); t++) {
        void *loop_t = stencil_loops[t];
        void *to_ref = nullptr;
        void *from_ref = nullptr;
        if (t != stencil_loops.size() - 1) {
          if (reuse_winds[i][t] != NOREUSE_WIND) {
            void *iter_init = m_ast->GetLoopIterator(loop_t);
            void *iter_type = m_ast->GetTypebyVar(iter_init);
            string iter_name = m_ast->GetVariableName(iter_init);
            m_ast->get_valid_local_name(func_decl, &iter_name);
            void *new_iter_decl =
                m_ast->CreateVariableDecl(iter_type, iter_name, nullptr,
                                          m_ast->TraceUpToScope(slide_loop));
            m_ast->PrependChild(insert_bb, new_iter_decl, true);
            void *new_iter_init =
                m_ast->GetVariableInitializedName(new_iter_decl);
            int64_t step = 1;
            void *lb_exp = nullptr;
            lb_exp = m_ast->CreateConst(0);

            void *new_loop_t = m_ast->CreateStmt(
                V_SgForStatement, new_iter_init, lb_exp,
                m_ast->CreateConst((int64_t)reuse_winds[i][t] - 1),
                m_ast->CreateBasicBlock(), &step, nullptr, slide_loop);
            if (count == 0) {
              m_ast->AppendChild(insert_bb, new_loop_t);
            } else {
              m_ast->AppendChild(bb_t, new_loop_t);
            }
            bb_t = m_ast->GetLoopBody(new_loop_t);
            count++;
            void *sg_pragma = m_ast->CreatePragma(new_parallel_pragma, bb_t);
            m_ast->PrependChild(bb_t, sg_pragma);
            to_ref =
                m_ast->CreateVariableRef(m_ast->GetLoopIterator(new_loop_t));
            from_ref =
                m_ast->CreateVariableRef(m_ast->GetLoopIterator(new_loop_t));
            v_idx1.push_back(to_ref);
            v_idx_from1.push_back(from_ref);
          }
        } else {
          to_ref = m_ast->CreateConst((int64_t)reuse_winds[i][t] - 1);
          v_idx1.push_back(to_ref);
        }
      }
      void *from_tmp_pntr = m_ast->CreateArrayRef(
          m_ast->CreateVariableRef(*tmp_buf_init), v_idx_from1);
      void *to_tmp_pntr =
          m_ast->CreateArrayRef(m_ast->CreateVariableRef(wind_init), v_idx1);
      void *new_assign =
          m_ast->CreateStmt(V_SgAssignStatement, to_tmp_pntr, from_tmp_pntr);
      if (bb_t != nullptr) {
        m_ast->AppendChild(bb_t, new_assign);
      }
    }
    slide_bb = insert_bb;
  }
  return slide_bb;
}

void LineBuffer::slide_line_buffer_and_load_new_data(
    void *arr_init, const vector<void *> &line_buffers,
    const vector<vector<size_t>> &reuse_winds, const vector<size_t> &cache_wind,
    void *slide_bb, const vector<void *> &stencil_loops,
    const vector<int64_t> &base_cache_address,
    const vector<size_t> &size_tmp_buf, int caching_start_dim,
    const vector<size_t> &new_arr_size, void *arr_base_type, void *slide_loop,
    void *func_decl, void *tmp_buf_init) {
  if (cache_wind.empty())
    return;
  string new_parallel_pragma =
      std::string(ACCEL_PRAGMA) + " " + CMOST_PARALLEL + " " + CMOST_complete;
  void *insert_bb1 = slide_bb;
  void *update_line_buffer_bb = m_ast->CreateBasicBlock();
  m_ast->AppendChild(insert_bb1, update_line_buffer_bb);
  insert_bb1 = update_line_buffer_bb;
  void *tmp_var_decl = m_ast->CreateVariableDecl(
      arr_base_type, "tmp_" + m_ast->_up(arr_init), nullptr, insert_bb1);
  m_ast->PrependChild(insert_bb1, tmp_var_decl, true);
  void *tmp_var_init = m_ast->GetVariableInitializedName(tmp_var_decl);
  //  Step6.1: update the line buffer
  //  Slide one layer of the line buffer
  {
    //   void *true_bb = nullptr;
    //   void *false_bb = m_ast->CreateBasicBlock();
    vector<void *> v_idx1, v_idx_tmp;
    void *bb_t = nullptr;
    void *dim_loop = nullptr;
    int count = 0;
    map<void *, void *> new_loops;
    for (size_t t = caching_start_dim; t < new_arr_size.size(); t++) {
      void *loop_t = stencil_loops[t];
      void *from_ref = nullptr;
      if (t == caching_start_dim || t >= stencil_loops.size()) {
        void *iter_t = m_ast->GetLoopIterator(loop_t);
        void *iter_type = m_ast->GetTypebyVar(iter_t);
        string iter_name = m_ast->GetVariableName(iter_t);
        m_ast->get_valid_local_name(func_decl, &iter_name);
        void *new_iter_decl = m_ast->CreateVariableDecl(
            iter_type, iter_name, nullptr, m_ast->TraceUpToScope(slide_loop));
        m_ast->PrependChild(insert_bb1, new_iter_decl, true);
        void *new_iter_init = m_ast->GetVariableInitializedName(new_iter_decl);
        int64_t step = 1;
        size_t loop_ub = 0;
        if (cache_wind.size() > t)
          loop_ub = cache_wind[t] - 1;
        void *new_loop_t = m_ast->CreateStmt(
            V_SgForStatement, new_iter_init, m_ast->CreateConst(0),
            m_ast->CreateConst(loop_ub), m_ast->CreateBasicBlock(), &step,
            nullptr, slide_loop);
        new_loops[loop_t] = new_iter_init;
        if (count == 0) {
          m_ast->AppendChild(insert_bb1, new_loop_t);
        } else {
          m_ast->AppendChild(bb_t, new_loop_t);
        }
        bb_t = m_ast->GetLoopBody(new_loop_t);
        count++;
        void *sg_pragma = m_ast->CreatePragma(new_parallel_pragma, bb_t);
        m_ast->PrependChild(bb_t, sg_pragma);
        if (t == caching_start_dim) {
          dim_loop = new_loop_t;
          from_ref = m_ast->CreateExp(V_SgAddOp,
                                      m_ast->CreateVariableRef(new_iter_init),
                                      m_ast->CreateConst(1));
        } else {
          void *to_ref = nullptr;
          to_ref = m_ast->CreateVariableRef(new_iter_init);
          from_ref = m_ast->CreateVariableRef(new_iter_init);
          v_idx1.push_back(to_ref);
        }
        v_idx_tmp.push_back(from_ref);
      } else {
        void *to_ref = nullptr;
        to_ref = m_ast->CreateVariableRef(m_ast->GetLoopIterator(loop_t));
        to_ref = m_ast->CreateExp(V_SgSubtractOp, to_ref,
                                  m_ast->CreateConst(loop_lb_cache[loop_t]));
        if (t == stencil_loops.size() - 1) {
          to_ref = m_ast->CreateExp(
              V_SgAddOp, to_ref,
              m_ast->CreateConst((int64_t)reuse_winds[0][t] - 1));
        }
        if (t < stencil_loops.size() - 1 && t < size_tmp_buf.size()) {
          from_ref = m_ast->CreateConst(0);
          v_idx_tmp.push_back(from_ref);
        }
        v_idx1.push_back(to_ref);
      }
    }
    void *from_true_pntr = m_ast->CreateArrayRef(
        m_ast->CreateVariableRef(tmp_buf_init), v_idx_tmp);
    void *new_assign = m_ast->CreateStmt(V_SgAssignStatement,
                                         m_ast->CreateVariableRef(tmp_var_init),
                                         from_true_pntr);
    m_ast->AppendChild(bb_t, new_assign);
    void *from_tmp_pntr = m_ast->CreateVariableRef(tmp_var_init);
    if (bb_t != nullptr) {
      vector<void *> from_vectors;
      vector<void *> to_vectors;
      for (size_t i = 0; i < line_buffers.size(); i++) {
        void *tmp_op = m_ast->CopyExp(from_tmp_pntr);
        from_vectors.push_back(tmp_op);
        vector<void *> tmp_refs;
        for (auto ref : v_idx1) {
          tmp_refs.push_back(m_ast->CopyExp(ref));
        }
        void *new_cache_init = line_buffers[i];
        void *to_tmp_pntr = m_ast->CreateArrayRef(
            m_ast->CreateVariableRef(new_cache_init), tmp_refs);
        to_vectors.push_back(to_tmp_pntr);
      }
      generate_conditions(from_vectors, to_vectors, dim_loop, bb_t);
    }
  }
}

void LineBuffer::loop_shifting(const vector<void *> &stencil_loops,
                               const vector<int64_t> &base_cache_address,
                               int start_dim) {
  cout << "Start from dimension=" << start_dim << endl;
  for (int t = static_cast<int>(stencil_loops.size() - 1); t >= start_dim;
       t--) {
    void *loop_t = stencil_loops[t];
    if (loop_t == NOLOOP || base_cache_address[t] == 0)
      continue;
    cout << "Shifting: " << m_ast->_p(loop_t) << "," << base_cache_address[t]
         << endl;
    void *iter_t = m_ast->GetLoopIterator(loop_t);
    int64_t new_lb = loop_lb_cache[loop_t] - base_cache_address[t];
    loop_lb_cache[loop_t] = new_lb;
    void *init_stmt = m_ast->GetLoopInit(loop_t);
    void *sg_init_stmt =
        (static_cast<SgForInitStatement *>(init_stmt)->get_init_stmt()[0]);

    if (m_ast->IsExprStatement(sg_init_stmt) != 0) {
      void *sg_init_expr = m_ast->GetExprFromStmt(sg_init_stmt);
      void *new_init_expr =
          m_ast->CreateExp(V_SgAssignOp, m_ast->CreateVariableRef(iter_t),
                           m_ast->CreateConst(new_lb), 0);
      m_ast->ReplaceExp(sg_init_expr, new_init_expr);
    } else if (m_ast->IsVariableDecl(sg_init_stmt) != 0) {
      m_ast->SetInitializer(
          iter_t, m_ast->CreateInitializer(m_ast->CreateConst(new_lb)));
    }
    int64_t new_ub = loop_ub_cache[loop_t] - base_cache_address[t];
    loop_ub_cache[loop_t] = new_ub;
    void *cond_stmt = m_ast->GetLoopTest(loop_t);
    void *sg_cond_expr = m_ast->GetExprFromStmt(cond_stmt);
    void *new_test =
        m_ast->CreateExp(V_SgLessThanOp, m_ast->CreateVariableRef(iter_t),
                         m_ast->CreateConst(new_ub + 1), 0);
    m_ast->ReplaceExp(sg_cond_expr, new_test);
    vector<void *> vec_refs;
    m_ast->get_ref_in_scope(iter_t, m_ast->GetLoopBody(loop_t), &vec_refs);
    for (auto ref : vec_refs) {
      m_ast->ReplaceExp(
          ref, m_ast->CreateExp(V_SgAddOp, m_ast->CreateVariableRef(iter_t),
                                m_ast->CreateConst(base_cache_address[t])));
    }
  }
}

void LineBuffer::replace_references_with_sliding_windows(
    const std::map<int, int> &reuse_pntr2wind,
    const vector<void *> &index_copy2pntr, const vector<void *> &vec_winds,
    const map<void *, vector<map<void *, int64_t>>> &pntr_index_expr,
    const vector<int64_t> &base_cache_address,
    const vector<size_t> &new_arr_size, const vector<void *> &stencil_loops,
    int caching_start_dim) {
  map<void *, void *> replace_pntr;
  for (auto it : reuse_pntr2wind) {
    int seq = it.first;
    int wind_index = it.second;
    void *sg_pntr = index_copy2pntr[seq];
    // For unrolled references, we only update the original pntr
    if (replace_pntr.count(sg_pntr) > 0)
      continue;
    auto index_it = pntr_index_expr.find(sg_pntr);
    if (index_it == pntr_index_expr.end())
      continue;
    auto &index_expr = index_it->second;
    vector<void *> v_idx_from;
    for (size_t t = caching_start_dim; t < new_arr_size.size(); t++) {
      void *from_ref = nullptr;
      int64_t const_offset = 0;
      // for each dimension, drop stencil loop iterator then sum all the
      // remaining atoms, finally minus base cache address
      for (auto atom : index_expr[t]) {
        if (t < stencil_loops.size() && atom.first == stencil_loops[t])
          continue;
        void *new_atom = nullptr;
        if (atom.second != 0) {
          if (m_ast->IsForStatement(atom.first)) {
            new_atom =
                m_ast->CreateVariableRef(m_ast->GetLoopIterator(atom.first));
            if (atom.second != 1)
              new_atom = m_ast->CreateExp(V_SgMultiplyOp, new_atom,
                                          m_ast->CreateConst(atom.second));
          } else if (atom.first == CONST_ITER) {
            const_offset += atom.second;
          } else {
            new_atom = m_ast->CopyExp(atom.first);
            if (atom.second != 1)
              new_atom = m_ast->CreateExp(V_SgMultiplyOp, new_atom,
                                          m_ast->CreateConst(atom.second));
          }
        } else {
          continue;
        }
        if (new_atom != nullptr) {
          if (from_ref == nullptr) {
            from_ref = new_atom;
          } else {
            from_ref = m_ast->CreateExp(V_SgAddOp, new_atom, from_ref);
          }
        }
      }
      if (t < base_cache_address.size() && base_cache_address[t] != 0) {
        const_offset -= base_cache_address[t];
      }
      if (from_ref == nullptr || const_offset != 0) {
        void *offset = m_ast->CreateConst(const_offset);
        if (from_ref == nullptr)
          from_ref = offset;
        else
          from_ref = m_ast->CreateExp(V_SgAddOp, from_ref, offset);
      }
      v_idx_from.push_back(from_ref);
    }
    void *from_pntr = m_ast->CreateArrayRef(
        m_ast->CreateVariableRef(vec_winds[wind_index]), v_idx_from);
    replace_pntr[sg_pntr] = from_pntr;
  }
  for (auto pair : replace_pntr) {
    auto sg_pntr = pair.first;
    auto from_pntr = pair.second;
    m_ast->ReplaceExp(sg_pntr, from_pntr);
    cout << "Replaced: " << m_ast->_up(from_pntr) << endl;
  }
  return;
}

void LineBuffer::update_interface_pragma(void *if_pragma, void *arr_init,
                                         void *kernel) {
  if (if_pragma == nullptr) {
    string pragma_str = "ACCEL interface";
    void *scope = m_ast->GetFuncBody(kernel);
    void *new_pragma = m_ast->CreatePragma(pragma_str, scope);
    m_ast->PrependChild(scope, new_pragma);
    if_pragma = new_pragma;
    mMars_ir_v2.set_pragma_attribute(&if_pragma, CMOST_variable,
                                     m_ast->GetVariableName(arr_init));
  }
  mMars_ir_v2.set_pragma_attribute(&if_pragma, CMOST_burst_off, "");
}
