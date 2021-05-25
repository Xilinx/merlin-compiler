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


#ifndef TRUNK_SOURCE_OPT_TOOLS_PERFORMANCE_ESTIMATION_SRC_FAST_DSE_H_
#define TRUNK_SOURCE_OPT_TOOLS_PERFORMANCE_ESTIMATION_SRC_FAST_DSE_H_

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "stdio.h"
#include "stdlib.h"

#include "cmdline_parser.h"
#include "file_parser.h"
#include "xml_parser.h"

#include "mars_ir.h"
#include "mars_opt.h"
#include "program_analysis.h"

#include "PolyModel.h"
#include "codegen.h"
#include "locations.h"
#include "report.h"
#include "rose.h"
#include "tldm_annotate.h"
#include "performance_estimation.h"

// using namespace std;
// using namespace rapidjson;

class FastDseBlock;
class FastDseBlock {
 public:
  BlockType Type;
  void *AstPtr;
  string TopoID;
  string KernelName;

  CSageCodeGen *codegen;

  int64_t II = 1;
  int64_t UF = 1;
  int64_t TC = 1;
  int64_t IL = 0;
  int64_t ILI = 0;
  string IsUnroll;

  FastDseBlock(CSageCodeGen *codegen, BlockType Type, void *AstPtr);
  ~FastDseBlock();
  int GenerateRequireDataStucture(void *pKernel, string Tool,
                                  vector<FastDseBlock *> *BlockList,
                                  map<void *, FastDseBlock *> *Ast_Block);
};
#endif  // TRUNK_SOURCE_OPT_TOOLS_PERFORMANCE_ESTIMATION_SRC_FAST_DSE_H_
