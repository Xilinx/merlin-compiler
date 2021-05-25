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
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "PolyModel.h"
#include "cmdline_parser.h"
#include "file_parser.h"
#include "mars_opt.h"
#include "mars_ir.h"
#include "mars_ir_v2.h"
#include "tldm_annotate.h"
#include "xml_parser.h"

class PreProcess {
 public:
  CSageCodeGen *m_ast;
  void *mTopFunc;
  CInputOptions mOptions;
  CMarsIr mMars_ir;
  CMarsIrV2 mMars_ir_v2;

 private:
  bool mNaive_tag;
  bool mAuto_fg_tag;
  bool mAltera_flow;
  bool mXilinx_flow;
  bool mPure_kernel;
  bool mValidAltera;
  bool mValidXilinx;
  bool mValid;
  enum effort mEffort;
  set<string> mHeaderFileReported;

 public:
  PreProcess(CSageCodeGen *codegen, void *pTopFunc,
             const CInputOptions &options)
      : m_ast(codegen), mTopFunc(pTopFunc), mOptions(options),
        mNaive_tag(false), mAuto_fg_tag(false), mAltera_flow(false),
        mXilinx_flow(false), mPure_kernel(false), mValidAltera(true),
        mValidXilinx(true), mValid(true), mEffort(MEDIUM) {
    init();
  }

  bool run();

  bool processPragma(bool pragma_in_loop);
  bool parseHLSPragma(bool pragma_in_loop);
  void build_mars_ir(bool check_pragma, bool pragma_in_loop, bool build_node);

  void pre_check();

  void post_check();

 private:
  bool replacePipelineFlatten();
  bool replacePipelineFlatten(CMirNode *lnode);
  bool insertParallelPragma(CMirNode *lnode, set<CMirNode *> *p_visited);

  //  Added by Yuxin, 16-Feb-2016
  bool autoFineGrainParallel();
  bool autoFineGrainPipeline();
  bool autoFineGrainParallel(CMirNode *lnode);

  void init();
  void build_mars_ir_v2(bool build_node);
  void clear_mars_ir();
  bool preprocessing();
  bool postprocessing();
  bool processPragma(CMirNode *node, bool flatten_pipe,
                     set<CMirNode *> *p_visited);

  bool generateNewPragma(CMirNode *node, bool is_fgpip);

  bool checkPragmaLocation(CMirNode *node);

  void checkAssertHeader(void *file);

  bool CanonicalizeLoop(map<void *, string> *p_loop2file,
                        map<void *, string> *p_loop2line);

  bool StandardizeLoop();

  void checkNonCanonicalLoop(map<void *, string> *p_loop2file,
                             map<void *, string> *p_loop2line);

  void checkMemberFunction();

  /* void checkMemberVariable(CMirNode *fNode); */

  void checkOverloadInKernelFunction();

  /* void CheckKernelFunDeclInHeaders(); */

  void PreCheckLocalVariableWithNonConstantDimension();
  void PostCheckLocalVariableWithNonConstantDimension();
  void checkLocalVariableWithNonConstantDimension(void *func);

  void bool2char();

  void checkReferenceType();

  // void check_func_decl(CSageCodeGen *codegen, void *pTopFunc);

  // void check_old_style(CSageCodeGen *codegen, void *pTopFunc);

  void check_user_defined_type_outside_header_file(CSageCodeGen *codegen,
                                                   void *pTopFunc);

  int check_valid_top(CSageCodeGen *codegen, void *pTopFunc);

  int check_kernel_argument_top(CSageCodeGen *codegen, void *pTopFunc);

  void check_func_inline_legality(CSageCodeGen *codegen, void *pTopFunc);

  void check_single_linear_index(CSageCodeGen *codegen, void *pTopFunc);

  void check_identical_indices(CSageCodeGen *codegen, void *pTopFunc);

  int check_comm_pre(CSageCodeGen *codegen, void *pTopFunc);

  int remove_dummy_return_top(CSageCodeGen *codegen, void *pTopFunc);

  void check_kernel_argument(CSageCodeGen *codegen, void *pKernelTop);

  int altera_check_kernel_file_limitation(CSageCodeGen *codegen,
                                          void *kernel_func, void *sub_func,
                                          std::set<void *> *p_visited,
                                          vector<string> *p_res);
  int check_kernel_file_limitation(CSageCodeGen *codegen, void *pKernelTop);

  void check_valid_top(CSageCodeGen *codegen, void *func,
                       set<void *> *p_visited, int top);

  void check_multiple_nodes();

  void check_file_structure();

  int check_interface_change(void *func);

  void check_type_definition(void *compound_type_decl);
  void kernel_list_extraction(CSageCodeGen *codegen, void *pTopFunc);

  int check_function_pointer_top(CSageCodeGen *codegen, void *pTopFunc);

  void check_function_pointer(CSageCodeGen *codegen, void *func,
                              set<void *> *p_visited);

  void check_empty_kernel();

  void parse_aggregate_init();

  int memcpy_check(void *func_call);

  int altera_memcpy_check_top(void *func_call);

  int memcpy_check_top();

  void check_range_analysis();

  void RemoveKernelFunDeclStaticModifier();

  int clean_label_scope();

  void check_result();

  void check_loop_special_syntax(void *sg_loop);
};
