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


#include <vector>
#include "stream_update.h"
using MerlinStreamModel::CStreamBase;
using MerlinStreamModel::CStreamChannel;
using MerlinStreamModel::CStreamIR;
using MerlinStreamModel::CStreamNode;
using MerlinStreamModel::CStreamPort;

void StreamingUpdate(CSageCodeGen *codegen, void *pTopFunc,
                     const CInputOptions &options, string impl_tool,
                     map<void *, vector<void *>> *map_kernel_spawn_kernel,
                     vector<string> *vec_auto_kernel) {
  printf("---------------------\n");
  printf("-----STREAMING_UPDATE\n");
  printf("---------------------\n");
  if (impl_tool == "hidden") {
    CMarsIr mars_ir;
    mars_ir.get_mars_ir(codegen, pTopFunc, options);
    CMarsIrV2 mars_ir_v2;
    mars_ir_v2.build_mars_ir(codegen, pTopFunc);
    CStreamIR stream_ir(codegen, options, &mars_ir, &mars_ir_v2);
    // Generate OpenCL channels
    stream_ir.ChannelReplacement();

    // Get all spawn pragma
    stream_ir.CodeTransformForSpawnKernels();

    // pass to data parser
    *map_kernel_spawn_kernel = stream_ir.map_kernel_spawn_kernel;
    *vec_auto_kernel = stream_ir.vec_auto_kernel;

    for (auto one_kernel_info : *map_kernel_spawn_kernel) {
      void *top_kernel = one_kernel_info.first;
      vector<void *> sub_kernels = one_kernel_info.second;
      for (auto sub : sub_kernels) {
        cout << "top_kernel = " << codegen->UnparseToString(top_kernel)
             << ", sub_kernel = " << codegen->UnparseToString(sub) << endl;
      }
    }
    //        vec_spawn_kernel = stream_ir.vec_spawn_kernel;
    //        for(auto one_kernel : vec_spawn_kernel) {
    //            cout << "one kernel = " <<
    //            codegen->UnparseToString(one_kernel)
    //            << endl;
    //        }
  }
  printf("----------------------------\n");
  printf("-----FINISH STREAMING_UPDATE\n");
  printf("----------------------------\n");
}
