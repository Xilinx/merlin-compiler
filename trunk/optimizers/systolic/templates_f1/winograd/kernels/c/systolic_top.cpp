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



#include <hls_stream.h>
#include <ap_int.h>

#include "systolic_params.h"

#include "systolic_top.h"

#define DEBUG_AP_INT_PACK 1


/*
 * ofmap[o]+=weight[o][i]*ifmap[i]
 * */

typedef ap_int<DATA_BITWIDTH * IN_PACK_SIZE> ap_in_t;
typedef ap_int<DATA_BITWIDTH * (C + K2 - 1) * IN_PACK_SIZE> ap_ifmap_t;
#if (WINOGRAD == 1)
typedef ap_int<DATA_BITWIDTH * (C + K2 - 1) * IN_PACK_SIZE> ap_filter_t;
typedef ap_int<DATA_BITWIDTH * (C + K2 - 1)> ap_pe_out_t;
#else
typedef ap_int<DATA_BITWIDTH * K2 * IN_PACK_SIZE> ap_filter_t;
typedef ap_int<DATA_BITWIDTH * C> ap_pe_out_t;
#endif
typedef ap_int<DATA_BITWIDTH * OUT_PACK_SIZE> ap_out_t;

struct in_data_loader_to_feeder
{
//	vec_input_t pe_data;
	ap_in_t pe_data;
	char new_row_col_pair;
};

struct wt_data_loader_to_feeder
{
//	vec_input_t pe_data;
	ap_in_t pe_data;
};

struct in_data_feeder_to_pe
{
//	vec_input_t pe_data[SYS_ARRAY_NUM_ROWS];
//	ap_in_t pe_data[SYS_ARRAY_NUM_ROWS];
	ap_ifmap_t pe_data;
	char new_row_col_pair;
};

struct wt_data_feeder_to_pe
{
//	vec_input_t pe_data;
	ap_filter_t pe_data;
};

struct o_data_from_pe
{
#if (WINOGRAD == 1)
	data_bitwidth data[C + K2 - 1];
#else
	data_bitwidth data[C];
#endif
};

//struct O_out_shift_reg_struct
//{
//	data_bitwidth data;
//	bool valid;
//};

struct custom_float_array
{
#if (WINOGRAD == 1)
	data_bitwidth drain_data[C + K2 - 1][SYS_ARRAY_NUM_COLS];
#else
	data_bitwidth drain_data[C][SYS_ARRAY_NUM_COLS];
#endif
};

static void vec_in_data_to_ap_in_data(data_bitwidth *vec_data, ap_in_t &ap_data)
{
#pragma HLS inline

	for (int i2 = 0; i2 < IN_PACK_SIZE; i2++)
	{
#pragma HLS UNROLL
	//	ap_data(sizeof(data_bitwidth) * (i2 + 1) - 1, sizeof(data_bitwidth) * i2) = vec_data[i2];
		ap_data(DATA_BITWIDTH * (i2 + 1) - 1, DATA_BITWIDTH * i2) = vec_data[i2];
	}
}

static void ap_in_data_to_vec_in_data(ap_in_t &ap_data, data_bitwidth *vec_data)
{
#pragma HLS inline

	for (int i2 = 0; i2 < I; i2++)
	{
#pragma HLS UNROLL
		vec_data[i2] = ap_data(DATA_BITWIDTH * (i2 + 1) - 1, DATA_BITWIDTH * i2);
	}
}

static void ap_ifmap_data_to_vec_in_data(ap_ifmap_t &ap_data, data_bitwidth *vec_data, int c2_q)
{
#pragma HLS inline

	for (int i2 = 0; i2 < I; i2++)
	{
#pragma HLS UNROLL
		// [c2_q][i2][7~0]
		vec_data[i2] = ap_data(c2_q * IN_PACK_SIZE * DATA_BITWIDTH + i2 * DATA_BITWIDTH + DATA_BITWIDTH - 1, c2_q * IN_PACK_SIZE * DATA_BITWIDTH + i2 * DATA_BITWIDTH);
	//	vec_data[i2] = 1;
	}
}

static void vec_in_data_to_ap_ifmap_data(data_bitwidth *vec_data, ap_ifmap_t &ap_data, int c2_q)
{
#pragma HLS inline

	for (int i2 = 0; i2 < I; i2++)
	{
#pragma HLS UNROLL
		ap_data(c2_q * IN_PACK_SIZE * DATA_BITWIDTH + i2 * DATA_BITWIDTH + DATA_BITWIDTH - 1, c2_q * IN_PACK_SIZE * DATA_BITWIDTH + i2 * DATA_BITWIDTH) = vec_data[i2];
	}
}

static void ap_filter_data_to_vec_in_data(ap_filter_t &ap_data, data_bitwidth *vec_data, int q)
{
#pragma HLS inline

	for (int i2 = 0; i2 < I; i2++)
	{
#pragma HLS UNROLL
		vec_data[i2] = ap_data(q * IN_PACK_SIZE * DATA_BITWIDTH + i2 * DATA_BITWIDTH + DATA_BITWIDTH - 1, q * IN_PACK_SIZE * DATA_BITWIDTH + DATA_BITWIDTH * i2);
	//	vec_data[i2] = 1;
	}
}

static void vec_in_data_to_ap_filter_data(data_bitwidth *vec_data, ap_filter_t &ap_data, int q)
{
#pragma HLS inline

	for (int i2 = 0; i2 < I; i2++)
	{
#pragma HLS UNROLL
		ap_data(q * IN_PACK_SIZE * DATA_BITWIDTH + i2 * DATA_BITWIDTH + DATA_BITWIDTH - 1, q * IN_PACK_SIZE * DATA_BITWIDTH + DATA_BITWIDTH * i2) = vec_data[i2];
	}
}

static void vec_out_data_to_ap_out_data(data_bitwidth *vec_data, ap_out_t &ap_data)
{
#pragma HLS inline

	for (int o2 = 0; o2 < OUT_PACK_SIZE; o2++)
	{
#pragma HLS UNROLL
		ap_data(DATA_BITWIDTH * (o2 + 1) - 1, DATA_BITWIDTH * o2) = vec_data[o2];
	}

}

static void ap_out_data_to_vec_out_data(ap_out_t &ap_data, data_bitwidth *vec_data)
{
#pragma HLS inline

	for (int o2 = 0; o2 < OUT_PACK_SIZE; o2++)
	{
#pragma HLS UNROLL
		vec_data[o2] = ap_data(DATA_BITWIDTH * (o2 + 1) - 1, DATA_BITWIDTH * o2);
	}

}

// kernels
//
#include "loader_in.cpp"

#include "loader_wt.cpp"

#include "feeder_in.cpp"

#include "feeder_wt.cpp"

#include "PE.cpp"

#include "drain_chain_out.cpp"

#include "relu.cpp"

#include "write_to_ddr.cpp"

//void systolic_top(data_bitwidth *ifmap, data_bitwidth *weight, data_bitwidth *ofmap)
void systolic_top(vec_input_t *ifmap, vec_input_t *weight, vec_output_t *ofmap)
//void systolic_top(vec_input_t *ifmap, vec_input_t *weight, data_bitwidth *ofmap)
{
#pragma HLS INTERFACE m_axi port=ifmap offset=slave bundle=ifmap
#pragma HLS INTERFACE m_axi port=weight offset=slave bundle=weight
#pragma HLS INTERFACE m_axi port=ofmap offset=slave bundle=ofmap
#pragma HLS INTERFACE s_axilite port=ifmap bundle=control
#pragma HLS INTERFACE s_axilite port=weight bundle=control
#pragma HLS INTERFACE s_axilite port=ofmap bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

#pragma HLS DATA_PACK variable=ifmap
#pragma HLS DATA_PACK variable=weight
#pragma HLS DATA_PACK variable=ofmap

	hls::stream<in_data_loader_to_feeder> ch_data_row_feeder_chain[R];
	hls::stream<in_data_feeder_to_pe> ch_data_in[R][O];

	hls::stream<wt_data_loader_to_feeder> ch_data_col_feeder_chain[O];
	hls::stream<wt_data_feeder_to_pe> ch_data_wt[O][R];

	hls::stream<o_data_from_pe> ch_data_o[R][O];

	hls::stream<custom_float_array> ch_drain_chain[O];

	hls::stream<vec_output_t> ch_drain_to_relu;
//	hls::stream<vec_output_t> ch_drain_to_mem;
	hls::stream<vec_output_t> ch_relu_to_mem;


#pragma HLS STREAM variable=ch_data_row_feeder_chain depth=1
#pragma HLS STREAM variable=ch_data_in depth=1
#pragma HLS STREAM variable=ch_data_col_feeder_chain depth=1
#pragma HLS STREAM variable=ch_data_wt depth=1
#pragma HLS STREAM variable=ch_data_o depth=8

#pragma HLS STREAM variable=ch_drain_chain depth=1

#pragma HLS STREAM variable=ch_drain_to_relu depth=1
//#pragma HLS STREAM variable=ch_drain_to_mem depth=1
#pragma HLS STREAM variable=ch_relu_to_mem depth=1


#pragma HLS dataflow

	// Loader IN
	load_in_and_forward(ifmap, ch_data_row_feeder_chain[0]);

	// Loader W
	load_wt_and_forward(weight, ch_data_col_feeder_chain[0]);

	// Feeder IN
	for (int r2 = 0; r2 < R - 1; r2++)
	{
#pragma HLS UNROLL

		feed_in(ch_data_row_feeder_chain[r2], ch_data_in[r2][0], ch_data_row_feeder_chain[r2], r2);
	}
	feed_in_last(ch_data_row_feeder_chain[R - 1], ch_data_in[R - 1][0]);

	// Feeder W
//	for (int o2 = 0; o2 < O; o2++)
	for (int o2 = 0; o2 < O - 1; o2++)
	{
#pragma HLS UNROLL

		feed_wt(ch_data_col_feeder_chain[o2], ch_data_wt[o2][0], ch_data_col_feeder_chain[o2 + 1], o2);
	}
	feed_wt_last(ch_data_col_feeder_chain[O - 1], ch_data_wt[O - 1][0]);

	// PE: computation
//	for (int o2 = 0; o2 < O; o2++)
	for (int o2 = 0; o2 < O - 1; o2++)
	{
#pragma HLS UNROLL

	//	PE(o2, ch_data_in[o2], ch_data_wt[o2], ch_data_in[o2 + 1], ch_data_o[o2]);
		PE_0_wrapper(ch_data_in[0][o2], ch_data_wt[o2][0], ch_data_in[0][o2 + 1], ch_data_o[0][o2]);
	}
	PE_1_wrapper(ch_data_in[0][O - 1], ch_data_wt[O - 1][0], ch_data_o[0][O - 1]);


	// Drain: write to output buffer
	drain_chain_node_last(ch_data_o[R - 1][O - 1], ch_drain_chain[O - 1]);
//	for (int o2 = O - 1; o2 >= 0; o2--)
	for (int o2 = O - 2; o2 >= 0; o2--)
	{
#pragma HLS UNROLL

	//	drain_chain_node(ch_data_o[o2], ch_drain_chain[o2 + 1], ch_drain_chain[o2], o2);
		drain_chain_node_wrapper(ch_data_o[R - 1][o2], ch_drain_chain[o2 + 1], ch_drain_chain[o2], o2);
	}

	drain_chain_root(ch_drain_chain[0], ch_drain_to_relu);
//	drain_chain_root(ch_drain_chain[0], ch_drain_to_mem);

	relu(ch_drain_to_relu, ch_relu_to_mem);

	write_to_ddr(ch_relu_to_mem, ofmap);
}
