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


#ifndef TRUNK_SOURCE_OPT_TOOLS_INCLUDE_KERNEL_SEPARATE_H_
#define TRUNK_SOURCE_OPT_TOOLS_INCLUDE_KERNEL_SEPARATE_H_
#include "cmdline_parser.h"
#include "file_parser.h"
#include "xml_parser.h"

#include "PolyModel.h"
#include "codegen.h"
#include "mars_message.h"
#include "mars_opt.h"
#include "tldm_annotate.h"

#include "mars_ir_v2.h"

#include "postwrap_process.h"

int kernel_separate_top(CSageCodeGen *codegen, void *pTopFunc,
                        const CInputOptions &options);
#endif  // TRUNK_SOURCE_OPT_TOOLS_INCLUDE_KERNEL_SEPARATE_H_
