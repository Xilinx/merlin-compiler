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

using MarsCommOpt::assign_channel_size;
using MarsCommOpt::assign_communication_type;
using MarsCommOpt::assign_node_scheduling;
using MarsCommOpt::channel_gen_top;
using MarsCommOpt::channel_reorder_top;
using MarsCommOpt::comm_refine_top;
using MarsCommOpt::comm_rename_top;
using MarsCommOpt::comm_sync_top;
using MarsCommOpt::false_data_dep_top;

//  pass 1. Refine communication, calculate the
//  pass 2. do loop distribution/flatten, channel generation, and reordering
//  pass 3. Self channel generation
//  pass 4. outline

int comm_opt_top(CSageCodeGen *codegen, void *pTopFunc,
                 const CInputOptions &options) {
  printf("Hello Communication Optimization ... \n");

  CMarsIrV2 mars_ir;
  mars_ir.build_mars_ir(codegen, pTopFunc, false, true);

  //  '-a comm_type'
  if ("comm_type" == options.get_option_key_value("-a", "comm_type")) {
    assign_communication_type(&mars_ir, true, true);
  } else if ("channel_size" ==
             options.get_option_key_value("-a", "channel_size")) {
    assign_communication_type(&mars_ir, true, true);
    assign_channel_size(&mars_ir);
  } else if ("channel_reorder" ==
             options.get_option_key_value("-a", "channel_reorder")) {
    assign_communication_type(&mars_ir, true, true);
    channel_reorder_top(&mars_ir);
  } else if ("false_data_dep" ==
             options.get_option_key_value("-a", "false_data_dep")) {
    assign_communication_type(&mars_ir, true, true);
    false_data_dep_top(&mars_ir, 0);
  } else if ("comm_refine" ==
             options.get_option_key_value("-a", "comm_refine")) {
    assign_communication_type(&mars_ir, true, true);
    comm_refine_top(&mars_ir);
  } else if ("comm_sync" == options.get_option_key_value("-a", "comm_sync")) {
    assign_communication_type(&mars_ir, true, true);
    comm_sync_top(&mars_ir, pTopFunc);
  } else if ("comm_rename" ==
             options.get_option_key_value("-a", "comm_rename")) {
    assign_communication_type(&mars_ir, true, true);
    comm_rename_top(&mars_ir);
  } else {
    //  2.1 assign communication type and channel size
    //  2.2 instantiate channel
    //  2.3 reordering generation
    assign_communication_type(&mars_ir, true, false);
    assign_node_scheduling(&mars_ir);
    assign_channel_size(&mars_ir);
    channel_gen_top(&mars_ir);

    if ("off" == options.get_option_key_value("-a", "false_data_dep")) {
      false_data_dep_top(&mars_ir, 1);
    } else if ("on" == options.get_option_key_value("-a", "false_data_dep")) {
      false_data_dep_top(&mars_ir, 0);
    } else {
      false_data_dep_top(&mars_ir, 2);
    }

    //  2.4 loop distribution and flattening
  }

  return 0;
}
