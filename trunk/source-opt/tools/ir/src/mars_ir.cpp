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


#include <iostream>
#include <string>

#include "codegen.h"
#include "mars_ir.h"
#include "mars_message.h"
#include "mars_opt.h"
#include "program_analysis.h"
#include "rose.h"
#include "report.h"
#include "cmdline_parser.h"
#include "file_parser.h"
#include "tldm_annotate.h"
#include "xml_parser.h"
using std::unordered_set;
#define USED_CODE_IN_COVERAGE_TEST 0
#if 0
bool CMarsIr::is_kernel(const string &func_name) {
    const string suffix = "_kernel";
    string::size_type pos = func_name.find(suffix);
    if (pos == string::npos || pos + suffix.size() != func_name.size())
        return false;

    string task_name = func_name.substr(0, func_name.size() - suffix.size());
    if (kernel.count(task_name)) {
        return true;
    }
    return false;
}
#endif
bool CMarsIr::is_kernel(CSageCodeGen *codegen, void *func_decl) {
  for (set<void *>::iterator i = kernel_decl.begin(), e = kernel_decl.end();
       i != e; ++i) {
    if (codegen->isSameFunction(*i, func_decl)) {
      return true;
    }
  }
  return false;
}

bool CMarsIr::is_task(CSageCodeGen *codegen, void *func_decl) {
  if (task_decl.find(codegen->GetFuncName(func_decl)) == task_decl.end()) {
    return false;
  }
  for (set<void *>::iterator i = kernel_decl.begin(), e = kernel_decl.end();
       i != e; ++i) {
    if (codegen->isSameFunction(*i, func_decl)) {
      return true;
    }
  }
  return false;
}

bool CMarsIr::is_bus_port(CSageCodeGen *codegen, void *func_decl,
                          size_t arg_index) {
  void *found_kernel = nullptr;
  for (set<void *>::iterator i = kernel_decl.begin(), e = kernel_decl.end();
       i != e; ++i) {
    if (codegen->isSameFunction(*i, func_decl)) {
      found_kernel = *i;
      break;
    }
  }
  if (nullptr == found_kernel) {
    return false;
  }
  unordered_set<string> interface;
  auto ret = kernel2interface.find(found_kernel);
  if (ret != kernel2interface.end()) {
    vector<unordered_set<string>> vec_interface = ret->second;
    if (arg_index < vec_interface.size()) {
      interface = vec_interface[arg_index];
    }
  }
  // 1) If it's Vitis flow, we generate MM interfaces
  // 2) If hls flow, we assume default interfaces (only non-array pointers will
  // have MM interfaces) 3) for both 1) and 2), we need to consider some ports
  // with interface pragmas 4) for non m_axi ports, we shouldn't do burst,
  // streaming, or coalescing
  if (m_flow == XILINX_SDX || m_flow == INTEL_AOCL) {
    if (!interface.empty()) {
      for (auto one_interface : interface) {
        if (HLS_GLOBAL_MEM_MODE.count(one_interface) > 0 ||
            one_interface == CMOST_global_memory)
          return true;
      }
      return false;
    }
    return true;
  }
  if (m_flow == XILINX_HLS) {
    for (auto one_interface : interface) {
      if (HLS_GLOBAL_MEM_MODE.count(one_interface) > 0 ||
          one_interface == CMOST_global_memory)
        return true;
    }
    return false;
  }
  return false;
}

bool CMarsIr::is_reduction(CSageCodeGen *codegen, void *func_decl) {
  for (set<void *>::iterator i = reduction_decl.begin(),
                             e = reduction_decl.end();
       i != e; ++i) {
    if (codegen->isSameFunction(*i, func_decl)) {
      return true;
    }
  }
  return false;
}

void CMarsIr::get_mars_ir(CSageCodeGen *codegen, void *pTopFunc,
                          const CInputOptions &options, bool build_node,
                          bool report, bool pragma_in_loop,
                          bool while_support) {
  clear();

  string tool_type = options.get_option_key_value("-a", "impl_tool");
  if (tool_type == "aocl") {
    m_flow = INTEL_AOCL;
  } else if (tool_type == "sdaccel") {
    string tool_version = options.get_option_key_value("-a", "tool_version");
    if (tool_version == "vivado_hls" || tool_version == "vitis_hls")
      m_flow = XILINX_HLS;
    else
      m_flow = XILINX_SDX;
  }

  //  Build the AST used by ROSE
  //
  //  1. analyze pragmas to get all the kernel functions
  vector<void *> pragmas;
  codegen->GetNodesByType_int(pTopFunc, "preorder", V_SgPragmaDeclaration,
                              &pragmas);
  set<string> task_set;
  for (size_t i = 0; i != pragmas.size(); ++i) {
    SgNode *pragma = static_cast<SgNode *>(pragmas[i]);
    SgPragmaDeclaration *decl = isSgPragmaDeclaration(pragma);
    assert(decl);
    CAnalPragma ana_pragma(codegen);
    if (ana_pragma.PragmasFrontendProcessing(decl, &mErrorOut, report)) {
      //  skip compiler generated statement
      void *next_stmt = codegen->GetNextStmt(decl, false);
      if (next_stmt == nullptr) {
        continue;
      }
      if (ana_pragma.is_task()) {
        //  FIXME: we need to support function outline in the future,
        //  the following may be a basic block which is outlined into a
        //  kernel function
        if (codegen->IsBasicBlock(next_stmt) != 0) {
        } else {
          vector<void *> vec_expr;
          codegen->GetNodesByType_int(next_stmt, "preorder",
                                      V_SgFunctionCallExp, &vec_expr);
          void *expr = nullptr;
          if (vec_expr.size() == 1) {
            expr = vec_expr[0];
          }
          if (vec_expr.size() != 1 || (codegen->IsFunctionCall(expr) == 0)) {
            string loc = getUserCodeFileLocation(codegen, decl, true);
            //  string msg = "Cannot find a function call"
            //             " directly after the task pragma " + loc;
            //  bool multiple_func_calls = vec_expr.size() > 1;
            //  if (report)
            //  dump_critical_message(PROCS_ERROR_12(loc,
            //  multiple_func_calls));
            //  throw std::exception();
            //  mErrorOut = true;
            //  exit(-1);
          }
          if (expr != nullptr) {
            void *kernel = codegen->GetFuncDeclByCall(expr);
            if (kernel == nullptr) {
              string func_name = codegen->GetFuncNameByCall(expr, true);
              task_set.insert(func_name);
              string func_info = "\'" + func_name + "\' " +
                                 getUserCodeFileLocation(codegen, expr, true);
              void *kernel_decl = codegen->GetFuncDeclByCall(expr, 0);
              if (kernel_decl != nullptr) {
                int param_num = codegen->GetFuncParamNum(kernel_decl);
                for (int j = 0; j < param_num; ++j) {
                  void *param = codegen->GetFuncParam(kernel_decl, j);
                  void *sg_base_type = codegen->get_array_base_type(param);
                  void *sg_base_orig_type =
                      codegen->GetOrigTypeByTypedef(sg_base_type, true);
                  if (codegen->IsAnonymousType(sg_base_orig_type) != 0) {
                    if (report) {
                      string type_info =
                          "\'" + codegen->GetStringByType(sg_base_type) + "\'";
                      dump_critical_message(PROCS_ERROR_41(type_info));
                      mErrorOut = true;
                    }
                  }
                }
                if (!codegen->IsCompilerGenerated(kernel_decl) &&
                    !codegen->IsTransformation(kernel_decl) &&
                    (codegen->IsFromInputFile(kernel_decl) == 0)) {
                  if (report) {
                    dump_critical_message(PROCS_ERROR_42(func_info));
                  }
                } else {
                  //  string msg = "Cannot find the function definition of " +
                  //  func_info;
                  if (report) {
                    dump_critical_message(PROCS_ERROR_16(func_info));
                  }
                }
              }
              //  throw std::exception();
              mErrorOut = true;
            } else {
              void *rec_func = nullptr;
              if (codegen->IsRecursiveFunction(kernel, &rec_func) != 0) {
                mErrorOut = true;
                if (report) {
                  void *owner = nullptr;
                  if (codegen->IsTemplateInstantiationFunctionDecl(rec_func)) {
                    owner = codegen->GetTemplateFuncDecl(rec_func);
                  } else if (codegen->IsTemplateInstantiationMemberFunctionDecl(
                                 rec_func)) {
                    owner = codegen->GetTemplateMemFuncDecl(rec_func);
                  } else {
                    owner = rec_func;
                  }
                  string func_name = codegen->GetFuncName(owner, true);
                  string location = getUserCodeFileLocation(codegen, owner);
                  string func_info = "\'" + func_name + "\' (" + location + ")";
                  //  string msg = "Cannot support recursive function " +
                  //  func_info;

                  dump_critical_message(PROCS_ERROR_17(func_info));
                }
                throw std::exception();
              }

              if (report) {
                checkKernelFunctionInterface(codegen, kernel, expr);
              }
              kernel_decl.insert(kernel);
            }
          }
        }
      }

      if (ana_pragma.is_kernel()) {
        void *kernel = nullptr;
        void *expr = nullptr;
        if (codegen->IsFunctionDeclaration(next_stmt) == 0) {
          vector<void *> vec_expr;
          codegen->GetNodesByType_int(next_stmt, "preorder",
                                      V_SgFunctionCallExp, &vec_expr);
          if (vec_expr.size() == 1) {
            expr = vec_expr[0];
          }
          if (vec_expr.size() != 1 || (codegen->IsFunctionCall(expr) == 0)) {
            string loc = getUserCodeFileLocation(codegen, decl, true);
            //  string msg = "Cannot find a function declaration or function
            //  call"
            //             " directly after the kernel pragma " + loc;
            bool multiple_func_calls = vec_expr.size() > 1;
            if (report) {
              dump_critical_message(PROCS_ERROR_14(loc, multiple_func_calls));
            }
            //  throw std::exception();
            mErrorOut = true;
            exit(-1);
          } else {
            kernel = codegen->GetFuncDeclByCall(expr);
          }
        } else {
          kernel = next_stmt;
        }

        if (kernel != nullptr) {
          void *rec_func = nullptr;
          if (codegen->IsRecursiveFunction(kernel, &rec_func) != 0) {
            if (report) {
              mErrorOut = true;
              void *owner = nullptr;
              if (codegen->IsTemplateInstantiationFunctionDecl(rec_func)) {
                owner = codegen->GetTemplateFuncDecl(rec_func);
              } else if (codegen->IsTemplateInstantiationMemberFunctionDecl(
                             rec_func)) {
                owner = codegen->GetTemplateMemFuncDecl(rec_func);
              } else {
                owner = rec_func;
              }
              string func_name = codegen->GetFuncName(owner, true);
              string location = getUserCodeFileLocation(codegen, owner);
              string func_info = "\'" + func_name + "\' (" + location + ")";
              //  string msg = "Cannot support recursive function " + func_info;

              dump_critical_message(PROCS_ERROR_17(func_info));
            }
            throw std::exception();
          }
          if (report) {
            checkKernelFunctionInterface(codegen, kernel, expr);
          }
          kernel_decl.insert(kernel);
        } else {
          cout << "no kernel found after kernel pragma " << codegen->_p(decl)
               << " " << codegen->get_location(decl, true, true) << endl;
        }
      }
      if (ana_pragma.is_reduction()) {
        void *expr = codegen->GetExprFromStmt(next_stmt);
        if (expr == nullptr) {
          continue;
        }
        if (codegen->IsFunctionCall(expr) != 0) {
          void *reduction = codegen->GetFuncDeclByCall(expr);
          if (reduction == nullptr) {
            string func_name = codegen->GetFuncNameByCall(expr, true);
            string func_info = "\'" + func_name + "\'";
            string pragma_info = " \'" + decl->get_pragma()->get_pragma() +
                                 "\' " +
                                 getUserCodeFileLocation(codegen, expr, true);
            string msg = "Cannot find the reduction function " + func_info +
                         "  specified by pragma " + pragma_info;

            dump_critical_message(PROCS_ERROR_26(func_info, pragma_info));
            //  throw std::exception();
            mErrorOut = true;
          } else {
            bool ret = check_valid_reduction(codegen, reduction);
            if (!ret) {
              string func_info =
                  "'" + codegen->GetFuncName(reduction, true) + "' " +
                  getUserCodeFileLocation(codegen, reduction, true);
              if (report) {
                dump_critical_message(PROCS_ERROR_34(func_info));
                //  invalid_reduc.insert(reduction);
              }
              mErrorOut = true;
            } else {
              reduction_decl.insert(reduction);
            }
          }
        }
      }
    }
  }
  if (kernel_decl.empty()) {
    string msg =
        "Cannot find any proper task pragma or kernel pragma in the program.\n";
    msg += "  Please add a task pragma right before the kernel";
    msg += " function call in the program or\n";
    msg += "  add a kernel pragma right before the kernel function";
    if (report) {
      dump_critical_message(PROCS_ERROR_19);
    }
    //  throw std::exception();
    mErrorOut = true;
  }

  //  generate task_decl
  string file = TASK_KERNEL_JSON;
  map<string, map<string, string>> task_kernel;  //  task-kernel-file_name
  std::ifstream ifile(file);
  if (ifile) {
    readInterReport(file, &task_kernel);
  }
  if (!task_kernel.empty()) {
    map<string, map<string, string>>::iterator iter0;
    for (iter0 = task_kernel.begin(); iter0 != task_kernel.end(); iter0++) {
      string task_name = iter0->first;
      task_decl.insert(task_name);
    }
  }

  if (kernel_decl.size() > 1) {
    for (auto kernel : kernel_decl) {
      string kernel_name = codegen->GetFuncName(kernel);
      if (task_decl.find(kernel_name) != task_decl.end()) {
        continue;
      }
      if (task_set.find(kernel_name) != task_set.end()) {
        continue;
      }
      //  YX: 20170725 bug1478 check whether one of sub functions is also kernel
      SetVector<void *> sub_funcs;
      SetVector<void *> sub_calls;
      codegen->GetSubFuncDeclsRecursively(kernel, &sub_funcs, &sub_calls);
      for (auto sub_func : sub_funcs) {
        if (is_kernel(codegen, sub_func)) {
          string msg = "Cannot support kernel ";
          string sub_func_info =
              "\'" + codegen->GetFuncName(sub_func) + "\' " +
              getUserCodeFileLocation(codegen, sub_func, true);
          msg += sub_func_info + " which is called by kernel ";
          string func_info = "\'" + codegen->GetFuncName(kernel) + "\' (" +
                             getUserCodeFileLocation(codegen, kernel, true);
          dump_critical_message(PROCS_ERROR_45(sub_func_info, func_info));
          mErrorOut = true;
        }
      }
    }
  }
  for (auto kernel : kernel_decl) {
    //  fix bug1775
    if ("main" == codegen->GetFuncName(kernel)) {
      string msg = "Cannot support kernel ";
      string func_info = "\'" + codegen->GetFuncName(kernel) + "\' (" +
                         getUserCodeFileLocation(codegen, kernel, true);
      dump_critical_message(PROCS_ERROR_47(func_info));
      mErrorOut = true;
    }
  }
  vector<void *> func_defs;
  codegen->GetNodesByType_int(pTopFunc, "preorder", V_SgFunctionDefinition,
                              &func_defs);
  SetVector<void *> all_kernel_funcs;
  SetVector<void *> all_sub_calls;
  SetVector<void *> all_func_decls;
  //  2. match kernel functions
  for (auto &kernel : kernel_decl) {
    string func_name = codegen->GetFuncName(kernel, true);
    //    cout << "[print ] ====== kernel ===== " <<codegen->_p(kernel)<<endl;
    set<void *> def_set;
    for (auto &func_def : func_defs) {
      void *func_decl = codegen->GetFuncDeclByDefinition(func_def);
      //    cout << "[print ] " <<codegen->_p(func_decl)<<endl;
      all_func_decls.insert(func_decl);
      if (codegen->isSameFunction(kernel, func_decl)) {
        def_set.insert(func_def);
        all_kernel_funcs.insert(func_decl);
        //  3. collect all the sub functions belonging to kernel part
        codegen->GetSubFuncDeclsRecursively(func_decl, &all_kernel_funcs,
                                            &all_sub_calls);
      }
    }
    if (def_set.empty()) {
      string func_info = "\'" + func_name + "\'";
      //  string msg = "Cannot find the function definition of " + func_info;
      if (report) {
        dump_critical_message(PROCS_ERROR_16(func_info));
      }
      //  throw std::exception();
      mErrorOut = true;
    } else if (def_set.size() > 1) {
      string func_info = "\'" + func_name + "\':\n";
      size_t num = 0;
      for (auto &def : def_set) {
        num++;
        func_info += "  " + my_itoa(num) + ": " +
                     getUserCodeFileLocation(codegen, def, true);
        if (num != def_set.size()) {
          func_info += "\n";
        }
      }
      //  string msg = "Finding multiple definitions for the function " +
      //  func_info;
      if (report) {
        dump_critical_message(PROCS_ERROR_20(func_info));
      }
      //  throw std::exception();
      mErrorOut = true;
    } else {
      void *func_def = *def_set.begin();
      void *func_decl = codegen->GetFuncDeclByDefinition(func_def);
      vector<void *> vec_pragma;
      codegen->GetNodesByType_int(func_def, "preorder", V_SgPragmaDeclaration,
                                  &vec_pragma);
      map<string, unordered_set<string>> port2interface;
      for (auto pragma : vec_pragma) {
        SgPragmaDeclaration *decl =
            isSgPragmaDeclaration(static_cast<SgNode *>(pragma));
        assert(decl);
        CAnalPragma ana_pragma(codegen);
        if (ana_pragma.PragmasFrontendProcessing(decl, &mErrorOut, false)) {
          if (ana_pragma.is_interface()) {
            if (!ana_pragma.get_attribute(CMOST_type).empty()) {
              string port = ana_pragma.get_attribute(CMOST_variable);
              string type = ana_pragma.get_attribute(CMOST_type);
              boost::algorithm::to_upper(type);
              port2interface[port].insert(type);
              continue;
            }
          }
        }
        {
          // try to see whether it is HLS pragma
          ana_pragma.PragmasFrontendProcessing(decl, &mErrorOut, false, true);
          if (ana_pragma.is_interface()) {
            string port = ana_pragma.get_attribute(HLS_port);
            string mode = ana_pragma.get_hls_interface_mode();
            boost::algorithm::to_upper(mode);
            port2interface[port].insert(mode);
          }
        }
      }
      vector<unordered_set<string>> vec_interface;
      int arg_num = codegen->GetFuncParamNum(func_decl);
      for (int i = 0; i != arg_num; ++i) {
        void *arg = codegen->GetFuncParam(func_decl, i);
        string arg_name = codegen->GetVariableName(arg);
        unordered_set<string> interface;
        if (port2interface.count(arg_name) > 0) {
          interface = port2interface[arg_name];
        }
        vec_interface.push_back(interface);
      }
      kernel2interface[kernel] = vec_interface;
      //  #if PROJDEBUG
      //            cout << "interface for kernel : " <<
      //  codegen->_p(kernel) << endl;             for (auto interface :
      //  kernel2interface[kernel])                 cout <<
      //  interface
      //  << " ";             cout << endl; #endif
    }
  }

  //  filter memcpy functions
  {
    SetVector<void *> tmp_kernel_funcs;
    for (auto kernel_func : all_kernel_funcs) {
      string func_name = codegen->GetFuncName(kernel_func, false);
      if (func_name.find("memcpy_wide_bus_") == 0) {
        continue;
      }
      tmp_kernel_funcs.insert(kernel_func);
    }
    all_kernel_funcs.clear();
    all_kernel_funcs.insert(tmp_kernel_funcs.begin(), tmp_kernel_funcs.end());
  }

  if (build_node) {
    build_mars_node(codegen, pTopFunc, all_kernel_funcs, pragma_in_loop, report,
                    while_support);
  }
}

void CMarsIr::build_mars_node(CSageCodeGen *codegen, void *pTopFunc,
                              const SetVector<void *> &all_kernel_funcs,
                              bool pragma_in_loop, bool report,
                              bool while_support) {
  SgProject *project =
      static_cast<SgProject *>(pTopFunc);  //  frontend (argc, argv);
  //  codegen->remove_double_brace();
  for (size_t i = 0; i < project->get_fileList().size(); i++) {
    SgSourceFile *file = isSgSourceFile(project->get_fileList()[i]);
    /*FIXME: file list traverse*/

    CMirNodeFuncSet new_CMirNodeFuncSet(codegen, file, all_kernel_funcs,
                                        pragma_in_loop, report);

    ir_list.insert(ir_list.end(), new_CMirNodeFuncSet.begin(),
                   new_CMirNodeFuncSet.end());
  }
  set<void *> all_funcs;
  map<void *, CMirNode *> func2Node;
  for (iterator i = ir_list.begin(); i != ir_list.end(); ++i) {
    CMirNode *top_node = i->get_top_node();
    if (!top_node->is_function) {
      continue;
    }
    void *block = top_node->ref;
    void *func_def = codegen->GetParent(block);
    void *func_decl = codegen->GetFuncDeclByDefinition(func_def);
    string func_name = codegen->GetFuncName(func_decl, true);
    if (is_kernel(codegen, func_decl)) {
      top_node->set_kernel();
    }
    if (is_task(codegen, func_decl)) {
      top_node->set_task();
    }
    all_funcs.insert(func_decl);
    func2Node[func_decl] = top_node;
  }
  for (auto func_decl : all_kernel_funcs) {
    vector<void *> func_calls;
    codegen->GetFuncCallsFromDecl(func_decl, nullptr, &func_calls);
    CMirNode *top_node = func2Node[func_decl];
    if (top_node == nullptr) {
      continue;
    }
    for (auto func_call : func_calls) {
      void *loop = codegen->TraceUpToLoopScope(func_call);
      if (loop != nullptr) {
        void *loop_body = codegen->GetLoopBody(loop);
        CMirNode *node = get_node(loop_body);
        if (node != nullptr) {
          node->add_one_child(top_node);
          top_node->add_one_predecessor(node);
        }
      } else {
        void *caller = codegen->GetEnclosingNode("Function", func_call);
        if (caller != nullptr) {
          void *func_body = codegen->GetFuncBody(caller);
          CMirNode *node = get_node(func_body);
          if (node != nullptr) {
            node->add_one_child(top_node);
            top_node->add_one_predecessor(node);
          }
        }
      }
    }
  }

  build_fine_grain(codegen, while_support);
}

void CMarsIr::build_fine_grain(CSageCodeGen *codegen, bool while_support) {
  vector<CMirNode *> vec_nodes;
  bool has_non_recursive_funcs = get_topological_order_nodes(&vec_nodes);
  assert(has_non_recursive_funcs);
  //  mark the grananity of each loop
  for (int i = static_cast<int>(vec_nodes.size() - 1); i >= 0; --i) {
    CMirNode *node = vec_nodes[i];
    vector<CMirNode *> childs = node->get_all_child();
    bool has_loops = false;
    bool has_loops_without_pragma = false;
    //  cout
    //  <<"[print:]"<<childs.size()<<"\n"<<node->ref->unparseToString()<<endl;
    for (size_t j = 0; j != childs.size(); ++j) {
      //  cout<<"[print:]" << codegen->_p(childs[j]->ref)<<endl;
      if (childs[j]->has_loop) {
        has_loops = true;
      }
      if (childs[j]->has_loop_without_pragma) {
        has_loops_without_pragma = true;
      }
    }
    bool has_unsupported_loop =
        codegen->ContainsUnsupportedLoop(node->ref, false, while_support);
    has_loops_without_pragma |= has_unsupported_loop;
    has_loops |= has_unsupported_loop;
    if (!node->is_function) {
      if (!has_loops) {
        if (!node->is_complete_unroll()) {
          has_loops = true;
        }
        node->is_fine_grain = true;

        //  Yuxin, Dec 15 2016, if the loop contains memcpy/assert,
        //  it is not fine grain
        vector<void *> vecExpr;
        codegen->GetNodesByType_int_compatible(node->ref, V_SgFunctionCallExp,
                                               &vecExpr);
        for (size_t j = 0; j < vecExpr.size(); j++) {
          void *sg_call = vecExpr[j];
          if ((codegen->GetFuncNameByCall(sg_call) == "memcpy")) {
            if (codegen->is_memcpy_with_small_const_trip_count(
                    sg_call, MERLIN_SMALL_TRIP_COUNT_THRESHOLD) == 0) {
              node->is_fine_grain = false;
            }
          }
        }
      } else {
        node->is_fine_grain = false;
      }
      node->is_innermost = !has_loops_without_pragma;
      has_loops_without_pragma = true;
    } else {
      if (node->funcname_without_args == "memcpy") {
        has_loops = has_loops_without_pragma = true;
      }
      node->is_fine_grain = !has_loops;
      node->is_innermost = false;
    }

    node->has_loop = has_loops;
    node->has_loop_without_pragma = has_loops_without_pragma;
  }
}

bool CMarsIr::every_trace_is_bus(CSageCodeGen *codegen, void *kernel_decl,
                                 void *target_arr, void *pos) {
  //  Return value
  //  true: connected to the bus;
  //  false: on chip buffer;
  map<void *, bool> res;
  set<void *> visited;
  bool ret =
      trace_to_bus(codegen, kernel_decl, target_arr, &res, pos, &visited);
  if (ret) {
    //  parse successfully
    for (map<void *, bool>::iterator i = res.begin(); i != res.end(); ++i) {
      //  found a on chip buffer
      if (!i->second) {
        return false;
      }
    }
    return !res.empty();
  }
  //  To be conservative, we do not know whether it is connected to bus port
  return false;
}

void CMarsIr::trace_to_bus_ports(CSageCodeGen *codegen, void *kernel_decl,
                                 void *target_arr, set<void *> *top_args,
                                 void *pos) {
  //  Return value
  //  true: connected to the bus;
  //  false: on chip buffer;
  map<void *, bool> res;
  set<void *> visited;
  bool ret =
      trace_to_bus(codegen, kernel_decl, target_arr, &res, pos, &visited);
  if (ret) {
    //  parse successfully
    for (map<void *, bool>::iterator i = res.begin(); i != res.end(); ++i) {
      //  found a on chip buffer
      if (i->second) {
        top_args->insert(i->first);
      }
    }
  }
}

bool CMarsIr::any_trace_is_bus(CSageCodeGen *codegen, void *kernel_decl,
                               void *target_arr, map<void *, bool> *res,
                               void *pos) {
  //  Return value
  //  true: connected to the bus;
  //  false: on chip buffer;
  bool ret = trace_to_bus(codegen, kernel_decl, target_arr, res, pos);
  if (ret) {
    //  parse successfully
    for (auto trace : *res) {
      //  found a on chip buffer
      if (trace.second) {
        return true;
      }
    }
    return false;
  }
  //  To be conservative, we do not know whether it is connected to bus port
  return true;
}

// Yuxin: Dec/11/2019, rewrite this function using codegen api
void CMarsIr::check_hls_partition(CSageCodeGen *codegen, void *pTopFunc,
                                  bool include_interface) {
  cout << "Check existing HLS partition pragma...\n";

  vector<void *> vec_pragmas;
  codegen->GetNodesByType_int(pTopFunc, "preorder", V_SgPragmaDeclaration,
                              &vec_pragmas);

  for (auto pragma : vec_pragmas) {
    string str_pragma = codegen->GetPragmaString(pragma);
    bool errorOut = false;
    CAnalPragma ana_pragma(codegen);
    if (ana_pragma.PragmasFrontendProcessing(pragma, &errorOut, false, true)) {
      if (ana_pragma.get_vendor_type() != "HLS")
        continue;
      if (ana_pragma.get_pragma_type() != HLS_ARR_PARTITION)
        continue;
      vector<void *> find_vars;
      string s_var = ana_pragma.get_attribute(CMOST_variable);
      cout << "variable: " << s_var << endl;
      SgInitializedName *var_init =
          codegen->getInitializedNameFromName(pragma, s_var, true);
      if (var_init != nullptr) {
        find_vars.push_back(var_init);
      } else {
        bool err = false;
        string msg_str = "";
        auto vars =
            codegen->GetVariablesUsedByPragma(pragma, &err, &msg_str, false);
        for (auto var : vars)
          find_vars.push_back(var.first);
      }
      if (find_vars.size() > 0) {
        for (auto var_init : find_vars) {
          if (!include_interface) {
            void *func_decl = codegen->GetEnclosingNode("Function", var_init);
            map<void *, bool> res;
            if (any_trace_is_bus(codegen, func_decl, var_init, &res,
                                 codegen->GetScope(pragma))) {
              continue;
            }
          }
          if (hls_partition_table.count(var_init) > 0) {
            hls_partition_table[var_init].push_back(pragma);
          } else {
            vector<void *> m_pragmas;
            m_pragmas.push_back(pragma);
            hls_partition_table[var_init] = m_pragmas;
          }
#if PROJDEBUG
          cout << "found arr: " << codegen->_up(var_init) << endl;
          cout << "   Add HLS array partition pragma: " << str_pragma << endl;
#endif
        }
      }
    }
  }
}

void CMarsIr::print_hls_partition(CSageCodeGen *codegen) {
  cout << "[print] Existing partition pragmas:\n";
  for (auto p : hls_partition_table) {
    cout << "arr: " << codegen->_up(p.first) << endl;
    cout << p.second.size() << endl;
    for (auto pp : p.second) {
      cout << codegen->_up(pp) << endl;
    }
  }
}

void CMarsIr::splitString(string str, string delim, vector<string> *results) {
  size_t cutAt;
  while ((cutAt = str.find_first_of(delim)) != std::string::npos) {
    if (cutAt > 0) {
      results->push_back(str.substr(0, cutAt));
    }
    str = str.substr(cutAt + 1);
  }

  if (str.length() > 0) {
    results->push_back(str);
  }
}

string CMarsIr::getDimString(SgPragmaDeclaration *decl) {
  SgPragma *pragma = decl->get_pragma();
  string pragmaString = pragma->get_pragma();
  string eqstr;

  vector<string> constructs;
  splitString(pragmaString, " ", &constructs);

  for (vector<string>::iterator itt = constructs.begin();
       itt != constructs.end(); itt++) {
    string subPragma_0 = (*itt);
    boost::algorithm::to_upper(subPragma_0);
    if (subPragma_0 == CMOST_dim) {
      eqstr = (*(++itt));

      if (eqstr == "=") {
        if (++itt != constructs.end()) {
          subPragma_0 = (*(itt));
          return subPragma_0;
        }
      }
    } else if (subPragma_0.find(CMOST_dim) != string::npos) {
      vector<string> constructs_0;
      subPragma_0 = *itt;
      splitString(subPragma_0, "=", &constructs_0);
      for (vector<string>::iterator it = constructs_0.begin();
           it != constructs_0.end(); it++) {
        string subPragma = (*it);
        boost::algorithm::to_upper(subPragma);

        if (subPragma == CMOST_dim) {
          if (++it != constructs.end()) {
            subPragma = (*(it));
            return subPragma;
          }
        }
      }
    }
  }
  return "1";
}

string CMarsIr::getFactorString(SgPragmaDeclaration *decl) {
  SgPragma *pragma = decl->get_pragma();
  string pragmaString = pragma->get_pragma();
  string eqstr;

  vector<string> constructs;
  splitString(pragmaString, " ", &constructs);

  for (vector<string>::iterator itt = constructs.begin();
       itt != constructs.end(); itt++) {
    string subPragma_0 = (*itt);
    boost::algorithm::to_upper(subPragma_0);
    if (subPragma_0 == CMOST_factor) {
      eqstr = (*(++itt));

      if (eqstr == "=") {
        if (++itt != constructs.end()) {
          subPragma_0 = (*(itt));
          return subPragma_0;
        }
      }
    } else if (subPragma_0.find(CMOST_factor) != string::npos) {
      vector<string> constructs_0;
      subPragma_0 = *itt;
      splitString(subPragma_0, "=", &constructs_0);
      for (vector<string>::iterator it = constructs_0.begin();
           it != constructs_0.end(); it++) {
        string subPragma = (*it);
        boost::algorithm::to_upper(subPragma);

        if (subPragma == CMOST_factor) {
          if (++it != constructs.end()) {
            subPragma = (*(it));
            return subPragma;
          }
        }
      }
    }
  }
  return "";
}

bool CMarsIr::getCompleteString(SgPragmaDeclaration *decl) {
  SgPragma *pragma = decl->get_pragma();
  string pragmaString = pragma->get_pragma();
  string eqstr;

  vector<string> constructs;
  splitString(pragmaString, " ", &constructs);

  for (vector<string>::iterator itt = constructs.begin();
       itt != constructs.end(); itt++) {
    string subPragma_0 = (*itt);
    boost::algorithm::to_upper(subPragma_0);
    if (subPragma_0 == CMOST_complete) {
      return true;
    }
  }
  return false;
}

bool CMarsIr::isHLSPartition(void *sg_decl) {
  SgPragmaDeclaration *decl =
      isSgPragmaDeclaration(static_cast<SgNode *>(sg_decl));
  SgPragma *pragma = decl->get_pragma();
  string pragmaString = pragma->get_pragma();
  string eqstr;

  vector<string> constructs;
  splitString(pragmaString, " ", &constructs);
  bool hls_id = false;

  for (vector<string>::iterator itt = constructs.begin();
       itt != constructs.end(); itt++) {
    string subPragma_0 = (*itt);
    boost::algorithm::to_upper(subPragma_0);

    if (subPragma_0 == HLS_PRAGMA || subPragma_0 == AP_PRAGMA) {
      hls_id = true;
    } else if (hls_id && subPragma_0 == CMOST_ARR_PARTITION) {
      return true;
    }
  }
  return false;
}

bool CMarsIr::isHLSTripCount(void *sg_decl) {
  SgPragmaDeclaration *decl =
      isSgPragmaDeclaration(static_cast<SgNode *>(sg_decl));
  SgPragma *pragma = decl->get_pragma();
  string pragmaString = pragma->get_pragma();
  string eqstr;

  vector<string> constructs;
  splitString(pragmaString, " ", &constructs);
  bool hls_id = false;

  for (vector<string>::iterator itt = constructs.begin();
       itt != constructs.end(); itt++) {
    string subPragma_0 = (*itt);
    boost::algorithm::to_upper(subPragma_0);

    if (subPragma_0 == HLS_PRAGMA || subPragma_0 == AP_PRAGMA) {
      hls_id = true;
    } else if (hls_id && subPragma_0 == HLS_TRIPCOUNT) {
      return true;
    }
  }
  return false;
}

CMarsIr::~CMarsIr() {
  for (size_t i = 0; i != ir_list.size(); ++i) {
    ir_list[i].releaseMemory();
  }
  ir_list.clear();
}

void CMarsIr::clear() {
  for (size_t i = 0; i != ir_list.size(); ++i) {
    ir_list[i].releaseMemory();
  }
  ir_list.clear();
  kernel_decl.clear();
  kernel2interface.clear();
  reduction_decl.clear();
  mErrorOut = false;
}

CMirNode *CMarsIr::get_node(void *body_ref) {
  for (size_t i = 0; i != ir_list.size(); ++i) {
    CMirNode *node = ir_list[i].get_node(body_ref);
    if (node != nullptr) {
      return node;
    }
  }
  return nullptr;
}

void CMarsIr::update_loop_node_info(CSageCodeGen *codegen) {
  vector<CMirNode *> res;
  get_topological_order_nodes(&res);
  for (auto node : res) {
    if (node->is_function) {
      continue;
    }
    void *loop_body = node->ref;
    void *func_decl = codegen->TraceUpToFuncDecl(loop_body);
    if (func_decl != nullptr) {
      void *func_body = codegen->GetFuncBody(func_decl);
      if ((func_body != nullptr) && !node->is_while) {
        node->order_vector_gen(codegen, func_body, loop_body);
      }
    }
  }
}

void CMarsIr::get_all_kernel_nodes(vector<CMirNode *> *kernels,
                                   set<CMirNode *> *all_nodes,
                                   vector<CMirNode *> *all_nodes_vec,
                                   bool exclude_task) {
  std::queue<CMirNode *> q;
  for (iterator i = ir_list.begin(); i != ir_list.end(); ++i) {
    CMirNode *top_node = i->get_top_node();
    if (top_node->is_function && top_node->is_kernel) {
      if (top_node->is_task && exclude_task) {
        continue;
      }
      q.push(top_node);
      kernels->push_back(top_node);
    }
  }
  while (!q.empty()) {
    CMirNode *top = q.front();
    q.pop();
    if (!all_nodes->insert(top).second) {
      continue;
    }
    all_nodes_vec->push_back(top);
    vector<CMirNode *> succs = top->get_all_child();
    for (size_t i = 0; i < succs.size(); ++i) {
      q.push(succs[i]);
    }
  }
}

#if USED_CODE_IN_COVERAGE_TEST
void CMarsIr::get_all_while_nodes(vector<CMirNode *> *all_nodes_vec) {
  for (iterator i = ir_list.begin(); i != ir_list.end(); ++i) {
    vector<CMirNode *> w_nodes = i->get_while_nodes();
    for (size_t i = 0; i < w_nodes.size(); ++i)
      all_nodes_vec->push_back(w_nodes[i]);
  }
  return;
}
#endif

bool CMarsIr::get_topological_order_nodes(vector<CMirNode *> *res) {
  std::set<CMirNode *> visited;
  std::set<CMirNode *> updated;
  std::vector<CMirNode *> kernels;
  set<CMirNode *> all_nodes_set;
  vector<CMirNode *> all_nodes_vec;
  get_all_kernel_nodes(&kernels, &all_nodes_set, &all_nodes_vec);

  for (size_t i = 0; i != all_nodes_vec.size(); ++i) {
    if (!dfs_traverse(all_nodes_vec[i], &visited, &updated, all_nodes_set,
                      res)) {
      //  detact recursively function call
      return false;
    }
  }
  assert(all_nodes_vec.size() == res->size());
  return true;
}

bool CMarsIr::dfs_traverse(CMirNode *curr, set<CMirNode *> *visited,
                           set<CMirNode *> *updated,
                           const set<CMirNode *> &all_nodes,
                           vector<CMirNode *> *res) {
  if (updated->count(curr) > 0) {
    return true;
  }
  visited->insert(curr);

  //  skip the predecessor check for the root node
  if (!curr->is_kernel) {
    vector<CMirNode *> preds = curr->get_all_predecessor();

    for (size_t i = 0; i < preds.size(); ++i) {
      CMirNode *pred = preds[i];
      if (all_nodes.count(pred) <= 0) {
        continue;
      }
      if (updated->count(pred) > 0) {
        continue;
      }
      //  detact cycle
      if (visited->count(pred) > 0) {
        return false;
      }
      if (!dfs_traverse(pred, visited, updated, all_nodes, res)) {
        //  detact cycle
        return false;
      }
    }
  }
  res->push_back(curr);
  updated->insert(curr);
#if 0
    vector<CMirNode *> succs = curr->get_all_child();
    for (size_t i =0; i < succs.size(); ++i)
        if (!dfs_traverse(succs[i], visited, updated, all_nodes, res))
            //  detact cycle
            return false;
#endif
  return true;
}

bool CMarsIr::trace_to_bus_available(CSageCodeGen *codegen, void *kernel_decl,
                                     void *target_arr, void *pos) {
  map<void *, bool> res;
  return trace_to_bus(codegen, kernel_decl, target_arr, &res, pos);
}

bool CMarsIr::trace_to_bus(CSageCodeGen *codegen, void *kernel_decl,
                           void *target_arr, map<void *, bool> *res,
                           void *pos) {
  set<void *> visited;
  return trace_to_bus(codegen, kernel_decl, target_arr, res, pos, &visited);
}

bool CMarsIr::trace_to_bus(CSageCodeGen *codegen, void *kernel_decl,
                           void *target_arr, map<void *, bool> *res, void *pos,
                           set<void *> *visited) {
  //  Return value
  //  true: parse successfully
  //       res: store all the instances <key, val> -> (var, is_off_chip_buffer)
  //  false: parse failed;
  if (target_arr == nullptr) {
    return false;
  }
  auto var_pos = std::make_pair(target_arr, pos);
  if (m_port_is_bus.count(var_pos) > 0) {
    if (m_port_is_bus[var_pos].empty())
      return false;
    res->insert(m_port_is_bus[var_pos].begin(), m_port_is_bus[var_pos].end());
    return true;
  }
  map<void *, bool> sub_res;
  m_port_is_bus[var_pos] = sub_res;  // mark empty
#if PROJDEBUG
#if 0
    cout << "[MARS-IR-ANALYZE] Function tracing process...\n";
    cout << "target array: " << codegen->_p(target_arr) << endl;
#endif
#endif
  if (codegen->IsArgumentInitName(target_arr) == 0) {
    void *type = codegen->GetTypebyVar(target_arr);
    if ((codegen->IsPointerType(type) != 0) && (pos != nullptr)) {
      if (visited->count(target_arr) > 0) {
        return false;
      }
      visited->insert(target_arr);
      //  handle pointer alias
      auto vec_source = codegen->get_alias_source(target_arr, pos);
      if (vec_source.empty()) {
        return false;
      }
      for (auto source : vec_source) {
        void *var_ref = nullptr;
        codegen->parse_array_ref_from_pntr(source, &var_ref);
        if (var_ref == nullptr) {
          return false;
        }
        void *new_target_arr = codegen->GetVariableInitializedName(var_ref);
        map<void *, bool> one_res;
        if (!trace_to_bus(codegen, kernel_decl, new_target_arr, &one_res,
                          source, visited)) {
          return false;
        }
        sub_res.insert(one_res.begin(), one_res.end());
      }
      visited->erase(target_arr);
    } else {
      sub_res[target_arr] = false;
#if 0
            cout << "[MARS-IR-MSG] Array/pointer '" << codegen->_p(target_arr)
                << "' is a local declared array/pointer." << endl;
#endif
    }
    m_port_is_bus[var_pos] = sub_res;
    res->insert(sub_res.begin(), sub_res.end());
    return true;
  }
  int arg_num = -1;
  for (int i = 0; i < codegen->GetFuncParamNum(kernel_decl); i++) {
    void *sg_name = codegen->GetFuncParam(kernel_decl, i);

    if (sg_name == target_arr) {
      arg_num = i;  //  Find where arr stays in the function argument list
      //  cout << "Find arg " << arg_num << endl;
      break;
    }
  }
  if (arg_num == -1) {
    return false;
  }
  if (is_kernel(codegen, kernel_decl)) {
    bool is_bus = is_bus_port(codegen, kernel_decl, arg_num);
    sub_res[target_arr] = is_bus;
    m_port_is_bus[var_pos] = sub_res;
    res->insert(sub_res.begin(), sub_res.end());
    return true;
  }
  vector<void *> funcCallList;
  codegen->GetFuncCallsFromDecl(kernel_decl, nullptr, &funcCallList);

  for (size_t i = 0; i != funcCallList.size(); ++i) {
    void *sg_call = funcCallList[i];
    void *sg_call_decl = codegen->GetEnclosingNode("Function", sg_call);
    void *call_arg = codegen->GetFuncCallParam(sg_call, arg_num);
    void *var_ref = nullptr;
    do {
      codegen->parse_array_ref_from_pntr(call_arg, &var_ref);
      if (var_ref != nullptr)
        break;
      codegen->remove_cast(&call_arg);
      if (codegen->IsAddressOfOp(call_arg)) {
        // handle address of local variables
        call_arg = codegen->GetExpUniOperand(call_arg);
      } else {
        break;
      }
    } while (true);

    if (var_ref == nullptr) {
      return false;
    }
    void *sg_name = codegen->GetVariableInitializedName(var_ref);
    if (sg_name == nullptr) {
      return false;
    }
    map<void *, bool> one_sub_res;
    if (!trace_to_bus(codegen, sg_call_decl, sg_name, &one_sub_res, sg_call,
                      visited)) {
      return false;
    }
    sub_res.insert(one_sub_res.begin(), one_sub_res.end());
  }
  m_port_is_bus[var_pos] = sub_res;
  res->insert(sub_res.begin(), sub_res.end());
  return true;
}

extern void GetTLDMInfo_withPointer4(void *sg_node, void *pArg);
#if USED_CODE_IN_COVERAGE_TEST
bool CMarsIr::HasLink(CSageCodeGen *codegen, void *sg_func_decl, void *sg_arg,
                      bool report) {
  //  printf("Enter HasLink!\n");
  vector<pair<void *, string>> vecTldmPragmas;
  codegen->TraverseSimple(sg_func_decl, "preorder", GetTLDMInfo_withPointer4,
                          &vecTldmPragmas);
  size_t i;
  for (i = 0; i < vecTldmPragmas.size(); i++) {
    string sFilter, sCmd;
    map<string, pair<vector<string>, vector<string>>> mapParams;
    tldm_pragma_parse_whole(vecTldmPragmas[i].second, sFilter, sCmd, mapParams);
    if (("ACCEL" != sFilter) || ("interface" != sCmd))
      continue;
    string string_arg = codegen->UnparseToString(sg_arg);
    //  printf("sg_arg = %s\n", string_arg.c_str());

    string interface_type;
    string variable_name;
    string link_depth;
    if (mapParams["type"].first.size() > 0) {
      interface_type = mapParams["type"].first[0];
    } else {
      interface_type = "globalmemory";
    }
    if (mapParams["variable"].first.size() > 0) {
      variable_name = mapParams["variable"].first[0];
    } else {
      return 0;
    }
    if (mapParams["depth"].first.size() > 0) {
      link_depth = mapParams["depth"].first[0];
      //  int tt = 0;
      //  for (int i = 0; i < codegen->GetFuncParamNum(sg_func_decl); i++) {
      //    string port_name =
      //    codegen->UnparseToString(codegen->GetFuncParam(sg_func_decl, i));
      //    bool is_number; if(std::all_of(link_depth.begin(),link_depth.end(),
      //    ::isdigit)) {
      //        is_number = 1;
      //    } else {
      //        is_number = 0;
      //    }
      //    printf("\n\nis_number = %d\n", is_number);
      //    if(port_name == link_depth && !is_number) {
      //        tt = 1;
      //    }
      //  }
      //  if(tt == 0) {
      //    printf("\n\nillegal depth\n");
      //  }
    } else {
      return 0;
    }
    //  printf("type = %s\n", interface_type.c_str());
    //  printf("variable = %s\n", variable_name.c_str());
    //  printf("depth = %s\n", link_depth.c_str());
    if (variable_name == string_arg && interface_type == "globalmemory") {
#if PROJDEBUG
      printf("Find a link for %s!\n", string_arg.c_str());
#endif
      return 1;
    }
  }
  return 0;
}
#endif

void CMarsIr::checkKernelFunctionInterface(CSageCodeGen *codegen,
                                           void *sg_func_decl, void *sg_call) {
  if (sg_func_decl != nullptr) {
    for (int i = 0; i < codegen->GetFuncParamNum(sg_func_decl); i++) {
      void *arg = codegen->GetFuncParam(sg_func_decl, i);
      string port_name = codegen->GetVariableName(arg);
      if (port_name.empty()) {
        string port_info = my_itoa(i) + "th interface port of function \'" +
                           codegen->GetFuncName(sg_func_decl) + "\' " +
                           getUserCodeFileLocation(codegen, sg_func_decl, true);
        dump_critical_message(PROCS_ERROR_54(port_info));
        mErrorOut = true;
      }
    }
    //  youxiang 20161019
    void *return_type = codegen->GetFuncReturnType(sg_func_decl);
    if ((codegen->IsPointerType(return_type) != 0) ||
        (codegen->IsReferenceType(return_type) != 0)) {
      string msg =
          "Cannot support returning a pointer or reference in function ";
      string func_info = "\'" + codegen->GetFuncName(sg_func_decl) + "\' " +
                         getUserCodeFileLocation(codegen, sg_func_decl, true);
      msg += func_info;
      cout << msg << endl;
      dump_critical_message(PROCS_ERROR_27(func_info));
      mErrorOut = true;
    }
  }

  if ((sg_func_decl == nullptr) || (sg_call == nullptr)) {
    return;
  }
  if (codegen->GetFuncParamNum(sg_func_decl) !=
      codegen->GetFuncCallParamNum(sg_call)) {
    string call_location = codegen->get_location(sg_call, true, true);
    string func_location = codegen->get_location(sg_func_decl, true, true);
    string func_name = codegen->GetFuncName(sg_func_decl);
    string func_info = "\'" + func_name + "\' (" + func_location + ")";
    string call_info = "(" + call_location + ")";
    string msg = "Finding the kernel " + func_info +
                 " function with unmatched task instance " + call_info;
    dump_critical_message(PROCS_ERROR_21(func_info, call_info));
    return;
  }
  for (int i = 0; i < codegen->GetFuncParamNum(sg_func_decl); i++) {
    void *arg = codegen->GetFuncParam(sg_func_decl, i);
    void *actual_arg = codegen->GetFuncCallParam(sg_call, i);
    string port_name = codegen->GetVariableName(arg);
    string port_info =
        "'" + port_name + "' " + getUserCodeFileLocation(codegen, arg, true);
    void *arg_type = codegen->GetTypebyVar(arg);
    if (codegen->IsReferenceType(arg_type) != 0) {
      arg_type = codegen->GetElementType(arg_type);
    }
    void *actual_arg_type = codegen->GetTypeByExp(actual_arg);
    //  check whether 'arg_type' is compatible with 'actual_arg_type'
    if (codegen->IsCompatibleType(arg_type, actual_arg_type, arg, false) == 0) {
      string msg = "Cannot support implict type cast for argument ";
      string argument_info = "\'" + codegen->_up(actual_arg) + "\' " +
                             getUserCodeFileLocation(codegen, actual_arg, true);
      string argument_type_info =
          "\'" + codegen->UnparseToString(actual_arg_type) + "\'";
      string parameter_type_info =
          "\'" + codegen->UnparseToString(arg_type) + "\'";
      dump_critical_message(PROCS_ERROR_32(argument_info, argument_type_info,
                                           parameter_type_info));
      mErrorOut = true;

      continue;
    }
  }
}

bool CMarsIr::check_valid_reduction(CSageCodeGen *codegen, void *func_decl) {
  int param_num = codegen->GetFuncParamNum(func_decl);
  if ((param_num % 2) != 0) {
    cout << "is not valid reduction function\n";
    return false;
  }
  for (int i = 0; i < param_num; i++) {
    void *arg = codegen->GetFuncParam(func_decl, i);
    SgInitializedName *sg_arg = isSgInitializedName(static_cast<SgNode *>(arg));
    SgType *arg_type = sg_arg->get_type();

    if (i >= 0 && i < param_num / 2) {
      if (isSgReferenceType(arg_type) == nullptr) {
        cout << "is not valid reduction function\n";
        return false;
      }
    } else {
      if ((isSgReferenceType(arg_type) != nullptr) ||
          (isSgArrayType(arg_type) != nullptr) ||
          (isSgPointerType(arg_type) != nullptr)) {
        cout << "is not valid reduction function\n";
        return false;
      }
    }
  }

  return true;
}

bool CMarsIr::is_fine_grained_scope(CSageCodeGen *codegen, void *insert_pos) {
  CMirNode *insert_node = nullptr;
  void *insert_parent_loop = nullptr;
  if (codegen->IsForStatement(insert_pos) != 0) {
    insert_parent_loop =
        codegen->TraceUpToForStatement(codegen->GetParent(insert_pos));
  } else {
    insert_parent_loop = codegen->TraceUpToForStatement(insert_pos);
  }
  if (insert_parent_loop != nullptr) {
    void *loop_body = codegen->GetLoopBody(insert_parent_loop);
    insert_node = get_node(loop_body);
  } else {
    void *func_decl = codegen->TraceUpToFuncDecl(insert_pos);
    void *func_body = codegen->GetFuncBody(func_decl);
    insert_node = get_node(func_body);
  }
  return (insert_node != nullptr) && insert_node->is_fine_grain;
}
