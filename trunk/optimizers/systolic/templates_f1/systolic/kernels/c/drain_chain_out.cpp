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



#if 0
//void drain_chain_node(hls::stream<o_data_in_pe> &ch_data_o, hls::stream<custom_float_array> &ch_drain_chain_in, hls::stream<custom_float_array> &ch_drain_chain_out, int drain_node_id)
void drain_chain_node(hls::stream<o_data_in_pe> &ch_data_o, hls::stream<o_data_from_pe> &ch_drain_chain_in, hls::stream<o_data_from_pe> &ch_drain_chain_out, int drain_node_id)
{
	struct o_data_in_pe PE_data_in;

	struct custom_float_array vec_prev_node_data_in;
	struct custom_float_array vec_data_out;

	struct o_data_from_pe prev_node_data_in;
	struct o_data_from_pe data_out;

//	for (int r0 = 0; r0 < BN_R; r0++)
//	for (int o0 = 0; o0 < BN_O; o0++)
//	{
//		for (int r2 = 0; r2 < R; r2++)
//		for (int r1 = 0; r1 < BS_R; r1++)
//		for (int o1 = 0; o1 < BS_O; o1++)
		for (int it = 0; it < (BN_R * BN_O * R * BS_R * BS_O); it++)
		{
	#pragma HLS PIPELINE
			PE_data_in = ch_data_o.read();
	
			if (drain_node_id < SYS_ARRAY_NUM_COLS - 1)
			{
				prev_node_data_in = ch_drain_chain_in.read();
			}

			ap_out_data_to_vec_out_data(prev_node_data_in.drain_data, vec_prev_node_data_in.drain_data);

		//	prev_node_data_in.drain_data[drain_node_id] = PE_data_in.data;
			vec_prev_node_data_in.drain_data[drain_node_id] = PE_data_in.data;
	
		//	for (int i = SYS_ARRAY_NUM_COLS - 1; i > drain_node_id; i--)
		//	for (int i = SYS_ARRAY_NUM_COLS - 1; i >= 0; i--)
			for (int i = 0; i < SYS_ARRAY_NUM_COLS; i++)
			{
#pragma HLS UNROLL
			//	if (i > drain_node_id)
				{
				//	data_out.drain_data[i] = prev_node_data_in.drain_data[i];
					vec_data_out.drain_data[i] = vec_prev_node_data_in.drain_data[i];
				}
			}

			vec_out_data_to_ap_out_data(vec_data_out.drain_data, data_out.drain_data);
	
		//	data_out.drain_data[drain_node_id] = PE_data_in.data;
	
		//	if (drain_node_id != 0)
		//		ch_drain_chain_out << data_out;

			ch_drain_chain_out << data_out;
		}
		
//	}
}
#endif

void drain_chain_node_last(hls::stream<o_data_in_pe> &ch_data_o, hls::stream<o_data_from_pe> &ch_drain_chain_out)
{
	struct o_data_in_pe PE_data_in;

//	struct custom_float_array vec_prev_node_data_in;
//	struct custom_float_array vec_data_out;

	struct o_data_from_pe prev_node_data_in;
	struct o_data_from_pe data_out;

//	for (int r0 = 0; r0 < BN_R; r0++)
//	for (int o0 = 0; o0 < BN_O; o0++)
//	{
//		for (int r2 = 0; r2 < R; r2++)
//		for (int r1 = 0; r1 < BS_R; r1++)
//		for (int o1 = 0; o1 < BS_O; o1++)
		for (int it = 0; it < (BN_R * BN_O * R * BS_R * BS_O); it++)
		{
	#pragma HLS PIPELINE
			PE_data_in = ch_data_o.read();
	
#if (DEBUG_AP_INT_PACK == 1)
			prev_node_data_in.drain_data(DATA_BITWIDTH * (SYS_ARRAY_NUM_COLS - 1 + 1) - 1, DATA_BITWIDTH * (SYS_ARRAY_NUM_COLS - 1)) = PE_data_in.data;
#else
			prev_node_data_in.drain_data.data[SYS_ARRAY_NUM_COLS - 1] = PE_data_in.data;
		//	vec_prev_node_data_in.drain_data[SYS_ARRAY_NUM_COLS - 1] = PE_data_in.data;
#endif
	
#if (DEBUG_AP_INT_PACK == 1)
			data_out = prev_node_data_in;
#else
		//	for (int i = SYS_ARRAY_NUM_COLS - 1; i > drain_node_id; i--)
		//	for (int i = SYS_ARRAY_NUM_COLS - 1; i >= 0; i--)
			for (int i = 0; i < SYS_ARRAY_NUM_COLS; i++)
			{
#pragma HLS UNROLL
			//	if (i > drain_node_id)
				{
					data_out.drain_data.data[i] = prev_node_data_in.drain_data.data[i];
				//	vec_data_out.drain_data[i] = vec_prev_node_data_in.drain_data[i];
				}
			}
#endif

		//	vec_out_data_to_ap_out_data(vec_data_out.drain_data, data_out.drain_data);
	
		//	data_out.drain_data[drain_node_id] = PE_data_in.data;
	
		//	if (drain_node_id != 0)
		//		ch_drain_chain_out << data_out;

			ch_drain_chain_out << data_out;
		}
		
//	}
}

void drain_chain_node(hls::stream<o_data_in_pe> &ch_data_o, hls::stream<o_data_from_pe> &ch_drain_chain_in, hls::stream<o_data_from_pe> &ch_drain_chain_out, int drain_node_id)
{
	struct o_data_in_pe PE_data_in;

//	struct custom_float_array vec_prev_node_data_in;
//	struct custom_float_array vec_data_out;

//#pragma HLS ARRAY_PARTITION variable=vec_prev_node_data_in.drain_data complete
//#pragma HLS ARRAY_PARTITION variable=vec_data_out.drain_data complete

	struct o_data_from_pe prev_node_data_in;
	struct o_data_from_pe data_out;

//	for (int r0 = 0; r0 < BN_R; r0++)
//	for (int o0 = 0; o0 < BN_O; o0++)
//	{
//		for (int r2 = 0; r2 < R; r2++)
//		for (int r1 = 0; r1 < BS_R; r1++)
//		for (int o1 = 0; o1 < BS_O; o1++)
		for (int it = 0; it < (BN_R * BN_O * R * BS_R * BS_O); it++)
		{
	#pragma HLS PIPELINE
			PE_data_in = ch_data_o.read();
	
			if (drain_node_id < SYS_ARRAY_NUM_COLS - 1)
			{
				prev_node_data_in = ch_drain_chain_in.read();
			}

#if (DEBUG_AP_INT_PACK == 1)
		//	ap_out_data_to_vec_out_data(prev_node_data_in.drain_data, vec_prev_node_data_in.drain_data);

		//	vec_prev_node_data_in.drain_data[drain_node_id] = PE_data_in.data;
			prev_node_data_in.drain_data(DATA_BITWIDTH * (drain_node_id + 1) - 1, DATA_BITWIDTH * drain_node_id) = PE_data_in.data;
#else
			prev_node_data_in.drain_data.data[drain_node_id] = PE_data_in.data;
#endif
	

#if (DEBUG_AP_INT_PACK == 1)
			data_out = prev_node_data_in;
#else
		//	for (int i = SYS_ARRAY_NUM_COLS - 1; i > drain_node_id; i--)
		//	for (int i = SYS_ARRAY_NUM_COLS - 1; i >= 0; i--)
			for (int i = 0; i < SYS_ARRAY_NUM_COLS; i++)
			{
#pragma HLS UNROLL
			//	if (i > drain_node_id)
				{
					data_out.drain_data.data[i] = prev_node_data_in.drain_data.data[i];
				}
			}
#endif

		//	vec_out_data_to_ap_out_data(vec_data_out.drain_data, data_out.drain_data);
	
		//	data_out.drain_data[drain_node_id] = PE_data_in.data;
	
		//	if (drain_node_id != 0)
		//		ch_drain_chain_out << data_out;

			ch_drain_chain_out << data_out;
		}
		
//	}
}

void drain_chain_node_wrapper(hls::stream<o_data_in_pe> &ch_data_o, hls::stream<o_data_from_pe> &ch_drain_chain_in, hls::stream<o_data_from_pe> &ch_drain_chain_out, int drain_node_id)
{
	drain_chain_node(ch_data_o, ch_drain_chain_in, ch_drain_chain_out, drain_node_id);
}

//void drain_chain_root(hls::stream<custom_float_array> &ch_drain_chain_0, hls::stream<vec_output_t> &ch_drain_to_relu)
void drain_chain_root(hls::stream<o_data_from_pe> &ch_drain_chain_0, hls::stream<o_data_from_pe> &ch_drain_to_relu)
{
//	for (int r0 = 0; r0 < BN_R; r0++)
//	for (int o0 = 0; o0 < BN_O; o0++)
//	{
//		for (int r2 = 0; r2 < R; r2++)
//		for (int r1 = 0; r1 < BS_R; r1++)
		for (int it = 0; it < (BN_R * BN_O * R * BS_R); it++)
		{
			for (int o1 = 0; o1 < BS_O; o1++)
			{
	//#pragma HLS UNROLL
	#pragma HLS PIPELINE
			//	struct custom_float_array root_data;
				struct o_data_from_pe root_data;
			//	struct vec_output_t v;
				
				root_data = ch_drain_chain_0.read();
		
//				for (int o2 = 0; o2 < O; o2++)
//				{
//	#pragma HLS UNROLL
//					v.data[o2] = root_data.drain_data[o2];
//				}

			//	ap_out_data_to_vec_out_data(root_data.drain_data, v.data);
			//	ch_drain_to_relu << v;
				ch_drain_to_relu << root_data;
			}
		}
//	}
}

