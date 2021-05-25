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

#if 0
// Move all the auto parallel related features to
// auto_parallel pass
bool MidEndPreProcess::autoFineGrainParallel(CMirNode *lnode) {
  size_t limit = mThreshold_fgpar;
  bool Change = false;
  CMirNode *node = lnode;
  //  Yuxin: Aug/08/2019
  //  Conservatively enhance
  while (autoFineGrainParallel(node, &limit)) {
    Change = true;
    node = node->get_parent();
    mMars_ir.build_fine_grain(m_ast, false);
    if (!node->is_fine_grain) {
      break;
    }
  }

  return Change;
}

//  Yuxin: Aug/09/2019
//  Enhance: if parallel pragma exists, still check if auto parallel can
//  continue to the upper level of loop
bool MidEndPreProcess::autoFineGrainParallel(CMirNode *lnode, size_t *limit,
                                             bool force) {
  if (lnode->has_hls_fg_opt() != 0) {
    return false;
  }
  if (lnode->has_pipeline() != 0) {
    return false;
  }
  if (lnode->is_function) {
    return false;
  }
  void *loop = m_ast->GetEnclosingNode("ForLoop", lnode->ref);
  int64_t exact_range = 0;
  int ret = m_ast->get_loop_trip_count(loop, &exact_range, nullptr);
  if (ret == 0) {
    return false;
  }

  // Yuxin: Feb 13 2020
  // If loop tripcount=1, just parallel it
  if (lnode->has_parallel() != 0) {
    if (exact_range > 0) {
      *limit = *limit / exact_range;
    }
    return true;
  } else {
    if (exact_range <= 1 || exact_range <= *limit || force) {
      void *sg_pragma =
          m_ast->CreatePragma("ACCEL PARALLEL COMPLETE ", lnode->ref);
      m_ast->PrependChild(lnode->ref, sg_pragma);
      CAnalPragma ana_pragma(m_ast);
      ana_pragma.set_pragma_type(CMOST_PARALLEL);
      ana_pragma.add_attribute(CMOST_complete, "");
      lnode->add_pragma(ana_pragma);
      if (exact_range > 0) {
        *limit = *limit / exact_range;
      }
      return true;
    }
  }
//  if (force == false)
//    insertFineGrainPipeline(lnode);

  return false;
}

bool MidEndPreProcess::autoFineGrainParallel() {
  vector<CMirNode *> all_nodes_vec;
  mMars_ir.get_topological_order_nodes(&all_nodes_vec);
  bool Changed = false;

  for (size_t i = 0; i != all_nodes_vec.size(); ++i) {
    CMirNode *new_node = all_nodes_vec[i];
    if (new_node->is_while) {
      continue;
    }
    if (new_node->is_function) {
      continue;
    }
    if (!new_node->is_innermost) {
      continue;
    }
    Changed |= autoFineGrainParallel(new_node);
  }
  return Changed;
}

bool MidEndPreProcess::autoFineGrainPipeline() {
  std::vector<CMirNode *> kernels;
  set<CMirNode *> all_nodes;
  vector<CMirNode *> all_nodes_vec;
  mMars_ir.get_all_kernel_nodes(&kernels, &all_nodes, &all_nodes_vec);
  bool Changed = false;

  for (size_t i = 0; i != all_nodes_vec.size(); ++i) {
    CMirNode *new_node = all_nodes_vec[i];
    if (new_node->is_function) {
      continue;
    }
    mMars_ir.build_fine_grain(m_ast, false);
    Changed |= insertFineGrainPipeline(new_node);
  }

  return Changed;
}

bool MidEndPreProcess::insertFineGrainPipeline(CMirNode *lnode) {
  if (!lnode->is_fine_grain) {
#if DEBUG
    cout << "[PREPROCESS-MSG] Due to analysis,"
            "this is a coarse-grained pipeline.\n";
#endif
    return false;
  }
  if (lnode->has_hls_fg_opt() != 0) {
    return false;
  }
  if (lnode->has_pipeline() != 0) {
    return false;
  }
  if ((lnode->has_parallel() != 0) &&
      lnode->get_attribute(CMOST_parallel_factor).empty()) {
    return false;
  }
  // Yuxin: Oct 10 2019
  // Check whether if parent nodes has HLS pipeline/parallel pragma
  CMirNode *parent_node = lnode->get_parent();
  vector<CMirNode *> nodes;
  nodes.push_back(lnode);
  while ((parent_node != nullptr) && (parent_node->is_function == 0)) {
    std::vector<void *> vec_pragmas;
    parent_node->collectVendorOptPragmas(m_ast, &vec_pragmas);
    if (vec_pragmas.size() > 0) {
      size_t limit = 1;
      for (auto node : nodes) {
        autoFineGrainParallel(node, &limit, true);
      }
      return false;
    }
    nodes.push_back(parent_node);
    if (parent_node->get_parent() != nullptr) {
      parent_node = parent_node->get_parent();
    }
  }
  // Yuxin: Spread the "priority" choice from sub-loops
  string set_prior;
  vector<void *> vec_loops;
  m_ast->GetNodesByType(lnode->ref, "preorder", "SgForStatement", &vec_loops);
  for (auto loop : vec_loops) {
    void *loop_body = m_ast->GetLoopBody(loop);
    CMirNode *node = mMars_ir.get_node(loop_body);
    if (node == nullptr) {
      continue;
    }
    for (auto pragma_info : node->get_pragma()) {
      if (pragma_info.is_parallel()) {
        string prior = node->get_attribute(CMOST_prior);
        if (prior == "logic") {
          set_prior = prior;
          break;
        }
      }
    }
    if (set_prior == "logic") {
      break;
    }
  }
  void *loop = m_ast->GetEnclosingNode("ForLoop", lnode->ref);
  int64_t exact_range = 0;
  m_ast->get_loop_trip_count(loop, &exact_range, nullptr);
  if (exact_range == 1) {
    size_t limit = 1;
    return autoFineGrainParallel(lnode, &limit, true);
  }
  string pipeline_pragma =
      std::string(ACCEL_PRAGMA) + " " + CMOST_PIPELINE + " AUTO";
  if (set_prior == "logic") {
    pipeline_pragma += std::string(" ") + CMOST_prior + "=" + set_prior;
  }

  void *sg_pragma = m_ast->CreatePragma(pipeline_pragma, lnode->ref);
  m_ast->PrependChild(lnode->ref, sg_pragma);
  CAnalPragma ana_pragma(m_ast);
  ana_pragma.set_pragma_type(CMOST_PIPELINE);
  lnode->add_pragma(ana_pragma);
  return true;
}
#endif

void MidEndPreProcess::init() {
  if ("naive_hls" == mOptions.get_option_key_value("-a", "naive_hls")) {
    mNaive_tag = true;
    cout << "[MARS-PARALLEL-MSG] Naive HLS mode.\n";
  }
  if ("aocl" == mOptions.get_option_key_value("-a", "impl_tool")) {
    mAltera_flow = true;
    cout << "[MARS-PARALLEL-MSG] Enable altera flow mode.\n";
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
  if (mAltera_flow) {
    check_const_array_size(m_ast, mTopFunc);
  }
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

#if 0
  //  Yuxin, 03-Nov-2017, seperate automatic fgpip and fgpar, add threshold
  //  setting For automatic fine-grained pipeline/parallel
  //  for xilinx flow, we integrate all auto parallel into auto_parallel passes
  //
  //  Yuxin, Aug/31/2020, keep only for Intel flow
  //
  if (mAuto_fgpar_tag && !mNaive_tag && !mXilinx_flow) {
    bool LocalChanged = false;
    clear_mars_ir();
    build_mars_ir(false, true);
    LocalChanged |= autoFineGrainParallel();

    if (LocalChanged) {
      dump_critical_message(PROCS_INFO_3, 1);
      Changed = true;
    }
  }
#endif

  //  Changed |= check_partial_paralleled_loop();
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

#if 0
bool MidEndPreProcess::check_partial_paralleled_loop() {
  bool Changed = false;
  SgProject *project = static_cast<SgProject *>(mTopFunc);
  for (size_t i = 0; i < project->get_fileList().size(); i++) {
    SgSourceFile *file = isSgSourceFile(project->get_fileList()[i]);
    if (!m_ast->IsFromInputFile(file) || m_ast->isWithInHeaderFile(file))
      continue;

    vector<void *> pragmaStmts;
    m_ast->GetNodesByType(file, "preorder",
        "SgPragmaDeclaration", &pragmaStmts);
    for (auto pragma : pragmaStmts) {
      SgPragmaDeclaration *decl =
        isSgPragmaDeclaration(static_cast<SgNode *>(pragma));
      ROSE_ASSERT(decl);
      CAnalPragma ana_pragma(m_ast);
      bool errorOut = false;
      if (ana_pragma.PragmasFrontendProcessing(decl, &errorOut, false)) {
        if (!ana_pragma.is_loop_related())
          continue;
        void *loop = m_ast->TraceUpToLoopScope(decl);
        string pragmaString = decl->get_pragma()->get_pragma();
        std::ostringstream oss;
        oss << "\"" << pragmaString
            << "\" " + getUserCodeFileLocation(m_ast, decl, true);
        string pragma_info = oss.str();
        if (loop && mAltera_flow) {
          if (ana_pragma.is_parallel() &&
              ("" != ana_pragma.get_attribute(CMOST_parallel_factor))) {
            dump_critical_message(PROCS_WARNING_7(pragma_info));
            m_ast->AddComment("Original: #pragma " + pragmaString,
                             m_ast->GetScope(decl));
            m_ast->RemoveStmt(decl);
            Changed = true;
          }
        }
      }
    }
  }
  return Changed;
}
#endif

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
