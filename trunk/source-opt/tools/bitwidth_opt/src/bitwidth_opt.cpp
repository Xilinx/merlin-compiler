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


#include "bitwidth_opt.h"
#include <iostream>
#include <string>

//  #include "rose.h"
#include "mars_message.h"
#include "mars_opt.h"
#include "memory_coalescing_xilinx.h"
#include "program_analysis.h"

extern int auto_coalescing_preproces_top(CSageCodeGen *codegen, void *pTopFunc,
                                         const CInputOptions &options);

int bitwidth_opt_top(CSageCodeGen *codegen, void *pTopFunc,
    const CInputOptions &options) {
  MemoryCoalescingXilinx btwopt(codegen, pTopFunc, options);
  return static_cast<int>(btwopt.run());
}
