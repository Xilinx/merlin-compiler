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


#include "preprocess.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <string>
#include <boost/tokenizer.hpp>

#include "array_linearize.h"
#include "codegen.h"
#include "mars_ir.h"
#include "mars_ir_v2.h"
#include "mars_message.h"
#include "mars_opt.h"
#include "program_analysis.h"
#include "rose.h"
#include "math.h"

#include "altera_preprocess.h"
#include "array_delinearize.h"
#include "comm_opt.h"
#include "function_inline.h"
#include "postwrap_process.h"
#include "stream_ir.h"
#include "report.h"
using MarsProgramAnalysis::CMarsExpression;
using MarsProgramAnalysis::CMarsRangeExpr;
using MarsProgramAnalysis::CMarsVariable;
using std::ifstream;
using std::istringstream;
using std::unordered_set;
#undef DEBUG
#define DISABLE_DEBUG 1
#if DISABLE_DEBUG
#define DEBUG(stmts)
#else
#define DEBUG(stmts)                                                           \
  do {                                                                         \
    stmts;                                                                     \
  } while (false)
#endif

extern int check_kernel_argument_number(CSageCodeGen *codegen,
                                        vector<void *> vec_kernels,
                                        string tool);
int check_kernel_argument_number_top(CSageCodeGen *codegen, void *pTopFunc,
                                     string tool);
extern bool check_opencl_conflict_name(CSageCodeGen *codegen, void *TopFunc,
                                       bool is_intel);
//  bool check_uniquify_function(CSageCodeGen *codegen, void *TopFunc);
bool check_kernel_name(CSageCodeGen *codegen, void *TopFunc, bool pure_kernel);
extern int check_const_array_size(CSageCodeGen *codegen, void *TopFunc);

extern void GetTLDMInfo_withPointer4(void *sg_node, void *pArg);

extern int local_static_replace_top(CSageCodeGen *codegen, void *pTopFunc);

extern int constant_propagation_top(CSageCodeGen *codegen, void *pTopFunc,
                                    const CInputOptions &option,
                                    bool assert_generation);

vector<void *> get_top_kernels(CSageCodeGen *codegen, void *pTopFunc) {
  vector<pair<void *, string>> vecTldmPragmas;
  codegen->TraverseSimple(pTopFunc, "preorder", GetTLDMInfo_withPointer4,
                          &vecTldmPragmas);
  //  printf("number = %d\n",vecTldmPragmas.size());
  vector<void *> vecTopKernels;
  for (size_t i = 0; i < vecTldmPragmas.size(); i++) {
    string sFilter;
    string sCmd;
    map<string, pair<vector<string>, vector<string>>> mapParams;
    tldm_pragma_parse_whole(vecTldmPragmas[i].second, &sFilter, &sCmd,
                            &mapParams);
    //  printf("Enter func_inline_top sFilter %s\n\n\n",sFilter.c_str());
    //  printf("Enter func_inline_top sCmd %s\n\n\n",sCmd.c_str());
    boost::algorithm::to_upper(sFilter);
    boost::algorithm::to_upper(sCmd);

    if (CMOST_PRAGMA != sFilter && ACCEL_PRAGMA != sFilter &&
        TLDM_PRAGMA != sFilter) {
      continue;
    }
    if (CMOST_TASK == sCmd) {
      void *sg_kernel_call = codegen->find_kernel_call(vecTldmPragmas[i].first);
      if (sg_kernel_call != nullptr) {
        void *kernel = codegen->GetFuncDeclByCall(sg_kernel_call);
        if (kernel != nullptr) {
          vecTopKernels.push_back(kernel);
        }
      }
      continue;
    }
    if (CMOST_KERNEL == sCmd) {
      void *sg_kernel_call = codegen->find_kernel_call(vecTldmPragmas[i].first);
      if (sg_kernel_call != nullptr) {
        void *kernel = codegen->GetFuncDeclByCall(sg_kernel_call);
        if (kernel != nullptr) {
          vecTopKernels.push_back(kernel);
        }
      }

      void *sg_kernel = codegen->GetNextStmt(vecTldmPragmas[i].first);
      if ((codegen->IsFunctionDeclaration(sg_kernel) != 0) &&
          (codegen->GetFuncBody(sg_kernel) != nullptr)) {
        if (std::find(vecTopKernels.begin(), vecTopKernels.end(), sg_kernel) ==
            vecTopKernels.end()) {
          vecTopKernels.push_back(sg_kernel);
        }
      }
      continue;
    }
  }
  return vecTopKernels;
}

int preprocess_top(CSageCodeGen *codegen, void *pTopFunc,
                   const CInputOptions &options) {
  PreProcess instance(codegen, pTopFunc, options);
  instance.run();
  return 0;
}

void PreProcess::init() {
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
  if ("on" == mOptions.get_option_key_value("-a", "pure_kernel")) {
    mPure_kernel = true;
    cout << "[MARS-PARALLEL-MSG] Enable pure kernel mode.\n";
  }

  string effort_level = mOptions.get_option_key_value("-a", "effort");
  if (effort_level == "low") {
    mEffort = LOW;
  } else if (effort_level == "standard") {
    mEffort = STANDARD;
  } else if (effort_level == "medium") {
    mEffort = MEDIUM;
  } else if (effort_level == "high") {
    mEffort = HIGH;
  }
}

void PreProcess::build_mars_ir(bool check_pragma, bool pragma_in_loop,
                               bool build_node) {
  //  build Mars IR
  mMars_ir.clear();
  mMars_ir.get_mars_ir(m_ast, mTopFunc, mOptions, build_node, check_pragma,
                       pragma_in_loop);

  mValid &= !mMars_ir.isErrorOut();
}

void PreProcess::build_mars_ir_v2(bool build_node) {
  //  build Mars IR v2
  mMars_ir_v2.clear();
  mMars_ir_v2.build_mars_ir(m_ast, mTopFunc, false, build_node);
}

void PreProcess::clear_mars_ir() {
  //  clear Mars IR
  mMars_ir.clear();
}

bool PreProcess::run() {
  cout << "==============================================" << std::endl;
  cout << "-----=# Pragma Process Optimization Start#=----\n";
  cout << "==============================================" << std::endl
       << std::endl;

  bool Changed = false;

  //  generate kernel list
  kernel_file_list_gen(m_ast, mTopFunc, static_cast<int>(mXilinx_flow));

  if (!mNaive_tag) {
    m_ast->fix_defuse_issues(nullptr, false);
  }

  //  if (m_ast->IsMixedCAndCPlusPlusDesign() != 0) {
  //    dump_critical_message(PROCS_ERROR_9);
  //    mValid = false;
  //  }

  Changed |= preprocessing();

  //  //////////////////////////////////////////  /
  //  1. Checkers for pragmas
  //  //////////////////////////////////////////  /
  build_mars_ir(mPure_kernel, false, true);

  //  mValid &= check_address_of_port_array(m_ast, mTopFunc);

  if (mAltera_flow) {
    if (mEffort > STANDARD) {
      //  Yuxin: 2016Nov18
      check_func_inline_legality(m_ast, mTopFunc);
    }
  }

  RemoveKernelFunDeclStaticModifier();
  //  //////////////////////////////////////////  /
  //  2. Checkers for C++ syntax in kernel
  //  //////////////////////////////////////////  /
  //  check_valid_top(m_ast, mTopFunc);
  if (!mNaive_tag && mAltera_flow) {
    if (mEffort > STANDARD) {
      remove_dummy_return_top(m_ast, mTopFunc);
    }
  }

  //  string tool = "Intel";
  //  if (mAltera_flow) {
  //    tool = "Intel";
  //  } else {
  //    tool = "Xilinx";
  //  }
  //  check_kernel_argument_number_top(m_ast, mTopFunc, tool);
  //  check_kernel_argument_top(m_ast, mTopFunc);

  build_mars_ir_v2(false);

  //  checkMemberFunction();

  //  if (mPure_kernel) {
  //    checkOverloadInKernelFunction();
  //    PreCheckLocalVariableWithNonConstantDimension();
  //  } else {
  //    PostCheckLocalVariableWithNonConstantDimension();
  //  }
  //
  //  check_opencl_conflict_name(m_ast, mTopFunc, mAltera_flow);

  if (!mNaive_tag) {
    map<void *, string> loop2file;
    map<void *, string> loop2line;
    clear_mars_ir();
    build_mars_ir(false, false, true);

    Changed |= CanonicalizeLoop(&loop2file, &loop2line);

    if (Changed) {
      m_ast->reset_func_decl_cache();
      m_ast->reset_func_call_cache();
      m_ast->init_defuse_range_analysis();
      clear_mars_ir();
      build_mars_ir(false, false, true);
    }

    checkNonCanonicalLoop(&loop2file, &loop2line);

    std::ifstream my_file("conflict_type_def");
    if (my_file.good()) {
      my_file.close();
      system("rm -f conflict_type_def");
      throw std::exception();
    }
    Changed |= StandardizeLoop();

    if (Changed) {
      m_ast->reset_func_decl_cache();
      m_ast->reset_func_call_cache();
      m_ast->init_defuse_range_analysis();
    }
  }
  check_result();
  //  //////////////////////////////////////////  /
  //  3. Checkers for SW/HW interface
  //  //////////////////////////////////////////  /

  //  //////////////////////////////////////////  /
  //  4. Checkers for QoR limitations
  //  //////////////////////////////////////////  /
  clear_mars_ir();
  build_mars_ir(false, false, true);

  Changed |= processPragma(false);

  Changed |= postprocessing();

  if (mAltera_flow) {
    Changed |= local_static_replace_top(m_ast, mTopFunc);
  }

  if (mAltera_flow) {
    if (Changed) {
      m_ast->init_defuse_range_analysis();
    }
    if (mEffort >= MEDIUM) {
      build_mars_ir_v2(true);
      check_empty_kernel();
      check_multiple_nodes();
      //  Yuxin: 2016Nov18
      check_single_linear_index(m_ast, mTopFunc);
      check_identical_indices(m_ast, mTopFunc);
      check_comm_pre(m_ast, mTopFunc);
    }
  }

  if (!mNaive_tag) {
    parse_aggregate_init();
  }

  check_result();

  //  Youxiang 20170330 check partital range
  //  check_range_analysis();

  return Changed;
}

bool PreProcess::insertParallelPragma(CMirNode *node,
                                      set<CMirNode *> *p_visited) {
  bool Changed = false;
  if (p_visited->count(node) <= 0) {
    p_visited->insert(node);
  } else {
    return false;
  }
  vector<CMirNode *> child = node->get_all_child(true);
  for (size_t i = 0; i != child.size(); ++i) {
    Changed |= insertParallelPragma(child[i], p_visited);
  }
  if (node->is_function) {
    if (mXilinx_flow) {
      //  PR1028
      string new_pragma = std::string(HLS_PRAGMA) + " INLINE ";
      void *sg_pragma = m_ast->CreatePragma(new_pragma, node->ref);
      m_ast->PrependChild(node->ref, sg_pragma);
    }
    return Changed;
  }

  for (size_t i = 0; i < node->pragma_table.size(); i++) {
    auto decl = node->pragma_table[i];
    CAnalPragma ana_pragma(m_ast);
    bool errorOut;
    ana_pragma.PragmasFrontendProcessing(decl, &errorOut, false, true);
    if (ana_pragma.is_line_buffer() || ana_pragma.is_hls_pragma()) {
      continue;
    }
    string pragmaString = m_ast->GetPragmaString(decl);
    m_ast->AddComment_v1(node->ref, "Original: #pragma " + pragmaString);
    m_ast->RemoveStmt(decl);
  }

  node->pragma_table.clear();

  void *sg_loop = m_ast->TraceUpToLoopScope(node->ref);
  if ((m_ast->IsDoWhileStatement(sg_loop) != 0) ||
      (m_ast->IsWhileStatement(sg_loop) != 0)) {
    string str_loop = m_ast->UnparseToString(sg_loop).substr(0, 20);
    string loop_info =
        "'" + str_loop + "' " + getUserCodeFileLocation(m_ast, sg_loop, true);
    dump_critical_message(PROCS_WARNING_36(loop_info));
    return Changed;
  }

  if (checkPragmaLocation(node)) {
    return Changed;
  }

  string new_pragma =
      std::string(ACCEL_PRAGMA) + " " + CMOST_PARALLEL + " " + CMOST_complete;
  void *sg_pragma = m_ast->CreatePragma(new_pragma, node->ref);
  m_ast->PrependChild(node->ref, sg_pragma);
  CAnalPragma ana_pragma(m_ast);
  ana_pragma.set_pragma_type(CMOST_PARALLEL);
  ana_pragma.add_attribute(CMOST_complete, "");
  node->add_pragma(ana_pragma);
  node->pragma_table.push_back(sg_pragma);
  return true;
}

bool PreProcess::processPragma(bool pragma_in_loop) {
  bool Changed = false;

  // Transform HLS pipeline into ACCEL pipeline flatten
  Changed |= parseHLSPragma(pragma_in_loop);
  if (Changed) {
    clear_mars_ir();
    build_mars_ir(false, pragma_in_loop, true);
  }

  vector<CMirNode *> all_nodes;
  mMars_ir.get_topological_order_nodes(&all_nodes);
  set<CMirNode *> visited;
  for (size_t i = 0; i != all_nodes.size(); ++i) {
    CMirNode *node = all_nodes[i];
    if (visited.count(node) > 0) {
      continue;
    }
    Changed |= processPragma(node, false, &visited);
  }

  return Changed;
}

bool PreProcess::processPragma(CMirNode *node, bool flatten,
                               set<CMirNode *> *p_visited) {
  bool Changed = false;
  vector<CMirNode *> childs = node->get_all_child();
  if (flatten) {
    Changed |= insertParallelPragma(node, p_visited);
    return Changed;
  }
  p_visited->insert(node);
  bool current_flatten = false;
  if (!node->is_function) {
    current_flatten = ((node->has_opt_pragmas()) != 0) &&
                      !node->get_attribute(CMOST_flatten_option).empty();
    bool is_fgpip = true;
    if (current_flatten) {
      if (m_ast->ContainsUnsupportedLoop(node->ref, true)) {
        void *sg_loop = m_ast->TraceUpToLoopScope(node->ref);
        string str_loop = m_ast->UnparseToString(sg_loop).substr(0, 20);
        string loop_info = "'" + str_loop + "' " +
                           getUserCodeFileLocation(m_ast, sg_loop, true);
        dump_critical_message(PROCS_WARNING_31(loop_info));
        is_fgpip = false;
      }
    }
    Changed |= generateNewPragma(node, is_fgpip);
  }
  flatten |= current_flatten;
  for (size_t i = 0; i != childs.size(); ++i) {
    Changed |= processPragma(childs[i], flatten, p_visited);
  }
  return Changed;
}

bool PreProcess::generateNewPragma(CMirNode *node, bool is_fgpip) {
  bool Changed = false;

  for (size_t i = 0; i < node->pragma_table.size(); i++) {
    auto decl = node->pragma_table[i];
    if (m_ast->is_floating_node(decl) != 0) {
      continue;
    }
    CAnalPragma ana_pragma(m_ast);
    bool errorOut;
    ana_pragma.PragmasFrontendProcessing(decl, &errorOut, false, true);
    if (ana_pragma.is_line_buffer() || ana_pragma.is_hls_pragma()) {
      continue;
    }
    string pragmaString = m_ast->GetPragmaString(decl);
    m_ast->AddComment_v1(node->ref, "Original: #pragma " + pragmaString);
    m_ast->RemoveStmt(decl);
    Changed = true;
  }
  node->pragma_table.clear();

  bool exclusive = !node->get_attribute(CMOST_exclusive_option).empty();
  node->remove_attribute(CMOST_flatten_option);
  node->remove_attribute(CMOST_exclusive_option);
  if (checkPragmaLocation(node)) {
    return Changed;
  }
  auto vec_pragma = node->get_all_pragmas();
  for (int i = static_cast<int>(vec_pragma.size() - 1); i >= 0; --i) {
    CAnalPragma curr_pragma = vec_pragma[i];
    if (curr_pragma.is_pipeline() && is_fgpip) {
      //  genreated new PIPELINE pragma
      CAnalPragma new_pragma = curr_pragma;
      new_pragma.set_pragma_type(CMOST_PIPELINE);
      new_pragma.remove_attribute(CMOST_parallel_factor);
      new_pragma.remove_attribute(CMOST_complete);
      new_pragma.remove_attribute(CMOST_REDUCTION);
      new_pragma.remove_attribute(CMOST_LINE_BUFFER);
      new_pragma.remove_attribute(CMOST_reduction_scheme);
      void *sg_pragma = new_pragma.update_pragma(nullptr, false, node->ref);
      node->pragma_table.push_back(sg_pragma);
      m_ast->PrependChild(node->ref, sg_pragma);
      //  generate new reduction/line_buffer pragma
      string reduc_var = curr_pragma.get_attribute(CMOST_REDUCTION);
      string linebuf_var = curr_pragma.get_attribute(CMOST_LINE_BUFFER);
      string scheme_str = curr_pragma.get_attribute(CMOST_reduction_scheme);
      if (!reduc_var.empty()) {
        CAnalPragma tmp_pragma(m_ast);
        tmp_pragma.set_pragma_type(CMOST_REDUCTION);
        tmp_pragma.add_attribute(CMOST_variable, reduc_var);
        string scheme;
        if (!scheme_str.empty()) {
          if (scheme_str == CMOST_block || scheme_str == CMOST_cyclic) {
            scheme = scheme_str;
          } else {
            scheme = "auto";
          }
        } else {
          scheme = "auto";
        }
        tmp_pragma.add_attribute(CMOST_scheme, scheme);
        void *sg_reduction_pragma =
            tmp_pragma.update_pragma(nullptr, false, node->ref);
        node->pragma_table.push_back(sg_reduction_pragma);
        m_ast->PrependChild(node->ref, sg_reduction_pragma);
      }
      if (!linebuf_var.empty()) {
        CAnalPragma tmp_pragma(m_ast);
        tmp_pragma.set_pragma_type(CMOST_LINE_BUFFER);
        tmp_pragma.add_attribute(CMOST_variable, linebuf_var);
        void *sg_pragma = tmp_pragma.update_pragma(nullptr, false, node->ref);
        node->pragma_table.push_back(sg_pragma);
        m_ast->PrependChild(node->ref, sg_pragma);
      }
      Changed = true;
    }
    //  generate new PARALLEL pragma
    //  ACCEL flatten does not parallel the parent loop itself
    //  but only parallel its sub loops recursively
    if (curr_pragma.is_parallel() && !exclusive) {
      CAnalPragma new_pragma = curr_pragma;
      new_pragma.set_pragma_type(CMOST_PARALLEL);
      new_pragma.remove_attribute(CMOST_ii);
      new_pragma.remove_attribute(CMOST_REDUCTION);
      new_pragma.remove_attribute(CMOST_reduction_scheme);
      new_pragma.remove_attribute(CMOST_LINE_BUFFER);
      string reduc_var = curr_pragma.get_attribute(CMOST_REDUCTION);
      string linebuf_var = curr_pragma.get_attribute(CMOST_LINE_BUFFER);
      string scheme_str = curr_pragma.get_attribute(CMOST_reduction_scheme);
      void *sg_pragma = new_pragma.update_pragma(nullptr, false, node->ref);
      m_ast->PrependChild(node->ref, sg_pragma);
      node->pragma_table.push_back(sg_pragma);
      new_pragma.remove_attribute(CMOST_parallel_factor);
      //  generate new reduction/line_buffer pragma
      if (!reduc_var.empty()) {
        CAnalPragma tmp_pragma(m_ast);
        tmp_pragma.set_pragma_type(CMOST_REDUCTION);
        tmp_pragma.add_attribute(CMOST_variable, reduc_var);
        string scheme;
        if (!scheme_str.empty()) {
          if (scheme_str == CMOST_block || scheme_str == CMOST_cyclic) {
            scheme = scheme_str;
          } else {
            scheme = "auto";
          }
        } else {
          scheme = "auto";
        }
        tmp_pragma.add_attribute(CMOST_scheme, scheme);
        void *sg_reduction_pragma =
            tmp_pragma.update_pragma(nullptr, false, node->ref);
        node->pragma_table.push_back(sg_reduction_pragma);
        m_ast->PrependChild(node->ref, sg_reduction_pragma);
      }
      if (!linebuf_var.empty()) {
        CAnalPragma tmp_pragma(m_ast);
        tmp_pragma.set_pragma_type(CMOST_LINE_BUFFER);
        tmp_pragma.add_attribute(CMOST_variable, linebuf_var);
        void *sg_pragma = tmp_pragma.update_pragma(nullptr, false, node->ref);
        node->pragma_table.push_back(sg_pragma);
        m_ast->PrependChild(node->ref, sg_pragma);
      }
      Changed = true;
    }
    if (curr_pragma.is_loop_tiling()) {
      //  generate new TILING pragma
      CAnalPragma new_pragma = curr_pragma;
      void *sg_pragma = new_pragma.update_pragma(nullptr, false, node->ref);
      m_ast->PrependChild(node->ref, sg_pragma);
      node->pragma_table.push_back(sg_pragma);
      Changed = true;
    }
    // generate new FALSE_DEP/reduction/line_buffer pragma
    if (curr_pragma.is_false_dep() || curr_pragma.is_reduction()) {
      // Yuxin: May 26, currently we dont move in line buffer pragma
      // since it is applied under a scope
      // curr_pragma.is_line_buffer()
      CAnalPragma new_pragma = curr_pragma;
      string var_str = curr_pragma.get_attribute(CMOST_variable);
      if (curr_pragma.is_false_dep()) {
        new_pragma.set_pragma_type(CMOST_false_dep);
      } else if (curr_pragma.is_reduction()) {
        new_pragma.set_pragma_type(CMOST_REDUCTION);
      }

      if (!var_str.empty()) {
        new_pragma.add_attribute(CMOST_variable, var_str);
      }
      void *sg_pragma = new_pragma.update_pragma(nullptr, false, node->ref);
      m_ast->PrependChild(node->ref, sg_pragma);
      node->pragma_table.push_back(sg_pragma);
      Changed = true;
    }
  }
  return Changed;
}

bool PreProcess::preprocessing() {
  bool Changed = false;
  SgProject *project = static_cast<SgProject *>(mTopFunc);
  map<void *, void *> unrolled_loop2pragma;
  set<void *> unrolled_loops;
  bool errorOut = false;
  for (size_t i = 0; i < project->get_fileList().size(); i++) {
    SgSourceFile *file = isSgSourceFile(project->get_fileList()[i]);
    if ((m_ast->IsFromInputFile(file) == 0) ||
        m_ast->isWithInHeaderFile(file)) {
      continue;
    }
    //  1. check missing assert.h header file
    //    checkAssertHeader(file);

    //  2. add missing bracket
    m_ast->add_missing_brace(file);
    //  4. remove pragma within while, do-while loop structures
    Rose_STL_Container<SgNode *> pragmaStmts =
        NodeQuery::querySubTree(file, V_SgPragmaDeclaration);
    for (auto pragma : pragmaStmts) {
      SgPragmaDeclaration *decl = isSgPragmaDeclaration(pragma);
      ROSE_ASSERT(decl);
      CAnalPragma ana_pragma(m_ast);
      if (ana_pragma.PragmasFrontendProcessing(decl, &errorOut, false)) {
        if (!ana_pragma.is_loop_related()) {
          continue;
        }
        void *loop = m_ast->GetSpecifiedLoopByPragma(decl);
        string pragmaString = decl->get_pragma()->get_pragma();
        std::ostringstream oss;
        oss << "\"" << pragmaString << "\" "
            << getUserCodeFileLocation(m_ast, decl, true);
        string pragma_info = oss.str();
        if ((m_ast->IsDoWhileStatement(loop) != 0) ||
            (m_ast->IsWhileStatement(loop) != 0)) {
          if (ana_pragma.is_pipeline() || ana_pragma.is_hls_pragma()) {
            continue;
          }
          dump_critical_message(PROCS_WARNING_5(pragma_info));
          m_ast->AddComment("Original: #pragma " + pragmaString,
                            m_ast->GetScope(decl));
          m_ast->RemoveStmt(decl);
        } else if (loop == nullptr) {
          if (ana_pragma.is_hls_pragma()) {
            continue;
          }
          if (ana_pragma.is_reduction() || ana_pragma.is_line_buffer()) {
            continue;
          }
          dump_critical_message(PROCS_WARNING_6(pragma_info));
          m_ast->AddComment("Original: #pragma " + pragmaString,
                            m_ast->GetScope(decl));
          m_ast->RemoveStmt(decl);
        } else if (mAltera_flow) {
          if (ana_pragma.is_parallel()) {
            if (!ana_pragma.get_attribute(CMOST_flatten_option).empty()) {
              unrolled_loop2pragma[loop] = decl;
            } else {
              vector<void *> sub_loops;
              m_ast->GetNodesByType_int(m_ast->GetLoopBody(loop), "preorder",
                                        V_SgForStatement, &sub_loops);
              unrolled_loops.insert(sub_loops.begin(), sub_loops.end());
            }
            if (!ana_pragma.get_attribute(CMOST_exclusive_option).empty()) {
              unrolled_loops.insert(loop);
            }
          }
        }
      }
    }
    Changed |= m_ast->simplify_pointer_access(file);
    //  6. remove double brace
    m_ast->remove_double_brace();

    //  7. remove redundant continue statement
    vector<void *> vec_all_loops;
    vector<void *> vec_sub_loops;
    m_ast->GetNodesByType_int(file, "preorder", V_SgForStatement,
                              &vec_sub_loops);
    vec_all_loops.insert(vec_all_loops.end(), vec_sub_loops.begin(),
                         vec_sub_loops.end());
    m_ast->GetNodesByType_int(file, "preorder", V_SgWhileStmt, &vec_sub_loops);
    vec_all_loops.insert(vec_all_loops.end(), vec_sub_loops.begin(),
                         vec_sub_loops.end());
    m_ast->GetNodesByType_int(file, "preorder", V_SgDoWhileStmt,
                              &vec_sub_loops);
    vec_all_loops.insert(vec_all_loops.end(), vec_sub_loops.begin(),
                         vec_sub_loops.end());
    for (auto loop : vec_all_loops) {
      void *body = m_ast->GetLoopBody(loop);
      if (body != nullptr) {
        if (m_ast->RemoveRedundantContinueStmt(body) != 0) {
          Changed = true;
        }
      }
    }
    //  8. replace a->b with (*a).b
    vector<void *> vec_arrow_exp;
    m_ast->GetNodesByType_int(file, "postorder", V_SgArrowExp, &vec_arrow_exp);
    for (auto &arrow_op : vec_arrow_exp) {
      void *rhs, *lhs;
      m_ast->GetExpOperand(arrow_op, &lhs, &rhs);
      void *new_lhs =
          m_ast->CreateExp(V_SgPointerDerefExp, m_ast->CopyExp(lhs));
      void *new_dot_exp =
          m_ast->CreateExp(V_SgDotExp, new_lhs, m_ast->CopyExp(rhs));
      m_ast->ReplaceExp(arrow_op, new_dot_exp);
      Changed = true;
    }
  }

  mValid &= !errorOut;

  //  check coarse grained paralleled loop
  //  for altera flow
  if (mAltera_flow && mEffort >= MEDIUM) {
    vector<void *> coarse_grained_unroll_loops;
    for (auto &loop_info : unrolled_loop2pragma) {
      void *loop = loop_info.first;

      vector<void *> sub_loops;
      m_ast->GetNodesByType_int(m_ast->GetLoopBody(loop), "preorder",
                                V_SgForStatement, &sub_loops);
      bool has_sub_loop = false;
      for (auto &sub_loop : sub_loops) {
        if (unrolled_loops.count(sub_loop) <= 0) {
          has_sub_loop = true;
          break;
        }
      }
      if (has_sub_loop) {
        coarse_grained_unroll_loops.push_back(loop);
      }
    }
    for (auto &loop : coarse_grained_unroll_loops) {
      void *decl = unrolled_loop2pragma[loop];

      string pragmaString = m_ast->GetPragmaString(decl);
      string loop_label = m_ast->get_internal_loop_label(loop);

      std::ostringstream oss;
      oss << "\"" << pragmaString
          << "\" " + getUserCodeFileLocation(m_ast, decl, true);
      string pragma_info = oss.str();
      oss.str("");
      oss.clear();
      oss << "\'" << loop_label << "\' "
          << getUserCodeFileLocation(m_ast, loop, true);
      string loop_info = oss.str();
      dump_critical_message(PROCS_WARNING_8(pragma_info, loop_info));
      m_ast->AddComment("Original: #pragma " + pragmaString,
                        m_ast->GetScope(decl));
      m_ast->RemoveStmt(decl);
    }
  }

  return Changed;
}

bool PreProcess::postprocessing() {
  bool Changed = false;
  SgProject *project = static_cast<SgProject *>(mTopFunc);
  for (size_t i = 0; i < project->get_fileList().size(); i++) {
    SgSourceFile *file = isSgSourceFile(project->get_fileList()[i]);
    if ((m_ast->IsFromInputFile(file) == 0) ||
        m_ast->isWithInHeaderFile(file)) {
      continue;
    }
    //  1. replace sizeof(exp)
    Changed |= m_ast->replace_sizeof(file);
    //  2. split condition expression
    bool LocalChanged;
    do {
      LocalChanged = false;
      vector<void *> vec_cond_exp;
      m_ast->GetNodesByType_int(file, "preorder", V_SgConditionalExp,
                                &vec_cond_exp);
      for (auto cond_exp : vec_cond_exp) {
        void *func_decl = m_ast->TraceUpToFuncDecl(cond_exp);
        if ((func_decl == nullptr) ||
            (((m_ast->IsFromInputFile(func_decl) == 0) &&
              !m_ast->IsTransformation(func_decl)) ||
             m_ast->isWithInHeaderFile(func_decl))) {
          continue;
        }
        SgConditionalExp *sg_cond_exp =
            isSgConditionalExp(static_cast<SgNode *>(cond_exp));
        void *stmt = m_ast->TraceUpToStatement(cond_exp);
        if ((m_ast->IsLoopInit(stmt) != 0) || (m_ast->IsLoopTest(stmt) != 0)) {
          continue;
        }
        if (m_ast->IsVariableDecl(stmt)) {
          void *var_decl = stmt;
          if (m_ast->IsStatic(var_decl) &&
              m_ast->IsConstType(m_ast->GetTypebyVar(
                  m_ast->GetVariableInitializedName(var_decl))))
            // do not convert constant expression into non-constant expression
            continue;
        }
        if (m_ast->is_lvalue(sg_cond_exp) != 0) {
          continue;
        }

        SgType *sg_type = sg_cond_exp->get_type();
        while ((isSgTypedefType(sg_type) != nullptr) ||
               (isSgModifierType(sg_type) != nullptr)) {
          sg_type = isSgType(
              static_cast<SgNode *>(m_ast->GetBaseTypeOneLayer(sg_type)));
        }
        //  Fix Bug1908 anonymous struct type
        if ((isSgTemplateType(sg_type) != nullptr) ||
            (isSgTypeOfType(sg_type) != nullptr) ||
            (isSgTypeVoid(sg_type) != nullptr) ||
            (isSgReferenceType(sg_type) != nullptr) ||
            (isSgArrayType(sg_type) != nullptr) ||
            (isSgFunctionType(sg_type) != nullptr)) {
          continue;
        }
        bool is_assert = false;
        vector<void *> vec_calls;
        m_ast->GetNodesByType_int(sg_cond_exp, "preorder", V_SgFunctionCallExp,
                                  &vec_calls);
        for (auto call : vec_calls) {
          if (m_ast->IsAssertFailCall(call)) {
            is_assert = true;
            break;
          }
        }
        if (is_assert) {
          continue;
        }
        m_ast->ReplaceExpWithStmt(sg_cond_exp);
        Changed = true;
        LocalChanged = true;
        break;
      }
    } while (LocalChanged);
  }

  return Changed;
}

bool PreProcess::checkPragmaLocation(CMirNode *node) {
  void *scope = node->ref;
  void *sg_loop = m_ast->GetParent(scope);
  assert(m_ast->IsForStatement(sg_loop) || m_ast->IsWhileStatement(sg_loop) ||
         m_ast->IsDoWhileStatement(sg_loop));
  if (m_ast->IsForStatement(sg_loop) != 0) {
    if (m_ast->isWithInHeaderFile(sg_loop)) {
      string sFileName = m_ast->GetFileInfo_filename(sg_loop, 1);
      if (mHeaderFileReported.count(sFileName) > 0) {
        return true;
      }
      mHeaderFileReported.insert(sFileName);
      string str_label = m_ast->get_internal_loop_label(sg_loop);
      string loc = getUserCodeFileLocation(m_ast, sg_loop, true);
      dump_critical_message(PROCS_WARNING_1(loc));
      return true;
    }
  } else if ((m_ast->IsWhileStatement(sg_loop) != 0) ||
             (m_ast->IsDoWhileStatement(sg_loop) != 0)) {
    //  FIXME:
    if (m_ast->isWithInHeaderFile(sg_loop)) {
      return true;
    }
  }
  return false;
}

bool PreProcess::CanonicalizeLoop(map<void *, string> *p_loop2file,
                                  map<void *, string> *p_loop2line) {
  bool ret = false;
  vector<CMirNode *> vec_nodes;
  mMars_ir.get_topological_order_nodes(&vec_nodes);

  for (int i = static_cast<int>(vec_nodes.size() - 1); i > 0; i--) {
    CMirNode *node = vec_nodes[i];
    if (node->is_function) {
      continue;
    }
    void *loop_body = node->ref;
    void *sg_loop = m_ast->GetParent(loop_body);
    int ret1 = m_ast->CanonicalizeForLoop(&sg_loop, true);
    string sFileName = m_ast->GetFileInfo_filename(sg_loop, 1);
    string sFileLine = my_itoa(m_ast->GetFileInfo_line(sg_loop));

    if (p_loop2file->find(sg_loop) == p_loop2file->end()) {
      (*p_loop2file)[sg_loop] = sFileName;
    }
    if (p_loop2line->find(sg_loop) == p_loop2line->end()) {
      (*p_loop2line)[sg_loop] = sFileLine;
    }
    ret |= ret1;
  }
  return ret;
}

bool PreProcess::StandardizeLoop() {
  vector<CMirNode *> vec_nodes;
  mMars_ir.get_topological_order_nodes(&vec_nodes);
  bool ret = false;

  for (int i = static_cast<int>(vec_nodes.size() - 1); i > 0; i--) {
    CMirNode *node = vec_nodes[i];
    if (node->is_function) {
      continue;
    }
    void *loop_body = node->ref;
    void *sg_loop = m_ast->GetParent(loop_body);
    ret |= m_ast->StandardizeForLoop(&sg_loop);
  }
  return ret;
}

void PreProcess::checkNonCanonicalLoop(map<void *, string> *p_loop2file,
                                       map<void *, string> *p_loop2line) {
  vector<CMirNode *> kernels;
  set<CMirNode *> all_nodes;
  vector<CMirNode *> all_nodes_vec;
  mMars_ir.get_all_kernel_nodes(&kernels, &all_nodes, &all_nodes_vec);
  bool hint_tag = false;
  for (size_t i = 0; i != all_nodes_vec.size(); ++i) {
    CMirNode *node = all_nodes_vec[i];
    if (node->is_function) {
      continue;
    }
    void *loop_body = node->ref;
    void *sg_loop = m_ast->GetParent(loop_body);
    string sFileName = m_ast->GetFileInfo_filename(sg_loop, 1);
    if (p_loop2file->find(sg_loop) != p_loop2file->end()) {
      sFileName = (*p_loop2file)[sg_loop];
    } else {
      sFileName = m_ast->GetFileInfo_filename(sg_loop, 1);
    }
    string sFileLine = my_itoa(m_ast->GetFileInfo_line(sg_loop));
    //  FIXME: str_loc is null
    string str_loc = getUserCodeFileLocation(m_ast, sg_loop, true);
    string str_loop = m_ast->UnparseToString(sg_loop).substr(0, 20);
    void *iv;
    void *lb;
    void *ub;
    void *step;
    void *body;
    bool inc_dir;
    bool inclusive_bound;
    bool ret = m_ast->IsCanonicalForLoop(sg_loop, &iv, &lb, &ub, &step, &body,
                                         &inc_dir, &inclusive_bound) != 0;
    int64_t nStep = 0;
    if (ret) {
      ret = (m_ast->GetLoopStepValueFromExpr(step, &nStep) != 0);
    }

    bool ret1 = ret;
    //  Yuxin: commented on Dec 04 2016, step 1: canonicalize; step 2: check the
    //  rest non-canonical loops

    if (!ret1) {
      //  FIXME: meta locationt info does not work for non-canonical loop, Yuxin
      //  Nov 30
      string loop_info = "\'" + str_loop + "' " + str_loc;
      bool is_switch = false;
      if (mAltera_flow && mEffort > STANDARD) {
        is_switch = true;
        system("touch merlin_altera_naive.log");
      }

      dump_critical_message(PROCS_WARNING_2(loop_info, is_switch));
    } else {
      bool has_loop_bound_spec = false;
      Rose_STL_Container<SgNode *> pragmaStmts = NodeQuery::querySubTree(
          static_cast<SgNode *>(sg_loop), V_SgPragmaDeclaration);
      for (size_t j = 0; j != pragmaStmts.size(); ++j) {
        void *sg_pragma = pragmaStmts[j];
        void *loop_scope = m_ast->TraceUpToLoopScope(sg_pragma);
        if (loop_scope != sg_loop) {
          continue;
        }
        if (mMars_ir.isHLSTripCount(sg_pragma)) {
          has_loop_bound_spec = true;
          break;
        }
      }
      if (has_loop_bound_spec) {
        continue;
      }
      CMarsRangeExpr var_range = CMarsVariable(sg_loop, m_ast).get_range();
      if ((var_range.is_const_ub() == 0) || (var_range.is_const_lb() == 0)) {
        CMirNode *parent_node = node->get_parent();
        while ((parent_node != nullptr) && !parent_node->is_kernel) {
          if (parent_node->get_parent() != nullptr) {
            parent_node = parent_node->get_parent();
          } else {
            vector<CMirNode *> preds = parent_node->get_all_predecessor();
            if (!preds.empty()) {
              parent_node = preds[0];
            } else {
              parent_node = nullptr;
            }
          }
        }
        var_range.refine_range_in_scope(
            parent_node != nullptr ? parent_node->ref : nullptr, sg_loop);
      }

      if ((var_range.is_const_ub() == 0) || (var_range.is_const_lb() == 0)) {
        //  FIXME: meta locationt info does not work for non-canonical loop,
        //  Yuxin Nov 30
        string loop_info = "'" + str_loop + "' " + str_loc;
        dump_critical_message(PROCS_WARNING_3(loop_info));
        hint_tag = true;
      }
    }
  }
  if (hint_tag) {
    dump_critical_message(PROCS_INFO_2);
  }
}

void PreProcess::RemoveKernelFunDeclStaticModifier() {
  auto kernels = get_top_kernels(m_ast, mTopFunc);

  for (auto kernel : kernels) {
    vector<void *> all_decls = m_ast->GetAllFuncDecl(kernel);

    for (auto decl : all_decls) {
      //  Youxiang: 20170601 remove static modifier of top kernel
      //  functions
      m_ast->UnSetStatic(decl);
    }
  }
}

int PreProcess::remove_dummy_return_top(CSageCodeGen *codegen, void *pTopFunc) {
  printf("    Enter removing dummy return...\n");
  vector<void *> vecTopKernels = get_top_kernels(codegen, pTopFunc);

  bool has_real_return = false;
  for (auto &func : vecTopKernels) {
    void *func_body = codegen->GetFuncBody(func);
    if (func_body == nullptr) {
      continue;
    }
    vector<void *> ret_stmts;
    codegen->GetNodesByType_int(func_body, "preorder", V_SgReturnStmt,
                                &ret_stmts);
    if (ret_stmts.empty()) {
      continue;
    }
    bool local_has_real_return = false;
    if (ret_stmts.size() > 1) {
      has_real_return = local_has_real_return = true;
    } else {
      void *ret_stmt = ret_stmts[0];
      if (codegen->GetChildStmtIdx(func_body, ret_stmt) !=
          codegen->GetChildStmtNum(func_body) - 1) {
        has_real_return = local_has_real_return = true;
      } else if (codegen->IsDummyReturnStmt(ret_stmt) != 0) {
        codegen->RemoveChild(ret_stmt);
      }
    }
    if (local_has_real_return && mEffort > STANDARD && mAltera_flow) {
      string function_string =
          "\'" + codegen->DemangleName(codegen->GetFuncName(func)) + "\' " +
          getUserCodeFileLocation(m_ast, func, true);

      dump_critical_message(PROCS_WARNING_10(function_string));
    }
  }
  if (has_real_return) {
    system("touch merlin_altera_naive.log");
  }
  return 0;
}

void PreProcess::check_multiple_nodes() {
  bool switch_low = false;
  //  for each edge
  set<void *> visited;
  for (auto &edge : mMars_ir_v2.get_all_edges()) {
    //  0. check if the array is an host interface
    void *array = edge->GetPort();
    if (visited.count(array) > 0) {
      continue;
    }
    visited.insert(array);
    //  1. # of access is two
    vector<CMarsEdge *> edges_of_array;
    for (auto &edge_t : mMars_ir_v2.get_all_edges()) {
      void *array_t = edge_t->GetPort();
      if (array_t == array) {
        edges_of_array.push_back(edge_t);
      }
    }
    assert(!edges_of_array.empty());
    vector<CMarsNode *> all_nodes;
    set<CMarsNode *> all_nodes_set;
    for (auto &edge : edges_of_array) {
      CMarsNode *node0 = edge->GetNode(0);
      CMarsNode *node1 = edge->GetNode(1);
      if (all_nodes_set.count(node0) <= 0) {
        all_nodes_set.insert(node0);
        all_nodes.push_back(node0);
      }
      if (all_nodes_set.count(node1) <= 0) {
        all_nodes_set.insert(node1);
        all_nodes.push_back(node1);
      }
    }
    string var_name = m_ast->GetVariableName(array);
    if (m_ast->IsScalarType(array) != 0) {
#if PROJDEBUG
      cout << "Cannot split scalar variable \'" << var_name << "\'"
           << " because its def/use is inaccurate" << endl;
#endif
      continue;
    }
    //  sort all the nodes according to original schedule vector
    sort(all_nodes.begin(), all_nodes.end(),
         [](CMarsNode *node0, CMarsNode *node1) {
           return !(node0->is_after(node1));
         });

    //  at least a write access
    //
    //  check read access
    string var_info =
        "\'" + var_name + "\' " + getUserCodeFileLocation(m_ast, array, true);
    for (auto node : all_nodes) {
      if (!node->port_is_read(array)) {
        continue;
      }
      if (mMars_ir_v2.has_definite_coarse_grained_def(node, array)) {
        continue;
      }
      if (mMars_ir_v2.is_kernel_port(array)) {
#if PROJDEBUG
        cout << "Cannot support synchronization on host interface \'"
             << var_name << "\'" << endl;
#endif
        switch_low = true;
        dump_critical_message(PROCS_WARNING_25(var_info));
        break;
      }
#if PROJDEBUG
      void *scope = node->get_scope();
      cout << "Cannot find definition of read access on shared memory \'"
           << var_name << "\' in scope \'" << m_ast->_up(scope) << " \'"
           << endl;
#endif
      dump_critical_message(PROCS_WARNING_24(var_info));
      switch_low = true;
      break;
    }
  }
  if (switch_low) {
    system("touch merlin_altera_naive.log");
  }
}

void PreProcess::check_func_inline_legality(CSageCodeGen *codegen,
                                            void *pTopFunc) {
  printf("    Enter check function inline legality...\n\n");

  vector<void *> kernels = get_top_kernels(codegen, pTopFunc);
  int kernel_num = kernels.size();

  for (int j = 0; j < kernel_num; ++j) {
    SetVector<void *> sub_funcs;
    SetVector<void *> sub_calls;
    codegen->GetSubFuncDeclsRecursively(kernels[j], &sub_funcs, &sub_calls);
    for (auto &sub_decl : sub_funcs) {
      vector<void *> all_calls;
      codegen->GetFuncCallsFromDecl(sub_decl, nullptr, &all_calls);

      for (int i = static_cast<int>(all_calls.size() - 1); i >= 0; i--) {
        func_inline(codegen, all_calls[i], mEffort, false, true);
      }
    }
  }
}

int check_const_array_size(CSageCodeGen *codegen, void *pTopFunc) {
  printf("    Check constant array size...\n");

  auto kernels = get_top_kernels(codegen, pTopFunc);
  for (auto kernel : kernels) {
    //  get all variable reference from function declaration cross function
    vector<void *> vec_refs;
    codegen->GetNodesByType(kernel, "preorder", "SgVarRefExp", &vec_refs, false,
                            true);
    for (auto ref : vec_refs) {
      void *var_init = codegen->GetVariableInitializedName(ref);
      void *type = codegen->GetTypebyVar(var_init);
      if (codegen->IsConstType(type) == 0) {
        continue;
      }
      if (codegen->IsArrayType(type) == 0) {
        continue;
      }
      cout << "find one const variable = " << codegen->UnparseToString(var_init)
           << endl;
      void *basic_type;
      vector<size_t> basic_size;
      codegen->get_type_dimension(type, &basic_type, &basic_size, var_init);
      basic_type = codegen->GetBaseType(basic_type, true);
      int64_t total_size = 1;
      for (auto size : basic_size) {
        total_size *= size;
      }
      int sizeof_type = codegen->get_bitwidth_from_type_str(
          codegen->UnparseToString(basic_type));
      total_size *=
          codegen->get_sizeof_arbitrary_precision_integer(sizeof_type);
      if (total_size > MAX_CONST_BYTE) {
        string msg1 = "'" + codegen->UnparseToString(var_init) + "'";
        msg1 += getUserCodeFileLocation(codegen, var_init, true);
        string msg2 = std::to_string(total_size);
        dump_critical_message(PROCS_WARNING_41(msg1, msg2));
      }
      cout << "Total size = " << total_size << endl;
    }
  }
  return 0;
}

void PreProcess::check_single_linear_index(CSageCodeGen *codegen,
                                           void *pTopFunc) {
  printf("    Enter check wether if an array can be liniearized...\n\n");

  //  Yuxin Nov 22 2016: just check shared port indices, host-if not included
  //  Host interface will be check after memory burst

  map<void *, int> map_array;  //  the array to transform

  int node_num = mMars_ir_v2.size();

  //  1. get the arrays to transform
  for (int j = 0; j < node_num; ++j) {
    CMarsNode *node = mMars_ir_v2.get_node(j);
    auto s_ports = mMars_ir_v2.get_shared_ports(node);
    for (auto &port : s_ports) {
      if (codegen->IsLocalInitName(port) != 0) {
        map_array[port] = 1;
      }
    }
  }

  //  2. transform each
  map<void *, int>::iterator it1;
  for (it1 = map_array.begin(); it1 != map_array.end(); it1++) {
    void *one_array = it1->first;
    vector<size_t> dim_split_steps;
    map<void *, size_t> mapAlias2BStep;
    bool is_simple = true;
    CMarsExpression zero(codegen, 0L);
    //  Offset by default is 0, for an index after burst might be a variable
    int delinearized = analysis_delinearize(
        codegen, one_array, 0, &dim_split_steps, &mapAlias2BStep, mAltera_flow,
        &is_simple, &zero, nullptr);
    if (!is_simple && (delinearized != 0)) {
      map<void *, vector<CMarsExpression>> mapIndex2Delinear;
      vector<size_t> new_array_dims;
      delinearized = apply_delinearize(
          codegen, one_array, 0, dim_split_steps, &mapAlias2BStep, true,
          &is_simple, &zero, nullptr, &mapIndex2Delinear, &new_array_dims);
      is_simple &= delinearized;
    }
    bool is_linear = true;
    if (is_simple) {
#if PROJDEBUG
      cout << "==> simple form array: " << codegen->_p(one_array) << endl;
#endif
      continue;
    }
    if ((dim_split_steps.empty()) || (dim_split_steps.empty())) {
      is_linear = false;
    }

    if (!is_linear && mEffort > STANDARD) {
      string var_name = "'" + codegen->UnparseToString(one_array) + "'";
      string var_info =
          var_name + " " + getUserCodeFileLocation(m_ast, one_array, true);

      dump_critical_message(PROCS_WARNING_26(var_info));
      system("touch merlin_altera_naive.log");
    }
  }
}

void PreProcess::check_identical_indices(CSageCodeGen *codegen,
                                         void *pTopFunc) {
  printf("    Enter check identical indices in the loop kernels...\n\n");

  int node_num = mMars_ir_v2.size();
  map<string, int> id2msg;

  for (int j = 0; j < node_num; ++j) {
    CMarsNode *node = mMars_ir_v2.get_node(j);
    auto ports = mMars_ir_v2.get_shared_ports(node);
    for (auto &port : ports) {
      set<void *> refs = node->get_port_references(port);
      bool is_identical =
          mMars_ir_v2.check_identical_indices_without_simple_type(refs);
      if (!is_identical && mEffort > STANDARD) {
        auto loops = node->get_loops();
        string port_info = "'" + m_ast->UnparseToString(port) + "'";
        if (!loops.empty()) {
          void *sg_loop = loops[loops.size() - 1];
          string str_loop = m_ast->UnparseToString(sg_loop).substr(0, 20);
          string loop_id = codegen->get_internal_loop_label(sg_loop);
          string loop_info = "loop '" + str_loop + "' (" + loop_id + ")" + " " +
                             getUserCodeFileLocation(m_ast, sg_loop, true);
          if (id2msg.find(loop_id) == id2msg.end()) {
            dump_critical_message(PROCS_WARNING_23(port_info, loop_info));
            id2msg[loop_id] = 1;
            system("touch merlin_altera_naive.log");
          }
        } else {
          auto stmts = node->get_stmts();
          if (stmts.empty()) {
            continue;
          }
          void *sg_stmt1 = stmts[stmts.size() - 1];
          void *sg_stmt0 = stmts[0];
          string str_stmt0 = m_ast->UnparseToString(sg_stmt0).substr(0, 20);
          string str_stmt1 = m_ast->UnparseToString(sg_stmt1).substr(0, 20);
          string stmt_info =
              "statements '" + str_stmt0 + "' to '" + str_stmt1 + "'";
          dump_critical_message(PROCS_WARNING_23(port_info, stmt_info));
          system("touch merlin_altera_naive.log");
        }
      }
    }
  }
}

int PreProcess::check_comm_pre(CSageCodeGen *codegen, void *pTopFunc) {
  printf("    Enter communication pre check ...\n\n");

  int node_num = mMars_ir_v2.size();
  bool valid = true;
  map<void *, MarsCommOpt::mark_struct> loop2mark;
  for (int j = 0; j < node_num; ++j) {
    CMarsNode *node = mMars_ir_v2.get_node(j);
    valid &= check_node(node, &loop2mark, &mMars_ir_v2, true);
  }
  if (!valid) {
    system("touch merlin_altera_naive.log");
    return 0;
  }
  return 1;
}

int local_static_replace(CSageCodeGen *codegen, void *func_decl,
                         set<void *> *visited) {
  vector<void *> all_vars;
  codegen->GetNodesByType_int(func_decl, "preorder", V_SgVariableDeclaration,
                              &all_vars);

  string func_name = codegen->GetFuncName(func_decl, false);
  bool Changed = false;
  for (auto var : all_vars) {
    if (!codegen->IsStatic(var)) {
      continue;
    }
    void *var_init = codegen->GetVariableInitializedName(var);
    vector<void *> vec_refs;
    codegen->get_ref_in_scope(var_init, func_decl, &vec_refs);
    if (vec_refs.empty()) {
      Changed = true;
      codegen->RemoveStmt(var);
      continue;
    }

    //  check whether we can move local static variable into global scope
    void *var_initializer = codegen->GetInitializer(var_init);
    void *var_initializer_rhs = codegen->GetInitializerOperand(var_init);
    if ((var_initializer != nullptr) && (var_initializer_rhs == nullptr)) {
      continue;
    }
    bool have_local_ref = false;
    if (var_initializer_rhs != nullptr) {
      vector<void *> sub_vec_refs;
      codegen->GetNodesByType_int(var_initializer_rhs, "preorder",
                                  V_SgVarRefExp, &sub_vec_refs);
      for (auto ref : sub_vec_refs) {
        auto init = codegen->GetVariableInitializedName(ref);
        if ((codegen->IsLocalInitName(init) != 0) ||
            (codegen->IsArgumentInitName(init) != 0)) {
          have_local_ref = true;
          break;
        }
      }
    }
    //  FIXME: variable name may conflict with existing variable names
    string new_var_name = func_name + "_" + codegen->GetVariableName(var_init);
    new_var_name = codegen->legalizeName(new_var_name);
    codegen->get_valid_global_name(codegen->GetGlobal(func_decl),
                                   &new_var_name);
    void *new_var_decl = codegen->CreateVariableDecl(
        codegen->GetTypebyVar(var_init), new_var_name,
        (var_initializer_rhs != nullptr) && !have_local_ref
            ? codegen->CopyExp(var_initializer_rhs)
            : nullptr,
        codegen->GetGlobal(func_decl));
    void *insert_pos = func_decl;
    while (codegen->IsPragmaDecl(codegen->GetPreviousStmt(insert_pos)) != 0) {
      insert_pos = codegen->GetPreviousStmt(insert_pos);
    }
    codegen->InsertStmt(new_var_decl, insert_pos);
    codegen->SetStatic(new_var_decl);
    void *new_var_init = codegen->GetVariableInitializedName(new_var_decl);

    for (auto ref : vec_refs) {
      codegen->ReplaceExp(ref, codegen->CreateVariableRef(new_var_init));
    }
    if (have_local_ref) {
      string new_var_flag_name = new_var_name + "_flag";
      new_var_flag_name = codegen->legalizeName(new_var_flag_name);
      codegen->get_valid_global_name(codegen->GetGlobal(func_decl),
                                     &new_var_flag_name);
      void *flag_decl = codegen->CreateVariableDecl(
          "int", new_var_flag_name,
          (var_initializer_rhs != nullptr) && !have_local_ref
              ? codegen->CopyExp(var_initializer_rhs)
              : nullptr,
          codegen->GetGlobal(func_decl));
      codegen->SetStatic(flag_decl);
      codegen->InsertStmt(flag_decl, new_var_decl);
      void *flag_init = codegen->GetVariableInitializedName(flag_decl);

      void *basic_block = codegen->CreateBasicBlock(nullptr, false, nullptr);
      void *new_var_assign = codegen->CreateStmt(
          V_SgAssignStatement, codegen->CreateVariableRef(new_var_init),
          codegen->CopyExp(var_initializer_rhs));
      codegen->AppendChild(basic_block, new_var_assign);

      void *flag_assign = codegen->CreateStmt(
          V_SgAssignStatement, codegen->CreateVariableRef(flag_init),
          codegen->CreateConst(1));
      codegen->AppendChild(basic_block, flag_assign);

      void *flag_not_cond =
          codegen->CreateExp(V_SgNotOp, codegen->CreateVariableRef(flag_init));
      void *if_stmt =
          codegen->CreateIfStmt(flag_not_cond, basic_block, nullptr);
      codegen->InsertStmt(if_stmt, var);
    }

    codegen->RemoveStmt(var);
    Changed = true;
  }
  SetVector<void *> sub_funcs;
  SetVector<void *> sub_calls;
  codegen->GetSubFuncDeclsRecursively(func_decl, &sub_funcs, &sub_calls);
  for (auto sub_decl : sub_funcs) {
    if (visited->count(sub_decl) > 0) {
      continue;
    }
    visited->insert(sub_decl);
    Changed |= local_static_replace(codegen, sub_decl, visited);
  }
  return static_cast<int>(Changed);
}

int local_static_replace_top(CSageCodeGen *codegen, void *pTopFunc) {
  cout << "\n====== Local static replace ========>\n";

  vector<void *> kernels = get_top_kernels(codegen, pTopFunc);

  //    cout << "[print kernel:]" << kernels.size() << endl;

  bool Changed = false;
  set<void *> visited_funcs;
  for (auto kernel_decl : kernels) {
    if (visited_funcs.count(kernel_decl) > 0) {
      continue;
    }
    visited_funcs.insert(kernel_decl);
    Changed |= local_static_replace(codegen, kernel_decl, &visited_funcs);
  }
  return static_cast<int>(Changed);
}

void PreProcess::check_empty_kernel() {
  bool empty = mMars_ir_v2.get_all_nodes().empty();
  if (empty) {
    mValidAltera = false;
    dump_critical_message(PROCS_ERROR_40);
  }
}

void PreProcess::parse_aggregate_init() {
  clear_mars_ir();
  build_mars_ir(false, true, true);

  vector<CMirNode *> all_nodes;
  mMars_ir.get_topological_order_nodes(&all_nodes);
  set<SgBasicBlock *> fg_set;
  for (size_t i = 0; i != all_nodes.size(); ++i) {
    CMirNode *node = all_nodes[i];
    if (node->is_fine_grain) {
      fg_set.insert(node->ref);
    }
  }

  m_ast->parse_aggregate();

  clear_mars_ir();
  build_mars_ir(false, true, true);

  vector<CMirNode *> all_nodes1;
  //    set<void *> visited;
  set<CMirNode *> visited;
  mMars_ir.get_topological_order_nodes(&all_nodes1);
  for (size_t i = 0; i != all_nodes1.size(); ++i) {
    CMirNode *node = all_nodes1[i];
    if (!node->is_fine_grain && fg_set.find(node->ref) != fg_set.end()) {
      vector<CMirNode *> child = node->get_all_child();
      for (size_t i = 0; i != child.size(); ++i) {
        CMirNode *node_c = child[i];
        if (visited.find(child[i]) == visited.end()) {
          visited.insert(child[i]);
          if (node_c->has_opt_pragmas() == 0) {
            string new_pragma = std::string(ACCEL_PRAGMA) + " " +
                                CMOST_PARALLEL + " " + CMOST_complete;
            void *sg_pragma = m_ast->CreatePragma(new_pragma, node_c->ref);
            m_ast->PrependChild(node_c->ref, sg_pragma);
            CAnalPragma ana_pragma(m_ast);
            ana_pragma.set_pragma_type(CMOST_PARALLEL);
            ana_pragma.add_attribute(CMOST_complete, "");
            node_c->add_pragma(ana_pragma);
            node_c->pragma_table.push_back(sg_pragma);
          }
        }
      }
    }
  }
}

void PreProcess::check_range_analysis() {
  auto top_kernels = get_top_kernels(m_ast, mTopFunc);
  set<void *> all_kernels;
  for (auto kernel : top_kernels) {
    all_kernels.insert(kernel);
    SetVector<void *> sub_kernels;
    SetVector<void *> sub_calls;
    m_ast->GetSubFuncDeclsRecursively(kernel, &sub_kernels, &sub_calls);
    all_kernels.insert(sub_kernels.begin(), sub_kernels.end());
  }

  int ret = constant_propagation_top(m_ast, mTopFunc, mOptions, true);
  if (ret != 0) {
    m_ast->reset_range_analysis();
  }

  auto range_map = MarsProgramAnalysis::get_range_analysis(m_ast);
  map<string, void *> loc_map;
  //  Sort by file location
  for (auto var_range : range_map) {
    void *var = var_range.first;
    string var_info = "\'" + m_ast->_up(var) + "\'";
    var_info += " " + getUserCodeFileLocation(m_ast, var, true);
    loc_map[var_info] = var;
  }
  for (auto var_loc : loc_map) {
    string var_info = var_loc.first;
    void *var = var_loc.second;
    CMarsRangeExpr range = range_map[var];
    if (isSgVarRefExp(static_cast<SgNode *>(var)) != nullptr) {
      continue;
    }
    void *func_decl = m_ast->TraceUpToFuncDecl(var);
    if ((func_decl == nullptr) || all_kernels.count(func_decl) <= 0) {
      continue;
    }
    if ((range.has_lb() != 0) && (range.has_ub() != 0)) {
      continue;
    }
    if ((range.has_lb() == 0) && (range.has_ub() == 0)) {
      continue;
    }
    bool no_lb = range.has_lb() == 0;
    dump_critical_message(SYNCHK_WARNING_3(no_lb, var_info));
  }
}

void PreProcess::check_result() {
  if (!mValid || (mAltera_flow && !mValidAltera) ||
      (mXilinx_flow && !mValidXilinx)) {
    throw std::exception();
  }
}

bool PreProcess::parseHLSPragma(bool pragma_in_loop) {
  bool Changed = false;
  vector<void *> vec_pragmas;
  m_ast->GetNodesByType_int(mTopFunc, "preorder", V_SgPragmaDeclaration,
                            &vec_pragmas);
  vector<void *> remove_pragmas;
  for (size_t i = 0; i < vec_pragmas.size(); i++) {
    auto pragma = vec_pragmas[i];
    bool errorOut = false;
    CAnalPragma ana_pragma(m_ast);
    if (ana_pragma.PragmasFrontendProcessing(pragma, &errorOut, false, true)) {
      if (ana_pragma.get_vendor_type() != HLS_PRAGMA)
        continue;
      if (ana_pragma.get_pragma_type() != HLS_PIPELINE)
        continue;
      string ii_str = ana_pragma.get_attribute(HLS_PIPELINE_II);
      void *sg_loop = m_ast->TraceUpToLoopScope(pragma);
      if (sg_loop == nullptr)
        continue;
      void *sg_pragma = nullptr;
      if (ii_str == "") {
        string new_pragma = "ACCEL PIPELINE FLATTEN";
        sg_pragma = m_ast->CreatePragma(
            new_pragma, pragma_in_loop ? m_ast->GetLoopBody(sg_loop)
                                       : m_ast->GetScope(sg_loop));
      } else {
        string new_pragma = "ACCEL PIPELINE FLATTEN II=" + ii_str;
        sg_pragma = m_ast->CreatePragma(
            new_pragma, pragma_in_loop ? m_ast->GetLoopBody(sg_loop)
                                       : m_ast->GetScope(sg_loop));
      }
      if (sg_pragma != nullptr) {
        if (pragma_in_loop)
          m_ast->PrependChild(m_ast->GetLoopBody(sg_loop), sg_pragma);
        else
          m_ast->InsertStmt(sg_pragma, sg_loop);
        m_ast->RemoveStmt(pragma);
        Changed = true;
      }
    }
  }
  return Changed;
}
