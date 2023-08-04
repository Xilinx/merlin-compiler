//(C) Copyright 2016-2021 Xilinx, Inc.
//All Rights Reserved.
//
//Licensed to the Apache Software Foundation (ASF) under one
//or more contributor license agreements.  See the NOTICE file
//distributed with this work for additional information
//regarding copyright ownership.  The ASF licenses this file
//to you under the Apache License, Version 2.0 (the
//"License"); you may not use this file except in compliance
//with the License.  You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
//Unless required by applicable law or agreed to in writing,
//software distributed under the License is distributed on an
//"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
//KIND, either express or implied.  See the License for the
//specific language governing permissions and limitations
//under the License. (edited)



#ifndef EMULATOR
__attribute__((packed))
#endif
struct custom_float_array { 
	float drain_data[SYS_ARRAY_NUM_COLS];
};

channel struct custom_float_array ch_drain_chain[SYS_ARRAY_NUM_COLS-1];

#define DRAIN_O_LAST_CHANNEL_DEPTH 1

channel struct custom_float_array ch_drain_O_tree_to_mem __attribute__((depth(DRAIN_O_LAST_CHANNEL_DEPTH)));

__attribute__((autorun))
__attribute__((max_global_work_dim(0)))
__attribute__((num_compute_units(SYS_ARRAY_NUM_COLS)))
__kernel void drain_O_chain_node_kernel() {
	unsigned drain_node_id = get_compute_id(0);

	

	struct ch_data_o_struct PE_data_in;
	struct custom_float_array prev_node_data_in;
	struct custom_float_array data_out;

	while(1) {
		PE_data_in = read_channel_altera(ch_data_o_boundary[drain_node_id]);

		if (drain_node_id != SYS_ARRAY_NUM_COLS-1) {
			prev_node_data_in = read_channel_altera(ch_drain_chain[drain_node_id]);
		}

		#pragma unroll
		for (int i = SYS_ARRAY_NUM_COLS-1 ; i > drain_node_id; --i) {
			data_out.drain_data[i] = prev_node_data_in.drain_data[i];
		}

		data_out.drain_data[drain_node_id] = PE_data_in.data;
		
		if (drain_node_id == 0) {
			write_channel_altera(ch_drain_O_tree_to_mem, data_out);
		} else {
			write_channel_altera(ch_drain_chain[drain_node_id-1], data_out);
		}

	}
}

__attribute__((max_global_work_dim(0)))
__kernel void drain_O_write_tree_root_to_mem_kernel(__global struct custom_float_array* 
        restrict out, int o_num_coalesced_words) {

	
    bool more_words_to_write = true;

    int i = 0;
    int i_next = 1;

    while (more_words_to_write) {

      struct custom_float_array root_data;

      root_data = read_channel_altera(ch_drain_O_tree_to_mem);






      out[i] = root_data;
      i++;


      if (o_num_coalesced_words == i_next) {
        more_words_to_write = false;
      } else {
        i_next++;
      }

    }
}
