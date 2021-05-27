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


#include "mars_opt.h"

#include <execinfo.h>
#include <signal.h>
#include <sys/resource.h>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <boost/algorithm/string/replace.hpp>
#include <boost/regex.hpp>

#include "cmdline_parser.h"
#include "file_parser.h"
#include "xml_parser.h"

#include "PolyModel.h"
#include "codegen.h"
#include "postwrap_process.h"
#include "program_analysis.h"
#include "tldm_annotate.h"

#include "history.h"

#include "common.h"

#define PRINT_INFO 1

//  also defined in codegen->cpp
#define USE_LOWERCASE_NAME 0
#define USED_CODE_IN_COVERAGE_TEST 0

//  define version string
#ifndef BUILD_VERSION
#define BUILD_VERSION "internal"
#endif
#ifndef BUILD_DATE
#define BUILD_DATE ""
#endif

int g_debug_mode;

void stop_point() { printf("stop_point here\n"); }
void pool_visit(SgProject *);
void ptree(void *, const CInputOptions &option);

//  ///////////////////////////////////////////////////////////////////////////
//  / Pass Function Table
//  ///////////////////////////////////////////////////////////////////////////
//  /
typedef int (*MARS_OPT_PASS_FUNC)(CSageCodeGen *codegen, void *pTopFunc,
                                  const CInputOptions &options);
map<string, MARS_OPT_PASS_FUNC> g_pass_func_table;
void init_pass_func_table() {
  g_pass_func_table.clear();
#ifdef HAS_DS_TILING
  int ds_tiling_top(CSageCodeGen * codegen, void *pTopFunc,
                    const CInputOptions &options);
  g_pass_func_table["ds_tiling"] = ds_tiling_top;
#endif
#ifdef HAS_DS_SCOPE
  int ds_scope_top(CSageCodeGen * codegen, void *pTopFunc,
                   const CInputOptions &options);
  g_pass_func_table["ds_scope"] = ds_scope_top;
#endif
#ifdef HAS_DS_RESOURCE_EVAL
  int ds_resource_eval_top(CSageCodeGen * codegen, void *pTopFunc,
                           const CInputOptions &options);
  g_pass_func_table["ds_resource_eval"] = ds_resource_eval_top;
#endif
#ifdef HAS_DS_SKELETON_EXTRACT
  int ds_skeleton_extract_top(CSageCodeGen * codegen, void *pTopFunc,
                              const CInputOptions &options);
  g_pass_func_table["ds_skeleton_extract"] = ds_skeleton_extract_top;
#endif
#ifdef HAS_DS_MARK
  int ds_mark_top(CSageCodeGen * codegen, void *pTopFunc,
                  const CInputOptions &options);
  g_pass_func_table["ds_mark"] = ds_mark_top;
#endif
#ifdef HAS_DATA_REUSE
  int data_reuse_top(CSageCodeGen * codegen, void *pTopFunc,
                     const CInputOptions &options);
  g_pass_func_table["data_reuse"] = data_reuse_top;
#endif
#ifdef HAS_DS_LOOP_PARALLEL
  int ds_loop_parallel_top(CSageCodeGen * codegen, void *pTopFunc,
                           const CInputOptions &options);
  g_pass_func_table["ds_loop_parallel"] = ds_loop_parallel_top;
#endif
#ifdef HAS_LOOP_PARALLEL
  int loop_parallel_top(CSageCodeGen * codegen, void *pTopFunc,
                        const CInputOptions &options);
  g_pass_func_table["loop_parallel"] = loop_parallel_top;
#endif
#ifdef HAS_DATA_STREAM
  int data_stream_top(CSageCodeGen * codegen, void *pTopFunc,
                      const CInputOptions &options);
  g_pass_func_table["data_stream"] = data_stream_top;
#endif
#ifdef HAS_OPENMP_PARSE
  int openmp_parse_top(CSageCodeGen * codegen, void *pTopFunc,
                       const CInputOptions &options);
  g_pass_func_table["openmp_parse"] = openmp_parse_top;
#endif
#ifdef HAS_MARS_IR
  int test_mars_ir_top(CSageCodeGen * codegen, void *pTopFunc,
                       const CInputOptions &options);
  g_pass_func_table["test_mars_ir"] = test_mars_ir_top;
  int test_mars_ir_v2_top(CSageCodeGen * codegen, void *pTopFunc,
                          const CInputOptions &options);
  g_pass_func_table["test_mars_ir_v2"] = test_mars_ir_v2_top;
#endif
#ifdef HAS_STREAM_IR
  int test_stream_ir_top(CSageCodeGen * codegen, void *pTopFunc,
                         const CInputOptions &options);
  g_pass_func_table["test_stream_ir"] = test_stream_ir_top;
#endif
#ifdef HAS_MEMORY_BURST
  int memory_burst_top(CSageCodeGen * codegen, void *pTopFunc,
                       const CInputOptions &options);
  g_pass_func_table["memory_burst"] = memory_burst_top;
  int stream_prefetch_top(CSageCodeGen * codegen, void *pTopFunc,
                          const CInputOptions &options);
  g_pass_func_table["stream_prefetch"] = stream_prefetch_top;
#endif
#ifdef HAS_LOWER_CPP
  int lower_cpp_top(CSageCodeGen * codegen, void *pTopFunc,
                    const CInputOptions &options);
  g_pass_func_table["lower_cpp"] = lower_cpp_top;
#endif
#ifdef HAS_DEPENDENCY_RESOLVE
  int dependency_resolve_top(CSageCodeGen * codegen, void *pTopFunc,
                             const CInputOptions &options);
  g_pass_func_table["dependency_resolve"] = dependency_resolve_top;
#endif
#ifdef HAS_LOWER_SEPARATE
  int lower_separate_top(CSageCodeGen * codegen, void *pTopFunc,
                         const CInputOptions &options);
  g_pass_func_table["lower_separate"] = lower_separate_top;
#endif
#ifdef HAS_COARSE_PIPELINE
  int coarse_pipeline_top(CSageCodeGen * codegen, void *pTopFunc,
                          const CInputOptions &options);
  g_pass_func_table["coarse_pipeline"] = coarse_pipeline_top;
#endif
#ifdef HAS_COARSE_PARALLEL
  int coarse_parallel_top(CSageCodeGen * codegen, void *pTopFunc,
                          const CInputOptions &options);
  g_pass_func_table["coarse_parallel"] = coarse_parallel_top;
#endif
#ifdef HAS_DS_PIPELINE
  int ds_pipeline_top(CSageCodeGen * codegen, void *pTopFunc,
                      const CInputOptions &options);
  g_pass_func_table["ds_pipeline"] = ds_pipeline_top;
#endif

#ifdef HAS_BITWIDTH_OPT
  int bitwidth_opt_top(CSageCodeGen * codegen, void *pTopFunc,
                       const CInputOptions &options);
  g_pass_func_table["bitwidth_opt"] = bitwidth_opt_top;
#endif
#ifdef HAS_DS_BITWIDTH_OPT
  int ds_bitwidth_opt_top(CSageCodeGen * codegen, void *pTopFunc,
                          const CInputOptions &options);
  g_pass_func_table["ds_bitwidth_opt"] = ds_bitwidth_opt_top;
#endif
#ifdef HAS_REDUCTION
  int reduction_top(CSageCodeGen * codegen, void *pTopFunc,
                    const CInputOptions &options);
  g_pass_func_table["reduction"] = reduction_top;
#endif
#ifdef HAS_REDUCTION_GENERAL
  int reduction_general_top(CSageCodeGen * codegen, void *pTopFunc,
                            const CInputOptions &options);
  g_pass_func_table["reduction_general"] = reduction_general_top;
#endif
#ifdef HAS_PROGRAM_ANALYSIS
  int program_analysis_top(CSageCodeGen * codegen, void *pTopFunc,
                           const CInputOptions &options);
  g_pass_func_table["program_analysis"] = program_analysis_top;
#endif
#ifdef HAS_ID_UPDATE
  int id_update_top(CSageCodeGen * codegen, void *pTopFunc,
                    const CInputOptions &options);
  g_pass_func_table["id_update"] = id_update_top;
#endif
//  #ifdef HAS_MSG_TEST
//   int msg_test_top(CSageCodeGen *codegen, void *pTopFunc,
//                    const CInputOptions &options);
//   g_pass_func_table["msg_test"] = msg_test_top;
//  #endif
#ifdef HAS_PREPROCESS
  int preprocess_top(CSageCodeGen * codegen, void *pTopFunc,
                     const CInputOptions &options);
  g_pass_func_table["preprocess"] = preprocess_top;
#endif
#ifdef HAS_FUNCTION_INLINE
  int func_inline_top(CSageCodeGen * codegen, void *pTopFunc,
                      const CInputOptions &options);
  g_pass_func_table["function_inline"] = func_inline_top;
  int func_inline_by_pragma_top(CSageCodeGen * codegen, void *pTopFunc,
                                const CInputOptions &options);
  g_pass_func_table["function_inline_by_pragma"] = func_inline_by_pragma_top;
  int func_inline_by_pragma_HLS_top(CSageCodeGen * codegen, void *pTopFunc,
                                    const CInputOptions &options);
  g_pass_func_table["function_inline_by_pragma_HLS"] =
      func_inline_by_pragma_HLS_top;
#endif
#ifdef HAS_AUTO_FUNC_INLINE
  int auto_func_inline_top(CSageCodeGen * codegen, void *pTopFunc,
                           const CInputOptions &options);
  g_pass_func_table["auto_func_inline"] = auto_func_inline_top;
#endif
#ifdef HAS_AUTO_DIM_INTERCHANGE
  int auto_dim_interchange_top(CSageCodeGen * codegen, void *pTopFunc,
                               const CInputOptions &options);
  g_pass_func_table["auto_dim_interchange"] = auto_dim_interchange_top;
#endif
#ifdef HAS_FUNCTION_OUTLINE
  int function_outline_top(CSageCodeGen * codegen, void *pTopFunc,
                           const CInputOptions &options);
  g_pass_func_table["function_outline"] = function_outline_top;
#endif
#ifdef HAS_KERNEL_WRAPPER
  int kernel_wrapper_top(CSageCodeGen * codegen, void *pTopFunc,
                         const CInputOptions &options);
  g_pass_func_table["kernel_wrapper"] = kernel_wrapper_top;
#endif
#ifdef HAS_KERNEL_PRAGMA_GEN
  int kernel_pragma_gen(CSageCodeGen * codegen, void *pTopFunc,
                        const CInputOptions &options);
  g_pass_func_table["kernel_pragma_gen"] = kernel_pragma_gen;
#endif
#ifdef HAS_OPENCL_TRANSFER_GEN
  int opencl_transfer_gen(CSageCodeGen * codegen, void *pTopFunc,
                          const CInputOptions &options);
  g_pass_func_table["opencl_transfer_gen"] = opencl_transfer_gen;
#endif
#ifdef HAS_LOOP_DISTRIBUTE
  int loop_distribute_top(CSageCodeGen * codegen, void *pTopFunc,
                          const CInputOptions &options);
  g_pass_func_table["loop_distribute"] = loop_distribute_top;
  int loop_flatten_top(CSageCodeGen * codegen, void *pTopFunc,
                       const CInputOptions &options);
  g_pass_func_table["loop_flatten"] = loop_flatten_top;
#endif
#ifdef HAS_LOOP_TILING
  int loop_tiling_top(CSageCodeGen * codegen, void *pTopFunc,
                      const CInputOptions &options);
  g_pass_func_table["loop_tiling"] = loop_tiling_top;
#endif
#ifdef HAS_MIDEND_PREPROCESS
  int midend_preprocess_top(CSageCodeGen * codegen, void *pTopFunc,
                            const CInputOptions &options);
  g_pass_func_table["midend_preprocess"] = midend_preprocess_top;
#endif
#ifdef HAS_COMM_OPT
  int comm_opt_top(CSageCodeGen * codegen, void *pTopFunc,
                   const CInputOptions &options);
  g_pass_func_table["comm_opt"] = comm_opt_top;
  int comm_rename_top(CSageCodeGen * codegen, void *pTopFunc,
                      const CInputOptions &options);
  g_pass_func_table["comm_rename"] = comm_rename_top;
  int comm_refine_top(CSageCodeGen * codegen, void *pTopFunc,
                      const CInputOptions &options);
  g_pass_func_table["comm_refine"] = comm_refine_top;
  int comm_shared_memory_token_top(CSageCodeGen * codegen, void *pTopFunc,
                                   const CInputOptions &options);
  g_pass_func_table["comm_token"] = comm_shared_memory_token_top;
  int comm_sync_top(CSageCodeGen * codegen, void *pTopFunc,
                    const CInputOptions &options);
  g_pass_func_table["comm_sync"] = comm_sync_top;
  int comm_check_top(CSageCodeGen * codegen, void *pTopFunc,
                     const CInputOptions &options);
  g_pass_func_table["comm_check"] = comm_check_top;
  int comm_dead_remove_top(CSageCodeGen * codegen, void *pTopFunc,
                           const CInputOptions &options);
  g_pass_func_table["comm_dead_remove"] = comm_dead_remove_top;
#endif
#ifdef HAS_STRUCT_DECOMPOSE
  int struct_decompose_top(CSageCodeGen * codegen, void *pTopFunc,
                           const CInputOptions &options);
  g_pass_func_table["struct_decompose"] = struct_decompose_top;
#endif
#ifdef HAS_ARRAY_LINEARIZE
  int array_linearize_top(CSageCodeGen * codegen, void *pTopFunc,
                          const CInputOptions &options);
  g_pass_func_table["array_linearize"] = array_linearize_top;
  int interface_transform_top(CSageCodeGen * codegen, void *pTopFunc,
                              const CInputOptions &options);
  g_pass_func_table["interface_transform"] = interface_transform_top;

  int data_transfer_gen_top(CSageCodeGen * codegen, void *pTopFunc,
                            const CInputOptions &options);
  g_pass_func_table["data_transfer_gen"] = data_transfer_gen_top;

  int constant_propagation_top(CSageCodeGen * codegen, void *pTopFunc,
                               const CInputOptions &options);
  g_pass_func_table["constant_propagation"] = constant_propagation_top;
  int simple_dce_top(CSageCodeGen * codegen, void *pTopFunc,
                     const CInputOptions &options);
  g_pass_func_table["simple_dce"] = simple_dce_top;
  int rename_interface_name_top(CSageCodeGen * codegen, void *pTopFunc,
                                const CInputOptions &options);
  g_pass_func_table["interface_renaming"] = rename_interface_name_top;
  int bool2char_top(CSageCodeGen * codegen, void *pTopFunc,
                    const CInputOptions &options);
  g_pass_func_table["bool2char"] = bool2char_top;
  int remove_simple_pointer_alias_top(CSageCodeGen * codegen, void *pTopFunc,
                                      const CInputOptions &options);
  g_pass_func_table["basic_pointer_alias_removal"] =
      remove_simple_pointer_alias_top;
  int check_non_static_pointer_interface_top(
      CSageCodeGen * codegen, void *pTopFunc, const CInputOptions &options);
  g_pass_func_table["detect_non_static_pointer"] =
      check_non_static_pointer_interface_top;
  int access_range_gen_top(CSageCodeGen * codegen, void *pTopFunc,
                           const CInputOptions &options);
  g_pass_func_table["gen_access_range"] = access_range_gen_top;
#endif
#ifdef HAS_KERNEL_SEPARATE
  int kernel_separate_top(CSageCodeGen * codegen, void *pTopFunc,
                          const CInputOptions &options);
  g_pass_func_table["kernel_separate"] = kernel_separate_top;
#endif
#ifdef HAS_POSTWRAP_PROCESS
  int postwrap_process_top(CSageCodeGen * codegen, void *pTopFunc,
                           const CInputOptions &options);
  g_pass_func_table["postwrap_process"] = postwrap_process_top;
#endif
// #ifdef HAS_MEMORY_PARTITION
//   int memory_partition_top(CSageCodeGen * codegen, void *pTopFunc,
//                            const CInputOptions &options);
//   g_pass_func_table["memory_partition"] = memory_partition_top;
// #endif
#ifdef HAS_CHANNEL_PARTITION
  int channel_partition_top(CSageCodeGen * codegen, void *pTopFunc,
                            const CInputOptions &options);
  g_pass_func_table["channel_partition"] = channel_partition_top;
#endif
#ifdef HAS_PERFORMANCE_ESTIMATION
  int performance_estimation_top(CSageCodeGen * codegen, void *pTopFunc,
                                 const CInputOptions &options);
  g_pass_func_table["performance_estimation"] = performance_estimation_top;
#endif
#ifdef HAS_GEN_TOKEN_ID
  int gen_token_id_top(CSageCodeGen * codegen, void *pTopFunc,
                       const CInputOptions &options);
  g_pass_func_table["gen_token_id"] = gen_token_id_top;
#endif
#ifdef HAS_GEN_HIERARCHY
  int gen_hierarchy_top(CSageCodeGen * codegen, void *pTopFunc,
                        const CInputOptions &options);
  g_pass_func_table["gen_hierarchy"] = gen_hierarchy_top;
#endif
#ifdef HAS_ADD_PRAGMA_KERNEL
  int add_pragma_kernel(CSageCodeGen * codegen, void *pTopFunc,
                        const CInputOptions &options);
  g_pass_func_table["add_pragma_kernel"] = add_pragma_kernel;
#endif
#ifdef HAS_FINAL_CODE_GEN
  int final_code_gen(CSageCodeGen * codegen, void *pTopFunc,
                     const CInputOptions &options);
  g_pass_func_table["final_code_gen"] = final_code_gen;
#endif
#ifdef HAS_LINE_BUFFER
  int line_buffer_top(CSageCodeGen * codegen, void *pTopFunc,
                      const CInputOptions &options);
  g_pass_func_table["line_buffer"] = line_buffer_top;
#endif

  //  #ifdef HAS_TEST_PASS
  //  int test_pass_top(CSageCodeGen *codegen, void *pTopFunc,
  //                    const CInputOptions &options);
  //  g_pass_func_table["test_pass"] = test_pass_top;
  //  #endif
  //

  int DetectPointerCompareInterfaceTop(CSageCodeGen * codegen, void *pTopFunc,
                                       const CInputOptions &options);
  g_pass_func_table["detect_pointer_compare"] =
      DetectPointerCompareInterfaceTop;
  int DetectPointerTypeCastInterfaceTop(CSageCodeGen * codegen, void *pTopFunc,
                                        const CInputOptions &options);
  g_pass_func_table["detect_pointer_type_cast"] =
      DetectPointerTypeCastInterfaceTop;
  int DetectMultiDimPointerInterfaceTop(CSageCodeGen * codegen, void *pTopFunc,
                                        const CInputOptions &options);
  g_pass_func_table["detect_multidim_pointer"] =
      DetectMultiDimPointerInterfaceTop;
  int AutoParallelInsertionTop(CSageCodeGen * codegen, void *pTopFunc,
                               const CInputOptions &options);
  g_pass_func_table["auto_parallel"] = AutoParallelInsertionTop;
  int StructuralFuncInlineTop(CSageCodeGen * codegen, void *pTopFunc,
                              const CInputOptions &options);
  g_pass_func_table["structural_func_inline"] = StructuralFuncInlineTop;
  int DetectLibraryCallsTop(CSageCodeGen * codegen, void *pTopFunc,
                            const CInputOptions &options);
  g_pass_func_table["detect_library_calls"] = DetectLibraryCallsTop;
  int SynthesisCheckTop(CSageCodeGen * codegen, void *pTopFunc,
                        const CInputOptions &options);
  g_pass_func_table["synthesis_check"] = SynthesisCheckTop;
}

MARS_OPT_PASS_FUNC get_pass_func(string pass_name) {
  if (g_pass_func_table.end() == g_pass_func_table.find(pass_name)) {
    return nullptr;
  }
  return g_pass_func_table[pass_name];
}

extern map<SgNode *, string> orgIdMap;

int extract_top(const vector<string> &vecSrcList, string sTldmFile,
                string sTopFunc, const CInputOptions &options) {
  size_t i;
  CSageCodeGen codegen;  //  Rose AST encaptulation

  string cflags = options.get_option_key_value("-a", "cflags");

  system("rm -rf .finish_read_files");
  DEFINE_START_END;
  STEMP(start);
  void *sg_project = codegen.OpenSourceFile(vecSrcList, cflags);
  ACCTM(opencsourcefile, start, end);
  if (sg_project == NULL) {
    throw std::exception();
  }
  system("touch .finish_read_files");
  codegen.InitBuiltinTypes();
  //  codegen.GeneratePDF();

#if 0
    vector<string> vecTldmPragmas;
    codegen.TraverseSimple(sg_project, "preorder", GetTLDMInfo,
                           &vecTldmPragmas);

    string str = "";
    for (i = 0; i < vecTldmPragmas.size(); i++) {
      str += vecTldmPragmas[i] + "\n";
    }
    write_string_into_file(str, sTldmFile);
#endif

  historyModuleInit(&codegen, options.get_option("-p"));

  //  1. get top function
  void *pTopFunc = nullptr;  //  void * to encaptulate Rose type
  if (!sTopFunc.empty()) {
    vector<void *> vecFuncs;
    codegen.GetNodesByType(sg_project, "preorder", "SgFunctionDeclaration",
                           &vecFuncs);

    for (i = 0; i < vecFuncs.size(); i++) {
      string sFuncName = codegen.GetFuncName(vecFuncs[i]);

      if ((codegen.GetFuncBody(vecFuncs[i]) != nullptr) &&
          sFuncName == sTopFunc) {
        pTopFunc = vecFuncs[i];
        break;
      }
    }

    if (pTopFunc == nullptr) {
      printf("[mars_opt] ERROR: top function is set (%s), but not found in the "
             "source code.\n",
             sTopFunc.c_str());
      assert(pTopFunc);
    }
  } else {
    pTopFunc = sg_project;
  }

#if USED_CODE_IN_COVERAGE_TEST
  int is_keep_code = (options.get_option("-a") == "bfm") ? 1 : 0;
#endif
  string curr_pass = options.get_option("-p");
  bool donothing = "none" == curr_pass;

  STEMP(start);
  // Yuxin: check double brace in the begining of mars_opt
  if (!donothing) {
    codegen.remove_double_brace();
  }

  ACCTM(remove_double_brace, start, end);
  //  replace begin/end with basic block
  if ("block" == options.get_option("-p")) {
#if USED_CODE_IN_COVERAGE_TEST
    tldm_pragma_check(&codegen, pTopFunc);
    //  func_decl_check(&codegen);

    all_region_to_basicblock(&codegen, pTopFunc, "graph");
    all_region_to_basicblock(&codegen, pTopFunc, "task");
  } else if ("mars_gen_preproc" == options.get_option("-p")) {
    //  printf("Hello mars_gen_preproc\n");
    mars_gen_preproc(&codegen, pTopFunc, options);
  } else if ("outline" == options.get_option("-p")) {
    //  ZP: only need to execute in testing pass
    testing_xml_gen(&codegen, pTopFunc, false);

    if ("simple" == options.get_option("-a")) {
      //  No iteration domain and access patten analysis
      mars_gen_check_task_interface_format(&codegen, pTopFunc, options);
      tldm_extraction_mars_gen(&codegen, pTopFunc, options);
    } else {
      //  tldm_extraction_and_outlining_new(&codegen, pTopFunc, options);
    }
  } else if ("linearize" == options.get_option("-p")) {
    //  ZP: do not need to perform linearization
    //  linearize_array_top(&codegen, pTopFunc);
  } else if ("tldm_gen" == options.get_option("-p")) {
    string str = tldm_gen_top(&codegen, pTopFunc);
    write_string_into_file(str, sTldmFile);
  } else if ("access_wrapper" == options.get_option("-p")) {
    assert(
        pTopFunc);  //  must specify kernel top func to perform access wrapping
    assert(pTopFunc != sg_project);
    assert(codegen.IsFunctionDeclaration(pTopFunc));
    access_wrapper_top(&codegen, pTopFunc, options);
  } else if ("pipeline_insert" == options.get_option("-p")) {
    //  ZP: 2014-08-21: pipeline insertion has bugs: 1. bus_e1 case is
    //  inserted; 2. innermost judgement has problem when there is if-condition
    //  outside for loop
    //  pipeline_insert(&codegen, pTopFunc, options);

    //  ZP: fixed for ap_bus requirement for memcpy in pre_evaluation
    if (options.get_option("-a") == "dse") {
      for (int i = 0; i < codegen.GetGlobalNum(); i++) {
        void *sg_scope_global = codegen.GetGlobal(i);
        codegen.AddHeader(
            "\n int __mcp_i; \n #define memcpy(x,y,len) for (__mcp_i = 0; "
            "__mcp_i < (len)/4; __mcp_i++) (x)[__mcp_i] = (y)[__mcp_i];\n",
            sg_scope_global);
      }
    }
  } else if ("opencl_gen" == options.get_option("-p")) {
    opencl_gen(&codegen, pTopFunc, options, XMD_GEN, is_keep_code);
  } else if ("opencl_gen_debug" == options.get_option("-p")) {
    opencl_gen(&codegen, pTopFunc, options, RUNTIME_GEN, is_keep_code);
  } else if ("opencl_gen_pcie" == options.get_option("-p")) {
    opencl_gen(&codegen, pTopFunc, options, PCIE_GEN, is_keep_code);
  } else if ("cpu_timer_gen" == options.get_option("-p")) {
    user_main_gen(&codegen, pTopFunc);
    cpu_timer_gen(&codegen, pTopFunc, CPU_GEN);
  } else if ("prof_gen" == options.get_option("-p")) {
    prof_gen(&codegen, pTopFunc);
  } else if ("profiling" == options.get_option("-p")) {
    profiling_top(&codegen, pTopFunc, options);
  } else if ("instrument" == options.get_option("-p")) {
    string instrument_type = options.get_option("-a", 0);
    vector<string> include_file;
    for (i = 1; i < (size_t)options.get_option_num("-a"); i++) {
      include_file.push_back(options.get_option("-a", i));
    }
    if (instrument_type == "")
      instrument_type = "test";  //  default type
    if (instrument_type == "test_and_cpu_gen_timer") {
      user_main_gen(&codegen, pTopFunc);
      cpu_timer_gen(&codegen, pTopFunc, CPU_GEN);
      instrument_type = "test";  //  default type
    }
    instrument_gen(&codegen, pTopFunc, instrument_type, include_file);

  } else if ("testing" == options.get_option("-p")) {
    //  added by Hui: generate testing.src automatically
    testing_xml_gen(&codegen, pTopFunc, true);
  } else if ("cpu_ref_add_timer" == options.get_option("-p")) {
    cpu_ref_add_timer(&codegen, pTopFunc, options);
#endif
  } else if ("none" == options.get_option("-p")) {
    //  do nothing.
  } else if ("show_pool" == options.get_option("-p")) {
    printf("Pass show_pool: \n");

    // tree_visit((SgProject*)pTopFunc);
    pool_visit(static_cast<SgProject *>(pTopFunc));
    stop_point();

    ptree(pTopFunc, options);

    printf("Pass show_pool: done \n");

    //  do nothing.
#if USED_CODE_IN_COVERAGE_TEST
  } else if ("preprocessing" == options.get_option("-p")) {
    preprocessing(&codegen, pTopFunc, options);
#endif
  } else if ("delinearization" == options.get_option("-p")) {
    local_array_delinearization(&codegen, pTopFunc, options);
  } else {
    init_pass_func_table();

    MARS_OPT_PASS_FUNC pass_func = get_pass_func(options.get_option("-p"));
    //  std::cout<<options.get_option("-p")<<std::endl;
    if (pass_func != nullptr) {
      (*pass_func)(&codegen, pTopFunc, options);
    } else {
      printf("[mars_opt] ERROR: Unrecognized pass \"%s\"\n\n",
             options.get_option("-p").c_str());
      assert(0);
      exit(0);
    }
  }
  STEMP(start);
  //  Sean: 20170523 remove double brace to avoid defuse assertion failure
  if (!donothing) {
    codegen.remove_double_brace();
    codegen.clean_empty_aggregate_initializer(nullptr);
    codegen.clean_implict_conversion_operator(nullptr);
  }
  ACCTM(remove_double_brace_other_clean, start, end);
  //  dump histories into Json file
  string jsonFileName = "history_" + options.get_option("-p") + ".json";
  STEMP(start);
  writeHistoriesToJson(
      jsonFileName, getSerializableHistories(&codegen, codegen.getHistories()));
  ACCTM(writeHistoriesToJson, start, end);

  //  check reference induction algorithm
  STEMP(start);
#if HIS_CHECK
  checkReferenceInduction(&codegen, orgIdMap, options.get_option("-p"));
  saveForInterPasses(&codegen, options.get_option("-p"), true);
#endif
  ACCTM(HIS_CHECK, start, end);

  //  cout << "Code generation ..." << endl;
  codegen.GenerateCode();

  return 1;
}

int extract_top_opencl(const vector<string> &vecSrcList, string sTldmFile) {
#if USED_CODE_IN_COVERAGE_TEST
  size_t i, j;
  CSageCodeGen codegen;
  void *sg_project = codegen.OpenSourceFile(vecSrcList);

  if (0) {
    vector<string> vecTldmPragmas;
    codegen.TraverseSimple(sg_project, "preorder", GetTLDMInfo,
                           &vecTldmPragmas);

    string str = "";
    for (i = 0; i < vecTldmPragmas.size(); i++) {
      str += vecTldmPragmas[i] + "\n";
    }
    write_string_into_file(str, sTldmFile);
  }

  if (1) {
    vector<void *> vecFuncs;
    codegen.GetNodesByType(sg_project, "preorder", "SgFunctionDeclaration",
                           &vecFuncs);

    string str = "<> :: (top__)\n";
    string prefix = "\t";
    str += "{\n";

    for (i = 0; i < vecFuncs.size(); i++) {
      string sFuncName = codegen.GetFuncName(vecFuncs[i]);
      if (sFuncName.find("_kernel") != string::npos &&
          sFuncName.find("_kernel") + strlen("_kernel") == sFuncName.size()) {
        //  str += sFuncName + " :\n";
        int len = sFuncName.size() - strlen("_kernel");
        string sTaskName = sFuncName.substr(0, len);

        vector<string> vecTldmPragmas;
        codegen.TraverseSimple(vecFuncs[i], "preorder", GetTLDMInfo,
                               &vecTldmPragmas);
        for (j = 0; j < vecTldmPragmas.size(); j++) {
          string sTLDMStmt =
              translate_pragma(sTaskName, vecTldmPragmas[j], prefix);
          if (sTLDMStmt != "")
            str += prefix + sTLDMStmt + "\n";
        }
      }

      if (sFuncName.find("_kernel_detected") != string::npos &&
          sFuncName.find("_kernel_detected") + strlen("_kernel_detected") ==
              sFuncName.size()) {
        //  str += sFuncName + " :\n";
        int len = sFuncName.size() - strlen("_kernel_detected");
        string sTaskName = sFuncName.substr(0, len);

        vector<string> vecTldmPragmas;
        codegen.TraverseSimple(vecFuncs[i], "preorder", GetTLDMInfo,
                               &vecTldmPragmas);
        for (j = 0; j < vecTldmPragmas.size(); j++) {
          string sTLDMStmt =
              translate_pragma(sTaskName, vecTldmPragmas[j], prefix);
          if (sTLDMStmt != "")
            str += prefix + sTLDMStmt + "\n";
        }
      }
    }
    str += "};\n\n";

    write_string_into_file(str, sTldmFile);
  }
#endif
  return 1;
}

//  KillSigs - Signals that represent that we have a bug, and our prompt
//  //  termination has been ordered.
static const int KillSigs[] = {SIGILL, SIGTRAP, SIGABRT, SIGFPE,
                               SIGBUS, SIGSEGV, SIGQUIT};
static void dump_trace(int Sig) {
  fprintf(stderr, "Print backtrace: \n");
  void *buffer[255];
  const int calls = backtrace(buffer, sizeof(buffer) / sizeof(void *));
  backtrace_symbols_fd(buffer, calls, STDERR_FILENO);
  exit(EXIT_FAILURE);
}

int64_t INIT_TIME;
vector<AccTime *> accTimes;

int main(int argc, char *argv[]) {
  STEMP(INIT_TIME);

  for (auto S : KillSigs) {
    signal(S, dump_trace);
  }

  //  /////////////////////////////////////////////  /
  //  Set stack size (default 7.4MB for gcc)
  const rlim_t kStackSize = 64 * 1024 * 1024;  //  min stack size = 512 MB
  struct rlimit rl;
  int result;

  result = getrlimit(RLIMIT_STACK, &rl);
  if (result == 0) {
    if (rl.rlim_cur < kStackSize) {
      rl.rlim_cur = kStackSize;
      result = setrlimit(RLIMIT_STACK, &rl);
      if (result != 0) {
        fprintf(stderr, "setrlimit returned result = %d\n", result);
        exit(0);
      }
    }
  }

  try {
    ROSE_INITIALIZE;

    int i;
    int ret;
    CInputOptions options;
    options.set_flag("-t", 1, 0);
    options.set_flag("-e", 1);
    options.set_flag("-p", 1);
    options.set_flag("-o", 1, 0);
    options.set_flag("-x", 1, 0);
    options.set_flag("-a", 20, 0);
    options.set_flag("-c", 1, 0);
    options.set_flag("", 100000, 2);

    if (options.parse(argc, argv) == 0) {
      cout << "Merlin Compiler version: " << BUILD_VERSION << endl;
      cout << "Build date: " << BUILD_DATE << endl;
      printf("Usage: mars_opt file0 file1 -e [cl|c] -p pass_name [-a arg0] [-a "
             "arg1] ... \n");
      printf("       file_list: C/openCL source file list \n");
      exit(0);
    }

    g_debug_mode = 0;
    if ("debug" == options.get_option_key_value("-a", "debug_mode")) {
      cout << "debug mode" << endl;
      g_debug_mode = 1;
    }

    string sTopFunc = options.get_option("-t");
    if (0 == options.get_option_num("-o")) {
      options.add_option("-o", sTopFunc + ".tldm");
    }

    vector<string> vecSrcFileList;

    //  No matter C or Cl files, we will change the file name to c file
    for (i = 1; i < options.get_option_num(""); i++) {
      string sFile = options.get_option("", i);
      string ext = sFile.substr(sFile.find(".") + 1);
      if (ext == "cl") {
        string sFileOrg = sFile;
        sFile = sFile.substr(0, sFile.find(".")) + ".c";
        string Cmd = "cp " + sFileOrg + " " + sFile;
        ret = system(Cmd.c_str());
        if (ret != 0) {
          fprintf(stderr, "Error: command %s failed\n", Cmd.c_str());
          exit(ret);
        }
      }
      vecSrcFileList.push_back(sFile);
    }

    string sTldmFileName = options.get_option("-o", 0);

    ret = system("rm -rf rose_succeed");
    if (ret != 0) {
      fprintf(stderr, "Error: rm -rf rose_succeed\n");
      exit(ret);
    }
    if ("cl" == options.get_option("-e")) {
      extract_top_opencl(vecSrcFileList, sTldmFileName);
    } else {
      extract_top(vecSrcFileList, sTldmFileName, sTopFunc, options);
    }
    ret = system("touch rose_succeed");
    if (ret != 0) {
      fprintf(stderr, "Error: touch rose_succeed, %d\n", ret);
      exit(ret);
    }

    for (i = 1; i < options.get_option_num(""); i++) {
      string sFile = options.get_option("", i);
      string ext = sFile.substr(sFile.find(".") + 1);
      if (ext == "cl") {
        string sCFile = sFile.substr(0, sFile.find(".")) + ".c";
        string Cmd = "rm -rf " + sFile;
        ret = system(Cmd.c_str());
        if (ret != 0) {
          fprintf(stderr, "Error: %s faild %d\n", Cmd.c_str(), ret);
          exit(ret);
        }
      }
    }

    tldm_clean_all_new_annotation();

  } catch (std::exception e) {
    std::cout << "Internal error." << e.what() << std::endl;
    exit(1);
  } catch (...) {
    std::cout << "Internal error." << std::endl;
    exit(1);
  }
#if 0
  if (accTimes.size() > 0) {
    std::cout << "start to print out accumulated times." << endl;
  }
  for (auto accTime : accTimes) {
    std::cout << accTime->to_string() << endl;
  }
#endif

  return 0;
}
