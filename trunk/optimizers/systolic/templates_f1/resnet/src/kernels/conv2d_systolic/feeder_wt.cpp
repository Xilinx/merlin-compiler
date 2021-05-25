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




static void write_wt_buf(int blk,
		int feeder_id, 
		ap_in_t *wt_buf,
		hls::stream<wt_data_loader_to_feeder> &ch_data_col_feeder_in,
		hls::stream<wt_data_loader_to_feeder> &ch_data_col_feeder_out, 
		OpCFG &cfg)
{
	int total_num_blocks = (cfg.BN_R * cfg.BN_C * cfg.BN_O + 1) * cfg.BN_I;
	// write
//	for (int o2 = 0; o2 < (O - feeder_id); o2++)
	for (int o2 = 0; o2 < O; o2++)
	for (int o1 = 0; o1 < BS_O; o1++)
	for (int p = 0; p < cfg.k1; p++)
	for (int q = 0; q < cfg.k2; q++)
	for (int i1 = 0; i1 < BS_I; i1++)
//	int i1 = 0;
//	int o1 = 0;
//	int o2 = 0;
//	for (int i = 0; i < (O * BS_O * BS_I); i++)
	{
#pragma HLS PIPELINE
		struct wt_data_loader_to_feeder wt_data;

	//	if (blk < TOTAL_NUM_BLOCKS)
		if (blk < total_num_blocks)
			wt_data = ch_data_col_feeder_in.read();

	//	if (0 == o2)
		if (feeder_id == o2)
		{
			// write buffer
		//	w_buf[buffer_id_to_write_to][o1][i1] = wt_data.data;
//			for (int i2 = 0; i2 < I; i2++)
//			{
//#pragma HLS UNROLL
//				wt_buf[o1 * BS_I + i1][i2] = wt_data.pe_simd[i2];
//			}
			wt_buf[((o1 * cfg.k1 + p) * cfg.k2 + q) * BS_I + i1] = wt_data.pe_data;
		}
	//	else
		{
			// forward
			if (feeder_id < SYS_ARRAY_NUM_COLS - 1)
			{
			//	if (blk < TOTAL_NUM_BLOCKS)
				if (blk < total_num_blocks)
					ch_data_col_feeder_out << wt_data;
			}
		}

	//	i1++;
	//	if (i1 == BS_I)
	//	{
	//		i1 = 0;
	//		o1++;
	//		if (o1 == BS_O)
	//		{
	//			o1 = 0;
	//			o2++;
	//			if (o2 == O)
	//			{
	//				o2 = 0;
	//			}
	//		}
	//	}
	}
}

//static void write_wt_buf_last(int blk, vec_input_t *wt_buf, hls::stream<wt_data_loader_to_feeder> &ch_data_col_feeder_in)
static void write_wt_buf_last(int blk,
		ap_in_t *wt_buf,
		hls::stream<wt_data_loader_to_feeder> &ch_data_col_feeder_in, 
		OpCFG &cfg)
{
	int total_num_blocks = (cfg.BN_R * cfg.BN_C * cfg.BN_O + 1) * cfg.BN_I;
	// write
//	for (int o2 = 0; o2 < (O - feeder_id); o2++)
	for (int o1 = 0; o1 < BS_O; o1++)
	for (int p = 0; p < cfg.K1; p++)
	for (int q = 0; q < cfg.K2; q++)
	for (int i1 = 0; i1 < BS_I; i1++)
//	int i1 = 0;
//	int o1 = 0;
//	int o2 = 0;
//	for (int i = 0; i < (O * BS_O * BS_I); i++)
	{
#pragma HLS PIPELINE
		struct wt_data_loader_to_feeder wt_data;

	//	if (blk < TOTAL_NUM_BLOCKS)
		if (blk < total_num_blocks)
			wt_data = ch_data_col_feeder_in.read();

	//	if (0 == o2)
	//	if (feeder_id == o2)
			// write buffer
		//	w_buf[buffer_id_to_write_to][o1][i1] = wt_data.data;
//			for (int i2 = 0; i2 < I; i2++)
//			{
//#pragma HLS UNROLL
//				wt_buf[o1 * BS_I + i1][i2] = wt_data.pe_simd[i2];
//			}
			wt_buf[((o1 * cfg.k1 + p) * cfg.k2 + q) * BS_I + i1] = wt_data.pe_data;

	//	i1++;
	//	if (i1 == BS_I)
	//	{
	//		i1 = 0;
	//		o1++;
	//		if (o1 == BS_O)
	//		{
	//			o1 = 0;
	//			o2++;
	//			if (o2 == O)
	//			{
	//				o2 = 0;
	//			}
	//		}
	//	}
	}
}

//static void read_wt_buf(int blk, vec_input_t *wt_buf, hls::stream<wt_data_feeder_to_pe> &ch_data_to_pe)
static void read_wt_buf(int blk,
		ap_in_t *wt_buf,
		hls::stream<wt_data_feeder_to_pe> &ch_data_to_pe, 
		OpCFG &cfg)
{
	struct wt_data_feeder_to_pe wt_data;

	// read
	for (int i1 = 0; i1 < BS_I; i1++)
	for (int p = 0; p < K1; p++)
	for (int r1 = 0; r1 < BS_R; r1++) // interleaving
	for (int c1 = 0; c1 < BS_C; c1++) // interleaving
	for (int o1 = 0; o1 < BS_O; o1++)
//	int i1 = 0;
//	int r1 = 0;
//	int o1 = 0;
//	for (int it = 0; it < (BS_I * BS_R * BS_O); it++)
	{
#pragma HLS PIPELINE

#if (WINOGRAD == 1)
		for (int q = 0; q < K2; q++)
		{
#pragma HLS UNROLL
			vec_input_t v;

			ap_in_data_to_vec_in_data(wt_buf[((o1 * K1 + p) * K2 + q) * BS_I + i1], v.data);
			for (int i2 = 0; i2 < I; i2++)
			{
#pragma HLS UNROLL
				g[i2 * K2 + q] = v.data[i2];
			}
		}

		winograd_feed_wt_4_3(g, Gg);

		for (int c2_q = 0; c2_q < (C + K2 - 1); c2_q++)
		{
#pragma HLS UNROLL
			vec_input_t v;

			for (int i2 = 0; i2 < I; i2++)
			{
#pragma HLS UNROLL
				v.data[i2] = Gg[i2 * (C + K2 - 1) + c2_q];
			}

			vec_in_data_to_ap_filter_data(v.data, wt_data.pe_data, c2_q);
		}

#else
		for (int q = 0; q < K2; q++)
		{
#pragma HLS UNROLL
			vec_input_t v;

			ap_in_data_to_vec_in_data(wt_buf[((o1 * K1 + p) * K2 + q) * BS_I + i1], v.data);
			vec_in_data_to_ap_filter_data(v.data, wt_data.pe_data, q);
		}
#endif

		if (blk > 0)
		{
			ch_data_to_pe << wt_data;
		}

	//	o1++;
	//	if (o1 == BS_O)
	//	{
	//		o1 = 0;
	//		r1++;
	//		if (r1 == BS_R)
	//		{
	//			r1 = 0;
	//			i1++;
	//			if (i1 == BS_I)
	//			{
	//				i1 = 0;
	//			}
	//		}
	//	}
	}
}

static void feed_wt(hls::stream<wt_data_loader_to_feeder> &ch_data_col_feeder_in,
		hls::stream<wt_data_feeder_to_pe> &ch_data_feeder_to_pe,
		hls::stream<wt_data_loader_to_feeder> &ch_data_col_feeder_out,
		int feeder_id, 
		OpCFG &cfg)
{
//	vec_input_t wt_buf0[BS_O * BS_I];
//	vec_input_t wt_buf1[BS_O * BS_I];
	ap_in_t wt_buf0[BS_O * MK1 * MK2 * BS_I];
	ap_in_t wt_buf1[BS_O * MK1 * MK2 * BS_I];

#pragma HLS RESOURCE variable=wt_buf0 core=RAM_2P_BRAM
#pragma HLS RESOURCE variable=wt_buf1 core=RAM_2P_BRAM

//#pragma HLS ARRAY_PARTITION variable=wt_buf0 dim=2 complete
//#pragma HLS ARRAY_PARTITION variable=wt_buf1 dim=2 complete

//	bool buffer_id_flag = 1;

//	write_wt_buf(feeder_id, wt_buf0, ch_data_col_feeder_in, ch_data_col_feeder_out);

	int total_num_blocks = (cfg.BN_R * cfg.BN_C * cfg.BN_O + 1) * cfg.BN_I;
//	for (int blk = 0; blk < TOTAL_NUM_BLOCKS + 1; blk++)
	for (int blk = 0; blk < total_num_blocks + 1; blk++)
	{
		if ((blk & 1) == 0)
		{
			write_wt_buf(blk, feeder_id, wt_buf0, ch_data_col_feeder_in, ch_data_col_feeder_out, cfg);
			read_wt_buf(blk, wt_buf1, ch_data_feeder_to_pe, cfg);
		}
		else
		{
			read_wt_buf(blk, wt_buf0, ch_data_feeder_to_pe, cfg);
			write_wt_buf(blk, feeder_id, wt_buf1, ch_data_col_feeder_in, ch_data_col_feeder_out, cfg);
		}
	}

//	read_wt_buf(wt_buf1, ch_data_feeder_to_pe);
}

static void feed_wt_last(hls::stream<wt_data_loader_to_feeder> &ch_data_col_feeder_in,
		hls::stream<wt_data_feeder_to_pe> &ch_data_feeder_to_pe, 
		OpCFG &cfg)
{
//	vec_input_t wt_buf0[BS_O * BS_I];
//	vec_input_t wt_buf1[BS_O * BS_I];
	ap_in_t wt_buf0[BS_O * MK1 * MK2 * BS_I];
	ap_in_t wt_buf1[BS_O * MK1 * MK2 * BS_I];

#pragma HLS RESOURCE variable=wt_buf0 core=RAM_2P_BRAM
#pragma HLS RESOURCE variable=wt_buf1 core=RAM_2P_BRAM

//#pragma HLS ARRAY_PARTITION variable=wt_buf0 dim=2 complete
//#pragma HLS ARRAY_PARTITION variable=wt_buf1 dim=2 complete

//	bool buffer_id_flag = 1;

//	write_wt_buf_last(feeder_id, wt_buf0, ch_data_col_feeder_in);

	int total_num_blocks = (cfg.BN_R * cfg.BN_C * cfg.BN_O + 1) * cfg.BN_I;

	for (int blk = 0; blk < total_num_blocks + 1; blk++)
	{
		if ((blk & 1) == 0)
		{
			write_wt_buf_last(blk, wt_buf0, ch_data_col_feeder_in, cfg);
			read_wt_buf(blk, wt_buf1, ch_data_feeder_to_pe, cfg);
		}
		else
		{
			read_wt_buf(blk, wt_buf0, ch_data_feeder_to_pe, cfg);
			write_wt_buf_last(blk, wt_buf1, ch_data_col_feeder_in, cfg);
		}
	}
}

