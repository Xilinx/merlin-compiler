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


#ifndef TRUNK_SOURCE_OPT_TOOLS_INCLUDE_ARRAY_DELINEARIZE_H_
#define TRUNK_SOURCE_OPT_TOOLS_INCLUDE_ARRAY_DELINEARIZE_H_

#include <map>
#include <vector>
#include "codegen.h"
#include "program_analysis.h"
int analysis_delinearize(CSageCodeGen *ast, void *array_init, int dim,
                         std::vector<size_t> *dim_split_steps,
                         std::map<void *, size_t> *mapAlias2BStep);
int analysis_delinearize(CSageCodeGen *ast, void *array_init, int dim,
                         std::vector<size_t> *dim_split_steps,
                         std::map<void *, size_t> *mapAlias2BStep,
                         bool *is_simple, void *offset,
                         void *scope);
void analysis_delinearize_postproc_alias_offset(
    CSageCodeGen *ast, void *array_init, int dim_idx, void *scope,
    std::vector<size_t> *merged_step_all_access,  //  input/output
    std::map<void *, size_t> *mapAlias2BStep);    //  output

int apply_delinearize(CSageCodeGen *ast, void *array_init, int dim,
                      const std::vector<size_t> &dim_split_steps,
                      std::map<void *, size_t> *mapAlias2BStep);
int apply_delinearize(
    CSageCodeGen *ast, void *array_init, int dim,
    const std::vector<size_t> &dim_split_steps,
    std::map<void *, size_t> *mapAlias2BStep, bool check_only, bool *is_simple,
    void *offset, void *scope,
    std::map<void *, std::vector<MarsProgramAnalysis::CMarsExpression>>
        *mapIndex2Delinear,
    std::vector<size_t> *new_array_dims);
int check_only_delinearize(
    CSageCodeGen *ast, void *array_init, int dim,
    const std::vector<size_t> &dim_split_steps,
    std::map<void *, size_t> *mapAlias2BStep,
    std::map<void *, std::vector<MarsProgramAnalysis::CMarsExpression>>
        *mapIndex2Delinear,
    std::vector<size_t> *new_array_dims);

#endif  // TRUNK_SOURCE_OPT_TOOLS_INCLUDE_ARRAY_DELINEARIZE_H_
