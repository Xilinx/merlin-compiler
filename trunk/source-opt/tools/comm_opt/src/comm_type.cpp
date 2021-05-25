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
//  assign_communication_type()
//  input:  MarsIR
//  output: MarsIR with updated type information annotated for the ports of
//  MarsIR nodes
//  Communication Types:
//   a) Global shared memory: DDR (by default)
//   b) FIFO channel
//   c) ...

#include "comm_opt.h"
#include "mars_ir_v2.h"
#include "mars_opt.h"

using MarsProgramAnalysis::ArrayRangeAnalysis;
using MarsProgramAnalysis::CMarsAccess;
using MarsProgramAnalysis::CMarsExpression;
using MarsProgramAnalysis::CMarsRangeExpr;

using std::cout;
using std::endl;
using std::map;
using std::set;
using std::string;
using std::vector;

namespace MarsCommOpt {

#define _DEBUG_COMMTYPE_OUTPUT_ 1

//  /////////////////////////  /
//  Algorithm description (SSST assumption)
//  0. it is not a host interface variable
//  1. # of access is two
//  2. the first one is write and the second one is read, ignore the inverse
//  edge
//  3. access range is matched
//

void assign_communication_type(CMarsIrV2 *mars_ir, bool check_parallel,
                               bool report) {
  cout << "Assign Communication Type" << endl;
  auto ast = mars_ir->get_ast();
  set<CMarsEdge *> visited;
  //  for each edge
  for (auto edge : mars_ir->get_all_edges()) {
    if (visited.count(edge) > 0) {
      continue;
    }
    visited.insert(edge);
    //  0. check if the array is an host interface
    void *array = edge->GetPort();
    string sFile = ast->get_file(array);
    string sLine = my_itoa(ast->get_line(array));
    string sName = ast->GetVariableName(array);
    string port_info = "\'" + sName + "\'";
    //  1. # of access is two
    vector<CMarsEdge *> edges_of_array;
    for (auto &edge_t : mars_ir->get_all_edges()) {
      void *array_t = edge_t->GetPort();
      if (array_t == array) {
        edges_of_array.push_back(edge_t);
      }
    }
    assert(!edges_of_array.empty());

    if (mars_ir->is_kernel_port(array)) {
      for (auto &one_edge : edges_of_array) {
        visited.insert(one_edge);
        one_edge->SetCommType(COMMTYPE_HOST_IF);
      }
      if (edges_of_array.size() >= 2 && !mars_ir->skip_synchronization(array)) {
        if (report) {
          //  dump_critical_message("CMOPT", "WARNING", msg, 201);
          dump_critical_message(CMOPT_WARNING_9(port_info));
        }
      }

      continue;
    }

    if (edges_of_array.size() > 2) {
      //              string msg = "Setting variable \'" + sName + "\' (" +
      //              sFile + ":"
      //                          + sLine + "\') into shared memory type "
      //                          + "because more than two loops access it\n";

      //  string msg = "The coarse-grained dataflow pipelining cannot be
      //  generated "
      //             "because \'" +
      //             sName + "\' " + "is accessed in more than two loops";
      if (report) {
        //  dump_critical_message("CMOPT", "WARNING", msg, 201);
        dump_critical_message(CMOPT_WARNING_1(port_info));
      }
      //  cout << msg << endl;
      for (auto &one_edge : edges_of_array) {
        visited.insert(one_edge);
        one_edge->SetCommType(COMMTYPE_SHARED_MEM_DDR);
      }
      continue;
    }
    assert(edges_of_array.size() == 2);

    //  2. the first one is write and the second one is read, ignore the inverse
    //  edge
    CMarsNode *node0 = edge->GetNode(0);
    CMarsNode *node1 = edge->GetNode(1);

    bool write0 = node0->port_is_write(array);
    bool write1 = node1->port_is_write(array);

    bool read0 = node0->port_is_read(array);
    bool read1 = node1->port_is_read(array);
    if (read0 && read1) {
      //               string msg = "Setting variable \'" + sName + "\' (" +
      //               sFile + ":"
      //                            + sLine + "\')  into shared memory type "
      //                            + "because both two kernels read it\n";
      //  string msg = "The coarse-grained dataflow pipelining cannot be
      //  generated "
      //             "because \'" +
      //             sName + "\' " + "is read in more than one loop";
      if (report) {
        //  dump_critical_message("CMOPT", "WARNING", msg, 202);
        dump_critical_message(CMOPT_WARNING_2(port_info));
      }
      //     cout << msg << endl;
      for (auto &one_edge : edges_of_array) {
        visited.insert(one_edge);
        one_edge->SetCommType(COMMTYPE_SHARED_MEM_DDR);
      }
      continue;
    }

    if (write0 && write1) {
      //                string msg = "Setting variable \'" + sName + "\' (" +
      //                sFile + ":"
      //                             + sLine + "\') into shared memory type "
      //                             + "because both two kernels write it\n";
      //    string msg = "The coarse-grained dataflow pipelining cannot be
      //    generated "
      //                 "because \'" +
      //                 sName + "\' " + "is writen in more than one loop";

      if (report) {
        //  dump_critical_message("CMOPT", "WARNING", msg, 203);
        dump_critical_message(CMOPT_WARNING_3(port_info));
      }
      //     cout << msg << endl;
      for (auto &one_edge : edges_of_array) {
        visited.insert(one_edge);
        one_edge->SetCommType(COMMTYPE_SHARED_MEM_DDR);
      }
      continue;
    }

    CMarsEdge *reverse_edge =
        edges_of_array[0] == edge ? edges_of_array[1] : edges_of_array[0];
    //  an edge starting from read
    if (!write0) {
      std::swap(node0, node1);
      std::swap(edge, reverse_edge);
      //  delay to set its communication type
    }

    if (node0->is_after(node1)) {
      auto loops = node0->get_common_loops(node1);
      if (loops.empty()) {
        //                   string msg = "Setting variable \'" + sName + "\' ("
        //                   + sFile + ":"
        //                                + sLine + "\') into shared memory type
        //                                "
        //                                + "because the write access is after
        //                                read access\n";
        //  string msg = "The coarse-grained dataflow pipelining cannot be "
        //              "generated because of the possible backward depenednecy
        //              " "on \'" + sName + "\'";
        if (report) {
          //   dump_critical_message("CMOPT", "WARNING", msg, 204);
          dump_critical_message(CMOPT_WARNING_4(port_info));
        }
        //       cout << msg << endl;
        for (auto &one_edge : edges_of_array) {
          visited.insert(one_edge);
          one_edge->SetCommType(COMMTYPE_SHARED_MEM_DDR);
        }
        continue;
      }
    }

    //  3. access number
    {
      set<void *> ref0 = node0->get_port_references(array);
      set<void *> ref1 = node1->get_port_references(array);
      if (ref0.size() != 1 || ref1.size() != 1) {
        //                    string msg = "Setting variable \'" + sName + "\'
        //                    (" + sFile + ":"
        //                                 + sLine + "\') into shared memory
        //                                 type "
        //                                 + "because multiple accesses happen
        //                                 in "
        //                                 + "one kernel\n";
        //        string msg = "The coarse-grained dataflow pipelining cannot be
        //        "
        //                     "generated because \'" +
        //                     sName + "\' " + "is accessed for multiple times
        //                     in a loop";
        if (report) {
          //   dump_critical_message("CMOPT", "WARNING", msg, 205);
          dump_critical_message(CMOPT_WARNING_5(port_info));
        }
        //       cout << msg << endl;
        edge->SetCommType(COMMTYPE_SHARED_MEM_DDR);
        reverse_edge->SetCommType(COMMTYPE_SHARED_MEM_DDR);
        visited.insert(reverse_edge);
        continue;
      }

      CMarsAccess ac0(*ref0.begin(), ast, nullptr);
      CMarsAccess ac1(*ref1.begin(), ast, nullptr);

      if (!ac0.is_simple_type_v1() || !ac1.is_simple_type_v1()) {
        //                   string msg = "Setting variable \'" + sName + "\' ("
        //                   + sFile + ":"
        //                                + sLine + "\') into shared memory type
        //                                "
        //                                + "because its accesses cannot be
        //                                determined "
        //                                "as element by element\n";
        //  string msg = "The coarse-grained dataflow pipelining cannot be "
        //              "generated because the indices of array \'" +
        //              sName + "\' " + "is too complex";
        if (report) {
          //   dump_critical_message("CMOPT", "WARNING", msg, 206);
          dump_critical_message(CMOPT_WARNING_6(port_info));
        }
        //       cout << msg << endl;
        edge->SetCommType(COMMTYPE_SHARED_MEM_DDR);
        reverse_edge->SetCommType(COMMTYPE_SHARED_MEM_DDR);
        visited.insert(reverse_edge);
        continue;
      }
    }
    //  4. access range is matched
    {
      auto loops = node0->get_common_loops(node1);
      void *scope = nullptr;
      if (!loops.empty()) {
        scope = ast->GetLoopBody(loops.back());
      }
      vector<CMarsRangeExpr> range0 =
          node0->get_port_access_union(array, scope, false);
      vector<CMarsRangeExpr> range1 =
          node1->get_port_access_union(array, scope, true);

      bool is_match = true;
      for (size_t i = 0; i < range0.size(); i++) {
        CMarsRangeExpr r0 = range0[i];
        CMarsRangeExpr r1 = range1[i];

        if (!((r0.is_exact() != 0) && (r1.is_exact() != 0))) {
          //                       string msg = "Setting variable \'" + sName +
          //                       "\' (" + sFile + ":"
          //                                    + sLine + "\') into shared
          //                                    memory type "
          //                                    + "because either of their
          //                                    access range is not exact\n";
          //  string msg = "The coarse-grained dataflow pipelining cannot be "
          //              "generated because the range of the read and write "
          //              "access to \'" +
          //              sName + "\' is not matched";
          if (report) {
            //     dump_critical_message("CMOPT", "WARNING", msg, 207);
            dump_critical_message(CMOPT_WARNING_7(port_info));
          }
          //         cout << msg << endl;
          edge->SetCommType(COMMTYPE_SHARED_MEM_DDR);
          reverse_edge->SetCommType(COMMTYPE_SHARED_MEM_DDR);
          visited.insert(reverse_edge);
          is_match = false;
          break;
        }

        CMarsExpression lb0;
        CMarsExpression ub0;
        CMarsExpression lb1;
        CMarsExpression ub1;
        if (!((r0.get_simple_bound(&lb0, &ub0) != 0) &&
              (r1.get_simple_bound(&lb1, &ub1) != 0))) {
          //                       string msg = "Setting variable \'" + sName +
          //                       "\' (" + sFile + ":"
          //                                    + sLine + "\') is set into
          //                                    shared memory type "
          //                                    + "because their access range is
          //                                    not matched\n";
          //  string msg = "The coarse-grained dataflow pipelining cannot be "
          //              "generated because the range of the read and write "
          //              "access to \'" +
          //              sName + "\' " + "is not matched";
          if (report) {
            //     dump_critical_message("CMOPT", "WARNING", msg, 207);
            dump_critical_message(CMOPT_WARNING_7(port_info));
          }
          //         cout << msg << endl;
          edge->SetCommType(COMMTYPE_SHARED_MEM_DDR);
          reverse_edge->SetCommType(COMMTYPE_SHARED_MEM_DDR);
          visited.insert(reverse_edge);
          is_match = false;
          break;
        }

        if (!(((lb0 - lb1 == 0) != 0) && ((ub0 - ub1 == 0) != 0))) {
          //   string msg = "Setting variable \'" + sName + "\' (" + sFile + ":"
          //                + sLine + "\') is set into shared memory type "
          //                + "because their access range is not matched\n";

          //  string msg = "The coarse-grained dataflow pipelining cannot be "
          //              "generated because the range of the read and write "
          //              "access to \'" +
          //              sName + "\' " + "is not matched";

          if (report) {
            //     dump_critical_message("CMOPT", "WARNING", msg, 207);
            dump_critical_message(CMOPT_WARNING_7(port_info));
          }
          //         cout << msg << endl;
          edge->SetCommType(COMMTYPE_SHARED_MEM_DDR);
          reverse_edge->SetCommType(COMMTYPE_SHARED_MEM_DDR);
          visited.insert(reverse_edge);
          is_match = false;
          break;
        }
      }
      if (!is_match) {
        continue;
      }
    }
    //  5. parallel match
    if (check_parallel) {
      int is_parallel = 1;
      int parallel_match = check_parallel_bits(mars_ir, edge, &is_parallel);
      if (parallel_match == 0) {
        //          string msg = "Setting variable \'" + sName + "\' (" + sFile
        //          + ":"
        //                        + sLine + "\') is set into shared memory type
        //                        "
        //                        + "because their parallelism is not
        //                        matched\n";

        //  string msg = "The coarse-grained dataflow pipelining cannot be "
        //              "generated because the parallelism of two references to
        //              "
        //              "\'" +
        //              sName + "\' is not matched";
        if (report) {
          //   dump_critical_message("CMOPT", "WARNING", msg, 208);
          if (is_parallel == 0) {
            dump_critical_message(CMOPT_WARNING_10(port_info));
          } else {
            dump_critical_message(CMOPT_WARNING_8(port_info));
          }
        }
        //   cout << msg << endl;
        edge->SetCommType(COMMTYPE_SHARED_MEM_DDR);
        reverse_edge->SetCommType(COMMTYPE_SHARED_MEM_DDR);
        visited.insert(reverse_edge);
        continue;
      }
    }

    //    string msg = "Setting the variable \'" + sName + "\' (" + sFile + ":"
    //    +
    //                 sLine + "\') into FIFO channel type\n";

    string file_info = "\'" + sName + "\' (" + sFile + ":" + sLine + ")";

#if PROJDEBUG
    if (report) {
      //  dump_critical_message("CMOPT", "INFO", msg, 101, 1);
      dump_critical_message(CMOPT_INFO_1(file_info), 1);
    }
#endif
    //  cout << msg << endl;
    edge->SetCommType(COMMTYPE_CHANNEL_FIFO);
    reverse_edge->SetCommType(COMMTYPE_CHANNEL_FIFO_INV);
    visited.insert(reverse_edge);

#if PROJDEBUG
#if _DEBUG_COMMTYPE_OUTPUT_
    cout << "Find a channel: " << ast->_p(array) << endl;
#endif
#endif
  }
}
}  //  namespace MarsCommOpt
