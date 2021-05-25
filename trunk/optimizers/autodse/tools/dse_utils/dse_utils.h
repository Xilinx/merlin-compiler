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


#ifndef TRUNK_OPTIMIZERS_AUTODSE_TOOLS_DSE_UTILS_DSE_UTILS_H_
#define TRUNK_OPTIMIZERS_AUTODSE_TOOLS_DSE_UTILS_DSE_UTILS_H_
#include <iterator>
#include <vector>
#include <set>
#include <string>
#include <algorithm>
#include <fstream>
#include <unordered_map>

#include "xml_parser.h"
#include "cmdline_parser.h"
#include "file_parser.h"

#include "mars_opt.h"
#include "PolyModel.h"
#include "tldm_annotate.h"
#include "codegen.h"
#include "program_analysis.h"

#include "mars_ir.h"
#include "dse_utils.h"

string getDesignSpaceParamName(string pragmaStr);
vector<string> getShadowedPipelineParams(CSageCodeGen *m_ast, void *loop);

void insertLoopDesignSpace(CSageCodeGen *m_ast,
                           void (*inserter)(CSageCodeGen *, void *, string),
                           void *stmt, string loop_code);

bool isFineGrainedLoop(CSageCodeGen *m_ast, void *loop);
bool isFlattenFunction(CSageCodeGen *m_ast, void *func);
bool hasNonFlattenFuncCall(CSageCodeGen *m_ast, void *loop);

bool IsLoopStatement(CSageCodeGen *m_ast, void *loop);
bool IsOutermostForLoop(CSageCodeGen *m_ast, void *loop);
void getSubPragmas(CSageCodeGen *m_ast, void *blk, set<void *> *setPragmas);
string getLoopHierfromParallelPragma(CSageCodeGen *m_ast, void *pragma);

std::unordered_map<string, int>
createWeightedCallgraphForFunction(CSageCodeGen *m_ast, void *func_body);
bool GetUnrollFactorFromPragma(CSageCodeGen *codegen, void *loop,
                               int64_t *factor);
#endif  // TRUNK_OPTIMIZERS_AUTODSE_TOOLS_DSE_UTILS_DSE_UTILS_H_
