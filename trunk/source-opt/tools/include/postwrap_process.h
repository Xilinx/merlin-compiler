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

#include <map>
#include <set>

#include "mars_opt.h"
#include "bsuNode_v2.h"
#include "codegen.h"
#include "mars_ir_v2.h"
#include "program_analysis.h"
#include "rose.h"

#define RETURN_VAR "merlin_return_value"
void return_replace_top(CSageCodeGen *codegen, void *pTopFunc,
                        const CInputOptions &options);
void typedef_spread_top(CSageCodeGen *codegen, void *pTopFunc,
                        const CInputOptions &options, bool castonly = false,
                        bool skip_builtin = false, void *scope = nullptr,
                        bool skip_include = true);
int reference_replace_top(CSageCodeGen *codegen, void *pTopFunc,
                          const CInputOptions &options);

void func_typedef_spread(CSageCodeGen *codegen, void *func_decl,
                         set<void *> *visited_type, bool skip_builtin,
                         void *scope = nullptr);

void func_reference_replace(CSageCodeGen *codegen, void *func_decl);

void castonly_typedef_spread(CSageCodeGen *codegen, void *func_decl,
                             set<void *> *visited_funcs, bool skip_builtin);

void sizeof_typedef_spread(CSageCodeGen *codegen, void *scope,
                           void *input_scope, bool skip_builtin);

void intype_expr_typedef_spread(CSageCodeGen *codegen, void *sg_type,
                                void **sg_new_type, void *pos,
                                bool skip_builtin);

void init_typedef_spread(CSageCodeGen *codegen, void *sg_init,
                         set<void *> *visited_type, bool skip_builtin,
                         void *scope);

int remove_init_typedef(CSageCodeGen *codegen, void *sg_init,
                        void **sg_new_type, bool skip_builtin);

int global_variable_conversion_checker(CSageCodeGen *codegen, void *pTopFunc,
                                       const CInputOptions &options);

void collect_used_global_variable(void *scope, set<void *> *s_global,
                                  map<void *, set<void *>> *var2refs,
                                  CSageCodeGen *ast, bool *errorOut,
                                  bool hls_flow);
