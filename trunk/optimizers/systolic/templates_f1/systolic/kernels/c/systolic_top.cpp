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

//typedef data_bitwidth vec_float_t[SYS_ARRAY_PE_DSIZE];

//typedef ap_int<sizeof(data_bitwidth) * IN_PACK_SIZE> ap_in_type;
//typedef ap_int<sizeof(data_bitwidth) * OUT_PACK_SIZE> ap_out_type;

#if (DEBUG_AP_INT_PACK == 1)
typedef ap_int<DATA_BITWIDTH * IN_PACK_SIZE> ap_in_type;
typedef ap_int<DATA_BITWIDTH * OUT_PACK_SIZE> ap_out_type;
#endif

//#if (IN_PACK_SIZE == 1)
//typedef ap_int<8> ap_in_type;
//typedef ap_int<8> ap_out_type;
//#elif (IN_PACK_SIZE == 4)
//typedef ap_int<32> ap_in_type;
//typedef ap_int<32> ap_out_type;
//#elif (IN_PACK_SIZE == 8)
//typedef ap_int<512> ap_in_type;
//typedef ap_int<512> ap_out_type;
//#elif (IN_PACK_SIZE == 16)
//typedef ap_int<128> ap_in_type;
//typedef ap_int<128> ap_out_type;
//#else
//#endif

struct in_data_loader_to_feeder
{
#if (DEBUG_AP_INT_PACK == 1)
	ap_in_type pe_data;
#else
	vec_input_t pe_data;
#endif
	char new_row_col_pair;
};

struct in_data_feeder_to_pe
{
#if (DEBUG_AP_INT_PACK == 1)
	ap_in_type pe_data;
#else
	vec_input_t pe_data;
#endif
	char new_row_col_pair;
};

struct wt_data_loader_to_feeder
{
#if (DEBUG_AP_INT_PACK == 1)
	ap_in_type pe_data;
#else
	vec_input_t pe_data;
#endif
};

struct wt_data_feeder_to_pe
{
#if (DEBUG_AP_INT_PACK == 1)
	ap_in_type pe_data;
#else
	vec_input_t pe_data;
#endif
};

struct o_data_in_pe
{
	data_bitwidth data;
};

struct O_out_shift_reg_struct
{
	data_bitwidth data;
	bool valid;
};

//struct custom_float_array
//{
//	data_bitwidth drain_data[SYS_ARRAY_NUM_COLS];
//};

struct o_data_from_pe
{
#if (DEBUG_AP_INT_PACK == 1)
	ap_out_type drain_data;
#else
	vec_output_t drain_data;
#endif
};

#if (DEBUG_AP_INT_PACK == 1)
static void vec_in_data_to_ap_in_data(data_bitwidth *vec_data, ap_in_type &ap_data)
{
#pragma HLS inline

	for (int i2 = 0; i2 < IN_PACK_SIZE; i2++)
	{
#pragma HLS UNROLL
	//	ap_data(sizeof(data_bitwidth) * (i2 + 1) - 1, sizeof(data_bitwidth) * i2) = vec_data[i2];
		ap_data(DATA_BITWIDTH * (i2 + 1) - 1, DATA_BITWIDTH * i2) = vec_data[i2];
	}
//#if (IN_PACK_SIZE == 1 || \
//		IN_PACK_SIZE == 4 || \
//		IN_PACK_SIZE == 8 || \
//		IN_PACK_SIZE == 16)
//	ap_data(7,0) = vec_data[0];
//#endif
//
//#if (IN_PACK_SIZE == 4 || \
//		IN_PACK_SIZE == 8 || \
//		IN_PACK_SIZE == 16)
//	ap_data(15,8) = vec_data[1];
//	ap_data(23,16) = vec_data[2];
//	ap_data(31,24) = vec_data[3];
//#endif
//
//#if (IN_PACK_SIZE == 8 || \
//		IN_PACK_SIZE == 16)
//	ap_data(39,32) = vec_data[4];
//	ap_data(47,40) = vec_data[5];
//	ap_data(55,48) = vec_data[6];
//	ap_data(63,56) = vec_data[7];
//#endif
//
//#if (IN_PACK_SIZE == 16)
//	ap_data(71,512) = vec_data[8];
//	ap_data(79,72) = vec_data[9];
//	ap_data(87,80) = vec_data[10];
//	ap_data(95,88) = vec_data[11];
//
//	ap_data(103,96) = vec_data[12];
//	ap_data(111,104) = vec_data[13];
//	ap_data(119,112) = vec_data[14];
//	ap_data(127,120) = vec_data[15];
//#endif
}

static void ap_in_data_to_vec_in_data(ap_in_type &ap_data, data_bitwidth *vec_data)
{
#pragma HLS inline

	for (int i2 = 0; i2 < I; i2++)
	{
#pragma HLS UNROLL
	//	vec_data[i2] = ap_data(sizeof(data_bitwidth) * (i2 + 1) - 1, sizeof(data_bitwidth) * i2);
		vec_data[i2] = ap_data(DATA_BITWIDTH * (i2 + 1) - 1, DATA_BITWIDTH * i2);
	}
//#if (IN_PACK_SIZE == 1 || \
//		IN_PACK_SIZE == 4 || \
//		IN_PACK_SIZE == 8 || \
//		IN_PACK_SIZE == 16)
//	vec_data[0] = ap_data(7,0);
//#endif
//
//#if (IN_PACK_SIZE == 4 || \
//		IN_PACK_SIZE == 8 || \
//		IN_PACK_SIZE == 16)
//	vec_data[1] = ap_data(15,8);
//	vec_data[2] = ap_data(23,16);
//	vec_data[3] = ap_data(31,24);
//#endif
//
//#if (IN_PACK_SIZE == 8 || \
//		IN_PACK_SIZE == 16)
//	vec_data[4] = ap_data(39,32);
//	vec_data[5] = ap_data(47,40);
//	vec_data[6] = ap_data(55,48);
//	vec_data[7] = ap_data(63,56);
//#endif
//
//#if (IN_PACK_SIZE == 16)
//	vec_data[8] = ap_data(71,512);
//	vec_data[9] = ap_data(79,72);
//	vec_data[10] = ap_data(87,80);
//	vec_data[11] = ap_data(95,88);
//
//	vec_data[12] = ap_data(103,96);
//	vec_data[13] = ap_data(111,104);
//	vec_data[14] = ap_data(119,112);
//	vec_data[15] = ap_data(127,120);
//#endif
}

static void vec_out_data_to_ap_out_data(data_bitwidth *vec_data, ap_out_type &ap_data)
{
#pragma HLS inline

	for (int o2 = 0; o2 < OUT_PACK_SIZE; o2++)
	{
#pragma HLS UNROLL
		ap_data(DATA_BITWIDTH * (o2 + 1) - 1, DATA_BITWIDTH * o2) = vec_data[o2];
	}

//#if (IN_PACK_SIZE == 1 || \
//		IN_PACK_SIZE == 4 || \
//		IN_PACK_SIZE == 8 || \
//		IN_PACK_SIZE == 16)
//	ap_data(7,0) = vec_data[0];
//#endif
//
//#if (IN_PACK_SIZE == 4 || \
//		IN_PACK_SIZE == 8 || \
//		IN_PACK_SIZE == 16)
//	ap_data(15,8) = vec_data[1];
//	ap_data(23,16) = vec_data[2];
//	ap_data(31,24) = vec_data[3];
//#endif
//
//#if (IN_PACK_SIZE == 8 || \
//		IN_PACK_SIZE == 16)
//	ap_data(39,32) = vec_data[4];
//	ap_data(47,40) = vec_data[5];
//	ap_data(55,48) = vec_data[6];
//	ap_data(63,56) = vec_data[7];
//#endif
//
//#if (IN_PACK_SIZE == 16)
//	ap_data(71,512) = vec_data[8];
//	ap_data(79,72) = vec_data[9];
//	ap_data(87,80) = vec_data[10];
//	ap_data(95,88) = vec_data[11];
//
//	ap_data(103,96) = vec_data[12];
//	ap_data(111,104) = vec_data[13];
//	ap_data(119,112) = vec_data[14];
//	ap_data(127,120) = vec_data[15];
//#endif
}

static void ap_out_data_to_vec_out_data(ap_out_type &ap_data, data_bitwidth *vec_data)
{
#pragma HLS inline

	for (int o2 = 0; o2 < OUT_PACK_SIZE; o2++)
	{
#pragma HLS UNROLL
		vec_data[o2] = ap_data(DATA_BITWIDTH * (o2 + 1) - 1, DATA_BITWIDTH * o2);
	}

//#if (IN_PACK_SIZE == 1 || \
//		IN_PACK_SIZE == 4 || \
//		IN_PACK_SIZE == 8 || \
//		IN_PACK_SIZE == 16)
//	vec_data[0] = ap_data(7,0);
//#endif
//
//#if (IN_PACK_SIZE == 4 || \
//		IN_PACK_SIZE == 8 || \
//		IN_PACK_SIZE == 16)
//	vec_data[1] = ap_data(15,8);
//	vec_data[2] = ap_data(23,16);
//	vec_data[3] = ap_data(31,24);
//#endif
//
//#if (IN_PACK_SIZE == 8 || \
//		IN_PACK_SIZE == 16)
//	vec_data[4] = ap_data(39,32);
//	vec_data[5] = ap_data(47,40);
//	vec_data[6] = ap_data(55,48);
//	vec_data[7] = ap_data(63,56);
//#endif
//
//#if (IN_PACK_SIZE == 16)
//	vec_data[8] = ap_data(71,512);
//	vec_data[9] = ap_data(79,72);
//	vec_data[10] = ap_data(87,80);
//	vec_data[11] = ap_data(95,88);
//
//	vec_data[12] = ap_data(103,96);
//	vec_data[13] = ap_data(111,104);
//	vec_data[14] = ap_data(119,112);
//	vec_data[15] = ap_data(127,120);
//#endif
}
#endif

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

//	hls::stream<in_data_loader_to_feeder> ch_data_row_feeder_chain[R + 1];
	hls::stream<in_data_loader_to_feeder> ch_data_row_feeder_chain[R];
//	hls::stream<in_data_feeder_to_pe> ch_data_in[R][O + 1];
	hls::stream<in_data_feeder_to_pe> ch_data_in[R][O];

//	hls::stream<wt_data_loader_to_feeder> ch_data_col_feeder_chain[O + 1];
	hls::stream<wt_data_loader_to_feeder> ch_data_col_feeder_chain[O];
//	hls::stream<wt_data_feeder_to_pe> ch_data_wt[O][R + 1];
	hls::stream<wt_data_feeder_to_pe> ch_data_wt[O][R];

//	hls::stream<o_data_in_pe> ch_data_o[R + 1][O];
	hls::stream<o_data_in_pe> ch_data_o[R][O];

//	hls::stream<custom_float_array> ch_drain_chain[O + 1];
//	hls::stream<o_data_from_pe> ch_drain_chain[O + 1];
	hls::stream<o_data_from_pe> ch_drain_chain[O];

//	hls::stream<vec_output_t> ch_drain_to_relu;
	hls::stream<o_data_from_pe> ch_drain_to_relu;
//	hls::stream<vec_output_t> ch_relu_to_mem;
	hls::stream<o_data_from_pe> ch_relu_to_mem;


#pragma HLS STREAM variable=ch_data_row_feeder_chain depth=1
#pragma HLS STREAM variable=ch_data_in depth=1
#pragma HLS STREAM variable=ch_data_col_feeder_chain depth=1
#pragma HLS STREAM variable=ch_data_wt depth=1
#pragma HLS STREAM variable=ch_data_o depth=512

#pragma HLS STREAM variable=ch_drain_chain depth=1

#pragma HLS STREAM variable=ch_drain_to_relu depth=1
#pragma HLS STREAM variable=ch_relu_to_mem depth=1


#pragma HLS dataflow

	// Loader IN
	load_in_and_forward(ifmap, ch_data_row_feeder_chain[0]);

	// Loader W
	load_wt_and_forward(weight, ch_data_col_feeder_chain[0]);

	// Feeder IN
//	for (int r2 = 0; r2 < R; r2++)
	for (int r2 = 0; r2 < R - 1; r2++)
	{
#pragma HLS UNROLL

		feed_in(ch_data_row_feeder_chain[r2], ch_data_in[r2][0], ch_data_row_feeder_chain[r2 + 1], r2);
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
//	for (int r2 = 0; r2 < R; r2++)
//#pragma HLS UNROLL
//	for (int o2 = 0; o2 < O; o2++)
//	{
//#pragma HLS UNROLL
//
//		PE(r2, o2, ch_data_in[r2][o2], ch_data_wt[o2][r2], ch_data_in[r2][o2 + 1], ch_data_wt[o2][r2 + 1], ch_data_o[r2][o2], ch_data_o[r2 + 1][o2]);
//	}

	// part 0
	for (int o2 = 0; o2 < O - 1; o2++)
	{
#pragma HLS UNROLL
		PE_0_wrapper(ch_data_in[0][o2], ch_data_wt[o2][0], ch_data_in[0][o2 + 1], ch_data_wt[o2][1], ch_data_o[0][o2]);
	}
	// part 1
	PE_1_wrapper(ch_data_in[0][O - 1], ch_data_wt[O - 1][0], ch_data_wt[O - 1][1], ch_data_o[0][O - 1]);

//	PE_0_0_wrapper(ch_data_in[0][0], ch_data_wt[0][0], ch_data_in[0][1], ch_data_wt[0][1], ch_data_o[1][0]);
//	PE_0_1_wrapper(ch_data_in[0][1], ch_data_wt[1][0], ch_data_in[0][2], ch_data_wt[1][1], ch_data_o[1][1]);
//	PE_0_2_wrapper(ch_data_in[0][2], ch_data_wt[2][0], ch_data_in[0][3], ch_data_wt[2][1], ch_data_o[1][2]);
//	PE_0_3_wrapper(ch_data_in[0][3], ch_data_wt[3][0], ch_data_wt[3][1], ch_data_o[1][3]);

	// part 2 and part 3
	for (int r2 = 1; r2 < R - 1; r2++)
	{
#pragma HLS UNROLL
		// part 2
		for (int o2 = 0; o2 < O - 1; o2++)
		{
#pragma HLS UNROLL
			PE_2_wrapper(ch_data_in[r2][o2], ch_data_wt[o2][r2], ch_data_in[r2][o2 + 1], ch_data_wt[o2][r2 + 1], ch_data_o[r2 - 1][o2], ch_data_o[r2][o2]);
		}
		// part 3
		PE_3_wrapper(ch_data_in[r2][O - 1], ch_data_wt[O - 1][r2], ch_data_wt[O - 1][r2 + 1], ch_data_o[r2 - 1][O - 1], ch_data_o[r2][O - 1]);
	}

//	PE_1_0_wrapper(ch_data_in[1][0], ch_data_wt[0][1], ch_data_in[1][1], ch_data_wt[0][2], ch_data_o[1][0], ch_data_o[2][0]);
//	PE_1_1_wrapper(ch_data_in[1][1], ch_data_wt[1][1], ch_data_in[1][2], ch_data_wt[1][2], ch_data_o[1][1], ch_data_o[2][1]);
//	PE_1_2_wrapper(ch_data_in[1][2], ch_data_wt[2][1], ch_data_in[1][3], ch_data_wt[2][2], ch_data_o[1][2], ch_data_o[2][2]);
//	PE_1_3_wrapper(ch_data_in[1][3], ch_data_wt[3][1], ch_data_wt[3][2], ch_data_o[1][3], ch_data_o[2][3]);
//
//	PE_2_0_wrapper(ch_data_in[2][0], ch_data_wt[0][2], ch_data_in[2][1], ch_data_wt[0][3], ch_data_o[2][0], ch_data_o[3][0]);
//	PE_2_1_wrapper(ch_data_in[2][1], ch_data_wt[1][2], ch_data_in[2][2], ch_data_wt[1][3], ch_data_o[2][1], ch_data_o[3][1]);
//	PE_2_2_wrapper(ch_data_in[2][2], ch_data_wt[2][2], ch_data_in[2][3], ch_data_wt[2][3], ch_data_o[2][2], ch_data_o[3][2]);
//	PE_2_3_wrapper(ch_data_in[2][3], ch_data_wt[3][2], ch_data_wt[3][3], ch_data_o[2][3], ch_data_o[3][3]);

	// part 4
	for (int o2 = 0; o2 < O - 1; o2++)
	{
#pragma HLS UNROLL
		PE_4_wrapper(ch_data_in[R - 1][o2], ch_data_wt[o2][R - 1], ch_data_in[R - 1][o2 + 1], ch_data_o[R - 2][o2], ch_data_o[R - 1][o2]);
	}

//	PE_3_0_wrapper(ch_data_in[3][0], ch_data_wt[0][3], ch_data_in[3][1], ch_data_o[3][0], ch_data_o[4][0]);
//	PE_3_1_wrapper(ch_data_in[3][1], ch_data_wt[1][3], ch_data_in[3][2], ch_data_o[3][1], ch_data_o[4][1]);
//	PE_3_2_wrapper(ch_data_in[3][2], ch_data_wt[2][3], ch_data_in[3][3], ch_data_o[3][2], ch_data_o[4][2]);

	// part 5
	PE_5_wrapper(ch_data_in[R - 1][O - 1], ch_data_wt[O - 1][R - 1], ch_data_o[R - 2][O - 1], ch_data_o[R - 1][O - 1]);
//	PE_3_3_wrapper(ch_data_in[3][3], ch_data_wt[3][3], ch_data_o[3][3], ch_data_o[4][3]);

	// Drain: write to output buffer
	drain_chain_node_last(ch_data_o[R - 1][O - 1], ch_drain_chain[O - 1]);
//	for (int o2 = O - 1; o2 >= 0; o2--)
	for (int o2 = O - 2; o2 >= 0; o2--)
	{
#pragma HLS UNROLL

	//	drain_chain_node(ch_data_o[R][o2], ch_drain_chain[o2 + 1], ch_drain_chain[o2], o2);
		drain_chain_node_wrapper(ch_data_o[R - 1][o2], ch_drain_chain[o2 + 1], ch_drain_chain[o2], o2);
	}

//	drain_write_chain_root_to_mem(ch_drain_chain[0], ofmap);
	drain_chain_root(ch_drain_chain[0], ch_drain_to_relu);

	relu(ch_drain_to_relu, ch_relu_to_mem);
	write_to_ddr(ch_relu_to_mem, ofmap);

}
