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
#include "program_analysis.h"
#include "rose.h"

#include "ir_types.h"
#include "mars_ir.h"
#include "mars_ir_v2.h"

#include "coarse_parallel.h"
#include "mars_message.h"

using MarsProgramAnalysis::ArrayRangeAnalysis;
using MarsProgramAnalysis::CMarsExpression;
using MarsProgramAnalysis::CMarsRangeExpr;
using MarsProgramAnalysis::CMarsVariable;
using MerlinStreamModel::CStreamIR;
using MerlinStreamModel::CStreamNode;

using std::map;
using std::set;
using std::string;
using std::vector;
struct port_info {
  int para_dim;
  enum access_type ac_type;
  vector<void *> offset_indices;
  port_info() {
    para_dim = -1;
    ac_type = NO_ACCESS;
  }
};
void remove_parallel_pragma(CSageCodeGen *codegen, CMirNode *node) {
  for (size_t i = 0; i < node->pragma_table.size(); i++) {
    auto decl = node->pragma_table[i];
    CAnalPragma ana_pragma(codegen);
    bool errorOut = false;
    if (ana_pragma.PragmasFrontendProcessing(decl, &errorOut)) {
      if (ana_pragma.is_parallel()) {
        string pragmaString = codegen->GetPragmaString(decl);
        codegen->AddComment("Original: #pragma " + pragmaString, node->ref);
        codegen->RemoveStmt(decl);
      }
    }
  }
}

// duplicate the whole data of variable 'port'
void *duplicate_read_only_data(CSageCodeGen *codegen, void *port,
                               void *for_loop, struct port_info *info) {
  // check whether we can lift copy data outside its parent loop 'for_loop'
  void *insert_pos = for_loop;
  vector<void *> vec_refs;
  codegen->get_ref_in_scope(port, for_loop, &vec_refs);
  void *port_decl = codegen->GetVariableDecl(port);
  do {
    //  Check if lifting is possible:
    void *parent_loop =
        codegen->TraceUpToLoopScope(codegen->GetParent(insert_pos));
    if (!parent_loop || codegen->IsInScope(port_decl, parent_loop))
      break;
    vector<void *> curr_refs;
    codegen->get_ref_in_scope(port, parent_loop, &curr_refs);
    //  if has different references, stop lifting
    if (vec_refs != curr_refs) {
      break;
    }
    insert_pos = parent_loop;
  } while (true);
  vector<size_t> dims;
  void *base_type;
  void *var_type = codegen->GetTypebyVar(port);
  codegen->get_type_dimension(var_type, &base_type, &dims, port);
  string var_name = codegen->GetVariableName(port);
  string var_buffer_name = var_name + "_dup";
  codegen->get_valid_local_name(codegen->TraceUpToFuncDecl(for_loop),
                                &var_buffer_name);
  auto loop_range = CMarsVariable(for_loop, codegen).get_range();
  CMarsExpression me_lb, me_ub;
  if (!loop_range.get_simple_bound(&me_lb, &me_ub) || !me_lb.IsConstant() ||
      !me_ub.IsConstant()) {
    return nullptr;
  }
  size_t bound_size = (me_ub - me_lb + 1).GetConstant();
  dims.push_back(bound_size);
  std::reverse(dims.begin(), dims.end());
  // for (auto dim : dims) {
  //   cout << dim << "\t";
  // }
  // cout << '\n';
  // dims: from leftmost to rightmost
  void *new_var = codegen->CreateVariableDecl(
      codegen->CreateArrayType(base_type, dims), var_buffer_name, nullptr,
      codegen->GetScope(codegen->GetParent(insert_pos)), port);
  codegen->InsertStmt(new_var, insert_pos);
  {
    // generate for loop to copy the original data
    //  for (i_1 = 0 ... dim0)
    //    for (i_2 = 0 ... dim1)
    //       ...
    //       for (i_0 = 0 ... bound_size) {
    //         #pragma HLS unroll
    //           new_var[i_0][i_1][i_2]... = port[i_1][i_2]...;
    //       }
    //  0 . Crate new Basicblock
    void *sg_bb = codegen->CreateBasicBlock(nullptr, false, for_loop);
    codegen->InsertStmt(sg_bb, insert_pos);
    //  1 . create iterator declaration
    int dim = dims.size();
    vector<void *> vec_decl;
    for (int i = 0; i < dim; i++) {
      void *decl = codegen->CreateVariableDecl(codegen->GetTypeByString("long"),
                                               "_i" + my_itoa(i), nullptr,
                                               sg_bb, for_loop);
      codegen->AppendChild(sg_bb, decl);
      vec_decl.push_back(decl);
    }
    // vec_decl: from leftmost to rightmost
    //  2. create assignement
    void *sg_assign_bb = codegen->CreateBasicBlock(nullptr, false, for_loop);
    {
      void *lhs = codegen->CreateVariableRef(new_var);
      for (int i = 0; i < dim; i++) {
        lhs = codegen->CreateExp(V_SgPntrArrRefExp, lhs,
                                 codegen->CreateVariableRef(vec_decl[i]), 0,
                                 for_loop);
      }
      void *rhs = codegen->CreateVariableRef(port);
      for (int i = 1; i < dim; i++) {
        rhs = codegen->CreateExp(V_SgPntrArrRefExp, rhs,
                                 codegen->CreateVariableRef(vec_decl[i]), 0,
                                 for_loop);
      }
      void *sg_assign = codegen->CreateExp(V_SgAssignOp, lhs, rhs, 0, for_loop);
      void *sg_stmt =
          codegen->CreateStmt(V_SgExprStatement, sg_assign, for_loop);
      codegen->AppendChild(sg_assign_bb, sg_stmt);
    }
    codegen->PrependChild(sg_assign_bb,
                          codegen->CreatePragma("HLS unroll", sg_assign_bb));

    int64_t step = 1;
    //  3. Create the loop levels
    void *new_loop = codegen->CreateStmt(
        V_SgForStatement, codegen->GetVariableInitializedName(vec_decl[0]),
        codegen->CreateConst(0), codegen->CreateConst(dims[0] - 1),
        sg_assign_bb, &step, nullptr, for_loop);

    for (int i = 1; i < dim; i++) {
      void *sg_body = codegen->CreateBasicBlock(nullptr, false, for_loop);
      if (i == 1) {
        codegen->AppendChild(sg_body,
                             codegen->CreatePragma("HLS pipeline", sg_body));
        if (dim >= 3)
          codegen->AppendChild(
              sg_body, codegen->CreatePragma("HLS loop_flatten", sg_body));
      }
      codegen->AppendChild(sg_body, new_loop);
      new_loop = codegen->CreateStmt(
          V_SgForStatement,
          codegen->GetVariableInitializedName(vec_decl[dim - i]),
          codegen->CreateConst(0), codegen->CreateConst(dims[dim - i] - 1),
          sg_body, &step, nullptr, for_loop);
    }
    codegen->AppendChild(sg_bb, new_loop);
  }

  // update reference of original port in the current loop
  vec_refs.clear();
  codegen->get_ref_in_scope(port, for_loop, &vec_refs, false, true);
  std::reverse(vec_refs.begin(), vec_refs.end());
  void *loop_iterator = codegen->GetLoopIterator(for_loop);
  void *offset = nullptr;
  for (auto ref : vec_refs) {
    void *new_ref = codegen->CreateVariableRef(new_var);
    void *index = codegen->CreateVariableRef(loop_iterator);
    if (me_lb != 0) {
      index = codegen->CreateExp(V_SgSubtractOp, index,
                                 me_lb.get_expr_from_coeff());
    }
    offset = index;
    new_ref =
        codegen->CreateExp(V_SgPntrArrRefExp, new_ref, index, 0, for_loop);
    codegen->ReplaceExp(ref, new_ref);
  }
  if (offset != nullptr) {
    info->offset_indices.push_back(offset);
  }
  info->ac_type = READ;
  info->para_dim = 1;
  return codegen->GetVariableInitializedName(new_var);
}

int coarse_parallel_top(CSageCodeGen *codegen, void *pTopFunc,
                        const CInputOptions &options) {
  return coarse_parallel_xilinx_top(codegen, pTopFunc, options);
}

int coarse_parallel_xilinx_top(CSageCodeGen *codegen, void *pTopFunc,
                               const CInputOptions &options) {
  //  1. construct mars ir
  CMarsIr mars_ir;
  mars_ir.get_mars_ir(codegen, pTopFunc, options, true);
  //  2. collect coarse grained parallel loops
  std::cout << "==============================================" << std::endl;
  std::cout << "-----=# Coarse Parallel Optimization Start#=----\n";
  std::cout << "==============================================" << std::endl;
  int naive_tag = 0;

  if (!options.get_option_key_value("-a", "naive_hls").empty()) {
    naive_tag = 1;
    cout << "[MARS-PARALLEL-MSG] Naive HLS mode.\n";
  }

  bool Changed = true;
  bool LocalChanged = false;
  int iterative_tag = 0;
  do {
    LocalChanged = false;

    //  Change the order of applying CGPara, using mars ir topological order
    vector<CMirNode *> vec_nodes;
    mars_ir.get_topological_order_nodes(&vec_nodes);

    for (size_t j = 0; j < vec_nodes.size(); j++) {
      CMirNode *node = vec_nodes[j];
      if (!node->is_function && node->is_complete_unroll(true) &&
          node->has_parallel()) {
#ifdef RPOJDEBUG
        cout << "[TESTING] BSU node is: " << node->unparseToString() << endl;
        cout << endl << "[TESTING] Children list " << endl;
        cout << "[TESTING] BSU node is:" << node->is_fine_grain
             << " unroll:" << node->is_complete_unroll(true)
             << " has_loop: " << node->has_loop << " \n"
             << node->unparseToString() << endl;
#endif
        if (!node->is_fine_grain) {
          if (naive_tag != 0) {
            // TODO(youxiang):
            continue;
          }
          if (generate_parallel_xilinx(codegen, &mars_ir, node)) {
            cout << "[MARS-PARALLEL-MSG] Coarse-grained parallel is applied."
                 << endl;
            LocalChanged = true;
            Changed = true;
            break;
          }
          cout << "[MARS-PARALLEL-MSG] Coarse-grained parallel isn't "
                  "applied."
               << endl;
          remove_parallel_pragma(codegen, node);
          continue;
        }
        cout << "[MARS-CGPAP-MSG] The target loop is a fine-grained loop."
             << endl;

        std::cout << "============================================"
                  << std::endl;
      }
    }
    if (LocalChanged) {
      mars_ir.clear();
      codegen->reset_func_decl_cache();
      codegen->reset_func_call_cache();
      codegen->init_defuse_range_analysis();
      mars_ir.get_mars_ir(codegen, pTopFunc, options, true);
    }
    iterative_tag++;
  } while (LocalChanged);

  std::cout << "============================================" << std::endl;
  std::cout << "-----=# Coarse Parallel Optimization End#=----\n";
  std::cout << "============================================" << std::endl
            << std::endl;
  return static_cast<int>(Changed);
}

bool is_loop_iterator(CSageCodeGen *codegen, void *var_init, void *scope) {
  vector<void *> vec_loops;
  codegen->GetNodesByType(scope, "preorder", "SgForStatement", &vec_loops);
  set<void *> all_refs_in_loop;
  for (auto loop : vec_loops) {
    if (codegen->GetLoopIterator(loop) == var_init) {
      vector<void *> vec_refs;
      codegen->get_ref_in_scope(var_init, loop, &vec_refs);
      all_refs_in_loop.insert(vec_refs.begin(), vec_refs.end());
    }
  }
  vector<void *> vec_refs;
  codegen->get_ref_in_scope(var_init, scope, &vec_refs);
  set<void *> all_refs_in_scope(vec_refs.begin(), vec_refs.end());
  return all_refs_in_loop == all_refs_in_scope;
}

bool check_valid_coarse_grained_loop(CSageCodeGen *codegen, void *for_loop,
                                     string loop_info, bool skip_channel) {
  map<string, string> message;
  if (codegen->IsForStatement(for_loop) == 0) {
#if PROJDEBUG
    cout << codegen->_p(for_loop)
         << " isnot for-statement. Quit Coarse grained parallel." << endl;
#endif
    message["coarse_grained_parallel"] = "off";
    message["coarse_grained_parallel_off_reason"] =
        "only for-statement is supported";
    insertMessage(codegen, for_loop, message);
    return false;
  }
  void *loop_body = codegen->GetLoopBody(for_loop);
  void *lb;
  void *ub;
  void *step;
  lb = ub = step = nullptr;
  bool included_ub = false;

  if (codegen->IsCanonicalForLoop(for_loop, nullptr, &lb, &ub, &step, nullptr,
                                  nullptr, &included_ub) == 0) {
#if PROJDEBUG
    cout << codegen->_p(for_loop)
         << " isnot canonical loop. Quit Coarse grained parallel." << endl;
#endif
    message["coarse_grained_parallel"] = "off";
    message["coarse_grained_parallel_off_reason"] =
        "only canonical for-statement is supported";
    insertMessage(codegen, for_loop, message);
    dump_critical_message(CGPAR_WARNING_3(loop_info));
    return false;
  }
  CMarsExpression me_lb(lb, codegen);
  CMarsExpression me_ub(ub, codegen);
  CMarsExpression me_step(step, codegen);
  if ((me_lb.IsConstant() == 0) || (me_ub.IsConstant() == 0) ||
      (me_step.IsConstant() == 0)) {
#if PROJDEBUG
    cout << codegen->_p(for_loop)
         << " has non-constant bound. Quit coarse grained parallel." << endl;
    cout << "low bound: " << me_lb.print_s() << endl;
    cout << "upper bound: " << me_ub.print_s() << endl;
    cout << "step: " << me_step.print_s() << endl;
#endif
    message["coarse_grained_parallel"] = "off";
    message["coarse_grained_parallel_off_reason"] =
        "only canonical for-statement "
        "with constant bound and step is supported";
    insertMessage(codegen, for_loop, message);
    dump_critical_message(CGPAR_WARNING_4(loop_info));
    return false;
  }

  //  check break/continue/return/goto/label
  vector<void *> vec_stmts;
  codegen->GetNodesByType(loop_body, "preorder", "SgBreakStmt", &vec_stmts);
  for (auto stmt : vec_stmts) {
    if (codegen->IsScopeBreak(stmt, for_loop)) {
#if PROJDEBUG
      cout << "Cannot support break stmt " << codegen->_p(stmt) << endl;
#endif
      message["coarse_grained_parallel"] = "off";
      message["coarse_grained_parallel_off_reason"] =
          "only canonical "
          "for-statement without break statement is supported";
      insertMessage(codegen, for_loop, message);
      dump_critical_message(CGPAR_WARNING_5(loop_info));
      return false;
    }
  }

  codegen->GetNodesByType(loop_body, "preorder", "SgContinueStmt", &vec_stmts);
  for (auto stmt : vec_stmts) {
    void *parent_loop = codegen->TraceUpToLoopScope(stmt);
    if (parent_loop == for_loop) {
#if PROJDEBUG
      cout << "Cannot support Continue stmt " << codegen->_p(stmt) << endl;
#endif
      message["coarse_grained_parallel"] = "off";
      message["coarse_grained_parallel_off_reason"] =
          "only canonical "
          "for-statement without continue statement is supported";
      insertMessage(codegen, for_loop, message);
      dump_critical_message(CGPAR_WARNING_6(loop_info));
      return false;
    }
  }

  codegen->GetNodesByType(loop_body, "preorder", "SgReturnStmt", &vec_stmts);
  if (!vec_stmts.empty()) {
#if PROJDEBUG
    for (auto stmt : vec_stmts) {
      cout << "Cannot support return stmt " << codegen->_p(stmt) << endl;
    }
#endif
    message["coarse_grained_parallel"] = "off";
    message["coarse_grained_parallel_off_reason"] =
        "only canonical "
        "for-statement without return statement is supported";
    insertMessage(codegen, for_loop, message);
    dump_critical_message(CGPAR_WARNING_7(loop_info));
    return false;
  }

  codegen->GetNodesByType(loop_body, "preorder", "SgGotoStatement", &vec_stmts);
  for (auto stmt : vec_stmts) {
    void *label = codegen->GetGotoLabel(stmt);
    if (codegen->is_located_in_scope(label, loop_body) == 0) {
#if PROJDEBUG
      cout << "Cannot support goto stmt " << codegen->_p(stmt) << endl;
#endif
      message["coarse_grained_parallel"] = "off";
      message["coarse_grained_parallel_off_reason"] =
          "only canonical "
          "for-statement without goto outside statement is supported";
      insertMessage(codegen, for_loop, message);
      dump_critical_message(CGPAR_WARNING_8(loop_info));
      return false;
    }
  }

  codegen->GetNodesByType(loop_body, "preorder", "SgLabelStatement",
                          &vec_stmts);
  if (!vec_stmts.empty()) {
    set<void *> all_labels(vec_stmts.begin(), vec_stmts.end());
    vector<void *> vec_goto;
    codegen->GetNodesByType(nullptr, "preorder", "SgGotoStatement", &vec_goto);
    for (auto goto_stmt : vec_goto) {
      void *label = codegen->GetGotoLabel(goto_stmt);
      if ((codegen->is_located_in_scope(goto_stmt, loop_body) == 0) &&
          all_labels.count(label) > 0) {
#if PROJDEBUG
        cout << "Cannot support label stmt " << codegen->_p(label)
             << " which has a goto outside the current loop" << endl;
#endif
        message["coarse_grained_parallel"] = "off";
        message["coarse_grained_parallel_off_reason"] =
            "only canonical "
            "for-statement without external label statement is supported";
        insertMessage(codegen, for_loop, message);
        dump_critical_message(CGPAR_WARNING_9(loop_info));
        return false;
      }
    }
  }

  //  check whether there is local static or global variables in the scope
  vector<void *> vec_refs_recursive;
  codegen->GetNodesByType(loop_body, "preorder", "SgVarRefExp",
                          &vec_refs_recursive, false, true, true);
  set<void *> global_variables;
  for (auto ref : vec_refs_recursive) {
    void *var_init = codegen->GetVariableInitializedName(ref);
    if (codegen->IsCompilerGenerated(var_init)) {
      continue;
    }
    if (codegen->IsArgumentInitName(var_init) != 0) {
      continue;
    }
    void *var_type = codegen->GetTypebyVar(var_init);
    if (codegen->IsConstType(var_type) != 0) {
      continue;
    }
    void *base_type = codegen->GetBaseType(var_type, true);
    //  skip merlin interal streaming channel
    if (skip_channel && codegen->GetStringByType(base_type).find(
                            "merlin_stream") != string::npos) {
      continue;
    }
    if (global_variables.count(var_init) > 0) {
      continue;
    }
    string port_name = codegen->GetVariableName(var_init);
    auto arg_user_info = getUserCodeInfo(codegen, var_init);
    if (!arg_user_info.name.empty()) {
      port_name = arg_user_info.name;
    }
    string port_info =
        "\'" + port_name + "\' (" + arg_user_info.file_location + ")";
    if (codegen->IsLocalInitName(var_init) != 0) {
      void *var_decl = codegen->GetVariableDecl(var_init);
      if (codegen->IsStatic(var_decl)) {
        message["coarse_grained_parallel"] = "off";
        message["coarse_grained_parallel_off_reason"] =
            "only canonical "
            "for-statement without local static variable access is supported";
        insertMessage(codegen, for_loop, message);
        dump_critical_message(CGPAR_WARNING_12(loop_info, port_info));
        global_variables.insert(var_init);
      }
      continue;
    }
    if (codegen->IsGlobalInitName(var_init) != 0) {
      message["coarse_grained_parallel"] = "off";
      message["coarse_grained_parallel_off_reason"] =
          "only canonical "
          "for-statement without global variable access is supported";
      insertMessage(codegen, for_loop, message);
      dump_critical_message(CGPAR_WARNING_11(loop_info, port_info));
      global_variables.insert(var_init);
    }
  }

  return global_variables.empty();
}

bool generate_parallel_xilinx(CSageCodeGen *codegen, CMarsIr *mars_ir,
                              CMirNode *node) {
  //  3. outline paralleled sub kernel
  void *loop_body = node->ref;

  void *for_loop = codegen->GetParent(loop_body);

  void *func_decl = codegen->TraceUpToFuncDecl(loop_body);

  void *func_body = codegen->GetFuncBody(func_decl);

  void *scope = codegen->TraceUpToBasicBlock(for_loop);
  string loop_label = codegen->get_internal_loop_label(for_loop);
  string loop_msg = loop_label;
  void *iter = codegen->GetLoopIterator(for_loop);
  if (iter) {
    loop_msg = codegen->UnparseToString(iter);
  }
  string func_name = codegen->GetFuncName(func_decl);
  string loop_info = '\'' + loop_msg + "\' " +
                     getUserCodeFileLocation(codegen, for_loop, true) +
                     " in function \'" + func_name + "\'";

  if (!check_valid_coarse_grained_loop(codegen, for_loop, loop_info, false)) {
    return false;
  }
  void *lb;
  void *ub;
  void *step;
  lb = ub = step = nullptr;
  bool included_ub = false;
  codegen->IsCanonicalForLoop(for_loop, nullptr, &lb, &ub, &step, nullptr,
                              nullptr, &included_ub);
  CMarsExpression me_lb(lb, codegen);
  CMarsExpression me_ub(ub, codegen);
  CMarsExpression me_step(step, codegen);

  if (codegen->dead_stmts_removal(loop_body)) {
    codegen->init_defuse_range_analysis();
  }

  vector<void *> vec_refs;

  codegen->GetNodesByType(loop_body, "preorder", "SgVarRefExp", &vec_refs);

  map<void *, set<void *>> var2refs;
  for (auto ref : vec_refs) {
    void *var_init = codegen->GetVariableInitializedName(ref);
    //  20180718: YX, skip debug variables, such as  '__PRETTY_FUNCTION__'
    if (codegen->IsCompilerGenerated(var_init)) {
      continue;
    }
    var2refs[var_init].insert(ref);
  }

  map<string, string> message;
  set<void *> ports;
  set<void *> read_only_duplicated_data_ports;
  map<void *, struct port_info> port2info;
  set<void *> dead_ports;
  //  check whether there is external memory access
  //  collect port information
  //  collect parallelism information for on-chip buffer
  set<void *> external_ports;
  for (auto var_ref_pair : var2refs) {
    auto var = var_ref_pair.first;
    auto refs = var_ref_pair.second;
    void *var_type = codegen->GetTypebyVar(var);
    // support array of hls::stream in the interface
    map<void *, bool> res;
    if (mars_ir->any_trace_is_bus(codegen, func_decl, var, &res) &&
        ((codegen->IsArrayType(var_type) != 0) ||
         (codegen->IsPointerType(var_type) != 0)) &&
        codegen->IsXilinxStreamType(codegen->GetBaseType(var_type, true)) ==
            0) {
      ArrayRangeAnalysis mr(var, codegen, loop_body, loop_body, false, false);
      if ((mr.has_read() != 0) || (mr.has_write() != 0)) {
        string port_name = codegen->GetVariableName(var);
        auto arg_user_info = getUserCodeInfo(codegen, var);
        if (!arg_user_info.name.empty()) {
          port_name = arg_user_info.name;
        }
        string port_info =

            "\'" + port_name + "\' (" + arg_user_info.file_location + ")";
        external_ports.insert(var);
#if PROJDEBUG
        cout << "external memory access on port \'" << codegen->_p(var)
             << "\'. Quit Coarse grained parallel." << endl;
#endif
        message["coarse_grained_parallel"] = "off";
        message["coarse_grained_parallel_off_reason"] =
            "only canonical "
            "for-statement without external memory access is supported";
        insertMessage(codegen, for_loop, message);
        dump_critical_message(CGPAR_WARNING_1(loop_info, port_info));
      } else {
        ports.insert(var);
        dead_ports.insert(var);
      }
      continue;
    }

    void *var_decl = codegen->GetVariableDecl(var);
    if (codegen->is_located_in_scope(var_decl, loop_body) != 0) {
      continue;
    }

    ports.insert(var);

    //  analyze read/write access

    bool is_array = codegen->IsArrayType(var_type) != 0;
    bool is_pointer = codegen->IsPointerType(var_type) != 0;
    if (!is_array && !is_pointer) {
      int read;
      int write;
      read = write = 0;
      for (auto ref : refs) {
        read |= codegen->has_read_conservative(ref);
        write |= codegen->has_write_conservative(ref);
      }
      port2info[var].ac_type =
          static_cast<enum access_type>(read | (write << 1));
    } else {
      int write = 0;
      ArrayRangeAnalysis mr(var, codegen, loop_body, loop_body, false, false);
      set<int> para_dims;
      vector<CMarsRangeExpr> vec_union_range;

      if ((mr.has_write() != 0) && (mr.has_read() != 0)) {
        vec_union_range =
            RangeUnioninVector(mr.GetRangeExprWrite(), mr.GetRangeExprRead());
        write = READWRITE;
      } else if (mr.has_write() != 0) {
        vec_union_range = mr.GetRangeExprWrite();
        write = WRITE;
      } else if (mr.has_read() != 0) {
        vec_union_range = mr.GetRangeExprRead();
        write = READ;
      } else {
        dead_ports.insert(var);
        continue;
      }

      vector<void *> offset_indices;
      //  current coding style requires the paralleled dimension should be
      //  leftmost dimension
      size_t dim = 1;
      bool found_partition_dim = false;
      for (size_t i = 0; i < vec_union_range.size(); ++i) {
        dim = i + 1;
        auto one_range = vec_union_range[i];
        CMarsExpression lb, ub;
        int ret = one_range.get_simple_bound(&lb, &ub);
        if (!ret) {
          break;
        }
        if (one_range.is_single_access() && lb.IsConstant()) {
          offset_indices.push_back(lb.get_expr_from_coeff());
          continue;
        }
        if (one_range.is_single_access()) {
          CMarsExpression index = lb;
          offset_indices.push_back(index.get_expr_from_coeff());
          map<void *, int64_t> iterators;
          map<void *, int64_t> invariants;
          if (index.IsStandardForm(for_loop, &iterators, &invariants)) {
            if (iterators.size() == 1 && iterators.count(for_loop) > 0 &&
                invariants.empty()) {
              port2info[var].para_dim = dim;
              found_partition_dim = true;
              port2info[var].offset_indices = offset_indices;
              if (dim == vec_union_range.size() && (mr.has_write() != 0)) {
                port2info[var].ac_type = WRITE;
              }
            }
          }
        }
        break;
      }
      if (!found_partition_dim && ((write & WRITE) == 0)) {
        bool duplicated_data = true;
        for (auto one_range : vec_union_range) {
          CMarsExpression lb, ub;
          int ret = one_range.get_simple_bound(&lb, &ub);
          if (!ret || !lb.IsLoopInvariant(for_loop) ||
              !ub.IsLoopInvariant(for_loop)) {
            duplicated_data = false;
            break;
          }
        }
        if (duplicated_data) {
          read_only_duplicated_data_ports.insert(var);
        }
      }
    }
  }

  if (!external_ports.empty()) {
    return false;
  }

  bool all_local_buf_partitioned = true;
  bool has_data_dependence = false;
  string diagnosis_info;
  string port_info;
  for (auto port : ports) {
    if (read_only_duplicated_data_ports.count(port) > 0)
      continue;
    void *type = codegen->GetTypebyVar(port);
    string port_name = codegen->GetVariableName(port);
    auto arg_user_info = getUserCodeInfo(codegen, port);
    if (!arg_user_info.name.empty()) {
      port_name = arg_user_info.name;
    }
    if ((codegen->IsArrayType(type) != 0) ||
        (codegen->IsPointerType(type) != 0)) {
      if (dead_ports.count(port) <= 0 && port2info[port].para_dim <= 0) {
        diagnosis_info +=
            "on-chip buffer \'" + port_name + "\' cannot be partitioned\n";
        all_local_buf_partitioned = false;
        port_info +=
            "\'" + port_name + "\' (" + arg_user_info.file_location + ")\n";
      }
    } else {
      if ((port2info[port].ac_type & WRITE) != 0) {
        //  YX: 20170630 bug1168
        if (is_loop_iterator(codegen, port, loop_body)) {
          port2info[port].ac_type = READ;
          continue;
        }
        diagnosis_info +=
            "on-chip scalar \'" + port_name + "\' has data dependence\n";
        has_data_dependence = true;
        port_info +=
            "\'" + port_name + "\' (" + arg_user_info.file_location + ")\n";
      }
    }
  }
  if (!all_local_buf_partitioned || has_data_dependence) {
#if PROJDEBUG
    cout << "Quit coarse grained parallel on loop " + loop_info << endl;
    cout << diagnosis_info;
#endif
    message["coarse_grained_parallel"] = "off";
    message["coarse_grained_parallel_off_reason"] =
        "only canonical "
        "for-statement without data dependence is supported";
    insertMessage(codegen, for_loop, message);
    dump_critical_message(CGPAR_WARNING_2(loop_info, port_info));
    return false;
  }

#if PROJDEBUG
  cout << "apply coarse grained parallel on loop " + loop_info << endl;
#endif
  for (auto port : read_only_duplicated_data_ports) {
    struct port_info info;
    void *new_port = duplicate_read_only_data(codegen, port, for_loop, &info);
    ports.erase(port);
    port2info.erase(port);
    ports.insert(new_port);
    port2info[new_port] = info;
  }

  dump_critical_message(CGPAR_INFO_1(loop_info));
  message["coarse_grained_parallel"] = "on";
  int64_t trip_count = 0;
  codegen->get_loop_trip_count(for_loop, &trip_count);
  message["coarse_grained_parallel_factor"] = my_itoa(trip_count);
  insertMessage(codegen, for_loop, message);

  void *loop_iter = codegen->GetLoopIterator(for_loop);
  //  add loop iterator as port for function instantiation
  if (ports.count(loop_iter) <= 0) {
    ports.insert(loop_iter);
  }
  //  create sub kernel function to
  //  include the coarse grained paralleled loop
  vector<void *> vec_parameters;
  map<void *, void *> origport2newport;
  set<void *> pointer_ports;
  for (auto port : ports) {
    //  skip loop iterator
    if (port == loop_iter) {
      continue;
    }
    void *type = codegen->GetTypebyVar(port);
    string var_name = codegen->GetVariableName(port);
    void *new_type = type;
    if ((codegen->IsArrayType(type) == 0) &&
        (codegen->IsPointerType(type) == 0) &&
        ((port2info[port].ac_type & WRITE) != 0)) {
      new_type = codegen->CreatePointerType(new_type);
      pointer_ports.insert(port);
    }
    void *parameter = codegen->CreateVariable(new_type, var_name);
    vec_parameters.push_back(parameter);
    origport2newport[port] = parameter;
  }

  string para_func_name = func_name + "_" + loop_label + "_para";
  void *para_func =
      codegen->CreateFuncDecl("void", para_func_name, vec_parameters,
                              codegen->GetGlobal(for_loop), true, for_loop);

  void *insert_pos = func_decl;
  while (codegen->IsPragmaDecl(codegen->GetPreviousStmt(insert_pos)) != 0) {
    insert_pos = codegen->GetPreviousStmt(insert_pos);
  }
  codegen->InsertStmt(para_func, insert_pos);

  void *para_func_body = codegen->GetFuncBody(para_func);

  {
    //  add inline off pragma
    string inline_off_pragma_str = "HLS inline off";
    void *inline_off_pragma =
        codegen->CreatePragma(inline_off_pragma_str, para_func_body);
    codegen->PrependChild(para_func_body, inline_off_pragma);
  }
  //  clone new loop
  void *new_loop_iter_decl = codegen->CreateVariableDecl(
      codegen->GetTypebyVar(loop_iter), codegen->GetVariableName(loop_iter),
      nullptr, para_func_body);
  codegen->AppendChild(para_func_body, new_loop_iter_decl);
  void *new_loop_iter = codegen->GetVariableInitializedName(new_loop_iter_decl);
  if (ports.count(loop_iter) > 0) {
    origport2newport[loop_iter] = new_loop_iter;
  }
  void *new_loop_body = codegen->CreateBasicBlock(nullptr, false, for_loop);
  assert(me_lb.IsConstant() && me_ub.IsConstant() && me_step.IsConstant());
  int64_t step_value = me_step.GetConstant();
  void *new_loop = codegen->CreateStmt(
      V_SgForStatement, new_loop_iter, me_lb.get_expr_from_coeff(),
      me_ub.get_expr_from_coeff(), new_loop_body, &step_value, &included_ub,
      for_loop);
  codegen->AppendChild(para_func_body, new_loop);
  setQoRMergeMode(codegen, new_loop, "minor", true);

  //  create new function call
  {
    vector<void *> vec_args;
    for (auto port : ports) {
      if (port == loop_iter) {
        continue;
      }
      void *arg = codegen->CreateVariableRef(port);
      if (pointer_ports.count(port) > 0) {
        arg = codegen->CreateExp(V_SgAddressOfOp, arg);
      }
      vec_args.push_back(arg);
    }

    void *para_func_call = codegen->CreateFuncCall(
        para_func_name, codegen->GetTypeByString("void"), vec_args, scope,
        for_loop);
    void *para_func_call_stmt =
        codegen->CreateStmt(V_SgExprStatement, para_func_call);
    codegen->InsertStmt(para_func_call_stmt, for_loop);
    setQoRMergeMode(codegen, para_func_call, "major", true);
    setFuncDeclUserCodeScopeId(codegen, para_func, para_func_call_stmt);
  }

  //  create sub kernel function to include the coarse grained paralleled
  //  loop body
  vec_parameters.clear();
  map<void *, void *> origport2newport_sub;
  for (auto port : ports) {
    void *type = codegen->GetTypebyVar(port);

    vector<size_t> dims;
    void *base_type;
    codegen->get_type_dimension(type, &base_type, &dims, port);
    void *new_type = type;
    if (port2info[port].para_dim > 0) {
      size_t par_dim = port2info[port].para_dim;
      vector<size_t> sub_dims;
      for (size_t i = par_dim; i < dims.size(); ++i) {
        sub_dims.push_back(dims[dims.size() - 1 - i]);
      }
      if (!sub_dims.empty()) {
        new_type = codegen->CreateArrayType(base_type, sub_dims);
      } else {
        new_type = base_type;
      }
    }
    string var_name = codegen->GetVariableName(port);
    if ((port2info[port].ac_type & WRITE) != 0) {
      new_type = codegen->CreatePointerType(new_type);
    }
    void *parameter = codegen->CreateVariable(new_type, var_name);
    vec_parameters.push_back(parameter);
    origport2newport_sub[port] = parameter;
  }
  string para_sub_func_name = func_name + "_" + loop_label + "_para_sub";
  void *para_sub_func = codegen->CreateFuncDecl(
      "void", para_sub_func_name, vec_parameters, codegen->GetGlobal(for_loop),
      true, codegen->GetLoopBody(for_loop));
  codegen->InsertStmt(para_sub_func, para_func);

  //  remove original accel pragma
  for (size_t i = 0; i < node->pragma_table.size(); i++) {
    auto decl = node->pragma_table[i];
    if (codegen->IsInScope(decl, node->ref)) {
      string pragmaString = codegen->GetPragmaString(decl);
      //  attachComment(node->ref, "Original: #pragma " + pragmaString);
      codegen->AddComment("Original: #pragma " + pragmaString, node->ref);
      codegen->RemoveStmt(decl);
    }
  }

  void *copy_loop_body = codegen->CopyStmt(loop_body);
  codegen->SetFuncBody(para_sub_func, copy_loop_body);
  void *sub_func_body = copy_loop_body;
  {
    //  replace variable reference
    for (auto port : ports) {
      vector<void *> vec_refs;
      codegen->get_ref_in_scope(port, sub_func_body, &vec_refs);
      std::reverse(vec_refs.begin(), vec_refs.end());
      for (auto ref : vec_refs) {
        void *new_port = origport2newport_sub[port];
        void *new_ref = codegen->CreateVariableRef(new_port);
        int new_array_dim = codegen->get_dim_num_from_var(new_port);
        if (port2info[port].para_dim > 0) {
          int para_dim = port2info[port].para_dim;
          void *sg_pntr = nullptr;
          void *sg_array = nullptr;
          vector<void *> indices;
          int pointer_dim;
          int old_array_dim = codegen->get_dim_num_from_var(port);
          codegen->parse_pntr_ref_by_array_ref(ref, &sg_array, &sg_pntr,
                                               &indices, &pointer_dim);
          if (pointer_ports.count(port) > 0) {
            pointer_dim++;
          }
#if PROJDEBUG
          assert(sg_array == port);
          assert(indices.size() >= (size_t)para_dim);
#endif
          vector<void *> new_indices;
          bool skip_tail_zero = true;
          for (size_t i = indices.size(); i > para_dim; --i) {
            if (skip_tail_zero) {
              CMarsExpression index(indices[i - 1], codegen);
              if ((index == 0) != 0) {
                continue;
              }
            }
            skip_tail_zero = false;
            new_indices.insert(new_indices.begin(),
                               codegen->CopyExp(indices[i - 1]));
          }
          if (!new_indices.empty()) {
            new_ref = codegen->CreateArrayRef(new_ref, new_indices);
          }
          size_t orig_dim_offset = old_array_dim - pointer_dim;
          size_t new_dim_offset = new_array_dim - new_indices.size();
          if (new_dim_offset < orig_dim_offset) {
            for (size_t i = new_dim_offset; i < orig_dim_offset; ++i) {
              new_ref = codegen->CreateExp(V_SgAddressOfOp, new_ref);
            }
          } else {
            for (size_t i = orig_dim_offset; i < new_dim_offset; ++i) {
              new_ref = codegen->CreateExp(V_SgPntrArrRefExp, new_ref,
                                           codegen->CreateConst(0));
            }
          }
          codegen->ReplaceExp(sg_pntr, new_ref);
          continue;
        }
        if (pointer_ports.count(port) > 0) {
          new_ref = codegen->CreateExp(V_SgPointerDerefExp, new_ref);
        }
        codegen->ReplaceExp(ref, new_ref);
      }
    }
  }

  {
    //  add inline off pragma
    string inline_off_pragma_str = "HLS inline off";
    void *inline_off_pragma =
        codegen->CreatePragma(inline_off_pragma_str, sub_func_body);
    codegen->PrependChild(sub_func_body, inline_off_pragma);
  }
#if 0
  // Youxiang: 20200804 MER2340 no need to apply instantiation explicitly
  {
    //  YX: 20180707 MER
    //  add function instantiation
    assert(origport2newport_sub.count(loop_iter) > 0);
    string function_instantiation_str =
        "HLS function_instantiate variable=" +
        codegen->GetVariableName(origport2newport_sub[loop_iter]);
    void *function_instantiation_pragma =
        codegen->CreatePragma(function_instantiation_str, sub_func_body);
    codegen->PrependChild(sub_func_body, function_instantiation_pragma);
  }
#endif
  //  only keep iterator calculation
  {
    vector<void *> vec_args;
    for (auto port : ports) {
      void *new_port = origport2newport[port];
      void *arg = codegen->CreateVariableRef(new_port);
      if (!port2info[port].offset_indices.empty()) {
        vector<void *> new_indices;
        for (auto offset : port2info[port].offset_indices) {
          new_indices.push_back(codegen->CopyExp(offset));
        }
        arg = codegen->CreateArrayRef(arg, new_indices);
      }
      if ((port2info[port].ac_type & WRITE) != 0) {
        arg = codegen->CreateExp(V_SgAddressOfOp, arg);
      }
      codegen->replace_variable_references_in_scope(loop_iter, new_loop_iter,
                                                    arg);
      vec_args.push_back(arg);
    }

    void *para_sub_func_call = codegen->CreateFuncCall(
        para_sub_func_name, codegen->GetTypeByString("void"), vec_args,
        new_loop_body, loop_body);
    void *para_sub_func_call_stmt =
        codegen->CreateStmt(V_SgExprStatement, para_sub_func_call);
    codegen->AppendChild(new_loop_body, para_sub_func_call_stmt);
    setFuncDeclUserCodeScopeId(codegen, para_sub_func, para_sub_func_call_stmt);
  }
  void *empty_loop_body = codegen->CreateBasicBlock(nullptr, false, loop_body);
  codegen->ReplaceStmt(loop_body, empty_loop_body);
  //  4. add HLS parallel pragma
  {
    string HLS_parallel_pragma_str = "HLS unroll complete";
    void *HLS_parallel_pragma =
        codegen->CreatePragma(HLS_parallel_pragma_str, new_loop_body);
    codegen->PrependChild(new_loop_body, HLS_parallel_pragma);
  }
  //  5. apply memory partition/duplication to remove memory dependency
  for (auto port : ports) {
    if (port2info[port].para_dim <= 0) {
      continue;
    }
    int dim = port2info[port].para_dim;
    string partition_pragma_str =
        "HLS array_partition variable=" + codegen->GetVariableName(port) +
        " dim=" + my_itoa(dim) + " complete";
    void *partition_pragma =
        codegen->CreatePragma(partition_pragma_str, func_body);
    if (codegen->IsLocalInitName(port) != 0) {
      void *var_decl = codegen->GetVariableDecl(port);
      codegen->InsertAfterStmt(partition_pragma, var_decl);
    } else {
      codegen->PrependChild(func_body, partition_pragma);
    }
  }
  return true;
}

bool generate_copy_in_out(CSageCodeGen *codegen, void *array, void *scope,
                          int read_only, MemoryBurst *burst_instance) {
  vector<void *> vec_refs;
  codegen->get_ref_in_scope(array, scope, &vec_refs);
  if (vec_refs.empty()) {
    return false;
  }

  void *copy_scope = codegen->CreateBasicBlock(nullptr, false, scope);
  if (read_only != 0) {
    codegen->PrependChild(scope, copy_scope, true);
  } else {
    codegen->AppendChild(scope, copy_scope);
  }

  ArrayRangeAnalysis mar(array, codegen, scope, scope, false, false);
  vector<CMarsRangeExpr> vec_range;
  if (read_only != 0) {
    vec_range = mar.GetRangeExprRead();
  } else {
    vec_range = mar.GetRangeExprWrite();
  }
  vector<CMarsExpression> vec_start;
  vector<CMarsExpression> vec_length;
  vector<size_t> vec_size;
  void *element_type = codegen->GetBaseType(codegen->GetTypebyVar(array), true);
  int element_size = codegen->get_bitwidth_from_type(element_type) / 8;
  int read_write = (mar.has_write() << 1) | mar.has_read();
  for (auto mr_merged : vec_range) {
    CMarsExpression me_start;
    CMarsExpression me_length;
    bool is_non_negative_len = true;
    int is_applied = 0;
    int64_t buff_size = 0;
    bool is_exact = false;
    burst_instance->cg_check_range_availability(
        scope, array, element_size, mr_merged, &me_start, &me_length,
        &is_applied, &buff_size, &read_write, &is_non_negative_len, &is_exact,
        false);

    if (is_applied != MemoryBurst::BURST_APPLIED) {
      return false;
    }
    vec_start.push_back(me_start);
    vec_length.push_back(me_length);
    vec_size.push_back(buff_size);
  }

  //  declar copy buffer
  void *new_type = element_type;
  if (static_cast<unsigned int>(!vec_size.empty()) != 0U) {
    new_type = codegen->CreateArrayType(new_type, vec_size);
  }

  string var_name = codegen->GetVariableName(array) +
                    (read_only != 0 ? "_copy_in" : "_copy_out");
  void *new_array =
      codegen->CreateVariableDecl(new_type, var_name, nullptr, scope, array);
  codegen->PrependChild(scope, new_array, true);

  //  update references
  std::reverse(vec_refs.begin(), vec_refs.end());
  for (auto ref : vec_refs) {
    void *pntr;
    void *array;
    vector<void *> indexes;
    int pointer_dim;
    codegen->parse_pntr_ref_by_array_ref(ref, &array, &pntr, &indexes,
                                         &pointer_dim);
    void *new_ref = codegen->CreateVariableRef(new_array);
    int i = 0;
    for (auto index : indexes) {
      CMarsExpression new_index =
          CMarsExpression(index, codegen, ref) - vec_start[i];
      new_ref = codegen->CreateExp(V_SgPntrArrRefExp, new_ref,
                                   new_index.get_expr_from_coeff());
      ++i;
      if (i >= pointer_dim) {
        break;
      }
    }
    codegen->ReplaceExp(pntr, new_ref);
  }

  //  generate copy_in/out
  void *inner_body = codegen->CreateBasicBlock(nullptr, false, scope);
  void *body = inner_body;
  vector<void *> vec_loop_iters;
  void *func_decl = codegen->TraceUpToFuncDecl(scope);
  void *copy_exp = codegen->CreateVariableRef(new_array);
  void *orig_exp = codegen->CreateVariableRef(array);
  for (size_t i = 0; i < vec_size.size(); ++i) {
    string iter_var_str = "i_" + my_itoa(i);
    codegen->get_valid_local_name(func_decl, &iter_var_str);
    void *iter_var = codegen->CreateVariableDecl(
        codegen->GetTypeByString("int"), iter_var_str, nullptr, copy_scope);
    codegen->AppendChild(copy_scope, iter_var);
    vec_loop_iters.push_back(iter_var);
    copy_exp = codegen->CreateExp(V_SgPntrArrRefExp, copy_exp,
                                  codegen->CreateVariableRef(iter_var));
    orig_exp = codegen->CreateExp(
        V_SgPntrArrRefExp, orig_exp,
        codegen->CreateExp(V_SgAddOp, vec_start[i].get_expr_from_coeff(),
                           codegen->CreateVariableRef(iter_var)));
  }

  for (size_t i = vec_size.size(); i > 0; --i) {
    void *sg_len = vec_length[i - 1].get_expr_from_coeff();
    void *start_exp = codegen->CreateConst(0);
    void *iter = codegen->GetVariableInitializedName(vec_loop_iters[i - 1]);
    int64_t step = 1;
    bool inclusive_bound = false;
    void *new_for_loop =
        codegen->CreateStmt(V_SgForStatement, iter, start_exp, sg_len, body,
                            &step, &inclusive_bound, array);
    body = codegen->CreateBasicBlock(nullptr, false, scope);
    codegen->AppendChild(body, new_for_loop);
  }
  codegen->AppendChild(copy_scope, body);

  void *copy_assign_stmt = nullptr;
  if (read_only != 0) {
    copy_assign_stmt =
        codegen->CreateStmt(V_SgAssignStatement, copy_exp, orig_exp);
  } else {
    copy_assign_stmt =
        codegen->CreateStmt(V_SgAssignStatement, orig_exp, copy_exp);
  }
  codegen->PrependChild(inner_body, copy_assign_stmt);
  return true;
}
