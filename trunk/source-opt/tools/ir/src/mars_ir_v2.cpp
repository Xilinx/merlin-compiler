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
#include <set>
#include <string>
#include <tuple>

#include "analPragmas.h"
#include "codegen.h"
#include "mars_ir_v2.h"
#include "mars_message.h"
#include "program_analysis.h"
#include "rose.h"

#include "PolyModel.h"
#include "cmdline_parser.h"
#include "file_parser.h"
#include "mars_opt.h"
#include "tldm_annotate.h"
#include "xml_parser.h"
#include <boost/algorithm/string/replace.hpp>

using MarsProgramAnalysis::CMarsAccess;
using MarsProgramAnalysis::CMarsExpression;
using MarsProgramAnalysis::CMarsRangeExpr;
using MarsProgramAnalysis::get_variable_range;
using std::list;
using std::queue;
using std::tuple;
using std::unordered_set;
#define USED_CODE_IN_COVERAGE_TEST 0
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

bool CMarsIrV2::is_known_type(void *type) {
  return (m_ast->IsXilinxAPIntType(type) != 0) ||
         (m_ast->IsXilinxAPFixedType(type) != 0);
}

bool CMarsIrV2::is_kernel(void *func_decl) {
  for (set<void *>::iterator i = mKernelTops.begin(), e = mKernelTops.end();
       i != e; ++i) {
    if (m_ast->isSameFunction(*i, func_decl)) {
      return true;
    }
  }
  return false;
}

bool CMarsIrV2::is_used_by_kernel(void *sg_node) {
  if (m_used_by_kernel_cached.count(sg_node) > 0) {
    return m_used_by_kernel_cached[sg_node];
  }
  bool res = is_used_by_kernel_sub(sg_node);
  m_used_by_kernel_cached[sg_node] = res;
  return res;
}

bool CMarsIrV2::is_used_by_kernel_sub(void *sg_node) {
  if ("rose_edg_required_macros_and_functions.h" == m_ast->get_file(sg_node))
    return false;
  if ((m_ast->IsGlobal(sg_node) != 0) || (m_ast->IsFile(sg_node) != 0) ||
      (m_ast->IsProject(sg_node) != 0) ||
      (m_ast->IsUsingDirectiveStmt(sg_node) != 0) ||
      //  FIXME: cannot support namespace stmt
      (m_ast->IsNameSpaceDeclStmt(sg_node) != 0) ||
      (m_ast->IsNameSpaceDefStmt(sg_node) != 0)) {
    return false;
  }
  if (!m_valid_kernel_node_analysis) {
    collect_all_kernel_nodes();
  }
  void *var_init = nullptr;
  if ((m_ast->IsVarRefExp(sg_node) != 0) ||
      (m_ast->IsVariableDecl(sg_node) != 0)) {
    var_init = m_ast->GetVariableInitializedName(sg_node);
  } else if (m_ast->IsInitName(sg_node) != 0) {
    var_init = sg_node;
  } else {
    var_init = m_ast->TraceUpToInitializedName(sg_node);
  }
  if (var_init != nullptr) {
    if (m_var_used_by_kernel.count(var_init) > 0) {
      return true;
    }
    auto var_symbol = isSgInitializedName(static_cast<SgNode *>(var_init))
                          ->search_for_symbol_from_symbol_table();
    if (var_symbol != nullptr)
      return m_var_used_by_kernel.count(var_symbol) > 0;
    return false;
  }

  void *type = nullptr;
  if (m_ast->IsTypeDecl(sg_node) != 0) {
    type = sg_node;
  } else if (m_ast->IsClassDefinition(sg_node) != 0) {
    type = m_ast->GetTypeDeclByDefinition(sg_node);
  } else if (m_ast->IsMemberFunction(sg_node)) {
    type = m_ast->GetTypeDeclByMemberFunction(sg_node);
  }
  if (m_ast->IsTemplateInstClassDecl(type) != 0) {
    type = m_ast->GetTemplateClassDecl(type);
  }
  if (type != nullptr) {
    if (auto cls_decl = isSgClassDeclaration(static_cast<SgNode *>(type))) {
      auto *cls_symbol = cls_decl->search_for_symbol_from_symbol_table();
      if (cls_symbol != nullptr)
        return m_type_used_by_kernel.count(cls_symbol) > 0;
    }
    return m_type_used_by_kernel.count(type) > 0;
  }

  void *func = nullptr;
  if (m_ast->IsFunctionDeclaration(sg_node) != 0) {
    func = sg_node;
  } else if (m_ast->IsFunctionRefExp(sg_node) != 0) {
    func = m_ast->GetFuncDeclByRef(sg_node);
  } else if (m_ast->IsTemplateInstantiationDirectiveStatement(sg_node) != 0) {
    func = m_ast->GetFuncDeclByInstDirectiveStmt(sg_node);
  } else {
    func = m_ast->TraceUpToFuncDecl(sg_node);
  }
  if (m_ast->IsTemplateInstantiationFunctionDecl(func) != 0) {
    func = m_ast->GetTemplateFuncDecl(func);
  }
  if (func != nullptr) {
    auto *func_symbol = static_cast<SgFunctionDeclaration *>(func)
                            ->search_for_symbol_from_symbol_table();
    if (func_symbol != nullptr)
      return m_func_used_by_kernel.count(func_symbol) > 0;
    return m_func_used_by_kernel.count(func) > 0;
  }

  if (m_ast->IsPragmaDecl(sg_node) != 0) {
    return false;
  }

  DEBUG(cout << "is_used_by_kernel saw an unknown node: "
             << m_ast->_p(sg_node, 80) << "\n");
  return false;
}

void CMarsIrV2::build_mars_ir(CSageCodeGen *codegen, void *pTopFunc,
                              bool check_valid, bool build_node) {
  m_valid_kernel_node_analysis = false;
  m_ast = codegen;
  mValidCheck = check_valid;
  mValid = true;
  mErrorOut = false;
  mPragmas.clear();
  // Build the AST used by ROSE
  //
  //  1. analyze pragmas to get all the kernel functions
  vector<void *> pragmas;
  m_ast->GetNodesByType_int(pTopFunc, "preorder", V_SgPragmaDeclaration,
                            &pragmas);

  int count_task = 0;
  set<void *> unrolled_loops;
  set<void *> kernels;
  for (size_t i = 0; i != pragmas.size(); ++i) {
    SgNode *pragma = static_cast<SgNode *>(pragmas[i]);
    SgPragmaDeclaration *decl = isSgPragmaDeclaration(pragma);
    assert(decl);
    CAnalPragma ana_pragma(m_ast);
    if (ana_pragma.PragmasFrontendProcessing(decl, &mErrorOut)) {
      //  skip compiler generated statement
      void *next_stmt = m_ast->GetNextStmt(decl, false);
      if (next_stmt == nullptr) {
        continue;
      }
      if (ana_pragma.is_task()) {
        count_task++;
        if (count_task > 1) {
          dump_critical_message(PROCS_ERROR_68());
          throw std::exception();
          mErrorOut = true;
          continue;
        }
        //        //  FIXME: we need to support function outline in the future,
        //        //  i.e. the following may be a basic block which is outlined
        //        //  into a kernel function
        //        if (codegen->IsBasicBlock(next_stmt)) {
        //        } else {
        //          vector<void *> vec_expr;
        //          m_ast->GetNodesByType_int(next_stmt, "preorder",
        //          V_SgFunctionCallExp,
        //                                    &vec_expr);
        //          void *expr = nullptr;
        //          if (vec_expr.size() == 1)
        //            expr = vec_expr[0];
        //          if (vec_expr.size() != 1 || !m_ast->IsFunctionCall(expr)) {
        //            string pragma_info = "\'" +
        //            decl->get_pragma()->get_pragma() +
        //                                 "\' " +
        //                                 getUserCodeFileLocation(m_ast, decl,
        //                                 true);
        //            //  string msg = "Cannot find a kernel function"
        //            //             " specified by the pragma " + pragma_info
        //            //             + ".\n";
        //            //  msg += "  Hint: the task pragma should be placed
        //            //  right before its "
        //            //       "specified kernel function call\n";
        //            bool multiple_func_calls = vec_expr.size() > 1;
        //            if (check_valid)
        //              dump_critical_message(
        //                  PROCS_ERROR_12(pragma_info, multiple_func_calls));
        //            //  throw std::exception();
        //            mErrorOut = true;
        //            continue;
        //          }
        //          void *kernel = m_ast->GetFuncDeclByCall(expr);
        //          if (!kernel) {
        //            string func_name = m_ast->GetFuncNameByCall(expr, true);
        //            string func_info = "\'" + func_name + "\'";
        //            string pragma_info = "\'" +
        //            decl->get_pragma()->get_pragma() +
        //                                 "\' " +
        //                                 getUserCodeFileLocation(m_ast, expr,
        //                                 true);
        //            //  string msg = "Cannot find the kernel function "+
        //            //  func_info +
        //            //             " specified by the pragma " +
        //            //             pragma_info;
        //
        //            if (check_valid)
        //              dump_critical_message(PROCS_ERROR_13(func_info,
        //              pragma_info));
        //            //  throw std::exception();
        //            mErrorOut = true;
        //          } else {
        //            kernels.insert(kernel);
        //            mKernel2Pragma[kernel] = decl;
        //          }
        //          continue;
        //        }
      }
      if (ana_pragma.is_kernel()) {
        void *kernel = next_stmt;
        if (m_ast->IsFunctionDeclaration(kernel) == 0) {
          vector<void *> vec_expr;
          m_ast->GetNodesByType_int(next_stmt, "preorder", V_SgFunctionCallExp,
                                    &vec_expr);
          void *expr = nullptr;
          if (vec_expr.size() == 1) {
            expr = vec_expr[0];
          }
          if (vec_expr.size() != 1 || (m_ast->IsFunctionCall(expr) == 0)) {
            string pragma_info = "\'" + decl->get_pragma()->get_pragma() +
                                 "\' " +
                                 getUserCodeFileLocation(m_ast, decl, true);
            //  string msg = "Cannot find a kernel function"
            //             " specified by the pragma " + pragma_info
            //             + ".\n";
            //  msg += "  Hint: the kernel pragma should be placed
            //  right before its "
            //       "specified kernel function\n";
            bool multiple_func_calls = vec_expr.size() > 1;
            if (check_valid) {
              dump_critical_message(
                  PROCS_ERROR_14(pragma_info, multiple_func_calls));
            }
            //  throw std::exception();
            mErrorOut = true;

          } else {
            kernel = m_ast->GetFuncDeclByCall(expr);
            if (kernel == nullptr) {
              void *kernel_decl = codegen->GetFuncDeclByCall(expr, 0);
              if (kernel_decl != nullptr) {
                int param_num = codegen->GetFuncParamNum(kernel_decl);
                for (int j = 0; j < param_num; ++j) {
                  void *param = codegen->GetFuncParam(kernel_decl, j);
                  void *sg_base_type = codegen->get_array_base_type(param);
                  void *sg_base_orig_type =
                      codegen->GetOrigTypeByTypedef(sg_base_type, true);
                  if (codegen->IsAnonymousType(sg_base_orig_type) != 0) {
                    if (check_valid) {
                      string type_info = codegen->GetStringByType(sg_base_type);
                      dump_critical_message(PROCS_ERROR_41(type_info));
                      mErrorOut = true;
                    }
                  }
                }
              }
              string func_name = m_ast->GetFuncNameByCall(expr, true);
              string func_info = "\'" + func_name + "\'";
              string pragma_info = "\'" + decl->get_pragma()->get_pragma() +
                                   "\' " +
                                   getUserCodeFileLocation(codegen, expr, true);
              string msg = "Cannot find the kernel function " + func_info +
                           " specified by the pragma " + pragma_info;
              cout << msg << endl;
              if (check_valid) {
                dump_critical_message(PROCS_ERROR_13(func_info, pragma_info));
              }
              //  throw std::exception();
              mErrorOut = true;
            } else {
              kernels.insert(kernel);
            }
          }
        } else {
          kernels.insert(kernel);
        }
        continue;
      }

      if (ana_pragma.is_skip_sync()) {
        string port_name = ana_pragma.get_attribute(CMOST_variable);
        void *port = m_ast->find_variable_by_name(port_name, decl);
        if (port != nullptr) {
          mNoSynchronizedPorts.insert(port);
        }
      }

      if (!ana_pragma.is_loop_related()) {
        continue;
      }

      void *specified_loop = m_ast->TraceUpToForStatement(decl);
      if (specified_loop != nullptr) {
        CMarsLoop *loop_info = get_loop_info(specified_loop);
        if (loop_info->is_complete_unroll() != 0) {
          unrolled_loops.insert(specified_loop);
        }
      }
      mPragmas.insert(decl);
    }
  }

  vector<void *> func_defs;
  m_ast->GetNodesByType_int(pTopFunc, "preorder", V_SgFunctionDefinition,
                            &func_defs);
  mAllKernelFuncs.clear();
  mKernelTops.clear();
  set<void *> all_func_decls;
  queue<void *> worklist;
  //  2. match kernel functions
  for (set<void *>::iterator i = kernels.begin(); i != kernels.end(); ++i) {
    void *kernel = *i;
    string func_name = m_ast->GetFuncName(kernel, true);
    bool found = false;
    for (size_t j = 0; j != func_defs.size(); ++j) {
      void *func_decl = m_ast->GetFuncDeclByDefinition(func_defs[j]);
      if (m_ast->isSameFunction(kernel, func_decl)) {
        if (found) {
          string func_info = "\'" + func_name + "\'";
          string msg =
              "Finding multiple definitions for the function " + func_info;
          if (check_valid) {
            dump_critical_message(PROCS_ERROR_20(func_info));
          }
          //  throw std::exception();
          mErrorOut = true;
        } else {
          worklist.push(func_decl);
          mKernelTops.insert(func_decl);
          found = true;
        }
        mAllKernelFuncs.insert(func_decl);
        SetVector<void *> sub_calls;
        SetVector<void *> sub_funcs;
        codegen->GetSubFuncDeclsRecursively(func_decl, &sub_funcs, &sub_calls);
        mAllKernelFuncs.insert(sub_funcs.begin(), sub_funcs.end());
      }
    }
    if (!found) {
      string func_info = "\'" + func_name + "\'";
      string msg = "Cannot find the kernel function " + func_info;
      if (check_valid) {
        dump_critical_message(PROCS_ERROR_22(func_info));
      }
      //  throw std::exception();
      mErrorOut = true;
    }
  }

  //  3. collect all the sub functions belonging to kernel part
  mFunc2Caller.clear();
  mFunc2Callee.clear();
  mCall2Func.clear();
  for (auto func : mAllKernelFuncs) {
    vector<void *> calls;
    m_ast->GetFuncCallsFromDecl(func, nullptr, &calls);
    for (auto call : calls) {
      void *caller = m_ast->GetEnclosingNode("Function", call);
      if (caller != nullptr) {
        mFunc2Caller[func].insert(caller);
        mFunc2Callee[caller].insert(func);
      }
      mCall2Func[call] = func;
    }
  }

  //  4. get the topological order for all kernel functions
  vector<void *> topo_funcs;
  get_topological_order_funcs(&topo_funcs);

  //  5. collect loop info for each function
  mFunc2Loop.clear();
  mFlattenLoop.clear();
  for (int i = static_cast<int>(topo_funcs.size() - 1); i >= 0; --i) {
    void *func = topo_funcs[i];
    mFunc2Loop[func] = false;
    void *func_body = m_ast->GetFuncBody(func);
    if (func_body == nullptr) {
      continue;
    }
    bool has_loops = false;
    vector<void *> loops;
    m_ast->GetNodesByType_int(func_body, "postorder", V_SgForStatement, &loops);
    for (auto loop : loops) {
      if (!is_flatten_loop(loop, unrolled_loops)) {
        mFlattenLoop[loop] = false;
        has_loops = true;
      } else {
        mFlattenLoop[loop] = true;
      }
    }
    //  has_loops |= loops.size() > 0;
    vector<void *> memcpy_calls;
    m_ast->GetFuncCallsByName("memcpy", func_body, &memcpy_calls);
    has_loops |= !memcpy_calls.empty();
    set<void *> &sub_funcs = mFunc2Callee[func];
    for (set<void *>::iterator j = sub_funcs.begin(); j != sub_funcs.end();
         ++j) {
      void *sub_func = *j;
      if (mFunc2Loop[sub_func]) {
        has_loops = true;
      }
    }
    mFunc2Loop[func] = has_loops;
  }

  //  6. init array dimension information, burst, coalescing,
  //  false dep info and etc from pragmas
  init_variable_info_pragma();

  //  7. build task grained pipeline model
  if (build_node) {
    build_node_info(unrolled_loops, topo_funcs);
  }
}

void CMarsIrV2::build_node_info(const set<void *> &unrolled_loops,
                                const vector<void *> &topo_funcs) {
  assert(m_ast);
  //  1. build each node from top to bottom
  //  merge all the continuous statements without sub loops
  for (set<void *>::iterator i = mKernelTops.begin(); i != mKernelTops.end();
       ++i) {
    void *kernel = *i;
    void *func_body = m_ast->GetFuncBody(kernel);
    if (func_body == nullptr) {
#if PROJDEBUG
      cout << "no function body for kernel " << m_ast->GetFuncName(kernel)
           << endl;
#endif
      assert(0);
      continue;
    }
    vector<void *> loops;
    map<void *, vector<pair<void *, bool>>> loop2cond;
    vector<int> orders;
    t_func_call_path call_path;
    vector<pair<void *, bool>> conds;
    orders.push_back(0);
    int start = size();
    buildNode(func_body, &loops, &loop2cond, &conds, &orders, &call_path,
              kernel, unrolled_loops);
    int end = size();
    mKernel2IrNodeRange[kernel] = std::make_pair(start, end);
  }

  //  2. remove all the internal ports
  update_ports_for_all_nodes();

  //  3. calculate_dependence_for_all_nodes
  update_all_edges();
  //  update_dependence_for_all_edges();

  //  4. set SSST flag for each edge
  update_ssst_flag_for_all_edges();

}

bool CMarsIrV2::every_trace_is_bus(void *target_arr, void *pos) {
  //  Return value
  //  true: connected to the bus;
  //  false: on chip buffer;
  map<void *, bool> res;
  bool ret = trace_to_bus(target_arr, &res, pos);
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

bool CMarsIrV2::is_kernel_port(void *sg_init_name) {
  return any_trace_is_bus(sg_init_name, nullptr);
}

bool CMarsIrV2::any_trace_is_bus(void *target_arr, void *pos) {
  //  Return value
  //  true: connected to the bus;
  //  false: on chip buffer;
  map<void *, bool> res;
  set<void *> visited;
  bool ret = trace_to_bus(target_arr, &res, pos, &visited);
  if (ret) {
    //  parse successfully
    for (map<void *, bool>::iterator i = res.begin(); i != res.end(); ++i) {
      //  found a on chip buffer
      if (i->second) {
        return true;
      }
    }
    return false;
  }
  //  To be conservative, we do not know whether it is connected to bus port
  return true;
}

CMarsIrV2::~CMarsIrV2() {
  mKernelTops.clear();
  mAllKernelFuncs.clear();
  mDimensionPragma.clear();
  mDepthPragma.clear();
  mContinueFlag.clear();
  mBurstFlag.clear();
  mWriteOnlyFlag.clear();
  mFalseDepFlag.clear();
  mBurstMaxSize.clear();
  for (auto &edge : mIrEdgesList) {
    delete edge;
  }
  mIrEdgesList.clear();
  for (auto &node : mIrNodes) {
    delete node;
  }
  mIrNodes.clear();
  mCall2Func.clear();
  mFunc2Caller.clear();
  mFunc2Callee.clear();
  mFunc2Loop.clear();
  mKernel2IrNodeRange.clear();
  mLoop2Orders.clear();
  mOrder2Loop.clear();
  mIrEdges.clear();
  for (auto &loop_info : mLoopInfo) {
    delete loop_info.second;
  }
  mLoopInfo.clear();
}

void CMarsIrV2::clear() {
  mDimensionPragma.clear();
  mDepthPragma.clear();
  mContinueFlag.clear();
  mBurstFlag.clear();
  mWriteOnlyFlag.clear();
  mFalseDepFlag.clear();
  mBurstMaxSize.clear();
  mKernelTops.clear();
  mAllKernelFuncs.clear();
  for (auto &node : mIrNodes) {
    delete node;
  }
  mIrNodes.clear();
  mCall2Func.clear();
  mFunc2Caller.clear();
  mFunc2Callee.clear();
  mFunc2Loop.clear();
  mKernel2IrNodeRange.clear();
  mLoop2Orders.clear();
  mOrder2Loop.clear();
  mIrEdges.clear();
  for (auto &edge : mIrEdgesList) {
    delete edge;
  }
  mIrEdgesList.clear();
  for (auto &loop_info : mLoopInfo) {
    delete loop_info.second;
  }
  mLoopInfo.clear();
}

CMarsNode *CMarsIrV2::get_node(size_t i) {
  if (i >= size()) {
    return nullptr;
  }
  return mIrNodes[i];
}

bool CMarsIrV2::get_topological_order_funcs(vector<void *> *res) {
  std::set<void *> visited;
  std::set<void *> updated;
  for (set<void *>::const_iterator i = mAllKernelFuncs.begin();
       i != mAllKernelFuncs.end(); ++i) {
    if (!dfs_traverse(*i, &visited, &updated, res)) {
      //  detact recursively function call
      return false;
    }
  }
  assert(mAllKernelFuncs.size() == res->size());
  return true;
}

bool CMarsIrV2::dfs_traverse(void *curr, set<void *> *visited,
                             set<void *> *updated, vector<void *> *res) {
  if (updated->count(curr) > 0) {
    return true;
  }
  visited->insert(curr);

  //  skip the predecessor check for the root node
  if (mKernelTops.count(curr) <= 0) {
    set<void *> &preds = mFunc2Caller[curr];

    for (set<void *>::iterator i = preds.begin(); i != preds.end(); ++i) {
      void *pred = *i;
      if (mAllKernelFuncs.count(pred) <= 0) {
        continue;
      }
      if (updated->count(pred) > 0) {
        continue;
      }
      //  detact cycle
      if (visited->count(pred) > 0) {
        return false;
      }
      if (!dfs_traverse(pred, visited, updated, res)) {
        //  detact cycle
        return false;
      }
    }
  }
  res->push_back(curr);
  updated->insert(curr);
  return true;
}

bool CMarsIrV2::trace_to_bus_available(void *target_arr, void *pos) {
  map<void *, bool> res;
  return trace_to_bus(target_arr, &res, pos);
}

bool CMarsIrV2::trace_to_bus(void *target_arr, map<void *, bool> *res,
                             void *pos) {
  set<void *> visited;
  return trace_to_bus(target_arr, res, pos, &visited);
}

bool CMarsIrV2::trace_to_bus(void *target_arr, map<void *, bool> *res,
                             void *pos, set<void *> *visited) {
  assert(m_ast);
  //  Return value
  //  true: parse successfully
  //       res: store all the instances <key, val> -> (var,
  //       is_off_chip_buffer)
  //  false: parse failed;

  if (target_arr == nullptr) {
    return false;
  }
  auto var_pos = std::make_pair(target_arr, pos);
  if (m_port_is_bus.count(var_pos) > 0) {
    auto &sub_res = m_port_is_bus[var_pos];
    if (sub_res.empty())
      return false;
    res->insert(sub_res.begin(), sub_res.end());
    return true;
  }
  map<void *, bool> sub_res;
  m_port_is_bus[var_pos] = sub_res;  // mark invalid result
  void *kernel_decl =
      m_ast->TraceUpByTypeCompatible(target_arr, V_SgFunctionDeclaration);
  DEBUG(cout << "[MARS-IR-ANALYZE] Function tracing process...\n";
        cout << "target array: " << m_ast->_p(target_arr) << endl;
        cout << "kernel: " << m_ast->_p(kernel_decl) << endl);

  if (m_ast->IsArgumentInitName(target_arr) == 0) {
    void *type = m_ast->GetTypebyVar(target_arr);
    if ((m_ast->IsPointerType(type) != 0) && (pos != nullptr)) {
      if (visited->count(target_arr) > 0) {
        return false;
      }
      visited->insert(target_arr);
      //  handle pointer alias
      auto vec_source = m_ast->get_alias_source(target_arr, pos);
      if (vec_source.empty()) {
        return false;
      }
      for (auto source : vec_source) {
        void *var_ref = nullptr;
        m_ast->parse_array_ref_from_pntr(source, &var_ref);
        if (var_ref == nullptr) {
          return false;
        }
        void *new_target_arr = m_ast->GetVariableInitializedName(var_ref);
        map<void *, bool> one_sub_res;
        if (!trace_to_bus(new_target_arr, &one_sub_res, source, visited)) {
          return false;
        }
        sub_res.insert(one_sub_res.begin(), one_sub_res.end());
      }
      visited->erase(target_arr);
    } else {
      sub_res[target_arr] = false;
      m_port_is_bus[var_pos] = sub_res;
      res->insert(sub_res.begin(), sub_res.end());
      DEBUG(cout << "[MARS-IR-MSG] Array/pointer '" << m_ast->_p(target_arr)
                 << "' is a local declared array/pointer." << endl);
      return true;
    }
  }

  if (kernel_decl == nullptr) {
    return false;
  }
  int arg_num = -1;
  for (int i = 0; i < m_ast->GetFuncParamNum(kernel_decl); i++) {
    void *sg_name = m_ast->GetFuncParam(kernel_decl, i);

    if (sg_name == target_arr) {
      arg_num = i;  //  Find where arr stays in the function argument list
      break;
    }
  }
  if (arg_num == -1) {
    return false;
  }
  if (is_kernel(kernel_decl)) {
    sub_res[target_arr] = true;
    m_port_is_bus[var_pos] = sub_res;
    res->insert(sub_res.begin(), sub_res.end());
    return true;
  }
  vector<void *> funcCallList;
  m_ast->GetFuncCallsFromDecl(kernel_decl, nullptr, &funcCallList);

  for (size_t i = 0; i != funcCallList.size(); ++i) {
    void *sg_call = funcCallList[i];
    void *call_arg = m_ast->GetFuncCallParam(sg_call, arg_num);
    void *var_ref = nullptr;
    m_ast->parse_array_ref_from_pntr(call_arg, &var_ref);
    if (var_ref == nullptr) {
      return false;
    }
    void *sg_name = m_ast->GetVariableInitializedName(var_ref);
    if (sg_name == nullptr) {
      return false;
    }
    map<void *, bool> one_res;
    if (!trace_to_bus(sg_name, &one_res, sg_call, visited)) {
      return false;
    }
    sub_res.insert(one_res.begin(), one_res.end());
  }
  m_port_is_bus[var_pos] = sub_res;
  res->insert(sub_res.begin(), sub_res.end());
  return true;
}

static bool CheckLoopDistributed(CSageCodeGen *codegen, void *loop) {
  void *loop_body = codegen->GetLoopBody(loop);
  vector<void *> vec_break;
  codegen->GetNodesByType_int(loop_body, "preorder", V_SgBreakStmt, &vec_break);
  for (auto break_stmt : vec_break) {
    if (codegen->IsScopeBreak(break_stmt, loop)) {
      return false;
    }
  }
  vector<void *> vec_continue;
  codegen->GetNodesByType_int(loop_body, "preorder", V_SgContinueStmt,
                              &vec_continue);
  for (auto contn : vec_continue) {
    void *loop_scope = codegen->TraceUpToLoopScope(contn);
    if (loop_scope == loop) {
      return false;
    }
  }
  return true;
}

void CMarsIrV2::buildNode(void *body, vector<void *> *loops,
                          map<void *, vector<pair<void *, bool>>> *loop2cond,
                          vector<pair<void *, bool>> *conds,
                          vector<int> *orders, t_func_call_path *call_path,
                          void *kernel, const set<void *> &unrolled_loops) {
  assert(m_ast);
  if (m_ast->IsBasicBlock(body) == 0) {
    return;
  }
  int num = m_ast->GetChildStmtNum(body);
  vector<void *> stmts;

  for (int i = 0; i != num; ++i) {
    void *stmt = m_ast->GetChildStmt(body, i);
    if ((m_ast->IsForStatement(stmt) != 0) &&
        !is_flatten_loop(stmt, unrolled_loops)) {
      createNode(&stmts, *loops, *loop2cond, *conds, orders, *call_path,
                 kernel);
      loops->push_back(stmt);
      mLoop2Orders[stmt].push_back(*orders);
      mOrder2Loop[kernel][*orders] = stmt;
      orders->push_back(0);
      (*loop2cond)[stmt] = *conds;
      vector<pair<void *, bool>> new_conds;
      void *loop_body = m_ast->GetLoopBody(stmt);
      if (CheckLoopDistributed(m_ast, stmt)) {
        buildNode(loop_body, loops, loop2cond, &new_conds, orders, call_path,
                  kernel, unrolled_loops);
      } else {
        stmts.push_back(loop_body);
        createNode(&stmts, *loops, *loop2cond, *conds, orders, *call_path,
                   kernel);
      }
      loops->pop_back();
      (*orders)[loops->size()]++;
      orders->pop_back();
      loop2cond->erase(stmt);

      continue;
    }
    if (m_ast->IsIfStatement(stmt) != 0) {
      if (checkSubLoops(stmt, unrolled_loops)) {
        createNode(&stmts, *loops, *loop2cond, *conds, orders, *call_path,
                   kernel);
        void *true_body = m_ast->GetIfStmtTrueBody(stmt);
        if (true_body != nullptr) {
          conds->push_back(pair<void *, bool>(stmt, true));
          buildNode(true_body, loops, loop2cond, conds, orders, call_path,
                    kernel, unrolled_loops);
          conds->pop_back();
        }
        void *false_body = m_ast->GetIfStmtFalseBody(stmt);
        if (false_body != nullptr) {
          conds->push_back(pair<void *, bool>(stmt, false));
          buildNode(false_body, loops, loop2cond, conds, orders, call_path,
                    kernel, unrolled_loops);
          conds->pop_back();
        }
        continue;
      }
    } else if (m_ast->IsExprStatement(stmt) != 0) {
      SgExprStatement *sg_stmt = isSgExprStatement(static_cast<SgNode *>(stmt));
      void *exp = sg_stmt->get_expression();
      if (m_ast->IsFunctionCall(exp) != 0) {
        if (mCall2Func.count(exp) > 0) {
          void *sub_func = mCall2Func[exp];
          if (mFunc2Loop[sub_func]) {
            createNode(&stmts, *loops, *loop2cond, *conds, orders, *call_path,
                       kernel);
            call_path->emplace_front(sub_func, exp);
            buildNode(m_ast->GetFuncBody(sub_func), loops, loop2cond, conds,
                      orders, call_path, kernel, unrolled_loops);
            call_path->pop_front();
            continue;
          }
        } else if (m_ast->IsMemCpy(exp)) {
          createNode(&stmts, *loops, *loop2cond, *conds, orders, *call_path,
                     kernel);
          stmts.push_back(stmt);
          createNode(&stmts, *loops, *loop2cond, *conds, orders, *call_path,
                     kernel);
          continue;
        }
      }
    } else if (m_ast->IsBasicBlock(stmt) != 0) {
      if (checkSubLoops(stmt, unrolled_loops)) {
        createNode(&stmts, *loops, *loop2cond, *conds, orders, *call_path,
                   kernel);
        buildNode(stmt, loops, loop2cond, conds, orders, call_path, kernel,
                  unrolled_loops);
        continue;
      }
    }
    //  skip variable declaration
    //  skip label statement
    if (m_ast->IsLabelStatement(stmt) != 0) {
      stmt = m_ast->GetStmtFromLabel(stmt);
    }
#if 0
    if (m_ast->IsVariableDecl(stmt)) {
      vector<void*> vec_refs;
      m_ast->GetNodesByType_int(stmt, "preorder", V_SgVarRefExp, &vec_refs);
      if (vec_refs.size() <= 0  &&
          !iswithininnermostloop(stmt)) continue;
    }
#endif
    //  skip access range intrinsic
    if (m_ast->IsExprStatement(stmt) != 0) {
      void *func_call = m_ast->GetExprFromStmt(stmt);
      if (m_ast->IsFunctionCall(func_call) != 0) {
        if (m_ast->IsMerlinInternalIntrinsic(
                m_ast->GetFuncNameByCall(func_call))) {
          continue;
        }
      }
    }
    stmts.push_back(stmt);
  }
  createNode(&stmts, *loops, *loop2cond, *conds, orders, *call_path, kernel);
}

#if USED_CODE_IN_COVERAGE_TEST
bool CMarsIrV2::is_undistributed_statement(void *stmt) {
  if (m_ast->IsBasicBlock(stmt) || m_ast->IsIfStatement(stmt) ||
      m_ast->IsForStatement(stmt))
    return false;
  return true;
}
#endif

bool CMarsIrV2::checkSubLoops(void *sg_scope,
                              const set<void *> &unrolled_loops) {
  assert(m_ast);
  bool has_sub_loops = false;
  vector<void *> sub_loops;
  m_ast->GetNodesByType_int(sg_scope, "preorder", V_SgForStatement, &sub_loops);
  for (auto &loop : sub_loops) {
    if (!is_flatten_loop(loop, unrolled_loops)) {
      has_sub_loops = true;
    }
  }
  //  has_sub_loops |= sub_loops.size() > 0;
  if (has_sub_loops) {
    return true;
  }
  vector<void *> sub_calls;
  m_ast->GetNodesByType_int(sg_scope, "preorder", V_SgFunctionCallExp,
                            &sub_calls);
  for (size_t j = 0; j != sub_calls.size(); ++j) {
    void *sub_call = sub_calls[j];
    if (m_ast->IsMemCpy(sub_call)) {
      has_sub_loops = true;
    } else if (mCall2Func.count(sub_call) > 0) {
      has_sub_loops |= mFunc2Loop[mCall2Func[sub_call]];
    }
    if (has_sub_loops) {
      return true;
    }
  }
  return false;
}

void CMarsIrV2::createNode(
    vector<void *> *stmts, const vector<void *> &loops,
    const map<void *, vector<pair<void *, bool>>> &loop2cond,
    const vector<pair<void *, bool>> &conds, vector<int> *orders,
    const t_func_call_path &call_path, void *kernel) {
  assert(m_ast);
  if (stmts->empty()) {
    return;
  }
  //  filter some nodes which only includes pragmas, variables without
  //  initialization
  if (!isEmptyStmtSequence(*stmts)) {
    CMarsNode *node = new CMarsNode(m_ast, *stmts, loops, loop2cond, conds,
                                    *orders, call_path, kernel);
    mIrNodes.push_back(node);
  }
  stmts->clear();
  (*orders)[loops.size()]++;
}

void CMarsIrV2::update_ports_for_all_nodes() {
  int num_nodes = size();
  mSharedPorts.clear();
  for (int i = 0; i != num_nodes; ++i) {
    CMarsNode *curr_node = get_node(i);
    curr_node->update_reference();
  }
  for (int i = 0; i != num_nodes; ++i) {
    set<void *> all_shared_ports;
    CMarsNode *curr_node = get_node(i);
    for (int j = 0; j != num_nodes; ++j) {
      if (i == j) {
        continue;
      }
      set<void *> shared_ports = curr_node->get_common_ports(get_node(j));
      all_shared_ports.insert(shared_ports.begin(), shared_ports.end());
    }
    set<void *> new_ports = all_shared_ports;
    auto orig_ports = curr_node->get_ports();
    mSharedPorts.insert(new_ports.begin(), new_ports.end());
    if (new_ports.size() == orig_ports.size()) {
      continue;
    }
    for (auto j = orig_ports.begin(); j != orig_ports.end(); ++j) {
      void *port = *j;
      if (new_ports.count(port) > 0) {
        continue;
      }
      if (every_trace_is_bus(port, nullptr)) {
        new_ports.insert(port);
      }
    }
    if (new_ports.size() == orig_ports.size()) {
      continue;
    }
    curr_node->set_ports(new_ports);
  }

  remove_writeonly_shared_local_ports();
}

bool CMarsIrV2::is_shared_port(void *sg_init_name) {
  return mSharedPorts.count(sg_init_name) > 0;
}

void CMarsIrV2::update_all_edges() {
  mIrEdges.clear();
  for (auto &edge : mIrEdgesList) {
    delete edge;
  }
  mIrEdgesList.clear();

  int num_nodes = size();
  for (int i = 0; i != num_nodes; ++i) {
    CMarsNode *curr_node0 = get_node(i);

    for (int j = 0; j != num_nodes; ++j) {
      if (i == j) {
        continue;
      }
      CMarsNode *curr_node1 = get_node(j);
#if 0
      vector<void*> comm_loops = curr_node0->get_common_loops(curr_node1);
      if (comm_loops.size() <= 0 && curr_node0->is_after(curr_node1))
        continue;
#endif
      set<void *> shared_ports = curr_node0->get_common_ports(curr_node1);

      set<void *>::iterator it;
      for (it = shared_ports.begin(); it != shared_ports.end(); it++) {
        void *port = *it;
        CMarsEdge *edge = GetEdgeByNode(curr_node0, curr_node1, port);
        mIrEdgesList.push_back(edge);
        assert(edge);
      }
    }
  }
}

bool CMarsIrV2::isEmptyStmtSequence(const vector<void *> &stmts) {
  assert(m_ast);
  if (stmts.empty()) {
    return true;
  }
  for (size_t i = 0; i != stmts.size(); ++i) {
    void *stmt = stmts[i];
    if (m_ast->IsPragmaDecl(stmt) != 0) {
      continue;
    }
    if (m_ast->IsVariableDecl(stmt) != 0) {
      void *var_init = m_ast->GetVariableInitializedName(stmt);
      if (m_ast->GetInitializer(var_init) == nullptr) {
        continue;
      }
    }
    return false;
  }
  return true;
}

bool CMarsIrV2::check_identical_indices(const set<void *> &refs) {
  assert(m_ast);
  if (refs.empty()) {
    return true;
  }
  auto refs_iter = refs.begin();
  CMarsAccess ac0(*refs_iter, m_ast, nullptr);
  auto dim_to_var0 = ac0.simple_type_v1_get_dim_to_var();
  if (!ac0.is_simple_type_v1()) {
    return false;
  }
  while (++refs_iter != refs.end()) {
    CMarsAccess ac(*refs_iter, m_ast, nullptr);
    if (!ac.is_simple_type_v1()) {
      return false;
    }
    auto dim_to_var = ac.simple_type_v1_get_dim_to_var();
    if (dim_to_var0.size() != dim_to_var.size()) {
      return false;
    }
    for (size_t i = 0; i < dim_to_var0.size(); ++i) {
      auto index_info0 = dim_to_var0[i];
      auto index_info = dim_to_var[i];
      if (index_info0 == index_info) {
        continue;
      }
      //  check whether indices are related to inner loops
      //  and their range are matched
      void *loop0 = std::get<0>(index_info0);
      void *loop = std::get<0>(index_info);
      if ((loop0 == nullptr) || (loop == nullptr)) {
        return false;
      }
      CMarsLoop *loop_info0 = get_loop_info(loop0);
      CMarsLoop *loop_info = get_loop_info(loop);
      if ((loop_info0->is_complete_unroll() == 0) ||
          (loop_info->is_complete_unroll() == 0)) {
        return false;
      }
      CMarsRangeExpr mr0 = ac0.GetRangeExpr(i);
      CMarsRangeExpr mr = ac.GetRangeExpr(i);
      CMarsExpression l0;
      CMarsExpression u0;
      CMarsExpression l;
      CMarsExpression u;
      if ((mr0.get_simple_bound(&l0, &u0) == 0) ||
          (mr.get_simple_bound(&l, &u) == 0)) {
        return false;
      }
      if (((l0 - l != 0) != 0) || ((u0 - u != 0) != 0)) {
        return false;
      }
    }
  }
  return true;
}

#if USED_CODE_IN_COVERAGE_TEST
void *CMarsIrV2::get_unique_wrapper_pragma(void *sg_init) {
  CSageCodeGen *ast = m_ast;
  CMarsIrV2 *mars_ir = this;

  void *kernel = ast->TraceUpToFuncDecl(sg_init);
  string func_name = ast->GetFuncName(kernel);
  if (!mars_ir->is_kernel(kernel))
    return nullptr;

  func_name = MERLIN_WRAPPER_PREFIX + func_name;
  void *wrapper = ast->GetFuncDeclByName(func_name);
  if (!wrapper) {
    return nullptr;
  }
  if (!ast->GetFuncBody(wrapper))
    return nullptr;

  vector<void *> vec_pragmas;
  ast->GetNodesByType_int(wrapper, "preorder", V_SgPragmaDeclaration,
                          &vec_pragmas);

  int hit = 0;
  void *sg_pragma = nullptr;
  for (auto &pragma : vec_pragmas) {
    string s_var = mars_ir->get_pragma_attribute(pragma, "port");
    string s_intf = mars_ir->get_pragma_attribute(pragma, "wrapper");
    boost::algorithm::to_lower(s_intf);
    if (s_var == ast->_up(sg_init) && s_intf == "wrapper") {
      if (hit != 0)
        return nullptr;
      sg_pragma = pragma;
      hit++;
    }
  }

  return sg_pragma;
}

void *CMarsIrV2::get_unique_interface_pragma(void *sg_init) {
  CSageCodeGen *ast = m_ast;
  CMarsIrV2 *mars_ir = this;
  void *kernel = ast->TraceUpToFuncDecl(sg_init);

  if (!mars_ir->is_kernel(kernel))
    return nullptr;

  void *body = ast->GetFuncBody(kernel);
  if (!ast->IsBasicBlock(body))
    return nullptr;

  vector<void *> vec_pragmas;
  ast->GetNodesByType_int(kernel, "preorder", V_SgPragmaDeclaration,
                          &vec_pragmas);

  int hit = 0;
  void *sg_pragma = nullptr;
  for (auto &pragma : vec_pragmas) {
    string s_var = mars_ir->get_pragma_attribute(pragma, CMOST_variable);
    string s_intf = mars_ir->get_pragma_attribute(pragma, CMOST_interface);
    boost::algorithm::to_upper(s_intf);
    if (s_var == ast->_up(sg_init) && s_intf == CMOST_interface) {
      if (hit != 0)
        return nullptr;
      sg_pragma = pragma;
      hit++;
    }
  }

  return sg_pragma;
}
#endif

int CMarsIrV2 ::get_node_idx(CMarsNode *node) {
  for (size_t j = 0; j < size(); ++j) {
    CMarsNode *node0 = get_node(j);
    if (node0 == node) {
      return j;
    }
  }
  return -1;
}

void CMarsIrV2::update_dependence_for_all_edges() {
  int num_nodes = size();
  for (int i = 0; i != num_nodes; ++i) {
    CMarsNode *curr_node0 = get_node(i);

    for (int j = 0; j != num_nodes; ++j) {
      if (i == j) {
        continue;
      }
      CMarsNode *curr_node1 = get_node(j);

      set<void *> shared_ports = curr_node0->get_common_ports(curr_node1);

      set<void *>::iterator it;
      for (it = shared_ports.begin(); it != shared_ports.end(); it++) {
        void *port = *it;
        CMarsEdge *edge = GetEdgeByNode(curr_node0, curr_node1, port);
        CMarsDepVector vec = edge->GetDependence();
        DEBUG(
            cout << "--dep_vector (2L+1) = "; for (CMarsDepVector::iterator it =
                                                       vec.begin();
                                                   it != vec.end(); it++) {
              cout << *it << ",";
            } cout << endl);
      }
    }
  }
}

#if USED_CODE_IN_COVERAGE_TEST
int CMarsIrV2::get_number_of_all_sub_nodes(void *kernel,
                                           const vector<int> &order_prefix) {
  if (mKernel2IrNodeRange.count(kernel) <= 0)
    return 0;
  pair<int, int> range = mKernel2IrNodeRange[kernel];
  bool found = false;
  int count = 0;
  for (int i = range.first; i != range.second; ++i) {
    CMarsNode *node = get_node(i);
    if (node->is_subNode(kernel, order_prefix)) {
      count++;
      found = true;
    } else if (found) {
      return count;
    }
  }
  return count;
}
#endif

void CMarsIrV2::get_order_of_sub_nodes(void *kernel,
                                       const vector<int> &order_prefix,
                                       vector<int> *sub_orders) {
  if (mKernel2IrNodeRange.count(kernel) <= 0) {
    return;
  }
  pair<int, int> range = mKernel2IrNodeRange[kernel];
  bool found = false;
  set<int> visited;
  int depth = order_prefix.size();
  for (int i = range.first; i != range.second; ++i) {
    CMarsNode *node = get_node(i);
    if (node->is_subNode(kernel, order_prefix)) {
      int sub_order = node->get_order(depth);
      if (visited.count(sub_order) > 0) {
        continue;
      }
      sub_orders->push_back(sub_order);
      found = true;
      visited.insert(sub_order);
    } else if (found) {
      return;
    }
  }
}

#if USED_CODE_IN_COVERAGE_TEST
vector<vector<int>> CMarsIrV2::get_order_for_loop(void *loop) {
  vector<vector<int>> res;
  map<void *, vector<vector<int>>>::iterator i = mLoop2Orders.find(loop);
  if (i == mLoop2Orders.end())
    return res;
  return i->second;
}
#endif

void *CMarsIrV2::get_loop_for_order(void *kernel_top,
                                    const vector<int> &order) {
  map<void *, map<vector<int>, void *>>::iterator i =
      mOrder2Loop.find(kernel_top);
  if (i == mOrder2Loop.end()) {
    return nullptr;
  }
  map<vector<int>, void *>::iterator j = i->second.find(order);
  if (j == i->second.end()) {
    return nullptr;
  }
  return j->second;
}

vector<CMarsNode *> CMarsIrV2::get_nodes_with_common_loop(void *loop) {
  vector<CMarsNode *> res;
  for (size_t i = 0; i != size(); ++i) {
    CMarsNode *curr = get_node(i);
    if (curr->contains(loop)) {
      res.push_back(curr);
    }
  }
  return res;
}

void CMarsIrV2::get_top_loop_info(
    map<void *, map<void *, bool>> *top_loop_2_ports,
    map<void *, vector<void *>> *top_loops) {
  assert(m_ast);
  size_t i = 0;
  map<void *, map<void *, access_type>> top_loop_2_port_access_type;
  map<void *, set<void *>> loop_2_preded_loops;
  map<void *, set<void *>> loop_2_succed_loops;
  while (i < size()) {
    CMarsNode *node = get_node(i);
    void *kernel_top = node->get_user_kernel();
    vector<int> order_res;
    get_order_of_sub_nodes(kernel_top, vector<int>(), &order_res);
    for (size_t j = 0; j != order_res.size(); ++j) {
      void *loop = get_loop_for_order(kernel_top, vector<int>(1, order_res[j]));
      if (loop != nullptr) {
        (*top_loops)[kernel_top].push_back(loop);
      }
    }
    map<void *, set<void *>> loop2ports;
    for (auto top_loop : (*top_loops)[kernel_top]) {
      vector<CMarsNode *> nodes = get_nodes_with_common_loop(top_loop);
      set<void *> merged_ports;
      for (size_t k = 0; k != nodes.size(); ++k) {
        CMarsNode *node = nodes[k];
        auto ports = node->get_ports();
        merged_ports.insert(ports.begin(), ports.end());
        for (auto &port : ports) {
          top_loop_2_port_access_type[top_loop][port] =
              static_cast<access_type>(
                  top_loop_2_port_access_type[top_loop][port] |
                  node->get_port_access_type(port));
        }
      }
      loop2ports[top_loop] = merged_ports;
    }

    for (auto loop0 : (*top_loops)[kernel_top]) {
      set<void *> orig_ports = loop2ports[loop0];
      set<void *> shared_ports;
      map<void *, bool> empty;
      (*top_loop_2_ports)[loop0] = empty;
      for (auto loop1 : (*top_loops)[kernel_top]) {
        if (loop0 == loop1) {
          continue;
        }
        set<void *> res;
        set<void *> other_ports = loop2ports[loop1];
        set_intersection(orig_ports.begin(), orig_ports.end(),
                         other_ports.begin(), other_ports.end(),
                         std::inserter(res, res.begin()));

        shared_ports.insert(res.begin(), res.end());
        for (auto &port : res) {
          //  FIXME: loop dependency circle
          if (top_loop_2_port_access_type[loop1][port] == WRITE &&
              top_loop_2_port_access_type[loop0][port] == READ) {
            loop_2_preded_loops[loop0].insert(loop1);
            loop_2_succed_loops[loop1].insert(loop0);
          }
        }
      }
      set<void *> new_merged_ports = shared_ports;
      for (auto &port : shared_ports) {
        if (is_kernel_port(port)) {
          (*top_loop_2_ports)[loop0][port] = true;
        } 
      }
      for (auto &port : orig_ports) {
        if (is_kernel_port(port)) {
          (*top_loop_2_ports)[loop0][port] = true;
        }
      }
    }
    //  sort top loops according to data dependency
    vector<void *> tmp_top_loops = (*top_loops)[kernel_top];
    (*top_loops)[kernel_top].clear();
    list<void *> q;
    for (auto loop : tmp_top_loops) {
      if (loop_2_preded_loops.count(loop) <= 0) {
        q.push_back(loop);
      }
    }

    while (!q.empty()) {
      void *top = q.front();
      q.pop_front();
      (*top_loops)[kernel_top].push_back(top);
      for (auto loop : loop_2_succed_loops[top]) {
        loop_2_preded_loops[loop].erase(top);
        if (loop_2_preded_loops[loop].empty()) {
          q.push_back(loop);
        }
      }
    }
    set<void *> visited((*top_loops)[kernel_top].begin(),
                        (*top_loops)[kernel_top].end());
    for (auto &loop : tmp_top_loops) {
      if (visited.count(loop) > 0) {
        continue;
      }
      (*top_loops)[kernel_top].push_back(loop);
    }

    pair<int, int> range = get_node_range(kernel_top);
    i = range.second;
  }
}

#if USED_CODE_IN_COVERAGE_TEST
vector<void *> CMarsIrV2::get_common_loops(vector<CMarsNode *> *nodes) {
  vector<void *> res;
  if (nodes->size() <= 0)
    return res;
  res = (*nodes)[0]->get_loops();
  for (size_t i = 1; i < nodes->size(); ++i) {
    vector<void *> tmp = res;
    res.clear();
    vector<void *> other_loops = (*nodes)[i]->get_loops();
    int max_comm_level = min(tmp.size(), other_loops.size());
    for (int j = 0; j != max_comm_level; ++j) {
      if (tmp[j] == other_loops[j])
        res.push_back(tmp[j]);
      else
        break;
    }
  }
  return res;
}
#endif

bool CMarsIrV2::check_identical_indices_without_simple_type(
    const set<void *> &refs) {
  assert(m_ast);
  if (refs.empty()) {
    return true;
  }
  auto refs_iter = refs.begin();
  CMarsAccess ac0(*refs_iter, m_ast, nullptr);
  while (++refs_iter != refs.end()) {
    CMarsAccess ac(*refs_iter, m_ast, nullptr);
    if (ac0.GetIndexes().size() != ac.GetIndexes().size()) {
      return false;
    }
    int dim_size = ac0.GetIndexes().size();
    for (int i = 0; i < dim_size; ++i) {
      CMarsExpression diff = ac0.GetIndex(i) - ac.GetIndex(i);
      if ((diff == 0) != 0) {
        continue;
      }
      if (diff.GetConstant() != 0) {
        return false;
      }
      auto vec_vars = diff.get_vars();
      for (auto var : vec_vars) {
        //  check whether indices are related to inner loops
        if (m_ast->IsForStatement(var) == 0) {
          return false;
        }
        void *loop = var;
        CMarsLoop *loop_info = get_loop_info(loop);
        if (loop_info->is_complete_unroll() == 0) {
          return false;
        }
      }
      CMarsRangeExpr mr0 = ac0.GetRangeExpr(i);
      CMarsRangeExpr mr = ac.GetRangeExpr(i);
      CMarsExpression l0;
      CMarsExpression u0;
      CMarsExpression l;
      CMarsExpression u;
      if ((mr0.get_simple_bound(&l0, &u0) == 0) ||
          (mr.get_simple_bound(&l, &u) == 0)) {
        return false;
      }
      if (((l0 - l != 0) != 0) || ((u0 - u != 0) != 0)) {
        return false;
      }
    }
  }
  return true;
}

void CMarsIrV2::update_ssst_flag_for_all_edges() {
  assert(m_ast);
  //  for each edge
  set<void *> visited;
  for (auto &edge : get_all_edges()) {
    void *array = edge->GetPort();
    if (visited.count(array) > 0) {
      continue;
    }
    visited.insert(array);
    //  0. check if the array is an host interface
    if (is_kernel_port(array)) {
      continue;
    }
    DEBUG(cout << "check ssst flag for variable \'"
               << m_ast->GetVariableName(array) << "\'" << endl);
    //  1. # of access is two
    vector<CMarsEdge *> edges_of_array;
    for (auto &edge_t : get_all_edges()) {
      void *array_t = edge_t->GetPort();
      if (array_t == array) {
        edges_of_array.push_back(edge_t);
      }
    }
    assert(!edges_of_array.empty());
    if (edges_of_array.size() > 2) {
      DEBUG(cout << "Not ssst because of multiple access nodes" << endl);
      continue;
    }
    assert(edges_of_array.size() == 2);

    for (int i = 0; i != 2; ++i) {
      CMarsEdge *one_edge = edges_of_array[i];
      //  2. the first one is write and the second one is read, ignore the
      //  inverse edge
      CMarsNode *node0 = one_edge->GetNode(0);
      CMarsNode *node1 = one_edge->GetNode(1);

      bool write0 = node0->port_is_write(array);
      //  bool write1 = node1->port_is_write(array);

      //  bool read0 = node0->port_is_read(array);
      bool read1 = node1->port_is_read(array);

      if (!(write0 && read1)) {
        continue;
      }

      if (node0->is_after(node1)) {
        continue;
      }

      if (m_ast->IsScalarType(array) != 0) {
        one_edge->set_ssst(true);
        continue;
      }

      //  3. index simple form check
      auto ref0 = node0->get_port_references(array);
      auto ref1 = node1->get_port_references(array);
      bool simple_type = true;
      for (auto &ref : ref0) {
        CMarsAccess ac(ref, m_ast, nullptr);
        if (ac.is_simple_type_v1()) {
          continue;
        }
        simple_type = false;
        break;
      }
      for (auto &ref : ref1) {
        CMarsAccess ac(ref, m_ast, nullptr);
        if (ac.is_simple_type_v1()) {
          continue;
        }
        simple_type = false;
        break;
      }
      if (!simple_type) {
        DEBUG(cout << "Not ssst because of complex indices" << endl);
        continue;
      }
      //  4. same index for multiple references
      if (!check_identical_indices(ref0) || !check_identical_indices(ref1)) {
        DEBUG(cout << "Not ssst because of multiple references with "
                   << "different indices" << endl);
        continue;
      }
      //  5. access range is matched
      {
        void *range_scope = nullptr;
        auto loops = node0->get_common_loops(node1);
        if (!loops.empty()) {
          range_scope = m_ast->GetLoopBody(loops.back());
        }

        vector<CMarsRangeExpr> range0 =
            node0->get_port_access_union(array, range_scope, false);
        vector<CMarsRangeExpr> range1 =
            node1->get_port_access_union(array, range_scope, true);

        bool is_match = true;
        if (range0.size() != range1.size()) {
          is_match = false;
        } else {
          for (size_t i = 0; i < range0.size(); i++) {
            CMarsRangeExpr r0 = range0[i];
            CMarsRangeExpr r1 = range1[i];

            if (!((r0.is_exact() != 0) && (r1.is_exact() != 0))) {
              is_match = false;
              break;
            }

            CMarsExpression lb0;
            CMarsExpression ub0;
            CMarsExpression lb1;
            CMarsExpression ub1;
            if (!((r0.get_simple_bound(&lb0, &ub0) != 0) &&
                  (r1.get_simple_bound(&lb1, &ub1) != 0))) {
              is_match = false;
              break;
            }

            if (!(((lb0 - lb1 == 0) != 0) && ((ub0 - ub1 == 0) != 0))) {
              is_match = false;
              break;
            }
          }
        }
        if (!is_match) {
          DEBUG(cout << "Not ssst flag because of range mismatch or inexact"
                     << endl);
          continue;
        }
      }
      one_edge->set_ssst(true);
    }
  }
}

CMarsLoop *CMarsIrV2::get_loop_info(void *loop) {
  auto ret = mLoopInfo.find(loop);
  if (ret != mLoopInfo.end()) {
    return ret->second;
  }
  CMarsLoop *loop_info = new CMarsLoop(loop, m_ast);
  mLoopInfo[loop] = loop_info;
  vector<void *> vec_pragma;
  m_ast->GetNodesByType_int(loop, "preorder", V_SgPragmaDeclaration,
                            &vec_pragma);
  for (auto pragma_decl : vec_pragma) {
    if (m_ast->TraceUpToForStatement(pragma_decl) == loop) {
      loop_info->analyze_pragma(pragma_decl);
    }
  }
  int64_t trip_count = -1;
  int64_t max_trip_count = -1;
  if (m_ast->get_loop_trip_count(loop, &trip_count, &max_trip_count)) {
    loop_info->set_trip_count(trip_count, max_trip_count);
  }
  return loop_info;
}

void CMarsIrV2::remove_all_pragmas() {
  vector<void *> vec_pragma(mPragmas.begin(), mPragmas.end());
  for (size_t i = 0; i < vec_pragma.size(); i++) {
    void *decl = vec_pragma[i];
    m_ast->RemoveStmt(decl);
  }
  mPragmas.clear();
}

set<void *> CMarsIrV2::get_shared_ports(CMarsNode *node) {
  set<void *> res;
  for (auto &port : node->get_ports()) {
    if (is_shared_port(port)) {
      res.insert(port);
    }
  }
  return res;
}

bool CMarsIrV2::remove_writeonly_shared_local_ports() {
  vector<void *> shared_ports(mSharedPorts.begin(), mSharedPorts.end());
  bool Changed = false;
  for (auto &port : shared_ports) {
    if (is_kernel_port(port)) {
      continue;
    }
    vector<CMarsNode *> nodes;
    bool write_only = true;
    for (auto &node : mIrNodes) {
      auto ports = node->get_ports();
      if (ports.count(port) <= 0) {
        continue;
      }
      if (node->port_is_read(port)) {
        write_only = false;
        break;
      }
      nodes.push_back(node);
    }
    if (!write_only) {
      continue;
    }
    mSharedPorts.erase(port);
    for (auto &node : nodes) {
      node->remove_port(port);
      Changed = true;
    }
  }
  return Changed;
}

string CMarsIrV2::get_depth_string(vector<size_t> size, int cont_flag) {
  string ret;
  size_t mult = 1;
  for (auto t : size) {
    ret += my_itoa(t) + ",";
    mult *= t;
  }

  int n_size = ret.size();
  string str_non_cont = ret.substr(0, n_size - 1);

  return cont_flag != 0 ? my_itoa(mult) : str_non_cont;
}

//  ////////////////////////////////////////////  /
//  Peng Zhang
//  Sep 7, 2016
//  Note: this function returns the constant dimension information of the
//  multi-dimensional arrays
//  ////////////////////////////////////////////  /
//
//  Source of dimension info (in priority order)
//   1. array dimension
//   2. interface/attribute pragma
//   3. malloc/new
//   4. induction from alias
//  Determination:
//   1. stop analysis if high priority source determines
//   2. report error if multiple mismatched results found in the same priority
//  Type of the input init_name
//   1. local variable/global variable
//   2. formal function argument
//   3. member variable
//  Input position (pos):
//   ??
//  Output
//   1. positive number if determined
//   2. zero if not determined
//   3. leftmost is the highest dimension (inversed from get_type_dimension)
//  Process
//   1. Get dimension first from type declaration
//   2. For each dimension, go through the remaining three kinds of sources
vector<size_t> CMarsIrV2::get_array_dimension(void *sg_name, int *cont_flag,
                                              void *pos) {
  if (pos == nullptr) {
    pos = sg_name;
  }

  void *sg_type = m_ast->GetTypebyVar(sg_name);

  void *sg_base_type;
  vector<size_t> ret_size;

  if (m_ast->IsScalarType(sg_type) != 0) {
    return ret_size;
  }

  m_ast->get_type_dimension_new(sg_type, &sg_base_type, &ret_size, pos);
  int dim = ret_size.size();

  *cont_flag = m_ast->is_all_dim_array(sg_type) != 0 ? 1 : 0;
  if (m_ast->IsStructureType(sg_base_type) != 0) {
    *cont_flag = 0;
  }

  //  FIXME: ZP 20160926: Can not assume cont_flag for MD structure array
  //  *cont_flag = 0; //  20170104: ZP: why not? to be confirmed

  int has_more = 0;
  int dim_size = ret_size.size();
  for (int i = 0; i < dim_size; i++) {
    //   1. array dimension
    if (0 < static_cast<int64_t>(ret_size[i])) {
      continue;
    }

    size_t n_size = get_dimension_by_pragma(sg_name, i, cont_flag);
    //   2. interface/attribute pragma
    ret_size[i] = n_size;
    if (0 < static_cast<int64_t>(ret_size[i])) {
      continue;
    }

    has_more = 1;
  }

  //   3. Induct from above: malloc/new (FIXME: now only handle one-D array)
  if (has_more != 0) {
    int arg_idx = m_ast->GetFuncParamIndex(sg_name);
    if (-1 != arg_idx) {
      void *sg_func_decl = m_ast->TraceUpToFuncDecl(sg_name);

      vector<void *> vec_calls;
      m_ast->GetFuncCallsFromDecl(sg_func_decl, nullptr, &vec_calls);

      vector<pair<int, string>> vec_pre_call;
      vec_pre_call.resize(dim);

      vector<int> mismatch;
      mismatch.resize(dim, 0);
      for (auto sg_call : vec_calls) {
        void *new_pntr = m_ast->GetFuncCallParam(sg_call, arg_idx);
        string curr_msg = m_ast->_up(new_pntr) + "at" + m_ast->_up(sg_call);
        //  YX: 20180127 do not cross function to avoid infinite recurisve
        //  analysis
        void *ref = nullptr;
        m_ast->parse_array_ref_from_pntr(new_pntr, &ref);
        if (ref == nullptr) {
          DEBUG(cout << "Warning: Dimension sizes unknown on "
                        "the interface \n";
                cout << ": " << curr_msg << endl);
          std::fill(mismatch.begin(), mismatch.end(), 1);
          continue;
        }
        CMarsAccess ma(ref, m_ast, m_ast->TraceUpToFuncDecl(sg_call));
        void *sg_array2;
        void *basic_type2;
        vector<size_t> vecSize2;
        int ret = m_ast->get_type_dimension_by_ref(ref, &sg_array2,
                                                   &basic_type2, &vecSize2);

        if ((ret != 0) && vecSize2.size() == static_cast<size_t>(dim)) {
          for (int j = 0; j < dim; j++) {
            if (mismatch[j] != 0) {
              continue;
            }
            if ((ma.GetIndex(ma.get_dim() - dim + j) != 0) != 0) {
              //  Youxiang: 20171103 (Bug1743) if passed with
              //  offset, we had better not use the original
              //  dimension size
              mismatch[j] = 1;
              continue;
            }
            auto &size = vec_pre_call[j].first;
            auto &msg = vec_pre_call[j].second;

            if (static_cast<int64_t>(vecSize2[j]) > 0) {
              if (static_cast<int64_t>(size) <= 0) {
                size = vecSize2[j];
                msg = curr_msg;
              } else if (static_cast<size_t>(size) != vecSize2[j]) {
                mismatch[j] = 1;
                DEBUG(cout << "Warning: Dimension sizes mismatch on "
                              "the interface \n";

                      cout << "  (1): " << msg << endl;
                      cout << "  (2): " << curr_msg << endl);
              }
            }
          }
        } else {
          DEBUG(cout << "Warning: Dimension sizes unknown on "
                        "the interface \n";
                cout << ": " << curr_msg << endl);
          std::fill(mismatch.begin(), mismatch.end(), 1);
        }
      }

      for (int i = 0; i < dim; i++) {
        if (0 < static_cast<int64_t>(ret_size[i])) {
          continue;
        }

        if (mismatch[i] == 0) {
          ret_size[i] = vec_pre_call[i].first;
        }
        if (0 < static_cast<int64_t>(ret_size[i])) {
          continue;
        }
      }
    }
  }

  //   4. Induct from below: alias (FIXME)
  {}

  return ret_size;
}

vector<size_t> CMarsIrV2::get_array_depth(void *sg_name, void *pos) {
  if (pos == nullptr) {
    pos = sg_name;
  }

  void *sg_type = m_ast->GetTypebyVar(sg_name);

  void *sg_base_type;
  vector<size_t> ret_size;

  if (m_ast->IsScalarType(sg_type) != 0) {
    return ret_size;
  }

  m_ast->get_type_dimension_new(sg_type, &sg_base_type, &ret_size, pos);
  int dim = ret_size.size();
  int pragma_depth_dim = mDimensionPragma[sg_name].size();
  if (pragma_depth_dim > dim) {
    ret_size.resize(pragma_depth_dim);
    dim = pragma_depth_dim;
  }
  for (int i = 0; i < dim; i++) {
    size_t n_size = get_depth_by_pragma(sg_name, i, pos);
    if (static_cast<int64_t>(n_size) > 0) {
      ret_size[i] = n_size;
    }
  }
  return ret_size;
}

//  return the last hit pragma which has the variable
void *CMarsIrV2::get_pragma_by_variable(string s_var, void *scope,
                                        string filter) {
  vector<void *> vec_pragmas;
  m_ast->GetNodesByType_int(scope, "preorder", V_SgPragmaDeclaration,
                            &vec_pragmas);

  void *sg_pragma = nullptr;
  string u_filter = filter;
  boost::algorithm::to_upper(u_filter);
  for (auto &pragma : vec_pragmas) {
    string s_var1 = get_pragma_attribute(pragma, CMOST_variable);
    string s_cmd = get_pragma_attribute(pragma, filter);
    boost::algorithm::to_upper(s_cmd);
    if (s_var1 == s_var && s_cmd == u_filter) {
      sg_pragma = pragma;
    }
  }
  return sg_pragma;
}

string CMarsIrV2::get_pragma_command(void *sg_pragma, bool allow_hls) {
  string str_pragma = m_ast->GetPragmaString(sg_pragma);

  map<string, pair<vector<string>, vector<string>>> mapParams;
  string sFilter;
  string sCmd;
  tldm_pragma_parse_whole(str_pragma, &sFilter, &sCmd, &mapParams, allow_hls);

  if (is_cmost_pragma(sFilter, allow_hls) == 0) {
    return "";
  }

  string u_sCmd = sCmd;
  boost::algorithm::to_upper(u_sCmd);
  return u_sCmd;
}

string CMarsIrV2::get_pragma_attribute(void *sg_pragma, string key,
                                       bool allow_hls) {
  string str_pragma = m_ast->GetPragmaString(sg_pragma);

  string u_key = key;
  boost::algorithm::to_upper(u_key);
  string l_key = key;
  boost::algorithm::to_lower(l_key);
  map<string, pair<vector<string>, vector<string>>> mapParams;
  string sFilter;
  string sCmd;
  tldm_pragma_parse_whole(str_pragma, &sFilter, &sCmd, &mapParams, allow_hls);

  if (is_cmost_pragma(sFilter, allow_hls) == 0) {
    return "";
  }

  string u_sCmd = sCmd;
  boost::algorithm::to_upper(u_sCmd);
  if (u_sCmd == u_key) {
    return sCmd;
  }
  if (mapParams.find(u_key) != mapParams.end()) {
    key = u_key;
  } else if (mapParams.find(l_key) != mapParams.end()) {
    key = l_key;
  }
  if (mapParams.find(key) != mapParams.end()) {
    if (key == "parallel") {
      return "__merlin_parallel";
    }
    if (key == "pipeline") {
      return "__merlin_pipeline";
    }
    if (mapParams[key].first.empty()) {
      return key;
    }
    { return mapParams[key].first[0]; }
  }

  return "";
}

//  Yuxin: 05/02/2018
//  Add the attribute parsing for the third party pragmas
string CMarsIrV2::get_all_pragma_attribute(void *sg_pragma, string key,
                                           bool *is_cmost) {
  string str_pragma = m_ast->GetPragmaString(sg_pragma);

  string u_key = key;
  boost::algorithm::to_upper(u_key);
  string l_key = key;
  boost::algorithm::to_lower(l_key);
  map<string, pair<vector<string>, vector<string>>> mapParams;
  vector<string> mapKeys;
  string sFilter;
  string sCmd;
  all_pragma_parse_whole(str_pragma, &sFilter, &sCmd, &mapParams, &mapKeys);

  if (is_cmost_pragma(sFilter) == 0) {
    *is_cmost = false;
  }

  DEBUG(cout << sFilter << ", " << *is_cmost << endl);
  string u_sCmd = sCmd;
  boost::algorithm::to_upper(u_sCmd);
  if (u_sCmd == u_key) {
    return sCmd;
  }
  if (mapParams.find(u_key) != mapParams.end()) {
    key = u_key;
  } else if (mapParams.find(l_key) != mapParams.end()) {
    key = l_key;
  }
  if (mapParams.find(key) != mapParams.end()) {
    if (key == "parallel") {
      return "__merlin_parallel";
    }
    if (key == "pipeline") {
      return "__merlin_pipeline";
    }
    return mapParams[key].first[0];
  }

  return "";
}

#if USED_CODE_IN_COVERAGE_TEST
bool BothAreSpaces(char lhs, char rhs) { return (lhs == rhs) && (lhs == ' '); }
#endif

static void remove_multiple_space(string *str) {
  std::string::iterator new_end =
      std::unique(str->begin(), str->end(), [](char lhs, char rhs) {
        return (lhs == rhs) && (isspace(lhs) != 0);
      });
  str->erase(new_end, str->end());
}

void CMarsIrV2::set_pragma_attribute(void **sg_pragma, string key, string value,
                                     bool third_party) {
  string str_pragma = m_ast->GetPragmaString(*sg_pragma);

  string u_key = key;
  boost::algorithm::to_upper(u_key);
  string l_key = key;
  boost::algorithm::to_lower(l_key);
  map<string, pair<vector<string>, vector<string>>> mapParams;
  vector<string> mapKeys;
  string sFilter;
  string sCmd;
  if (!third_party) {
    // TODO(XXX) :pragma_parse should be in an indepenent utils
    tldm_pragma_parse_whole(str_pragma, &sFilter, &sCmd, &mapParams);
  } else {
    all_pragma_parse_whole(str_pragma, &sFilter, &sCmd, &mapParams, &mapKeys);
  }

  if ((is_cmost_pragma(sFilter) == 0) && !third_party) {
    return;
  }

  boost::algorithm::to_upper(sCmd);
  if (sCmd == u_key) {
    return;
  }

  string new_str = str_pragma;

  if (mapParams.find(u_key) != mapParams.end()) {
    key = u_key;
  } else if (mapParams.find(l_key) != mapParams.end()) {
    key = l_key;
  }
  auto white_space = [&](string s) {
    for (auto c : value) {
      if (std::isspace(c) != 0) {
        return true;
      }
    }
    return false;
  };

  if (mapParams.find(key) != mapParams.end()) {
    vector<string> vec_value;
    for (auto val : mapParams[key].first) {
      if (white_space(val)) {
        val = "\"" + val + "\"";
      }
      vec_value.push_back(val);
    }
    string orig_value = str_merge(',', vec_value);
    //  remove the original subclause

    //  FIXME: ZP 20161002
    //  Two cases need to be enhanced to support
    //  1. there is spaces between key and "="
    //  2. there is quote "" in the value part, e.g. name="Peter Huang"
    //

    remove_multiple_space(&new_str);
    boost::replace_all(new_str, " =", "=");
    boost::replace_all(new_str, "= ", "=");

    size_t j0 = new_str.find(key + "=");
    //  assert(j0 != string::npos);
    //  FIXME: Yuxin, keep the oiginal setting?
    if (j0 == string::npos)
      return;
    size_t j1 = j0 + (key + "=" + orig_value).size();
    string head = new_str.substr(0, j0);
    string tail = (j1 != string::npos ? new_str.substr(j1) : "");
    if (value.empty()) {
      new_str = head + key;
    } else {
      new_str = head + key + "=" +
                (white_space(value) ? ("\"" + value + "\"") : value) + tail;
    }
  } else {
    if (value.empty()) {
      new_str += " " + key;
    } else {
      new_str += " " + key + "=" +
                 (white_space(value) ? ("\"" + value + "\"") : value);
    }
  }
  *sg_pragma = m_ast->SetPragmaString(*sg_pragma, new_str);
}

//  Note:
//  This funtion fill up the two data structure mDimPragma and mDepthPragma from
//  the pragmas (and related assertions)
//  1. #pragma interface variable=xxx depth=xxx
//  2. #pragma attribute depth=xxx (followed by a varible declaration)
void CMarsIrV2::init_variable_info_pragma() {
  mDimensionPragma.clear();
  mDepthPragma.clear();
  mContinueFlag.clear();
  mBurstFlag.clear();
  mWriteOnlyFlag.clear();
  mBurstMaxSize.clear();
  mFalseDepFlag.clear();
  vector<void *> vec_pragmas;
  m_ast->GetNodesByType_int(m_ast->GetProject(), "preorder",
                            V_SgPragmaDeclaration, &vec_pragmas);

  for (auto pragma : vec_pragmas) {
    CAnalPragma ana_pragma(m_ast);
    bool errout;
    if (!ana_pragma.PragmasFrontendProcessing(pragma, &errout, false)) {
      continue;
    }
    void *sg_init = nullptr;
    string s_var;
    if (ana_pragma.is_interface() || ana_pragma.is_attribute() ||
        ana_pragma.is_coalescing()) {
      string str_depth = ana_pragma.get_attribute(CMOST_depth);
      string str_max_depth = ana_pragma.get_attribute(CMOST_max_depth);

      string msg;
      if (ana_pragma.is_interface() || ana_pragma.is_coalescing()) {
        s_var = ana_pragma.get_attribute(CMOST_variable);
        sg_init = m_ast->find_variable_by_name(s_var, pragma);
      }
      if (ana_pragma.is_attribute()) {
        void *sg_next = m_ast->GetNextStmt(pragma);
        if ((sg_next != nullptr) && (m_ast->IsVariableDecl(sg_next) != 0)) {
          sg_init = m_ast->GetVariableInitializedName(sg_next);
          s_var = m_ast->UnparseToString(sg_init);
        }
      }

      if (sg_init == nullptr) {
        continue;
      }
      if (ana_pragma.is_coalescing()) {
        string val = ana_pragma.get_attribute(CMOST_force);
        if (!val.empty()) {
          boost::algorithm::to_upper(val);
          if (val == CMOST_ON) {
            mCoalescingFlag[sg_init] = true;
          } else if (val == CMOST_OFF) {
            mCoalescingFlag[sg_init] = false;
          }
        }

        continue;
      }
      //  {

      //    string msg = "Can not determine the variable in pragma : \n
      //    variable="+
      //        s_var+"\n    pragma=" +
      //        str_pragma + " \n    at " + getUserCodeFileLocation(m_ast,
      //        pragma, false)+ "\n";
      //    if (check_valid)
      //       dump_critical_message("MDARR", "ERROR", msg, 301, 0);

      //    cout << "[ERROR@Outline]" << msg << endl;
      //    exit(1);
      //  }

      //  cout << "Array depth from Pragma: " <<
      //  m_ast->UnparseToString(sg_init) << " " << depth << endl; cout <<
      //  " at " << m_ast->UnparseToString(pragma) << endl;
      if (!ana_pragma.get_attribute(CMOST_continue).empty()) {
        mContinueFlag[sg_init] = ana_pragma.get_attribute(CMOST_continue);
      }

      //  //////////////////  /
      if (!ana_pragma.get_attribute(CMOST_burst_off).empty()) {
        mBurstFlag[sg_init] = false;
      } else {
        string val = ana_pragma.get_attribute(CMOST_memory_burst);
        if (!val.empty()) {
          boost::algorithm::to_upper(val);
          if (val == CMOST_ON) {
            mBurstFlag[sg_init] = true;
          } else if (val == CMOST_OFF) {
            mBurstFlag[sg_init] = false;
          }
        }
      }
      string burst_max_size = ana_pragma.get_attribute(CMOST_burst_max_size);
      if (!burst_max_size.empty()) {
        mBurstMaxSize[sg_init] = my_atoi(burst_max_size);
      }
      if (!ana_pragma.get_attribute(CMOST_write_only).empty()) {
        mWriteOnlyFlag[std::make_pair(sg_init, m_ast->GetScope(pragma))] = true;
      }
      //  ///////////////////  /

      vector<string> sizes = str_split(str_depth, ',');

      vector<string> max_sizes = str_split(str_max_depth, ',');

      if (mDimensionPragma.find(sg_init) == mDimensionPragma.end()) {
        vector<size_t> empty;
        mDimensionPragma[sg_init] = empty;
        vector<void *> empty0;
        mDepthPragma[sg_init] = empty0;
      }

      for (size_t i = 0; i < max(sizes.size(), max_sizes.size()); ++i) {
        string s_depth;
        if (i < sizes.size()) {
          s_depth = sizes[i];
        }
        size_t n_max_depth = 0;
        size_t n_depth = 0;
        if (i < max_sizes.size()) {
          n_max_depth = my_atoi(max_sizes[i]);
        }

        if (n_depth == 0 && !s_depth.empty()) {
          n_depth = my_atoi(s_depth);
        }
        if (n_depth == 0) {
          if (!s_depth.empty()) {
            void *sg_depth_init = m_ast->find_variable_by_name(s_depth, pragma);
            mDepthPragma[sg_init].push_back(sg_depth_init);
            if (sg_depth_init != nullptr) {
              CMarsRangeExpr mr =
                  get_variable_range(sg_depth_init, m_ast, pragma, nullptr);
              if (mr.is_const_ub() != 0) {
                size_t n_ub = mr.get_const_ub();
                mDimensionPragma[sg_init].push_back(n_ub);
                DEBUG(cout << "--" << m_ast->UnparseToString(sg_init) << "["
                           << my_itoa(i) << "] " << my_itoa(n_ub) << ","
                           << m_ast->_p(sg_depth_init) << endl);
                continue;
              }
            }
          } else {
            mDepthPragma[sg_init].push_back(nullptr);
          }
          if (n_max_depth != 0U) {
            mDimensionPragma[sg_init].push_back(n_max_depth);
          } else {
            mDimensionPragma[sg_init].push_back(0);
          }
        } else {
          mDepthPragma[sg_init].push_back(nullptr);
          mDimensionPragma[sg_init].push_back(n_depth);

          DEBUG(cout << "--" << m_ast->UnparseToString(sg_init) << "["
                     << my_itoa(i) << "] " << my_itoa(n_depth) << endl);
        }
      }
    }
    if (ana_pragma.is_false_dep()) {
      s_var = ana_pragma.get_attribute(CMOST_variable);
      sg_init = m_ast->find_variable_by_name(s_var, pragma);
      if (sg_init != nullptr) {
        mFalseDepFlag[sg_init] = true;
      }
    }
  }
}

size_t CMarsIrV2::get_dimension_by_pragma(void *sg_name, int i,
                                          int *cont_flag) {
  if (!(mContinueFlag.find(sg_name) == mContinueFlag.end())) {
    *cont_flag = mContinueFlag[sg_name] == "true" ? 1 : 0;
  }

  if (mDimensionPragma.find(sg_name) == mDimensionPragma.end()) {
    return 0;
  }
  if (mDimensionPragma[sg_name].size() <= static_cast<size_t>(i)) {
    return 0;
  }

  return mDimensionPragma[sg_name][i];
}

size_t CMarsIrV2::get_depth_by_pragma(void *sg_name, int i, void *pos) {
  if (mDepthPragma.find(sg_name) == mDepthPragma.end()) {
    return 0;
  }
  if (mDepthPragma[sg_name].size() <= static_cast<size_t>(i)) {
    return 0;
  }
  void *depth = mDepthPragma[sg_name][i];
  int cont_flag = 0;
  if (nullptr == depth) {
    return get_dimension_by_pragma(sg_name, i, &cont_flag);
  }
  CMarsRangeExpr mr = get_variable_range(depth, m_ast, pos, nullptr);
  return mr.get_const_ub();
}

bool CMarsIrV2::get_burst_flag(void *sg_name) {
  if (mBurstFlag.count(sg_name) <= 0) {
    return true;
  }
  return mBurstFlag[sg_name];
}

bool CMarsIrV2::get_write_only_flag(void *sg_name, void *scope) {
  if (mWriteOnlyFlag.count(std::make_pair(sg_name, scope)) <= 0) {
    return false;
  }
  return mWriteOnlyFlag[std::make_pair(sg_name, scope)];
}

#if USED_CODE_IN_COVERAGE_TEST
bool CMarsIrV2::get_false_dep_flag(void *sg_name) {
  if (mFalseDepFlag.count(sg_name) <= 0)
    return false;
  return mFalseDepFlag[sg_name];
}
#endif

bool CMarsIrV2::get_coalescing_flag(void *sg_name) {
  if (mCoalescingFlag.count(sg_name) <= 0) {
    return false;
  }
  return mCoalescingFlag[sg_name];
}

size_t CMarsIrV2::get_burst_max_size(void *sg_name) {
  if (mBurstMaxSize.count(sg_name) <= 0) {
    return 0;
  }
  return mBurstMaxSize[sg_name];
}

void CMarsIrV2::get_function_bound(void *func_decl, set<void *> *bound_set) {
  bound_set->clear();
  if (is_kernel(func_decl)) {
    bound_set->insert(func_decl);
    return;
  }
  vector<void *> vec_calls;
  m_ast->GetFuncCallsFromDecl(func_decl, nullptr, &vec_calls);
  //  m_ast->GetNodesByType_int(nullptr, "preorder", V_SgFunctionCallExp,
  //  &vec_calls);
  for (auto call : vec_calls) {
    //  void *callee = m_ast->GetFuncDeclByCall(call, 0);
    //  if (!callee || !m_ast->isSameFunction(callee, func_decl)) continue;
    void *caller = m_ast->TraceUpToFuncDecl(call);
    if (bound_set->count(caller) > 0) {
      continue;
    }
    set<void *> bound_sub_set;
    get_function_bound(caller, &bound_sub_set);
    bound_set->insert(bound_sub_set.begin(), bound_sub_set.end());
  }
  if (bound_set->empty()) {
    bound_set->insert(func_decl);
  }
}

//  get all the possible defined nodes
//  tuple<CMarsNode*,
//  bool /*true: definitely dominate *; false: may dominate/,
//  int /* 1: forward; 0: itself; -1: backwrd/>
vector<tuple<CMarsNode *, bool, int>>
CMarsIrV2::get_coarse_grained_def(CMarsNode *node, void *port) {
  vector<tuple<CMarsNode *, bool, int>> res;
#if 1
  if (m_ast->IsScalarType(port) != 0) {
#if PROJDEBUG
    cout << "Cannot split scalar variable \'" << m_ast->GetVariableName(port)
         << "\'"
         << " because its def/use is inaccurate" << endl;
#endif
    return res;
  }
#endif
  if (!node->port_is_read(port)) {
    return res;
  }

  vector<CMarsEdge *> edges_of_array;
  for (auto &edge_t : get_all_edges()) {
    void *array_t = edge_t->GetPort();
    if (array_t == port) {
      edges_of_array.push_back(edge_t);
    }
  }
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
  //  sort all the nodes according to original schedule vector
  sort(all_nodes.begin(), all_nodes.end(),
       [](CMarsNode *node0, CMarsNode *node1) {
         return !(node0->is_after(node1));
       });

  int pos = -1;
  for (size_t i = 0; i != all_nodes.size(); ++i) {
    if (all_nodes[i] == node) {
      pos = i;
      break;
    }
  }
  if (pos == -1) {
    return res;
  }
  if (node->port_is_write(port)) {
    //  if there is write access within the same node,
    //  return conservative result
    res.push_back(std::make_tuple(node, false, 0));
  }
  bool pre_dominate = false;
  bool post_dominate = false;
  auto loops = node->get_loops();
  int comm_loops_depth = static_cast<int>(loops.size() + 1);
  while (--comm_loops_depth >= 0) {
    for (int i = pos - 1; i >= 0; --i) {
      CMarsNode *curr_node = all_nodes[i];
      if (!curr_node->port_is_write(port)) {
        continue;
      }
      void *range_scope = nullptr;
      auto comm_loops = node->get_common_loops(curr_node);
      if (comm_loops.size() != static_cast<size_t>(comm_loops_depth)) {
        continue;
      }
      if (!comm_loops.empty()) {
        range_scope = m_ast->GetLoopBody(comm_loops.back());
      }
      vector<CMarsRangeExpr> range0 =
          curr_node->get_port_access_union(port, range_scope, false);
      vector<CMarsRangeExpr> range1 =
          node->get_port_access_union(port, range_scope, true);
      bool is_cover = true;
      if (range0.size() != range1.size()) {
        is_cover = false;
      } else {
        for (size_t k = 0; k < range0.size(); k++) {
          CMarsRangeExpr r0 = range0[k];
          CMarsRangeExpr r1 = range1[k];
          CMarsRangeExpr res;
          if (r0.is_cover(r1, &res) == 0) {
            is_cover = false;
          }
        }
      }
      res.push_back(std::make_tuple(curr_node, is_cover, true));
      if (is_cover) {
        pre_dominate = true;
        break;
      }
    }
    if (pre_dominate) {
      break;
    }
    if (post_dominate) {
      continue;
    }
    for (int i = static_cast<int>(all_nodes.size() - 1); i > pos; --i) {
      CMarsNode *curr_node = all_nodes[i];
      if (!curr_node->port_is_write(port)) {
        continue;
      }
      auto comm_loops = node->get_common_loops(curr_node);
      if (comm_loops.empty()) {
        continue;
      }
      //  check whether current comm_loops is outside
      //  pre_dominated_common_loops
      if (comm_loops.size() != static_cast<size_t>(comm_loops_depth)) {
        continue;
      }
      void *range_scope = m_ast->GetLoopBody(comm_loops.back());
      vector<CMarsRangeExpr> range0 =
          curr_node->get_port_access_union(port, range_scope, false);
      vector<CMarsRangeExpr> range1 =
          node->get_port_access_union(port, range_scope, true);
      bool is_cover = true;
      if (range0.size() != range1.size()) {
        is_cover = false;
      } else {
        for (size_t k = 0; k < range0.size(); k++) {
          CMarsRangeExpr r0 = range0[k];
          CMarsRangeExpr r1 = range1[k];
          CMarsRangeExpr res;
          if (r0.is_cover(r1, &res) == 0) {
            is_cover = false;
          }
        }
      }
      res.push_back(std::make_tuple(curr_node, is_cover, -1));
      if (is_cover) {
        post_dominate = true;
        break;
      }
    }
  }

  return res;
}

bool CMarsIrV2::has_definite_coarse_grained_def(CMarsNode *node, void *port) {
  auto res = get_coarse_grained_def(node, port);
  for (auto def : res) {
    if (std::get<1>(def) && std::get<2>(def) == 1) {
      //  has a definite forward def
      return true;
    }
  }
  return false;
}

#if USED_CODE_IN_COVERAGE_TEST
bool CMarsIrV2::iswithininnermostloop(void *stmt) {
  void *body = m_ast->TraceUpToBasicBlock(stmt);
  if (!body)
    return false;

  vector<void *> sub_loops;
  m_ast->GetNodesByType_int(body, "preorder", V_SgForStatement, &sub_loops);

  for (auto sub_loop : sub_loops) {
    CMarsLoop *loop_info = get_loop_info(sub_loop);
    if (loop_info->is_complete_unroll())
      continue;
    void *scope = m_ast->GetParent(sub_loop);
    bool is_innermost = false;
    while (scope != body) {
      if (is_undistributed_statement(scope)) {
        is_innermost = true;
        break;
      }
      scope = m_ast->GetParent(scope);
    }
    if (!is_innermost)
      return false;
  }
  return true;
}
#endif

bool CMarsIrV2::is_flatten_loop(void *loop, const set<void *> &unrolled_loops) {
  if (unrolled_loops.count(loop) <= 0) {
    return false;
  }
  if (mFlattenLoop.count(loop) > 0) {
    return mFlattenLoop[loop];
  }
  vector<void *> sub_loops;
  void *loop_body = m_ast->GetLoopBody(loop);
  m_ast->GetNodesByType_int(loop_body, "preorder", V_SgForStatement,
                            &sub_loops);
  for (auto sub_loop : sub_loops) {
    if (!is_flatten_loop(sub_loop, unrolled_loops)) {
      return false;
    }
  }
  vector<void *> vec_calls;
  m_ast->GetNodesByType_int(loop_body, "preorder", V_SgFunctionCallExp,
                            &vec_calls);
  for (auto call_exp : vec_calls) {
    void *sub_func = m_ast->GetFuncDeclByCall(call_exp, 1);
    if ((sub_func != nullptr) && mFunc2Loop[sub_func]) {
      return false;
    }
    if (m_ast->GetFuncNameByCall(call_exp) == "memcpy") {
      return false;
    }
  }
  return true;
}

void CMarsIrV2::collect_all_kernel_nodes() {
  m_var_used_by_kernel.clear();
  m_func_used_by_kernel.clear();
  m_type_used_by_kernel.clear();
  unordered_set<void *> visited;
  for (auto top_kernel : mKernelTops) {
    collect_all_kernel_nodes(top_kernel, &visited);
  }
  m_valid_kernel_node_analysis = true;
}

void CMarsIrV2::collect_all_kernel_nodes(void *scope,
                                         unordered_set<void *> *visited) {
  if (visited->count(scope) > 0) {
    return;
  }
  if (auto *tc = isSgTemplateInstantiationDecl(static_cast<SgNode *>(scope))) {
    for (auto *targ : tc->get_templateArguments()) {
      if (auto *tt = targ->get_type()) {
        process_typedef(tt, visited);
        void *base_type = m_ast->GetBaseType(tt, true);
        process_base_type(base_type, visited);
      } else if (auto *te = targ->get_expression()) {
        collect_all_kernel_nodes(te, visited);
      }
    }
  }
  if (auto *tf =
          isSgTemplateInstantiationFunctionDecl(static_cast<SgNode *>(scope))) {
    for (auto *targ : tf->get_templateArguments()) {
      if (auto *tt = targ->get_type()) {
        process_typedef(tt, visited);
        void *base_type = m_ast->GetBaseType(tt, true);
        process_base_type(base_type, visited);
      } else if (auto *te = targ->get_expression()) {
        collect_all_kernel_nodes(te, visited);
      }
    }
  }
  if (void *tp_class = m_ast->GetTemplateClassDecl(scope)) {
    if (m_ast->IsFromMerlinSystemFile(tp_class) != 0) {
      return;
    }
  }
  if (void *tp_func = m_ast->GetTemplateFuncDecl(scope)) {
    if (m_ast->IsFromMerlinSystemFile(tp_func) != 0) {
      return;
    }
  }
  if (void *tp_mem_func = m_ast->GetTemplateMemFuncDecl(scope)) {
    if (m_ast->IsFromMerlinSystemFile(tp_mem_func) != 0) {
      return;
    }
  }
  visited->insert(scope);
  if (m_ast->IsFunctionDeclaration(scope) != 0) {
    vector<void *> vec_decls = m_ast->GetAllFuncDecl(scope);
    for (auto decl : vec_decls) {
      m_func_used_by_kernel.insert(decl);
      auto *func_symbol = static_cast<SgFunctionDeclaration *>(decl)
                              ->search_for_symbol_from_symbol_table();
      if (func_symbol != nullptr)
        m_func_used_by_kernel.insert(func_symbol);
      collect_all_kernel_nodes(decl, visited);
    }
  }
  set<void *> set_init;
  vector<void *> vec_init;
  m_ast->GetNodesByType_int(scope, "preorder", V_SgInitializedName, &vec_init);
  set_init.insert(vec_init.begin(), vec_init.end());
  auto vars_used_in_pragma =
      m_ast->GetVariablesUsedByPragmaInScope(scope, false);
  set_init.insert(vars_used_in_pragma.begin(), vars_used_in_pragma.end());
  vector<void *> vec_ref;
  m_ast->GetNodesByType_int(scope, "preorder", V_SgVarRefExp, &vec_ref);
  for (auto ref : vec_ref) {
    void *var_init = m_ast->GetVariableInitializedName(ref);
    set_init.insert(var_init);
  }
  set<void *> vec_base_type;
  for (auto init : set_init) {
    m_var_used_by_kernel.insert(init);
    auto var_symbol = isSgInitializedName(static_cast<SgNode *>(init))
                          ->search_for_symbol_from_symbol_table();
    if (var_symbol != nullptr)
      m_var_used_by_kernel.insert(var_symbol);
    void *type = m_ast->GetTypebyVar(init);
    process_typedef(type, visited);
    void *base_type = m_ast->GetBaseType(type, true);
    if (!is_known_type(base_type)) {
      vec_base_type.insert(base_type);
    }
    void *initilizer = m_ast->GetInitializer(init);
    if (initilizer != nullptr) {
      collect_all_kernel_nodes(initilizer, visited);
    }
    void *name_scope = m_ast->TraceUpToClassDecl(init);
    if (name_scope != nullptr) {
      collect_type_decl(name_scope, visited);
    }
  }
  vector<void *> vec_decls;
  m_ast->GetNodesByType_int_compatible(scope, V_SgFunctionDeclaration,
                                       &vec_decls);
  {
    vector<void *> vec_calls;
    m_ast->GetNodesByType_int(scope, "preorder", V_SgFunctionCallExp,
                              &vec_calls);
    for (auto call : vec_calls) {
      void *decl = m_ast->GetFuncDeclByCall(call);
      if (decl == nullptr) {
        decl = m_ast->GetFuncDeclByCall(call, 0);
      }
      if (decl != nullptr) {
        vec_decls.push_back(decl);
      }
    }
  }

  {
    vector<void *> vec_func_refs;
    m_ast->GetNodesByType_int(scope, "preorder", V_SgFunctionRefExp,
                              &vec_func_refs);
    for (auto func_ref : vec_func_refs) {
      void *decl = m_ast->GetFuncDeclByRef(func_ref);
      if (decl != nullptr) {
        vec_decls.push_back(decl);
      }
    }
  }
  {
    vector<void *> vec_exp;
    m_ast->GetNodesByType_int_compatible(scope, V_SgExpression, &vec_exp);
    for (auto exp : vec_exp) {
      void *exp_type = nullptr;
      SgSizeOfOp *sizeof_op = isSgSizeOfOp(static_cast<SgNode *>(exp));
      SgConstructorInitializer *ctor_init =
          isSgConstructorInitializer(static_cast<SgNode *>(exp));
      if (sizeof_op != nullptr) {
        exp_type = sizeof_op->get_operand_type();
        if (exp_type == nullptr) {
          continue;
        }
      } else if (ctor_init != nullptr) {
        void *decl = ctor_init->get_declaration();
        if (decl != nullptr) {
          vec_decls.push_back(decl);
        }
        continue;
      } else {
        exp_type = m_ast->GetTypeByExp(exp);
      }
      if (exp_type == nullptr) {
        continue;
      }
      void *exp_base_type = m_ast->GetBaseType(exp_type, true);
      SgFunctionType *func_type =
          isSgFunctionType(static_cast<SgNode *>(exp_base_type));
      if (func_type == nullptr) {
        continue;
      }
      SgSymbol *sg_symbol = func_type->get_symbol_from_symbol_table();
      SgFunctionSymbol *func_symbol = isSgFunctionSymbol(sg_symbol);
      if (func_symbol == nullptr) {
        continue;
      }
      void *decl = func_symbol->get_declaration();
      if (decl != nullptr) {
        vec_decls.push_back(decl);
      }
    }
  }
  for (auto decl : vec_decls) {
    void *ret_type = m_ast->GetFuncReturnType(decl);
    process_typedef(ret_type, visited);
    void *ret_base_type = m_ast->GetBaseType(ret_type, true);
    if (!is_known_type(ret_base_type)) {
      vec_base_type.insert(ret_base_type);
    }
    m_func_used_by_kernel.insert(decl);
    auto *func_symbol = static_cast<SgFunctionDeclaration *>(decl)
                            ->search_for_symbol_from_symbol_table();
    if (func_symbol != nullptr)
      m_func_used_by_kernel.insert(func_symbol);
    void *tp_decl = m_ast->GetTemplateFuncDecl(decl);
    if ((tp_decl != nullptr) && (m_ast->IsFromMerlinSystemFile(tp_decl) == 0)) {
      m_func_used_by_kernel.insert(tp_decl);
      auto *func_symbol = static_cast<SgFunctionDeclaration *>(tp_decl)
                              ->search_for_symbol_from_symbol_table();
      if (func_symbol != nullptr)
        m_func_used_by_kernel.insert(func_symbol);
    }
    collect_all_kernel_nodes(decl, visited);

    if (m_ast->IsMemberFunction(decl)) {
      void *name_scope = m_ast->GetTypeDeclByMemberFunction(decl);
      if (m_ast->IsClassDecl(name_scope) != 0) {
        collect_type_decl(name_scope, visited);
      }
    }
  }

  vec_decls.clear();
  m_ast->GetNodesByType_int_compatible(scope, V_SgClassDeclaration, &vec_decls);
  {
    vector<void *> vec_enum_decls;
    m_ast->GetNodesByType_int(scope, "preorder", V_SgEnumDeclaration,
                              &vec_enum_decls);
    vec_decls.insert(vec_decls.end(), vec_enum_decls.begin(),
                     vec_enum_decls.end());
  }
  {
    vector<void *> vec_typedef;
    m_ast->GetNodesByType_int(scope, "preorder", V_SgTypedefDeclaration,
                              &vec_typedef);
    for (auto decl : vec_typedef) {
      void *base_type = m_ast->GetBaseType(
          static_cast<SgTypedefDeclaration *>(decl)->get_type(), true);
      if (!is_known_type(base_type)) {
        vec_base_type.insert(base_type);
      }
    }
    vec_decls.insert(vec_decls.end(), vec_typedef.begin(), vec_typedef.end());
  }

  {
    vector<void *> vec_exp;
    m_ast->GetNodesByType_int_compatible(scope, V_SgExpression, &vec_exp);
    for (auto exp : vec_exp) {
      void *exp_type = nullptr;
      SgSizeOfOp *sizeof_op = isSgSizeOfOp(static_cast<SgNode *>(exp));
      SgConstructorInitializer *ctor_init =
          isSgConstructorInitializer(static_cast<SgNode *>(exp));
      if (sizeof_op != nullptr) {
        exp_type = sizeof_op->get_operand_type();
        if (exp_type == nullptr) {
          continue;
        }
      } else if (ctor_init != nullptr) {
        void *type_decl = ctor_init->get_class_decl();
        if (type_decl != nullptr) {
          vec_decls.push_back(type_decl);
        }
        continue;
      } else if (m_ast->IsEnumVal(exp) != 0) {
        exp_type = m_ast->GetTypeByEnumVal(exp);
      } else {
        exp_type = m_ast->GetTypeByExp(exp);
      }
      if (exp_type == nullptr) {
        continue;
      }
      process_typedef(exp_type, visited);
      void *exp_base_type = m_ast->GetBaseType(exp_type, true);
      if (!is_known_type(exp_base_type)) {
        vec_base_type.insert(exp_base_type);
      }
    }
  }
  for (auto base_type : vec_base_type) {
    process_base_type(base_type, visited);
  }

  for (auto type_decl : vec_decls) {
    collect_type_decl(type_decl, visited);
    void *name_scope = m_ast->TraceUpToClassDecl(m_ast->GetParent(type_decl));
    if (name_scope != nullptr) {
      collect_type_decl(name_scope, visited);
    }
  }
}

void CMarsIrV2::process_base_type(void *base_type,
                                  unordered_set<void *> *visited) {
  void *type_decl = m_ast->GetTypeDeclByType(base_type);
  if (type_decl != nullptr) {
    collect_type_decl(type_decl, visited);
    void *name_scope = m_ast->TraceUpToClassDecl(m_ast->GetParent(type_decl));
    if (name_scope != nullptr) {
      collect_type_decl(name_scope, visited);
    }
  } else {
    type_decl = m_ast->GetTypeDeclByType(base_type, 0);
    if (type_decl != nullptr) {
      collect_type_decl(type_decl, visited);
    }
  }
}

void CMarsIrV2::collect_type_decl(void *decl, unordered_set<void *> *visited) {
  m_type_used_by_kernel.insert(decl);
  if (auto cls_decl = isSgClassDeclaration(static_cast<SgNode *>(decl))) {
    auto *cls_symbol = cls_decl->search_for_symbol_from_symbol_table();
    if (cls_symbol != nullptr)
      m_type_used_by_kernel.insert(cls_symbol);
  }
  if (void *tp_class = m_ast->GetTemplateClassDecl(decl)) {
    if (m_ast->IsFromMerlinSystemFile(tp_class) == 0) {
      m_type_used_by_kernel.insert(tp_class);
      auto *cls_symbol = static_cast<SgClassDeclaration *>(tp_class)
                             ->search_for_symbol_from_symbol_table();
      if (cls_symbol != nullptr)
        m_type_used_by_kernel.insert(cls_symbol);
    }
  }
  for (auto base_decl : m_ast->GetInheritances(decl)) {
    collect_type_decl(base_decl, visited);
  }
  collect_all_kernel_nodes(decl, visited);
}

void CMarsIrV2::process_typedef(void *type, unordered_set<void *> *visited) {
  void *parent_type = type;
  do {
    if (m_ast->IsTypedefType(parent_type) != 0) {
      void *type_decl = m_ast->GetTypeDeclByType(parent_type, 0);
      if (type_decl != nullptr) {
        void *type_scope = m_ast->TraceUpToClassDecl(type_decl);
        if (type_scope != nullptr) {
          collect_type_decl(type_scope, visited);
        }
      }
      m_type_used_by_kernel.insert(type_decl);
    }
    void *base_type = m_ast->GetBaseTypeOneLayer(parent_type);
    if (base_type == parent_type)
      break;
    parent_type = base_type;
  } while (true);
}

bool CMarsIrV2::is_tiled_loop(void *scope) {
  if (m_ast->IsForStatement(scope) == 0) {
    return false;
  }
  CMarsLoop *loop_info = get_loop_info(scope);
  return loop_info->is_tiled_loop() != 0;
}

#if USED_CODE_IN_COVERAGE_TEST
void *CMarsIrV2::TraceUpToOneKernelByPos(void *pos) {
  vector<void *> hit_kernels;
  for (auto one_kernel : get_top_kernels()) {
    if (m_ast->is_located_in_scope(pos, one_kernel))
      hit_kernels.push_back(one_kernel);
  }
  if (hit_kernels.size() == 0 || hit_kernels.size() > 1)
    return nullptr;
  else
    return hit_kernels[0];
}
#endif
