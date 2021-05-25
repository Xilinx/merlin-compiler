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


#ifndef TRUNK_SOURCE_OPT_TOOLS_INCLUDE_INTERFACE_TRANSFORM_H_
#define TRUNK_SOURCE_OPT_TOOLS_INCLUDE_INTERFACE_TRANSFORM_H_
#include <vector>
#include <string>

#include "cmdline_parser.h"
#include "file_parser.h"
#include "xml_parser.h"

#include "PolyModel.h"
#include "codegen.h"
#include "mars_opt.h"
#include "tldm_annotate.h"

#include "mars_ir.h"
#include "mars_ir_v2.h"

#include "postwrap_process.h"

int rename_interface_name_top(CSageCodeGen *codegen, void *pTopFunc,
                              const CInputOptions &options);
int array_linearize_top(CSageCodeGen *codegen, void *pTopFunc,
                        const CInputOptions &options);
int data_transfer_gen_top(CSageCodeGen *codegen, void *pTopFunc,
                          const CInputOptions &options);
int struct_decompose_top(CSageCodeGen *codegen, void *pTopFunc,
                         const CInputOptions &options);
int access_range_gen_top(CSageCodeGen *codegen, void *pTopFunc,
                         const CInputOptions &options);
int remove_simple_pointer_alias_top(CSageCodeGen *codegen, void *pTopFunc,
                                    const CInputOptions &option);
int constant_propagation_top(CSageCodeGen *codegen, void *pTopFunc,
                             const CInputOptions &option,
                             bool assert_generation);
void bool2char_top(CSageCodeGen *codegen, void *pTopFunc,
                   const CInputOptions &options);
int check_non_static_pointer_interface_top(CSageCodeGen *codegen,
                                           void *pTopFunc,
                                           const CInputOptions &options);
int DetectMultiDimPointerInterfaceTop(CSageCodeGen *codegen, void *pTopFunc,
                                      const CInputOptions &options);
//  helper function

void *get_unique_wrapper_pragma(void *sg_init, CSageCodeGen *ast,
                                CMarsIrV2 *mars_ir);

vector<void *> get_unique_interface_pragma(void *sg_init, CSageCodeGen *ast,
                                           CMarsIrV2 *mars_ir);
vector<string> get_unique_interface_attribute(void *sg_init, string attribute,
                                              CSageCodeGen *ast,
                                              CMarsIrV2 *mars_ir);
int is_interface_related(vector<string> depths, void *kernel,
                         CSageCodeGen *ast);

int IsStructElement(string str, void *kernel, CSageCodeGen *ast);

int get_total_size_from_string(string s_depth);

int is_number_string(string str);
#endif  // TRUNK_SOURCE_OPT_TOOLS_INCLUDE_INTERFACE_TRANSFORM_H_
