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


#include "cmdline_parser.h"
#include "comm_opt.h"
#include "file_parser.h"
#include "mars_ir_v2.h"
#include "mars_opt.h"
#include "xml_parser.h"
#include "program_analysis.h"

#define _DEBUG_CHOPT_OUTPUT_ 1
using MarsProgramAnalysis::CMarsAccess;
using MarsProgramAnalysis::CMarsRangeExpr;
using std::map;
using std::set;
using std::tuple;
using std::vector;

namespace MarsCommOpt {

int comm_sync_top(CMarsIrV2 *mars_ir, void *pTopFunc) {
  cout << "Communication synchronization for parallelism" << endl;
  bool stop_s = false;
  map<CMarsEdge *, int> edge2anal;
  map<void *, int> loop2spread;
  map<void *, int> node2level;
  set<CMarsEdge *> node_edge;

  while (stop_s == false) {
    int anal_size = edge2anal.size();
    CSageCodeGen *codegen = mars_ir->get_ast();

    for (auto &edge : mars_ir->get_all_edges()) {
      if (node_edge.find(edge) != node_edge.end()) {
        continue;
      }
#if PROJDEBUG
      cout << edge->print();
#endif
      if (!edge->is_ssst()) {
        cout << "Not ssst\n";
        continue;
      }

      void *port = edge->GetPort();
      CMarsNode *node0 = edge->GetNode(0);
      CMarsNode *node1 = edge->GetNode(1);
      int parallel_level0 = 0;
      int parallel_level1 = 0;
#if PROJDEBUG
      cout << "\nAnalysis on port : " << codegen->_p(port) << endl;
#endif
      //  1. Check whether if the parallelism is matched in the edge
      int parallel_match = check_parallel_match(mars_ir, edge);
      if ((parallel_match != 0) && edge2anal.find(edge) == edge2anal.end()) {
        continue;
      }
      cout << "Parallel check didnt match, start analysis \n";

      //  2. Calculate which level need to propagate the parallel pragma
      map<void *, int> loop2parallel;
      int propagate =
          analyze_parallelism(mars_ir, edge, &loop2parallel, loop2spread,
                              &parallel_level0, &parallel_level1);
      if (propagate == 0) {
        continue;
      }

      //  3. Test the continuous of the parallelized loops
      int ret = test_parallel_validity(codegen, node1, loop2parallel,
                                       loop2spread, port);
      ret &= test_parallel_validity(codegen, node0, loop2parallel, loop2spread,
                                    port);

      if (ret == 0) {
        //                node_edge.insert(edge);
        cout << "Validity check fail!\n";
        continue;
      }

      if ((parallel_level0 == 0) && (parallel_level1 == 0)) {
        cout << "Parallel level check fail!\n";
        continue;
      }

      //  4. Insert the parallel pragma according to the above analysis
      insert_parallelism(mars_ir, edge, loop2parallel, &loop2spread,
                         &node2level, parallel_level0, parallel_level1);
    }

    //  Parallel propagation result testing
    edge2anal.clear();
    for (auto &edge : mars_ir->get_all_edges()) {
      if (node_edge.find(edge) != node_edge.end()) {
        continue;
      }
      if (!edge->is_ssst()) {
        cout << "Not ssst\n";
        continue;
      }

#if PROJDEBUG
      void *port = edge->GetPort();
      cout << "Analysis on port : " << codegen->_p(port) << endl;
#endif
      //  5. Check whether is the iterative analysis can be stopped
      int fin = check_propagate_fin(mars_ir, edge, loop2spread);
      if (fin == 0) {
        cout << "\n\nERROR: channel parallelized data access does not match: "
             << endl;
#if PROJDEBUG
        cout << edge->print() << endl;
#endif
        edge2anal[edge] = 1;
      }
    }
    stop_s = static_cast<size_t>(anal_size) >= edge2anal.size();
  }

  cout << "Quit successfully.\n";
  return 0;
}

int test_parallel_validity(CSageCodeGen *codegen, CMarsNode *node,
                           map<void *, int> loop2parallel,
                           map<void *, int> loop2spread, void *port) {
  vector<void *> loops = node->get_loops();  //  unparallelzied loops
  bool gap = false;
  for (int i = static_cast<int>(loops.size()) - 1; i >= 0; i--) {
    void *loop = loops[i];
    if (loop2parallel.find(loop) == loop2parallel.end() &&
        loop2spread.find(loop) == loop2spread.end()) {
      gap = true;
      continue;
    }
    if (gap && loop2parallel.find(loop) != loop2parallel.end()) {
      cout << "[ERROR] Parallelized loops are not continuous\n\n";
      return 0;
    }
  }
  return 1;
}

int check_parallel_match(CMarsIrV2 *mars_ir, CMarsEdge *edge) {
  CMarsAST_IF *ast = edge->get_ast();
  CMarsNode *node0 = edge->GetNode(0);  //  write
  CMarsNode *node1 = edge->GetNode(1);  //  read

  void *array = edge->GetPort();
  vector<size_t> dims;
  void *base_type = nullptr;
  ast->get_type_dimension(ast->GetTypebyVar(array), &base_type, &dims, array);
  void *ref0 = *(node0->get_port_references(array).begin());
  void *ref1 = *(node1->get_port_references(array).begin());
  CMarsAccess ac0(ref0, ast, nullptr);
  CMarsAccess ac1(ref1, ast, nullptr);

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
    if ((ast->IsForStatement(loop0) == 0) &&
        (ast->IsForStatement(loop0) == 0)) {
      continue;
    }
    if (ast->IsForStatement(loop0) != ast->IsForStatement(loop1)) {
      return 0;
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

int check_propagate_fin(CMarsIrV2 *mars_ir, CMarsEdge *edge,
                        const map<void *, int> &loop2spread) {
  //  Yuxin revise, 2016 July-26
  cout << "analyze propagation" << endl;

  CSageCodeGen *codegen = mars_ir->get_ast();
  CMarsNode *node0 = edge->GetNode(0);  //  write
  CMarsNode *node1 = edge->GetNode(1);  //  read
  void *port = edge->GetPort();
  vector<size_t> dims;
  void *base_type = nullptr;
  codegen->get_type_dimension(codegen->GetTypebyVar(port), &base_type, &dims,
                              port);
  void *ref0 = *(node0->get_port_references(port).begin());
  void *ref1 = *(node1->get_port_references(port).begin());

  CMarsAccess ac0(ref0, codegen, nullptr);
  CMarsAccess ac1(ref1, codegen, nullptr);

  //  for each array dim: tuple<var, coeff, const>
  //  vector<tuple<var, int, int> >
  auto dim_to_var0 = ac0.simple_type_v1_get_dim_to_var();
  auto dim_to_var1 = ac1.simple_type_v1_get_dim_to_var();
  if (dim_to_var0.size() != dim_to_var1.size()) {
    return 0;
  }

  //  Analyze which loops should add parallel pragma
  for (size_t i = 0; i < dim_to_var0.size(); i++) {
    auto info0 = dim_to_var0[i];
    auto info1 = dim_to_var1[i];
    void *loop0 = std::get<0>(info0);
    void *loop1 = std::get<0>(info1);

#if PROJDEBUG
    cout << "[tmp info] " << codegen->_p(loop0) << ", " << codegen->_p(loop1)
         << endl;
#endif
    if (loop0 == nullptr && loop1 == nullptr) {
      cout << "Constant index in dimension " << i << endl;
      continue;
    }

    assert(codegen->IsForStatement(loop0) && codegen->IsForStatement(loop1));
    //  if (!codegen->IsForStatement(loop0)) {
    //  reportIllegalRef(ref0, port, codegen);
    //  return false;
    //  }

    //  if (!codegen->IsForStatement(loop1)) {
    //  reportIllegalRef(ref1, port, codegen);
    //  return false;
    //  }

    CMarsLoop *loop_info0 = mars_ir->get_loop_info(loop0);
    CMarsLoop *loop_info1 = mars_ir->get_loop_info(loop1);
    bool is_parallel0 = false;
    bool is_parallel1 = false;
    if ((loop_info0->is_complete_unroll() != 0) ||
        loop2spread.find(loop0) != loop2spread.end()) {
      is_parallel0 = true;
    }

    if ((loop_info1->is_complete_unroll() != 0) ||
        loop2spread.find(loop1) != loop2spread.end()) {
      is_parallel1 = true;
    }

    if (!is_parallel0 || !is_parallel1) {
      return 0;
    }
  }
  return 1;

  //  Yuxin: 2016-06-29, if it is a invalid access, quit comm_sync and report.
  //  Example: A[0][i]
}

int analyze_parallelism(CMarsIrV2 *mars_ir, CMarsEdge *edge,
                        map<void *, int> *loop2parallel,
                        const map<void *, int> &loop2spread,
                        int *parallel_level0, int *parallel_level1) {
  //  Yuxin revise, 2016 July-26
  cout << "analyze propagation" << endl;

  CSageCodeGen *codegen = mars_ir->get_ast();
  CMarsNode *node0 = edge->GetNode(0);  //  write
  CMarsNode *node1 = edge->GetNode(1);  //  read
  void *port = edge->GetPort();
  vector<size_t> dims;
  void *base_type = nullptr;
  codegen->get_type_dimension(codegen->GetTypebyVar(port), &base_type, &dims,
                              port);
  void *ref0 = *(node0->get_port_references(port).begin());
  void *ref1 = *(node1->get_port_references(port).begin());

  CMarsAccess ac0(ref0, codegen, nullptr);
  CMarsAccess ac1(ref1, codegen, nullptr);

  //  for each array dim: tuple<var, coeff, const>
  //  vector<tuple<var, int, int> >
  auto dim_to_var0 = ac0.simple_type_v1_get_dim_to_var();
  auto dim_to_var1 = ac1.simple_type_v1_get_dim_to_var();
  if (dim_to_var0.size() != dim_to_var1.size()) {
    return 0;
  }

  //  Analyze which loops should add parallel pragma
  for (size_t i = 0; i < dim_to_var0.size(); i++) {
    auto info0 = dim_to_var0[i];
    auto info1 = dim_to_var1[i];
    void *loop0 = std::get<0>(info0);
    void *loop1 = std::get<0>(info1);

#if PROJDEBUG
    cout << "[tmp info] " << codegen->_p(loop0) << ", " << codegen->_p(loop1)
         << endl;
#endif
    if (loop0 == nullptr && loop1 == nullptr) {
      cout << "Constant index in dimension " << i << endl;
      continue;
    }

    assert(codegen->IsForStatement(loop0) && codegen->IsForStatement(loop1));
    //  if (!codegen->IsForStatement(loop0)) {
    //  reportIllegalRef(ref0, port, codegen);
    //  return false;
    //  }

    //  if (!codegen->IsForStatement(loop1)) {
    //  reportIllegalRef(ref1, port, codegen);
    //  return false;
    //  }

    CMarsLoop *loop_info0 = mars_ir->get_loop_info(loop0);
    CMarsLoop *loop_info1 = mars_ir->get_loop_info(loop1);
    bool is_parallel0 = false;
    bool is_parallel1 = false;
    if ((loop_info0->is_complete_unroll() != 0) ||
        loop2spread.find(loop0) != loop2spread.end()) {
      is_parallel0 = true;
    }

    if ((loop_info1->is_complete_unroll() != 0) ||
        loop2spread.find(loop1) != loop2spread.end()) {
      is_parallel1 = true;
    }

    if (is_parallel0 && !is_parallel1) {
      (*loop2parallel)[loop1] = 1;
#if PROJDEBUG
      cout << "Pragma need insert in node 1: " << codegen->_p(loop1) << endl;
#endif
      (*parallel_level1)++;
    } else if (!is_parallel0 && is_parallel1) {
      (*loop2parallel)[loop0] = 0;
#if PROJDEBUG
      cout << "Pragma need insert in node 0: " << codegen->_p(loop0) << endl;
#endif
      (*parallel_level0)++;
    }
  }
  return 1;

  //  Yuxin: 2016-06-29, if it is a invalid access, quit comm_sync and report.
  //  Example: A[0][i]
}

void insert_parallelism(CMarsIrV2 *mars_ir, CMarsEdge *edge,
                        const map<void *, int> &loop2parallel,
                        map<void *, int> *loop2spread,
                        map<void *, int> *node2level,
                        const int &parallel_level0,
                        const int &parallel_level1) {
  cout << "Pragma insertion level (" << parallel_level0 << ","
       << parallel_level1 << ")\n";
  CSageCodeGen *codegen = mars_ir->get_ast();
  CMarsNode *node0 = edge->GetNode(0);  //  write
  CMarsNode *node1 = edge->GetNode(1);  //  read
  vector<void *> loops1 = node1->get_loops();
  vector<void *> loops0 = node0->get_loops();

  void *port = edge->GetPort();

  for (int j = 0; j < 2; j++) {
    CMarsNode *node;
    int parallel_level = 0;
    vector<void *> m_loops;

    if (j == 0) {
      parallel_level = parallel_level0;
      node = node0;
      m_loops = loops0;
    } else {
      parallel_level = parallel_level1;
      node = node1;
      m_loops = loops1;
    }

    if (node2level->find(node) != node2level->end()) {
      if ((*node2level)[node] < parallel_level) {
        (*node2level)[node] = parallel_level;
      }
    } else {
      (*node2level)[node] = parallel_level;
    }

    //            for(int i=0; i < parallel_level ; i++) {
    for (auto &l2p : loop2parallel) {
      //            void *inner_loop = m_loops[m_loops.size()- 1 - i];
      void *inner_loop = l2p.first;
      if (loop2spread->find(inner_loop) == loop2spread->end() &&
          l2p.second == j) {
        void *loop_body = codegen->GetLoopBody(inner_loop);
        std::ostringstream oss;
        oss << "ACCEL parallel";
        void *sg_pragma = codegen->CreatePragma(oss.str(), loop_body);
        codegen->PrependChild(loop_body, sg_pragma);
        cout << "Insert pragma level:" << parallel_level << " in node "
             << mars_ir->get_node_idx(node) << endl;

        string port_str = codegen->_p(port);
        codegen->AddComment("Parallel pragma comes from " + port_str,
                            sg_pragma);
        (*loop2spread)[inner_loop] = 1;
      }
    }
  }
}

#if 0
void reportCGParallel(void *sg_loop, void *port, CSageCodeGen *codegen) {
  string sFile = codegen->get_file(sg_loop);
  int nLine = codegen->get_line(sg_loop);
  string str_loop = codegen->UnparseToString(sg_loop).substr(0, 20);
  std::ostringstream oss;
  oss << "Cannot sync parallelism on port \'" << codegen->GetVariableName(port)
      << "\' because the loop \'" << str_loop << "\' (" << sFile << ":" << nLine
      << ") contains unparallelised sub loops" << endl;
  oss << "  Hint: ";
  oss << "  try to interchange the above loop into inner level";
  dump_critical_message("CMOPT", "WARNING", oss.str(), 201, 1);
  return;
}

void reportIllegalRef(void *ref, void *port, CSageCodeGen *codegen) {
  string sFile = codegen->get_file(ref);
  int nLine = codegen->get_line(ref);
  string str_ref = codegen->UnparseToString(ref);
  std::ostringstream oss;
  oss << "Cannot sync parallelism on port \'" << codegen->GetVariableName(port)
      << "\' because array indices on reference \'" << str_ref << "\' ("
      << sFile << ":" << nLine << ") cannot be supported" << endl;
  dump_critical_message("CMOPT", "WARNING", oss.str(), 202, 1);
  return;
}
#endif
/*
     void reportCannotMatch(CMarsEdge *edge, CSageCodeGen *codegen) {

     CMarsNode *node0 = edge->GetNode(0);
     CMarsNode *node1 = edge->GetNode(1);
     vector<void*> loops1 = node1->get_loops();
     vector<void*> loops0 = node0->get_loops();
     void *port = edge->GetPort();

     string sFile0= codegen->get_file(loops1.begin());
     string sFile1= codegen->get_file(loops0.begin());
     int nLine   = codegen->get_line(sg_loop);
     string str_loop = codegen->UnparseToString(sg_loop).substr(0, 20);
     std::ostringstream oss;
     oss << "Comm_sync can NOT be applied due to non-parallelized loop: \'"
     << str_loop << "\' (" << sFile << ":" << nLine << ")" << endl;
     oss << "  Reason: ";
     oss << "parallelism will be coarse-grained. Currently merlin_flow only
   support "
     << " fine-grained loop parallelism.\n";
     oss << "  Tips: ";
     oss << "  try loop interchange so that fine-grained parallelsim can be
   applied.\n"; dump_critical_message("CMOPT", "ERROR", oss.str(), 301); return;
     }
     */
}  //  namespace MarsCommOpt

int comm_sync_top(CSageCodeGen *codegen, void *pTopFunc,
                  const CInputOptions &options) {
  printf("Hello Communication Synchronization for Parallelism... \n");

  CMarsIrV2 mars_ir;
  mars_ir.build_mars_ir(codegen, pTopFunc, false, true);
  MarsCommOpt::assign_node_scheduling(&mars_ir);
  return MarsCommOpt::comm_sync_top(&mars_ir, pTopFunc);
}
