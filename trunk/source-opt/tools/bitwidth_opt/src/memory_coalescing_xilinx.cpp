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


#include "memory_coalescing_xilinx.h"
#include <iostream>
#include <string>
#include <boost/algorithm/string.hpp>
//  #include "rose.h"
#include "mars_message.h"
#include "mars_opt.h"
#include "program_analysis.h"
#include "memory_burst.h"

using MarsProgramAnalysis::CMarsAccess;
using MarsProgramAnalysis::CMarsExpression;
using MarsProgramAnalysis::CMarsRangeExpr;
#ifdef DEBUG
#undef DEBUG
#endif
#define DISABLE_DEBUG 0
#if DISABLE_DEBUG
#define DEBUG(stmts)
#else
#define DEBUG(stmts)                                                           \
  do {                                                                         \
    stmts;                                                                     \
  } while (false)
#endif
#define DEFAULT_BUS_BITWIDTH 512
#define MAX_PARALLEL_FACTOR 16
#define CLASS_PREFIX "class "
bool MemoryCoalescingXilinx::checkInvalidBitWidth(int bitwidth,
                                                  int orig_bitwidth) {
  if (bitwidth != 32 && bitwidth != 16 && bitwidth != 64 && bitwidth != 128 &&
      bitwidth != 256 && bitwidth != 512) {
    return true;
  }
  if ((bitwidth % orig_bitwidth) != 0) {
    return true;
  }
  return false;
}

bool MemoryCoalescingXilinx::run() {
  if (mNaiveHLS) {
    DEBUG(cout << "naive flow\n");
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
  std::ofstream header_list("merlin_coalescing_header_file.list");
  for (auto header_file : mCoalescingHeader) {
    header_list << header_file << "\n";
  }
  std::cout << "============================================" << std::endl;
  std::cout << "-----=# wide bus Interface Optimization End#=----\n";
  std::cout << "============================================" << std::endl
            << std::endl;

  return Changed;
}

void MemoryCoalescingXilinx::AddMemCpyHeader(void *scope) {
  //  include the large bit width interface implementation
  void *file_global = m_ast->GetGlobal(scope);
  if (mGlobals_with_memcpy_header.count(file_global) > 0) {
    return;
  }
  mGlobals_with_memcpy_header.insert(file_global);
  for (auto bitwidth : mBitWidthSet) {
    std::ostringstream oss;
    oss << bitwidth;
    string header_file = "memcpy_" + oss.str() + "_1d.h";
    mCoalescingHeader.insert(header_file);
    mCoalescingHeader.insert("mars_wide_bus.h");
    mCoalescingHeader.insert("memcpy_" + oss.str() + ".h");
    string memcpy_header = "\n#include \"" + header_file + "\"\n";

    m_ast->AddHeader(memcpy_header, file_global);
  }
}
void MemoryCoalescingXilinx::AddDataTypeHeader(void *scope) {
  //  include the large bit width interface implementation
  void *file_global = m_ast->GetGlobal(scope);
  if (mGlobals_with_data_type_header.count(file_global) > 0) {
    return;
  }
  mGlobals_with_data_type_header.insert(file_global);
  for (auto bitwidth : mBitWidthSet) {
    std::ostringstream oss;
    oss << bitwidth;

    string header_file = "memcpy_" + oss.str() + ".h";
    mCoalescingHeader.insert(header_file);
    string memcpy_header = "\n#include \"" + header_file + "\"\n";

    m_ast->AddHeader(memcpy_header, file_global);
  }
}

void MemoryCoalescingXilinx::init(CSageCodeGen *Codegen) {
  if (!mOptions.get_option_key_value("-a", "naive_hls").empty()) {
    mNaiveHLS = true;
    cout << "[MARS-REDUCTION-MSG] Naive HLS mode.\n";
    return;
  }

  mEnableCDesign = true;
  if (!mOptions.get_option_key_value("-a", "aggressive_opt_for_cpp").empty()) {
    mEnableSubOptimal = true;
  }
  string bitwidth_str = mOptions.get_option_key_value("-a", "bus_bitwidth");
  if (!bitwidth_str.empty()) {
    mBitWidth = my_atoi(bitwidth_str);
    mBitWidthSpecified = true;
  }
  string len_threshold_str =
      mOptions.get_option_key_value("-a", "bus_length_threshold");
  if (!len_threshold_str.empty()) {
    m_length_threshold = my_atoi(len_threshold_str);
  }
  mMars_ir.get_mars_ir(Codegen, mTopFunc, mOptions, true);
  mMars_ir_v2.build_mars_ir(Codegen, mTopFunc);
}

bool MemoryCoalescingXilinx::update_interface(CMirNode *fNode) {
  void *top_decl = m_ast->GetFuncDeclByDefinition(m_ast->GetParent(fNode->ref));
  assert(top_decl);
  vector<void *> func_decls;
  mFunc_defs.clear();
  mCpp_design = m_ast->isWithInCppFile(fNode->ref);

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
        string func_info = m_ast->_up(decl);
        dump_critical_message(WDBUS_WARNING_7(func_info));
        return false;
      }
      //  FIXME: all the functions with definition should be
      //  in Cpp files or C files
      if (m_ast->isWithInCppFile(def) != mCpp_design) {
        dump_critical_message(WDBUS_WARNING_8);
        return false;
      }
      mFunc_defs.push_back(def);
    }
  }
  collectInterfaceInfo();
  bool Changed = false;
  Changed |= updateKernelFunction();
  Changed |= updateFunctionInterface();
  return Changed;
}

void MemoryCoalescingXilinx::auto_cache_for_coalescing() {
  MemoryBurst instance(m_ast, mTopFunc, mOptions, false);

  for (auto func_def : mFunc_defs) {
    SgFunctionDeclaration *func_decl =
        isSgFunctionDefinition(static_cast<SgNode *>(func_def))
            ->get_declaration();
    void *unique_decl = m_ast->GetUniqueFuncDecl(func_decl);
    if (unique_decl == nullptr) {
      continue;
    }
    if (mMars_ir.is_kernel(m_ast, func_decl)) {
      instance.set_top_kernel(func_decl);
    }
    assert(mFunc2Info.count(unique_decl) > 0);
    const vector<ArgumentInfo> &arg_info = mFunc2Info[unique_decl];
    int arg_num = m_ast->GetFuncParamNum(func_decl);
    for (int i = 0; i < arg_num; ++i) {
      instance.set_arg_opt_bitwidth(m_ast->GetFuncParam(func_decl, i),
                                    arg_info[i].specified_bitwidth
                                        ? arg_info[i].opt_bitwidth
                                        : arg_info[i].bitwidth << 1);
    }
  }
  instance.auto_cache_for_coalescing();
}

void MemoryCoalescingXilinx::collectInterfaceInfo() {
  for (size_t i = 0; i != mFunc_defs.size(); ++i) {
    InitInterfaceInfo(mFunc_defs[i]);
  }

  auto_cache_for_coalescing();

  for (size_t i = mFunc_defs.size(); i > 0; --i) {
    void *func_def = mFunc_defs[i - 1];
    backwardInterfaceInfo(func_def);
  }

  for (size_t i = 0; i != mFunc_defs.size(); ++i) {
    void *func_def = mFunc_defs[i];
    forwardInterfaceInfo(func_def);
  }

  AdjustInterfaceInfo();
}

void MemoryCoalescingXilinx::InitInterfaceInfo(void *func_def) {
  SgFunctionDeclaration *func_decl =
      isSgFunctionDefinition(static_cast<SgNode *>(func_def))
          ->get_declaration();
  int param_num = m_ast->GetFuncParamNum(func_decl);
  void *unique_decl = m_ast->GetUniqueFuncDecl(func_decl);
  if (unique_decl == nullptr) {
    return;
  }
  map<void *, int> paralists;
  for (int i = 0; i != param_num; ++i) {
    void *arg = m_ast->GetFuncParam(func_decl, i);
    if (mMars_ir.every_trace_is_bus(m_ast, func_decl, arg)) {
      paralists[arg] = i;
    }
  }
  void *func_body = m_ast->GetFuncBody(func_decl);
  bool kernel = mMars_ir.is_kernel(m_ast, func_decl);
  if (kernel) {
    mFunc2Info[unique_decl].resize(param_num);
    vector<ArgumentInfo> &arg_info = mFunc2Info[unique_decl];
    vector<void *> vec_pragma;
    m_ast->GetNodesByType(func_body, "preorder", "SgPragmaDeclaration",
                          &vec_pragma);

    for (int i = 0; i != param_num; ++i) {
      void *arg = m_ast->GetFuncParam(func_decl, i);
      map<string, string> message;
      void *type = m_ast->GetTypebyVar(arg);
      void *base_type = m_ast->GetBaseType(type, false);
      ArgumentInfo &info = arg_info[i];
      string arg_name = m_ast->GetVariableName(arg);
      auto arg_user_info = getUserCodeInfo(m_ast, arg);
      if (!arg_user_info.name.empty()) {
        arg_name = arg_user_info.name;
      }
      string var_info =
          "'" + arg_name + "' (" + arg_user_info.file_location + ")";
      info.base_type = base_type;
      info.bitwidth = m_ast->get_bitwidth_from_type(base_type);
      info.all_support_access =
          mMars_ir.every_trace_is_bus(m_ast, func_decl, arg);
      // we only support power of two bitwidth and no less than 8 now
      if (info.bitwidth < 8 || ((info.bitwidth - 1) & info.bitwidth) != 0) {
        info.all_support_access = false;
        message["memory_coalescing"] = "off";
        message["memory_coalescing_off_reason"] =
            "not power of two bitwidth or less than 8";
        insertMessage(m_ast, arg, message);
        dump_critical_message(
            WDBUS_WARNING_15(var_info, my_itoa(info.bitwidth)), 0, m_ast, arg);
        continue;
      }
      get_opt_bitwidth(&info.opt_bitwidth, &info.specified_bitwidth, arg,
                       info.bitwidth, vec_pragma, true);
    }
  }
  vector<ArgumentInfo> arg_info = mFunc2Info[unique_decl];
  vector<void *> vecCalls;
  m_ast->GetNodesByType(func_body, "preorder", "SgFunctionCallExp", &vecCalls);
  //  analyze all sub function calls
  for (size_t i = 0; i < vecCalls.size(); ++i) {
    void *func_call = vecCalls[i];
    if (isMemcpy(func_call)) {
      continue;
    }
    void *sub_func_decl = m_ast->GetFuncDeclByCall(func_call, 1);
    if (sub_func_decl == nullptr) {
      continue;
    }
    void *sub_unique_decl = m_ast->GetUniqueFuncDecl(sub_func_decl);
    if (sub_unique_decl == nullptr) {
      continue;
    }
    int sub_param_num = m_ast->GetFuncCallParamNum(func_call);
    if (mFunc2Info.count(sub_unique_decl) > 0) {
      continue;
    }
    vector<ArgumentInfo> sub_arg_info(sub_param_num);
    //  ordinary function calls
    for (int j = 0; j != sub_param_num; ++j) {
      void *arg = m_ast->GetFuncParam(sub_func_decl, j);
      if (arg == nullptr) {
        continue;
      }
      void *type = m_ast->GetTypebyVar(arg);
      void *base_type = m_ast->GetBaseType(type, false);
      ArgumentInfo &info = sub_arg_info[j];
      info.base_type = base_type;
      info.bitwidth = m_ast->get_bitwidth_from_type(base_type);
      info.all_support_access =
          mMars_ir.every_trace_is_bus(m_ast, sub_func_decl, arg);
      info.opt_bitwidth = info.bitwidth;
      // 1. we only support power of two bitwidth and no less than 8 now
      // if we wanted to support bitwidth less than 8, we would update
      // coalescing header files under
      // trunk/mars-gen/drivers/code_transform/coalescing
      // 2. we only support one dimension array/pointer interface port now
      if (m_ast->get_dim_num_from_var(arg) != 1 || info.bitwidth <= 7 ||
          (info.bitwidth & (info.bitwidth - 1)) != 0) {
        info.all_support_access = false;
      }
      if (!info.all_support_access) {
        continue;
      }
      void *array_ref = m_ast->GetFuncCallParam(func_call, j);
      m_ast->remove_cast(&array_ref);
      //  FIXME:we cannot handle partial array access now
#if 0
      if (isSgAddressOfOp(static_cast<SgNode*>(arg))) {
        void *exp = m_ast->GetExpLeftOperand(arg);
        if (isSgPntrArrRefExp(static_cast<SgNode*>(exp))) {
          m_ast->get_var_from_pntr_ref(arg, &array_ref);
        }
      }
#endif
      if (m_ast->IsVarRefExp(array_ref) != 0) {
        void *sg_name = m_ast->GetVariableInitializedName(array_ref);
        if (paralists.count(sg_name) > 0) {
          int arg_index = paralists[sg_name];
          sub_arg_info[j].opt_bitwidth = arg_info[arg_index].opt_bitwidth;
          sub_arg_info[j].specified_bitwidth =
              arg_info[arg_index].specified_bitwidth;
        }
      }
    }
    mFunc2Info[sub_unique_decl] = sub_arg_info;
  }
}

void MemoryCoalescingXilinx::parse_memcpy_info(
    void *func_call, const map<void *, int> &paralists,
    vector<ArgumentInfo> *arg_info_) {
  vector<ArgumentInfo> &arg_info = *arg_info_;
  struct MemcpyInfo info;
  int valid = m_ast->parse_memcpy(
      func_call, &info.length_exp, &info.port[0].array, &info.port[0].pntr,
      &info.port[0].index, &info.port[0].pointer, &info.port[1].array,
      &info.port[1].pntr, &info.port[1].index, &info.port[1].pointer);
  // only for debug
  // if (valid != 0) {
  //   for (int i = 0; i < 2; ++i) {
  //     MemcpyPort port_info = info.port[i];
  //     cout << port_info.print() << endl;
  //   }
  // }
  if ((valid == 0) ||
      !m_ast->is_identical_base_type(info.port[0].array, info.port[1].array)) {
    return;
  }
  if (paralists.count(info.port[0].array) > 0 &&
      paralists.count(info.port[1].array) > 0) {
    cout << "do not need to apply memory colescing for data transfer "
         << "between external memory ports" << endl;
    return;
  }
  int arg_index = -1;
  bool fined_grained_scope = mMars_ir.is_fine_grained_scope(m_ast, func_call);
  for (int j = 0; j != 2; ++j) {
    void *port = info.port[j].array;
    map<string, string> message;
    string arg_name = m_ast->GetVariableName(port);
    auto arg_user_info = getUserCodeInfo(m_ast, port);
    if (!arg_user_info.name.empty()) {
      arg_name = arg_user_info.name;
    }
    string var_info =
        "'" + arg_name + "' (" + arg_user_info.file_location + ")";
    if (paralists.count(port) > 0) {
      arg_index = paralists.find(port)->second;
      if (!arg_info[arg_index].all_support_access)
        return;
      void *basic_type;
      void *buf = info.port[1 - j].array;
      vector<size_t> DSize;
      int dimension = m_ast->get_type_dimension(m_ast->GetTypebyVar(buf),
                                                &basic_type, &DSize, func_call);
      int element_bitwidth = m_ast->get_bitwidth_from_type(basic_type);
      if (element_bitwidth == 0) {
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
      int element_bytes = (element_bitwidth + 7) / 8;
      info.read = j == 1;
      //  FIXME: currrently we can only support 1D, 2D, 3D, 4D, 5D arrays
      if (dimension > mMaxDimension) {
        string msg =
            "Memory coalescing NOT inferred: variable " + var_info + "\n";
        msg += "  Reason: the number of array dimensions is larger than 5 \n";
        msg += "  Note: high dimensional array is not supported in memory "
               "coalescing \n";

        message["memory_coalescing"] = "off";
        message["memory_coalescing_off_reason"] =
            " the number of on-chip buffer dimensions is more than " +
            my_itoa(mMaxDimension);
        insertMessage(m_ast, port, message);
        dump_critical_message(WDBUS_WARNING_4(var_info), 0, m_ast, port);

        arg_info[arg_index].all_support_access = false;
        return;
      }
      info.dimension = dimension;
      info.dimension_size = DSize;
      info.base_type = basic_type;
      int opt_bitwidth = arg_info[arg_index].opt_bitwidth;
      int opt_dimension = info.dimension;
      bool unaligned_buffer = true;
      bool unaligned_global_memory_offset = true;
      bool unaligned_length = true;
      int aligned_dimension = 0;
      CMarsExpression me_total_offset =
          (CMarsExpression(info.length_exp, m_ast) / element_bytes);
      if (me_total_offset.IsConstant() && me_total_offset.GetConstant() == 1) {
        // skip single element
        return;
      }
      arg_info[arg_index].any_burst_access = true;
      do {
        unaligned_buffer = true;
        unaligned_global_memory_offset = true;
        aligned_dimension = 0;
        arg_info[arg_index].all_support_access = true;
        // try to find the optimal bitwidth in order to achieve II=1
        int num_elements = opt_bitwidth / element_bitwidth;
        unaligned_length = divisible(me_total_offset, num_elements, false) == 0;
        if (opt_dimension > 1) {
          //  youxiang: 20170321
          //  if offset + length is less than the lowest dimension,
          //  we can use the 1-D wide bus to avoid unalignment issue
          int curr_dim = 0;
          int64_t curr_size = 1;
          while (curr_dim < dimension) {
            void *curr_index = info.port[1 - j].index[dimension - 1 - curr_dim];
            CMarsExpression me_curr_offset =
                CMarsExpression(curr_index, m_ast) * curr_size;
            me_total_offset = me_total_offset + me_curr_offset;
            CMarsRangeExpr mr_total_offset = me_total_offset.get_range();
            int64_t c_ub = mr_total_offset.get_const_ub();
            curr_size *= DSize[curr_dim];
            if (c_ub <= curr_size) {
              opt_dimension = curr_dim + 1;
              break;
            }
            curr_dim++;
          }
        }

        bool report = (opt_bitwidth >> 1) <= element_bitwidth ||
                      arg_info[arg_index].specified_bitwidth ||
                      (opt_dimension == 1 &&
                       opt_bitwidth <= info.bitwidth * MAX_PARALLEL_FACTOR);
        //  predict whether wide bus can achieve II=1 by checking
        //  1. offset is aligned
        //  2. the rightmost dimension size is aligned
        {
          int curr_dim = 0;
          int64_t total_size = 1;
          while (curr_dim < opt_dimension - 1) {
            int64_t curr_size = DSize[curr_dim];
            total_size *= curr_size;
            if (total_size < num_elements) {
              if ((num_elements % total_size) != 0) {
                break;
              }
              curr_dim++;
              continue;
            }
            if ((total_size % num_elements) == 0) {
              unaligned_buffer = false;
              aligned_dimension = curr_dim + 1;
            }
            CMarsExpression index_offset(m_ast, 0);
            while (curr_dim >= 0) {
              CMarsExpression curr_index(
                  info.port[1 - j].index[dimension - 1 - curr_dim], m_ast);
              index_offset = index_offset + curr_index;
              if (curr_dim > 0) {
                index_offset = index_offset * DSize[curr_dim - 1];
              }
              curr_dim--;
            }

            if (divisible(index_offset, num_elements, false) == 0) {
              aligned_dimension = 0;
            }
            break;
          }
          if (unaligned_buffer) {
            if ((num_elements % total_size) == 0) {
              bool aligned_left_most_dim = true;
              for (int i = 0; i < opt_dimension - 1; ++i) {
                CMarsExpression curr_index(
                    info.port[1 - j].index[dimension - 1 - i], m_ast);
                if ((curr_index != 0) != 0) {
                  aligned_left_most_dim = false;
                  break;
                }
              }
              if (aligned_left_most_dim) {
                CMarsExpression curr_index(
                    info.port[1 - j].index[dimension - opt_dimension], m_ast);
                if (divisible(curr_index * total_size, num_elements, false) ==
                    0) {
                  aligned_left_most_dim = false;
                }
              }

              if (aligned_left_most_dim) {
                aligned_dimension = opt_dimension;
                unaligned_buffer = false;
              }
            }
          }

          if (unaligned_buffer) {
            if (opt_dimension >= 2 && report) {
              string msg =
                  "Suboptimal memory coalescing: variable " + var_info + "\n";
              msg += "  Reason: the access stride is not divisible by " +
                     my_itoa(num_elements) + "\n";
              string num_elements_info = my_itoa(num_elements);
              message["memory_coalescing"] = "off";
              message["memory_coalescing_off_reason"] = "the access stride "
                                                        "is not divisible by " +
                                                        num_elements_info;
              insertMessage(m_ast, port, message);
              dump_critical_message(
                  WDBUS_WARNING_3(var_info, num_elements_info), 0, m_ast, port);
              if (!mEnableSubOptimal) {
                DEBUG(cout << "disable wide bus for port " << var_info
                           << " because of sub optimial access "
                           << m_ast->_p(m_ast->TraceUpToStatement(func_call), 0)
                           << "\n");
                arg_info[arg_index].all_support_access = false;
                return;
              }
              arg_info[arg_index].suboptimal_coalescing = true;
            }
          }
          CMarsExpression offset_exp(info.port[j].index[0], m_ast, func_call);
          if (divisible(offset_exp, num_elements, false) == 0) {
            if (fined_grained_scope) {
              arg_info[arg_index].all_support_access = false;
            } else if (report) {
              if (opt_dimension >= 2) {
                unaligned_buffer = true;
                aligned_dimension = 0;
                string msg =
                    "Suboptimal memory coalescing: variable " + var_info + "\n";
                msg += "  Reason: the access offset is not divisible by " +
                       my_itoa(num_elements) + "\n";
                string num_elements_info = my_itoa(num_elements);
                message["memory_coalescing"] = "off";
                message["memory_coalescing_off_reason"] =
                    "the access offset "
                    "is not divisible by " +
                    num_elements_info;
                insertMessage(m_ast, port, message);
                dump_critical_message(
                    WDBUS_WARNING_5(var_info, num_elements_info), 0, m_ast,
                    port);
                if (!mEnableSubOptimal) {
                  DEBUG(cout
                        << "disable wide bus for port " << var_info
                        << " because of sub optimal access "
                        << m_ast->_p(m_ast->TraceUpToStatement(func_call), 0)
                        << "\n");
                  arg_info[arg_index].all_support_access = false;
                  return;
                }
                arg_info[arg_index].suboptimal_coalescing = true;
              } else {
                if (!arg_info[arg_index].specified_bitwidth)
                  //  Youxiang 20170413 bug1166 to avoid HLS stuck, limit
                  //  parallel factor is no more than MAX_PARALLEL_FACTOR
                  opt_bitwidth = min(DEFAULT_BUS_BITWIDTH,
                                     info.bitwidth * MAX_PARALLEL_FACTOR);
                num_elements = opt_bitwidth / info.bitwidth;
                string msg = "Memory coalescing variable " + var_info;
                msg += " may cause long compilation time of downstream tool.";
                msg += "  Reason: the access offset is not divisible by " +
                       my_itoa(num_elements) + "\n";
                string num_elements_info = my_itoa(num_elements);
                message["memory_coalescing"] = "off";
                message["memory_coalescing_off_reason"] =
                    " the access offset is not divisible by " +
                    my_itoa(num_elements);
                insertMessage(m_ast, port, message);
                dump_critical_message(
                    WDBUS_WARNING_9(var_info, num_elements_info), 0, m_ast,
                    port);
              }
            }
          } else {
            unaligned_global_memory_offset = false;
          }
          if (unaligned_buffer || unaligned_global_memory_offset ||
              unaligned_length) {
            if (fined_grained_scope && aligned_dimension == 0) {
              DEBUG(cout << "disable wide bus for port " << var_info
                         << " because of unaligned access "
                         << m_ast->_p(m_ast->TraceUpToStatement(func_call), 0)
                         << "\n");
              if (report) {
                arg_info[arg_index].all_support_access = false;
                return;
              }
            }
          }
        }
        if (!unaligned_buffer || report)
          break;
        opt_bitwidth >>= 1;
        //  we will pass all the indices of buffer into wide bus api
      } while (true);
      info.unaligned_buffer |= unaligned_buffer;
      info.aligned_dimension = aligned_dimension;
      info.opt_bitwidth = opt_bitwidth;
      info.opt_dimension = opt_dimension;
      arg_info[arg_index].memcpy[func_call] = info;
      arg_info[arg_index].any_unaligned_access |=
          unaligned_buffer || unaligned_global_memory_offset ||
          unaligned_length;
      arg_info[arg_index].ac_type = (enum access_type)(
          arg_info[arg_index].ac_type | (info.read ? READ : WRITE));
      arg_info[arg_index].opt_bitwidth =
          min(arg_info[arg_index].opt_bitwidth, opt_bitwidth);
    }
  }
}

void MemoryCoalescingXilinx::backwardInterfaceInfo(void *func_def) {
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
    void *arg_type = m_ast->GetTypebyVar(arg);
    if ((m_ast->IsPointerType(arg_type) == 0) &&
        (m_ast->IsArrayType(arg_type) == 0)) {
      continue;
    }
    if (mMars_ir.every_trace_is_bus(m_ast, func_decl, arg)) {
      paralists[arg] = i;
    }
  }
  vector<void *> vecCalls;
  m_ast->GetNodesByType(func_def, "preorder", "SgFunctionCallExp", &vecCalls);
  //  analyze all sub function calls
  for (size_t i = 0; i < vecCalls.size(); ++i) {
    void *func_call = vecCalls[i];
    void *sub_func_decl = m_ast->GetFuncDeclByCall(func_call, 0);
    if (m_ast->IsMerlinInternalIntrinsic(m_ast->GetFuncName(sub_func_decl))) {
      continue;
    }
    bool is_memcpy = isMemcpy(func_call);
    if (sub_func_decl == nullptr) {
      continue;
    }
    void *sub_unique_decl = m_ast->GetUniqueFuncDecl(sub_func_decl);
    if (sub_unique_decl == nullptr) {
      continue;
    }
    int sub_param_num = m_ast->GetFuncCallParamNum(func_call);
    vector<ArgumentInfo> sub_arg_info(sub_param_num);
    if (mFunc2Info.count(sub_unique_decl) > 0) {
      sub_arg_info = mFunc2Info[sub_unique_decl];
    }
    if (is_memcpy) {
      parse_memcpy_info(func_call, paralists, &arg_info);
    } else {
      //  ordinary function calls
      for (int j = 0; j != sub_param_num; ++j) {
        void *arg = m_ast->GetFuncCallParam(func_call, j);
        void *array_ref = arg;
        m_ast->remove_cast(&array_ref);
        //  FIXME:we cannot handle partial array access now
        if (m_ast->IsVarRefExp(array_ref) != 0) {
          void *sg_name = m_ast->GetVariableInitializedName(array_ref);
          if (paralists.count(sg_name) > 0) {
            int arg_index = paralists[sg_name];
            arg_info[arg_index].any_burst_access |=
                sub_arg_info[j].any_burst_access;
            arg_info[arg_index].all_support_access &=
                sub_arg_info[j].all_support_access;
            arg_info[arg_index].suboptimal_coalescing |=
                sub_arg_info[j].suboptimal_coalescing;
            arg_info[arg_index].opt_bitwidth = min(
                sub_arg_info[j].opt_bitwidth, arg_info[arg_index].opt_bitwidth);
            arg_info[arg_index].ac_type = (enum access_type)(
                arg_info[arg_index].ac_type | sub_arg_info[j].ac_type);
            arg_info[arg_index].any_unaligned_access |=
                sub_arg_info[j].any_unaligned_access;
            CallInfo info;
            info.pos = j;
            info.ref = array_ref;
            arg_info[arg_index].calls2ref[func_call].push_back(info);
          }
        }
      }
    }
  }
  //  check whether access is in the paralleled loop with continuous access
  vector<void *> var_ref;
  m_ast->GetNodesByType(func_decl, "preorder", "SgVarRefExp", &var_ref);
  for (size_t i = 0; i != var_ref.size(); ++i) {
    void *ref = var_ref[i];
    void *var = m_ast->GetVariableInitializedName(ref);
    if (paralists.count(var) <= 0) {
      continue;
    }
    int arg_index = paralists[var];
    if (arg_info[arg_index].any_burst_access) {
      continue;
    }
    if (!arg_info[arg_index].all_support_access) {
      continue;
    }
    void *pntr_ref = nullptr;
    void *sg_array;
    vector<void *> sg_indexes;
    int pointer_dim;
    m_ast->parse_pntr_ref_by_array_ref(ref, &sg_array, &pntr_ref, &sg_indexes,
                                       &pointer_dim);
    assert(sg_array == var);

    if (m_ast->GetFuncCallParamIndex(pntr_ref) != -1) {
      continue;
    }
    detect_access_in_paralleled_loop(ref, &arg_info[arg_index]);
  }
  //  check whether all reference are supported
  m_ast->GetNodesByType(func_decl, "preorder", "SgVarRefExp", &var_ref);
  for (size_t i = 0; i != var_ref.size(); ++i) {
    void *ref = var_ref[i];
    void *var = m_ast->GetVariableInitializedName(ref);
    map<string, string> message;
    if (paralists.count(var) <= 0) {
      continue;
    }
    int arg_index = paralists[var];
    //  if (!arg_info[arg_index].any_burst_access)
    //  continue;
    if (!arg_info[arg_index].all_support_access) {
      continue;
    }
    //
    string arg_name = m_ast->GetVariableName(var);
    auto arg_user_info = getUserCodeInfo(m_ast, var);
    if (!arg_user_info.name.empty()) {
      arg_name = arg_user_info.name;
    }
    string var_info =
        "'" + arg_name + "' (" + arg_user_info.file_location + ")";
    //  support mix access, i.e. burst acces and single access
    void *pntr_ref = nullptr;
    void *sg_array;
    vector<void *> sg_indexes;
    int pointer_dim;
    m_ast->parse_pntr_ref_by_array_ref(ref, &sg_array, &pntr_ref, &sg_indexes,
                                       &pointer_dim);
    assert(sg_array == var);

    if (m_ast->GetFuncCallParamIndex(pntr_ref) != -1) {
      void *func_call = m_ast->TraceUpToFuncCall(pntr_ref);
      if (m_ast->IsMemCpy(func_call)) {
        void *sg_length;
        void *sg_array1;
        void *sg_array2;
        void *sg_pntr1;
        void *sg_pntr2;
        vector<void *> index1;
        vector<void *> index2;
        int pointer_dim1;
        int pointer_dim2;
        int valid = m_ast->parse_memcpy(
            func_call, &sg_length, &sg_array1, &sg_pntr1, &index1,
            &pointer_dim1, &sg_array2, &sg_pntr2, &index2, &pointer_dim2);
        if ((valid != 0) &&
            m_ast->is_identical_base_type(sg_array1, sg_array2)) {
          continue;
        }
      } else {
        //  check whether there is offset
        if (pointer_dim == 0) {
          bool has_offset = false;
          for (auto index : sg_indexes) {
            CMarsExpression me_index(index, m_ast, func_call);
            if ((me_index != 0) != 0) {
              has_offset = true;
              break;
            }
          }
          if (!has_offset) {
            continue;
          }
        }
      }
    }

    sg_indexes.clear();
    void *sg_base_type;
    int standarded_pntr = m_ast->parse_standard_pntr_ref_from_array_ref(
        ref, &sg_array, &pntr_ref, &sg_base_type, &sg_indexes, &pointer_dim);
    if ((standarded_pntr == 0) || pointer_dim != 1) {
      string ref_info =
          m_ast->UnparseToString(pntr_ref != nullptr ? pntr_ref : ref)
              .substr(0, 20) +
          " " + getUserCodeFileLocation(m_ast, ref, true);

      if (arg_info[arg_index].any_burst_access) {
        string msg =
            "Memory coalescing NOT inferred: variable " + var_info + "\n";
        msg += "  Reason: unsupported reference " +
               m_ast->UnparseToString(pntr_ref != nullptr ? pntr_ref : ref)
                   .substr(0, 20) +
               " " + getUserCodeFileLocation(m_ast, ref, true) + "\n";
        msg += "  Hint: Please use simple assignment or memcpy if possible\n";

        message["memory_coalescing"] = "off";
        message["memory_coalescing_off_reason"] =
            "unsupported reference " + ref_info;
        insertMessage(m_ast, var, message);
        dump_critical_message(WDBUS_WARNING_2(var_info, ref_info), 0, m_ast,
                              var);
      }
      DEBUG(cout << "disable wide bus for port " << var_info
                 << " because of unsupported access " << ref_info << "\n");
      arg_info[arg_index].all_support_access = false;
      continue;
    }
    SingleAssignInfo info;
    info.arg = var;
    info.index = sg_indexes;
    info.opt_bitwidth = arg_info[arg_index].opt_bitwidth;
    info.bitwidth = arg_info[arg_index].bitwidth;
    void *expr = pntr_ref;
    if (m_ast->has_write_conservative(pntr_ref) != 0) {
      void *single_assign = m_ast->GetParent(pntr_ref);
      if (isUnsupportedSingleAssign(single_assign)) {
        string ref_info =
            m_ast->UnparseToString(pntr_ref != nullptr ? pntr_ref : ref)
                .substr(0, 20) +
            " " + getUserCodeFileLocation(m_ast, ref, true);
        if (arg_info[arg_index].any_burst_access) {
          string msg =
              "Memory coalescing NOT inferred: variable " + var_info + "\n";
          msg += "  Reason: unsupported reference " +
                 m_ast->UnparseToString(pntr_ref != nullptr ? pntr_ref : ref)
                     .substr(0, 20) +
                 " " + getUserCodeFileLocation(m_ast, ref, true) + "\n";
          msg += "  Hint: Please use simple assignment or memcpy if possible\n";
          message["memory_coalescing"] = "off";
          message["memory_coalescing_off_reason"] =
              "unsupported reference " + ref_info;
          insertMessage(m_ast, var, message);
          dump_critical_message(WDBUS_WARNING_2(var_info, ref_info), 0, m_ast,
                                var);
        }
        DEBUG(cout << "disable wide bus for port " << var_info
                   << " because of unsupported single access " << ref_info
                   << "\n");
        arg_info[arg_index].all_support_access = false;
        continue;
      }
      info.write = true;
      //  expr = single_assign;
    }
    info.read = (m_ast->has_read_conservative(pntr_ref) != 0);
    arg_info[arg_index].single_assign[expr] = info;
    if (info.read)
      arg_info[arg_index].ac_type =
          (enum access_type)(arg_info[arg_index].ac_type | READ);
    if (info.write)
      arg_info[arg_index].ac_type =
          (enum access_type)(arg_info[arg_index].ac_type | WRITE);
    arg_info[arg_index].any_unaligned_access = true;
  }
  // MER-2264 check whether there is write-only access but with unaligend
  // accesss in the dataflow process. If so, we cannot infer wide bus which
  // will bring read access
  bool is_dataflow_process = m_ast->IsDataflowProcess(func_decl);
  if (is_dataflow_process) {
    for (size_t arg_index = 0; arg_index != param_num; ++arg_index) {
      if (!arg_info[arg_index].any_burst_access ||
          !arg_info[arg_index].all_support_access ||
          arg_info[arg_index].ac_type != WRITE ||
          !arg_info[arg_index].any_unaligned_access)
        continue;
      void *var = m_ast->GetFuncParam(func_decl, arg_index);
      string arg_name = m_ast->GetVariableName(var);
      string var_info =
          "'" + arg_name + "' " + getUserCodeFileLocation(m_ast, var, true);
      string func_info = "'" + m_ast->GetFuncName(func_decl) + "' " +
                         getUserCodeFileLocation(m_ast, func_decl, true);
      string msg =
          "Memory coalescing NOT inferred: variable " + var_info + "\n";
      msg += "  Reason: unaligned write only access in dataflow process\n";
      msg += "  Hint: Please align access if possible\n";
      map<string, string> message;
      message["memory_coalescing"] = "off";
      message["memory_coalescing_off_reason"] =
          "unaligned write only access in dataflow process";
      insertMessage(m_ast, var, message);
      dump_critical_message(WDBUS_WARNING_16(var_info, func_info), 0, m_ast,
                            var);
      DEBUG(
          cout << "disable wide bus for port " << var_info
               << " because of unaligned write-only access in dataflow process "
               << func_info << "\n");
      arg_info[arg_index].all_support_access = false;
    }
  }
}

void MemoryCoalescingXilinx::forwardInterfaceInfo(void *func_def) {
  SgFunctionDeclaration *func_decl =
      isSgFunctionDefinition(static_cast<SgNode *>(func_def))
          ->get_declaration();
  void *unique_decl = m_ast->GetUniqueFuncDecl(func_decl);
  if ((unique_decl == nullptr) || mFunc2Info.count(unique_decl) <= 0) {
    return;
  }

  bool kernel = mMars_ir.is_kernel(m_ast, func_decl);
  if (kernel) {
    vector<ArgumentInfo> &arg_info = mFunc2Info[unique_decl];
    for (size_t i = 0; i != arg_info.size(); ++i) {
      arg_info[i].need_changed =
          ((static_cast<int>(arg_info[i].any_burst_access) &
            static_cast<int>(arg_info[i].all_support_access) &
            static_cast<int>(arg_info[i].opt_bitwidth >
                             arg_info[i].bitwidth)) != 0);
    }
  }
  const vector<ArgumentInfo> &arg_info = mFunc2Info[unique_decl];
  int param_num = m_ast->GetFuncParamNum(func_decl);
  for (int i = 0; i != param_num; ++i) {
    if (!arg_info[i].need_changed) {
      continue;
    }
    for (map<void *, vector<CallInfo>>::const_iterator ii =
             arg_info[i].calls2ref.begin();
         ii != arg_info[i].calls2ref.end(); ++ii) {
      void *func_call = ii->first;
      void *sub_func_decl = m_ast->GetFuncDeclByCall(func_call);
      if (sub_func_decl == nullptr) {
        continue;
      }
      void *sub_unique_decl = m_ast->GetUniqueFuncDecl(sub_func_decl);
      if ((sub_unique_decl == nullptr) ||
          mFunc2Info.count(sub_unique_decl) <= 0) {
        continue;
      }
      vector<ArgumentInfo> &sub_arg_info = mFunc2Info[sub_unique_decl];
      const vector<CallInfo> &info = ii->second;
      for (size_t j = 0; j != info.size(); ++j) {
        sub_arg_info[info[j].pos].need_changed |= arg_info[i].need_changed;
      }
    }
  }
}

void MemoryCoalescingXilinx::AdjustInterfaceInfo() {
  bool LocalChanged;
  do {
    LocalChanged = false;
    for (auto func_def : mFunc_defs) {
      SgFunctionDeclaration *func_decl =
          isSgFunctionDefinition(static_cast<SgNode *>(func_def))
              ->get_declaration();
      void *unique_decl = m_ast->GetUniqueFuncDecl(func_decl);
      if ((unique_decl == nullptr) || mFunc2Info.count(unique_decl) <= 0) {
        continue;
      }
      vector<ArgumentInfo> &arg_info = mFunc2Info[unique_decl];
      int param_num = m_ast->GetFuncParamNum(func_decl);
      for (int i = 0; i != param_num; ++i) {
        for (map<void *, vector<CallInfo>>::const_iterator ii =
                 arg_info[i].calls2ref.begin();
             ii != arg_info[i].calls2ref.end(); ++ii) {
          void *func_call = ii->first;
          void *sub_func_decl = m_ast->GetFuncDeclByCall(func_call);
          if (sub_func_decl == nullptr) {
            continue;
          }
          void *sub_unique_decl = m_ast->GetUniqueFuncDecl(sub_func_decl);
          if ((sub_unique_decl == nullptr) ||
              mFunc2Info.count(sub_unique_decl) <= 0) {
            continue;
          }
          vector<ArgumentInfo> &sub_arg_info = mFunc2Info[sub_unique_decl];
          const vector<CallInfo> &info = ii->second;
          for (size_t j = 0; j != info.size(); ++j) {
            //  if there is inconsistence in interface info,
            //  disable both of caller and callee
            if (sub_arg_info[info[j].pos].need_changed !=
                arg_info[i].need_changed) {
              sub_arg_info[info[j].pos].need_changed =
                  arg_info[i].need_changed = false;
              DEBUG(void *param = m_ast->GetFuncParam(func_decl, i);
                    cout
                    << "disable parameter \'" << m_ast->GetVariableName(param)
                    << "\' in function \'" << m_ast->GetFuncName(func_decl)
                    << "\' because of inconsistent between call hierarchy\n");

              LocalChanged = true;
            }
            if (sub_arg_info[info[j].pos].opt_bitwidth !=
                arg_info[i].opt_bitwidth) {
              sub_arg_info[info[j].pos].opt_bitwidth =
                  arg_info[i].opt_bitwidth =
                      min(sub_arg_info[info[j].pos].opt_bitwidth,
                          arg_info[i].opt_bitwidth);
              LocalChanged = true;
            }
          }
        }
      }
    }
  } while (LocalChanged);
}

bool MemoryCoalescingXilinx::updateKernelFunction() { return false; }

bool MemoryCoalescingXilinx::updateFunctionInterface() {
  bool Changed = false;
  for (size_t i = 0; i != mFunc_defs.size(); ++i) {
    Changed |= updateFunctionInterface(mFunc_defs[i]);
  }
  return Changed;
}

bool MemoryCoalescingXilinx::updateFunctionInterface(void *func_def) {
  bool Changed = false;
  SgFunctionDeclaration *func_decl =
      isSgFunctionDefinition(static_cast<SgNode *>(func_def))
          ->get_declaration();
  void *unique_decl = m_ast->GetUniqueFuncDecl(func_decl);
  if (unique_decl == nullptr) {
    return Changed;
  }
  assert(mFunc2Info.count(unique_decl) > 0);
  bool kernel = mMars_ir.is_kernel(m_ast, func_decl);
  void *func_body = m_ast->GetFuncBody(func_decl);
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
  for (int i = 0; i != arg_num; ++i) {
    ArgumentInfo info = arg_info[i];
    void *arg = m_ast->GetFuncParam(func_decl, i);
    map<string, string> message;
    string arg_name = m_ast->GetVariableName(arg);
    string curr_name = arg_name;
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
          message["memory_coalescing"] = "off";
          message["memory_coalescing_off_reason"] =
              curr_type_info + " is not supported now.";
          insertMessage(m_ast, arg, message);
          dump_critical_message(
              WDBUS_WARNING_10(var_info, curr_type_info, supported_type_info),
              0, m_ast, arg);
        }
        continue;
      }
#if PROJDEBUG
      cout << arg_name << " need to enlarge bit width" << endl;
#endif
      void *type = m_ast->GetTypebyVar(arg);
      m_ast->SetTypetoVar(arg, getLargeBitWidthType(type, info.opt_bitwidth,
                                                    info.bitwidth, arg));
      if (kernel) {
        string data_pack_str = "HLS DATA_PACK VARIABLE=" + curr_name;
        void *data_pack_pragma = m_ast->CreatePragma(data_pack_str, func_body);
        m_ast->PrependChild(func_body, data_pack_pragma);
        mBitWidthSet.insert(info.opt_bitwidth);
        string msg = "Memory coalescing inferred: variable " + var_info + "\n";
        msg +=
            "  interface bitwidth = " + my_itoa(info.opt_bitwidth) + " bits\n";
        string bitwidth = my_itoa(info.opt_bitwidth);
        message["memory_coalescing"] = "on";
        message["memory_coalescing_bitwidth"] = my_itoa(info.opt_bitwidth);
        insertMessage(m_ast, arg, message);
        if (!info.suboptimal_coalescing) {
          dump_critical_message(WDBUS_INFO_1(var_info, bitwidth));
        }
      }
      Changed = true;
    }
  }
  if (!Changed) {
    return false;
  }
  for (size_t j = 0; j != all_decls.size(); ++j) {
    void *decl = all_decls[j];
    if (decl == func_decl) {
      continue;
    }
    //  do not touch C host files
    //  1. not Cpp files
    //  2. no function body
    //  3. decl and func_decl are not the same files
    if (!m_ast->isWithInCppFile(decl) &&
        (m_ast->GetFuncBody(decl) == nullptr) &&
        m_ast->GetGlobal(decl) != m_ast->GetGlobal(func_decl)) {
      continue;
    }
    {
      //  create new function declaration
      string ret_type = m_ast->GetStringByType(m_ast->GetFuncReturnType(decl));
      string sFuncName = m_ast->GetFuncName(decl);
      vector<void *> fp_list;
      int num_args = m_ast->GetFuncParamNum(decl);
      for (int i = 0; i != num_args; i++) {
        ArgumentInfo info = arg_info[i];
        void *arg = m_ast->GetFuncParam(decl, i);
        if (info.need_changed && isSupportedType(info.base_type)) {
          arg = m_ast->GetFuncParam(func_decl, i);
        }
        void *sg_param = m_ast->CreateVariable(m_ast->GetTypebyVar(arg),
                                               m_ast->GetVariableName(arg));
        fp_list.push_back(sg_param);
      }
      void *new_decl = m_ast->CreateFuncDecl(
          ret_type, sFuncName, fp_list, m_ast->GetScope(decl), false, nullptr);
      if (m_ast->IsStatic(decl)) {
        m_ast->SetStatic(new_decl);
      }
      if (m_ast->IsExtern(decl)) {
        m_ast->SetExtern(new_decl);
      }
      m_ast->ReplaceChild(decl, new_decl);
    }
    //  SageInterface::setSourcePositionAsTransformation((SgNode*)decl);
  }
  AddMemCpyHeader(func_decl);
  //  2. update the sub function calls which are affected
  for (int i = 0; i != arg_num; ++i) {
    ArgumentInfo info = arg_info[i];
    if (!info.need_changed || !isSupportedType(info.base_type)) {
      continue;
    }
    for (map<void *, MemcpyInfo>::iterator ii = info.memcpy.begin();
         ii != info.memcpy.end(); ++ii) {
      void *memcpy_call = ii->first;
      MemcpyInfo memcpy_info = ii->second;
      memcpy_info.opt_bitwidth = info.opt_bitwidth;
      Changed |= updateMemcpyCallWithLargeBitWidth(memcpy_info, memcpy_call);
    }
    for (map<void *, SingleAssignInfo>::iterator ii =
             info.single_assign.begin();
         ii != info.single_assign.end(); ++ii) {
      SingleAssignInfo single_assign_info = ii->second;
      single_assign_info.opt_bitwidth = info.opt_bitwidth;
      Changed |= updateSingleAssignWithLargeBitWidth(
          single_assign_info, info.base_type, ii->first, func_decl);
    }
  }
  if (!kernel) {
    return Changed;
  }
  //  3. update previous function calls
  for (size_t i = 0; i != calls.size(); ++i) {
    void *call = calls[i];
    //  skip function call in C host file
    if (!m_ast->isWithInCppFile(call) &&
        m_ast->GetGlobal(call) != m_ast->GetGlobal(func_decl)) {
      continue;
    }
    int arg_num = m_ast->GetFuncCallParamNum(call);
    for (int j = 0; j != arg_num; ++j) {
      if (!arg_info[j].need_changed ||
          !isSupportedType(arg_info[j].base_type)) {
        continue;
      }
      void *orig_arg = m_ast->GetFuncCallParam(call, j);
      void *new_param = m_ast->GetFuncParam(func_decl, j);
      if (new_param == nullptr) {
        continue;
      }
      void *new_type = m_ast->GetTypebyVar(new_param);
      m_ast->AddCastToExp(orig_arg, m_ast->ArrayToPointerRecur(new_type, true));
    }
    AddDataTypeHeader(call);
  }
  return Changed;
}

void *MemoryCoalescingXilinx::getLargeBitWidthType(void *orig_type,
                                                   int opt_bitwidth,
                                                   int bitwidth, void *pos) {
  void *basic_type = nullptr;
  vector<size_t> DSize;
  m_ast->get_type_dimension(orig_type, &basic_type, &DSize, pos);
  size_t flatten_size = 1;
  for (auto size : DSize) {
    flatten_size *= size;
  }
  int elements = opt_bitwidth / bitwidth;
  flatten_size = (flatten_size + elements - 1) / elements;

  std::ostringstream ss;
  ss << opt_bitwidth;
  string new_base_type_str = "merlin_uint_" + ss.str();
  if (mCpp_design)
    new_base_type_str = "ap_uint<" + ss.str() + ">";
  m_ast->RegisterType(new_base_type_str);
  void *new_base_type = m_ast->GetTypeByString(new_base_type_str);
  std::stack<SgTypeModifier> st;
  while (isSgModifierType(static_cast<SgNode *>(basic_type)) ||
         isSgTypedefType(static_cast<SgNode *>(basic_type))) {
    auto sg_m_type = isSgModifierType(static_cast<SgNode *>(basic_type));
    if (sg_m_type != nullptr) {
      SgTypeModifier orig_modifier = sg_m_type->get_typeModifier();
      st.push(orig_modifier);
      basic_type = sg_m_type->get_base_type();
    } else {
      basic_type =
          isSgTypedefType(static_cast<SgNode *>(basic_type))->get_base_type();
    }
  }
  while (!st.empty()) {
    SgModifierType *new_sg_m_type = SageBuilder::buildModifierType(
        isSgType(static_cast<SgNode *>(new_base_type)));
    new_sg_m_type->get_typeModifier() = st.top();
    st.pop();
    new_base_type = new_sg_m_type;
  }
  if (flatten_size > 0) {
    return m_ast->CreateArrayType(new_base_type, vector<size_t>{flatten_size});
  }
  return m_ast->CreatePointerType(new_base_type);
}

void *
MemoryCoalescingXilinx::getLargeBitWidthTypeWithoutModifier(void *base_type,
                                                            int opt_bitwidth) {
  std::ostringstream ss;
  ss << opt_bitwidth;
  string new_base_type_str = "merlin_uint_" + ss.str();
  if (mCpp_design)
    new_base_type_str = "ap_uint<" + ss.str() + ">";
  m_ast->RegisterType(new_base_type_str);
  return m_ast->CreatePointerType(m_ast->GetTypeByString(new_base_type_str));
}

bool MemoryCoalescingXilinx::isMemcpy(void *func_call) {
  void *sub_func_decl = m_ast->GetFuncDeclByCall(func_call);
  if (sub_func_decl == nullptr) {
    sub_func_decl = m_ast->GetFuncDeclByCall(func_call, 0);
  }
  string func_name = m_ast->GetFuncName(sub_func_decl);
  return func_name == "memcpy";
}

bool MemoryCoalescingXilinx::updateMemcpyCallWithLargeBitWidth(
    const MemcpyInfo &info, void *orig_memcpy_call) {
  mWideBusFlag = true;
  void *scope = m_ast->TraceUpByType(orig_memcpy_call, "SgBasicBlock");
  SgBasicBlock *sg_scope = isSgBasicBlock(static_cast<SgNode *>(scope));
  void *caller = m_ast->TraceUpToFuncDecl(orig_memcpy_call);
  void *base_type = info.base_type;
  string new_func_name = "memcpy_wide_bus_";
  string template_header_file = "memcpy_" + my_itoa(info.opt_bitwidth);
  new_func_name += info.read ? "read" : "write";
  new_func_name += "_" + StripSignnessInType(base_type, true);
  if (info.opt_dimension > 1) {
    std::ostringstream oss;
    oss << info.opt_dimension << "d";
    new_func_name += "_" + oss.str();
    mCoalescingHeader.insert(template_header_file + ".h");
    template_header_file += "_" + oss.str() + ".h";
    mCoalescingHeader.insert(template_header_file);
    mCoalescingHeader.insert("mars_wide_bus_" + oss.str() + ".h");
    string macro_call;
    for (int i = 1; i < info.opt_dimension; ++i) {
      macro_call += "\n#define SIZE_" + my_itoa(i) + " " +
                    my_itoa(info.dimension_size[info.opt_dimension - 1 - i]);
      new_func_name +=
          "_" + my_itoa(info.dimension_size[info.opt_dimension - i - 1]);
    }
    macro_call += "\n#include \"" + template_header_file + "\"";
    for (int i = 1; i < info.opt_dimension; ++i) {
      macro_call += "\n#undef SIZE_" + my_itoa(i);
    }
    void *global_scope = m_ast->GetGlobal(caller);
    if (mMacroCall[global_scope].count(macro_call) <= 0) {
      m_ast->AddHeader(macro_call, global_scope);
      mMacroCall[global_scope].insert(macro_call);
    }
  }

  new_func_name += "_" + my_itoa(info.opt_bitwidth);
  int64_t flatten_burst_size = 1;
  for (int i = 0; i < info.opt_dimension; ++i) {
    flatten_burst_size *= info.dimension_size[info.opt_dimension - i - 1];
  }
  vector<void *> sg_init_list;
  void *new_type =
      getLargeBitWidthTypeWithoutModifier(base_type, info.opt_bitwidth);
  string buf_name;

  if (info.read) {
    void *target_exp = nullptr;
    if (info.opt_dimension == 1) {
      target_exp = m_ast->CopyExp(m_ast->RemoveCast(info.port[0].pntr));
      target_exp = AddCastToStripSignness(target_exp, base_type, info);
      sg_init_list.push_back(target_exp);
    } else {
      target_exp = m_ast->CreateVariableRef(info.port[0].array);
      int i = 0;
      while (i < info.dimension - info.opt_dimension) {
        target_exp = m_ast->CreateExp(V_SgPntrArrRefExp, target_exp,
                                      m_ast->CopyExp(info.port[0].index[i]));
        ++i;
      }
      target_exp = AddCastToStripSignness(target_exp, base_type, info);
      sg_init_list.push_back(target_exp);
      while (i < info.dimension) {
        sg_init_list.push_back(m_ast->CopyExp(info.port[0].index[i]));
        ++i;
      }
    }
    buf_name = m_ast->GetVariableName(info.port[0].array);

    void *src_exp = m_ast->CreateVariableRef(info.port[1].array);
    src_exp = m_ast->CreateExp(V_SgCastExp, src_exp, new_type);
    sg_init_list.push_back(src_exp);

    if (info.port[1].index.size() == 1 && info.port[1].index[0] != nullptr) {
      void *orig_offset = m_ast->CopyExp(info.port[1].index[0]);
      void *new_offset = m_ast->CreateExp(
          V_SgMultiplyOp, orig_offset, m_ast->CreateConst(info.bitwidth / 8));
      sg_init_list.push_back(new_offset);

    } else {
      sg_init_list.push_back(m_ast->CreateConst(0));
    }
    sg_init_list.push_back(m_ast->CopyExp(info.length_exp));
    //  if (info.opt_dimension > 1) {
    //  //  head align
    //  void *orig_offset = m_ast->CopyExp(info.port[1].index[0]);
    //  void *head_align = m_ast->CreateExp(V_SgBitAndOp, orig_offset,
    //                                     m_ast->CreateConst(num_elements -
    //                                     1));
    //  sg_init_list.push_back(head_align);
    //  //  aligned
    //  sg_init_list.push_back(m_ast->CreateConst(info.aligned_dimension));
    //  }
  } else {
    void *target_exp = m_ast->CreateVariableRef(info.port[0].array);
    buf_name = m_ast->GetVariableName(info.port[1].array);
    target_exp = m_ast->CreateExp(V_SgCastExp, target_exp, new_type);
    sg_init_list.push_back(target_exp);
    void *src_exp = nullptr;
    if (info.opt_dimension == 1) {
      src_exp = m_ast->CopyExp(m_ast->RemoveCast(info.port[1].pntr));
      src_exp = AddCastToStripSignness(src_exp, base_type, info);
      sg_init_list.push_back(src_exp);
    } else {
      src_exp = m_ast->CreateVariableRef(info.port[1].array);
      int i = 0;
      while (i < info.dimension - info.opt_dimension) {
        src_exp = m_ast->CreateExp(V_SgPntrArrRefExp, src_exp,
                                   m_ast->CopyExp(info.port[1].index[i]));
        ++i;
      }
      src_exp = AddCastToStripSignness(src_exp, base_type, info);
      sg_init_list.push_back(src_exp);
      while (i < info.dimension) {
        sg_init_list.push_back(m_ast->CopyExp(info.port[1].index[i]));
        ++i;
      }
    }
    if (info.port[0].index.size() == 1 && info.port[0].index[0] != nullptr) {
      void *orig_offset = m_ast->CopyExp(info.port[0].index[0]);
      void *new_offset = m_ast->CreateExp(
          V_SgMultiplyOp, m_ast->CreateConst(info.bitwidth / 8), orig_offset);
      sg_init_list.push_back(new_offset);
    } else {
      sg_init_list.push_back(m_ast->CreateConst(0));
    }
    sg_init_list.push_back(m_ast->CopyExp(info.length_exp));
    //  if (info.opt_dimension > 1) {
    //  //  head align
    //  void *orig_offset = m_ast->CopyExp(info.port[0].index[0]);
    //  void *head_align = m_ast->CreateExp(V_SgBitAndOp, orig_offset,
    //                                     m_ast->CreateConst(num_elements -
    //                                     1));
    //  sg_init_list.push_back(head_align);
    //  //  aligned
    //  sg_init_list.push_back(m_ast->CreateConst(info.aligned_dimension));
    //  }
  }

  {
    //  YX: FixBug1754 add const burst size in order to get upper bound
    CMarsExpression me_length(info.length_exp, m_ast, orig_memcpy_call);
    int64_t len_ub =
        (me_length / (info.bitwidth / 8)).get_range().get_const_ub();
    sg_init_list.push_back(m_ast->CreateConst(min(len_ub, flatten_burst_size)));
  }

  //  add partition pragma to achieve II=1
  AddPartitionPragma(buf_name, m_ast->GetParent(orig_memcpy_call), info);

  void *new_func_call =
      m_ast->CreateFuncCall(new_func_name, m_ast->GetTypeByString("void"),
                            sg_init_list, sg_scope, orig_memcpy_call);
#if 1
  void *orig_func_stat = m_ast->GetParent(orig_memcpy_call);
  void *new_func_stat = m_ast->CreateStmt(V_SgExprStatement, new_func_call);
  m_ast->InsertAfterStmt(new_func_stat, orig_func_stat);
  //  m_ast->RemoveStmt(orig_func_stat);
  mDead_func_calls.insert(orig_func_stat);
#else
  m_ast->ReplaceExp(orig_memcpy_call, new_func_call);
#endif
  map<string, string> message;
  message["memory_coalescing"] = "on";
  insertMessage(m_ast, new_func_call, message);

  return true;
}

bool MemoryCoalescingXilinx::updateSingleAssignWithLargeBitWidth(
    const SingleAssignInfo &info, void *base_type, void *orig_access,
    void *kernel_decl) {
  void *scope = m_ast->TraceUpByType(orig_access, "SgBasicBlock");
  void *stmt = m_ast->TraceUpToStatement(orig_access);
  SgBasicBlock *sg_scope = isSgBasicBlock(static_cast<SgNode *>(scope));
  void *buf = createBuffer(base_type, sg_scope, kernel_decl);
  string read_func_name = "memcpy_wide_bus_single_read_" +
                          StripSignnessInType(base_type, true) + "_" +
                          my_itoa(info.opt_bitwidth);
  string write_func_name = "memcpy_wide_bus_single_write_" +
                           StripSignnessInType(base_type, true) + "_" +
                           my_itoa(info.opt_bitwidth);
  vector<void *> sg_init_list;
  void *new_type =
      getLargeBitWidthTypeWithoutModifier(base_type, info.opt_bitwidth);
  if (info.read) {
    void *src_exp = m_ast->CreateVariableRef(info.arg);
    src_exp = m_ast->CreateExp(V_SgCastExp, src_exp, new_type);
    sg_init_list.push_back(src_exp);
    if (info.index.size() == 1 && info.index[0] != nullptr) {
      void *orig_offset = m_ast->CopyExp(info.index[0]);
      void *new_offset = m_ast->CreateExp(
          V_SgMultiplyOp, orig_offset, m_ast->CreateConst(info.bitwidth / 8));
      sg_init_list.push_back(new_offset);
    } else {
      sg_init_list.push_back(m_ast->CreateConst(0));
    }
    void *new_func_call = m_ast->CreateFuncCall(read_func_name, base_type,
                                                sg_init_list, sg_scope);

    void *new_stmt = m_ast->CreateStmt(V_SgAssignStatement, buf, new_func_call);
    m_ast->InsertStmt(new_stmt, stmt);
    if (!info.write) {
      m_ast->ReplaceExp(orig_access, m_ast->CreateVariableRef(buf));
    }
  }
  if (info.write) {
    sg_init_list.clear();
    void *target_exp = m_ast->CreateVariableRef(info.arg);
    target_exp = m_ast->CreateExp(V_SgCastExp, target_exp, new_type);
    sg_init_list.push_back(target_exp);
    sg_init_list.push_back(m_ast->CreateVariableRef(buf));
    if (info.index.size() == 1 && info.index[0] != nullptr) {
      void *orig_offset = m_ast->CopyExp(info.index[0]);
      void *new_offset = m_ast->CreateExp(
          V_SgMultiplyOp, m_ast->CreateConst(info.bitwidth / 8), orig_offset);
      sg_init_list.push_back(new_offset);
    } else {
      sg_init_list.push_back(m_ast->CreateConst(0));
    }
    void *new_func_call =
        m_ast->CreateFuncCall(write_func_name, m_ast->GetTypeByString("void"),
                              sg_init_list, sg_scope);
    void *write_stmt = m_ast->CreateStmt(V_SgExprStatement, new_func_call);

    m_ast->InsertAfterStmt(write_stmt, stmt);
    m_ast->ReplaceExp(orig_access, m_ast->CreateVariableRef(buf));
  }
  return true;
}

bool MemoryCoalescingXilinx::isSupportedType(void *base_type) {
  if (mCpp_design)
    return true;
  string str = m_ast->GetStringByType(base_type);
  auto pos = str.find('<');
  if (pos != string::npos)
    str = str.substr(0, pos);
  pos = str.find(CLASS_PREFIX);
  if (pos == 0)
    str = str.substr(sizeof(CLASS_PREFIX) - 1);
  boost::trim(str);
  DEBUG(cout << "basic type is " << str << endl);
  for (auto one_type : m_supported_types) {
    if (str == one_type) {
      return true;
    }
  }
  return false;
}

void *MemoryCoalescingXilinx::AddCastToStripSignness(void *address_exp,
                                                     void *base_type,
                                                     const MemcpyInfo &info) {
  string org_type_str = m_ast->GetStringByType(base_type);
  string new_type_str = StripSignnessInType(base_type, false);
  if (org_type_str == new_type_str || new_type_str == "struct") {
    return address_exp;
  }
  void *new_type = m_ast->GetTypeByString(new_type_str);
  vector<size_t> sub_dims;
  for (int i = 0; i < info.opt_dimension - 1; ++i) {
    sub_dims.push_back(info.dimension_size[info.opt_dimension - 2 - i]);
  }
  if (!sub_dims.empty()) {
    new_type = m_ast->CreateArrayType(new_type, sub_dims);
  }
  new_type = m_ast->CreatePointerType(new_type);
  return m_ast->CreateExp(V_SgCastExp, address_exp, new_type);
}

string MemoryCoalescingXilinx::StripSignnessInType(void *base_type,
                                                   bool replace_space) {
  string org_type = m_ast->GetStringByType(base_type);
  if (m_ast->IsStructureType(m_ast->GetBaseType(base_type, true)) ||
      m_ast->IsClassType(m_ast->GetBaseType(base_type, true))) {
    return "struct";
  }
  string signed_p = "signed ";
  string unsigned_p = "unsigned ";
  string::size_type pos = org_type.find(unsigned_p);
  string new_type = org_type;
  if (pos != string::npos) {
    new_type.replace(pos, unsigned_p.size(), "");
  } else {
    pos = org_type.find(signed_p);
    if (pos != string::npos) {
      new_type.replace(pos, signed_p.size(), "");
    }
  }
  if (replace_space) {
    pos = new_type.find('<');
    if (pos != string::npos)
      new_type = new_type.substr(0, pos);
    pos = new_type.find(CLASS_PREFIX);
    if (pos == 0)
      new_type = new_type.substr(sizeof(CLASS_PREFIX) - 1);
    boost::trim(new_type);
    if (new_type == "ap_uint")
      new_type = "ap_int";
    std::replace_if(new_type.begin(), new_type.end(),
                    [](auto c) { return c == ' '; }, '_');
  }

  return new_type;
}

void MemoryCoalescingXilinx::AddPartitionPragma(const string &buf_name,
                                                void *stmt,
                                                const MemcpyInfo &info) {
  int num_dimension = info.dimension_size.size();
  if (info.unaligned_buffer && num_dimension > 1) {
    return;
  }
  int element_size = info.opt_bitwidth / info.bitwidth;
  int curr_dim = 0;
  int total_size = 1;
  int prev_total_size = 1;
  int total_dim = info.dimension_size.size();
  while (curr_dim < info.opt_dimension) {
    int curr_size = info.dimension_size[curr_dim];
    total_size *= curr_size;
    if (total_size < element_size) {
      string partition_pragma = " HLS array_partition  variable=";
      partition_pragma += buf_name + " complete ";
      partition_pragma += " dim=" + my_itoa(total_dim - curr_dim);
      void *pragma =
          m_ast->CreatePragma(partition_pragma, m_ast->GetScope(stmt));
      m_ast->InsertStmt(pragma, stmt);
    } else {
      break;
    }
    prev_total_size *= curr_size;
    curr_dim++;
  }
  string partition_pragma = " HLS array_partition  variable=";
  partition_pragma += buf_name + " cyclic factor = ";
  partition_pragma += my_itoa(element_size / prev_total_size);
  partition_pragma += " dim=" + my_itoa(total_dim - curr_dim);
  void *pragma = m_ast->CreatePragma(partition_pragma, m_ast->GetScope(stmt));
  m_ast->InsertStmt(pragma, stmt);
}

MemoryCoalescingXilinx::~MemoryCoalescingXilinx() {
  vector<void *> func_stmts(mDead_func_calls.begin(), mDead_func_calls.end());
  mDead_func_calls.clear();
  for (size_t i = 0; i != func_stmts.size(); ++i) {
    m_ast->RemoveStmt(func_stmts[i]);
  }
}

bool MemoryCoalescingXilinx::isUnsupportedSingleAssign(void *single_assign) {
  if (m_ast->IsAssignOp(single_assign) != 0) {
    return false;
  }
  if (m_ast->GetBinaryOperationFromCompoundAssignOp(single_assign) != 0) {
    return false;
  }
  if (m_ast->IsPlusPlusOp(single_assign) != 0) {
    return false;
  }
  if (m_ast->IsMinusMinusOp(single_assign) != 0) {
    return false;
  }
  return true;
}

void *MemoryCoalescingXilinx::createBuffer(void *base_type, void *sg_scope,
                                           void *func_decl) {
  string bufname_str = "tmp";
  m_ast->get_valid_name(sg_scope, &bufname_str);
  void *new_buf =
      m_ast->CreateVariableDecl(base_type, bufname_str, nullptr, sg_scope);

  m_ast->PrependChild(sg_scope, new_buf, true);

  return new_buf;
}

void MemoryCoalescingXilinx::detect_access_in_paralleled_loop(
    void *ref, ArgumentInfo *info) {
  //  1. check whether the immediate loop of 'ref' is a for-loop
  void *loop = m_ast->TraceUpToLoopScope(ref);
  if ((loop == nullptr) || (m_ast->IsForStatement(loop) == 0)) {
    return;
  }
  //  2. check whether the index of external memory is linear to for-loop
  CMarsAccess access(ref, m_ast, loop);
  CMarsExpression index = access.GetIndexes()[0];
  map<void *, int64_t> iterators;
  map<void *, int64_t> invariants;
  if (!index.IsStandardForm(loop, &iterators, &invariants)) {
    return;
  }
  if (iterators.count(loop) <= 0 || iterators[loop] != 1) {
    return;
  }
  //  3. check whether the loop is completely unrolled and loop lower
  //  bound/upper bound is constant, step is one
  CMarsLoop *loop_info = mMars_ir_v2.get_loop_info(loop);
  if (loop_info->has_parallel() == 0) {
    return;
  }
  void *iv;
  void *lb;
  void *ub;
  void *step;
  int64_t nStep = 0;
  bool ret = m_ast->IsCanonicalForLoop(loop, &iv, &lb, &ub, &step) != 0;

  ret &= m_ast->GetLoopStepValueFromExpr(step, &nStep);
  if (!ret || std::abs(nStep) != 1) {
    return;
  }
  CMarsExpression me_lb(lb, m_ast);
  CMarsExpression me_ub(ub, m_ast);
  if ((me_lb.IsConstant() == 0) || (me_ub.IsConstant() == 0)) {
    return;
  }
  // 4. check whether the start index is aligned
  index.substitute(loop, me_lb);
  int opt_bitwidth = info->opt_bitwidth;
  int orig_bitwidth = info->bitwidth;
  if (info->specified_bitwidth) {
    int num_elements = opt_bitwidth / orig_bitwidth;
    if (!divisible(index, num_elements, false)) {
      info->all_support_access = false;
      DEBUG(cout << "disable wide bus because of unaligned access in "
                    "paralleled loop: \n"
                 << m_ast->_p(ref, 0) << endl;);
    } else {
      info->any_burst_access = true;
    }
  } else {
    while (opt_bitwidth > orig_bitwidth) {
      int num_elements = opt_bitwidth / orig_bitwidth;
      if (divisible(index, num_elements, false)) {
        info->any_burst_access = true;
        info->opt_bitwidth = opt_bitwidth;
        return;
      }
      opt_bitwidth >>= 1;
    }
    info->all_support_access = false;
    DEBUG(cout << "disable wide bus because of unaligned access in paralleled "
                  "loop: \n"
               << m_ast->_p(ref, 0) << endl;);
  }
  return;
}

void MemoryCoalescingXilinx::get_opt_bitwidth(int *opt_bitwidth,
                                              bool *specified_bitwidth,
                                              void *arg, int bitwidth,
                                              const vector<void *> &vec_pragma,
                                              bool report) {
  bool disable_opt_bitwidth = false;
  string arg_name = m_ast->GetVariableName(arg);
  auto arg_user_info = getUserCodeInfo(m_ast, arg);
  if (!arg_user_info.name.empty()) {
    arg_name = arg_user_info.name;
  }
  for (auto pragma : vec_pragma) {
    if (mMars_ir_v2.get_pragma_attribute(pragma, "variable") == arg_name) {
      string bw_str = mMars_ir_v2.get_pragma_attribute(pragma, "bus_bitwidth");
      if (!bw_str.empty()) {
        int bw = my_atoi(bw_str);
        if (!checkInvalidBitWidth(bw, bitwidth)) {
          *opt_bitwidth = bw;
          *specified_bitwidth = true;
        } else {
          boost::algorithm::to_lower(bw_str);
          disable_opt_bitwidth = true;
          if (report) {
            string var_info = "'" + arg_name + "' (";
            var_info += arg_user_info.file_location + ")";
            map<string, string> message;
            if (bw_str == "off") {
              message["memory_coalescing"] = "off";
              message["memory_coalescing_off_reason"] = "disabled by users";
              insertMessage(m_ast, arg, message);
              dump_critical_message(WDBUS_WARNING_12(var_info), 0, m_ast, arg);
            } else {
              message["memory_coalescing"] = "off";
              message["memory_coalescing_off_reason"] =
                  "invalid bitwidth: " + bw_str;
              insertMessage(m_ast, arg, message);
              dump_critical_message(WDBUS_WARNING_13(var_info, bw_str), 0,
                                    m_ast, arg);
            }
          }
        }
      }
    }
  }
  if (*opt_bitwidth == 0) {
    if (!disable_opt_bitwidth) {
      if (mBitWidthSpecified) {
        if (!checkInvalidBitWidth(mBitWidth, bitwidth)) {
          *opt_bitwidth = mBitWidth;
        } else {
          *opt_bitwidth = bitwidth;
        }
        *specified_bitwidth = true;
      } else {
        *opt_bitwidth = DEFAULT_BUS_BITWIDTH;
        *specified_bitwidth = false;
      }
    } else {
      *opt_bitwidth = bitwidth;
      *specified_bitwidth = false;
    }
  }
}
