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
#include "file_parser.h"
#include "mars_ir_v2.h"

namespace MarsCommOpt {

#define _DEBUG_CHGEN_OUTPUT_ 1

int channel_gen_top(CMarsIrV2 *mars_ir) {
  cout << "Channel Generation" << endl;
  auto ast = mars_ir->get_ast();

  //  for each edge
  for (auto &edge : mars_ir->get_all_edges()) {
    if (edge->GetCommType() != COMMTYPE_CHANNEL_FIFO) {
      continue;
    }

    void *array = edge->GetPort();
    int ch_size = my_atoi(edge->GetProperty("channel_size"));
    int reorder_flag = my_atoi(edge->GetProperty("reorder_flag"));
    int parallel_bit = my_atoi(edge->GetProperty("parallel_bits"));

    CMarsNode *node0 = edge->GetNode(0);  //  write
    CMarsNode *node1 = edge->GetNode(1);  //  read
    assert(node0->port_is_write(edge->GetPort()));

    void *ref0 = *(node0->get_port_references(array).begin());
    void *ref1 = *(node1->get_port_references(array).begin());

    void *data_type = ast->get_array_base_type(array);

    if (reorder_flag == 0) {
      string channel_name = "__ch_" + ast->UnparseToString(array);
      gen_one_channel(mars_ir->get_ast(), ref0, ref1, channel_name, ch_size, 1,
                      data_type, nullptr, parallel_bit);
    } else {
      CMarsScheduleVector schedule0 = node0->get_schedule();
      CMarsScheduleVector schedule1 = node1->get_schedule();
      int sync_level = get_sync_level_from_schedule(schedule0, schedule1);
      if (sync_level % 2 == 0) {
        //  sequential edge
        //  leave it as shared memory in order to avoid potential stuck
        continue;
      }
      generate_reorder_channel(mars_ir->get_ast(), edge);  //  ...
    }
  }
  return 1;
}
}  //  namespace MarsCommOpt
