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

#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <string>
#include <map>
#include <vector>
#include <utility>
#include <memory>
#include <stack>

#include "mars_opt.h"
#include "codegen.h"
#include "mars_ir.h"
#include "mars_ir_v2.h"
#include "analPragmas.h"

using std::cout;
using std::make_shared;
using std::map;
using std::priority_queue;
using std::shared_ptr;
using std::stack;
using std::string;
using std::unordered_map;
using std::unordered_set;
using std::vector;
typedef unordered_map<void *, int64_t> TripCountMap;
typedef unordered_map<void *, unordered_set<void *>> LoopGraph;
typedef map<void *, int64_t> LatencyMap;
// <array, vector<pair<reference, map<dimension, coefficent>>>>
typedef unordered_map<void *, vector<pair<void *, unordered_map<int, int64_t>>>>
    AccessMap;
enum PragmaEnum {
  NONE,
  PARALLEL,
  PIPELINE,
  REDUCTION,
  TILING,
  LINEBUFFER,
  TILED_TAG,
  PARTIAL_PARALLEL_TAG
};

struct LoopInfo {
  void *loop;
  void *parent_loop;
  int64_t orig_tc;
  int64_t range_lb;
  int64_t tc;
  int64_t par_tc;
  int64_t child_loop_count;
  int64_t orig_child_count;
  int64_t child_ops;
  int64_t body_lat;
  int64_t latency;
  int64_t unroll_factor;
  int64_t par_unroll_factor;
  // Ming: for tc estimation only, dont dp op at this loop
  bool estimation_bound;
  // Ming: issue warning, stop doing the any further insertion
  bool no_bound;
  bool invalid_bound;
  bool hier_pipeline_pg;  // pg -> pragma
  bool hier_parallel_pg;
  bool cur_pipeline_pg;
  bool cur_parallel_pg;
  bool cur_partial_unroll;
  bool cur_tiling;
  bool cur_linebuf;
  bool cur_reduction;
  bool tiled_sub_loop;
  bool partial_sub_loop;
  bool partial_parent_loop;
  AccessMap access_info;
  // TODO(Min) :Resource estimation
  explicit LoopInfo(void *loop_) : loop(loop_) {
    orig_tc = -1;
    tc = -1;
    range_lb = 0;
    parent_loop = nullptr;
    par_tc = 1;
    child_loop_count = 0;
    child_ops = 0;
    latency = -1;
    body_lat = 0;
    estimation_bound = false;
    no_bound = false;
    invalid_bound = false;
    hier_pipeline_pg = false;
    hier_parallel_pg = false;
    cur_pipeline_pg = false;
    cur_parallel_pg = false;
    cur_partial_unroll = false;
    cur_tiling = false;
    cur_linebuf = false;
    cur_reduction = false;
    tiled_sub_loop = false;
    partial_sub_loop = false;
    partial_parent_loop = false;
    unroll_factor = 1;
    par_unroll_factor = 1;
  }
};

class LoopInfoBuilder {
 public:
  LoopInfoBuilder(CSageCodeGen *codegen_, void *kernel_, CMarsIrV2 *mars_ir_v2_,
                  CMarsIr *mars_ir_)
      : codegen(codegen_), kernel(kernel_), mars_ir(mars_ir_) {}

  int run();
  //  void PrintLoopTripCount();
  vector<vector<void *>> paths;
  int64_t GetTripCount(void *loop);
  int64_t GetOrigTripCount(void *loop);
  int64_t GetSubLoopOps(void *loop);
  int64_t GetLoopBodyLatency(void *loop);
  void SetLoopBodyLatency(void *loop, int64_t lat);
  unordered_set<void *> GetTerminalLoops();
  void *LoopHasPragma(void *loop, PragmaEnum pragma_ty, bool hls, string add);
  AccessMap GetAccessInfo(void *loop);
  void RemoveTagPragmas();

  unordered_set<void *> terminal_loops;
  unordered_set<void *> top_loops;
  unordered_map<void *, LoopInfo *> loop_info_map;
  LoopGraph loop_graph;

 private:
  void ScanLoop();
  void TraverseLoops(void *loop);
  void BuildLoopInfo();
  void TopoSortLoops(void *loop, unordered_map<void *, bool> *p_visited,
                     vector<void *> *p_topo_vec);
  void TagLoopAttributes(const vector<void *> &topo_vec);
  void FilterNonTopLoop(vector<void *> *p_loops, void *top_loop);
  unordered_set<void *> GetLoopFrontier(void *fn);

  int64_t AnalyzeLoopTripCount(void *loop);
  void AnalyzeAccess(void *loop);
  int64_t GetPragmaFactor(void *pragma);

 private:
  CSageCodeGen *codegen;
  void *kernel;
  CMarsIr *mars_ir;
  vector<void *> to_remove_pragmas;
};

class LatencyEstimator {
 public:
  LatencyEstimator(CSageCodeGen *codegen_, void *kernel_,
                   shared_ptr<LoopInfoBuilder> loop_info_builder_)
      : codegen(codegen_), kernel(kernel_) {
    codegen->GetNodesByType(kernel, "preorder", "SgForStatement", &loops, false,
                            true);
    loop_info_builder = loop_info_builder_;
  }

  int64_t GetLoopLatency(void *loop, bool is_pipeline);
  int run();

 private:
  int64_t GetLoopII(void *loop);
  bool CanPipeline(void *loop);
  bool CanParallel(void *loop);

 private:
  CSageCodeGen *codegen;
  void *kernel;
  shared_ptr<LoopInfoBuilder> loop_info_builder;
  vector<void *> loops;
  LatencyMap loop_latency_info;
};

class ResourceEstimator {
 public:
  ResourceEstimator(CSageCodeGen *codegen_, void *kernel_,
                    shared_ptr<LoopInfoBuilder> loop_info_builder_)
      : codegen(codegen_) {
    loop_info_builder = loop_info_builder_;
  }

  bool IsHeavyMathFunction(void *scope);
  int64_t GetResourceUsage(void *loop, bool accumulate_parent);
  int64_t GetHeavyOps(void *loop);

 private:
  CSageCodeGen *codegen;
  // void *kernel;
  shared_ptr<LoopInfoBuilder> loop_info_builder;
  unordered_set<string> heavy_functions = {
      "exp",    "expf",   "expl",    "exp2",    "exp2f",  "exp2l",   "expm1",
      "expm1f", "expm1l", "log",     "logf",    "logl",   "log10",   "log10f",
      "log10l", "log2",   "log2f",   "log2l",   "log1p",  "log1pf",  "log1pl",
      "pow",    "powf",   "powl",    "sqrt",    "sqrtf",  "sqrtl",   "cbrt",
      "cbrtf",  "cbrtl",  "hypot",   "hypotf",  "hypotl", "sin",     "sinf",
      "sinl",   "cos",    "cosf",    "cosl",    "tan",    "tanf",    "tanl",
      "asin",   "asinf",  "asinl",   "acos",    "acosf",  "acosl",   "atan",
      "atanf",  "atanl",  "erf",     "erff",    "erfl",   "erfc",    "erfcf",
      "erfcl",  "tgamma", "tgammaf", "tgammal", "lgamma", "lgammaf", "lgammal"};
  unordered_map<void *, int64_t> heavy_op_cache;
};

class AutoParallel {
 public:
  AutoParallel(CSageCodeGen *codegen_, void *pTopFunc_,
               const CInputOptions &options_)
      : codegen(codegen_), pTopFunc(pTopFunc_), options(options_) {
    mars_ir_v2.build_mars_ir(codegen, pTopFunc);
    mars_ir.get_mars_ir(codegen, pTopFunc, options, true, false, true, true);
    if ("sdaccel" == options.get_option_key_value("-a", "impl_tool")) {
      mXilinx_flow = true;
    }
  }

  int run();

 private:
  void ScanCandidates();
  void ProcessingCriticalLoopsByLatency(int64_t total_lat);
  void ProcessingTrivialLoopsByPartition();
  bool InsertParallelPragma(void *loop);
  bool InsertPipelinePragma(void *loop);
  bool InsertPragma();
  bool UpdateArrayInfo(void *loop, int64_t parallel_factor);
  void SetLoopByArrayInfo(void *loop, int64_t loop_tc);

  bool insertPragmaBasedOnXilinxChannel(
      void *func, unordered_set<void *> *visited_funcs,
      std::unordered_set<void *> *processed_loops,
      std::stack<std::pair<void *, void *>> *call_stack);
  bool insertPragmaBasedOnXilinxChannel(void *kernel);
  bool insertParallelBasedOnXilinxChannelIndex(
      void *ref, std::unordered_set<void *> *processed_loops,
      std::stack<std::pair<void *, void *>> *call_stack);
  bool contains_channel_access(void *scope);
  bool remove_parallel_pipeline_off_pragma();
  bool check_range_function(void *loop);
  bool is_only_relate_to_loop(void *range_param, void *loop);
  bool is_apint_or_apfixed(void *funcalexp);

 private:
  CSageCodeGen *codegen;
  void *pTopFunc;
  CInputOptions options;
  CMarsIr mars_ir;
  CMarsIrV2 mars_ir_v2;
  bool mXilinx_flow;

  shared_ptr<LoopInfoBuilder> loop_info_builder;
  shared_ptr<LatencyEstimator> lty_estimator;
  shared_ptr<ResourceEstimator> res_estimator;

  priority_queue<pair<int64_t, void *>> parallel_frontiers;
  std::queue<void *> rest_frontiers;
  unordered_map<void *, PragmaEnum> loop_tags;
  unordered_map<void *, unordered_map<int, int64_t>> array_parallel_range;
  unordered_map<void *, unordered_map<int, int64_t>> array_parallelism;
  // <array, map<dimension, coefficent>>
  unordered_map<void *, unordered_map<void *, unordered_map<int, int64_t>>>
      access_parallel_tags;
  unordered_map<void *, bool> m_channel_access;
};
