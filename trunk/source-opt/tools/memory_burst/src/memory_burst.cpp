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

#include "math.h"
#include "codegen.h"
#include "mars_ir.h"
#include "mars_opt.h"

#include "mars_message.h"
#include "program_analysis.h"
#define CHECK_BURST_LENGTH_IS_LOOP_INVARIANT

using MarsProgramAnalysis::CMarsAccess;
using MarsProgramAnalysis::CMarsExpression;
using MarsProgramAnalysis::CMarsRangeExpr;
using std::list;
using std::map;
using std::pair;
using std::set;
using std::vector;

extern vector<void *> get_top_kernels(CSageCodeGen *codegen, void *pTopFunc);

static bool get_range_definite_length_upper_bound(const CMarsRangeExpr &range,
                                                  void *loop,
                                                  int64_t *buffer_size) {
  vector<CMarsExpression> lower = range.get_lb_set();
  vector<CMarsExpression> upper = range.get_ub_set();
  if (lower.size() != 1 || upper.size() != 1) {
    return false;
  }
  CMarsExpression len = upper[0] - lower[0] + 1;
  CMarsExpression start_index = lower[0];
  if (len.IsConstant() != 0) {
    *buffer_size = len.GetConstant();
    if (*buffer_size == 1) {
      CMarsExpression index = lower[0] + 1;
      if (!index.IsLoopInvariant(loop)) {
        CMarsRangeExpr len_range = index.get_range();
        if (len_range.is_const_ub() != 0) {
          *buffer_size = len_range.get_const_ub();
        } else {
          return false;
        }
      }
    } else if (!start_index.IsLoopInvariant(loop)) {
      return false;
    }
    return *buffer_size != CMarsRangeExpr::MARS_INT64_MAX;
  }
  if (start_index.IsLoopInvariant(loop)) {
    CMarsRangeExpr len_range = len.get_range();

    //  ZP: 2016-02-29
    //  FIXME: currently use project scope for the range propogation,
    //        to be changed to kernel scope later
    {
      len_range.refine_range_in_scope(len_range.get_ast()->GetProject(), loop);
    }

    if (len_range.is_const_ub() != 0) {
      *buffer_size = len_range.get_const_ub();
      return *buffer_size != CMarsRangeExpr::MARS_INT64_MAX;
    }
  }
  return false;
}

int memory_burst_top(CSageCodeGen *codegen, void *pTopFunc,
                     const CInputOptions &options) {
  MemoryBurst instance(codegen, pTopFunc, options, true);
  instance.run();
  instance.clean_tiling_tag_and_cache();
  return 0;
}

int auto_coalescing_preproces_top(CSageCodeGen *codegen, void *pTopFunc,
                                  const CInputOptions &options) {
  MemoryBurst instance(codegen, pTopFunc, options, false);
  instance.auto_cache_for_coalescing();
  return 0;
}

void MemoryBurst::set_arg_opt_bitwidth(void *arg, int opt_bitwidth) {
  assert(mArgOptBitwidth.count(arg) <= 0 && "duplicated setting");
  mArgOptBitwidth[arg] = opt_bitwidth;
}

int MemoryBurst::get_arg_opt_bitwidth(void *arg) const {
  if (mArgOptBitwidth.find(arg) == mArgOptBitwidth.end()) {
    return 0;
  }
  return mArgOptBitwidth.find(arg)->second;
}

void MemoryBurst::init() {
  //  add by Youxiang 2015-11-20
  //  the threshold for the total buffer size is 3M byte
  mSizeofBRAM = 3 * 1024 * 1024;
  string size_of_bram_str =
      mOptions.get_option_key_value("-a", "burst_total_size_threshold");
  if (!size_of_bram_str.empty()) {
    mSizeofBRAM = my_atoi(size_of_bram_str);
  }
  //  the threshold for the single buffer size is 1M byte
  mSingleBufferSize = SINGLE_BUFFER_SIZE_THRESHOLD;
  string max_size_of_buffer_str =
      mOptions.get_option_key_value("-a", "burst_single_size_threshold");
  if (!max_size_of_buffer_str.empty()) {
    mSingleBufferSize = my_atoi(max_size_of_buffer_str);
  }
  //  the lifting threshold, after reaching lifting threshold
  //  we do not need to lifting
  mLiftingThreshold = 64 * 1024;
  string threshold_burst_lifting =
      mOptions.get_option_key_value("-a", "burst_lifting_size_threshold");
  if (!threshold_burst_lifting.empty()) {
    mLiftingThreshold = my_atoi(threshold_burst_lifting);
  }
  string len_threshold_str =
      mOptions.get_option_key_value("-a", "bus_length_threshold");
  if (!len_threshold_str.empty()) {
    m_wide_bus_length_lower_threshold = my_atoi(len_threshold_str);
  }
  mLiftingRatio = 1.5;
#ifdef ENABLE_MEMCPY_INLINE
  mMemcpy_inline = false;
#endif
  m_current_total_usage = 0;
#ifdef PROJDEBUG
  cout << "\n Total BRAM size is " << mSizeofBRAM << " byte." << endl;
#endif
  if (!mOptions.get_option_key_value("-a", "naive_hls").empty()) {
    mNaive_tag = true;
#if PROJDEBUG
    cout << "[MARS-PARALLEL-MSG] Naive HLS mode.\n";
#endif
  }

  string effort_level = mOptions.get_option_key_value("-a", "effort");
  if (effort_level == "low") {
    mEffort = LOW;
  } else if (effort_level == "high") {
    mEffort = HIGH;
  } else if (effort_level == "standard") {
    mEffort = STANDARD;
  } else if (effort_level == "medium") {
    mEffort = MEDIUM;
  }

  string lift_fine_grained_cache =
      mOptions.get_option_key_value("-a", "lift_fine_grained_cache");
  if ("off" == lift_fine_grained_cache) {
    m_lift_fine_grained_cache = false;
  }
  if (mNaive_tag) {
    return;
  }

  //  build Mars IR
  mMars_ir.get_mars_ir(m_ast, mTopFunc, mOptions, true);
  //  compute the rest BRAM resource available
  vector<CMirNode *> kernels;
  set<CMirNode *> all_nodes;
  vector<CMirNode *> all_nodes_vec;
  mMars_ir.get_all_kernel_nodes(&kernels, &all_nodes, &all_nodes_vec);
  for (size_t j = 0; j != all_nodes_vec.size(); ++j) {
    CMirNode *node = all_nodes_vec[j];
    if (!node->is_function || (node->ref == nullptr)) {
      continue;
    }
    vector<void *> local_decl;
    m_ast->GetNodesByType(node->ref, "preorder", "SgVariableDeclaration",
                          &local_decl);
    for (size_t i = 0; i != local_decl.size(); ++i) {
      void *var_init = m_ast->GetVariableInitializedName(local_decl[i]);
      void *sg_type = m_ast->GetTypebyVar(var_init);
      void *initializer = m_ast->GetInitializer(var_init);
      if (initializer != nullptr) {
        sg_type = m_ast->GetTypeByExp(initializer);
      }
      if (m_ast->IsArrayType(sg_type) == 0) {
        continue;
      }
      void *base_type;
      vector<size_t> nSizes;
      m_ast->get_type_dimension(sg_type, &base_type, &nSizes, var_init);
      int element_size = m_ast->get_bitwidth_from_type(sg_type);
      if (element_size <= 0) {
        continue;
      }
      int num_dim = nSizes.size();
      if (num_dim <= 0) {
        continue;
      }
      size_t size = 1;
      size_t t;
      for (t = 0; t < nSizes.size(); t++) {
        assert((int64_t)nSizes[t] >= 0);
        size *= nSizes[t];
      }
      size_t size_in_byte = element_size / 8 * size;
      if (m_current_total_usage <
          std::numeric_limits<size_t>::max() - size_in_byte) {
        m_current_total_usage += size_in_byte;
      } else {
        m_current_total_usage = std::numeric_limits<size_t>::max();
      }
    }
    if (mSizeofBRAM < m_current_total_usage) {
      if (!m_report_out_of_resource) {
        m_report_out_of_resource = true;
        dump_critical_message(BURST_WARNING_20);
      }
    }
  }
}

bool MemoryBurst::run() {
  if (mNaive_tag) {
    return false;
  }
  cout << "==============================================" << std::endl;
  cout << "-----=# Memory Burst Optimization Start#=----\n";
  cout << "==============================================" << std::endl
       << std::endl;

  //  build Mars IR v2
  mMars_ir_v2.build_mars_ir(m_ast, mTopFunc, false, false);

  //  Bug 1585
  detect_user_burst();

  //  1. first try to infer coarse grained burst
  process_coarse_grained_top();

  //  2. secondly try to infer fine grained burst
  //  build Mars IR
  mMars_ir.update_loop_node_info(m_ast);
  vector<CMirNode *> vec_nodes;
  mMars_ir.get_topological_order_nodes(&vec_nodes);
  for (auto index = vec_nodes.size(); index > 0; --index) {
    CMirNode *new_node = vec_nodes[index - 1];
    if (new_node->is_function) {
#if PROJDEBUG
      cout << "[MARS-BURST-MSG] Current analysis will skip function body "
              "or any function call in the loop."
           << endl;
#endif
      continue;
    }
    CMirNode *fNode = new_node->get_func();
    if (m_ast->UnSupportedMemberFunction(
            fNode->ref->get_parent()->get_parent())) {
      continue;
    }
    //  FIXME: wont burst for while
    if (new_node->is_while) {
      continue;
    }
    //  //  This is from an innermost loop.
    //  if (!new_node->has_opt_pragmas())
    //  continue;
    //  Burst inference applys only when
    //  pipeline/parallel/pipeline_parallel pragma inserted
    int parallel_tag = 0;
    if (new_node->has_parallel() != 0) {
      parallel_tag = 1;
    }
    int parent_pipeline_t = 0;
    CMirNode *parent_node = new_node->get_parent();
    if (parent_node != nullptr) {
      if (parent_node->has_pipeline() != 0) {
#if PROJDEBUG
        cout << "parent pipeline found" << endl;
#endif
        parent_pipeline_t = 1;
      }
    }

    if (insert_burst(new_node, fNode, index, parallel_tag, parent_pipeline_t)) {
#if PROJDEBUG
      cout << "parent pipeline found" << endl;
#endif
    }
  }

  //  add offset to work around vendor tool limits
  add_offset();
  std::cout << "============================================" << std::endl;
  std::cout << "-----=# Memory Burst Optimization End#=----\n";
  std::cout << "============================================" << std::endl
            << std::endl;
  return true;
}

void MemoryBurst::add_header(void *pos) {
  void *Global = m_ast->GetGlobal(pos);
  if (mInsertedGlobal.count(Global) > 0) {
    return;
  }
  mInsertedGlobal.insert(Global);
  m_ast->AddHeader("#include <string.h> \n", Global);
}

bool MemoryBurst::insert_burst(CMirNode *bNode, CMirNode *fNode, int node_num,
                               int parallel_tag, int parent_pipeline_t) {
  //  1. Get all the initialized names of the interface arrays of the current
  //  scope
  void *func = m_ast->TraceUpToFuncDecl(fNode->ref);
  void *curr_scope = bNode->ref;
  vector<void *> vec_refs;
  m_ast->GetNodesByType(curr_scope, "preorder", "SgVarRefExp", &vec_refs);
  set<void *> set_arrays;
  vector<void *> vec_arrays;
  for (auto ref : vec_refs) {
    void *var_init = m_ast->GetVariableInitializedName(ref);
    if (set_arrays.count(var_init) > 0) {
      continue;
    }
    set<void *> top_args;
    mMars_ir.trace_to_bus_ports(m_ast, func, var_init, &top_args, ref);
    for (auto arg : top_args) {
      if (!mMars_ir_v2.get_burst_flag(arg)) {
        map<string, string> msg_map;
        msg_map["memory_burst_flag"] = "off";
        msg_map["burst_flag"] =
            my_itoa(static_cast<int64_t>(mMars_ir_v2.get_burst_flag(arg)));
        insertMessage(m_ast, arg, msg_map);
      }
    }
    set_arrays.insert(var_init);
    if (is_burst_candidate(var_init, func, ref, false)) {
      vec_arrays.push_back(var_init);
    }
  }

  for (auto var : vec_arrays) {
    vector<size_t> dims;
    string var_name = m_ast->GetVariableName(var);

    void *type = m_ast->GetTypebyVar(var);
    void *base_type = nullptr;
    m_ast->get_type_dimension(type, &base_type, &dims, var);
    if (dims.size() > 1) {
      cout << "[MARS-BURST-WARN] Multidimensional pointer on the port is not "
              "supported for burst."
           << endl;
      continue;
    }

    vector<void *> vec_refs;
    m_ast->get_ref_in_scope(var, curr_scope, &vec_refs, true);
    if (vec_refs.empty()) {
#if PROJDEBUG
      cout << "[MARS-BURST-MSG] Array '" << var_name
           << "' don't need burst in this loop." << endl;
#endif
      continue;
    }

    int buffer_count = 0;
#if 0
      if (vec_refs.size() > 15) {
#if PROJDEBUG
        cout << "[MARS-BURST-WARN] Too many references to array '" << p->first
             << "' is found in the loop. It might consume lots of BRAM "
                "resources. Tips: try manually applying data reuse."
             << endl;
#endif
        continue;
      }
#endif
    bool is_write_depend = false;
    vector<pair<void *, void *>> vec_supported_ref_pntrs;
    for (auto ref : vec_refs) {
      void *pntr = nullptr;
      void *array = nullptr;
      vector<void *> indices;
      int pointer_dim;
      void *base_type = nullptr;
      int valid = m_ast->parse_standard_pntr_ref_from_array_ref(
          ref, &array, &pntr, &base_type, &indices, &pointer_dim, ref);
      if ((pntr == nullptr) || (m_ast->has_write_conservative(pntr) != 0)) {
        is_write_depend = true;
      }
      if ((valid != 0) && !isUnsupported(pntr)) {
        vec_supported_ref_pntrs.emplace_back(ref, pntr);
      }
    }
    if (vec_refs.size() > 1 && vec_supported_ref_pntrs.size() >= 1 &&
        is_write_depend) {
      cout << "[MARS-BURST-WARN] data dependence to array '" << var_name
           << "' is found in the loop. Tips: try manually applying data reuse."
           << endl;
      reportDataDependence(bNode, var);
      continue;
    }
    std::reverse(vec_supported_ref_pntrs.begin(),
                 vec_supported_ref_pntrs.end());
    for (auto ref_pntr : vec_supported_ref_pntrs) {
      void *pntr = ref_pntr.second;
      void *ref = ref_pntr.first;
#if PROJDEBUG
      if (m_ast->is_write_type(pntr) == 0) {
        cout << "[MARS-BURST-MSG] Read to array/pointer '" << var_name << "' ."
             << endl;
      } else if (m_ast->is_write_type(pntr) == 1) {
        cout << "[MARS-BURST-MSG] write to array/pointer '" << var_name << "' ."
             << endl;
      } else if (m_ast->is_write_type(pntr) == 2) {
        cout << "[MARS-BURST-MSG] read_write to array/pointer '" << var_name
             << "' ." << endl;
      }
#endif
      if (isUnsupportedAndReport(pntr) ||
          (m_ast->is_write_conservative(ref, false) != 0)) {
        string var_name = m_ast->GetVariableName(var);
        auto var_user_info = getUserCodeInfo(m_ast, var);
        if (!var_user_info.name.empty()) {
          var_name = var_user_info.name;
        }
        string var_info =
            "'" + var_name + "' " + getUserCodeFileLocation(m_ast, pntr, true);
        dump_critical_message(BURST_WARNING_7(var_info));
        continue;
      }

      if (m_ast->detect_user_for_loop_burst(ref, pntr, &mMars_ir)) {
        map<string, string> msg_map;
        msg_map["manual_memory_burst"] = "on";
        void *element_type = m_ast->GetBaseType(m_ast->GetTypebyVar(var), true);
        int element_size = m_ast->get_bitwidth_from_type(element_type);
        msg_map["manual_burst_memory_bitwidth"] = my_itoa(element_size);
        insertMessage(m_ast, var, msg_map);
#ifdef PROJDEBUG
        cout << "detect user manual memory burst " << m_ast->_p(pntr, 0)
             << endl;
#endif
        continue;
      }
      int64_t buffer_size = 0;
      int element_size = 0;
      void *scope = nullptr;
      bool intra_reuse = false;
      vector<void *> single(1, ref);
      if (burst_anal(bNode, fNode, var, single, &buffer_size, &element_size,
                     &scope, &intra_reuse, parent_pipeline_t)) {
        if (check_single_resource_limit(var, buffer_size, element_size)) {
          reportOutofSingleResourceLimit(var, m_ast->GetParent(bNode->ref),
                                         buffer_size * element_size);
          continue;
        }
        //  Generate memcpy for the new buffers
        if (!build_memcpy(bNode, fNode, single, var, buffer_size, scope,
                          node_num, buffer_count, element_size)) {
          continue;
        }
        buffer_count++;
        add_header(fNode->ref);
      }
    }
  }
  return true;
}

bool MemoryBurst::burst_anal(CMirNode *bNode, CMirNode *fNode, void *array,
                             const vector<void *> &refs, int64_t *buffer_size,
                             int *element_size, void **burst_scope,
                             bool *intra_reuse, int parent_pipeline_t,
                             int offset) {
  int burst_level = -1;
  int burst_gran = -1;

  vector<size_t> arr_size;
  void *array_type = m_ast->GetTypebyVar(array);
  void *basic_type;
  m_ast->get_type_dimension(array_type, &basic_type, &arr_size, array);
  void *element_type = m_ast->GetBaseType(basic_type, true);
  *element_size = m_ast->get_bitwidth_from_type(element_type) / 8;
  void *cached_scope = nullptr;
  void *curr_scope = m_ast->TraceUpToScope(refs[0]);
  while (curr_scope != nullptr) {
    if (nullptr == cached_scope) {
      if (m_cached_scope.count(curr_scope) > 0) {
        if (m_cached_scope[curr_scope].count(array) > 0) {
          cached_scope = curr_scope;
          break;
        }
      }
    }
    curr_scope = m_ast->TraceUpToScope(m_ast->GetParent(curr_scope));
  }
  if (cached_scope != nullptr) {
    *burst_scope = cached_scope;
  } else {
    burst_gran =
        get_granularity(bNode, refs, array, &burst_level, parent_pipeline_t);
    if (burst_gran < 0) {
      return false;
    }
    int burst_location =
        get_location(bNode, refs, array, &burst_gran, burst_level,
                     *element_size, offset, intra_reuse);

    if (burst_gran < 0) {
      return false;
    }
    *burst_scope = get_insert_loop(bNode, burst_location);
  }

  void *loop = *burst_scope;
  CMarsAccess access(refs[0], m_ast, loop);
  CMarsExpression index = access.GetIndex(0);
  int64_t new_buffer_size = 0;
  CMarsRangeExpr range = access.GetRangeExpr(0);
  bool has_upper_bound =
      get_range_definite_length_upper_bound(range, loop, &new_buffer_size);
  if (!has_upper_bound) {
    reportUnboundAccess(array, range, loop);
    cout << "[MARS-BURST-WARN] Cannot get the range of buffer size."
            "Mars memory burst is turned off. Tips: try to set the range "
            "of loop trip count. "
         << endl;
    return false;
  }
#ifdef CHECK_BURST_LENGTH_IS_LOOP_INVARIANT
  CMarsExpression len = range.get_ub(0) - range.get_lb(0);
  if (!len.IsLoopInvariant(loop)) {
    reportUnboundAccess(array, range, loop);
    return false;
  }
#if 0
  CMarsRangeExpr mr_len = len.get_range();
  if (void *pos = len.get_pos()) {
    mr_len.refine_range_in_scope(nullptr, pos);
  }
  if (!mr_len.IsNonNegative()) {
    reportPossibleNegativeAccessLength(bNode->ref, array, len);
    return false;
  }
#endif
#endif
  *buffer_size += new_buffer_size;
  *buffer_size += offset;
  if ((m_ast->IsArrayType(array_type) != 0) &&
      (*buffer_size <= 0 || static_cast<size_t>(*buffer_size) > arr_size[0])) {
#if PROJDEBUG
    cout << "[MARS-BURST-WARN] Inference buffer size is larger than the "
            "original array size."
         << endl;
#endif
    *buffer_size = arr_size[0];
  }

#if PROJDEBUG
  cout << "[MARS-BURST-PARSING] Burst inference buffer size of array '"
       << m_ast->GetVariableName(array) << "' is " << *buffer_size << endl;
#endif
  return true;
}

int MemoryBurst::get_granularity(CMirNode *bNode, const vector<void *> &bAccess,
                                 void *array, int *burst_level,
                                 int parent_pipeline_t) {
  //  might be a flattened  multidimensional array, for
  //  support 1 dimensional opencl port,
  //  for 1 dimensional array, loop level for granularity and location is the
  //  same.
  CMarsAccess access(bAccess[0], m_ast, bNode->ref);
  CMarsExpression index = access.GetIndex(0);
  int burst_gran = -1;
  int i = bNode->iterators_num - 1;
  CMirNode *scope = bNode;
  CMirNode *parent = scope->get_parent();
  //  start from outmost fine grained loop
  while ((parent != nullptr) && !parent->is_function && parent->is_fine_grain) {
    scope = parent;
    parent = parent->get_parent();
    --i;
  }
  //  stop the outmost tiled loop
  int outmost_level = 0;
  for (int j = 0; j <= i; ++j) {
    if (mMars_ir_v2.is_tiled_loop(bNode->loops_outside[j])) {
      outmost_level = j;
      break;
    }
  }

  for (; i >= outmost_level; i--) {
    if (!checkDataDependence(scope, bNode, bAccess.size() > 1, array) &&
        check_control_flow_availability(m_ast->GetParent(scope->ref)) &&
        (m_ast->IsInScope(array, scope->ref) == 0)) {
      burst_gran = i;
      *burst_level = i - 1;
#if PROJDEBUG
      cout << "[MARS-BURST-PARSING] Burst inference granularity of array '"
           << m_ast->GetVariableName(array) << "' is depends on loop iterator "
           << i << endl;
#endif
      break;
    }
    burst_gran = -1;
    return burst_gran;

    if (scope == nullptr) {
      return -1;
    }
    scope = scope->get_parent();
  }
  return burst_gran;
}

int MemoryBurst::get_location(CMirNode *bNode, const vector<void *> &bAccess,
                              void *array, int *gran, int burst_level,
                              int element_size, int offset, bool *intra_reuse) {
  int location = burst_level;  //  By default
  //  support 1 dimensional opencl port
  void *orig_loop = bNode->loops_outside[location + 1];

  CMarsAccess access(bAccess[0], m_ast, orig_loop);
  CMarsExpression index = access.GetIndex(0);
  CMarsRangeExpr orig_range = access.GetRangeExpr(0);
  int64_t orig_buffer_size = 1;
  int64_t last_buffer_size = 1;
  bool has_orig_buffer_size = get_range_definite_length_upper_bound(
      orig_range, orig_loop, &orig_buffer_size);
  if (has_orig_buffer_size) {
#ifdef CHECK_BURST_LENGTH_IS_LOOP_INVARIANT
    CMarsExpression len = orig_range.get_ub(0) - orig_range.get_lb(0);
    if (!len.IsLoopInvariant(orig_loop)) {
      has_orig_buffer_size = false;
    }
#endif
  }
  if (!has_orig_buffer_size) {
    reportUnboundAccess(array, orig_range, orig_loop);
    *gran = -1;

    return -1;
  }

  if (check_lifting_efficiency(MemoryAccessInfo(bAccess.size(), 1),
                               MemoryAccessInfo(1, orig_buffer_size + offset),
                               bAccess[0], orig_loop)) {
    *gran = -1;
  }

  int i;
  bool location_found = true;
  if (location < 0) {
    return location;
  }
  CMirNode *insertNode = bNode;
  {
    int i = bNode->iterators_num - 1;
    while (i > location) {
      if (insertNode == nullptr) {
        return location;
      }
      insertNode = insertNode->get_parent();
      --i;
    }
  }

  last_buffer_size = orig_buffer_size;
  void *last_loop = orig_loop;
  for (i = burst_level; i >= 0; i--) {
    void *loop = bNode->loops_outside[i];
    if (checkDataDependence(insertNode, bNode, !bAccess.empty(), array) ||
        (check_control_flow_availability(m_ast->GetParent(insertNode->ref)) ==
         0) ||
        (m_ast->IsInScope(array, insertNode->ref) != 0) ||
        (m_ast->IsInScope(loop, m_ast->TraceUpToScope(array)) == 0)) {
      location = i;
      location_found = false;
      break;
    }
    access.set_scope(loop);
    CMarsRangeExpr new_range = access.GetRangeExpr(0);
    int64_t new_buffer_size = 0;
    bool has_new_buffer_size = get_range_definite_length_upper_bound(
        new_range, loop, &new_buffer_size);
    if (has_new_buffer_size) {
#ifdef CHECK_BURST_LENGTH_IS_LOOP_INVARIANT
      CMarsExpression len = new_range.get_ub(0) - new_range.get_lb(0);
      if (!len.IsLoopInvariant(loop)) {
        has_new_buffer_size = false;
      }
#endif
    }
    if (!has_new_buffer_size ||
        check_lifting_resource_limit(array, loop, last_buffer_size + offset,
                                     new_buffer_size + offset, element_size)) {
      location = i;
      location_found = false;
      break;
    }
    //  enhance lifting model by considering lifting benefit and
    //  overhead
    if (!check_lifting_efficiency(
            MemoryAccessInfo(1, last_buffer_size + offset),
            MemoryAccessInfo(1, new_buffer_size + offset), last_loop, loop)) {
      last_buffer_size = new_buffer_size;
      *gran = i;
    } else {
      location = i;
      location_found = false;
    }

    insertNode = insertNode->get_parent();
    last_loop = loop;
  }
  if (location_found) {
    location = -1;
  }

  if (location < burst_level || (bNode->iterators_num > 1 && location == -1)) {
#if PROJDEBUG
    cout << "[MARS-BURST-MSG] Intra loop data reuse can be achieved by MARS "
            "burst inference."
         << endl;
#endif
    *intra_reuse = true;
  }

#if PROJDEBUG
  cout << "[MARS-BURST-PARSING] Burst inference location of array '"
       << m_ast->GetVariableName(array) << "' is " << location << ", lifted by "
       << burst_level - location << " level " << endl;
#endif

  return location;
}

bool MemoryBurst::build_memcpy(CMirNode *bNode, CMirNode *fNode,
                               const vector<void *> &bAccess, void *array,
                               int64_t buffer_size, void *burst_scope,
                               int node_num, int buffer_count,
                               int element_size) {
  void *insert_for = isSgForStatement(static_cast<SgNode *>(burst_scope));
  //  cout << bAccess.ref->unparseToString() << endl;

  //  Insert memcpy(), replace the original expressions
  void *array_ref = m_ast->CopyExp(bAccess[0]);
  CMarsAccess access(bAccess[0], m_ast, burst_scope);
  CMarsExpression index = access.GetIndex(0);
  CMarsRangeExpr range = access.GetRangeExpr(0);
  vector<CMarsExpression> lower = range.get_lb_set();
  vector<CMarsExpression> upper = range.get_ub_set();
  //  SgPntrArrRefExp *arr = isSgPntrArrRefExp(start_expr);
  assert(lower.size() == 1 && upper.size() == 1);

  CMarsExpression start_index = lower[0];
  CMarsExpression len = upper[0] - lower[0] + 1;
  if ((lower[0] == upper[0]) && !start_index.IsLoopInvariant(burst_scope)) {
    //  non-linear index access
    start_index = CMarsExpression(m_ast, 0);
    len = CMarsExpression(m_ast, buffer_size);
  }
  CMarsExpression me_burst_start = start_index;
  if (me_burst_start.any_var_located_in_scope(burst_scope) ||
      me_burst_start.is_movable_to(m_ast->GetFirstNonTrivalStmt(burst_scope)) ==
          0) {
    reportLocalBurstStartAddress(array, me_burst_start, burst_scope);
    return false;
  }
  CMarsExpression me_burst_len = len;
  bool is_non_negative_length = true;
  bool is_non_negative_start = true;
  void *burst_len = nullptr;
  void *start_expr = nullptr;
  refine_burst_start_and_length(
      me_burst_len, me_burst_start, burst_scope, burst_scope, array, true,
      &buffer_size, &burst_len, &start_expr, &is_non_negative_length,
      &is_non_negative_start);
  if (!is_non_negative_start) {
    reportPossibleNegativeAccessStart(me_burst_start, array, burst_scope);
  }
  if (buffer_size <= 0) {
    reportUnboundAccess(array, range, burst_scope);
    return false;
  }
  //  Generate new buffer declaration

  void *buff_decl = create_local_buffer(bNode, fNode, array, node_num,
                                        burst_scope, buffer_count, buffer_size);

  reportMessage(bNode, array, buffer_size, element_size);

  //  bool is_single_access =
  //    me_burst_len.IsConstant() && me_burst_len.get_const() == 1;
  void *burst_start_expr =
      m_ast->CreateExp(V_SgPntrArrRefExp, array_ref, start_expr);

  vector<void *> sg_read_init_list;
  vector<void *> sg_write_init_list;
  void *target_expr =
      m_ast->CreateExp(V_SgPntrArrRefExp, m_ast->CreateVariableRef(buff_decl),
                       m_ast->CreateConst(0));
  void *target_exp_addr = m_ast->CreateExp(V_SgAddressOfOp, target_expr);
  void *source_exp_addr = m_ast->CreateExp(V_SgAddressOfOp, burst_start_expr);

  void *sg_type = m_ast->GetBaseType(m_ast->GetTypebyVar(array), false);
  void *sizeof_expr = nullptr;
  sizeof_expr = m_ast->CreateExp(
      V_SgMultiplyOp, m_ast->CreateExp(V_SgSizeOfOp, sg_type), burst_len);
  //  return 0 for read, 1 for write, 2 for read_write
  bool read = false;
  bool write = false;
  bool is_partial_write = false;
  vector<void *> vec_pntr;
  for (size_t i = 0; i != bAccess.size(); ++i) {
    void *ref = bAccess[i];
    void *pntr = nullptr;
    void *array;
    void *base_type;
    vector<void *> indexes;
    int pointer_dim;
    m_ast->parse_standard_pntr_ref_from_array_ref(
        ref, &array, &pntr, &base_type, &indexes, &pointer_dim);
    vec_pntr.push_back(pntr);
    write |= m_ast->has_write_conservative(pntr);
    read |= m_ast->has_read_conservative(pntr);
    if (write) {
      is_partial_write |= is_partial_access(bAccess[i], burst_scope);
    }
  }
  read |= is_partial_write;
  void *read_func_stmt = nullptr;
  void *read_burst;
  void *write_burst;
  read_burst = write_burst = nullptr;
  if (read) {
    sg_read_init_list.push_back(m_ast->CreateCastExp(
        target_exp_addr, m_ast->GetTypeByString("void *")));
    sg_read_init_list.push_back(m_ast->CreateCastExp(
        source_exp_addr, m_ast->GetTypeByString("const void *")));
    sg_read_init_list.push_back(sizeof_expr);
    void *sg_read_func_call = m_ast->CreateFuncCall(
        "memcpy", m_ast->GetTypeByString("void"), sg_read_init_list, fNode->ref,
        get_top_arg(array));
    read_burst = sg_read_func_call;
    read_func_stmt = m_ast->CreateStmt(V_SgExprStatement, sg_read_func_call);
    if (!is_non_negative_length) {
      void *cond_expr = m_ast->CreateExp(V_SgGreaterOrEqualOp,
                                         m_ast->CreateSignnesCast(burst_len),
                                         m_ast->CreateConst(0));
      void *true_body = m_ast->CreateBasicBlock();
      m_ast->AppendChild(true_body, read_func_stmt);
      read_func_stmt = m_ast->CreateIfStmt(cond_expr, true_body, nullptr);
    }
  }
  void *write_func_stmt = nullptr;
  if (write) {
    sg_write_init_list.push_back(m_ast->CreateCastExp(
        source_exp_addr, m_ast->GetTypeByString("void *")));
    sg_write_init_list.push_back(m_ast->CreateCastExp(
        target_exp_addr, m_ast->GetTypeByString("const void *")));
    sg_write_init_list.push_back(sizeof_expr);
    void *sg_write_func_call = m_ast->CreateFuncCall(
        "memcpy", m_ast->GetTypeByString("void"), sg_write_init_list,
        fNode->ref, get_top_arg(array));
    write_burst = sg_write_func_call;
    write_func_stmt = m_ast->CreateStmt(V_SgExprStatement, sg_write_func_call);
    if (!is_non_negative_length) {
      void *cond_expr =
          m_ast->CreateExp(V_SgGreaterOrEqualOp,
                           m_ast->CreateSignnesCast(
                               read ? m_ast->CopyExp(burst_len) : burst_len),
                           m_ast->CreateConst(0));
      void *true_body = m_ast->CreateBasicBlock();
      m_ast->AppendChild(true_body, write_func_stmt);
      write_func_stmt = m_ast->CreateIfStmt(cond_expr, true_body, nullptr);
    }
  }

  const std::pair<void *, void *> block_key(array, burst_scope);
  if (read) {
    void *block = nullptr;
    if (mMemcpyBlockBefore.count(block_key) <= 0) {
      block = m_ast->CreateBasicBlock();
      if (insert_for != nullptr) {
        m_ast->InsertStmt(block, update_position(insert_for));
      } else {
        m_ast->InsertAfterStmt(block, buff_decl);
      }
      mMemcpyBlockBefore[block_key] = block;
    } else {
      block = mMemcpyBlockBefore[block_key];
    }
    m_ast->AppendChild(block, read_func_stmt);
    if (read_burst != nullptr) {
      map<string, string> msg_map;
      msg_map["display_name"] = "auto memory burst for '" +
                                m_ast->GetVariableName(get_top_arg(array)) +
                                "'";
      msg_map["burst"] = "on";
      msg_map["is_read"] = "1";
      msg_map["burst_length"] =
          CMarsExpression(sizeof_expr, m_ast, burst_scope).print_s();
      msg_map["burst_length_max"] = my_itoa(buffer_size * element_size);
      insertMessage(m_ast, read_burst, msg_map);
      setNoHierarchy(m_ast, read_burst, 1);
    }
  }

  if (write) {
    void *block = nullptr;
    if (mMemcpyBlockAfter.count(block_key) <= 0) {
      block = m_ast->CreateBasicBlock();
      if (insert_for != nullptr) {
        m_ast->InsertAfterStmt(block, insert_for);
      } else {
        m_ast->AppendChild(burst_scope, block);
      }
      mMemcpyBlockAfter[block_key] = block;
    } else {
      block = mMemcpyBlockAfter[block_key];
    }
    m_ast->AppendChild(block, write_func_stmt);
    if (write_burst != nullptr) {
      map<string, string> msg_map;
      msg_map["display_name"] = "auto memory burst for '" +
                                m_ast->GetVariableName(get_top_arg(array)) +
                                "'";
      msg_map["burst"] = "on";
      msg_map["is_write"] = "1";
      msg_map["burst_length"] =
          CMarsExpression(sizeof_expr, m_ast, burst_scope).print_s();
      msg_map["burst_length_max"] = my_itoa(buffer_size * element_size);
      insertMessage(m_ast, write_burst, msg_map);
      setNoHierarchy(m_ast, write_burst, 1);
    }
  }
  vector<void *> new_indexes;
  for (size_t i = 0; i != bAccess.size(); ++i) {
    CMarsAccess one(bAccess[i], m_ast, bNode->ref);
    CMarsExpression orig_index = one.GetIndex(0);
    CMarsExpression me_offset(start_expr, m_ast, orig_index.get_pos(),
                              bNode->ref);
    CMarsExpression new_index = orig_index - me_offset;
    void *new_index_expr = new_index.get_expr_from_coeff(true);
    new_indexes.push_back(new_index_expr);
  }
  for (size_t i = 0; i != bAccess.size(); ++i) {
    void *newAccess = m_ast->CreateVariableRef(buff_decl);
    void *newAccess_expr =
        m_ast->CreateExp(V_SgPntrArrRefExp, newAccess, new_indexes[i]);
    m_ast->ReplaceExp(vec_pntr[i], newAccess_expr);
  }

#if PROJDEBUG
  //  Important: to test the correctness of the AST after code generation
  cout << fNode->ref->unparseToString() << endl;
#endif
  return true;
}

//  void MemoryBurst::insert_flatten(CMirNode *bNode, string tag_str) {
//  string pragma_str = VIVADO_str + " loop_flatten " + tag_str;
//  SgPragmaDeclaration *new_pragma =
//      buildPragmaDeclaration(pragma_str, bNode->ref);
//  prependStatement(new_pragma, bNode->ref);
//  }

bool MemoryBurst::is_partial_access(void *ref, void *loop) {
  cout << "check whether is partial access \'" << m_ast->_up(ref) << "\' "
       << m_ast->get_location(ref, true, true) << endl;
  void *parent = m_ast->GetParent(ref);
  while (parent != loop) {
    if (m_ast->IsIfStatement(parent) != 0) {
      return true;
    }
    parent = m_ast->GetParent(parent);
  }
  CMarsAccess access(ref, m_ast, loop);
  CMarsExpression index = access.GetIndexes()[0];
  void *index_expr = index.get_expr_from_coeff();
  if (!m_ast->IsContinuousAccess(index_expr, loop, ref)) {
    m_ast->DeleteNode(index_expr);
    return true;
  }
  { m_ast->DeleteNode(index_expr); }
  map<void *, int64_t> iterators;
  map<void *, int64_t> invariants;
  if (!index.IsStandardForm(loop, &iterators, &invariants)) {
    return true;
  }
  for (map<void *, int64_t>::iterator it = iterators.begin();
       it != iterators.end(); ++it) {
    void *for_loop = it->first;
    void *iv;
    void *lb;
    void *ub;
    void *step = nullptr;
    void *body;
    bool inc_dir;
    bool inclusive_bound;
    if (m_ast->IsCanonicalForLoop(for_loop, &iv, &lb, &ub, &step, &body,
                                  &inc_dir, &inclusive_bound) != 0) {
      int64_t nStep = 0;
      int ret = m_ast->GetLoopStepValueFromExpr(step, &nStep);
      if ((step != nullptr) && (m_ast->GetParent(step) == nullptr)) {
        m_ast->DeleteNode(step);
      }
      if ((ret == 0) || nStep != 1) {
        return true;
      }
    } else {
      if ((step != nullptr) && (m_ast->GetParent(step) == nullptr)) {
        m_ast->DeleteNode(step);
      }
      return true;
    }
  }
  return false;
}

bool MemoryBurst::checkDataDependence(CMirNode *scope, CMirNode *lnode,
                                      bool group, void *array) {
  int access_num = 0;
  int rest_access_num = 0;
  bool write_dep = false;
  vector<void *> vec_refs;
  m_ast->get_ref_in_scope(array, scope->ref, &vec_refs, true);
  if (static_cast<unsigned int>(!vec_refs.empty()) != 0U) {
    access_num += vec_refs.size();
    if (scope != lnode) {
      rest_access_num += vec_refs.size();
    }
    for (auto ref : vec_refs) {
      write_dep |= m_ast->has_write_conservative(ref);
    }
  }
#if 0
  vector<CMirNode *> child_nodes = scope->get_all_child(true);
  for (int i = 0; i != child_nodes.size(); ++i) {
    CMirNode *child_node = child_nodes[i];
    vector<void*> vec_refs;
    m_ast->get_ref_in_scope(array, child_node->ref, &vec_refs);
    if (vec_refs.size()) {
      access_num += vec_refs.size();
      if (child_node != lnode)
        rest_access_num += vec_refs.size();
      for (auto ref : vec_refs)
        write_dep |= m_ast->has_write_conservative(ref);
    }
  }
#endif
  if (!group) {
    if (access_num > 1 && write_dep) {
      return true;
    }
  } else {
    if (rest_access_num > 0 && write_dep) {
      return true;
    }
  }
  return false;
}

void *MemoryBurst::create_local_buffer(CMirNode *lNode, CMirNode *fNode,
                                       void *arr, int node_num,
                                       void *burst_scope, int buffer_count,
                                       int64_t buffer_size) {
  char temp[64];
  string bufname_str;

  snprintf(temp, sizeof(temp), "%d", node_num);
  bufname_str = m_ast->GetVariableName(arr) + "_" + temp;

  //  New buffer name
  snprintf(temp, sizeof(temp), "%d", buffer_count);
  bufname_str = bufname_str + "_" + temp + "_buf";

  void *scope = m_ast->TraceUpToBasicBlock(burst_scope);
  m_ast->get_valid_name(scope, &bufname_str);
  void *sg_type = m_ast->GetBaseType(m_ast->GetTypebyVar(arr), false);
  void *arr_type =
      m_ast->CreateArrayType(sg_type, vector<size_t>(1, buffer_size));

  void *new_buf =
      m_ast->CreateVariableDecl(arr_type, bufname_str, nullptr, scope, arr);
  //  m_ast->SetStatic(new_buf);
  m_ast->PrependChild(scope, new_buf, true);
  void *top_arg = get_top_arg(arr);
  if (mMars_ir_v2.get_coalescing_flag(top_arg)) {
    string coalescing_force_pragma_str =
        std::string(ACCEL_PRAGMA) + " " + CMOST_coalescing + " " +
        CMOST_variable + "=" + bufname_str + " " + CMOST_force + "=" + CMOST_ON;
    void *coalescing_force_pragma =
        m_ast->CreatePragma(coalescing_force_pragma_str, scope);
    m_ast->InsertStmt(coalescing_force_pragma, new_buf);
  }
  m_ast->PropagatePragma(arr, scope, bufname_str);
  return new_buf;
}

#if 0
bool MemoryBurst::check_memory_resource(void *array, void *scope,
                                        int64_t buffer_size,
                                        int element_size) {
#ifdef PROJDEBUG
  cout << "[MARS-BURST-MSG] BRAM resource available is " << mSizeofBRAM
       << " byte." << endl;
#endif

  if (!check_resource_limit(array, buffer_size, element_size)) {
#if PROJDEBUG
    cout << "[MARS-BURST-MSG] Rest of the BRAM resource is " << mSizeofBRAM
         << " byte." << endl;
#endif
    return true;
  } else {
    if (check_single_resource_limit(array, buffer_size, element_size))
      reportOutofSingleResourceLimit(array, scope, buffer_size * element_size);
    else
      reportOutofResourceLimit(array, scope, buffer_size * element_size);
#ifdef PROJDEBUG
    cout << "[MARS-BURST-WARN] BRAM resource for the usage of burst "
            "inference is used up. The limit is half of the "
            "resource "
            "by default."
         << endl;
#endif
    return false;
  }
}
#endif

size_t MemoryBurst::get_single_threshold(void *array, int element_size) {
  void *top_arg = get_top_arg(array);
  if (top_arg != nullptr) {
    size_t max_threshold = mMars_ir_v2.get_burst_max_size(top_arg);
    if (max_threshold != 0) {
      return max_threshold * element_size;
    }
  }
  return mSingleBufferSize;
}

bool MemoryBurst::check_single_resource_limit(void *array, int64_t buffer_size,
                                              int element_size) {
  return get_single_threshold(array, element_size) <
         static_cast<double>(buffer_size) * element_size;
}

bool MemoryBurst::check_whole_resource_limit(int64_t buffer_size,
                                             int element_size) {
  bool out_of_resource =
      mSizeofBRAM <
      m_current_total_usage + static_cast<double>(buffer_size) * element_size;
  if (out_of_resource) {
    bool has_report = m_report_out_of_resource;
    m_report_out_of_resource = true;
    return !has_report;
  }
  return false;
}

#if 0
bool MemoryBurst::check_resource_limit(void *array, int64_t orig_buffer_size,
                                       int element_size) {
  if (orig_buffer_size <= 0 ||
      check_single_resource_limit(array, orig_buffer_size, element_size) ||
      check_whole_resource_limit(orig_buffer_size, element_size))
    return true;
  return false;
}
#endif

bool MemoryBurst::check_lifting_resource_limit(void *array, void *scope,
                                               int64_t last_buffer_size,
                                               int64_t new_buffer_size,
                                               int element_size) {
  if (new_buffer_size <= 0) {
    return true;
  }
  string name = m_ast->GetVariableName(array);
  auto var_user_info = getUserCodeInfo(m_ast, array);
  if (!var_user_info.name.empty()) {
    name = var_user_info.name;
  }
  string var_info = "\'" + name + "\' (" + var_user_info.file_location + ")";
  string scope_info = getUserCodeFileLocation(m_ast, scope, true);
  bool has_tiled_loop = mMars_ir_v2.is_tiled_loop(scope);
  size_t single_buffer_size_threshold =
      get_single_threshold(array, element_size);
  size_t required_buffer_size = new_buffer_size * element_size;
  size_t lifting_threshold = mLiftingThreshold;
  if ((((new_buffer_size * 1.0L / last_buffer_size) > mLiftingRatio) &&
       (last_buffer_size * element_size > lifting_threshold))) {
    dump_critical_message(BURST_WARNING_16(var_info, scope_info,
                                           my_itoa(required_buffer_size),
                                           my_itoa(lifting_threshold)),
                          1);
    if (check_single_resource_limit(array, new_buffer_size, element_size)) {
      dump_critical_message(
          BURST_WARNING_18(var_info, scope_info, has_tiled_loop,
                           my_itoa(required_buffer_size),
                           my_itoa(single_buffer_size_threshold)),
          1);
    }
    return true;
  }
#if 0
  if (check_whole_resource_limit(new_buffer_size, element_size)) {
    dump_critical_message(BURST_WARNING_17(var_info),
        0, m_ast, get_top_arg(array));
    return true;
  }
#endif
  if (check_single_resource_limit(array, new_buffer_size, element_size)) {
    dump_critical_message(
        BURST_WARNING_18(var_info, scope_info, has_tiled_loop,
                         my_itoa(required_buffer_size),
                         my_itoa(single_buffer_size_threshold)),
        1);

    return true;
  }
  return false;
}

//  MemoryAccessInfo : <burst_number, burst_length>
bool MemoryBurst::check_lifting_efficiency(MemoryAccessInfo last_burst_info,
                                           MemoryAccessInfo new_burst_info,
                                           void *last_scope, void *new_scope) {
  if (last_burst_info.get_burst_number() ==
          std::numeric_limits<int64_t>::max() ||
      last_burst_info.get_burst_length() ==
          std::numeric_limits<int64_t>::max()) {
    return false;
  }
  if (new_burst_info.get_burst_number() ==
          std::numeric_limits<int64_t>::max() ||
      new_burst_info.get_burst_length() ==
          std::numeric_limits<int64_t>::max()) {
    return true;
  }
  int64_t total_trip_count = 0;
  if (last_scope == new_scope) {
    total_trip_count = 1;
  } else {
    list<t_func_call_path> vec_paths;
    int ret = m_ast->get_call_paths(last_scope, new_scope, &vec_paths);

    if ((ret == 0) || vec_paths.empty()) {
      vec_paths.clear();
      t_func_call_path path;
      vec_paths.push_back(path);
    }
    for (auto path : vec_paths) {
      int64_t trip_count = m_ast->get_average_loop_trip_count(
          m_ast->GetParent(last_scope), new_scope, path);
      if (trip_count == std::numeric_limits<int64_t>::max()) {
        total_trip_count = std::numeric_limits<int64_t>::max();
        break;
      }
      total_trip_count += trip_count;
    }
  }

  if (total_trip_count == std::numeric_limits<int64_t>::max()) {
    return false;
  }

  //  FIXME: 100 is the number of cycle of single access
  if (last_burst_info.get_burst_number() *
          (100 + (last_burst_info.get_burst_length() - 1)) * total_trip_count *
          1.0L * mLiftingRatio <
      (new_burst_info.get_burst_length() - 1 + 100) *
          new_burst_info.get_burst_number()) {
    //  stop lifting because of efficiency degradation
    return true;
  }

  return false;
}

void MemoryBurst::reportDataDependence(CMirNode *node, void *array) {
  void *ref = node->ref;
  void *loop = m_ast->GetParent(ref);
  string qorMergeMode = getQoRMergeMode(m_ast, loop);
  if (qorMergeMode == "tail")
    return;
  string name = m_ast->GetVariableName(array);
  auto array_user_info = getUserCodeInfo(m_ast, array);
  if (!array_user_info.name.empty())
    name = array_user_info.name;
  string sFile = m_ast->get_file(array);
  std::ostringstream oss;
  string loop_label = m_ast->get_internal_loop_label(loop);

  oss << "Cannot burst all the accesses of variable \'" << name << "\'"
      << " because of data dependence.\n";
  void *top_array = get_top_arg(array);
  map<string, string> msg_map;
  msg_map["burst"] = "off";
  msg_map["burst_off_reasons"] = "possible data dependence";
  insertMessage(m_ast, top_array, msg_map);
  string var_info = "\'" + name + "\' (" + array_user_info.file_location + ")";
  dump_critical_message(BURST_WARNING_3(var_info), 0, m_ast, top_array);
}

void MemoryBurst::reportUnboundAccess(void *array, const CMarsRangeExpr &range,
                                      void *scope) {
  // Only report for major loops
  string qorMergeMode = getQoRMergeMode(m_ast, scope);
  if (qorMergeMode == "tail")
    return;
  string name = m_ast->GetVariableName(array);
  auto array_user_info = getUserCodeInfo(m_ast, array);
  if (!array_user_info.name.empty())
    name = array_user_info.name;
  string sFile = m_ast->get_file(array);
  std::ostringstream oss;
  string var_info = "\'" + name + "\' (" + array_user_info.file_location + ")";
  string scope_info = getUserCodeFileLocation(m_ast, scope, true);
  string range_info = range.print_e();
  void *top_array = get_top_arg(array);
  map<string, string> msg_map;
  msg_map["burst"] = "off";
  msg_map["burst_off_reasons"] = "infinite bound";
  insertMessage(m_ast, top_array, msg_map);
  dump_critical_message(BURST_WARNING_1(var_info, range_info, scope_info), 0,
                        m_ast, top_array);
}

void MemoryBurst::reportLocalBurstStartAddress(
    void *array, const CMarsExpression &start_address, void *scope) {
  // Only report for major loops
  string qorMergeMode = getQoRMergeMode(m_ast, scope);
  if (qorMergeMode == "tail")
    return;
  string name = m_ast->GetVariableName(array);
  string sFile = m_ast->get_file(array);
  std::ostringstream oss;
  string var_info =
      "\'" + name + "\' " + getUserCodeFileLocation(m_ast, array, true);
  string scope_info = getUserCodeFileLocation(m_ast, scope, true);
  string start_address_info = start_address.print_s();
  void *top_array = get_top_arg(array);
  map<string, string> msg_map;
  msg_map["burst"] = "off";
  msg_map["burst_off_reasons"] = "start address is variable in the scope";
  insertMessage(m_ast, top_array, msg_map);
  dump_critical_message(
      BURST_WARNING_23(var_info, start_address_info, scope_info), 0, m_ast,
      top_array);
}

void MemoryBurst::reportOutofResourceLimit(void *array, void *scope,
                                           int64_t size_in_bytes) {
  string qormergemode = getQoRMergeMode(m_ast, scope);
  if (qormergemode == "tail")
    return;
  string name = m_ast->GetVariableName(array);
  auto array_user_info = getUserCodeInfo(m_ast, array);
  if (!array_user_info.name.empty())
    name = array_user_info.name;
  string sFile = m_ast->get_file(array);
  std::ostringstream oss;
  string var_info = "\'" + name + "\' (" + array_user_info.file_location + ")";
  string size_info = my_itoa(size_in_bytes);
  string scope_info = getUserCodeFileLocation(m_ast, scope, true);
  bool has_tiled_loop = mMars_ir_v2.is_tiled_loop(scope);
  void *top_array = get_top_arg(array);
  map<string, string> msg_map;
  msg_map["burst"] = "off";
  msg_map["burst_off_reasons"] = "out of total resource limitation";
  insertMessage(m_ast, top_array, msg_map);
  dump_critical_message(
      BURST_WARNING_2(var_info, scope_info, size_info, has_tiled_loop), 0,
      m_ast, top_array);
}

void MemoryBurst::reportOutofSingleResourceLimit(void *array, void *scope,
                                                 int64_t size_in_bytes) {
  string qormergemode = getQoRMergeMode(m_ast, scope);
  if (qormergemode == "tail")
    return;
  static set<pair<void *, void *>> reported_array_scope;
  auto array_scope = std::make_pair(array, scope);
  if (reported_array_scope.count(array_scope) > 0)
    return;
  void *curr_scope = scope;
  do {
    array_scope = std::make_pair(array, curr_scope);
    reported_array_scope.insert(array_scope);
    void *parent = m_ast->GetParent(curr_scope);
    if (m_ast->IsFunctionDeclaration(curr_scope) ||
        m_ast->IsFunctionDeclaration(parent))
      break;
    curr_scope = m_ast->TraceUpToScope(parent);
  } while (true);
  string name = m_ast->GetVariableName(array);
  auto array_user_info = getUserCodeInfo(m_ast, array);
  if (!array_user_info.name.empty())
    name = array_user_info.name;
  string sFile = m_ast->get_file(array);
  std::ostringstream oss;
  string var_info = "\'" + name + "\' (" + array_user_info.file_location + ")";
  string size_info = my_itoa(size_in_bytes);
  string scope_info = getUserCodeFileLocation(m_ast, scope, true);
  bool has_tiled_loop = mMars_ir_v2.is_tiled_loop(scope);
  void *top_array = get_top_arg(array);
  map<string, string> msg_map;
  msg_map["burst"] = "off";
  msg_map["burst_off_reasons"] = "out of single resource limitation";
  insertMessage(m_ast, top_array, msg_map);
  dump_critical_message(
      BURST_WARNING_5(var_info, scope_info, size_info, has_tiled_loop), 0,
      m_ast, top_array);
}

#if 0
void MemoryBurst::reportPossibleNegativeAccessLength(void *scope, void *array,
                                                 const CMarsExpression &len) {
  string name = m_ast->GetVariableName(array);
  string sFile = m_ast->get_file(array);
  std::ostringstream oss;

  string var_info =
      "\'" + name + "\' " + getUserCodeFileLocation(m_ast, array, true);
  string len_info = len.print_s();
  dump_critical_message(BURST_WARNING_4(var_info, len_info), 0, m_ast,
                        get_top_arg(array));
  return;
}
#endif

void MemoryBurst::reportMessage(CMirNode *node, void *array,
                                int64_t buffer_size, int element_bytes) {
  string name = m_ast->GetVariableName(array);
  string sFile = m_ast->get_file(array);
  void *top_array = get_top_arg(array);
  std::ostringstream oss;
  auto array_info = getUserCodeInfo(m_ast, array);
  if (!array_info.name.empty())
    name = array_info.name;
  string var_info;
  var_info = "\'" + name + "\' (" + array_info.file_location + ")";
  oss << "  burst buffer size = " << (buffer_size * element_bytes)
      << " bytes\n";
  map<string, string> msg_map;
  msg_map["burst"] = "on";
  insertMessage(m_ast, top_array, msg_map);
  void *scope = m_ast->GetParent(node->ref);
  string scope_info = getUserCodeFileLocation(m_ast, scope, true);
  int64_t size_in_bytes = buffer_size * element_bytes;
  string len_info = my_itoa(size_in_bytes);
  if (check_whole_resource_limit(buffer_size, element_bytes)) {
    reportOutofResourceLimit(array, scope, buffer_size * element_bytes);
  } else {
    dump_critical_message(BURST_INFO_1(var_info, scope_info, len_info));
  }
  if (static_cast<size_t>(size_in_bytes) <= m_length_threshold) {
    vector<size_t> dim_sizes = mMars_ir_v2.get_array_depth(top_array);
    size_t total_size = 1;
    for (auto size : dim_sizes) {
      total_size *= size;
    }
    bool is_full_access =
        total_size > 0 && (total_size * element_bytes == size_in_bytes);
    if (!is_full_access) {
      bool has_tiled_loop = mMars_ir_v2.is_tiled_loop(scope);
      string qormergemode = getQoRMergeMode(m_ast, scope);
      if (qormergemode != "tail") {
        dump_critical_message(BURST_WARNING_15(var_info, len_info, scope_info,
                                               has_tiled_loop, is_full_access),
                              0, m_ast, get_top_arg(array));
      }
    }
  }
  if (m_current_total_usage <
      std::numeric_limits<size_t>::max() - size_in_bytes) {
    m_current_total_usage += size_in_bytes;
  } else {
    m_current_total_usage = std::numeric_limits<size_t>::max();
  }
  mMars_ir_v2.clear_port_is_bus_cache();
  mMars_ir.clear_port_is_bus_cache();
}

void *MemoryBurst::get_insert_loop(CMirNode *bNode, int burst_location) {
  int i;
  int parent_level = 0;

  SgBasicBlock *ref_body = bNode->ref;
  SgForStatement *insert_for =
      SageInterface::getEnclosingNode<SgForStatement>(ref_body);

  if (burst_location >= 0) {
    parent_level = bNode->iterators_num - 1 - burst_location - 1;
  } else if (burst_location == -1) {
    parent_level = bNode->iterators_num - 1;
  }

  for (i = 0; i < parent_level; i++) {
    insert_for = SageInterface::getEnclosingNode<SgForStatement>(insert_for);
  }
  return insert_for;
}

bool MemoryBurst::isUnsupported(void *ref) {
  //  FIXME: we cannot support access in the sub function now
  if ((m_ast->IsAddressOfOp(ref) != 0) || (m_ast->IsVarRefExp(ref) != 0)) {
    return true;
  }
  void *type = m_ast->GetTypeByExp(ref);
  return (m_ast->IsPointerType(type) != 0) || (m_ast->IsArrayType(type) != 0);
}

bool MemoryBurst::isUnsupportedAndReport(void *ref) {
  if (m_ast->is_write_conservative(ref) == 0) {
    return false;
  }
  void *parent = m_ast->GetParent(ref);
  if (parent == nullptr) {
    return true;
  }
  if (m_ast->IsDotExp(parent) &&
      m_ast->IsFunctionCall(m_ast->GetParent(parent))) {
    parent = m_ast->GetParent(parent);
  }
  void *sg_left;
  void *sg_right;
  m_ast->GetExpOperand(parent, &sg_left, &sg_right);
  if ((m_ast->IsGeneralAssignOp(parent, &sg_left, &sg_right) != 0) &&
      ref == sg_left) {
    return false;
  }
  if ((m_ast->IsOverideAssignOp(parent) != 0) && ref == sg_left) {
    return false;
  }
  if ((m_ast->IsCompoundAssignOp(parent) != 0) && ref == sg_left) {
    return false;
  }
  if ((m_ast->IsPlusPlusOp(parent) != 0) && ref == sg_left) {
    return false;
  }
  if ((m_ast->IsMinusMinusOp(parent) != 0) && ref == sg_left) {
    return false;
  }
  int param_index = m_ast->GetFuncCallParamIndex(ref);
  if (-1 != param_index) {
    void *sg_call = m_ast->TraceUpToFuncCall(ref);
    assert(m_ast->IsFunctionCall(sg_call) ||
           m_ast->IsConstructorInitializer(sg_call));
    void *sg_type = nullptr;
    void *sg_decl = m_ast->GetFuncDeclByCall(sg_call, 0);
    void *sg_param = m_ast->GetFuncParam(sg_decl, param_index);
    //  FIXME: we need to do more check to get correct result for
    //  function with variable parameter lists
    if (sg_param == nullptr) {
      return true;
    }
    sg_type = m_ast->GetTypebyVar(sg_param);
    //  reference type has no partial access issue
    return m_ast->IsReferenceType(sg_type) == 0;
  }

  //  FIXME: can we support more complicate write access
  return true;
}

void *MemoryBurst::update_position(void *orig_pos) {
  //  BUG1558:
  void *scope = m_ast->GetParent(orig_pos);
  if (scope == nullptr) {
    return orig_pos;
  }
  size_t idx = m_ast->GetChildStmtIdx(scope, orig_pos);
  if (0 == idx || idx >= m_ast->GetChildStmtNum(scope)) {
    return orig_pos;
  }
  while (idx > 0) {
    --idx;
    void *curr_pos = m_ast->GetChildStmt(scope, idx);
    if ((m_ast->IsLabelStatement(curr_pos) != 0) ||
        (m_ast->IsPragmaDecl(curr_pos) != 0)) {
      continue;
    }
    ++idx;
    break;
  }
  return m_ast->GetChildStmt(scope, idx);
}

void *MemoryBurst::get_top_arg(void *var_init) {
  if (mArray2TopArg.count(var_init) > 0) {
    return mArray2TopArg[var_init];
  }
  return var_init;
}

void *MemoryBurst::create_offset_expr(const CMarsExpression &start_index,
                                      void **burst_len,
                                      bool *is_non_negative_length,
                                      bool *is_non_negative_start,
                                      int64_t *buff_size, bool refine_len,
                                      bool final_expr, void *pos) {
  void *tmp_start_index_expr = start_index.get_expr_from_coeff(final_expr);
  void *start_index_expr = tmp_start_index_expr;
  CMarsRangeExpr me_start = start_index.get_range();
  void *start_index_pos = start_index.get_pos();
  if (start_index_pos != nullptr) {
    me_start.refine_range_in_scope(nullptr, start_index_pos);
  }
  if (me_start.IsNonNegative() == 0) {
    if (start_index.IsConstant() != 0) {
      start_index_expr = m_ast->CreateConst(0);
      *burst_len = m_ast->CreateExp(V_SgAddOp, *burst_len,
                                    m_ast->CopyExp(tmp_start_index_expr));
      CMarsRangeExpr me_length =
          CMarsExpression(*burst_len, m_ast, pos).get_range();
      me_length.refine_range_in_scope(nullptr, pos);
      *is_non_negative_length &= me_length.IsNonNegative();
      *buff_size += start_index.GetConstant();
    } else {
      if (refine_len) {
        *is_non_negative_start = false;
        start_index_expr = m_ast->CreateConditionExp(
            m_ast->CreateExp(
                V_SgGreaterOrEqualOp,
                m_ast->CreateSignnesCast(m_ast->CopyExp(tmp_start_index_expr)),
                m_ast->CreateConst(0)),
            m_ast->CopyExp(tmp_start_index_expr), m_ast->CreateConst(0));
        *burst_len = m_ast->CreateConditionExp(
            m_ast->CreateExp(
                V_SgGreaterOrEqualOp,
                m_ast->CreateSignnesCast(m_ast->CopyExp(tmp_start_index_expr)),
                m_ast->CreateConst(0)),
            *burst_len,
            m_ast->CreateExp(V_SgAddOp, *burst_len,
                             m_ast->CopyExp(tmp_start_index_expr)));
        CMarsRangeExpr me_length =
            CMarsExpression(*burst_len, m_ast, pos).get_range();
        me_length.refine_range_in_scope(nullptr, pos);
        *is_non_negative_length &= me_length.IsNonNegative();
      }
    }
  }
  return start_index_expr;
}

void MemoryBurst::detect_user_burst() {
  vector<void *> kernels = get_top_kernels(m_ast, mTopFunc);
  set<void *> all_kernel_funcs;
  for (auto kernel : kernels) {
    SetVector<void *> sub_decls;
    SetVector<void *> sub_calls;
    m_ast->GetSubFuncDeclsRecursively(kernel, &sub_decls, &sub_calls);
    all_kernel_funcs.insert(kernel);
    all_kernel_funcs.insert(sub_decls.begin(), sub_decls.end());
  }

  vector<void *> all_memcpy_calls;
  for (auto func : all_kernel_funcs) {
    vector<void *> vec_calls;
    m_ast->GetNodesByType_int(func, "preorder", V_SgFunctionCallExp,
                              &vec_calls);
    for (auto call : vec_calls) {
      if (m_ast->IsMemCpy(call)) {
        all_memcpy_calls.push_back(call);
      }
    }
  }

  set<void *> set_ports;
  for (auto memcpy_call : all_memcpy_calls) {
    void *func = m_ast->TraceUpToFuncDecl(memcpy_call);
    void *length_exp;
    void *sg_array1;
    void *sg_pntr1;
    vector<void *> sg_index1;
    int pointer_dim1;
    void *sg_array2;
    void *sg_pntr2;
    vector<void *> sg_index2;
    int pointer_dim2;
    int ret = m_ast->parse_memcpy(
        memcpy_call, &length_exp, &sg_array1, &sg_pntr1, &sg_index1,
        &pointer_dim1, &sg_array2, &sg_pntr2, &sg_index2, &pointer_dim2);
    if (ret == 0) {
      continue;
    }
    CMarsExpression m_len(length_exp, m_ast, memcpy_call);
    size_t burst_len = 0;
    if (m_len.IsConstant() != 0) {
      burst_len = m_len.GetConstant();
    } else {
      CMarsRangeExpr mr_len = m_len.get_range();
      if (mr_len.is_const_ub()) {
        burst_len = mr_len.get_const_ub();
      }
    }
    vector<void *> vec_ports{sg_array1, sg_array2};
    for (size_t i = 0; i < vec_ports.size(); ++i) {
      void *sg_array = vec_ports[i];
      if (mMars_ir.every_trace_is_bus(m_ast, func, sg_array, memcpy_call)) {
        set<void *> top_args;
        mMars_ir.trace_to_bus_ports(m_ast, func, sg_array, &top_args,
                                    memcpy_call);
        for (auto arg : top_args) {
          map<string, string> msg_map;
          msg_map["display_name"] =
              "manual memory burst for '" + m_ast->GetVariableName(arg) + "'";
          msg_map["burst"] = "on";
          msg_map["burst_length"] = m_len.print_s();
          msg_map["burst_length_max"] = my_itoa(burst_len);
          if (i == 0) {
            msg_map["is_write"] = "1";
          } else {
            msg_map["is_read"] = "1";
          }
          insertMessage(m_ast, memcpy_call, msg_map);
          setNoHierarchy(m_ast, memcpy_call, 1);
          if (set_ports.count(arg) > 0) {
            continue;
          }
          set_ports.insert(arg);
          vector<size_t> arr_size;
          void *array_type = m_ast->GetTypebyVar(arg);
          void *basic_type;
          m_ast->get_type_dimension(array_type, &basic_type, &arr_size, arg);
          size_t element_bytes = m_ast->get_bitwidth_from_type(basic_type) / 8;
          auto arg_info = getUserCodeInfo(m_ast, arg);
          string arg_name = m_ast->GetVariableName(arg);
          if (!arg_info.name.empty())
            arg_name = arg_info.name;
          string var_info =
              "\'" + arg_name + "\' (" + arg_info.file_location + ")";
          size_t burst_len_in_bytes = burst_len * element_bytes;
          string len_info = my_itoa(burst_len_in_bytes);
          if (burst_len_in_bytes != 0U) {
            dump_critical_message(BURST_INFO_2(var_info, len_info));
          } else {
            dump_critical_message(BURST_INFO_3(var_info));
          }
          if (burst_len_in_bytes > 0 &&
              burst_len_in_bytes <= m_length_threshold) {
            string scope_info = getUserCodeFileLocation(
                m_ast, m_ast->TraceUpToScope(memcpy_call), true);
            vector<size_t> dim_sizes = mMars_ir_v2.get_array_depth(arg);
            size_t total_size = 1;
            for (auto dim : dim_sizes) {
              total_size *= dim;
            }
            bool is_full_access =
                total_size > 0 &&
                (total_size * element_bytes == burst_len_in_bytes);
            if (!is_full_access) {
              dump_critical_message(BURST_WARNING_15(var_info, len_info,
                                                     scope_info, false,
                                                     is_full_access),
                                    0, m_ast, arg);
            }
          }
        }
      }
    }
  }
}

void MemoryBurst::reportPossibleNegativeAccessStart(
    const CMarsExpression &me_start, void *sg_array, void *scope) {
  string qormergemode = getQoRMergeMode(m_ast, scope);
  if (qormergemode == "tail")
    return;
  string lower_bound_info = "(" + me_start.print_s() + ")";
  vector<void *> vec_refs;
  m_ast->get_ref_in_scope_recursive(sg_array, scope, &vec_refs, true);
  string var_name = m_ast->GetVariableName(sg_array);
  auto var_user_info = getUserCodeInfo(m_ast, sg_array);
  if (!var_user_info.name.empty())
    var_name = var_user_info.name;
  string var_info = "'" + var_name + "' (" + var_user_info.file_location + ")";
  string ref_info = "(";
  bool first = true;
  for (auto ref : vec_refs) {
    if (!first) {
      ref_info += ", ";
    }
    ref_info += getUserCodeFileLocation(m_ast, ref, false);
    first = false;
  }
  ref_info += ")";
  dump_critical_message(BURST_WARNING_19(lower_bound_info, var_info, ref_info),
                        0, m_ast, get_top_arg(sg_array));
}

void MemoryBurst::clean_tiling_tag_and_cache() {
  vector<void *> kernels = get_top_kernels(m_ast, mTopFunc);
  set<void *> all_kernel_funcs;
  for (auto kernel : kernels) {
    SetVector<void *> sub_decls;
    SetVector<void *> sub_calls;
    m_ast->GetSubFuncDeclsRecursively(kernel, &sub_decls, &sub_calls);
    all_kernel_funcs.insert(kernel);
    all_kernel_funcs.insert(sub_decls.begin(), sub_decls.end());
  }

  vector<void *> all_memcpy_calls;
  for (auto func : all_kernel_funcs) {
    vector<void *> vec_pragma;
    m_ast->GetNodesByType(func, "preorder", "SgPragmaDeclaration", &vec_pragma);
    for (auto pragma : vec_pragma) {
      CAnalPragma ana_pragma(m_ast);
      bool errorOut;
      if (ana_pragma.PragmasFrontendProcessing(pragma, &errorOut)) {
        if (ana_pragma.is_loop_tiled_tag() || ana_pragma.is_cache()) {
          if (ana_pragma.is_cache()) {
            m_ast->AddComment("Original: #pragma " +
                                  m_ast->GetPragmaString(pragma),
                              m_ast->TraceUpToBasicBlock(pragma));
          }
          m_ast->RemoveStmt(pragma);
        }
      }
    }
  }
}

//  Youxiang: 2019-03-29
//  MER969 MER976
//  Infer small cache in the innermost loop scope to enable coalescing
//  if we detected multiple accesses or access under the fine grained paralleled
//  loop
bool MemoryBurst::auto_cache_for_coalescing() {
  if (mNaive_tag) {
    return false;
  }
  cout << "==============================================" << std::endl;
  cout << "-----=# Memory Burst Optimization Start#=----\n";
  cout << "==============================================" << std::endl
       << std::endl;

  //  build Mars IR v2
  mMars_ir_v2.build_mars_ir(m_ast, mTopFunc, false, false);

  //  For xilinx, we use 16 x 512bits as the threshold
  mSingleBufferSize = 512 / 8 * 16;
  process_fine_grained_top();

  std::cout << "============================================" << std::endl;
  std::cout << "-----=# Memory Burst Optimization End#=----\n";
  std::cout << "============================================" << std::endl
            << std::endl;
  return true;
}
