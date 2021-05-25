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



void drain_chain_node(hls::stream<o_data_from_pe> &ch_data_o, 
		hls::stream<custom_float_array> &ch_drain_chain_in,
		hls::stream<custom_float_array> &ch_drain_chain_out, 
		int drain_node_id, 
		OpCFG &cfg)
{
	struct o_data_from_pe PE_data_in;
	struct custom_float_array prev_node_data_in;
	struct custom_float_array data_out;

#pragma HLS ARRAY_PARTITION variable=prev_node_data_in.drain_data complete
#pragma HLS ARRAY_PARTITION variable=data_out.drain_data complete

//	for (int r0 = 0; r0 < BN_R; r0++)
//	for (int o0 = 0; o0 < BN_O; o0++)
//	{
//		for (int r2 = 0; r2 < R; r2++)
//		for (int r1 = 0; r1 < BS_R; r1++)
//		for (int o1 = 0; o1 < BS_O; o1++)
	//	for (int it = 0; it < (BN_R * BN_O * R * BS_R * BS_O); it++)
		for (int it = 0; it < (cfg.BN_R * cfg.BN_O * R * BS_R * BS_C * BS_O); it++)
		{
	#pragma HLS PIPELINE
			PE_data_in = ch_data_o.read();
	
			if (drain_node_id < SYS_ARRAY_NUM_COLS - 1)
			{
				prev_node_data_in = ch_drain_chain_in.read();
			}

#if (WINOGRAD == 1)
			for (int c2_q = 0; c2_q < (C + K2 - 1); c2_q++)
			{
#pragma HLS UNROLL
				prev_node_data_in.drain_data[c2_q][drain_node_id] = PE_data_in.data[c2_q];
			}
	
			for (int i = SYS_ARRAY_NUM_COLS - 1; i >= 0; i--)
			{
#pragma HLS UNROLL
				{
					for (int c2_q = 0; c2_q < (C + K2 - 1); c2_q++)
					{
#pragma HLS UNROLL
						data_out.drain_data[c2_q][i] = prev_node_data_in.drain_data[c2_q][i];
					}
				}
			}
#else
		//	data_out.drain_data[drain_node_id] = PE_data_in.data;
			for (int c2 = 0; c2 < C; c2++)
			{
#pragma HLS UNROLL
			//	prev_node_data_in.drain_data[drain_node_id][r2] = PE_data_in.data[r2];
				prev_node_data_in.drain_data[c2][drain_node_id] = PE_data_in.data[c2];
			}
	
		//	for (int i = SYS_ARRAY_NUM_COLS - 1; i > drain_node_id; i--)
			for (int i = SYS_ARRAY_NUM_COLS - 1; i >= 0; i--)
			{
#pragma HLS UNROLL
			//	if (i > drain_node_id)
				{
				//	data_out.drain_data[i] = prev_node_data_in.drain_data[i];
					for (int c2 = 0; c2 < C; c2++)
					{
#pragma HLS UNROLL
					//	data_out.drain_data[i][r2] = prev_node_data_in.drain_data[i][r2];
						data_out.drain_data[c2][i] = prev_node_data_in.drain_data[c2][i];
					}
				}
			}
#endif
	
	
			ch_drain_chain_out << data_out;
		}
		
//	}
}

void drain_chain_node_wrapper(hls::stream<o_data_from_pe> &ch_data_o, 
		hls::stream<custom_float_array> &ch_drain_chain_in, 
		hls::stream<custom_float_array> &ch_drain_chain_out, 
		int drain_node_id, 
		OpCFG &cfg)
{
	drain_chain_node(ch_data_o, ch_drain_chain_in, ch_drain_chain_out, drain_node_id, cfg);
}

void drain_chain_node_last(hls::stream<o_data_from_pe> &ch_data_o, 
		hls::stream<custom_float_array> &ch_drain_chain_out, 
		OpCFG &cfg)
{
	struct o_data_from_pe PE_data_in;
	struct custom_float_array prev_node_data_in;
	struct custom_float_array data_out;

//	for (int r0 = 0; r0 < BN_R; r0++)
//	for (int o0 = 0; o0 < BN_O; o0++)
//	{
//		for (int r2 = 0; r2 < R; r2++)
//		for (int r1 = 0; r1 < BS_R; r1++)
//		for (int o1 = 0; o1 < BS_O; o1++)
	//	for (int it = 0; it < (BN_R * BN_O * R * BS_R * BS_O); it++)
	//	for (int it = 0; it < (BN_R * BN_O * R * BS_R * BS_C * BS_O); it++)
		for (int it = 0; it < (cfg.BN_R * cfg.BN_O * R * BS_R * BS_C * BS_O); it++)
		{
	#pragma HLS PIPELINE
			PE_data_in = ch_data_o.read();
	
		//	if (drain_node_id < SYS_ARRAY_NUM_COLS - 1)
		//	{
		//		prev_node_data_in = ch_drain_chain_in.read();
		//	}

#if (WINOGRAD == 1)
			for (int c2_q = 0; c2_q < (C + K2 - 1); c2_q++)
			{
#pragma HLS UNROLL
				prev_node_data_in.drain_data[c2_q][SYS_ARRAY_NUM_COLS - 1] = PE_data_in.data[c2_q];
			}
	
			for (int i = SYS_ARRAY_NUM_COLS - 1; i >= 0; i--)
			{
#pragma HLS UNROLL
				{
					for (int c2_q = 0; c2_q < (C + K2 - 1); c2_q++)
					{
#pragma HLS UNROLL
						data_out.drain_data[c2_q][i] = prev_node_data_in.drain_data[c2_q][i];
					}
				}
			}
#else
		//	data_out.drain_data[drain_node_id] = PE_data_in.data;
			for (int c2 = 0; c2 < C; c2++)
			{
#pragma HLS UNROLL
			//	prev_node_data_in.drain_data[SYS_ARRAY_NUM_COLS - 1][r2] = PE_data_in.data[r2];
				prev_node_data_in.drain_data[c2][SYS_ARRAY_NUM_COLS - 1] = PE_data_in.data[c2];
			}
	
		//	for (int i = SYS_ARRAY_NUM_COLS - 1; i > drain_node_id; i--)
			for (int i = SYS_ARRAY_NUM_COLS - 1; i >= 0; i--)
			{
#pragma HLS UNROLL
			//	if (i > drain_node_id)
				{
				//	data_out.drain_data[i] = prev_node_data_in.drain_data[i];
					for (int c2 = 0; c2 < C; c2++)
					{
#pragma HLS UNROLL
					//	data_out.drain_data[i][r2] = prev_node_data_in.drain_data[i][r2];
						data_out.drain_data[c2][i] = prev_node_data_in.drain_data[c2][i];
					}
				}
			}
#endif
			ch_drain_chain_out << data_out;
		}
		
//	}
}


#if 0
//static void write_o_buf(int it, hls::stream<custom_float_array> &ch_drain_chain_0, data_bitwidth *o_buf)
static void write_o_buf(int it, hls::stream<custom_float_array> &ch_drain_chain_0, custom_float_array &o_buf)
{
//	struct custom_float_array root_data;
//	vec_output_t v;

	if (it < (BN_R * BN_O * R * BS_R * BS_C * BS_O))
	{
	//	root_data = ch_drain_chain_0.read();
		o_buf = ch_drain_chain_0.read();
	}

//	for (int o2 = 0; o2 < O; o2++)
//#pragma HLS UNROLL
//	for (int r2 = 0; r2 < R; r2++)
//	{
//#pragma HLS UNROLL
//		int o_buf_idx = r2 * O + o2;
//
//		o_buf[o_buf_idx] = root_data.drain_data[o2][r2];
//	}
}

//static void read_o_buf(int it, data_bitwidth *o_buf, hls::stream<vec_output_t> &ch_drain_to_relu)
static void read_o_buf(int it, custom_float_array &o_buf, hls::stream<vec_output_t> &ch_drain_to_relu)
{
	vec_output_t v;

	for (int c2 = 0; c2 < C; c2++)
	{
#pragma HLS PIPELINE
	}
	
//	for (int r2 = 0; r2 < R; r2++)
//	{
//#pragma HLS UNROLL
//		for (int o2 = 0; o2 < O; o2++)
//		{
//#pragma HLS UNROLL
//			int o_buf_idx = r2 * O + o2;
//
//			v.data[o2] = o_buf[o_buf_idx];
//		}
//
//		if (it > 0)
//			ch_drain_to_relu << v;
//	}

}

void drain_chain_root(hls::stream<custom_float_array> &ch_drain_chain_0, hls::stream<vec_output_t> &ch_drain_to_relu)
{
//	data_bitwidth o_buf0[O * R];
//	data_bitwidth o_buf1[O * R];
	custom_float_array o_buf0;
	custom_float_array o_buf1;

//#pragma HLS ARRAY_PARTITION variable=o_buf0 complete
//#pragma HLS ARRAY_PARTITION variable=o_buf1 complete

//	for (int r0 = 0; r0 < BN_R; r0++)
//	for (int o0 = 0; o0 < BN_O; o0++)
//	{
		for (int it = 0; it < (BN_R * BN_O * R * BS_R * BS_C * BS_O + 1); it++)
		{
			if ((it & 1) == 0)
			{
				write_o_buf(it, ch_drain_chain_0, o_buf0);
				read_o_buf(it, o_buf1, ch_drain_to_relu);
			}
			else
			{
				read_o_buf(it, o_buf0, ch_drain_to_relu);
				write_o_buf(it, ch_drain_chain_0, o_buf1);
			}
		}
//	}
}
#endif

void write_o_buf(hls::stream<custom_float_array> &ch_drain_chain_0, 
		hls::stream<custom_float_array> &ch_winograd_Y)
{
	struct custom_float_array root_data;

	root_data = ch_drain_chain_0.read();

	ch_winograd_Y << root_data;
}

static void winograd_drain_chain_root_4_3(data_bitwidth *GB, data_bitwidth *Y)
{
#pragma HLS inline
	for (int o2 = 0; o2 < O; o2++)
	{
		Y[o2 * C + 0] = GB[o2 * (C + K2 - 1) + 0] + GB[o2 * (C + K2 - 1) + 1] + GB[o2 * (C + K2 - 1) + 2] + GB[o2 * (C + K2 - 1) + 3] + GB[o2 * (C + K2 - 1) + 4];
		Y[o2 * C + 1] = GB[o2 * (C + K2 - 1) + 1] - GB[o2 * (C + K2 - 1) + 2] + 2 * GB[o2 * (C + K2 - 1) + 3] - 2 * GB[o2 * (C + K2 - 1) + 4];
		Y[o2 * C + 2] = GB[o2 * (C + K2 - 1) + 1] + GB[o2 * (C + K2 - 1) + 2] + 4 * GB[o2 * (C + K2 - 1) + 3] + 4 * GB[o2 * (C + K2 - 1) + 4];
		Y[o2 * C + 3] = GB[o2 * (C + K2 - 1) + 1] - GB[o2 * (C + K2 - 1) + 2] + 8 * GB[o2 * (C + K2 - 1) + 3] - 8 * GB[o2 * (C + K2 - 1) + 4] + GB[o2 * (C + K2 - 1) + 5];
	}
}

void read_o_buf(hls::stream<custom_float_array> &ch_winograd_Y, 
		hls::stream<vec_output_t> &ch_drain_to_relu)
//void read_o_buf(hls::stream<custom_float_array> &ch_winograd_Y, 
//		hls::stream<vec_input_t> &ch_drain_to_relu)
{
	struct custom_float_array root_data;
	vec_output_t v;
//	vec_input_t v;

#if (WINOGRAD == 1)
	data_bitwidth GB[O * (C + K2 - 1)];
	data_bitwidth Y[O * C];

	root_data = ch_winograd_Y.read();

	for (int c2_q = 0; c2_q < (C + K2 - 1); c2_q++)
	{
#pragma HLS UNROLL
		for (int o2 = 0; o2 < O; o2++)
		{
#pragma HLS UNROLL
			GB[o2 *(C + K2 - 1) + c2_q] = root_data.drain_data[c2_q][o2];
		//	GB[o2 *(C + K2 - 1) + c2_q] = 1;
		}
	}

	winograd_drain_chain_root_4_3(GB, Y);

	for (int c2 = 0; c2 < C; c2++)
	{
#pragma HLS PIPELINE
		for (int o2 = 0; o2 < O; o2++)
		{
#pragma HLS UNROLL
			v.data[o2] = Y[o2 * C + c2];
		//	v.data[o2] = 0;
		}

		ch_drain_to_relu << v;
	}
#else
	root_data = ch_winograd_Y.read();

	for (int c2 = 0; c2 < C; c2++)
	{
#pragma HLS UNROLL
//	for (int o2_i2 = 0; o2_i2 < (O / I); o2_i2++)
//	{
//#pragma HLS PIPELINE
//		for (int i2 = 0; i2 < I; i2++)
//		{
//#pragma HLS UNROLL
//			v.data[i2] = root_data.drain_data[c2][o2_i2 * (O / I) + i2];
//		}
//
//		ch_drain_to_relu << v;
//	}
	for (int o2 = 0; o2 < O; o2++)
	{
#pragma HLS UNROLL
		v.data[o2] = root_data.drain_data[c2][o2];
	}
	ch_drain_to_relu << v;
	}
#endif // WINOGRAD
}

void drain_chain_root(hls::stream<custom_float_array> &ch_drain_chain_0,
		hls::stream<vec_output_t> &ch_drain_to_relu, 
		OpCFG &cfg)
//void drain_chain_root(hls::stream<custom_float_array> &ch_drain_chain_0,
//		hls::stream<vec_input_t> &ch_drain_to_relu)
{
//	struct custom_float_array root_data;
//	vec_output_t v;
	hls::stream<custom_float_array> ch_winograd_Y;

#pragma HLS STREAM variable=ch_winograd_Y depth=1

//	for (int r0 = 0; r0 < BN_R; r0++)
//	for (int o0 = 0; o0 < BN_O; o0++)
	for (int r0 = 0; r0 < cfg.BN_R; r0++)
	for (int o0 = 0; o0 < cfg.BN_O; o0++)
	{
		for (int r1 = 0; r1 < BS_R; r1++)
		for (int r2 = 0; r2 < R; r2++)
		for (int c1 = 0; c1 < BS_C; c1++)
		for (int o1 = 0; o1 < BS_O; o1++)
		{
//#pragma HLS PIPELINE
#pragma HLS dataflow
			write_o_buf(ch_drain_chain_0, ch_winograd_Y);
			read_o_buf(ch_winograd_Y, ch_drain_to_relu);
//			root_data = ch_drain_chain_0.read();
//
//			for (int c2 = 0; c2 < C; c2++)
//			{
//#pragma HLS UNROLL
//				for (int o2 = 0; o2 < O; o2++)
//				{
//#pragma HLS UNROLL
//					v.data[o2] = root_data.drain_data[c2][o2];
//				}
//
//				ch_drain_to_relu << v;
//			}
		}
	}
}
