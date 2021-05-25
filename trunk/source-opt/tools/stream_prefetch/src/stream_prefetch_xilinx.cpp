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


#include "stream_prefetch_xilinx.h"
#include <tuple>
#include <unordered_set>
#include "memory_coalescing_xilinx.h"
#include "mars_ir.h"
#include "mars_ir_v2.h"
#include "mars_message.h"
#define DEFAULT_WIDE_BUS_BITWIDTH 512
#undef DEBUG
#define DISABLE_DEBUG 0
#if DISABLE_DEBUG
#define DEBUG(stmts)
#else
#define DEBUG(stmts)                                                           \
  do {                                                                         \
    stmts;                                                                     \
  } while (false)
#endif
using MarsProgramAnalysis::ArrayRangeAnalysis;
using MarsProgramAnalysis::CMarsAccess;
using MarsProgramAnalysis::CMarsExpression;
using MarsProgramAnalysis::CMarsRangeExpr;
using MarsProgramAnalysis::CMarsVariable;
using std::tuple;
using std::unordered_set;
void StreamPrefetchXilinx::init() {
  m_burst_single_threshold = SINGLE_BUFFER_SIZE_THRESHOLD;
  string max_size_of_buffer_str =
      mOptions.get_option_key_value("-a", "burst_single_size_threshold");
  if (!max_size_of_buffer_str.empty()) {
    m_burst_single_threshold = my_atoi(max_size_of_buffer_str);
  }
}
int StreamPrefetchXilinx::run() {
  if (m_ast->IsPureCDesign()) {
    DEBUG(cout << "cannot support stream prefetch for C design but only for "
                  "C++ design\n");
    if (!m_analysis)
      dump_critical_message(STREAM_WARNING_3);
    return 0;
  }
  mars_ir_v2.build_mars_ir(m_ast, mTopFunc);
  mars_ir.get_mars_ir(m_ast, mTopFunc, mOptions, true);
  MemoryCoalescingXilinx memory_coalescing_xilinx(m_ast, mTopFunc, mOptions);
  auto kernels = mars_ir_v2.get_top_kernels();
  set<void *> insert_hls_stream_h;
  for (auto kernel : kernels) {
    map<void *, tuple<void *, void *>> read_port_to_streaming_func;
    map<void *, tuple<void *, void *>> write_port_to_streaming_func;
    string kernel_name = m_ast->GetFuncName(kernel);
    vector<void *> vec_pragma;
    m_ast->GetNodesByType_int(kernel, "preorder", V_SgPragmaDeclaration,
                              &vec_pragma);
    string scope_info = getUserCodeFileLocation(m_ast, kernel, true);
    for (auto arg : m_ast->GetFuncParams(kernel)) {
      m_loop_info.clear();
      auto arg_info = getUserCodeInfo(m_ast, arg);
      string arg_name = m_ast->GetVariableName(arg);
      string orig_var_name =
          arg_info.name.empty() ? m_ast->GetVariableName(arg) : arg_info.name;
      string port_info =
          "'" + orig_var_name + "' (" + arg_info.file_location + ")";
      void *type = m_ast->GetTypebyVar(arg);
      if ((m_ast->IsPointerType(type) == 0) &&
          (m_ast->IsArrayType(type) == 0)) {
        continue;
      }
      if (!mars_ir.every_trace_is_bus(m_ast, kernel, arg, arg))
        continue;
      bool enable_stream = false;
      bool disable_stream = false;
      for (auto pragma : vec_pragma) {
        if (mars_ir_v2.get_pragma_attribute(pragma, "interface") != "") {
          if (mars_ir_v2.get_pragma_attribute(pragma, "variable") == arg_name) {
            string option =
                mars_ir_v2.get_pragma_attribute(pragma, CMOST_stream);
            if (option != "") {
              boost::to_upper(option);
              if (option == CMOST_ON) {
                enable_stream = true;
              } else if (option == CMOST_OFF) {
                disable_stream = true;
              } else if (option == CMOST_auto) {
                DEBUG(cout << "auto stream ");
              } else {
                DEBUG(cout << "invalid stream option \'" << option << '\n');
              }
            }
            break;
          }
        }
      }
      if (disable_stream) {
        DEBUG(cout << "disable streaming for port \'" << arg_name << "\'\n");
        continue;
      }
      bool report_warning = enable_stream;
      DEBUG(cout << "try to apply streaming prefetch for port \'" << arg_name
                 << "\'\n");
      if (m_ast->get_dim_num_from_var(arg) != 1) {
        DEBUG(cout << "cannot support multiple dimension array/pointer \'"
                   << arg_name << "\'\n");
        continue;
      }
      void *basic_type = m_ast->GetBaseType(type, false);
      int bitwidth = m_ast->get_bitwidth_from_type(basic_type, false);
      int opt_bitwidth = DEFAULT_WIDE_BUS_BITWIDTH;
      bool specified_opt_bitwidth = false;
      memory_coalescing_xilinx.get_opt_bitwidth(&opt_bitwidth,
                                                &specified_opt_bitwidth, arg,
                                                bitwidth, vec_pragma, false);
      int wide_bus_factor = 1;
      if (opt_bitwidth && bitwidth >= 8 && (bitwidth & (bitwidth - 1)) == 0)
        wide_bus_factor = opt_bitwidth / bitwidth;
      void *single_access = nullptr;
      if (!get_single_access(arg, &single_access, kernel)) {
        DEBUG(cout << "not satisfy single access checking\n");
        if (report_warning && !m_analysis)
          dump_critical_message(STREAM_WARNING_11(port_info));
        continue;
      }
      void *array = m_ast->get_array_from_pntr(single_access);
      if (!array) {
        if (!m_analysis && report_warning)
          dump_critical_message(STREAM_WARNING_12(port_info));
        continue;
      }
      void *array_ref = nullptr;
      if (!m_ast->parse_array_ref_from_pntr(single_access, &array_ref,
                                            single_access) ||
          m_ast->detect_user_for_loop_burst(array_ref, single_access,
                                            &mars_ir)) {
        cout << "do not infer streaming because of manual burst" << endl;
        continue;
      }
      CMarsExpression c_start, c_length;
      void *streaming_intrinsic = get_streaming_access_info_from_intrinsic(
          array, kernel, single_access, &c_start, &c_length, &array_ref);
      CMarsRangeExpr mr_range;
      bool read_only = true;
      vector<int> vec_parallel_factor;
      vector<void *> vec_parallel_loops;
      vector<tuple<void *, int64_t, int64_t, int64_t, bool>>
          non_sequentil_loops;
      void *top_sequential_scope = nullptr;
      bool checking_res = false;
      if (streaming_intrinsic != nullptr) {
        checking_res = check_streaming_access_from_intrinsic(
            arg, single_access, kernel, report_warning, c_start, c_length,
            array_ref, &mr_range, &read_only, &vec_parallel_loops,
            &vec_parallel_factor, &non_sequentil_loops, &top_sequential_scope);

      } else {
        checking_res = check_streaming_access(
            arg, single_access, kernel, report_warning, &mr_range, &read_only,
            &vec_parallel_loops, &vec_parallel_factor, &non_sequentil_loops,
            &top_sequential_scope);
      }
      void *assign_val = nullptr;
      void *assign_exp = m_ast->GetParent(single_access);
      if (m_ast->IsDotExp(assign_exp) &&
          m_ast->IsFunctionCall(m_ast->GetParent(assign_exp))) {
        assign_exp = m_ast->GetParent(assign_exp);
      }
      if (!read_only) {
        void *left_exp = nullptr;
        m_ast->IsGeneralAssignOp(assign_exp, &left_exp, &assign_val);
        if (left_exp != single_access) {
          DEBUG(cout << "not general single write access\n");
          if (!m_analysis && report_warning)
            dump_critical_message(STREAM_WARNING_12(port_info));
          continue;
        }
      }

      // check whether it has been streaming prefetched
      if (read_only) {
        if (m_ast->GetFuncCallParamIndex(single_access) != -1) {
          void *ref = nullptr;
          if (m_ast->is_xilinx_channel_write(
                  m_ast->TraceUpToFuncCall(single_access), &ref)) {
            DEBUG(cout << "has been applied by streaming prefetch\n");
            continue;
          }
        }
      } else {
        void *ref = nullptr;
        if (m_ast->is_xilinx_channel_read(assign_val, &ref)) {
          DEBUG(cout << "has been applied by streaming prefetch\n");
          continue;
        }
        if (m_ast->GetFuncCallParamIndex(single_access) != -1) {
          if (m_ast->is_xilinx_channel_read(
                  m_ast->TraceUpToFuncCall(single_access), &ref)) {
            // e.g. stream >> a[i]
            DEBUG(cout << "has been applied by streaming prefetch\n");
            continue;
          }
        }
      }
      if (!checking_res || top_sequential_scope == nullptr) {
        continue;
      }
      for (auto loop : vec_parallel_loops) {
        CMarsLoop *loop_info = mars_ir_v2.get_loop_info(loop);
        string str_loop_info = getUserCodeFileLocation(m_ast, loop, true);
        if (!loop_info->is_complete_unroll() ||
            !is_constant_bound_canonical_loop(loop)) {
          if (!m_analysis && report_warning)
            dump_critical_message(STREAM_WARNING_10(port_info, str_loop_info));
          continue;
        }
      }
      int total_parallel_factor = 1;
      for (auto factor : vec_parallel_factor) {
        total_parallel_factor *= factor;
      }
      int orig_wide_bus_factor = wide_bus_factor;
      if (!check_parallel_factor_matched_with_wide_bus_factor(
              total_parallel_factor, &wide_bus_factor,
              specified_opt_bitwidth)) {
        if (!m_analysis && report_warning)
          dump_critical_message(STREAM_WARNING_8(port_info,
                                                 my_itoa(total_parallel_factor),
                                                 my_itoa(wide_bus_factor)));
        DEBUG(cout << "compuation parallel factor is not matched with wide bus "
                      "factor\n");
        continue;
      }
      CMarsExpression me_start, me_length;
      if (!is_constant_and_aligned_address_and_length(
              mr_range, &me_start, &me_length, wide_bus_factor)) {
        if (!m_analysis && report_warning)
          dump_critical_message(STREAM_WARNING_9(port_info, me_start.print_s(),
                                                 me_length.print_s(),
                                                 my_itoa(wide_bus_factor)));
        DEBUG(cout << "not satisfy constant and aligned address and length "
                      "checking\n");
        continue;
      }
      if (is_short_streaming_length(me_length, orig_wide_bus_factor)) {
        if (!m_analysis && report_warning)
          dump_critical_message(
              STREAM_WARNING_13(port_info, me_length.print_s()));
        DEBUG(cout << "too short streaming length \n");
        continue;
      }

      update_interface_pragma(arg, kernel, wide_bus_factor * bitwidth, me_start,
                              me_length, top_sequential_scope);
      if (m_analysis)
        continue;
      DEBUG(cout << "generating streaming prefetch for port \'" << arg_name
                 << "\'\n");
      dump_critical_message(STREAM_INFO_1(port_info, scope_info,
                                          me_length.print_s(),
                                          my_itoa(wide_bus_factor)));
      {
        map<string, string> msg_map;
        msg_map["display_name"] =
            "auto streaming for \'" + orig_var_name + "\'";
        msg_map["streaming"] = "on";
        if (read_only) {
          msg_map["is_read"] = "1";
        } else {
          msg_map["is_write"] = "1";
        }
        insertMessage(m_ast, arg, msg_map);
      }
      string type_string =
          "hls::stream<" + m_ast->GetStringByType(basic_type) + " >";
      m_ast->RegisterType(type_string);
      void *channel_type = m_ast->GetTypeByString(type_string);
      int channel_size = 1;
      if (total_parallel_factor > 1) {
        if (total_parallel_factor >= wide_bus_factor) {
          if (read_only && wide_bus_factor > 1) {
            channel_type = m_ast->CreateArrayType(
                channel_type, vector<size_t>(1, wide_bus_factor));
            channel_size = wide_bus_factor;
          }
          if (!read_only) {
            // MER2177 for output, in order not to block computation part, we
            // make output channel matched with parallel factor
            channel_type = m_ast->CreateArrayType(
                channel_type, vector<size_t>(1, total_parallel_factor));
            channel_size = total_parallel_factor;
          }
        } else {
          channel_size = total_parallel_factor;
          channel_type = m_ast->CreateArrayType(
              channel_type, vector<size_t>(1, total_parallel_factor));
        }
      }
      void *new_channel = m_ast->CreateVariableDecl(
          channel_type, "__merlin" + kernel_name + "_" + arg_name + "_ch",
          nullptr, m_ast->GetScope(kernel));
      m_ast->InsertStmt(new_channel, m_ast->GetFirstFuncDeclInGlobal(kernel),
                        true);
      m_ast->SetStatic(new_channel);
      void *streaming_func = createStreamingFunction(
          arg, "__merlin" + kernel_name + "_" + arg_name + "_streaming",
          me_start, me_length, new_channel, channel_size, vec_parallel_factor,
          wide_bus_factor, read_only, m_ast->GetScope(kernel),
          non_sequentil_loops);
      m_ast->InsertStmt(streaming_func, kernel, true);
      replaceSingleAccessWithChannel(
          single_access, assign_exp, assign_val, new_channel,
          vec_parallel_loops, channel_size, read_only, m_ast->GetScope(kernel));
      if (read_only)
        read_port_to_streaming_func[arg] =
            std::make_tuple(streaming_func, new_channel);
      else
        write_port_to_streaming_func[arg] =
            std::make_tuple(streaming_func, new_channel);
    }
    if (read_port_to_streaming_func.empty() &&
        write_port_to_streaming_func.empty())
      continue;
    void *computation_kernel =
        m_ast->CloneFuncDecl(kernel, m_ast->GetScope(kernel),
                             "__merlin" + kernel_name + "_computation");
    m_ast->InsertStmt(computation_kernel, kernel, true);
    clean_interface_pragma(computation_kernel);
    remove_all_logic_but_keep_interface_pragma(kernel);
    void *kernel_body = m_ast->GetFuncBody(kernel);
    m_ast->AppendChild(kernel_body,
                       m_ast->CreatePragma("HLS dataflow", kernel_body));
    for (auto read_port : read_port_to_streaming_func) {
      m_ast->AppendChild(
          kernel_body,
          m_ast->CreatePragma(
              "HLS stream variable=" +
                  m_ast->GetVariableName(std::get<1>(read_port.second)),
              kernel_body));
      void *streaming_call = m_ast->CreateFuncCallStmt(
          std::get<0>(read_port.second),
          vector<void *>(1, m_ast->CreateVariableRef(read_port.first)));
      m_ast->AppendChild(kernel_body, streaming_call);
    }
    vector<void *> arg_list;
    for (auto arg : m_ast->GetFuncParams(kernel)) {
      if (read_port_to_streaming_func.count(arg) > 0 ||
          write_port_to_streaming_func.count(arg) > 0) {
        // pass a nullptr to avoid HLS tool issue
        arg_list.push_back(m_ast->CreateConst(0));
      } else {
        arg_list.push_back(m_ast->CreateVariableRef(arg));
      }
    }
    void *computation_call =
        m_ast->CreateFuncCallStmt(computation_kernel, arg_list);
    m_ast->AppendChild(kernel_body, computation_call);
    for (auto write_port : write_port_to_streaming_func) {
      m_ast->AppendChild(
          kernel_body,
          m_ast->CreatePragma(
              "HLS stream variable=" +
                  m_ast->GetVariableName(std::get<1>(write_port.second)),
              kernel_body));
      void *streaming_call = m_ast->CreateFuncCallStmt(
          std::get<0>(write_port.second),
          vector<void *>(1, m_ast->CreateVariableRef(write_port.first)));
      m_ast->AppendChild(kernel_body, streaming_call);
    }
    void *sg_global = m_ast->GetGlobal(kernel);
    if (insert_hls_stream_h.count(sg_global) > 0)
      continue;
    m_ast->AddHeader("#include<hls_stream.h>", sg_global);
    insert_hls_stream_h.insert(sg_global);
  }
  return 1;
}

bool StreamPrefetchXilinx::get_single_access(void *arg, void **single_access,
                                             void *scope) {
  vector<void *> vec_accesses;
  int ret = m_ast->GetPntrReferenceOfArray(arg, scope, &vec_accesses, false);
  if (!ret) {
    DEBUG(cout << "not sure whether it is single access\n");
    return false;
  }
  // skip function argument
  int access_times = 0;
  for (auto access : vec_accesses) {
    void *type = m_ast->GetTypeByExp(access);
    if (m_ast->IsArrayType(type) || m_ast->IsPointerType(type)) {
      // skip partial access as function argument
      if (m_ast->GetFuncCallParamIndex(access) != -1) {
        void *func_decl =
            m_ast->GetFuncDeclByCall(m_ast->TraceUpToFuncCall(access));
        if (func_decl != nullptr)
          continue;
      }
    }
    *single_access = access;
    access_times++;
  }
  if (access_times != 1) {
    DEBUG(cout << "not sure whether it is single access\n");
    return false;
  }
  void *exp_type = m_ast->GetTypeByExp(*single_access);
  if (m_ast->IsPointerType(exp_type) || m_ast->IsArrayType(exp_type)) {
    DEBUG(cout << "cannot support pointer access\n");
    return false;
  }
  return true;
}

CMarsExpression
StreamPrefetchXilinx::parse_index(void *single_access, void *scope,
                                  const CMarsExpression &offset) {
  CMarsAccess mar_access(single_access, m_ast,
                         m_ast->TraceUpToFuncDecl(single_access));
  CMarsExpression me = mar_access.GetIndex(0) + offset;
  void *array = mar_access.GetArray();
  if (m_ast->IsArgumentInitName(array)) {
    void *func = m_ast->TraceUpToFuncDecl(array);
    if (func != scope) {
      vector<void *> vec_calls;
      m_ast->GetFuncCallsFromDecl(func, nullptr, &vec_calls);
      if (vec_calls.size() == 1) {
        void *call = vec_calls[0];
        CMarsExpression me_copy = me;
        for (auto var : me.get_vars()) {
          if (m_ast->IsArgumentInitName(var)) {
            void *actual_arg =
                m_ast->GetFuncCallParam(call, m_ast->GetFuncParamIndex(var));
            me_copy.substitute(var, CMarsExpression(actual_arg, m_ast));
          }
        }
        void *actual_arg =
            m_ast->GetFuncCallParam(call, m_ast->GetFuncParamIndex(array));
        return parse_index(actual_arg, scope, me_copy);
      }
    }
  }
  return me;
}

bool StreamPrefetchXilinx::is_unconditional_access(void *single_access,
                                                   void *scope) {
  vector<pair<void *, void *>> vec_loopifs_with_last;
  m_ast->get_surrounding_control_stmt(single_access, scope,
                                      &vec_loopifs_with_last);
  for (auto one : vec_loopifs_with_last) {
    void *ctr_stmt = one.first;
    void *last_pos = one.second;
    if (m_ast->IsForStatement(ctr_stmt)) {
      continue;
    } else if (m_ast->IsIfStatement(ctr_stmt)) {
      if (MarsProgramAnalysis::is_exact_condition(
              m_ast, m_ast->GetIfStmtCondition(ctr_stmt), scope,
              m_ast->IsInScope(last_pos, m_ast->GetIfStmtTrueBody(ctr_stmt))))
        continue;
    }
    return false;
  }
  return true;
}

void *StreamPrefetchXilinx::get_sequential_scope(
    const CMarsExpression &index, void *pos, void *scope,
    const vector<pair<void *, void *>> &vec_loops, int64_t start_range,
    vector<void *> *vec_atoms, CMarsRangeExpr *res_range) {
  int64_t curr_range = start_range;
  void *top_sequential_scope = m_ast->GetScope(pos);
  CMarsExpression curr_index = index;
  for (auto one : vec_loops) {
    auto loop = one.first;
    auto loop_pos = one.second;
    CMarsExpression coeff = index.get_coeff(loop);
    if (coeff == 0)
      break;
    if (coeff.IsConstant() == 0) {
      break;
    }
    int64_t step = coeff.GetConstant();
    if (step != curr_range)
      break;
    int64_t nStep = 0;
    if (m_ast->GetLoopStep(loop, &nStep) == 0 || nStep != 1) {
      break;
    }
    CMarsRangeExpr mr_var(
        CMarsVariable(loop, m_ast, loop_pos, scope).get_range());
    mr_var.refine_range_in_scope(scope, pos);
    if (mr_var.is_const_bound() == 0 || mr_var.is_exact() == 0) {
      break;
    }
    int64_t n_lb = mr_var.get_const_lb();
    int64_t n_ub = mr_var.get_const_ub();
    m_loop_info[loop] =
        std::make_tuple(n_lb, n_ub, nStep, (n_ub - n_lb + nStep) / nStep);
    curr_index = curr_index + n_lb * step;
    curr_range = (n_ub - n_lb + 1) * step;
    top_sequential_scope = loop;
    curr_index.substitute(loop, CMarsExpression(m_ast, 0));
    vec_atoms->push_back(loop);
  }
  *res_range = CMarsRangeExpr(curr_index, curr_index + curr_range - 1, m_ast);
  return top_sequential_scope;
}

bool StreamPrefetchXilinx::check_parallel_factor_matched_with_wide_bus_factor(
    int total_parallel_factor, int *wide_bus_factor,
    bool specified_opt_bitwidth) {
  int orig_wide_bus_factor = *wide_bus_factor;
  int new_wide_bus_factor = orig_wide_bus_factor;
  while (new_wide_bus_factor > 1) {
    if (total_parallel_factor == new_wide_bus_factor)
      return true;
    if (total_parallel_factor < new_wide_bus_factor &&
        (new_wide_bus_factor % total_parallel_factor) == 0) {
      if (!specified_opt_bitwidth) {
        new_wide_bus_factor = total_parallel_factor;
        *wide_bus_factor = new_wide_bus_factor;
      }
      return true;
    }
    if (total_parallel_factor > new_wide_bus_factor &&
        (total_parallel_factor % new_wide_bus_factor) == 0)
      return true;
    // adjust wide bus factor so that it can be compatible with parallel factor
    new_wide_bus_factor >>= 1;
    if (!specified_opt_bitwidth) {
      *wide_bus_factor = new_wide_bus_factor;
    } else {
      return false;
    }
  }
  return true;
}

void *StreamPrefetchXilinx::createStreamingFunction(
    void *arg, const string &new_func_name, const CMarsExpression &me_start,
    const CMarsExpression &me_length, void *new_channel, int channel_size,
    const vector<int> &vec_parallel_factor, int wide_bus_factor, bool read_only,
    void *scope,
    const vector<tuple<void *, int64_t, int64_t, int64_t, bool>>
        &non_sequentil_loops) {
  void *arg_type = m_ast->GetTypebyVar(arg);
  void *new_arg = m_ast->CreateVariable(arg_type, m_ast->GetVariableName(arg));
  void *new_func = m_ast->CreateFuncDecl("void", new_func_name,
                                         std::vector<void *>(1, new_arg), scope,
                                         true, nullptr);
  assert(me_length.IsConstant());
  int iter_suffix = 0;
  void *func_body = m_ast->GetFuncBody(new_func);
  m_ast->AppendChild(func_body,
                     m_ast->CreatePragma("HLS INLINE off", func_body));
  void *inner_body = func_body;
  map<void *, void *> old_loop2new_iter;
  if (!non_sequentil_loops.empty()) {
    for (size_t i = non_sequentil_loops.size(); i > 0; --i) {
      auto loop_info = non_sequentil_loops[i - 1];
      void *old_loop = std::get<0>(loop_info);
      int64_t lower_bound = std::get<1>(loop_info);
      int64_t upper_bound = std::get<2>(loop_info);
      int64_t step = std::get<3>(loop_info);
      int64_t hasIncrementalIterationSpace = std::get<4>(loop_info);
      void *reuse_index = m_ast->CreateVariableDecl(
          "long", "merlin_it_" + my_itoa(iter_suffix++),
          m_ast->CreateConst(lower_bound), inner_body);
      void *reuse_loop_body = m_ast->CreateBasicBlock();
      int cmp_op = 0;
      if (hasIncrementalIterationSpace) {
        cmp_op = V_SgLessOrEqualOp;
      } else {
        cmp_op = V_SgGreaterOrEqualOp;
      }
      void *test_stmt = m_ast->CreateStmt(
          V_SgExprStatement,
          m_ast->CreateExp(cmp_op, m_ast->CreateVariableRef(reuse_index),
                           m_ast->CreateConst(upper_bound)));
      void *incr_exp = m_ast->CreateExp(V_SgPlusAssignOp,
                                        m_ast->CreateVariableRef(reuse_index),
                                        m_ast->CreateConst(step));

      void *reuse_loop = m_ast->CreateForLoop(reuse_index, test_stmt, incr_exp,
                                              reuse_loop_body);
      m_ast->AppendChild(inner_body, reuse_loop);
      inner_body = reuse_loop_body;
      old_loop2new_iter[old_loop] = reuse_index;
    }
  }
  void *stream_index = m_ast->CreateVariableDecl(
      "long", "merlin_it_" + my_itoa(iter_suffix++), nullptr, inner_body);
  m_ast->AppendChild(inner_body, stream_index);
  void *streaming_loop_body = m_ast->CreateBasicBlock();
  void *streaming_loop = m_ast->CreateStmt(
      V_SgForStatement, m_ast->GetVariableInitializedName(stream_index),
      m_ast->CreateConst(0L),
      m_ast->CreateConst(me_length.GetConstant() / wide_bus_factor),
      streaming_loop_body, nullptr, nullptr, nullptr);
  m_ast->AppendChild(inner_body, streaming_loop);
  m_ast->AppendChild(
      streaming_loop_body,
      m_ast->CreatePragma("ACCEL pipeline", streaming_loop_body));
  int total_factor = 1;
  for (auto factor : vec_parallel_factor) {
    total_factor *= factor;
  }
  void *arg_pntr = nullptr;
  void *channel_pntr = nullptr;
  inner_body = streaming_loop_body;
  if (total_factor >= wide_bus_factor || total_factor == 1) {
    int stream_parallel_factor = 1;
    void *mod_result = nullptr;
    if (channel_size > wide_bus_factor) {
      assert(channel_size % wide_bus_factor == 0);
      stream_parallel_factor = channel_size / wide_bus_factor;
      mod_result = m_ast->CreateVariableDecl(
          "long", m_ast->GetVariableName(stream_index) + "_mod",
          m_ast->CreateExp(V_SgModOp, m_ast->CreateVariableRef(stream_index),
                           m_ast->CreateConst(stream_parallel_factor)),
          inner_body);
      m_ast->AppendChild(inner_body, mod_result);
    }
    void *wide_bus_index = m_ast->CreateVariableDecl(
        "long", "merlin_it_" + my_itoa(iter_suffix++), nullptr, inner_body);
    m_ast->AppendChild(inner_body, wide_bus_index);
    void *wide_bus_loop_body = m_ast->CreateBasicBlock();
    void *wide_bus_loop = m_ast->CreateStmt(
        V_SgForStatement, m_ast->GetVariableInitializedName(wide_bus_index),
        m_ast->CreateConst(0L), m_ast->CreateConst(wide_bus_factor),
        wide_bus_loop_body, nullptr, nullptr, nullptr);
    m_ast->AppendChild(inner_body, wide_bus_loop);
    m_ast->AppendChild(
        wide_bus_loop_body,
        m_ast->CreatePragma("ACCEL parallel", wide_bus_loop_body));
    inner_body = wide_bus_loop_body;
    void *base_type = m_ast->GetBaseType(arg_type, false);
    void *tmp_var = m_ast->CreateVariableDecl(
        base_type, "merlin_tmp_" + my_itoa(iter_suffix++), nullptr, inner_body);
    m_ast->AppendChild(inner_body, tmp_var);
    void *arg_index = nullptr;
    bool lift_res = lift_start_address(me_start, scope, false,
                                       &old_loop2new_iter, &arg_index, nullptr);
    assert(lift_res);
    arg_index = m_ast->CreateExp(
        V_SgAddOp, arg_index,
        m_ast->CreateExp(V_SgMultiplyOp, m_ast->CreateVariableRef(stream_index),
                         m_ast->CreateConst(wide_bus_factor)));
    arg_index = m_ast->CreateExp(V_SgAddOp, arg_index,
                                 m_ast->CreateVariableRef(wide_bus_index));
    arg_pntr = m_ast->CreateExp(V_SgPntrArrRefExp,
                                m_ast->CreateVariableRef(new_arg), arg_index);
    void *outmost_inner_body = inner_body;
    if (read_only) {
      void *read_stmt = m_ast->CreateStmt(
          V_SgAssignStatement, m_ast->CreateVariableRef(tmp_var), arg_pntr);
      m_ast->AppendChild(outmost_inner_body, read_stmt);
    }
    for (int i = 0; i < stream_parallel_factor; ++i) {
      void *next_inner_body = nullptr;
      if (stream_parallel_factor > 1 && i + 1 < stream_parallel_factor) {
        void *true_body = m_ast->CreateBasicBlock();
        void *false_body = m_ast->CreateBasicBlock();
        void *if_stmt = m_ast->CreateIfStmt(
            m_ast->CreateExp(V_SgEqualityOp,
                             m_ast->CreateVariableRef(mod_result),
                             m_ast->CreateConst(i)),
            true_body, false_body);
        m_ast->AppendChild(inner_body, if_stmt);
        inner_body = true_body;
        next_inner_body = false_body;
      }
      if (channel_size == 1) {
        channel_pntr = m_ast->CreateVariableRef(new_channel);
      } else {
        void *channel_index = m_ast->CreateVariableRef(wide_bus_index);
        if (stream_parallel_factor > 1) {
          channel_index =
              m_ast->CreateExp(V_SgAddOp, channel_index,
                               m_ast->CreateConst(i * wide_bus_factor));
        }
        channel_pntr = m_ast->CreateExp(V_SgPntrArrRefExp,
                                        m_ast->CreateVariableRef(new_channel),
                                        channel_index);
      }
      generate_channel_access(inner_body, channel_pntr,
                              m_ast->CreateVariableRef(tmp_var), arg, scope,
                              read_only);
      if (next_inner_body != nullptr)
        inner_body = next_inner_body;
    }
    if (!read_only) {
      void *write_stmt = m_ast->CreateStmt(V_SgAssignStatement, arg_pntr,
                                           m_ast->CreateVariableRef(tmp_var));
      m_ast->AppendChild(outmost_inner_body, write_stmt);
    }
  } else {
    void *wide_bus_index = m_ast->CreateVariableDecl(
        "long", "merlin_it_" + my_itoa(iter_suffix++), nullptr, inner_body);
    m_ast->AppendChild(inner_body, wide_bus_index);
    void *wide_bus_loop_body = m_ast->CreateBasicBlock();
    void *wide_bus_loop = m_ast->CreateStmt(
        V_SgForStatement, m_ast->GetVariableInitializedName(wide_bus_index),
        m_ast->CreateConst(0L),
        m_ast->CreateConst(wide_bus_factor / total_factor), wide_bus_loop_body,
        nullptr, nullptr, nullptr);
    m_ast->AppendChild(inner_body, wide_bus_loop);
    m_ast->AppendChild(
        wide_bus_loop_body,
        m_ast->CreatePragma("ACCEL parallel", wide_bus_loop_body));
    void *parallel_loop_body = m_ast->CreateBasicBlock();
    void *parallel_index = m_ast->CreateVariableDecl(
        "long", "merlin_it_" + my_itoa(iter_suffix++), nullptr, inner_body);
    m_ast->AppendChild(inner_body, parallel_index);
    void *parallel_loop = m_ast->CreateStmt(
        V_SgForStatement, m_ast->GetVariableInitializedName(parallel_index),
        m_ast->CreateConst(0L), m_ast->CreateConst(total_factor),
        parallel_loop_body, nullptr, nullptr, nullptr);
    m_ast->AppendChild(inner_body, parallel_loop);
    m_ast->AppendChild(
        parallel_loop_body,
        m_ast->CreatePragma("ACCEL parallel", parallel_loop_body));
    inner_body = parallel_loop_body;
    void *start_address = nullptr;
    bool lift_res = lift_start_address(
        me_start, scope, false, &old_loop2new_iter, &start_address, nullptr);
    assert(lift_res);
    void *arg_index = m_ast->CreateExp(
        V_SgAddOp, start_address,
        m_ast->CreateExp(
            V_SgAddOp, m_ast->CreateVariableRef(parallel_index),
            m_ast->CreateExp(
                V_SgAddOp,
                m_ast->CreateExp(V_SgMultiplyOp,
                                 m_ast->CreateVariableRef(wide_bus_index),
                                 m_ast->CreateConst(total_factor)),
                m_ast->CreateExp(V_SgMultiplyOp,
                                 m_ast->CreateVariableRef(stream_index),
                                 m_ast->CreateConst(wide_bus_factor)))));
    arg_pntr = m_ast->CreateExp(V_SgPntrArrRefExp,
                                m_ast->CreateVariableRef(new_arg), arg_index);
    channel_pntr = m_ast->CreateExp(V_SgPntrArrRefExp,
                                    m_ast->CreateVariableRef(new_channel),
                                    m_ast->CreateVariableRef(parallel_index));
    generate_channel_access(inner_body, channel_pntr, arg_pntr, arg, scope,
                            read_only);
  }
  return new_func;
}

void StreamPrefetchXilinx::generate_channel_access(void *inner_body,
                                                   void *channel_pntr,
                                                   void *arg_pntr, void *arg,
                                                   void *scope,
                                                   bool read_only) {
  if (read_only) {
    void *dummy_write_func = get_stream_write_function(
        m_ast->GetBaseType(m_ast->GetTypebyVar(arg), false), scope);
    channel_pntr = m_ast->CreateExp(V_SgDotExp, channel_pntr,
                                    m_ast->CreateFunctionRef(dummy_write_func));
    void *assign_stmt =
        m_ast->CreateFuncCallStmt(channel_pntr, vector<void *>(1, arg_pntr));
    m_ast->AppendChild(inner_body, assign_stmt);
  } else {
    void *dummy_read_func = get_stream_read_function(
        m_ast->GetBaseType(m_ast->GetTypebyVar(arg), false), scope);
    channel_pntr = m_ast->CreateExp(V_SgDotExp, channel_pntr,
                                    m_ast->CreateFunctionRef(dummy_read_func));
    void *assign_stmt = m_ast->CreateStmt(
        V_SgAssignStatement, arg_pntr,
        m_ast->CreateFuncCall(channel_pntr, vector<void *>{}));
    m_ast->AppendChild(inner_body, assign_stmt);
  }
}

void *StreamPrefetchXilinx::get_stream_write_function(void *input_type,
                                                      void *scope) {
  if (m_stream_write_function[scope].count(input_type) > 0)
    return m_stream_write_function[scope][input_type];
  if (m_stream_class.count(scope) <= 0) {
    m_stream_class[scope] = SageBuilder::buildStructDeclaration(
        "__merlin_hls_stream",
        isSgScopeStatement(static_cast<SgNode *>(scope)));
  }
  void *write_function = m_ast->CreateMemberFuncDecl(
      "void", "write",
      vector<void *>(1, m_ast->CreateVariable(input_type, "data")),
      m_ast->GetClassDefinition(m_stream_class[scope]), false, nullptr);
  m_stream_write_function[scope][input_type] = write_function;
  return write_function;
}

void *StreamPrefetchXilinx::get_stream_read_function(void *return_type,
                                                     void *scope) {
  if (m_stream_read_function[scope].count(return_type) > 0)
    return m_stream_read_function[scope][return_type];
  if (m_stream_class.count(scope) <= 0) {
    m_stream_class[scope] = SageBuilder::buildStructDeclaration(
        "__merlin_hls_stream",
        isSgScopeStatement(static_cast<SgNode *>(scope)));
  }
  void *read_function = m_ast->CreateMemberFuncDecl(
      return_type, "read", vector<void *>{},
      m_ast->GetClassDefinition(m_stream_class[scope]), false, nullptr);
  m_stream_read_function[scope][return_type] = read_function;
  return read_function;
}

void StreamPrefetchXilinx::replaceSingleAccessWithChannel(
    void *single_access, void *assign_exp, void *assign_val, void *new_channel,
    const vector<void *> &vec_parallel_loops, int channel_size, bool read_only,
    void *scope) {
  void *channel_index = nullptr;
  if (channel_size > 1) {
    int64_t total_range = 1;
    for (auto loop : vec_parallel_loops) {
      void *loop_iter;
      int64_t c_start, c_end, c_step, trip_count;
      int ret = get_canonical_loop_info(loop, &loop_iter, &c_start, &c_end,
                                        &c_step, &trip_count);
      assert(ret);
      // support cross function loop iterator
      loop_iter =
          pass_loop_iterator_cross_function(loop, loop_iter, single_access);
      void *iter_ref = m_ast->CreateVariableRef(loop_iter);
      if (c_start != 0) {
        iter_ref = m_ast->CreateExp(V_SgSubtractOp, iter_ref,
                                    m_ast->CreateConst(c_start));
      }
      if (c_step != 1) {
        iter_ref = m_ast->CreateExp(V_SgDivideOp, iter_ref,
                                    m_ast->CreateConst(c_step));
      }
      if (channel_index == nullptr)
        channel_index = iter_ref;
      else
        channel_index =
            m_ast->CreateExp(V_SgAddOp, channel_index,
                             m_ast->CreateExp(V_SgMultiplyOp, iter_ref,
                                              m_ast->CreateConst(total_range)));
      total_range *= trip_count;
      if (total_range == channel_size) {
        break;
      }
      if ((total_range > channel_size) && (total_range % channel_size) == 0) {
        channel_index = m_ast->CreateExp(V_SgModOp, channel_index,
                                         m_ast->CreateConst(channel_size));
        break;
      }
    }
  }
  void *basic_type =
      m_ast->GetBaseType(m_ast->GetTypeByExp(single_access), false);
  if (read_only) {
    void *dummy_read_func = get_stream_read_function(basic_type, scope);
    void *channel_pntr = nullptr;
    if (channel_index != nullptr)
      channel_pntr = m_ast->CreateExp(V_SgPntrArrRefExp,
                                      m_ast->CreateVariableRef(new_channel),
                                      channel_index);
    else
      channel_pntr = m_ast->CreateVariableRef(new_channel);
    channel_pntr = m_ast->CreateExp(V_SgDotExp, channel_pntr,
                                    m_ast->CreateFunctionRef(dummy_read_func));
    void *new_access = m_ast->CreateFuncCall(channel_pntr, vector<void *>{});
    m_ast->ReplaceExp(single_access, new_access);
  } else {
    void *dummy_write_func = get_stream_write_function(basic_type, scope);
    void *channel_pntr = nullptr;
    if (channel_index != nullptr)
      channel_pntr = m_ast->CreateExp(V_SgPntrArrRefExp,
                                      m_ast->CreateVariableRef(new_channel),
                                      channel_index);
    else
      channel_pntr = m_ast->CreateVariableRef(new_channel);
    channel_pntr = m_ast->CreateExp(V_SgDotExp, channel_pntr,
                                    m_ast->CreateFunctionRef(dummy_write_func));
    m_ast->detachFromParent(static_cast<SgNode *>(assign_val));
    void *new_access =
        m_ast->CreateFuncCall(channel_pntr, vector<void *>(1, assign_val));
    m_ast->ReplaceExp(assign_exp, new_access);
  }
}

bool StreamPrefetchXilinx::is_constant_and_aligned_address_and_length(
    const CMarsRangeExpr &mr_range, CMarsExpression *me_start,
    CMarsExpression *me_length, int wide_bus_factor) {
  CMarsExpression me_lb, me_ub;
  int ret = mr_range.get_simple_bound(&me_lb, &me_ub);
  if (ret == 0) {
    return false;
  }
  if (divisible(me_lb, wide_bus_factor, false) == 0) {
    return false;
  }
  CMarsExpression me_len = me_ub - me_lb + 1;
  if (me_len.IsConstant() == 0) {
    return false;
  }
  if (divisible(me_len, wide_bus_factor, false) == 0) {
    return false;
  }
  *me_start = me_lb;
  *me_length = me_len;
  return true;
}

void StreamPrefetchXilinx::clean_interface_pragma(void *computation_kernel) {
  void *func_body = m_ast->GetFuncBody(computation_kernel);
  m_ast->PrependChild(func_body,
                      m_ast->CreatePragma("HLS inline off", func_body));
  vector<void *> vec_pragmas;
  m_ast->GetNodesByType_int(func_body, "preorder", V_SgPragmaDeclaration,
                            &vec_pragmas);
  for (auto pragma : vec_pragmas) {
    CAnalPragma ana(m_ast);
    bool errorReport = false;
    if (ana.PragmasFrontendProcessing(pragma, &errorReport, false, true)) {
      if (ana.is_interface()) {
        m_ast->RemoveStmt(pragma);
      }
    }
  }
}

void StreamPrefetchXilinx::remove_all_logic_but_keep_interface_pragma(
    void *kernel) {
  void *func_body = m_ast->GetFuncBody(kernel);
  vector<void *> vec_vars;
  unordered_set<void *> used_vars;
  for (auto stmt : m_ast->GetChildStmts(func_body)) {
    if (m_ast->IsPragmaDecl(stmt)) {
      CAnalPragma ana(m_ast);
      bool errorReport = false;
      if (ana.PragmasFrontendProcessing(stmt, &errorReport, false, true)) {
        if (ana.is_interface()) {
          auto res = m_ast->GetVariablesUsedByPragma(stmt, &errorReport,
                                                     nullptr, false);
          for (auto var : res) {
            used_vars.insert(var.first);
          }
          continue;
        }
      }
    }
    if (m_ast->IsExprStatement(stmt)) {
      void *func_call = m_ast->GetExprFromStmt(stmt);
      if (m_ast->IsFunctionCall(func_call) &&
          m_ast->GetFuncNameByCall(func_call) == "__merlin_access_range") {
        continue;
      }
    }
    if (m_ast->IsVariableDecl(stmt)) {
      vec_vars.push_back(stmt);
      continue;
    }
    m_ast->RemoveStmt(stmt);
  }
  // keep variables used by interface pragma
  for (auto var_decl : vec_vars) {
    auto var = m_ast->GetVariableInitializedName(var_decl);
    if (used_vars.count(var) > 0)
      continue;
    m_ast->RemoveStmt(var_decl);
  }
}

// disable memory burst for this port
// enable explict bundle for this port
void StreamPrefetchXilinx::update_interface_pragma(
    void *arg, void *kernel, int bus_bitwidth, const CMarsExpression &me_start,
    const CMarsExpression &me_length, void *top_sequential_scope) {
  vector<void *> vec_pragmas;
  m_ast->GetNodesByType_int(kernel, "preorder", V_SgPragmaDeclaration,
                            &vec_pragmas);
  string var_name = m_ast->GetVariableName(arg);
  vector<void *> vec_interface_pragma;
  for (auto pragma : vec_pragmas) {
    if (mars_ir_v2.get_pragma_attribute(pragma, "interface") != "") {
      if (mars_ir_v2.get_pragma_attribute(pragma, "variable") == var_name) {
        vec_interface_pragma.push_back(pragma);
      }
    }
  }
  if (vec_interface_pragma.empty()) {
    void *func_body = m_ast->GetFuncBody(kernel);
    if (func_body != nullptr) {
      void *new_pragma = m_ast->CreatePragma(
          "ACCEL interface variable=" + var_name, func_body);
      m_ast->AppendChild(func_body, new_pragma);
      vec_interface_pragma.push_back(new_pragma);
    }
  }

  for (auto pragma : vec_interface_pragma) {
    if (!m_analysis) {
      mars_ir_v2.set_pragma_attribute(&pragma, CMOST_burst_off, "");
      mars_ir_v2.set_pragma_attribute(&pragma, CMOST_explict_bundle, "");
      mars_ir_v2.set_pragma_attribute(&pragma, CMOST_bus_bitwidth,
                                      my_itoa(bus_bitwidth));
    } else {
      if (m_ast->TraceUpToFuncDecl(top_sequential_scope) == kernel) {
        vector<void *> vec_args;
        vec_args.push_back(m_ast->CreateVariableRef(arg));
        vec_args.push_back(me_start.get_expr_from_coeff());
        vec_args.push_back(me_length.get_expr_from_coeff());
        void *streaming_func = m_ast->CreateFuncCall(
            "__merlin_streaming_range", m_ast->GetTypeByString("void"),
            vec_args, top_sequential_scope);
        void *func_call_stmt =
            m_ast->CreateStmt(V_SgExprStatement, streaming_func);
        if (m_ast->IsForStatement(top_sequential_scope)) {
          m_ast->InsertStmt(func_call_stmt, top_sequential_scope);
        } else if (m_ast->IsBasicBlock(top_sequential_scope)) {
          m_ast->AppendChild(top_sequential_scope, func_call_stmt);
        } else {
          DEBUG(cout << "found an unsupported scope\n");
        }
        void *global_scope = m_ast->GetGlobal(kernel);
        static set<void *> global_with_header;
        if (global_with_header.count(global_scope) <= 0) {
          m_ast->AddHeader("\n#include \"cmost.h\"\n", global_scope);
          global_with_header.insert(global_scope);
        }
      } else {
        // FIXME: support sub function
        DEBUG(cout << "streaming scope is in sub function\n");
      }
    }
  }
}

bool StreamPrefetchXilinx::check_streaming_access(
    void *arg, void *single_access, void *scope, bool report,
    CMarsRangeExpr *mr_range, bool *read_only,
    vector<void *> *vec_parallel_loops, vector<int> *vec_parallel_factor,
    vector<std::tuple<void *, int64_t, int64_t, int64_t, bool>>
        *non_sequentil_loops,
    void **top_sequential_scope) {
  string arg_name = m_ast->GetVariableName(arg);
  auto arg_user_info = getUserCodeInfo(m_ast, arg);
  if (!arg_user_info.name.empty()) {
    arg_name = arg_user_info.name;
  }
  string port_info = "'" + arg_name + "' (" + arg_user_info.file_location + ")";
  ArrayRangeAnalysis mar(arg, m_ast, single_access, scope, false, true);
  bool ret = true;
  if (mar.has_read() && mar.has_write()) {
    DEBUG(cout << "cannot support read-write access\n");
    ret = false;
  }
  if (mar.has_read()) {
    *read_only = true;
  } else {
    *read_only = false;
  }

  if (ret && !is_unconditional_access(single_access, scope)) {
    DEBUG(cout << "not satisfy unconditional access checking\n");
    ret = false;
  }
  CMarsExpression index =
      parse_index(single_access, scope, CMarsExpression(m_ast, 0));
  vector<pair<void *, void *>> vec_loops;
  if (ret && !m_ast->get_loop_nest_in_scope(single_access, scope, &vec_loops)) {
    DEBUG(cout << "can not get nest loops\n");
    ret = false;
  }
  // original 'vec_loops' is from outmost to inner
  std::reverse(vec_loops.begin(), vec_loops.end());
  vector<void *> vec_atoms;
  set<void *> start_index_releated_loops;
  int64_t stream_length = -1;
  if (ret) {
    *top_sequential_scope = get_sequential_scope(
        index, single_access, scope, vec_loops, 1, &vec_atoms, mr_range);
    CMarsExpression start_index, upper_bound;
    ret = mr_range->get_simple_bound(&start_index, &upper_bound);
    if (ret && !lift_start_address(start_index, scope, true, nullptr, nullptr,
                                   &start_index_releated_loops)) {
      DEBUG(cout << "can not lift the start address\n");
      ret = false;
    } else {
      auto me_length = upper_bound - start_index + 1;
      if (me_length.IsConstant()) {
        stream_length = me_length.GetConstant();
      }
    }
  }
  if (!ret) {
    if (report) {
      dump_critical_message(STREAM_WARNING_4(port_info));
    }
  }
  set<void *> related_loops(vec_atoms.begin(), vec_atoms.end());
  bool unsupported_data_reuse = false;
  bool cg_parallel = false;
  bool partial_unroll = false;
  string loop_info_str;
  for (size_t i = 0; i < vec_loops.size(); ++i) {
    auto loop = vec_loops[i].first;
    auto loop_pos = vec_loops[i].second;
    int64_t trip_count = -1;
    int64_t n_lb = -1;
    int64_t n_ub = -1;
    int64_t n_step = -1;
    bool valid_trip_count =
        get_and_analyze_loop_info(loop, loop_pos, single_access, scope, &n_lb,
                                  &n_ub, &n_step, &trip_count);
    loop_info_str = getUserCodeFileLocation(m_ast, loop, true);
    CMirNode *node = mars_ir.get_node(m_ast->GetLoopBody(loop));
    if (node) {
      CMarsLoop *loop_info = mars_ir_v2.get_loop_info(loop);
      if (loop_info) {
        int factor = 0;
        if (loop_info->is_complete_unroll()) {
          if (valid_trip_count) {
            if (node->is_fine_grain) {
              vec_parallel_factor->push_back(trip_count);
              vec_parallel_loops->push_back(loop);
            } else {
              cg_parallel = true;
              break;
            }
          } else {
            partial_unroll = true;
            break;
          }
        } else if (loop_info->is_partial_unroll(&factor) && factor > 1) {
          if (node->is_fine_grain) {
            if (valid_trip_count && (trip_count % factor == 0)) {
              vec_parallel_factor->push_back(factor);
              vec_parallel_loops->push_back(loop);
            } else {
              partial_unroll = true;
              break;
            }
          } else {
            cg_parallel = true;
            break;
          }
        }
      }
    }
    if (related_loops.count(loop) <= 0) {
      void *ivar = nullptr, *lb = nullptr, *ub = nullptr, *step = nullptr;
      bool hasIncrementalIterationSpace = false, isInclusiveUpperBound = false;
      bool constant_canonical_loop = false;
      if (m_ast->IsCanonicalForLoop(loop, &ivar, &lb, &ub, &step, nullptr,
                                    &hasIncrementalIterationSpace,
                                    &isInclusiveUpperBound)) {
        CMarsExpression me_lb(lb, m_ast);
        CMarsExpression me_ub(ub, m_ast);
        CMarsExpression me_step(step, m_ast);
        if (me_lb.IsConstant() && me_ub.IsConstant() && me_step.IsConstant()) {
          int64_t c_lb = me_lb.GetConstant();
          int64_t c_ub = me_ub.GetConstant();
          if (!isInclusiveUpperBound)
            c_ub--;
          if (c_lb <= c_ub) {
            c_lb = std::max(c_lb, n_lb);
            c_ub = std::min(c_ub, n_ub);
          } else {
            c_lb = std::min(c_lb, n_ub);
            c_ub = std::max(c_ub, n_lb);
          }
          non_sequentil_loops->push_back(std::make_tuple(
              loop, c_lb, c_ub, n_step, hasIncrementalIterationSpace));
          constant_canonical_loop = true;
        }
      }
      if (!constant_canonical_loop) {
        unsupported_data_reuse = true;
        break;
      }
      if (start_index_releated_loops.count(loop) <= 0) {
        if (stream_length < m_burst_single_threshold) {
          // for small streaming length, if it is possible to be cached, leave
          // it for caching
          unsupported_data_reuse = true;
          break;
        }
      }
    }
  }
  if (unsupported_data_reuse || cg_parallel || partial_unroll) {
    if (unsupported_data_reuse) {
      DEBUG(cout << "there is unsupported data reuse\n");
      if (report && !m_analysis)
        dump_critical_message(STREAM_WARNING_5(port_info, loop_info_str));
    } else if (cg_parallel) {
      DEBUG(cout << "there is coarse grained parallel\n");
      if (report && !m_analysis)
        dump_critical_message(STREAM_WARNING_6(port_info, loop_info_str));
    } else {
      DEBUG(cout << "there is indivisible partial parallel\n");
      if (report && !m_analysis)
        dump_critical_message(STREAM_WARNING_7(port_info, loop_info_str));
    }
    ret = false;
  }
  return ret;
}

void *StreamPrefetchXilinx::get_streaming_access_info_from_intrinsic(
    void *var, void *scope, void *pos, CMarsExpression *c_start,
    CMarsExpression *c_length, void **var_ref) {
  void *curr_scope = m_ast->GetScope(pos);
  do {
    if (m_ast->IsBasicBlock(curr_scope)) {
      for (auto stmt : m_ast->GetChildStmts(curr_scope)) {
        if (m_ast->IsExprStatement(stmt) == 0)
          continue;
        void *expr = m_ast->GetExprFromStmt(stmt);
        if (m_ast->IsFunctionCall(expr) == 0)
          continue;
        string func_name = m_ast->GetFuncNameByCall(expr);
        if (func_name.find("__merlin_streaming_range") != 0)
          continue;
        assert(m_ast->GetFuncCallParamNum(expr) == 3);
        void *ref = m_ast->GetFuncCallParam(expr, 0);
        if (m_ast->IsVarRefExp(ref) &&
            m_ast->GetVariableInitializedName(ref) == var) {
          *c_start = CMarsExpression(m_ast->GetFuncCallParam(expr, 1), m_ast,
                                     nullptr, curr_scope);
          *c_length = CMarsExpression(m_ast->GetFuncCallParam(expr, 2), m_ast,
                                      nullptr, curr_scope);
          *var_ref = ref;
          return expr;
        }
      }
    } else if (m_ast->IsFunctionDeclaration(curr_scope)) {
      if (m_ast->IsArgumentInitName(var) == 0)
        return nullptr;
      vector<void *> vec_calls;
      m_ast->GetFuncCallsFromDecl(curr_scope, nullptr, &vec_calls);
      if (vec_calls.size() != 1)
        return nullptr;
      int arg_idx = m_ast->GetFuncParamIndex(var);
      void *arg = m_ast->GetFuncCallParam(vec_calls[0], arg_idx);
      var = m_ast->get_array_from_pntr(arg);
      pos = vec_calls[0];
      curr_scope = m_ast->GetScope(pos);
    }
    if (curr_scope == scope)
      break;
    curr_scope = m_ast->GetParent(curr_scope);
  } while (true);

  return nullptr;
}

bool StreamPrefetchXilinx::check_streaming_access_from_intrinsic(
    void *arg, void *single_access, void *scope, bool report,
    const CMarsExpression &start, const CMarsExpression &length,
    void *new_var_ref, CMarsRangeExpr *mr_range, bool *read_only,
    vector<void *> *vec_parallel_loops, vector<int> *vec_parallel_factor,
    vector<tuple<void *, int64_t, int64_t, int64_t, bool>> *non_sequentil_loops,
    void **top_sequential_scope) {
  string arg_name = m_ast->GetVariableName(arg);
  auto arg_user_info = getUserCodeInfo(m_ast, arg);
  if (!arg_user_info.name.empty()) {
    arg_name = arg_user_info.name;
  }
  string port_info = "'" + arg_name + "' (" + arg_user_info.file_location + ")";
  ArrayRangeAnalysis mar(arg, m_ast, single_access, scope, false, true);
  bool ret = true;
  if (mar.has_read() && mar.has_write()) {
    DEBUG(cout << "cannot support read-write access\n");
    ret = false;
  }
  if (mar.has_read()) {
    *read_only = true;
  } else {
    *read_only = false;
  }

  if (ret && !is_unconditional_access(new_var_ref, scope)) {
    DEBUG(cout << "not satisfy unconditional access checking\n");
    ret = false;
  }
  CMarsExpression index;
  if (ret) {
    index = parse_index(new_var_ref, scope, start);
  }
  vector<pair<void *, void *>> vec_loops;
  if (ret && !m_ast->get_loop_nest_in_scope(new_var_ref, scope, &vec_loops)) {
    DEBUG(cout << "can not get nest loops\n");
    ret = false;
  }
  if (ret && !length.IsConstant()) {
    DEBUG(cout << "streaming access length is not constant\n");
    ret = false;
  }
  // original 'vec_loops' is from outmost to inner
  std::reverse(vec_loops.begin(), vec_loops.end());
  vector<void *> vec_atoms;
  set<void *> start_index_releated_loops;
  int64_t stream_length = -1;
  if (ret) {
    *top_sequential_scope =
        get_sequential_scope(index, new_var_ref, scope, vec_loops,
                             length.GetConstant(), &vec_atoms, mr_range);
    CMarsExpression start_index, upper_bound;
    ret = mr_range->get_simple_bound(&start_index, &upper_bound);
    if (ret && !lift_start_address(start_index, scope, true, nullptr, nullptr,
                                   &start_index_releated_loops)) {
      DEBUG(cout << "can not lift the start address\n");
      ret = false;
    } else {
      auto me_length = upper_bound - start_index + 1;
      if (me_length.IsConstant()) {
        stream_length = me_length.GetConstant();
      }
    }
  }
  if (!ret) {
    if (report) {
      dump_critical_message(STREAM_WARNING_4(port_info));
    }
    return false;
  }
  set<void *> related_loops(vec_atoms.begin(), vec_atoms.end());
  bool unsupported_data_reuse = false;
  bool cg_parallel = false;
  bool partial_unroll = false;
  string loop_info_str;
  for (size_t i = 0; i < vec_loops.size(); ++i) {
    auto loop = vec_loops[i].first;
    auto loop_pos = vec_loops[i].second;
    int64_t trip_count = -1;
    int64_t n_lb = -1;
    int64_t n_ub = -1;
    int64_t n_step = -1;
    bool valid_trip_count =
        get_and_analyze_loop_info(loop, loop_pos, single_access, scope, &n_lb,
                                  &n_ub, &n_step, &trip_count);
    loop_info_str = getUserCodeFileLocation(m_ast, loop, true);
    // TODO(youxiang): check whether the current loop is completely
    // parallel; if so, what is its trip count
    CMirNode *node = mars_ir.get_node(m_ast->GetLoopBody(loop));
    if (node && !node->is_fine_grain) {
      CMarsLoop *loop_info = mars_ir_v2.get_loop_info(loop);
      if (loop_info) {
        if (loop_info->is_complete_unroll()) {
          cg_parallel = true;
          break;
        }
      }
    }
    CMarsLoop *loop_info = mars_ir_v2.get_loop_info(loop);
    if (loop_info) {
      int factor = 0;
      if (loop_info->is_complete_unroll()) {
        if (related_loops.count(loop) > 0) {
          if (valid_trip_count) {
            vec_parallel_factor->push_back(trip_count);
            vec_parallel_loops->push_back(loop);
          } else {
            partial_unroll = true;
            break;
          }
        }
      } else if (loop_info->is_partial_unroll(&factor) && factor > 1) {
        if (related_loops.count(loop) > 0) {
          if (valid_trip_count && (trip_count % factor == 0)) {
            vec_parallel_factor->push_back(factor);
            vec_parallel_loops->push_back(loop);
          } else {
            partial_unroll = true;
            break;
          }
        }
      }
    }

    if (related_loops.count(loop) <= 0) {
      void *ivar = nullptr, *lb = nullptr, *ub = nullptr, *step = nullptr;
      bool hasIncrementalIterationSpace = false, isInclusiveUpperBound = false;
      bool constant_canonical_loop = false;
      if (m_ast->IsCanonicalForLoop(loop, &ivar, &lb, &ub, &step, nullptr,
                                    &hasIncrementalIterationSpace,
                                    &isInclusiveUpperBound)) {
        CMarsExpression me_lb(lb, m_ast);
        CMarsExpression me_ub(ub, m_ast);
        CMarsExpression me_step(step, m_ast);
        if (me_lb.IsConstant() && me_ub.IsConstant() && me_step.IsConstant()) {
          int64_t c_lb = me_lb.GetConstant();
          int64_t c_ub = me_ub.GetConstant();
          if (!isInclusiveUpperBound)
            c_ub--;
          if (c_lb <= c_ub) {
            c_lb = std::max(c_lb, n_lb);
            c_ub = std::min(c_ub, n_ub);
          } else {
            c_lb = std::min(c_lb, n_ub);
            c_ub = std::max(c_ub, n_lb);
          }
          non_sequentil_loops->push_back(std::make_tuple(
              loop, c_lb, c_ub, n_step, hasIncrementalIterationSpace));
          constant_canonical_loop = true;
        }
      }
      if (!constant_canonical_loop) {
        unsupported_data_reuse = true;
        break;
      }
      if (start_index_releated_loops.count(loop) <= 0) {
        if (stream_length < m_burst_single_threshold) {
          unsupported_data_reuse = true;
          break;
        }
      }
    }
  }
  if (unsupported_data_reuse || cg_parallel || partial_unroll) {
    if (unsupported_data_reuse) {
      DEBUG(cout << "there is unsupported data reuse\n");
      if (report && !m_analysis)
        dump_critical_message(STREAM_WARNING_5(port_info, loop_info_str));
    } else if (cg_parallel) {
      DEBUG(cout << "there is coarse grained parallel\n");
      if (report && !m_analysis)
        dump_critical_message(STREAM_WARNING_6(port_info, loop_info_str));
    } else {
      DEBUG(cout << "there is indivisible partial parallel\n");
      if (report && !m_analysis)
        dump_critical_message(STREAM_WARNING_7(port_info, loop_info_str));
    }
    ret = false;
  }
  vec_loops.clear();
  if (ret && !m_ast->get_loop_nest_in_scope(
                 single_access, m_ast->GetScope(new_var_ref), &vec_loops)) {
    DEBUG(cout << "can not get nest loops\n");
    ret = false;
  }
  std::reverse(vec_loops.begin(), vec_loops.end());
  index = parse_index(single_access, m_ast->TraceUpToFuncDecl(new_var_ref),
                      CMarsExpression(m_ast, 0));
  related_loops.clear();
  for (auto var : index.get_vars()) {
    if (m_ast->IsForStatement(var) != 0) {
      related_loops.insert(var);
    }
  }
  vector<void *> all_parallel_loops;
  vector<int> all_parallel_factor;
  for (auto one : vec_loops) {
    auto loop = one.first;
    auto loop_pos = one.second;
    if (related_loops.count(loop) <= 0)
      continue;
    int64_t trip_count = -1;
    int64_t n_lb = -1;
    int64_t n_ub = -1;
    int64_t n_step = -1;
    bool valid_trip_count =
        get_and_analyze_loop_info(loop, loop_pos, single_access, scope, &n_lb,
                                  &n_ub, &n_step, &trip_count);
    loop_info_str = getUserCodeFileLocation(m_ast, loop, true);
    // TODO(youxiang): check whether the current loop is completely
    // parallel; if so, what is its trip count
    CMarsLoop *loop_info = mars_ir_v2.get_loop_info(loop);
    if (loop_info) {
      int factor = 0;
      if (loop_info->is_complete_unroll()) {
        if (valid_trip_count) {
          all_parallel_factor.push_back(trip_count);
          all_parallel_loops.push_back(loop);
        } else {
          partial_unroll = true;
          break;
        }
      } else if (loop_info->is_partial_unroll(&factor) && factor > 1) {
        if (valid_trip_count && (trip_count % factor == 0)) {
          all_parallel_factor.push_back(factor);
          all_parallel_loops.push_back(loop);
        } else {
          partial_unroll = true;
          break;
        }
      }
    }
  }
  if (partial_unroll) {
    DEBUG(cout << "there is partial parallel\n");
    if (report && !m_analysis) {
      dump_critical_message(STREAM_WARNING_7(port_info, loop_info_str));
    }
    ret = false;
  }
  for (size_t i = 0; i < vec_parallel_factor->size(); ++i) {
    all_parallel_loops.push_back((*vec_parallel_loops)[i]);
    all_parallel_factor.push_back((*vec_parallel_factor)[i]);
  }
  vec_parallel_loops->swap(all_parallel_loops);
  vec_parallel_factor->swap(all_parallel_factor);
  return ret;
}

bool StreamPrefetchXilinx::get_canonical_loop_info(void *loop, void **iter,
                                                   int64_t *i_lb, int64_t *i_ub,
                                                   int64_t *c_step,
                                                   int64_t *trip_count) {
  void *loop_init = m_ast->GetLoopIterator(loop);
  if (loop_init != nullptr) {
    *iter = loop_init;
    if (m_loop_info.count(loop) > 0) {
      auto res = m_loop_info[loop];
      *i_lb = std::get<0>(res);
      *i_ub = std::get<1>(res);
      *c_step = std::get<2>(res);
      *trip_count = std::get<3>(res);
      return true;
    }
  }
  return false;
}

bool StreamPrefetchXilinx::is_constant_bound_canonical_loop(void *loop) {
  if (m_loop_info.count(loop) > 0)
    return true;
  return false;
}

void *StreamPrefetchXilinx::pass_loop_iterator_cross_function(
    void *loop, void *loop_iter, void *single_access) {
  if (m_ast->TraceUpToFuncDecl(loop) ==
      m_ast->TraceUpToFuncDecl(single_access)) {
    return loop_iter;
  }
  std::list<t_func_call_path> vec_path;
  m_ast->get_call_paths(single_access, loop, &vec_path);
  assert(vec_path.size() == 1);
  DEBUG(cout << m_ast->print_func_path(*vec_path.begin()) << '\n');
  auto func_path = *vec_path.begin();
  std::reverse(func_path.begin(), func_path.end());
  void *iter_type = m_ast->GetTypebyVar(loop_iter);
  void *new_loop_iter = loop_iter;
  string iter_name = m_ast->GetVariableName(loop_iter);
  auto it = func_path.begin();
  it++;
  while (it != func_path.end()) {
    auto it_next = it;
    it_next++;
    if (it_next != func_path.end()) {
      void *func = it->first;
      void *call = it->second;
      m_ast->insert_argument_to_call(call,
                                     m_ast->CreateVariableRef(new_loop_iter));
      string new_iter_name = iter_name;
      m_ast->get_valid_local_name(func, &new_iter_name);
      new_loop_iter = m_ast->CreateVariable(iter_type, new_iter_name);
      m_ast->insert_param_to_decl(func, new_loop_iter, true);
      void *func_body = m_ast->GetFuncBody(func);
      m_ast->AppendChild(
          func_body,
          m_ast->CreatePragma(
              "HLS function_instantiate variable=" + new_iter_name, func_body));
    }
    it++;
  }
  return new_loop_iter;
}

bool StreamPrefetchXilinx::lift_start_address(
    const CMarsExpression &start_index, void *scope, bool check,
    map<void *, void *> *old_loop2new_var, void **new_address,
    set<void *> *non_continuous_loops) {
  if (start_index.IsConstant()) {
    if (new_address != nullptr)
      *new_address = start_index.get_expr_from_coeff();
    return true;
  }
  CMarsExpression me_copy = start_index;
  for (auto atom : start_index.get_vars()) {
    if (m_ast->IsForStatement(atom)) {
      if (non_continuous_loops != nullptr) {
        non_continuous_loops->insert(atom);
      }
      if (!check) {
        me_copy.set_coeff(
            m_ast->GetVariableInitializedName((*old_loop2new_var)[atom]),
            start_index.get_coeff(atom));
        me_copy.set_coeff(atom, CMarsExpression(m_ast, 0));
      }
      continue;
    }
    if (m_ast->IsExpression(atom)) {
      void *exp = atom;
      if (m_ast->IsInitializer(atom)) {
        exp = m_ast->GetInitializerOperand(atom);
      }
      void *copy_var = nullptr;
      if (!check) {
        copy_var = m_ast->CopyExp(exp);
      }
      vector<void *> vec_refs;
      vector<void *> vec_copy_refs;
      m_ast->GetNodesByType_int(exp, "preorder", V_SgVarRefExp, &vec_refs);
      m_ast->GetNodesByType_int(copy_var, "preorder", V_SgVarRefExp,
                                &vec_copy_refs);
      for (size_t i = 0; i < vec_refs.size(); ++i) {
        void *ref = vec_refs[i];
        void *copy_ref = vec_copy_refs[i];
        void *single_def = m_ast->GetSingleAssignExpr(
            m_ast->GetVariableInitializedName(ref), ref, 0, scope, true, true);
        if (single_def != nullptr) {
          ref = single_def;
        }
        void *var = ref;
        if (m_ast->IsVarRefExp(ref)) {
          var = m_ast->GetVariableInitializedName(ref);
        }
        if (m_ast->IsGlobalInitName(var) &&
            m_ast->IsConstType(m_ast->GetTypebyVar(var))) {
          continue;
        } else {
          if (m_ast->IsValueExp(ref)) {
            if (!check)
              m_ast->ReplaceExp(copy_ref, m_ast->CopyExp(ref));
          } else if (auto *old_loop = m_ast->GetLoopFromIteratorRef(ref)) {
            if (non_continuous_loops != nullptr) {
              non_continuous_loops->insert(old_loop);
            }
            if (old_loop2new_var != nullptr) {
              m_ast->ReplaceExp(copy_ref, m_ast->CreateVariableRef(
                                              (*old_loop2new_var)[old_loop]));
            }
          } else {
            return false;
          }
        }
      }
      if (!check) {
        me_copy.set_coeff(copy_var, start_index.get_coeff(atom));
        me_copy.set_coeff(atom, CMarsExpression(m_ast, 0));
      }
    }
  }
  if (new_address != nullptr)
    *new_address = me_copy.get_expr_from_coeff();
  return true;
}

bool StreamPrefetchXilinx::is_short_streaming_length(
    const CMarsExpression &me_length, int wide_bus_factor) {
  if (me_length.IsConstant() && me_length.GetConstant() <= wide_bus_factor) {
    return true;
  }
  return false;
}

bool StreamPrefetchXilinx::get_and_analyze_loop_info(
    void *loop, void *loop_pos, void *pos, void *scope, int64_t *c_lb,
    int64_t *c_ub, int64_t *c_step, int64_t *c_trip_count) {
  if (m_loop_info.count(loop) <= 0) {
    CMarsRangeExpr mr_var(
        CMarsVariable(loop, m_ast, loop_pos, scope).get_range());
    mr_var.refine_range_in_scope(scope, pos);
    if (mr_var.is_const_bound()) {
      int64_t n_lb = mr_var.get_const_lb();
      int64_t n_ub = mr_var.get_const_ub();
      int64_t n_step;
      if (m_ast->GetLoopStep(loop, &n_step)) {
        m_loop_info[loop] = std::make_tuple(n_lb, n_ub, n_step,
                                            (n_ub - n_lb + n_step) / n_step);
      }
    }
  }
  if (m_loop_info.count(loop) <= 0) {
    return false;
  }
  auto res = m_loop_info[loop];
  *c_lb = std::get<0>(res);
  *c_ub = std::get<1>(res);
  *c_step = std::get<2>(res);
  *c_trip_count = std::get<3>(res);
  return true;
}
