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

#include "codegen.h"
#include "mars_ir_v2.h"
#include "mars_ir.h"
#include "program_analysis.h"
#include "rose.h"

#include "mars_opt.h"

#define USED_CODE_IN_COVERAGE_TEST 0

int test_mars_ir_v2_top(CSageCodeGen *codegen, void *pTopFunc,
                        const CInputOptions &options) {
  CMarsIrV2 mars_ir;
  mars_ir.build_mars_ir(codegen, pTopFunc, true, true);
  mars_ir.update_dependence_for_all_edges();
  map<void *, map<void *, bool>> top_loop_2_ports;
  map<void *, vector<void *>> top_loops;
  mars_ir.get_top_loop_info(&top_loop_2_ports, &top_loops);
  cout << "[print] top loop info\n";
  for (auto it : top_loop_2_ports) {
    cout << "Loop : " << codegen->_up(it.first) << endl;
    for (auto itt : it.second) {
      cout << "  port : " << codegen->_up(itt.first) << ", " << itt.second
           << endl;
    }
  }

  int node_num = mars_ir.size();
  for (int i = 0; i != node_num; ++i) {
    CMarsNode *node = mars_ir.get_node(i);
    cout << "node" << i << ":" << endl;
    node->print();
    cout << endl;
    void *kernel_top = node->get_user_kernel();
    vector<int> order_res;
    mars_ir.get_order_of_sub_nodes(kernel_top, vector<int>(), &order_res);
    vector<void *> top_loops;
    for (size_t j = 0; j != order_res.size(); ++j) {
      void *loop =
          mars_ir.get_loop_for_order(kernel_top, vector<int>(1, order_res[j]));
      cout << "sub loop: " << codegen->_p(loop) << endl;
      if (loop != nullptr) {
        top_loops.push_back(loop);
      }
    }
    pair<int, int> range = mars_ir.get_node_range(kernel_top);
    for (int j = range.first; j != range.second; ++j) {
      CMarsNode *node = mars_ir.get_node(j);
      if (node->has_loops()) {
        cout << j << " ... has sub loops\n";
      }
      if (node->is_dead()) {
        cout << j << " ... loops are dead\n";
      }
      node->code_generation();
    }
  }
  for (int i = 0; i != node_num; ++i) {
    cout << "node " << i << " ... remove_all_statements\n";
    CMarsNode *node = mars_ir.get_node(i);
    node->remove_all_statements(true);
  }
  cout << "[TESTING] Total file number is: " << node_num << endl;
  return 0;
}

int test_mars_ir_top(CSageCodeGen *codegen, void *pTopFunc,
                     const CInputOptions &options) {
  {
    cout << "====> pragma outside loop \n";
    CMarsIr mars_ir;
    mars_ir.get_mars_ir(codegen, pTopFunc, options, true, true, false);
    vector<CMirNode *> res;
    mars_ir.get_topological_order_nodes(&res);
    for (auto node : res) {
      void *func = codegen->TraceUpToFuncDecl(node->ref);
      if (node->is_function) {
        cout << "function: " << codegen->GetFuncName(func) << endl;
      } else {
        void *loop = codegen->TraceUpToLoopScope(node->ref);
        cout << "function: " << codegen->GetFuncName(func)
             << ", loop: " << codegen->get_internal_loop_label(loop) << ", "
             << (node->is_fine_grain ? "fine grained" : "coarse grained")
             << endl;
      }
      cout << codegen->_p(node->ref, 0) << endl;
    }
  }
  {
    cout << "====> pragma in loop \n";
    CMarsIr mars_ir;
    mars_ir.get_mars_ir(codegen, pTopFunc, options, true, true, true, true);
    vector<CMirNode *> res;
    mars_ir.get_topological_order_nodes(&res);
    for (auto node : res) {
      void *func = codegen->TraceUpToFuncDecl(node->ref);
      if (node->is_function) {
        cout << "function: " << codegen->GetFuncName(func) << endl;
      } else {
        void *loop = codegen->TraceUpToLoopScope(node->ref);
        cout << "function: " << codegen->GetFuncName(func)
             << ", loop: " << codegen->get_internal_loop_label(loop) << ", "
             << (node->is_fine_grain ? "fine grained" : "coarse grained")
             << endl;
      }
      cout << codegen->_p(node->ref, 0) << endl;
    }
  }
  return 1;
}
