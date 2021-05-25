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


//  //////////////////////////////////////////  /
//  communication_spilit_top()
//  input:  MarsIR
//  output: updated AST with more SSST pattern
#include <tuple>
#include "cmdline_parser.h"
#include "comm_opt.h"
#include "mars_ir_v2.h"
#include "mars_opt.h"
#include "xml_parser.h"
using std::tuple;
namespace MarsCommOpt {

#define _DEBUG_COMMTYPE_OUTPUT_ 1

//  /////////////////////////  /
//  Algorithm description
//  collect all the ports which occurs in more thant two nodes
//  if one read node is immediately dominated and fully covered
//  by one write node, a new shared port is created and used
//  between the two nodes

bool communication_transformation(CMarsIrV2 *mars_ir, CMarsNode *read_node,
                                  CMarsNode *write_node, void *array) {
  CMarsAST_IF *ast = mars_ir->get_ast();

  void *func_decl = ast->TraceUpToFuncDecl(array);
  void *func_body = ast->GetFuncBody(func_decl);
  void *sg_type = ast->GetTypebyVar(array);
  void *sg_basic_type = ast->GetBaseType(sg_type, false);
  string buf_name = ast->GetVariableName(array) + "_rn";
  ast->get_valid_local_name(func_decl, &buf_name);
  void *new_buf =
      ast->CreateVariableDecl(sg_type, buf_name, nullptr, func_body);
  ast->PrependChild(func_body, new_buf);
  auto ref0 = read_node->get_port_references(array);
  for (auto &ref : ref0) {
    void *var_ref;
    ast->parse_array_ref_from_pntr(ref, &var_ref);
    ast->ReplaceExp(var_ref, ast->CreateVariableRef(new_buf));
  }
  auto ref1 = write_node->get_port_references(array);
  vector<void *> vec_write_ref;
  for (auto &ref : ref1) {
    if (ast->has_write_conservative(ref) != 0) {
      vec_write_ref.push_back(ref);
    }
  }
  for (auto &ref : vec_write_ref) {
    void *write_stmt = ast->TraceUpToStatement(ref);
    void *write_exp = ast->GetExprFromStmt(write_stmt);
    void *copy_ref_read = nullptr;
    void *copy_ref_write = ast->CopyExp(ref);
    if (ast->IsAssignOp(write_exp) != 0) {
      void *val_exp = ast->GetExpRightOperand(write_exp);
      string tmp_name = ast->GetVariableName(array) + "_tmp";
      ast->get_valid_local_name(func_decl, &tmp_name);
      void *new_tmp =
          ast->CreateVariableDecl(sg_basic_type, tmp_name, nullptr, func_body);
      ast->PrependChild(func_body, new_tmp);
      void *assign_to_tmp =
          ast->CreateStmt(V_SgAssignStatement, ast->CreateVariableRef(new_tmp),
                          ast->CopyExp(val_exp));
      ast->InsertStmt(assign_to_tmp, write_stmt);
      write_node->insert_statement(assign_to_tmp, write_stmt);
      ast->ReplaceExp(val_exp, ast->CreateVariableRef(new_tmp));
      copy_ref_read = ast->CreateVariableRef(new_tmp);
    } else {
      copy_ref_read = ast->CopyExp(ref);
    }
    void *new_write_stmt =
        ast->CreateStmt(V_SgAssignStatement, copy_ref_write, copy_ref_read);
    ast->InsertAfterStmt(new_write_stmt, write_stmt);
    write_node->insert_statement(new_write_stmt, write_stmt, false);
    void *var_ref;
    ast->parse_array_ref_from_pntr(copy_ref_write, &var_ref);
    ast->ReplaceExp(var_ref, ast->CreateVariableRef(new_buf));
  }
  write_node->update_reference();
  read_node->update_reference();
  return true;
}

bool split_edges(CMarsIrV2 *mars_ir, const vector<CMarsEdge *> &edges_of_array,
                 void *array) {
  assert(edges_of_array.size() >= 2);
  auto ast = mars_ir->get_ast();
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
  string var_name = ast->GetVariableName(array);
  if (all_nodes.size() <= 1) {
    return false;
  }
  if (all_nodes.size() == 2) {
    bool node0_read = all_nodes[0]->port_is_read(array);
    bool node1_read = all_nodes[1]->port_is_read(array);
    bool node0_write = all_nodes[0]->port_is_write(array);
    bool node1_write = all_nodes[1]->port_is_write(array);
    //  if there are read-only and write-only, nothing to do
    if (((node0_read && !node0_write) && (!node1_read && node1_write)) ||
        ((node1_read && !node1_write) && (!node0_read && node0_write))) {
#if PROJDEBUG
      cout << "no need for splitting variable \'" << var_name << "\'"
           << " because of only " << all_nodes.size() << " node(s) access"
           << endl;
#endif
      return false;
    }
  }

#if 1
  if (ast->IsScalarType(array) != 0) {
#if PROJDEBUG
    cout << "Cannot split scalar variable \'" << var_name << "\'"
         << " because its def/use is inaccurate" << endl;
#endif
    return false;
  }
#endif
  //  sort all the nodes according to original schedule vector
  sort(all_nodes.begin(), all_nodes.end(),
       [](CMarsNode *node0, CMarsNode *node1) {
         return !(node0->is_after(node1));
       });
#if PROJDEBUG
  cout << "check variable \'" << var_name << "\'.." << endl;
#endif
  bool Changed = false;
  //  for each read, find its immediate dominated and fully covered write
  //  map<int, int> num_consumers;
  for (size_t i = 0; i != all_nodes.size(); ++i) {
    CMarsNode *node0 = all_nodes[i];
    if (!node0->port_is_read(array) || node0->port_is_write(array)) {
      continue;
    }
    auto res = mars_ir->get_coarse_grained_def(node0, array);
    if (res.size() != 1) {
      continue;
    }
    CMarsNode *node1 = std::get<0>(res[0]);
    communication_transformation(mars_ir, node0, node1, array);
    Changed = true;
  }
  return Changed;
}

int comm_rename_top(CMarsIrV2 *mars_ir) {
  cout << "Split Communication to be single source single target model" << endl;

  //  for each edge
  set<void *> visited;
  bool Changed = false;
  for (auto &edge : mars_ir->get_all_edges()) {
    //  0. check if the array is an host interface
    void *array = edge->GetPort();
    if (mars_ir->is_kernel_port(array)) {
      continue;
    }
    if (visited.count(array) > 0) {
      continue;
    }
    visited.insert(array);
    //  1. # of access is two
    vector<CMarsEdge *> edges_of_array;
    for (auto &edge_t : mars_ir->get_all_edges()) {
      void *array_t = edge_t->GetPort();
      if (array_t == array) {
        edges_of_array.push_back(edge_t);
      }
    }
    assert(!edges_of_array.empty());
    if (edges_of_array.size() >= 2) {
      Changed |= split_edges(mars_ir, edges_of_array, array);
    }
  }
  return static_cast<int>(Changed);
}
}  //  namespace MarsCommOpt

int comm_rename_top(CSageCodeGen *codegen, void *pTopFunc,
                    const CInputOptions &options) {
  printf("Hello Communication Optimization (Renaming)... \n");

  CMarsIrV2 mars_ir;
  mars_ir.build_mars_ir(codegen, pTopFunc, false, true);
  return MarsCommOpt::comm_rename_top(&mars_ir);
}
