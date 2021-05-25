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


#ifndef TRUNK_SOURCE_OPT_TOOLS_FINAL_CODE_GEN_SRC_STREAM_UPDATE_H_
#define TRUNK_SOURCE_OPT_TOOLS_FINAL_CODE_GEN_SRC_STREAM_UPDATE_H_

#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <boost/algorithm/string.hpp>
#include "analPragmas.h"
#include "cmdline_parser.h"
#include "codegen.h"
#include "file_parser.h"
#include "id_update.h"
#include "ir_types.h"
#include "mars_ir.h"
#include "mars_ir_v2.h"
#include "mars_message.h"
#include "mars_opt.h"
#include "math.h"
#include "program_analysis.h"
#include "rose.h"
#include "stream_ir.h"
#include "tldm_annotate.h"
#include "report.h"
#include "xml_parser.h"
#include "common.h"

void StreamingUpdate(CSageCodeGen *codegen, void *pTopFunc,
                     const CInputOptions &options, string impl_tool,
                     map<void *, vector<void *>> *map_kernel_spawn_kernel,
                     vector<string> *vec_auto_kernel);

#endif  // TRUNK_SOURCE_OPT_TOOLS_FINAL_CODE_GEN_SRC_STREAM_UPDATE_H_
