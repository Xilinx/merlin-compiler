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


#ifndef TRUNK_SOURCE_OPT_TOOLS_INCLUDE_COARSE_PARALLEL_H_
#define TRUNK_SOURCE_OPT_TOOLS_INCLUDE_COARSE_PARALLEL_H_

#include "rose.h"

#include "PolyModel.h"
#include "cmdline_parser.h"
#include "file_parser.h"
#include "mars_opt.h"
#include "memory_burst.h"
#include "stream_ir.h"
#include "tldm_annotate.h"
#include "xml_parser.h"

int coarse_parallel_xilinx_top(CSageCodeGen *codegen, void *pTopFunc,
                               const CInputOptions &options);

bool generate_parallel_xilinx(CSageCodeGen *codegen, CMarsIr *mars_ir,
                              CMirNode *node);

#endif  // TRUNK_SOURCE_OPT_TOOLS_INCLUDE_COARSE_PARALLEL_H_
