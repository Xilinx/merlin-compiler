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


#include "midend_preprocess.h"

#include <iostream>
#include <string>

#include "math.h"
#include "mars_message.h"
#include "program_analysis.h"
#include "rose.h"
#include "comm_opt.h"
#include "function_inline.h"
#include "postwrap_process.h"
#include "preprocess.h"

extern int loop_tiling_top(CSageCodeGen *codegen, void *pTopFunc,
                           const CInputOptions &options, bool pragma_in_loop);

extern int constant_propagation_top(CSageCodeGen *codegen, void *pTopFunc,
                                    const CInputOptions &option,
                                    bool assert_generation);
extern int simple_dce_top(CSageCodeGen *codegen, void *pTopFunc,
                          const CInputOptions &option);
extern int check_const_array_size(CSageCodeGen *codegen, void *TopFunc);

int midend_preprocess_top(CSageCodeGen *codegen, void *pTopFunc,
                          const CInputOptions &options) {
  //  2017-05-12 support complex pragmas inserted by AutoDSE
  PreProcess pre_instance(codegen, pTopFunc, options);
  pre_instance.build_mars_ir(false, true, true);
  pre_instance.processPragma(true);

  MidEndPreProcess mid_instance(codegen, pTopFunc, options);
  mid_instance.run();

  return 0;
}

void MidEndPreProcess::init() {
  if ("naive_hls" == mOptions.get_option_key_value("-a", "naive_hls")) {
    mNaive_tag = true;
    cout << "[MARS-PARALLEL-MSG] Naive HLS mode.\n";
  }

  if ("sdaccel" == mOptions.get_option_key_value("-a", "impl_tool")) {
    mXilinx_flow = true;
    cout << "[MARS-PARALLEL-MSG] Enable xilinx flow mode.\n";
  }
  //  ===== Automatic find-grained pipeline/parallel settings
  if ("auto_fg_opt" == mOptions.get_option_key_value("-a", "auto_fg_opt")) {
    mAuto_fgpip_tag = true;
    mAuto_fgpar_tag = true;
    cout << "[MARS-PARALLEL-MSG] Automatic fine-grained pipeline/parallel "
            "mode.\n";
  }
  if ("auto_fgpip_opt" ==
      mOptions.get_option_key_value("-a", "auto_fgpip_opt")) {
    mAuto_fgpip_tag = true;
    cout << "[MARS-PARALLEL-MSG] Automatic fine-grained pipeline "
            "mode.\n";
  }
  if ("auto_fgpar_opt" ==
      mOptions.get_option_key_value("-a", "auto_fgpar_opt")) {
    mAuto_fgpar_tag = true;
    cout << "[MARS-PARALLEL-MSG] Automatic fine-grained parallel "
            "mode.\n";
  }
  mThreshold_fgpar = 16;
  string threshold_fgpar_str =
      mOptions.get_option_key_value("-a", "auto_fgpar_threshold");
  if (!threshold_fgpar_str.empty()) {
    mThreshold_fgpar = my_atoi(threshold_fgpar_str);
  }
  //  =====================================

  string effort_level = mOptions.get_option_key_value("-a", "effort");
  if (effort_level == "low") {
    mEffort = LOW;
  } else if (effort_level == "high") {
    mEffort = HIGH;
  }
}

bool MidEndPreProcess::run() {
  bool Changed = false;

  //  check whether there is 'auto' value in the current pragma
  Changed |= check_auto_value_in_pragma();
  //  20171205 delay constant propagation to be friendly with design space
  //  exploration
  //  Youxiang bug1547 20170823
  cout << "\n\n++++++++++++++++++++++++++++++++++++++++" << endl;
  cout << "Apply constant propagation " << endl;
  cout << "++++++++++++++++++++++++++++++++++++++++" << endl;
  Changed |= constant_propagation_top(m_ast, mTopFunc, mOptions, false);
  Changed |= simple_dce_top(m_ast, mTopFunc, mOptions);

  return Changed;
}

void MidEndPreProcess::build_mars_ir(bool check_pragma, bool pragma_in_loop) {
  //  build Mars IR
  mMars_ir.get_mars_ir(m_ast, mTopFunc, mOptions, true, check_pragma,
                       pragma_in_loop);
  mMars_ir_v2.build_mars_ir(m_ast, mTopFunc, false, false);
}

void MidEndPreProcess::clear_mars_ir() {
  //  clear Mars IR
  mMars_ir.clear();
  mMars_ir_v2.clear();
}

bool MidEndPreProcess::check_auto_value_in_pragma() {
  bool Changed = false;
  SgProject *project = static_cast<SgProject *>(mTopFunc);
  for (size_t i = 0; i < project->get_fileList().size(); i++) {
    SgSourceFile *file = isSgSourceFile(project->get_fileList()[i]);
    if ((m_ast->IsFromInputFile(file) == 0) ||
        m_ast->isWithInHeaderFile(file)) {
      continue;
    }

    vector<void *> pragmaStmts;
    m_ast->GetNodesByType(file, "preorder", "SgPragmaDeclaration",
                          &pragmaStmts);
    for (auto pragma : pragmaStmts) {
      SgPragmaDeclaration *decl =
          isSgPragmaDeclaration(static_cast<SgNode *>(pragma));
      ROSE_ASSERT(decl);
      CAnalPragma ana_pragma(m_ast);
      string pragmaString = decl->get_pragma()->get_pragma();
      std::ostringstream oss;
      oss << "\"" << pragmaString
          << "\" " + getUserCodeFileLocation(m_ast, decl, true);
      string pragma_info = oss.str();
      bool errorOut = false;
      if (ana_pragma.PragmasFrontendProcessing(decl, &errorOut, false)) {
        if (ana_pragma.contain_auto_value() && !ana_pragma.is_reduction()) {
          dump_critical_message(PROCS_WARNING_39(pragma_info));
          m_ast->RemoveStmt(decl);
          Changed = true;
        }
      }
    }
  }
  return Changed;
}
