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


#ifndef TRUNK_SOURCE_OPT_TOOLS_INCLUDE_MIDEND_PREPROCESS_H_
#define TRUNK_SOURCE_OPT_TOOLS_INCLUDE_MIDEND_PREPROCESS_H_
#include <vector>
#include "mars_opt.h"
#include "mars_ir.h"
#include "mars_ir_v2.h"
#include "codegen.h"

class MidEndPreProcess {
  CSageCodeGen *m_ast;
  void *mTopFunc;
  CInputOptions mOptions;
  CMarsIr mMars_ir;
  CMarsIrV2 mMars_ir_v2;
  bool mNaive_tag;
  bool mAuto_fgpip_tag;
  bool mAuto_fgpar_tag;
  bool mXilinx_flow;
  int mThreshold_fgpar;
  enum effort mEffort;

 public:
  MidEndPreProcess(CSageCodeGen *codegen, void *pTopFunc,
                   const CInputOptions &options)
      : m_ast(codegen), mTopFunc(pTopFunc), mOptions(options),
        mNaive_tag(false), mAuto_fgpip_tag(false), mAuto_fgpar_tag(false),
        mXilinx_flow(false), mEffort(MEDIUM) {
    init();
  }

  bool run();

 private:
  bool insertFineGrainPipeline(CMirNode *lnode);
  //  Added by Yuxin, 16-Feb-2016
  bool autoFineGrainParallel();
  bool autoFineGrainPipeline();
  bool autoFineGrainParallel(CMirNode *lnode);
  bool autoFineGrainParallel(CMirNode *lnode, size_t *limit,
                             bool force = false);
  void init();
  void build_mars_ir(bool check_pragma, bool pragma_in_loop);
  void clear_mars_ir();

  bool remove_parallel_pipeline_off_pragma();
  bool check_auto_value_in_pragma();
};
#endif  // TRUNK_SOURCE_OPT_TOOLS_INCLUDE_MIDEND_PREPROCESS_H_
