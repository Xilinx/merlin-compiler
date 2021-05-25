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
#include "file_parser.h"
#include "xml_parser.h"

#include "PolyModel.h"
#include "codegen.h"
#include "mars_opt.h"
#include "tldm_annotate.h"

#include "comm_opt.h"
#include "mars_ir_v2.h"
namespace MarsCommOpt {

bool check_dead_node(CMarsNode *node, CMarsIrV2 *mars_ir) {
  auto ports = node->get_ports();
  bool dead_node = false;
  if (ports.empty()) {
    //  no any port
    dead_node = true;
  } else {
    //  only read-only ports
    bool read_only = true;
    for (auto &port : ports) {
      if (node->port_is_write(port) ||
          //  youxiang 20161028 keep read access on external memory
          mars_ir->is_kernel_port(port)) {
        read_only = false;
        break;
      }
    }
    if (read_only) {
      dead_node = true;
    }
  }

  if (dead_node) {
    node->remove_all_statements(true /*keep assert statement */);
    return true;
  }
  return false;
}
}  //  namespace MarsCommOpt

using MarsCommOpt::check_dead_node;
int comm_dead_remove_top(CSageCodeGen *codegen, void *pTopFunc,
                         const CInputOptions &options) {
  size_t i;
  printf("Hello Communication Dead Removal ... \n");

  CMarsIrV2 mars_ir;
  mars_ir.build_mars_ir(codegen, pTopFunc, false, true);
  set<void *> dead_nodes;
  bool Changed = false;
  bool LocalChanged;
  do {
    LocalChanged = false;
    mars_ir.remove_writeonly_shared_local_ports();
    for (i = 0; i != mars_ir.size(); ++i) {
      CMarsNode *node = mars_ir.get_node(i);
      if (dead_nodes.count(node) > 0) {
        continue;
      }
      if (check_dead_node(node, &mars_ir)) {
        dead_nodes.insert(node);
        LocalChanged = true;
      }
    }
    Changed |= LocalChanged;
  } while (LocalChanged);

  //  youxiang 20161117 lift scalar into array to be friendly to range analysis
  for (auto port : mars_ir.get_all_shared_ports()) {
    if (codegen->analysis_lift_scalar(port) != 0) {
      Changed |= codegen->apply_lift_scalar(port);
    }
  }
  return 1;
}
