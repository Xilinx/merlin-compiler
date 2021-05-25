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


#pragma once

#include <vector>
#include <set>
#include <string>

#include "rose.h"
#include "mars_opt.h"
#include "mars_ir.h"

void reduction_run(CSageCodeGen *codegen, void *pTopFunc,
                   const CInputOptions &options);
bool insert_reduction(CSageCodeGen *codegen, void *pTopFunc, CMarsIr *mars_ir,
                      CMirNode *bNode, int node_num, string str_scheme,
                      set<void *> *rdc_buf);
bool anal_reduction(CSageCodeGen *codegen, CMirNode *bNode, CMirNode *cNode,
                    SgExpression *arr_ref);
int lift_location(CSageCodeGen *codegen, CMirNode *bNode, CMirNode *cNode,
                  std::vector<void *> pntr_to_reduce);
int lift_location_internal(CSageCodeGen *codegen, CMirNode *bNode,
                           CMirNode *cNode, SgExpression *arr_ref);
bool build_reduction(CSageCodeGen *codegen, CMirNode *bNode, CMirNode *cNode,
                     SgExpression *curr_op, int lift_level,
                     std::vector<void *> pntr_to_reduce, string str_scheme,
                     set<void *> *rdc_buf);
int get_ref_num(CSageCodeGen *codegen, void *sg_bb, void *arr_init);

bool check_loop_initializer(CSageCodeGen *codegen, void *sg_scope,
                            int lift_level);

void remove_reduction_pragma(CSageCodeGen *codegen, void *pTopFunc,
                             set<void *> rdc_buf);
void remove_labels(CSageCodeGen *codegen, void *pTopFunc);

void reportMessage(CSageCodeGen *codegen, CMirNode *cNode, void *arr_ref,
                   std::vector<void *> pntr_to_reduce);
void reportNoBenefit(CSageCodeGen *codegen, void *curr_op);
void reportUnCanonicalLoop(CSageCodeGen *codegen, void *loop);
void reportLoopLowerBound(CSageCodeGen *codegen, void *loop);
