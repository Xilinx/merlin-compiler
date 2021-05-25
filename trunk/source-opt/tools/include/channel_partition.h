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


#ifndef TRUNK_SOURCE_OPT_TOOLS_INCLUDE_CHANNEL_PARTITION_H_
#define TRUNK_SOURCE_OPT_TOOLS_INCLUDE_CHANNEL_PARTITION_H_
#include <map>
#include <set>
#include <vector>
#include "codegen.h"
#include "mars_ir.h"
#include "mars_opt.h"
#include "stream_ir.h"

using MerlinStreamModel::CStreamBase;
using MerlinStreamModel::CStreamIR;

int channel_partition_intel_top(CSageCodeGen *codegen, void *pTopFunc,
                                const CInputOptions &options);

void partition_analysis_intel(CSageCodeGen *codegen, CMarsIr *mars_ir,
                              MerlinStreamModel::CStreamIR *stream_ir);

bool partition_analysis_var(CSageCodeGen *codegen, void *var, CMarsIr *mars_ir,
                            std::map<int, int> *m_factors);

bool analyze_channel(CSageCodeGen *codegen, void *var, void *sg_loop,
                     std::map<int, int> *m_factors, int tmp_dim);

void insert_internal_pragma(CSageCodeGen *codegen,
                            std::map<void *, std::map<int, int>> p_channels,
                            std::map<void *, std::map<int, int>> p_buffers);

void partition_transform_intel(CSageCodeGen *codegen, CMarsIr *mars_ir,
                               MerlinStreamModel::CStreamIR *stream_ir);

bool parse_internal_pragma(CSageCodeGen *codegen, void *sg_pragma,
                           CMarsIr *mars_ir, CStreamIR *stream_ir,
                           std::map<void *, std::map<int, int>> *p_channels,
                           std::vector<void *> *ch_pragma);

bool transform_channel(CSageCodeGen *codegen, CMarsIr *mars_ir,
                       CStreamIR *stream_ir,
                       const std::map<void *, map<int, int>> &p_channels);

void transform_buffer(CSageCodeGen *codegen);

void delete_internal_pragma(CSageCodeGen *codegen,
                            const vector<void *> &ch_pragma);

bool parse_iterator_for_call(
    CSageCodeGen *codegen, CStreamIR *stream_ir, void *sg_loop,
    vector<void *> *vec_new_expr,
    std::map<CStreamBase *, std::set<int>> *new_attribute,
    const std::vector<void *> &funcs, const std::vector<void *> &calls);

bool check_user_channel(CSageCodeGen *codegen, void *sg_ref,
                        std::vector<void *> vec_new_expr);
#endif  // TRUNK_SOURCE_OPT_TOOLS_INCLUDE_CHANNEL_PARTITION_H_
