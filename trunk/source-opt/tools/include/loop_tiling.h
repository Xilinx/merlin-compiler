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


#ifndef TRUNK_SOURCE_OPT_TOOLS_INCLUDE_LOOP_TILING_H_
#define TRUNK_SOURCE_OPT_TOOLS_INCLUDE_LOOP_TILING_H_
#include "mars_ir.h"

int loop_tiling_top(CSageCodeGen *codegen, void *pTopFunc,
                    const CInputOptions &options);
int loop_tiling_top(CSageCodeGen *codegen, void *pTopFunc,
                    const CInputOptions &options, bool pragma_in_loop);

void collect_tripcount_pragma(CSageCodeGen *codegen, CMarsIr *mars_ir,
                              void *scope, void **sg_bound_pragma,
                              int64_t *trip_count);
#endif  // TRUNK_SOURCE_OPT_TOOLS_INCLUDE_LOOP_TILING_H_
