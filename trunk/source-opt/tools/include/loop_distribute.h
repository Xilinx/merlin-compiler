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


#ifndef TRUNK_SOURCE_OPT_TOOLS_INCLUDE_LOOP_DISTRIBUTE_H_
#define TRUNK_SOURCE_OPT_TOOLS_INCLUDE_LOOP_DISTRIBUTE_H_
#include <set>
#include <vector>

#include "codegen.h"
#include "mars_ir_v2.h"

class LoopDistribute {
 private:
  CSageCodeGen &mCodegen;
  CMarsIrV2 mMars_ir;

 public:
  LoopDistribute(CSageCodeGen *codegen, void *pTopFunc,
                 const CInputOptions &option);
  bool run();

 private:
  bool process_all_scopes(const vector<void *> &top_loops);

  bool lift_local_variable_out_of_loop();

  bool lift_local_variable_out_of_loop(void *loop);

  bool distribute(const vector<void *> &top_loops);
};

class LoopFlatten {
 private:
  CSageCodeGen &mCodegen;
  CMarsIrV2 mMars_ir;
  CInputOptions &mOptions;

 public:
  LoopFlatten(CSageCodeGen *codegen, void *pTopFunc,
              const CInputOptions &option);
  bool run();

 private:
  bool flatten(const vector<void *> &top_loops,
               const set<void *> &unrolled_loops);

  int loop_flatten_nested_loops(void *sg_loop,
                                const set<void *> &not_touched_loops,
                                bool no_final_reset);
};
#endif  // TRUNK_SOURCE_OPT_TOOLS_INCLUDE_LOOP_DISTRIBUTE_H_
