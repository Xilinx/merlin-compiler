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




//#include "systolic.h"

typedef float data_bitwidth;

//pipe int p0 __attribute__ ((xcl_reqd_pipe_depth(32)))

extern "C" {
void systolic_top(data_bitwidth *ifmap, data_bitwidth *weight, data_bitwidth *ofmap)
{

#pragma HLS INTERFACE m_axi port=ifmap offset=slave bundle=ifmap
#pragma HLS INTERFACE m_axi port=weight offset=slave bundle=weight
#pragma HLS INTERFACE m_axi port=ofmap offset=slave bundle=ofmap
#pragma HLS INTERFACE s_axilite port=ifmap bundle=control
#pragma HLS INTERFACE s_axilite port=weight bundle=control
#pragma HLS INTERFACE s_axilite port=ofmap bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

//	for (int o2 = 0; o2 < O; o2++)
//	for (int o1 = 0; o1 < BS_O; o1++)
//	for (int r1 = 0; r1 < BS_R; r1++)
//	{
//		int ofmap_idx = (o2 * BS_O + o1) * BS_R + r1;
//
//		ofmap[ofmap_idx] = 0;
//		for (int i1 = 0; i1 < BS_I; i1++)
//		{
//			int ifmap_idx = r1 * BS_I + i1;
//			int wt_idx = (o2 * BS_O + o1) * BS_I + i1;
//
//			ofmap[ofmap_idx] += weight[wt_idx] * ifmap[ifmap_idx];
//		}
//	}
	ofmap[0] += weight[0] * ifmap[0];

#if 0
	struct in_data_to_feeder ch_data_row_feeder_chain[FIFO_DEPTH];
//	struct in_data_to_feeder ch_data_in[O + 1][FIFO_DEPTH];
	struct in_data_to_feeder ch_data_in_0[FIFO_DEPTH];
	struct in_data_to_feeder ch_data_in_1[FIFO_DEPTH];
	struct in_data_to_feeder ch_data_in_2[FIFO_DEPTH];

//	struct wt_data_to_feeder ch_data_col_feeder_chain[O + 1][FIFO_DEPTH];
	struct wt_data_to_feeder ch_data_col_feeder_chain_0[FIFO_DEPTH];
	struct wt_data_to_feeder ch_data_col_feeder_chain_1[FIFO_DEPTH];
	struct wt_data_to_feeder ch_data_col_feeder_chain_2[FIFO_DEPTH];

//	struct wt_data_to_feeder ch_data_wt[O][FIFO_DEPTH];
	struct wt_data_to_feeder ch_data_wt_0[FIFO_DEPTH];
	struct wt_data_to_feeder ch_data_wt_1[FIFO_DEPTH];

//	struct o_data_in_pe ch_data_o[O][FIFO_DEPTH];
	struct o_data_in_pe ch_data_o_0[FIFO_DEPTH];
	struct o_data_in_pe ch_data_o_1[FIFO_DEPTH];

//	struct custom_float_array ch_drain_chain[O + 1][FIFO_DEPTH];
	struct custom_float_array ch_drain_chain_0[FIFO_DEPTH];
	struct custom_float_array ch_drain_chain_1[FIFO_DEPTH];
	struct custom_float_array ch_drain_chain_2[FIFO_DEPTH];

//	data_bitwidth ch_drain_to_write_to_ddr[FIFO_DEPTH];

	// Loader IN
	load_in_and_forward(ifmap, ch_data_row_feeder_chain);

	// Loader W
//	load_wt_and_forward(weight, ch_data_col_feeder_chain[0]);
	load_wt_and_forward(weight, ch_data_col_feeder_chain_0);

	// Feeder IN
//	feed_in(ch_data_row_feeder_chain, ch_data_in[0]);
	feed_in(ch_data_row_feeder_chain, ch_data_in_0);

	// Feeder W
//	for (int o2 = 0; o2 < O; o2++)
//	{
//#pragma HLS UNROLL
//
//		feed_wt(ch_data_col_feeder_chain[o2], ch_data_wt[o2], ch_data_col_feeder_chain[o2 + 1], o2);
//
//	}
	feed_wt(ch_data_col_feeder_chain_0, ch_data_wt_0, ch_data_col_feeder_chain_1, 0);
	feed_wt(ch_data_col_feeder_chain_1, ch_data_wt_1, ch_data_col_feeder_chain_2, 1);

	// PE: computation and data propogation
//	for (int o2 = 0; o2 < O; o2++)
//	{
//#pragma HLS UNROLL
//
//		PE(o2, ch_data_in[o2], ch_data_wt[o2], ch_data_in[o2 + 1], ch_data_o[o2]);
//	}
	PE(0, ch_data_in_0, ch_data_wt_0, ch_data_in_1, ch_data_o_0);
	PE(1, ch_data_in_1, ch_data_wt_1, ch_data_in_2, ch_data_o_1);

	// Drain: write to output buffer
//	for (int o2 = O - 1; o2 >= 0; o2--)
//	for (int o2 = 0; o2 < O; o2++)
//	{
//#pragma HLS UNROLL
//
//		drain_chain_node(ch_data_o[o2], ch_drain_chain[o2 + 1], ch_drain_chain[o2], o2);
//	}
	drain_chain_node(ch_data_o_1, ch_drain_chain_2, ch_drain_chain_1, 1);
	drain_chain_node(ch_data_o_0, ch_drain_chain_1, ch_drain_chain_0, 0);

//	drain_write_tree_root_to_mem(ch_drain_chain[0], ofmap);
	drain_write_tree_root_to_mem(ch_drain_chain_0, ofmap);
#endif
}

}
