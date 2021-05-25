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



//static void write_in_buf(int blk, vec_input_t *in_buf, hls::stream<in_data_loader_to_feeder> &ch_data_row_feeder, bool &new_row_col_pair_buffer)
static void write_in_buf(int blk,
		int feeder_id,
		ap_in_t *in_buf, 
		hls::stream<in_data_loader_to_feeder> &ch_data_row_feeder_in,
		hls::stream<in_data_loader_to_feeder> &ch_data_row_feeder_out,
		bool &new_row_col_pair_buffer)
{

	// write
//	for (int r2 = 0; r2 < (R - feeder_id); r2++)
//	for (int r2 = 0; r2 < R; r2++)
//	for (int r1 = 0; r1 < BS_R; r1++)
//	for (int i1 = 0; i1 < BS_I; i1++)
//	int r2 = 0;
//	int r1 = 0;
//	int i1 = 0;
//	for (int i = 0; i < ((BS_R * R + K1 - 1) * (BS_C * C + K2 - 1) * BS_I); i++)
	for (int r2 = 0; r2 < R; r2++)
	for (int r1_p = 0; r1_p < (BS_R + K1 - 1); r1_p++)
	for (int c2_c1_q = 0; c2_c1_q < (C * BS_C + K2 - 1); c2_c1_q++)
	for (int i1 = 0; i1 < BS_I; i1++)
	{
#pragma HLS PIPELINE
		struct in_data_loader_to_feeder in_data;

		if (blk < TOTAL_NUM_BLOCKS)
			in_data = ch_data_row_feeder_in.read();

		new_row_col_pair_buffer = in_data.new_row_col_pair;

	//	in_buf[(r1 * R + r2) * BS_I + i1] = in_data.pe_data;
		if (r2 == feeder_id)
		{
			in_buf[r1_p * (C * BS_C + K2 + 1) * BS_I + c2_c1_q * BS_I + i1] = in_data.pe_data;
		}
		if (feeder_id < SYS_ARRAY_NUM_ROWS - 1)
		{
			if (blk < TOTAL_NUM_BLOCKS)
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
		bool &new_row_col_pair_buffer)
{

	// write
//	for (int r2 = 0; r2 < (R - feeder_id); r2++)
//	for (int r2 = 0; r2 < R; r2++)
//	for (int r1 = 0; r1 < BS_R; r1++)
//	for (int i1 = 0; i1 < BS_I; i1++)
//	int r2 = 0;
//	int r1 = 0;
//	int i1 = 0;
//	for (int i = 0; i < ((BS_R * R + K1 - 1) * (BS_C * C + K2 - 1) * BS_I); i++)
	for (int r2 = 0; r2 < R; r2++)
	for (int r1_p = 0; r1_p < (BS_R + K1 - 1); r1_p++)
	for (int c2_c1_q = 0; c2_c1_q < (C * BS_C + K2 - 1); c2_c1_q++)
	for (int i1 = 0; i1 < BS_I; i1++)
	{
#pragma HLS PIPELINE
		struct in_data_loader_to_feeder in_data;

		if (blk < TOTAL_NUM_BLOCKS)
			in_data = ch_data_row_feeder_in.read();

		new_row_col_pair_buffer = in_data.new_row_col_pair;

	//	in_buf[(r1 * R + r2) * BS_I + i1] = in_data.pe_data;
		if (r2 == (R - 1))
		{
			in_buf[r1_p * (C * BS_C + K2 - 1) * BS_I + c2_c1_q * BS_I + i1] = in_data.pe_data;
		}
	}
}

#define AA(a, xx) a[i2 * (C + K2 - 1) + xx]

static void winograd_feed_in_4_3(data_bitwidth *d, data_bitwidth *BTd)
{
#pragma HLS inline
	for (int i2 = 0; i2 < I; i2++)
	{
#pragma HLS UNROLL
		BTd[i2 * (C + K2 - 1) + 0] = 4 * d[i2 * (C + K2 - 1) + 0] - 5 * d[i2 * (C + K2 - 1) + 2] + d[i2 * (C + K2 - 1) + 4];
	//	AA(BTd, 0) = 4 * AA(d, 0) - 5 * AA(d, 2) + AA(d, 4);
		BTd[i2 * (C + K2 - 1) + 1] = -4 * d[i2 * (C + K2 - 1) + 1] - 4 * d[i2 * (C + K2 - 1) + 2] + d[i2 * (C + K2 - 1) + 3] + d[i2 * (C + K2 - 1) + 4];
	//	AA(BTd, 1) = -4 * AA(d, 1) - 4 * AA(d, 2) + AA(d, 3) + AA(d, 4);
		BTd[i2 * (C + K2 - 1) + 2] = 4 * d[i2 * (C + K2 - 1) + 1] - 4 * d[i2 * (C + K2 - 1) + 2] - d[i2 * (C + K2 - 1) + 3] + d[i2 * (C + K2 - 1) + 4];
	//	AA(BTd, 2) = 4 * AA(d, 1) - 4 * AA(d, 2) - AA(d, 3) + AA(d, 4);
		BTd[i2 * (C + K2 - 1) + 3] = -2 * d[i2 * (C + K2 - 1) + 1] - 1 * d[i2 * (C + K2 - 1) + 2] + 2 * d[i2 * (C + K2 - 1) + 3] + d[i2 * (C + K2 - 1) + 4];
	//	AA(BTd, 3) = -2 * AA(d, 1) - 1 * AA(d, 2) + 2 * AA(d, 3) + AA(d, 4);
		BTd[i2 * (C + K2 - 1) + 4] = 2 * d[i2 * (C + K2 - 1) + 1] - 1 * d[i2 * (C + K2 - 1) + 2] - 2 * d[i2 * (C + K2 - 1) + 3] + d[i2 * (C + K2 - 1) + 4];
	//	AA(BTd, 4) = 2 * AA(d, 1) - 1 * AA(d, 2) - 2 * AA(d, 3) + AA(d, 4);
		BTd[i2 * (C + K2 - 1) + 5] = 4 * d[i2 * (C + K2 - 1) + 2] - 5 * d[i2 * (C + K2 - 1) + 3] + d[i2 * (C + K2 - 1) + 5];
	//	AA(BTd, 5) = 4 * AA(d, 2) - 5 * AA(d, 3) + AA(d, 5);
	
	//	BTd[i2 * (C + K2 - 1) + 0] = 1;
	//	BTd[i2 * (C + K2 - 1) + 1] = 1;
	//	BTd[i2 * (C + K2 - 1) + 2] = 1;
	//	BTd[i2 * (C + K2 - 1) + 3] = 1;
	//	BTd[i2 * (C + K2 - 1) + 4] = 1;
	//	BTd[i2 * (C + K2 - 1) + 5] = 1;
	}
}

//static void read_in_buf(int blk, vec_input_t *in_buf, hls::stream<in_data_feeder_to_pe> &ch_data_feeder_to_pe, bool new_row_col_pair_feed)
static void read_in_buf(int blk, 
		ap_in_t *in_buf,
		hls::stream<in_data_feeder_to_pe> &ch_data_feeder_to_pe,
		bool new_row_col_pair_feed)
{
	struct in_data_feeder_to_pe in_data;

#if (WINOGRAD == 1)
	data_bitwidth d[I * (C + K2 - 1)];
	data_bitwidth BTd[I * (C + K2 - 1)];
#endif

	// read
	int read_idx = 0;
	for (int i1 = 0; i1 < BS_I; i1++)
	for (int p = 0; p < K1; p++)
	for (int r1 = 0; r1 < BS_R; r1++)
	for (int c1 = 0; c1 < BS_C; c1++)
	for (int o1 = 0; o1 < BS_O; o1++) // interleaving
//	int i1 = 0;
//	int r1 = 0;
//	int o1 = 0;
//	for (int it = 0; it < (BS_I * BS_R * BS_O); it++)
	{
#pragma HLS PIPELINE

#if (WINOGRAD == 1)
		for (int c2_q = 0; c2_q < (C + K2 - 1); c2_q++)
		{
#pragma HLS UNROLL
			vec_input_t v;
			int in_buf_idx = (r1 + p) * (BS_C * C + K2 - 1) * BS_I + (c1 * C + c2_q) * BS_I + i1;

			ap_in_data_to_vec_in_data(in_buf[in_buf_idx], v.data);
			for (int i2 = 0; i2 < I; i2++)
			{
#pragma HLS UNROLL
				d[i2 * (C + K2 - 1) + c2_q] = v.data[i2];
			//	d[i2 * (C + K2 - 1) + c2_q] = 1;
			}
		}

	//	d[0] = -103;
	//	d[1] = -105;
	//	d[2] = -81;
	//	d[3] = -74;
	//	d[4] = -41;
	//	d[5] = 70;
	//	d[6] = 58;
	//	d[7] = -115;
	//	d[8] = 1;
	//	d[9] = 20;
	//	d[10] = 51;
	//	d[11] = 85;

		// Winograd transformation
		winograd_feed_in_4_3(d, BTd);

	//	BTd[0] = -48;
	//	BTd[1] = 117;
	//	BTd[2] = -63;
	//	BTd[3] = 102;
	//	BTd[4] = -22;
	//	BTd[5] = 116;
	//	BTd[6] = 22;
	//	BTd[7] = 15;
	//	BTd[8] = 79;
	//	BTd[9] = 64;
	//	BTd[10] = 36;
	//	BTd[11] = -11;


		for (int c2_q = 0; c2_q < (C + K2 - 1); c2_q++)
		{
#pragma HLS UNROLL
			vec_input_t v;

			for (int i2 = 0; i2 < I; i2++)
			{
#pragma HLS UNROLL
				v.data[i2] = BTd[i2 * (C + K2 - 1) + c2_q];
			}

			vec_in_data_to_ap_ifmap_data(v.data, in_data.pe_data, c2_q);
		}

#else
		for (int c2_q = 0; c2_q < (C + K2 - 1); c2_q++)
		{
#pragma HLS UNROLL
			vec_input_t v;

			ap_in_data_to_vec_in_data(in_buf[(r1 + p) * (BS_C * C + K2 - 1) * BS_I + (c1 * C + c2_q) * BS_I + i1], v.data);

			vec_in_data_to_ap_ifmap_data(v.data, in_data.pe_data, c2_q);
		}
#endif // WINOGRAD

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
		int feeder_id)
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

	for (int blk = 0; blk < TOTAL_NUM_BLOCKS + 1; blk++)
	{
		if ((blk & 1) == 0)
		{
			write_in_buf(blk, feeder_id, in_buf0, ch_data_row_feeder_in, ch_data_row_feeder_out, new_row_col_pair_buffer);
			read_in_buf(blk, in_buf1, ch_data_feeder_to_pe, new_row_col_pair_feed);
		}
		else
		{
			read_in_buf(blk, in_buf0, ch_data_feeder_to_pe, new_row_col_pair_feed);
			write_in_buf(blk, feeder_id, in_buf1, ch_data_row_feeder_in, ch_data_row_feeder_out, new_row_col_pair_buffer);
		}

		new_row_col_pair_feed = new_row_col_pair_buffer;
		new_row_col_pair_buffer = false;
	}
}

static void feed_in_last(hls::stream<in_data_loader_to_feeder> &ch_data_row_feeder, 
		hls::stream<in_data_feeder_to_pe> &ch_data_feeder_to_pe)
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

	for (int blk = 0; blk < TOTAL_NUM_BLOCKS + 1; blk++)
	{
		if ((blk & 1) == 0)
		{
			write_in_buf_last(blk, in_buf0, ch_data_row_feeder, new_row_col_pair_buffer);
			read_in_buf(blk, in_buf1, ch_data_feeder_to_pe, new_row_col_pair_feed);
		}
		else
		{
			read_in_buf(blk, in_buf0, ch_data_feeder_to_pe, new_row_col_pair_feed);
			write_in_buf_last(blk, in_buf1, ch_data_row_feeder, new_row_col_pair_buffer);
		}

		new_row_col_pair_feed = new_row_col_pair_buffer;
		new_row_col_pair_buffer = false;
	}
}
