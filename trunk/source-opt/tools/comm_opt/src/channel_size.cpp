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


#include "comm_opt.h"
#include "mars_ir_v2.h"

#include "file_parser.h"

#define _DEBUG_CHOPT_OUTPUT_ 1

using MarsProgramAnalysis::CMarsAccess;
using MarsProgramAnalysis::CMarsExpression;
using MarsProgramAnalysis::CMarsRangeExpr;

namespace MarsCommOpt {

int assign_channel_size(CMarsIrV2 *mars_ir) {
  cout << "Assign Channel Size" << endl;
#if PROJDEBUG
  CMarsAST_IF *ast = mars_ir->get_ast();
#endif
  //  for each edge:
  for (auto &edge : mars_ir->get_all_edges()) {
    if (edge->GetCommType() != COMMTYPE_CHANNEL_FIFO) {
      continue;
    }

    CMarsNode *node0 = edge->GetNode(0);  //  write
    CMarsNode *node1 = edge->GetNode(1);  //  read
    assert(node0->port_is_write(edge->GetPort()));
    //  assert(!node1->port_is_write(edge->GetPort())); //  node1 not necessary
    //  to be read
    CMarsScheduleVector schedule0 = node0->get_schedule();
    CMarsScheduleVector schedule1 = node1->get_schedule();

    assert(!schedule0.empty() && !schedule1.empty());

    int sync_level = get_sync_level_from_schedule(schedule0, schedule1);

    int dist = 0;
    if (sync_level % 2 == 0) {
      //  FIXME: to add at least a warning for feedback dependence
      //  assert(node1->is_after(node0));
      //  assert(schedule0[sync_level] < schedule1[sync_level]);

      dist = 2;
    } else {
#if PROJDEBUG
      void *array = edge->GetPort();
      cout << "[assertion] fail on : " << ast->_p(array) << endl;
#endif
      //  2016.07.07 sean & peng, no need to check location
      //  assert(node1->is_after(node0)); //  FIXME: now only support fowarding
      //  dependence
      assert(schedule0[sync_level] < schedule1[sync_level]);

      //  if (node1->is_after(node0) && schedule0[sync_level] <
      //  schedule1[sync_level])
      dist = schedule1[sync_level] - schedule0[sync_level];
      //  else
      //    dist = 1;

      assert(dist > 0);
    }

    int reorder_flag = check_reorder_flag(edge);

    int is_parallel = 1;
    int parallel_match = check_parallel_bits(mars_ir, edge, &is_parallel);
    if (parallel_match == 0) {
      if (is_parallel == 0) {
        cout << "\n\nERROR: channel access indices patterns does not match: "
             << endl;
      } else {
        cout << "\n\nERROR: channel parallelized data access does not match: "
             << endl;
      }

#if PROJDEBUG
      cout << edge->print() << endl;
#endif
      assert(parallel_match);
    }

    //  if (reorder_flag) dist--;  //  Note that reorder buffer is at least one
    //  block of buffer
    dist--;  //  20160603 ZP: balanced for all the cases
    CMarsExpression port_id_size0 =
        node0->get_iteration_domain_size_by_port_and_level(edge->GetPort(),
                                                           sync_level);
    CMarsExpression port_id_size1 =
        node1->get_iteration_domain_size_by_port_and_level(edge->GetPort(),
                                                           sync_level);

    if ((port_id_size0.is_INF() != 0) || (port_id_size1.is_INF() != 0)) {
      cout << "\n\nERROR: channel data access domain is unknown: " << endl;
#if PROJDEBUG
      cout << edge->print() << endl;
      cout << "  w_size : " << port_id_size0.print_s() << endl;
      cout << "  r_size : " << port_id_size1.print_s() << endl;
#endif
    }
    assert(port_id_size0 >= 0);
    assert(port_id_size1 >= 0);
    if ((port_id_size0 != port_id_size1) != 0) {
      cout << "\n\nERROR: channel data access domain does not match: " << endl;
#if PROJDEBUG
      cout << edge->print() << endl;
      cout << "  w_size : " << port_id_size0.print_s() << endl;
      cout << "  r_size : " << port_id_size1.print_s() << endl;
#endif
      assert(port_id_size0 == port_id_size1);
    }

    int64_t channel_size = dist * port_id_size0.get_range().get_const_ub();
    if (channel_size == 0) {
      channel_size = 1;
    }

    //  ZP: fix for Altera device feature: to guarantee performance, channel
    //  size need to be no less than 32
    if (channel_size < 32) {
      channel_size = 32;
    }

    edge->SetProperty("channel_size", my_itoa(channel_size));
    edge->SetProperty("sync_level", my_itoa(sync_level));
    edge->SetProperty("reorder_flag", my_itoa(reorder_flag));

#if PROJDEBUG
#if _DEBUG_CHOPT_OUTPUT_
    cout << "[Channel " << ast->UnparseToString(edge->GetPort())
         << "] size=" << dist << "*" << port_id_size0.print_s() << " sync@"
         << sync_level << "between node" << mars_ir->get_node_idx(node0)
         << " and node" << mars_ir->get_node_idx(node1)
         << " reorder=" << reorder_flag << endl;
#endif
#endif
  }

  return 0;
}

int check_reorder_flag(CMarsEdge *edge) {
  assert(edge->GetCommType() == COMMTYPE_CHANNEL_FIFO);
  CMarsAST_IF *ast = edge->get_ast();

  CMarsNode *node0 = edge->GetNode(0);  //  write
  CMarsNode *node1 = edge->GetNode(1);  //  read
  assert(node0->port_is_write(edge->GetPort()));

  void *array = edge->GetPort();

  void *ref0 = *(node0->get_port_references(array).begin());
  void *ref1 = *(node1->get_port_references(array).begin());

  CMarsAccess ac0(ref0, ast, nullptr);
  CMarsAccess ac1(ref1, ast, nullptr);

  assert(ac0.is_simple_type_v1());
  assert(ac1.is_simple_type_v1());

  //  for each array dim: tuple<var, coeff, const>
  //  vector<tuple<int, int, int> >
  int is_compact = 1;
  auto var_to_dim0 =
      ac0.simple_type_v1_get_var_to_dim(node0->get_loops(), is_compact);
  auto var_to_dim1 =
      ac1.simple_type_v1_get_var_to_dim(node1->get_loops(), is_compact);

  if (var_to_dim0.size() != var_to_dim1.size()) {
    return 1;
  }

  for (size_t i = 0; i < var_to_dim0.size(); i++) {
    auto info0 = var_to_dim0[i];
    auto info1 = var_to_dim1[i];

    if (std::get<0>(info0) != std::get<0>(info1)) {
      return 1;
    }
    if (std::get<1>(info0) != std::get<1>(info1)) {
      return 1;
    }
    //  if (std::get<2>(info0) != std::get<2>(info1)) return 1;
    //  if (info0 != info1) return 1;
  }

  return 0;
}

int check_parallel_bits(CMarsIrV2 *mars_ir, CMarsEdge *edge, int *is_parallel) {
  //  assert(edge->GetCommType() == COMMTYPE_CHANNEL_FIFO);
  CMarsAST_IF *ast = edge->get_ast();

  CMarsNode *node0 = edge->GetNode(0);  //  write
  CMarsNode *node1 = edge->GetNode(1);  //  read
  assert(node0->port_is_write(edge->GetPort()));

  vector<void *> inner_loops0 = node0->get_inner_loops();
  vector<void *> inner_loops1 = node1->get_inner_loops();
  if (inner_loops0.empty() && inner_loops1.empty()) {
    *is_parallel = 0;
  }

  void *array = edge->GetPort();
  vector<size_t> dims;
  void *base_type = nullptr;
  ast->get_type_dimension(ast->GetTypebyVar(array), &base_type, &dims, array);
  void *ref0 = *(node0->get_port_references(array).begin());
  void *ref1 = *(node1->get_port_references(array).begin());

  CMarsAccess ac0(ref0, ast, nullptr);
  CMarsAccess ac1(ref1, ast, nullptr);

  assert(ac0.is_simple_type_v1());
  assert(ac1.is_simple_type_v1());

  //  for each array dim: tuple<var, coeff, const>
  //  vector<tuple<var, int, int> >
  auto dim_to_var0 = ac0.simple_type_v1_get_dim_to_var();
  auto dim_to_var1 = ac1.simple_type_v1_get_dim_to_var();

  if (dim_to_var0.size() != dim_to_var1.size()) {
    return 0;
  }

  int parallel_bit = 0;
  for (size_t i = 0; i < dim_to_var0.size(); i++) {
    auto info0 = dim_to_var0[i];
    auto info1 = dim_to_var1[i];
    void *loop0 = std::get<0>(info0);
    void *loop1 = std::get<0>(info1);
    if (ast->IsForStatement(loop0) != ast->IsForStatement(loop1)) {
      return 0;
    }
    if (ast->IsForStatement(loop0) == 0) {
      continue;
    }
    CMarsLoop *loop_info0 = mars_ir->get_loop_info(loop0);
    CMarsLoop *loop_info1 = mars_ir->get_loop_info(loop1);
    if (loop_info0->is_complete_unroll() != loop_info1->is_complete_unroll()) {
      return 0;
    }
    int dimension = dims[dims.size() - 1 - i];
    if (loop_info0->is_complete_unroll() != 0) {
      CMarsRangeExpr index_range = ac0.GetRangeExpr(i);
      if (!index_range.is_full_access(dimension)) {
        return 0;
      }
      CMarsRangeExpr index_range1 = ac1.GetRangeExpr(i);
      if (!index_range1.is_full_access(dimension)) {
        return 0;
      }
      parallel_bit |= 1 << i;
    }
  }

  edge->SetProperty("parallel_bits", my_itoa(parallel_bit));
  return 1;
}

int assign_node_scheduling(CMarsIrV2 *mars_ir, bool detect_shared_memory) {
  cout << "Assign Node Scheduling" << endl;

  //  defined in coarse_pipeline_altera
  assign_schedule_for_marsIR_nodes(mars_ir, detect_shared_memory);

  return 0;
}
}  //  namespace MarsCommOpt
