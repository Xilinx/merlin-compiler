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



static void drain_tree_node_kernel_1_0(hls::stream<o_data_in_pe> &ch_shadow_chain_0, hls::stream<o_data_in_pe> &ch_shadow_chain_1, hls::stream<custom_float2> &ch_drain_tree_1_0)
{
	for (int r0 = 0; r0 < BN_R; r0++)
	for (int o0 = 0; o0 < BN_O; o0++)
	{
		for (int r2 = 0; r2 < R; r2++)
//#pragma HLS UNROLL
		for (int r1 = 0; r1 < BS_R; r1++)
//#pragma HLS UNROLL
		for (int o1 = 0; o1 < BS_O; o1++)
		{
//#pragma HLS UNROLL
			struct o_data_in_pe data0;
			struct o_data_in_pe data1;

			custom_float2 data_out;

			data0 = ch_shadow_chain_0.read();
			data1 = ch_shadow_chain_1.read();

			data_out.drain_data[0] = data0.data;
			data_out.drain_data[1] = data1.data;

			ch_drain_tree_1_0 << data_out;
		}
	}
}

static void drain_tree_node_kernel_1_1(hls::stream<o_data_in_pe> &ch_shadow_chain_2, hls::stream<o_data_in_pe> &ch_shadow_chain_3, hls::stream<custom_float2> &ch_drain_tree_1_1)
{
	for (int r0 = 0; r0 < BN_R; r0++)
	for (int o0 = 0; o0 < BN_O; o0++)
	{
		for (int r2 = 0; r2 < R; r2++)
//#pragma HLS UNROLL
		for (int r1 = 0; r1 < BS_R; r1++)
//#pragma HLS UNROLL
		for (int o1 = 0; o1 < BS_O; o1++)
		{
//#pragma HLS UNROLL
			struct o_data_in_pe data0;
			struct o_data_in_pe data1;

			custom_float2 data_out;

			data0 = ch_shadow_chain_2.read();
			data1 = ch_shadow_chain_3.read();

			data_out.drain_data[0] = data0.data;
			data_out.drain_data[1] = data1.data;

			ch_drain_tree_1_1 << data_out;
		}
	}
}

static void drain_tree_node_kernel_1_2(hls::stream<o_data_in_pe> &ch_shadow_chain_4, hls::stream<o_data_in_pe> &ch_shadow_chain_5, hls::stream<custom_float2> &ch_drain_tree_1_2)
{
	for (int r0 = 0; r0 < BN_R; r0++)
	for (int o0 = 0; o0 < BN_O; o0++)
	{
		for (int r2 = 0; r2 < R; r2++)
		for (int r1 = 0; r1 < BS_R; r1++)
		for (int o1 = 0; o1 < BS_O; o1++)
		{
			struct o_data_in_pe data0;
			struct o_data_in_pe data1;

			custom_float2 data_out;

			data0 = ch_shadow_chain_4.read();
			data1 = ch_shadow_chain_5.read();

			data_out.drain_data[0] = data0.data;
			data_out.drain_data[1] = data1.data;

			ch_drain_tree_1_2 << data_out;
		}
	}
}

static void drain_tree_node_kernel_1_3(hls::stream<o_data_in_pe> &ch_shadow_chain_6, hls::stream<o_data_in_pe> &ch_shadow_chain_7, hls::stream<custom_float2> &ch_drain_tree_1_3)
{
	for (int r0 = 0; r0 < BN_R; r0++)
	for (int o0 = 0; o0 < BN_O; o0++)
	{
		for (int r2 = 0; r2 < R; r2++)
		for (int r1 = 0; r1 < BS_R; r1++)
		for (int o1 = 0; o1 < BS_O; o1++)
		{
			struct o_data_in_pe data0;
			struct o_data_in_pe data1;

			custom_float2 data_out;

			data0 = ch_shadow_chain_6.read();
			data1 = ch_shadow_chain_7.read();

			data_out.drain_data[0] = data0.data;
			data_out.drain_data[1] = data1.data;

			ch_drain_tree_1_3 << data_out;
		}
	}
}

static void drain_tree_node_kernel_2_0(hls::stream<custom_float2> &ch_drain_tree_1_0, hls::stream<custom_float2> &ch_drain_tree_1_1, hls::stream<custom_float4> &ch_drain_tree_2_0)
{
	for (int r0 = 0; r0 < BN_R; r0++)
	for (int o0 = 0; o0 < BN_O; o0++)
	{
		for (int r2 = 0; r2 < R; r2++)
//#pragma HLS UNROLL
		for (int r1 = 0; r1 < BS_R; r1++)
//#pragma HLS UNROLL
		for (int o1 = 0; o1 < BS_O; o1++)
		{
//#pragma HLS UNROLL
			struct custom_float2 data0;
			struct custom_float2 data1;

			struct custom_float4 data_out;

			data0 = ch_drain_tree_1_0.read();
			data1 = ch_drain_tree_1_1.read();

			data_out.drain_data[0] = data0.drain_data[0];
			data_out.drain_data[1] = data0.drain_data[1];
			data_out.drain_data[2] = data1.drain_data[0];
			data_out.drain_data[3] = data1.drain_data[1];

			ch_drain_tree_2_0 << data_out;
		}
	}
}

static void drain_tree_node_kernel_2_1(hls::stream<custom_float2> &ch_drain_tree_1_2, hls::stream<custom_float2> &ch_drain_tree_1_3, hls::stream<custom_float4> &ch_drain_tree_2_1)
{
	for (int r0 = 0; r0 < BN_R; r0++)
	for (int o0 = 0; o0 < BN_O; o0++)
	{
		for (int r2 = 0; r2 < R; r2++)
		for (int r1 = 0; r1 < BS_R; r1++)
		for (int o1 = 0; o1 < BS_O; o1++)
		{
			struct custom_float2 data0;
			struct custom_float2 data1;

			struct custom_float4 data_out;

			data0 = ch_drain_tree_1_2.read();
			data1 = ch_drain_tree_1_3.read();

			data_out.drain_data[0] = data0.drain_data[0];
			data_out.drain_data[1] = data0.drain_data[1];
			data_out.drain_data[2] = data1.drain_data[0];
			data_out.drain_data[3] = data1.drain_data[1];

			ch_drain_tree_2_1 << data_out;
		}
	}
}

static void drain_tree_node_kernel_3_0(hls::stream<custom_float4> &ch_drain_tree_2_0, hls::stream<custom_float4> &ch_drain_tree_2_1, hls::stream<custom_float8> &ch_drain_tree_3_0)
{
	for (int r0 = 0; r0 < BN_R; r0++)
	for (int o0 = 0; o0 < BN_O; o0++)
	{
		for (int r2 = 0; r2 < R; r2++)
		for (int r1 = 0; r1 < BS_R; r1++)
		for (int o1 = 0; o1 < BS_O; o1++)
		{
			struct custom_float4 data0;
			struct custom_float4 data1;

			struct custom_float8 data_out;

			data0 = ch_drain_tree_2_0.read();
			data1 = ch_drain_tree_2_1.read();

			data_out.drain_data[0] = data0.drain_data[0];
			data_out.drain_data[1] = data0.drain_data[1];
			data_out.drain_data[2] = data0.drain_data[2];
			data_out.drain_data[3] = data0.drain_data[3];

			data_out.drain_data[4] = data1.drain_data[0];
			data_out.drain_data[5] = data1.drain_data[1];
			data_out.drain_data[6] = data1.drain_data[2];
			data_out.drain_data[7] = data1.drain_data[3];

			ch_drain_tree_3_0 << data_out;
		}
	}
}

#if BIG_DESIGN == 1
static void write_o_buf(hls::stream<custom_float8> &ch_drain_tree_3_0, data_bitwidth *o_buf)
#else
static void write_o_buf(hls::stream<custom_float4> &ch_drain_tree_2_0, data_bitwidth *o_buf)
#endif
{
	for (int o1 = 0; o1 < BS_O; o1++)
	{
		//#pragma HLS UNROLL
#pragma HLS PIPELINE

#if BIG_DESIGN == 1
		struct custom_float8 root_data;

		root_data = ch_drain_tree_3_0.read();
#else
		struct custom_float4 root_data;

		root_data = ch_drain_tree_2_0.read();
#endif

		for (int o2 = 0; o2 < O; o2++)
		{
#pragma HLS UNROLL
			//int o_buf_idx = (r1 * O + o2) * BS_O + o1;
			int o_buf_idx = o2 * BS_O + o1;

			o_buf[o_buf_idx] = root_data.drain_data[o2];
		}
	}
}

static void read_o_buf(data_bitwidth *o_buf, hls::stream<vec_output_t> &ch_drain_to_postproc)
{
	vec_output_t v;
	//#pragma HLS ARRAY_PARTITION variable=v.data complete
	for (int oo = 0; oo < ((O * BS_O) / OUT_PACK_SIZE); oo++)
	{
#pragma HLS PIPELINE
		for (int ooo = 0; ooo < OUT_PACK_SIZE; ooo++)
		{
#pragma HLS UNROLL
			v.data[ooo] = o_buf[oo * OUT_PACK_SIZE + ooo];
		}

		ch_drain_to_postproc << v;
	}
}

#if BIG_DESIGN == 1
//static void drain_tree_root(hls::stream<custom_float8> &ch_drain_tree_3_0, data_bitwidth *ofmap)
static void drain_tree_root(hls::stream<custom_float8> &ch_drain_tree_3_0, hls::stream<vec_output_t> &ch_drain_to_postproc)
#else
//static void drain_tree_root(hls::stream<custom_float4> &ch_drain_tree_2_0, data_bitwidth *ofmap)
static void drain_tree_root(hls::stream<custom_float4> &ch_drain_tree_2_0, hls::stream<vec_output_t> &ch_drain_to_postproc)
#endif
{
	data_bitwidth o_buf0[O * BS_O];
	data_bitwidth o_buf1[O * BS_O];

#pragma HLS ARRAY_PARTITION variable=o_buf0 complete
#pragma HLS ARRAY_PARTITION variable=o_buf1 complete

	bool buffer_id_flag = 1;

#if (BIG_DESIGN == 1)
	write_o_buf(ch_drain_tree_3_0, o_buf0);
#else
	write_o_buf(ch_drain_tree_2_0, o_buf0);
#endif

//	for (int r0 = 0; r0 < BN_R; r0++)
//	for (int o0 = 0; o0 < BN_O; o0++)
//	{
//		for (int r2 = 0; r2 < R; r2++)
//		for (int r1 = 0; r1 < BS_R; r1++)
	//	for (int o1 = 0; o1 < BS_O; o1++)
	//	int r2 = 0;
	//	int r1 = 0;
	//	int o1 = 0;
		for (int it = 0; it < (BN_R * BN_O * R * BS_R); it++)
		{
			if (buffer_id_flag)
			{
				read_o_buf(o_buf0, ch_drain_to_postproc);
#if (BIG_DESIGN == 1)
				write_o_buf(ch_drain_tree_3_0, o_buf1);
#else
				write_o_buf(ch_drain_tree_2_0, o_buf1);
#endif
			}
			else 
			{
#if (BIG_DESIGN == 1)
				write_o_buf(ch_drain_tree_3_0, o_buf0);
#else
				write_o_buf(ch_drain_tree_2_0, o_buf0);
#endif
				read_o_buf(o_buf1, ch_drain_to_postproc);
			}

			buffer_id_flag = !buffer_id_flag;
	
		}
	read_o_buf(o_buf1, ch_drain_to_postproc);

//	}
}
