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


#include <tuple>
#include "codegen.h"
#include "comm_opt.h"
#include "mars_ir_v2.h"

using MarsProgramAnalysis::CMarsAccess;
using MarsProgramAnalysis::CMarsExpression;
using MarsProgramAnalysis::CMarsRangeExpr;
using MarsProgramAnalysis::CMarsVariable;

using std::map;
using std::set;
using std::tuple;
using std::vector;

namespace MarsCommOpt {

int generate_reorder_channel(CSageCodeGen *codegen, CMarsEdge *edge) {
  CMarsNode *write = edge->GetNode(0);
  CMarsNode *read = edge->GetNode(1);
  CMarsScheduleVector schedule0 = write->get_schedule();
  CMarsScheduleVector schedule1 = read->get_schedule();
  int sync_level = get_sync_level_from_schedule(schedule0, schedule1);
  int inner_loop_level_start = sync_level / 2;
  int channel_size = my_atoi(edge->GetProperty("channel_size"));
  int parallel_bits = my_atoi(edge->GetProperty("parallel_bits"));
  void *port = edge->GetPort();
  void *var_type = codegen->GetTypebyVar(port);
  vector<size_t> dims;
  void *data_type = nullptr;
  codegen->get_type_dimension(var_type, &data_type, &dims, port);
  vector<size_t> new_dims(dims.size() + 1);
  new_dims[0] = 2;
  for (size_t i = 0; i < dims.size(); ++i) {
    new_dims[i + 1] = dims[dims.size() - 1 - i];
  }
  string var_name = codegen->GetVariableName(port);
  void *user_kernel = write->get_user_kernel();
  void *func_body = codegen->GetFuncBody(user_kernel);
  void *double_buffer_type = codegen->CreateArrayType(data_type, new_dims);
  //  1. create double buffer
  string buffer_name = var_name + "_reorder_buffer";
  codegen->get_valid_local_name(user_kernel, &buffer_name);
  void *double_buffer = codegen->CreateVariableDecl(
      double_buffer_type, buffer_name, nullptr, func_body);
  codegen->PrependChild(func_body, double_buffer);

  set<void *> write_refs = write->get_port_references(port);
  assert(write_refs.size() == 1);
  void *write_ref = *write_refs.begin();
  CMarsAccess write_access(write_ref, codegen, nullptr);
  assert(write_access.is_simple_type_v1());
  vector<tuple<void *, int64_t, int64_t>> write_idxs =
      write_access.simple_type_v1_get_dim_to_var();
  set<void *> read_refs = read->get_port_references(port);
  assert(read_refs.size() == 1);
  void *read_ref = *read_refs.begin();
  CMarsAccess read_access(read_ref, codegen, nullptr);
  assert(read_access.is_simple_type_v1());
  vector<tuple<void *, int64_t, int64_t>> read_idxs =
      read_access.simple_type_v1_get_dim_to_var();
  set<void *> write_index_loops;
  set<void *> read_index_loops;
  vector<void *> write_loops = write->get_loops();
  vector<void *> read_loops = read->get_loops();
  assert(write_idxs.size() == read_idxs.size());
  for (size_t i = 0; i != write_idxs.size(); ++i) {
    void *loop = std::get<0>(write_idxs[i]);
    if (loop != nullptr) {
      write_index_loops.insert(loop);
    }
    void *read_loop = std::get<0>(read_idxs[i]);
    if (read_loop != nullptr) {
      read_index_loops.insert(read_loop);
    }
  }
  vector<void *> new_outer_loops;

  for (int i = 0; i < inner_loop_level_start; ++i) {
    assert(codegen->IsomorphicLoops(write_loops[i], read_loops[i]));
    new_outer_loops.push_back(write_loops[i]);
  }
  vector<void *> write_inner_loops;
  for (size_t i = inner_loop_level_start; i != write_loops.size(); ++i) {
    void *loop = write_loops[i];
    if (write_index_loops.count(loop) > 0) {
      write_inner_loops.push_back(loop);
    }
  }
  vector<void *> read_inner_loops;
  for (size_t i = inner_loop_level_start; i != read_loops.size(); ++i) {
    void *loop = read_loops[i];
    if (read_index_loops.count(loop) > 0) {
      read_inner_loops.push_back(loop);
    }
  }
  assert(write_inner_loops.size() == read_inner_loops.size());
  string out_iter_name = var_name + "__outer_i";
  codegen->get_valid_local_name(user_kernel, &out_iter_name);
  void *out_iter =
      codegen->CreateVariableDecl("int", out_iter_name, nullptr, func_body);
  vector<void *> vec_loop_iter_init;
  codegen->PrependChild(func_body, out_iter);
  vec_loop_iter_init.push_back(
      codegen->CreateExp(V_SgAssignOp, codegen->CreateVariableRef(out_iter),
                         codegen->CreateConst(0)));
  string total_iter_name = var_name + "_total_i";
  codegen->get_valid_local_name(user_kernel, &total_iter_name);
  void *total_iter =
      codegen->CreateVariableDecl("int", total_iter_name, nullptr, func_body);
  codegen->PrependChild(func_body, total_iter);

  void *low = codegen->CreateConst(0);
  void *up = codegen->CreateConst(1);
  for (auto &loop : new_outer_loops) {
    CMarsVariable mv(loop, codegen, write_ref);
    CMarsRangeExpr mr = mv.get_range();
    vector<CMarsExpression> lb_set = mr.get_lb_set();
    assert(lb_set.size() == 1);
    vector<CMarsExpression> ub_set = mr.get_ub_set();
    assert(ub_set.size() == 1);
    CMarsExpression length = ub_set[0] - lb_set[0] + 1;
    up = codegen->CreateExp(V_SgMultiplyOp, up, length.get_expr_from_coeff());
  }
  void *outer_up = codegen->CopyExp(up);
  //  plus one for double buffer
  up = codegen->CreateExp(V_SgAddOp, up, codegen->CreateConst(1));
  map<void *, void *> loop2new_iter;
  for (auto &loop : write_inner_loops) {
    void *loop_iter = codegen->GetLoopIterator(loop);
    string iter_name =
        var_name + "_" + codegen->GetVariableName(loop_iter) + "_r";
    codegen->get_valid_local_name(user_kernel, &iter_name);
    void *new_loop_iter = codegen->CreateVariableDecl(
        codegen->GetTypebyVar(loop_iter), iter_name, nullptr, func_body);
    codegen->PrependChild(func_body, new_loop_iter);
    loop2new_iter[loop] = new_loop_iter;
    CMarsVariable mv(loop, codegen);
    CMarsRangeExpr mr = mv.get_range();
    vector<CMarsExpression> lb_set = mr.get_lb_set();
    assert(lb_set.size() == 1);
    vector<CMarsExpression> ub_set = mr.get_ub_set();
    assert(ub_set.size() == 1);
    CMarsExpression length = ub_set[0] - lb_set[0] + 1;
    up = codegen->CreateExp(V_SgMultiplyOp, up, length.get_expr_from_coeff());
    void *iter_init = codegen->CreateExp(
        V_SgAssignOp, codegen->CreateVariableRef(new_loop_iter),
        lb_set[0].get_expr_from_coeff());
    vec_loop_iter_init.push_back(iter_init);
  }
  for (auto &loop : read_inner_loops) {
    void *loop_iter = codegen->GetLoopIterator(loop);
    string iter_name =
        var_name + "_" + codegen->GetVariableName(loop_iter) + "_w";
    codegen->get_valid_local_name(user_kernel, &iter_name);
    void *new_loop_iter = codegen->CreateVariableDecl(
        codegen->GetTypebyVar(loop_iter), iter_name, nullptr, func_body);
    codegen->PrependChild(func_body, new_loop_iter);
    loop2new_iter[loop] = new_loop_iter;
    CMarsVariable mv(loop, codegen);
    CMarsRangeExpr mr = mv.get_range();
    vector<CMarsExpression> lb_set = mr.get_lb_set();
    assert(lb_set.size() == 1);
    vector<CMarsExpression> ub_set = mr.get_ub_set();
    assert(ub_set.size() == 1);
    CMarsExpression length = ub_set[0] - lb_set[0] + 1;
    vec_loop_iter_init.push_back(codegen->CreateExp(
        V_SgAssignOp, codegen->CreateVariableRef(new_loop_iter),
        lb_set[0].get_expr_from_coeff()));
  }

  up = codegen->CreateExp(V_SgSubtractOp, up, codegen->CreateConst(1));
  void *post_update;
  {
    post_update = codegen->CreateBasicBlock();
    int max_depth = write_inner_loops.size();
    assert(max_depth > 0);
    for (int j = 0; j != 2; ++j) {
      vector<void *> &nested_loops =
          j == 0 ? write_inner_loops : read_inner_loops;
      //  ++i;
      void *innermost_loop = nested_loops[max_depth - 1];
      void *innermost_iter = loop2new_iter[innermost_loop];
      void *innermost_incr = codegen->CreateStmt(
          V_SgExprStatement,
          codegen->CreateExp(V_SgPlusPlusOp,
                             codegen->CreateVariableRef(innermost_iter)));
      codegen->AppendChild(post_update, innermost_incr);

      for (int i = max_depth - 1; i >= 0; --i) {
        void *curr_loop = nested_loops[i];
        CMarsVariable mv(curr_loop, codegen);
        CMarsRangeExpr mr = mv.get_range();
        CMarsExpression me_ub = mr.get_ub(0);
        CMarsExpression me_lb = mr.get_lb(0);
        void *iter_2 = loop2new_iter[curr_loop];

        //  if (i>UBI) { j++; i = LBI; }
        void *incr_1_cond = codegen->CreateExp(
            V_SgGreaterThanOp, codegen->CreateVariableRef(iter_2),
            me_ub.get_expr_from_coeff());
        void *if_body = codegen->CreateBasicBlock();
        if (i > 0) {
          void *upper_loop = nested_loops[i - 1];
          void *iter_1 = loop2new_iter[upper_loop];
          void *incr_1 = codegen->CreateStmt(
              V_SgExprStatement,
              codegen->CreateExp(V_SgPlusPlusOp,
                                 codegen->CreateVariableRef(iter_1)));
          codegen->AppendChild(if_body, incr_1);
        }
        if (i == 0 && j == 0) {
          void *incr_1 = codegen->CreateStmt(
              V_SgExprStatement,
              codegen->CreateExp(V_SgPlusPlusOp,
                                 codegen->CreateVariableRef(out_iter)));
          codegen->AppendChild(if_body, incr_1);
        }
        void *reset_2 = codegen->CreateStmt(V_SgAssignStatement,
                                            codegen->CreateVariableRef(iter_2),
                                            me_lb.get_expr_from_coeff());
        codegen->AppendChild(if_body, reset_2);

        void *if_stmt = codegen->CreateIfStmt(incr_1_cond, if_body, nullptr);
        codegen->AppendChild(post_update, if_stmt);
      }
    }
  }
  void *loop_body = codegen->CreateBasicBlock();
  void *not_start_cond = codegen->CreateExp(
      V_SgGreaterThanOp, codegen->CreateVariableRef(out_iter),
      codegen->CreateConst(0));
  void *write_fifo_body = codegen->CreateBasicBlock();
  //  create assignement: write fifo
  //  if (out_i > 0)
  //   for (...) (optional parallelized loop)
  //     write_fifo(double_buf[(out_i - 1) % 2]
  void *write_val = nullptr;
  void *dummy_write_ref = nullptr;
  {
    void *buf_pntr = codegen->CreateVariableRef(double_buffer);
    void *port_pntr = codegen->CreateVariableRef(port);
    void *out_iter_ref = codegen->CreateVariableRef(out_iter);
    void *out_iter_minus_one = codegen->CreateExp(V_SgSubtractOp, out_iter_ref,
                                                  codegen->CreateConst(1));
    void *choice_index = codegen->CreateExp(V_SgModOp, out_iter_minus_one,
                                            codegen->CreateConst(2));
    buf_pntr = codegen->CreateExp(V_SgPntrArrRefExp, buf_pntr, choice_index);
    map<void *, void *> paralleled_loop2new_iters;
    void *last_loop_body = nullptr;
    bool first_loop = true;
    for (size_t i = 0; i < dims.size(); ++i) {
      int dim_size = dims[dims.size() - 1 - i];
      auto idx = read_idxs[i];
      void *loop = std::get<0>(idx);
      if ((parallel_bits & (1 << i)) != 0) {
        void *loop_iter = codegen->GetLoopIterator(loop);
        string iter_name = "p_i_" + my_itoa(i);
        codegen->get_valid_local_name(user_kernel, &iter_name);
        void *new_loop_iter = codegen->CreateVariableDecl(
            codegen->GetTypebyVar(loop_iter), iter_name, nullptr, func_body);
        codegen->PrependChild(func_body, new_loop_iter);
        paralleled_loop2new_iters[loop] = new_loop_iter;
        void *loop_body = codegen->CreateBasicBlock();
        void *new_loop = codegen->CreateStmt(
            V_SgForStatement,
            codegen->GetVariableInitializedName(new_loop_iter),
            codegen->CreateConst(0), codegen->CreateConst(dim_size), loop_body);
        if (first_loop) {
          codegen->AppendChild(write_fifo_body, new_loop);
          first_loop = false;
        } else {
          codegen->AppendChild(last_loop_body, new_loop);
        }
        const string parallel_pragma = "ACCEL parallel";
        void *sg_pragma = codegen->CreatePragma(parallel_pragma, loop_body);
        codegen->PrependChild(loop_body, sg_pragma);
        last_loop_body = loop_body;
      }
    }

    // TODO(youxiang): check idx order
    for (auto &idx : read_idxs) {
      void *loop = std::get<0>(idx);
      void *new_index = nullptr;
      if (loop2new_iter.count(loop) > 0) {
        void *new_iter = loop2new_iter[loop];
        new_index = codegen->CreateVariableRef(new_iter);
        new_index = codegen->CreateExp(V_SgMultiplyOp, new_index,
                                       codegen->CreateConst(std::get<1>(idx)));
        new_index = codegen->CreateExp(V_SgAddOp, new_index,
                                       codegen->CreateConst(std::get<2>(idx)));
      } else {
        void *new_iter = paralleled_loop2new_iters[loop];
        new_index = codegen->CreateVariableRef(new_iter);
      }
      buf_pntr = codegen->CreateExp(V_SgPntrArrRefExp, buf_pntr, new_index);
      port_pntr = codegen->CreateExp(V_SgPntrArrRefExp, port_pntr,
                                     codegen->CopyExp(new_index));
    }
    write_val = buf_pntr;
    dummy_write_ref = port_pntr;
    void *sg_stmt = codegen->CreateStmt(V_SgExprStatement, dummy_write_ref);
    if (last_loop_body != nullptr) {
      codegen->AppendChild(last_loop_body, sg_stmt);
    } else {
      codegen->AppendChild(write_fifo_body, sg_stmt);
    }
  }

  void *if_write_fifo =
      codegen->CreateIfStmt(not_start_cond, write_fifo_body, nullptr);
  codegen->AppendChild(loop_body, if_write_fifo);

  void *not_end_cond = codegen->CreateExp(
      V_SgLessThanOp, codegen->CreateVariableRef(out_iter), outer_up);
  void *read_fifo_body = codegen->CreateBasicBlock();
  //  create assignement: read fifo
  //  if (out_i < N)
  //   for (...) (optional parallelized loop)
  //     double_buf[(out_i) % 2] = read_fifo()
  void *dummy_read_ref = nullptr;
  {
    void *buf_pntr = codegen->CreateVariableRef(double_buffer);
    void *port_pntr = codegen->CreateVariableRef(port);
    void *out_iter_ref = codegen->CreateVariableRef(out_iter);
    void *choice_index =
        codegen->CreateExp(V_SgModOp, out_iter_ref, codegen->CreateConst(2));
    buf_pntr = codegen->CreateExp(V_SgPntrArrRefExp, buf_pntr, choice_index);
    map<void *, void *> paralleled_loop2new_iters;
    void *last_loop_body = nullptr;
    bool first_loop = true;
    for (size_t i = 0; i < dims.size(); ++i) {
      int dim_size = dims[dims.size() - 1 - i];
      auto idx = write_idxs[i];
      void *loop = std::get<0>(idx);
      if ((parallel_bits & (1 << i)) != 0) {
        void *loop_iter = codegen->GetLoopIterator(loop);
        string iter_name = "p_i_" + my_itoa(i);
        codegen->get_valid_local_name(user_kernel, &iter_name);
        void *new_loop_iter = codegen->CreateVariableDecl(
            codegen->GetTypebyVar(loop_iter), iter_name, nullptr, func_body);
        codegen->PrependChild(func_body, new_loop_iter);
        paralleled_loop2new_iters[loop] = new_loop_iter;
        void *loop_body = codegen->CreateBasicBlock();
        void *new_loop = codegen->CreateStmt(
            V_SgForStatement,
            codegen->GetVariableInitializedName(new_loop_iter),
            codegen->CreateConst(0), codegen->CreateConst(dim_size), loop_body);
        if (first_loop) {
          codegen->AppendChild(read_fifo_body, new_loop);
          first_loop = false;
        } else {
          codegen->AppendChild(last_loop_body, new_loop);
        }
        const string parallel_pragma = "ACCEL parallel";
        void *sg_pragma = codegen->CreatePragma(parallel_pragma, loop_body);
        codegen->PrependChild(loop_body, sg_pragma);
        last_loop_body = loop_body;
      }
    }
    // TODO(youxiang): check idx order
    for (auto &idx : write_idxs) {
      void *loop = std::get<0>(idx);
      void *new_index = nullptr;
      if (loop2new_iter.count(loop) > 0) {
        void *new_iter = loop2new_iter[loop];
        new_index = codegen->CreateVariableRef(new_iter);
        new_index = codegen->CreateExp(V_SgMultiplyOp, new_index,
                                       codegen->CreateConst(std::get<1>(idx)));
        new_index = codegen->CreateExp(V_SgAddOp, new_index,
                                       codegen->CreateConst(std::get<2>(idx)));
      } else {
        void *new_iter = paralleled_loop2new_iters[loop];
        new_index = codegen->CreateVariableRef(new_iter);
      }
      buf_pntr = codegen->CreateExp(V_SgPntrArrRefExp, buf_pntr, new_index);
      port_pntr = codegen->CreateExp(V_SgPntrArrRefExp, port_pntr,
                                     codegen->CopyExp(new_index));
    }
    void *sg_stmt =
        codegen->CreateStmt(V_SgAssignStatement, buf_pntr, port_pntr);
    dummy_read_ref = port_pntr;
    if (last_loop_body != nullptr) {
      codegen->AppendChild(last_loop_body, sg_stmt);
    } else {
      codegen->AppendChild(read_fifo_body, sg_stmt);
    }
  }
  void *if_read_fifo =
      codegen->CreateIfStmt(not_end_cond, read_fifo_body, nullptr);
  codegen->AppendChild(loop_body, if_read_fifo);

  codegen->AppendChild(loop_body, post_update);

  //  create all the reorder kernel
#if 0
  void *body = codegen->CreateBasicBlock();
  int64_t new_step = 1;
  void *new_loop = codegen->CreateStmt(V_SgForStatement,
                        codegen->GetVariableInitializedName(total_iter),
                        low, up, loop_body, &new_step);
  //  init all iterators
  for (auto &iter_init : vec_loop_iter_init)
    codegen->PrependChild(body, codegen->CreateStmt(
                         V_SgExprStatement, iter_init));

  codegen->AppendChild(body, new_loop);

  codegen->AppendChild(func_body, body);
#else
  void *new_iter_init = codegen->CreateExp(
      V_SgAssignOp, codegen->CreateVariableRef(total_iter), low);
  vec_loop_iter_init.insert(vec_loop_iter_init.begin(), new_iter_init);
  void *new_init_stmt = codegen->CreateStmt(
      V_SgExprStatement, codegen->CreateExpList(vec_loop_iter_init));
  void *new_test_stmt = codegen->CreateStmt(
      V_SgExprStatement,
      codegen->CreateExp(V_SgLessOrEqualOp,
                         codegen->CreateVariableRef(total_iter), up));
  void *new_incr = codegen->CreateExp(V_SgPlusPlusOp,
                                      codegen->CreateVariableRef(total_iter));
  void *new_for_loop = SageBuilder::buildForStatement(
      isSgStatement(static_cast<SgNode *>(new_init_stmt)),
      isSgStatement(static_cast<SgNode *>(new_test_stmt)),
      isSgExpression(static_cast<SgNode *>(new_incr)),
      isSgStatement(static_cast<SgNode *>(loop_body)));

  codegen->AppendChild(func_body, new_for_loop);

#endif
  //  generate channel
  string write_channel_name = "__ch_reorder_w_" + var_name;
  gen_one_channel(codegen, dummy_write_ref, read_ref, write_channel_name,
                  channel_size, 1, data_type, write_val, parallel_bits);

  string channel_read_name = "__ch_reorder_r_" + var_name;
  gen_one_channel(codegen, write_ref, dummy_read_ref, channel_read_name,
                  channel_size, 1, data_type, nullptr, parallel_bits);

  return 1;
}

void check_reorder(CSageCodeGen *codegen, CMarsEdge *edge) {
  comm_type ct = edge->GetCommType();
  if (ct != COMMTYPE_CHANNEL_FIFO) {
    return;
  }
  if (edge->GetProperty("reordering_flag") == "0") {
    return;
  }
  generate_reorder_channel(codegen, edge);
}

int channel_reorder_top(CMarsIrV2 *mars_ir) {
  CSageCodeGen *codegen = mars_ir->get_ast();
  int size = mars_ir->size();
  for (int i = 0; i != size; ++i) {
    CMarsNode *one = mars_ir->get_node(i);
    auto ports = one->get_ports();
    set<void *> set_ports(ports.begin(), ports.end());
    for (int j = 0; j != size; ++j) {
      if (i == j) {
        continue;
      }
      CMarsNode *other = mars_ir->get_node(j);
      auto other_ports = other->get_ports();
      set<void *> set_other_ports(other_ports.begin(), other_ports.end());
      set<void *> shared_ports;
      set_intersection(set_ports.begin(), set_ports.end(),
                       set_other_ports.begin(), set_other_ports.end(),
                       inserter(shared_ports, shared_ports.begin()));
      for (set<void *>::iterator pi = shared_ports.begin();
           pi != shared_ports.end(); ++pi) {
        void *port = *pi;
        CMarsEdge *edge = mars_ir->get_edge_by_node(one, other, port);
        check_reorder(codegen, edge);
      }
    }
  }
  return 0;
}
}  //  namespace MarsCommOpt
