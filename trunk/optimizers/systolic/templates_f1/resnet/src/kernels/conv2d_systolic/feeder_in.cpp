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



static void write_in_buf(int blk,
		int feeder_id,
		ap_in_t *in_buf, 
		hls::stream<in_data_loader_to_feeder> &ch_data_row_feeder_in,
		hls::stream<in_data_loader_to_feeder> &ch_data_row_feeder_out,
		bool &new_row_col_pair_buffer, 
		OpCFG &cfg)
{

	int total_num_blocks = (cfg.BN_R * cfg.BN_O + 1) * cfg.BN_I;

	int in_buf_idx = 0;

	// write
//	for (int r2 = 0; r2 < (R - feeder_id); r2++)
//	for (int r2 = 0; r2 < R; r2++)
//	for (int r1 = 0; r1 < BS_R; r1++)
//	for (int i1 = 0; i1 < BS_I; i1++)
//	int r2 = 0;
//	int r1 = 0;
//	int i1 = 0;
//	for (int i = 0; i < ((BS_R * R + K1 - 1) * (BS_C * C + K2 - 1) * BS_I); i++)
	for (int r2_r1_p = 0; r2_r1_p < (R * BS_R + cfg.k1 - 1); r2_r1_p++)
	for (int c2_c1_q = 0; c2_c1_q < (C * BS_C + cfg.k2 - 1); c2_c1_q++)
	for (int i1 = 0; i1 < BS_I; i1++)
	{
#pragma HLS PIPELINE
		struct in_data_loader_to_feeder in_data;

		if (blk < total_num_blocks)
			in_data = ch_data_row_feeder_in.read();

		new_row_col_pair_buffer = in_data.new_row_col_pair;

	//	if (r2 == feeder_id)
		if (r2_r1_p >= feeder_id * BS_R && r2_r1_p < (feeder_id + 1) * BS_R + cfg.k1 - 1)
		{
		//	in_buf[r1_p * (C * BS_C + K2 + 1) * BS_I + c2_c1_q * BS_I + i1] = in_data.pe_data;
			in_buf[in_buf_idx++] = in_data.pe_data;
		}
		if (feeder_id < SYS_ARRAY_NUM_ROWS - 1)
		{
			if (blk < total_num_blocks)
			{
				ch_data_row_feeder_out << in_data;
			}
		}

		//i1++;
		//if (i1 == BS_I)
		//{
		//	i1 = 0;
		//	r2++;
		//	if (r2 == R)
		//	{
		//		r2 = 0;
		//		r1++;
		//		if (r1 == BS_R)
		//		{
		//			r1 = 0;
		//		}
		//	}
		//}
	}
}

static void write_in_buf_last(int blk,
		ap_in_t *in_buf, 
		hls::stream<in_data_loader_to_feeder> &ch_data_row_feeder_in,
		bool &new_row_col_pair_buffer, 
		OpCFG &cfg)
{
	int total_num_blocks = (cfg.BN_R * cfg.BN_C * cfg.BN_O + 1) * cfg.BN_I;

	int in_buf_idx = 0;

	// write
//	for (int r2 = 0; r2 < (R - feeder_id); r2++)
//	for (int r2 = 0; r2 < R; r2++)
//	for (int r1 = 0; r1 < BS_R; r1++)
//	for (int i1 = 0; i1 < BS_I; i1++)
//	int r2 = 0;
//	int r1 = 0;
//	int i1 = 0;
//	for (int i = 0; i < ((BS_R * R + K1 - 1) * (BS_C * C + K2 - 1) * BS_I); i++)
	for (int r1_p = 0; r1_p < (BS_R + cfg.k1 - 1); r1_p++)
	for (int c2_c1_q = 0; c2_c1_q < (C * BS_C + cfg.k2 - 1); c2_c1_q++)
	for (int i1 = 0; i1 < BS_I; i1++)
	{
#pragma HLS PIPELINE
		struct in_data_loader_to_feeder in_data;

	//	if (blk < TOTAL_NUM_BLOCKS)
		if (blk < total_num_blocks)
			in_data = ch_data_row_feeder_in.read();

		new_row_col_pair_buffer = in_data.new_row_col_pair;

	//	in_buf[(r1 * R + r2) * BS_I + i1] = in_data.pe_data;
		in_buf[in_buf_idx++] = in_data.pe_data;
	}
}

//static void read_in_buf(int blk, vec_input_t *in_buf, hls::stream<in_data_feeder_to_pe> &ch_data_feeder_to_pe, bool new_row_col_pair_feed)
static void read_in_buf(int blk, 
		ap_in_t *in_buf,
		hls::stream<in_data_feeder_to_pe> &ch_data_feeder_to_pe,
		bool new_row_col_pair_feed, 
		OpCFG &cfg)
{
	struct in_data_feeder_to_pe in_data;

	// read
	int read_idx = 0;
	for (int i1 = 0; i1 < BS_I; i1++)
	for (int p = 0; p < cfg.k1; p++)
	for (int r1 = 0; r1 < BS_R; r1++)
	for (int c1 = 0; c1 < BS_C; c1++)
	for (int o1 = 0; o1 < BS_O; o1++) // interleaving
//	int i1 = 0;
//	int r1 = 0;
//	int o1 = 0;
//	for (int it = 0; it < (BS_I * BS_R * BS_O); it++)
	{
#pragma HLS PIPELINE

		for (int c2_q = 0; c2_q < (C + MK2 - 1); c2_q++)
		{
#pragma HLS UNROLL
			vec_input_t v;

			ap_in_data_to_vec_in_data(in_buf[(r1 + p) * (BS_C * C + cfg.k2 - 1) * BS_I + (c1 * C + c2_q) * BS_I + i1], v.data);

			vec_in_data_to_ap_ifmap_data(v.data, in_data.pe_data, c2_q);
		}

		in_data.new_row_col_pair = new_row_col_pair_feed;

		if (blk > 0)
			ch_data_feeder_to_pe << in_data;

		if (++read_idx == (BS_R * BS_C * BS_O))
			new_row_col_pair_feed = false;

	}
}

static void feed_in(hls::stream<in_data_loader_to_feeder> &ch_data_row_feeder_in,
		hls::stream<in_data_feeder_to_pe> &ch_data_feeder_to_pe,
		hls::stream<in_data_loader_to_feeder> &ch_data_row_feeder_out,
		int feeder_id, 
		OpCFG &cfg)
{
//	vec_input_t in_buf0[BS_R * R * BS_I];
//	vec_input_t in_buf1[BS_R * R * BS_I];
	ap_in_t in_buf0[(BS_R + K1 - 1) * (BS_C * C + K2 - 1) * BS_I];
	ap_in_t in_buf1[(BS_R + K1 - 1) * (BS_C * C + K2 - 1) * BS_I];

//#pragma HLS RESOURCE variable=in_buf0 core=RAM_2P
//#pragma HLS RESOURCE variable=in_buf1 core=RAM_2P


#pragma HLS ARRAY_PARTITION variable=in_buf0 cyclic factor=4
#pragma HLS ARRAY_PARTITION variable=in_buf1 cyclic factor=4

//#pragma HLS resource variable=in_buf core=RAM_2P

	bool new_row_col_pair_feed = false; // mark for block write in last iteration
	bool new_row_col_pair_buffer = false;

	int total_num_blocks = (cfg.BN_R * cfg.BN_C * cfg.BN_O + 1) * cfg.BN_I;

	for (int blk = 0; blk < total_num_blocks + 1; blk++)
	{
		if ((blk & 1) == 0)
		{
			write_in_buf(blk, feeder_id, in_buf0, ch_data_row_feeder_in, ch_data_row_feeder_out, new_row_col_pair_buffer, cfg);
			read_in_buf(blk, in_buf1, ch_data_feeder_to_pe, new_row_col_pair_feed, cfg);
		}
		else
		{
			read_in_buf(blk, in_buf0, ch_data_feeder_to_pe, new_row_col_pair_feed, cfg);
			write_in_buf(blk, feeder_id, in_buf1, ch_data_row_feeder_in, ch_data_row_feeder_out, new_row_col_pair_buffer, cfg);
		}

		new_row_col_pair_feed = new_row_col_pair_buffer;
		new_row_col_pair_buffer = false;
	}
}

static void feed_in_last(hls::stream<in_data_loader_to_feeder> &ch_data_row_feeder, 
		hls::stream<in_data_feeder_to_pe> &ch_data_feeder_to_pe, 
		OpCFG &cfg)
{
//	vec_input_t in_buf0[BS_R * R * BS_I];
//	vec_input_t in_buf1[BS_R * R * BS_I];
	ap_in_t in_buf0[(BS_R + MK1 - 1) * (BS_C * C + MK2 - 1) * BS_I];
	ap_in_t in_buf1[(BS_R + MK1 - 1) * (BS_C * C + MK2 - 1) * BS_I];

//#pragma HLS RESOURCE variable=in_buf0 core=RAM_2P
//#pragma HLS RESOURCE variable=in_buf1 core=RAM_2P

#pragma HLS ARRAY_PARTITION variable=in_buf0 cyclic factor=4
#pragma HLS ARRAY_PARTITION variable=in_buf1 cyclic factor=4

//#pragma HLS resource variable=in_buf core=RAM_2P

	bool new_row_col_pair_feed = false; // mark for block write in last iteration
	bool new_row_col_pair_buffer = false;

	int total_num_blocks = (cfg.BN_R * cfg.BN_C * cfg.BN_O + 1) * cfg.BN_I;

	for (int blk = 0; blk < total_num_blocks + 1; blk++)
	{
		if ((blk & 1) == 0)
		{
			write_in_buf_last(blk, in_buf0, ch_data_row_feeder, new_row_col_pair_buffer, cfg);
			read_in_buf(blk, in_buf1, ch_data_feeder_to_pe, new_row_col_pair_feed, cfg);
		}
		else
		{
			read_in_buf(blk, in_buf0, ch_data_feeder_to_pe, new_row_col_pair_feed, cfg);
			write_in_buf_last(blk, in_buf1, ch_data_row_feeder, new_row_col_pair_buffer, cfg);
		}

		new_row_col_pair_feed = new_row_col_pair_buffer;
		new_row_col_pair_buffer = false;
	}
}
