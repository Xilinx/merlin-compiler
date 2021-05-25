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


#include "memory_coalescing_intel.h"
#include <iostream>
#include <string>
#include <tuple>
//  #include "rose.h"
#include "mars_message.h"
#include "mars_opt.h"
#include "program_analysis.h"

using MarsProgramAnalysis::CMarsAccess;
using MarsProgramAnalysis::CMarsExpression;
using MarsProgramAnalysis::CMarsRangeExpr;
using std::tuple;

static unsigned int find_max_factor(uint64_t loop_len,
                                    unsigned int upper_bound) {
  if (loop_len <= upper_bound) {
    return loop_len;
  }
  set<int> factors;
  for (uint64_t i = 1; i <= sqrt(loop_len); ++i) {
    if ((loop_len % i) == 0) {
      factors.insert(i);
      factors.insert(loop_len / i);
    }
  }
  auto it = factors.lower_bound(upper_bound);
  if (it == factors.end() || *it > upper_bound) {
    it--;
  }
  return *it;
}

bool MemoryCoalescingIntel::checkInvalidBitWidth(int opt_bitwidth,
                                                 int orig_bitwidth) {
  return (opt_bitwidth % orig_bitwidth) != 0 || opt_bitwidth <= 0;
}

bool MemoryCoalescingIntel::run() {
  if (mNaiveHLS) {
    return false;
  }
  std::cout << "==============================================" << std::endl;
  std::cout << "-----=# wide bus Interface Optimization Start#=----\n";
  std::cout << "==============================================" << std::endl;
  bool Changed = false;

  vector<CMirNode *> vec_nodes;
  mMars_ir.get_topological_order_nodes(&vec_nodes);
  for (auto node : vec_nodes) {
    CMirNode *fNode = node;
    if (!fNode->check_kernel()) {
      continue;
    }
#if PROJDEBUG
    cout << "found the kenerl function" << endl;
#endif
    Changed |= update_interface(fNode);
  }
  std::cout << "============================================" << std::endl;
  std::cout << "-----=# wide bus Interface Optimization End#=----\n";
  std::cout << "============================================" << std::endl
            << std::endl;

  return Changed;
}

void MemoryCoalescingIntel::init(CSageCodeGen *Codegen) {
  int option_num = mOptions.get_option_num("-a");
  for (int i = 0; i != option_num; ++i) {
    if ("naive_hls" == mOptions.get_option("-a", i)) {
      mNaiveHLS = true;
      cout << "[MARS-REDUCTION-MSG] Naive HLS mode.\n";
      return;
    }
  }
  string bitwidth_str = mOptions.get_option_key_value("-a", "bus_bitwidth");
  if (!bitwidth_str.empty()) {
    mBitWidth = my_atoi(bitwidth_str);
    mBitWidthSpecified = true;
  }
  mMars_ir.get_mars_ir(Codegen, mTopFunc, mOptions, true);
  mMars_ir_v2.build_mars_ir(Codegen, mTopFunc);
}

bool MemoryCoalescingIntel::update_interface(CMirNode *fNode) {
  void *top_decl = m_ast->GetFuncDeclByDefinition(m_ast->GetParent(fNode->ref));
  assert(top_decl);
  mFunc_defs.clear();
  vector<void *> func_decls;

  //  ZP: 20160812: call graph is not stable, so we moved to here so that it can
  //  be turned off
  //  m_ast->init_callgraph();

  if (!m_ast->GetTopoOrderFunctions(top_decl, &func_decls)) {
    return false;
  }
  for (size_t i = 0; i < func_decls.size(); ++i) {
    void *decl = func_decls[i];
    if (m_ast->IsMerlinInternalIntrinsic(m_ast->GetFuncName(decl))) {
      continue;
    }
    void *def = m_ast->GetFuncDefinitionByDecl(decl);
    if (def != nullptr) {
      if (m_ast->UnSupportedMemberFunction(decl)) {
        string func_name = m_ast->GetFuncName(decl);
        auto func_user_info = getUserCodeInfo(m_ast, decl);
        if (!func_user_info.name.empty()) {
          func_name = func_user_info.name;
        }
        string func_info =
            "\'" + func_name + "\' (" + func_user_info.file_location + ")";
        dump_critical_message(WDBUS_WARNING_7(func_info));
        return false;
      }
      mFunc_defs.push_back(def);
    }
  }
  collectInterfaceInfo();
  bool Changed = false;
  Changed |= updateFunctionInterface();
  return Changed;
}

void MemoryCoalescingIntel::collectInterfaceInfo() {
  for (size_t i = mFunc_defs.size(); i > 0; --i) {
    void *func_def = mFunc_defs[i - 1];
    collectInterfaceInfo(func_def);
  }
}

void MemoryCoalescingIntel::collectInterfaceInfo(void *func_def) {
  SgFunctionDeclaration *func_decl =
      isSgFunctionDefinition(static_cast<SgNode *>(func_def))
          ->get_declaration();
  map<void *, int> paralists;
  int param_num = m_ast->GetFuncParamNum(func_decl);
  void *unique_decl = m_ast->GetUniqueFuncDecl(func_decl);
  if (unique_decl == nullptr) {
    return;
  }
  mFunc2Info[unique_decl].resize(param_num);
  vector<ArgumentInfo> &arg_info = mFunc2Info[unique_decl];
  vector<void *> vec_pragma;
  void *func_body = m_ast->GetFuncBody(func_decl);
  m_ast->GetNodesByType(func_body, "preorder", "SgPragmaDeclaration",
                        &vec_pragma);

  for (int i = 0; i != param_num; ++i) {
    void *arg = m_ast->GetFuncParam(func_decl, i);
    if (mMars_ir.every_trace_is_bus(m_ast, func_decl, arg)) {
      paralists[arg] = i;
    }
    void *type = m_ast->GetTypebyVar(arg);
    void *base_type = m_ast->GetBaseType(type, false);
    ArgumentInfo &info = arg_info[i];
    info.base_type = base_type;
    info.bitwidth = m_ast->get_bitwidth_from_type(base_type);
    info.all_support_access =
        mMars_ir.every_trace_is_bus(m_ast, func_decl, arg);
    if (info.bitwidth <= 0) {
      continue;
    }
    string arg_name = m_ast->GetVariableName(arg);
    auto arg_user_info = getUserCodeInfo(m_ast, arg);
    if (!arg_user_info.name.empty()) {
      arg_name = arg_user_info.name;
    }
    string var_info =
        "'" + arg_name + "' (" + arg_user_info.file_location + ")";
    bool disable_opt_bitwidth = false;
    for (auto pragma : vec_pragma) {
      if (mMars_ir_v2.get_pragma_attribute(pragma, "variable") == arg_name) {
        string bw_str =
            mMars_ir_v2.get_pragma_attribute(pragma, "bus_bitwidth");
        if (!bw_str.empty()) {
          int bitwidth = my_atoi(bw_str);
          if (!checkInvalidBitWidth(bitwidth, info.bitwidth) ||
              bitwidth == info.bitwidth) {
            info.opt_bitwidth = bitwidth;
          } else {
            boost::algorithm::to_lower(bw_str);
            //  disable_opt_bitwidth = bw_str == "off";
            disable_opt_bitwidth = true;
            if (bw_str == "off") {
              dump_critical_message(WDBUS_WARNING_12(var_info), 0, m_ast, arg);
            } else {
              dump_critical_message(WDBUS_WARNING_13(var_info, bw_str), 0,
                                    m_ast, arg);
            }
          }
        }
      }
    }
    if (info.opt_bitwidth == 0) {
      if (!disable_opt_bitwidth &&
          !checkInvalidBitWidth(mBitWidth, info.bitwidth)) {
        info.opt_bitwidth = mBitWidth;
      } else {
        info.opt_bitwidth = info.bitwidth;
      }
    }
  }
  vector<void *> vecCalls;
  m_ast->GetNodesByType(func_def, "preorder", "SgFunctionCallExp", &vecCalls);
  //  analyze all sub function calls
  for (size_t i = 0; i < vecCalls.size(); ++i) {
    void *func_call = vecCalls[i];
    void *sub_func_decl = m_ast->GetFuncDeclByCall(func_call);
    if (m_ast->IsMerlinInternalIntrinsic(m_ast->GetFuncName(sub_func_decl))) {
      continue;
    }
    bool is_memcpy = isMemcpy(func_call);
    if (is_memcpy) {
      struct MemcpyInfo info;
      int valid = m_ast->parse_memcpy(
          func_call, &info.length_exp, &info.port[0].array, &info.port[0].pntr,
          &info.port[0].index, &info.port[0].pointer, &info.port[1].array,
          &info.port[1].pntr, &info.port[1].index, &info.port[1].pointer);
      if (valid == 0) {
#if PROJDEBUG
        cout << "cannot inline memcpy " << m_ast->_p(func_call) << endl;
#endif
        string info = "    Memcpy: " + m_ast->_up(func_call) + " " +
                      getUserCodeFileLocation(m_ast, func_call, true) + "\n";
        info += "    In function: " +
                m_ast->_up(m_ast->TraceUpToFuncDecl(func_call)) + "\n";
        dump_critical_message(FUCIN_ERROR_4(info), 1);
        m_valid = false;
        continue;
      }
      if (paralists.count(info.port[0].array) > 0 &&
          paralists.count(info.port[1].array) > 0) {
        cout << "do not need to apply memory colescing for data transfer "
             << "between external memory ports" << endl;
        continue;
      }
      int arg_index = -1;
      for (int j = 0; j != 2; ++j) {
        void *port = info.port[j].array;
        string arg_name = m_ast->GetVariableName(port);
        auto arg_user_info = getUserCodeInfo(m_ast, port);
        if (!arg_user_info.name.empty()) {
          arg_name = arg_user_info.name;
        }

        string var_info =
            "'" + arg_name + "' (" + arg_user_info.file_location + ")\n";
        if (paralists.count(port) > 0) {
          arg_index = paralists[port];
          arg_info[arg_index].any_burst_access = true;
          info.read = j == 1;
          void *basic_type;
          void *buf = info.port[1 - j].array;
          vector<size_t> DSize;
          string buf_name = m_ast->GetVariableName(buf);
          string sBufFileLine = my_itoa(m_ast->GetFileInfo_line(buf));
          int dimension = m_ast->get_type_dimension(
              m_ast->GetTypebyVar(buf), &basic_type, &DSize, func_call);
          info.dimension = dimension;
          info.dimension_size = DSize;
          info.base_type = basic_type;
          int element_bitwidth = m_ast->get_bitwidth_from_type(basic_type);
          if (element_bitwidth == 0) {
            std::map<string, string> message;
            message["memory_coalescing"] = "off";
            message["memory_coalescing_off_reason"] =
                " unknown bitwidth for basic type \'" + m_ast->_up(basic_type) +
                "\'";
            insertMessage(m_ast, port, message);
            dump_critical_message(WDBUS_WARNING_14(var_info), 0, m_ast, port);

            arg_info[arg_index].all_support_access = false;
            return;
          }
          info.bitwidth = element_bitwidth;
          info.opt_bitwidth = arg_info[arg_index].opt_bitwidth;
          arg_info[arg_index].memcpy[func_call] = info;
          info.opt_dimension = info.dimension;
          if (info.opt_dimension > 1) {
            int curr_dim = 0;
            int element_bytes = element_bitwidth / 8;
            CMarsExpression me_total_offset =
                (CMarsExpression(info.length_exp, m_ast, func_call) /
                 element_bytes);
            int64_t curr_size = 1;
            while (curr_dim < dimension) {
              void *curr_index =
                  info.port[1 - j].index[dimension - 1 - curr_dim];
              CMarsExpression me_curr_offset =
                  CMarsExpression(curr_index, m_ast, func_call) * curr_size;
              me_total_offset = me_total_offset + me_curr_offset;
              CMarsRangeExpr mr_total_offset = me_total_offset.get_range();
              int64_t c_ub = mr_total_offset.get_const_ub();
              curr_size *= DSize[curr_dim];
              if (c_ub <= curr_size) {
                info.opt_dimension = curr_dim + 1;
                break;
              }
              curr_dim++;
            }
          }
          arg_info[arg_index].memcpy[func_call] = info;
        }
      }
    }
  }

  for (size_t i = 0; i != arg_info.size(); ++i) {
    arg_info[i].need_changed =
        ((static_cast<int>(arg_info[i].any_burst_access) &
          static_cast<int>(arg_info[i].all_support_access) &
          static_cast<int>(arg_info[i].opt_bitwidth > arg_info[i].bitwidth)) !=
         0);
  }
}

bool MemoryCoalescingIntel::updateFunctionInterface() {
  bool Changed = false;
  for (size_t i = 0; i != mFunc_defs.size(); ++i) {
    Changed |= updateFunctionInterface(mFunc_defs[i]);
  }
  return Changed;
}

bool MemoryCoalescingIntel::updateFunctionInterface(void *func_def) {
  bool Changed = false;
  SgFunctionDeclaration *func_decl =
      isSgFunctionDefinition(static_cast<SgNode *>(func_def))
          ->get_declaration();
  void *unique_decl = m_ast->GetUniqueFuncDecl(func_decl);
  if (unique_decl == nullptr) {
    return Changed;
  }
  assert(mFunc2Info.count(unique_decl) > 0);
  const vector<ArgumentInfo> &arg_info = mFunc2Info[unique_decl];
  int arg_num = m_ast->GetFuncParamNum(func_decl);
  vector<void *> calls;
  m_ast->GetFuncCallsFromDecl(func_decl, nullptr, &calls);
  //  1. update interface by changing the type
  string func_name = m_ast->GetFuncName(func_decl);
  vector<void *> all_decls(m_ast->GetAllFuncDecl(func_decl));
#if PROJDEBUG
  cout << "Current is in function " << func_name << endl;
#endif
  bool kernel = mMars_ir.is_kernel(m_ast, func_decl);
  for (int i = 0; i != arg_num; ++i) {
    ArgumentInfo info = arg_info[i];
    void *arg = m_ast->GetFuncParam(func_decl, i);
    string arg_name = m_ast->GetVariableName(arg);
    auto arg_user_info = getUserCodeInfo(m_ast, arg);
    if (!arg_user_info.name.empty()) {
      arg_name = arg_user_info.name;
    }
    string var_info = "'" + arg_name + "' ";
    var_info += "(" + arg_user_info.file_location + ")";
    if (info.need_changed) {
      string curr_type_info =
          "\'" + m_ast->GetStringByType(info.base_type) + "\'";
      string supported_type_info = str_merge(',', m_supported_types);

      if (!isSupportedType(info.base_type)) {
        string msg =
            "Memory coalescing NOT inferred: variable '" + arg_name + "'\n";
        msg += "  Reason: type " + curr_type_info;
        msg += "is not supported now.\n";
        msg += "  Note: " + supported_type_info + " are supported now.";
        if (kernel) {
          map<string, string> msg_map;
          msg_map["memory_coalescing"] = "off";
          msg_map["memory_coalescing_off_reason"] = "unsupported data type";
          insertMessage(m_ast, arg, msg_map);
          dump_critical_message(
              WDBUS_WARNING_10(var_info, curr_type_info, supported_type_info),
              0, m_ast, arg);
        }
        continue;
      }
#if PROJDEBUG
      cout << arg_name << " need to enlarge bit width" << endl;
#endif

      if (kernel) {
        string msg =
            "Memory coalescing inferred: variable '" + arg_name + "'\n";
        msg +=
            "  interface bitwidth = " + my_itoa(info.opt_bitwidth) + " bits\n";
        string bitwidth = my_itoa(info.opt_bitwidth);
        if (!info.suboptimal_coalescing) {
          dump_critical_message(WDBUS_INFO_2(var_info, bitwidth));
        }
      }
      Changed = true;
    }
  }
  if (!Changed) {
    return false;
  }
  //  2. update the sub function calls which are affected
  for (int i = 0; i != arg_num; ++i) {
    ArgumentInfo info = arg_info[i];
    if (!info.need_changed || !isSupportedType(info.base_type)) {
      continue;
    }
    for (map<void *, MemcpyInfo>::iterator ii = info.memcpy.begin();
         ii != info.memcpy.end(); ++ii) {
      Changed |= updateMemcpyCallWithLargeBitWidth(ii->second, ii->first);
    }
  }
  return Changed;
}

bool MemoryCoalescingIntel::isMemcpy(void *func_call) {
  void *sub_func_decl = m_ast->GetFuncDeclByCall(func_call);
  if (sub_func_decl == nullptr) {
    sub_func_decl = m_ast->GetFuncDeclByCall(func_call, 0);
  }
  string func_name = m_ast->GetFuncName(sub_func_decl);
  return func_name == "memcpy";
}

bool MemoryCoalescingIntel::updateMemcpyCallWithLargeBitWidth(
    const MemcpyInfo &info, void *orig_memcpy_call) {
  return memcpy_replace(info, orig_memcpy_call) != 0;
}

bool MemoryCoalescingIntel::isSupportedType(void *base_type) {
  string str = m_ast->GetStringByType(base_type);
  for (auto one_type : m_supported_types) {
    if (str == one_type) {
      return true;
    }
  }
  return false;
}

MemoryCoalescingIntel::~MemoryCoalescingIntel() {
  vector<void *> func_stmts(mDead_func_calls.begin(), mDead_func_calls.end());
  mDead_func_calls.clear();
  for (size_t i = 0; i != func_stmts.size(); ++i) {
    m_ast->RemoveStmt(func_stmts[i]);
  }
}

int MemoryCoalescingIntel::generate_nested_loops(
    void *sg_array1, const vector<void *> &sg_idx1, const vector<size_t> &dim1,
    void *sg_array2, const vector<void *> &sg_idx2, const vector<size_t> &dim2,
    void *sg_length, int type_size, void *func_call, const MemcpyInfo &info) {
  //  only support one is 1D array and the other is multiple dimension array
  if (dim1.size() > 1 && dim2.size() > 1) {
    return 0;
  }
  if (dim1.size() == 1 && dim2.size() == 1) {
    return 0;
  }
  CMarsExpression length(sg_length, m_ast, func_call);
  if (length.IsConstant() == 0) {
    return 0;
  }
  size_t c_length = length.GetConstant() / type_size;
  bool choose_dim1 = true;
  void *choose_array = sg_array1;
  vector<size_t> choose_dim = dim1;
  if (dim2.size() > 1) {
    choose_dim1 = false;
    choose_array = sg_array2;
    choose_dim = dim2;
  }

  size_t leftmost_dim = 0;
  size_t total_size = 1;
  bool aligned = false;
  while (leftmost_dim < choose_dim.size()) {
    total_size *= choose_dim[leftmost_dim];
    if (c_length == total_size) {
      aligned = true;
    } else if (c_length < total_size) {
      //  handle dimension size equal to 1
      leftmost_dim--;
      break;
    }
    leftmost_dim++;
  }
  size_t rightfirst_dim = choose_dim.size() - leftmost_dim;
  if (leftmost_dim == choose_dim.size()) {
    leftmost_dim--;
  }

  if (!aligned) {
    return 0;
  }

  vector<void *> idxs = choose_dim1 ? sg_idx1 : sg_idx2;
  for (size_t i = idxs.size() - 1, j = 0; j <= leftmost_dim; --i, ++j) {
    CMarsExpression idx_exp(idxs[i], m_ast, func_call);
    if ((idx_exp != 0) != 0) {
      return 0;
    }
  }

  map<int, int> reorder;
  for (size_t i = 0; i <= leftmost_dim; ++i) {
    reorder[i] = i;
  }
  {
    bool order_analysis = true;
    bool need_reorder = false;
    vector<int> orders;
    //  analysis the original access order
    void *func_decl = m_ast->TraceUpToFuncDecl(func_call);
    vector<void *> all_refs;
    m_ast->get_ref_in_scope(choose_array, func_decl, &all_refs);
    bool first = true;
    for (auto ref : all_refs) {
      CMarsAccess ac(ref, m_ast, func_decl);
      if (!ac.is_simple_type_v1()) {
        order_analysis = false;
        break;
      }
      auto index_info = ac.simple_type_v1_get_dim_to_var();
      if (index_info.size() != choose_dim.size()) {
        order_analysis = false;
        break;
      }
      vector<int> curr_order(choose_dim.size(), -1);
      set<int> parallel_loop_level;
      void *pos = ref;
      int level = 0;
      while (pos != nullptr) {
        void *curr_loop = m_ast->TraceUpToForStatement(pos);
        if (curr_loop == nullptr) {
          break;
        }
        for (size_t i = rightfirst_dim; i != choose_dim.size(); ++i) {
          if (std::get<0>(index_info[i]) == curr_loop) {
            curr_order[i] = level;
            CMarsLoop *loop_info = mMars_ir_v2.get_loop_info(curr_loop);
            if (loop_info->is_complete_unroll() != 0) {
              parallel_loop_level.insert(choose_dim.size() - 1 - i);
            }
            level++;
            break;
          }
        }
        pos = m_ast->GetParent(curr_loop);
      }

      bool is_all_const = true;
      for (size_t i = rightfirst_dim; i != choose_dim.size(); ++i) {
        if (curr_order[i] != -1) {
          is_all_const = false;
          break;
        }
      }
      if (is_all_const) {
        continue;
      }

      //  need to reorder only if a parallel loop level is in coarse grained
      //  level
      need_reorder |= !parallel_loop_level.empty() &&
                      static_cast<size_t>(*parallel_loop_level.rbegin()) >=
                          parallel_loop_level.size();
      for (size_t i = rightfirst_dim; i != choose_dim.size(); ++i) {
        if (curr_order[i] == -1) {
          order_analysis = false;
          break;
        }
      }
      if (first) {
        orders = curr_order;
        first = false;
      } else if (orders != curr_order) {
        order_analysis = false;
      }

      if (!order_analysis) {
        break;
      }
    }
    if (order_analysis && !first && need_reorder) {
      for (size_t i = rightfirst_dim; i != choose_dim.size(); ++i) {
        reorder[orders[i]] = choose_dim.size() - i - 1;
      }
    }
  }
  void *func_decl = m_ast->TraceUpToFuncDecl(func_call);
  void *func_body = m_ast->GetFuncBody(func_decl);
  vector<void *> loop_iters;
  vector<void *> loop_uppers;
  vector<int> loop_length;
  string order = my_itoa(m_count_memcpy);
  for (size_t i = 0; i <= leftmost_dim; ++i) {
    loop_uppers.push_back(m_ast->CreateConst(static_cast<int>(choose_dim[i])));
    loop_length.push_back(choose_dim[i]);
    string iter_name = "_memcpy_i" + order + "_" + my_itoa(i);
    m_ast->get_valid_local_name(func_decl, &iter_name);
    void *iter_decl =
        m_ast->CreateVariableDecl("int", iter_name, nullptr, func_body);
    m_ast->PrependChild(func_body, iter_decl, true);
    loop_iters.push_back(iter_decl);
  }
  void *new_pntr1 = choose_dim1 != 0 ? sg_array1 : sg_array2;
  void *new_pntr2 = choose_dim1 != 0 ? sg_array2 : sg_array1;
  void *new_index2 = m_ast->CopyExp(choose_dim1 != 0 ? sg_idx2[0] : sg_idx1[0]);
  void *new_index2_offset = m_ast->CreateConst(0);
  for (size_t i = 0; i < choose_dim.size() - 1 - leftmost_dim; ++i) {
    new_pntr1 = m_ast->CreateExp(
        V_SgPntrArrRefExp, new_pntr1,
        m_ast->CopyExp(choose_dim1 != 0 ? sg_idx1[i] : sg_idx2[i]));
  }
  for (size_t j = leftmost_dim + 1; j > 0; --j) {
    new_pntr1 = m_ast->CreateExp(V_SgPntrArrRefExp, new_pntr1,
                                 m_ast->CreateVariableRef(loop_iters[j - 1]));
    new_index2_offset = m_ast->CreateExp(
        V_SgMultiplyOp, new_index2_offset,
        m_ast->CreateConst(static_cast<int>(choose_dim[j - 1])));
    new_index2_offset =
        m_ast->CreateExp(V_SgAddOp, new_index2_offset,
                         m_ast->CreateVariableRef(loop_iters[j - 1]));
  }
  new_index2 = m_ast->CreateExp(V_SgAddOp, new_index2, new_index2_offset);
  new_pntr2 = m_ast->CreateExp(V_SgPntrArrRefExp, new_pntr2, new_index2);
  void *assign_stmt = m_ast->CreateStmt(
      V_SgAssignStatement, choose_dim1 != 0 ? new_pntr1 : new_pntr2,
      choose_dim1 != 0 ? new_pntr2 : new_pntr1);
  void *body = m_ast->CreateBasicBlock();
  m_ast->AppendChild(body, assign_stmt);
  void *outer_loop = nullptr;
  int loop_level = loop_iters.size();
  vector<void *> vec_loop_body;
  for (int i = 0; i != loop_level; ++i) {
    void *iter = loop_iters[reorder[i]];
    void *upper = loop_uppers[reorder[i]];
    void *new_loop = m_ast->CreateStmt(
        V_SgForStatement, m_ast->GetVariableInitializedName(iter),
        m_ast->CreateConst(0), upper, body, nullptr, nullptr, func_call);
    vec_loop_body.push_back(body);
    if (static_cast<size_t>(i) + 1 != loop_iters.size()) {
      body = m_ast->CreateBasicBlock();
      m_ast->AppendChild(body, new_loop);
    }
    outer_loop = new_loop;
  }
  setQoRMergeMode(m_ast, outer_loop, "major");

  //  apply parallel in the inner loops to achieve memory coalescing
  unsigned int unroll_factor = info.opt_bitwidth / info.bitwidth;
  unsigned int real_unroll_factor = 1;
  unsigned int curr_unroll_factor = 0;
  for (int i = 0; i < loop_level; ++i) {
    size_t curr_loop_length = loop_length[reorder[i]];
    string str_parallel_pragma =
        std::string(ACCEL_PRAGMA) + " " + CMOST_PARALLEL + " " + CMOST_SKIP;
    if (static_cast<size_t>(unroll_factor) >= curr_loop_length) {
      str_parallel_pragma += std::string(" ") + CMOST_complete;
      curr_unroll_factor = curr_loop_length;
    } else {
      //  Bug1531: using the biggest factor of curr_loop_length
      //  MER306: complete tiling the small loops
      curr_unroll_factor = unroll_factor;
      if ((curr_loop_length % unroll_factor) != 0U) {
        curr_unroll_factor = find_max_factor(
            curr_loop_length, m_unroll_threshold / real_unroll_factor);
      }
      if (curr_unroll_factor >= curr_loop_length) {
        str_parallel_pragma += std::string(" ") + CMOST_complete;
      } else {
        if (curr_unroll_factor > 1) {
          str_parallel_pragma += std::string(" ") + CMOST_factor + "=" +
                                 my_itoa(curr_unroll_factor);
        } else {
          continue;
        }
      }
    }
    void *loop_body = vec_loop_body[i];
    void *parallel_pragma = m_ast->CreatePragma(str_parallel_pragma, loop_body);
    m_ast->PrependChild(loop_body, parallel_pragma);
    if (curr_unroll_factor == 0)
      break;
    real_unroll_factor *= curr_unroll_factor;
    unroll_factor =
        (unroll_factor + curr_unroll_factor - 1) / curr_unroll_factor;
  }

  void *parentStmt = m_ast->TraceUpByTypeCompatible(func_call, V_SgStatement);
  m_ast->ReplaceStmt(parentStmt, outer_loop);
  void *outer_loop_body = m_ast->GetLoopBody(outer_loop);
  if (outer_loop_body != nullptr) {
    //  Bug2053
    void *coalescing_bitwidth_pragma = m_ast->CreatePragma(
        std::string(HLS_PRAGMA) + " " + CMOST_coalescing_width + "=" +
            my_itoa(info.bitwidth * real_unroll_factor),
        outer_loop_body);
    m_ast->PrependChild(outer_loop_body, coalescing_bitwidth_pragma);
  }
  if (real_unroll_factor > 1) {
    map<string, string> msg_map;
    msg_map["memory_coalescing"] = "on";
    msg_map["memory_coalescing_bitwidth"] = my_itoa(std::min(
        m_max_bitwidth, static_cast<int>(info.bitwidth * real_unroll_factor)));
    insertMessage(m_ast, outer_loop, msg_map);
  }
  m_count_memcpy++;

  return 1;
}

int MemoryCoalescingIntel::handle_single_assignment(
    void *sg_array1, const vector<void *> &sg_idx1, const vector<size_t> &dim1,
    void *sg_array2, const vector<void *> &sg_idx2, const vector<size_t> &dim2,
    void *sg_length, int type_size, void *func_call) {
  CMarsExpression length(sg_length, m_ast, func_call);
  if (length.IsConstant() == 0) {
    return 0;
  }
  size_t c_length = length.GetConstant() / type_size;
  if (c_length != 1) {
    return 0;
  }
  void *pntr1 = m_ast->CreateVariableRef(sg_array1);
  for (auto sg_idx : sg_idx1) {
    pntr1 = m_ast->CreateExp(V_SgPntrArrRefExp, pntr1, m_ast->CopyExp(sg_idx));
  }

  void *pntr2 = m_ast->CreateVariableRef(sg_array2);
  for (auto sg_idx : sg_idx2) {
    pntr2 = m_ast->CreateExp(V_SgPntrArrRefExp, pntr2, m_ast->CopyExp(sg_idx));
  }

  void *single_assign_expr = m_ast->CreateExp(V_SgAssignOp, pntr1, pntr2);
  m_ast->ReplaceExp(func_call, single_assign_expr);
  m_count_memcpy++;
  return 1;
}

int MemoryCoalescingIntel::memcpy_replace(const MemcpyInfo &info,
                                          void *func_call) {
  void *func_decl = m_ast->TraceUpToFuncDecl(func_call);
  void *parentStmt = m_ast->TraceUpByTypeCompatible(func_call, V_SgStatement);
  void *sg_array1 = info.port[0].array;

  vector<void *> sg_idx1 = info.port[0].index;
  void *sg_array2 = info.port[1].array;

  vector<void *> sg_idx2 = info.port[1].index;
  void *sg_length = info.length_exp;

  void *array1_type_var = m_ast->GetTypebyVar(sg_array1);
  int type_size =
      m_ast->get_type_unit_size(m_ast->RemoveCast(array1_type_var), sg_array1);
  void *array1_basic_type = nullptr;
  vector<size_t> array1_basic_size;
  m_ast->get_type_dimension(array1_type_var, &array1_basic_type,
                            &array1_basic_size, sg_array1);
  void *array2_type_var = m_ast->GetTypebyVar(sg_array2);

  void *array2_basic_type = nullptr;
  vector<size_t> array2_basic_size;
  m_ast->get_type_dimension(array2_type_var, &array2_basic_type,
                            &array2_basic_size, sg_array2);
  if ((array1_basic_type == nullptr) ||
      m_ast->GetBaseType(array1_basic_type, true) !=
          m_ast->GetBaseType(array2_basic_type, true)) {
    //  FIXME: support general memcpy
#if PROJDEBUG
    cout << "cannot inline memcpy " << m_ast->_p(func_call)
         << " because of different types" << endl;
#endif
    string info = "    Memcpy: " + m_ast->_up(func_call) + " " +
                  getUserCodeFileLocation(m_ast, func_call, true) + "\n";
    info +=
        "    In function: " + m_ast->_up(m_ast->TraceUpToFuncDecl(func_call)) +
        "\n";
    dump_critical_message(FUCIN_ERROR_5(info), 1);
    m_valid = false;
    return 0;
  }

  if (handle_single_assignment(sg_array1, sg_idx1, array1_basic_size, sg_array2,
                               sg_idx2, array2_basic_size, sg_length, type_size,
                               func_call) != 0) {
    return 1;
  }

  if (generate_nested_loops(sg_array1, sg_idx1, array1_basic_size, sg_array2,
                            sg_idx2, array2_basic_size, sg_length, type_size,
                            func_call, info) != 0) {
    return 1;
  }

  /*********************************************************************/
  /**********************************create for
   * loop*************************************/
  /*********************************************************************/
  string for_index = "__memcpy_i_" + my_itoa(m_count_memcpy);
  m_ast->get_valid_local_name(func_decl, &for_index);
  void *sg_index_i = m_ast->CreateVariableDecl("long", for_index, nullptr,
                                               m_ast->GetFuncBody(func_decl));
  m_ast->InsertStmt(sg_index_i, parentStmt);
  void *body = m_ast->CreateBasicBlock();
  void *index_name = m_ast->GetVariableInitializedName(sg_index_i);

  //  generate leftside of "="
  vector<void *> array1_index;
  void *sg_total_offset1 = m_ast->CreateConst(0);
  int dim1 = array1_basic_size.size();
  int opt_dim1 = dim1;
  if (opt_dim1 > 1) {
    int curr_dim = 0;
    CMarsExpression me_total_offset =
        (CMarsExpression(sg_length, m_ast, func_call) / type_size);
    int64_t curr_size = 1;
    while (curr_dim < dim1) {
      void *curr_index = sg_idx1[dim1 - 1 - curr_dim];
      CMarsExpression me_curr_offset =
          CMarsExpression(curr_index, m_ast, func_call) * curr_size;
      me_total_offset = me_total_offset + me_curr_offset;
      CMarsRangeExpr mr_total_offset = me_total_offset.get_range();
      int64_t c_ub = mr_total_offset.get_const_ub();
      curr_size *= array1_basic_size[curr_dim];
      if (c_ub <= curr_size) {
        opt_dim1 = curr_dim + 1;
        break;
      }
      curr_dim++;
    }
  }
  for (int i = dim1 - opt_dim1; i < dim1; i++) {
    sg_total_offset1 = m_ast->CreateExp(
        V_SgMultiplyOp, sg_total_offset1,
        m_ast->CreateConst(static_cast<int>(array1_basic_size[dim1 - 1 - i])));
    sg_total_offset1 = m_ast->CreateExp(V_SgAddOp, sg_total_offset1,
                                        m_ast->CopyExp(sg_idx1[i]));
  }
  string s_total_offset1 = "total_offset1";
  void *sg_total_offset_decl1 = m_ast->CreateVariableDecl(
      "long", s_total_offset1, sg_total_offset1, body);
  m_ast->AppendChild(body, sg_total_offset_decl1);
  for (int i = 0; i < dim1; i++) {
    if (i >= dim1 - opt_dim1) {
      array1_index.push_back(m_ast->CreateExp(
          V_SgAddOp, m_ast->CreateVariableRef(sg_total_offset_decl1),
          m_ast->CreateVariableRef(sg_index_i)));
    } else {
      array1_index.push_back(m_ast->CopyExp(sg_idx1[i]));
    }
  }

  if (opt_dim1 > 1) {
    for (int i = dim1 - opt_dim1; i < dim1; i++) {
      for (int j = 0; j < dim1 - i; j++) {
        if (0 == array1_basic_size[j]) {
          continue;
        }
        if (j == dim1 - i - 1) {
          if (i > dim1 - opt_dim1) {
            array1_index[i] = m_ast->CreateExp(
                V_SgModOp, array1_index[i],
                m_ast->CreateConst(static_cast<int>(array1_basic_size[j])));
          }
        } else {
          array1_index[i] = m_ast->CreateExp(
              V_SgDivideOp, array1_index[i],
              m_ast->CreateConst(static_cast<int>(array1_basic_size[j])));
        }
      }
    }
  }
  void *array1_ref =
      m_ast->CreateArrayRef(m_ast->CreateVariableRef(sg_array1), array1_index);

  //  generate rightside of "="

  vector<void *> array2_index;
  void *sg_total_offset2 = m_ast->CreateConst(0);
  int dim2 = array2_basic_size.size();
  int opt_dim2 = dim2;

  if (opt_dim2 > 1) {
    int curr_dim = 0;
    CMarsExpression me_total_offset =
        (CMarsExpression(sg_length, m_ast, func_call) / type_size);
    int64_t curr_size = 1;
    while (curr_dim < dim2) {
      void *curr_index = sg_idx2[dim2 - 1 - curr_dim];
      CMarsExpression me_curr_offset =
          CMarsExpression(curr_index, m_ast, func_call) * curr_size;
      me_total_offset = me_total_offset + me_curr_offset;
      CMarsRangeExpr mr_total_offset = me_total_offset.get_range();
      int64_t c_ub = mr_total_offset.get_const_ub();
      curr_size *= array2_basic_size[curr_dim];
      if (c_ub <= curr_size) {
        opt_dim2 = curr_dim + 1;
        break;
      }
      curr_dim++;
    }
  }
  for (int i = dim2 - opt_dim2; i < dim2; i++) {
    sg_total_offset2 = m_ast->CreateExp(
        V_SgMultiplyOp, sg_total_offset2,
        m_ast->CreateConst(static_cast<int>(array2_basic_size[dim2 - i - 1])));
    sg_total_offset2 = m_ast->CreateExp(V_SgAddOp, sg_total_offset2,
                                        m_ast->CopyExp(sg_idx2[i]));
  }
  string s_total_offset2 = "total_offset2";
  void *sg_total_offset_decl2 = m_ast->CreateVariableDecl(
      "long", s_total_offset2, sg_total_offset2, body);
  m_ast->AppendChild(body, sg_total_offset_decl2);
  for (int i = 0; i < dim2; i++) {
    if (i >= dim2 - info.opt_dimension) {
      array2_index.push_back(m_ast->CreateExp(
          V_SgAddOp, m_ast->CreateVariableRef(sg_total_offset_decl2),
          m_ast->CreateVariableRef(sg_index_i)));
    } else {
      array2_index.push_back(m_ast->CopyExp(sg_idx2[i]));
    }
  }

  if (opt_dim2 > 1) {
    for (int i = dim2 - opt_dim2; i < dim2; i++) {
      for (int j = 0; j < dim2 - i; j++) {
        if (0 == array2_basic_size[j]) {
          continue;
        }
        if (j == dim2 - i - 1) {
          if (i > dim2 - opt_dim2) {
            array2_index[i] = m_ast->CreateExp(
                V_SgModOp, array2_index[i],
                m_ast->CreateConst(static_cast<int>(array2_basic_size[j])));
          }
        } else {
          array2_index[i] = m_ast->CreateExp(
              V_SgDivideOp, array2_index[i],
              m_ast->CreateConst(static_cast<int>(array2_basic_size[j])));
        }
      }
    }
  }

  void *array2_ref =
      m_ast->CreateArrayRef(m_ast->CreateVariableRef(sg_array2), array2_index);

  //  generate "=" assignment expression
  void *transfer_exp = m_ast->CreateExp(V_SgAssignOp, array1_ref, array2_ref);
  string string_transfer_exp = m_ast->UnparseToString(transfer_exp);

  //  generate for loop and replace old memcpy function call
  void *sg_length_c = m_ast->CreateExp(V_SgDivideOp, m_ast->CopyExp(sg_length),
                                       m_ast->CreateConst(type_size));
  void *for_body_stmt = m_ast->CreateStmt(V_SgExprStatement, transfer_exp);
  m_ast->AppendChild(body, for_body_stmt);
  void *for_loop_stmt =
      m_ast->CreateStmt(V_SgForStatement, index_name, m_ast->CreateConst(0),
                        sg_length_c, body, nullptr, nullptr, func_call);

  //  apply parallel in the inner loops to achieve memory coalescing
  unsigned int unroll_factor = info.opt_bitwidth / info.bitwidth;
  CMarsExpression me(sg_length, m_ast, func_call);
  bool enable_unroll = true;
  if (me.IsConstant() != 0) {
    size_t c_len = me.GetConstant() / type_size;
    //  Bug1531: using the biggest factor of curr_loop_length
    string str_parallel_pragma = std::string(ACCEL_PRAGMA) + " " +
                                 CMOST_PARALLEL + " " + CMOST_SKIP + " ";
    if ((c_len % unroll_factor) != 0U) {
      //  MER306: complete tiling the small loops
      unroll_factor = find_max_factor(c_len, m_unroll_threshold);
    }
    if (unroll_factor >= c_len || unroll_factor > 1) {
      if (unroll_factor >= c_len) {
        str_parallel_pragma += CMOST_complete;
      } else if (unroll_factor > 1) {
        str_parallel_pragma +=
            std::string(CMOST_factor) + "=" + my_itoa(unroll_factor);
      }
      void *parallel_pragma = m_ast->CreatePragma(str_parallel_pragma, body);
      m_ast->PrependChild(body, parallel_pragma);
    } else {
      enable_unroll = false;
    }
  } else {
    int64_t total_dim1 = 1;
    for (int i = 0; i < opt_dim1; ++i) {
      total_dim1 *= array1_basic_size[i];
    }
    int64_t total_dim2 = 1;
    for (int i = 0; i < opt_dim2; ++i) {
      total_dim2 *= array2_basic_size[i];
    }
    int64_t upper = max(total_dim1, total_dim2);
    upper = min(upper, (me / type_size).get_range().get_const_ub());
    const string str_tripcount_pragma = std::string(HLS_PRAGMA) + " " +
                                        HLS_TRIPCOUNT + " " + HLS_MAX + "=" +
                                        my_itoa(upper);
    void *trip_pragma = m_ast->CreatePragma(str_tripcount_pragma, body);
    m_ast->PrependChild(body, trip_pragma);
    if (unroll_factor > 1) {
      string str_parallel_pragma = std::string(ACCEL_PRAGMA) + " " +
                                   CMOST_PARALLEL + " " + CMOST_SKIP + " ";
      str_parallel_pragma +=
          std::string(CMOST_factor) + "=" + my_itoa(unroll_factor);
      void *parallel_pragma = m_ast->CreatePragma(str_parallel_pragma, body);
      m_ast->PrependChild(body, parallel_pragma);
    }
  }
  m_ast->ReplaceStmt(parentStmt, for_loop_stmt);
  m_count_memcpy++;
  if (enable_unroll) {
    map<string, string> msg_map;
    msg_map["memory_coalescing"] = "on";
    msg_map["memory_coalescing_bitwidth"] = my_itoa(std::min(
        m_max_bitwidth, static_cast<int>(info.bitwidth * unroll_factor)));
    insertMessage(m_ast, for_loop_stmt, msg_map);
  }
  return 1;
}
