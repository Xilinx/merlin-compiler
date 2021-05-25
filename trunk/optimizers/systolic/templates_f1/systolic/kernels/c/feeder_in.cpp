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



#if (DEBUG_AP_INT_PACK == 1)
static void write_in_buf(int blk, int feeder_id, ap_in_type *in_buf, hls::stream<in_data_loader_to_feeder> &ch_data_row_feeder_in, hls::stream<in_data_loader_to_feeder> &ch_data_row_feeder_out, bool &new_row_col_pair_buffer)
#else
static void write_in_buf(char blk, int feeder_id, vec_input_t *in_buf, hls::stream<in_data_loader_to_feeder> &ch_data_row_feeder_in, hls::stream<in_data_loader_to_feeder> &ch_data_row_feeder_out, bool &new_row_col_pair_buffer)
#endif
{

	struct in_data_loader_to_feeder in_data;
//#pragma HLS DATA_PACK variable=in_data
//	vec_input_t vec_in;
	// write
//	for (int r2 = 0; r2 < (R - feeder_id); r2++)
	for (int r2 = 0; r2 < R; r2++)
	for (int r1 = 0; r1 < BS_R; r1++)
	for (int i1 = 0; i1 < BS_I; i1++)
//	int r2 = 0;
//	int r1 = 0;
//	int i1 = 0;
//	for (int i = 0; i < R * BS_R * BS_I; i++)
	{
#pragma HLS PIPELINE

		if (blk < TOTAL_NUM_BLOCKS)
		{
			in_data = ch_data_row_feeder_in.read();
		}

		new_row_col_pair_buffer = in_data.new_row_col_pair;

	//	if (r2 == 0)
		if (r2 == feeder_id)
		{
		//	ap_in_data_to_vec_in_data(in_data.pe_data, vec_in.data);
		//	in_buf[r1 * BS_I + i1] = vec_in;
			in_buf[r1 * BS_I + i1] = in_data.pe_data;
		}
	//	else
		{
			if (feeder_id < SYS_ARRAY_NUM_ROWS - 1)
			{
				if (blk < TOTAL_NUM_BLOCKS)
					ch_data_row_feeder_out << in_data;
			}
		}

	//	i1++;
	//	if (i1 == BS_I)
	//	{
	//		i1 = 0;
	//		r1++;
	//		if (r1 == BS_R)
	//		{
	//			r1 = 0;
	//			r2++;
	//			if (r2 == R)
	//			{
	//				r2 = 0;
	//			}
	//		}
	//	}
	}
}

#if (DEBUG_AP_INT_PACK == 1)
static void write_in_buf_last(int blk, ap_in_type *in_buf, hls::stream<in_data_loader_to_feeder> &ch_data_row_feeder_in, bool &new_row_col_pair_buffer)
#else
static void write_in_buf_last(int blk, vec_input_t *in_buf, hls::stream<in_data_loader_to_feeder> &ch_data_row_feeder_in, bool &new_row_col_pair_buffer)
#endif
{
//#pragma HLS inline

	struct in_data_loader_to_feeder in_data;
//#pragma HLS DATA_PACK variable=in_data
//	vec_input_t vec_in;
	// write
//	for (int r2 = 0; r2 < (R - feeder_id); r2++)
	for (int r2 = 0; r2 < R; r2++)
	for (int r1 = 0; r1 < BS_R; r1++)
	for (int i1 = 0; i1 < BS_I; i1++)
//	int r2 = 0;
//	int r1 = 0;
//	int i1 = 0;
//	for (int i = 0; i < R * BS_R * BS_I; i++)
	{
#pragma HLS PIPELINE

		if (blk < TOTAL_NUM_BLOCKS)
		{
			in_data = ch_data_row_feeder_in.read();
		}
		new_row_col_pair_buffer = in_data.new_row_col_pair;

	//	if (r2 == 0)
	//	if (r2 == feeder_id)
		if (r2 == (R - 1))
		{
		//	ap_in_data_to_vec_in_data(in_data.pe_data, vec_in.data);
		//	in_buf[r1 * BS_I + i1] = vec_in;
			in_buf[r1 * BS_I + i1] = in_data.pe_data;
		}

	//	i1++;
	//	if (i1 == BS_I)
	//	{
	//		i1 = 0;
	//		r1++;
	//		if (r1 == BS_R)
	//		{
	//			r1 = 0;
	//			r2++;
	//			if (r2 == R)
	//			{
	//				r2 = 0;
	//			}
	//		}
	//	}
	}
}

#if (DEBUG_AP_INT_PACK == 1)
static void read_in_buf(int blk, ap_in_type *in_buf, hls::stream<in_data_feeder_to_pe> &ch_data_feeder_to_pe, bool new_row_col_pair_feed)
#else
static void read_in_buf(int blk, vec_input_t *in_buf, hls::stream<in_data_feeder_to_pe> &ch_data_feeder_to_pe, bool new_row_col_pair_feed)
#endif
{
//	vec_input_t vec_in;
	struct in_data_feeder_to_pe in_data;
//#pragma HLS DATA_PACK variable=in_data

	// read
	int read_idx = 0;
//	for (char i1 = 0; i1 < BS_I; i1++)
//	for (char r1 = 0; r1 < BS_R; r1++)
//	for (char o1 = 0; o1 < BS_O; o1++) // interleaving
	int i1 = 0;
	int r1 = 0;
	int o1 = 0;
	for (int i = 0; i < (BS_I * BS_R * BS_O); i++)
	{
#pragma HLS PIPELINE

		in_data.pe_data = in_buf[r1 * BS_I + i1];
	//	in_data.pe_data = in_buf[r1 * BS_I + i1];
	//	vec_in = in_buf[r1 * BS_I + i1];
	//	vec_in_data_to_ap_in_data(vec_in.data, in_data.pe_data);

		in_data.new_row_col_pair = new_row_col_pair_feed;

		if (blk > 0)
		{
			ch_data_feeder_to_pe << in_data;
		}

		if (++read_idx == (BS_R * BS_O))
			new_row_col_pair_feed = false;

		o1++;
		if (o1 == BS_O)
		{
			o1 = 0;
			r1++;
			if (r1 == BS_R)
			{
				r1 = 0;
				i1++;
				if (i1 == BS_I)
				{
					i1 = 0;
				}
			}
		}
	}
}

static void feed_in(hls::stream<in_data_loader_to_feeder> &ch_data_row_feeder_in, hls::stream<in_data_feeder_to_pe> &ch_data_feeder_to_pe, hls::stream<in_data_loader_to_feeder> &ch_data_row_feeder_out, int feeder_id)
{
#if (DEBUG_AP_INT_PACK == 1)
	ap_in_type in_buf0[BS_R * BS_I];
	ap_in_type in_buf1[BS_R * BS_I];
#else
	vec_input_t in_buf0[BS_R * BS_I];
	vec_input_t in_buf1[BS_R * BS_I];
#endif

#pragma HLS RESOURCE variable=in_buf0 core=RAM_2P_BRAM
#pragma HLS RESOURCE variable=in_buf1 core=RAM_2P_BRAM

//#pragma HLS ARRAY_PARTITION variable=in_buf0[0].data complete
//#pragma HLS ARRAY_PARTITION variable=in_buf1[0].data complete

//#pragma HLS ARRAY_PARTITION variable=in_buf0 dim=2 complete
//#pragma HLS ARRAY_PARTITION variable=in_buf1 dim=2 complete

//#pragma HLS resource variable=in_buf core=RAM_2P

	bool new_row_col_pair_feed = false; // mark for block write in last iteration
	bool new_row_col_pair_buffer = false;

//	bool buffer_id_flag = 1;

//	write_in_buf(feeder_id, in_buf0, ch_data_row_feeder_in, ch_data_row_feeder_out, new_row_col_pair_buffer);
//	if (feeder_id < R - 1)
//		write_in_buf(feeder_id, in_buf0, ch_data_row_feeder_in, ch_data_row_feeder_out, new_row_col_pair_buffer);
//	else
//		write_in_buf_last(feeder_id, in_buf0, ch_data_row_feeder_in, new_row_col_pair_buffer);
//
//
//	new_row_col_pair_feed = new_row_col_pair_buffer;
//	new_row_col_pair_buffer = false;

	for (int blk = 0; blk < TOTAL_NUM_BLOCKS + 1; blk++)
	{
		if ((blk & 1) == 0)
		{
		//	if (feeder_id < R - 1)
				write_in_buf(blk, feeder_id, in_buf0, ch_data_row_feeder_in, ch_data_row_feeder_out, new_row_col_pair_buffer);
		//	else
		//		write_in_buf_last(blk, feeder_id, in_buf0, ch_data_row_feeder_in, new_row_col_pair_buffer);

			read_in_buf(blk, in_buf1, ch_data_feeder_to_pe, new_row_col_pair_feed);
		}
		else
		{
			read_in_buf(blk, in_buf0, ch_data_feeder_to_pe, new_row_col_pair_feed);

		//	if (feeder_id < R - 1)
				write_in_buf(blk, feeder_id, in_buf1, ch_data_row_feeder_in, ch_data_row_feeder_out, new_row_col_pair_buffer);
		//	else
		//		write_in_buf_last(blk, feeder_id, in_buf1, ch_data_row_feeder_in, new_row_col_pair_buffer);

		}

	//	buffer_id_flag = !buffer_id_flag;
		new_row_col_pair_feed = new_row_col_pair_buffer;
		new_row_col_pair_buffer = false;
	}

//	read_in_buf(in_buf1, ch_data_feeder_to_pe, new_row_col_pair_feed);
}

static void feed_in_last(hls::stream<in_data_loader_to_feeder> &ch_data_row_feeder_in, hls::stream<in_data_feeder_to_pe> &ch_data_feeder_to_pe)
{
#if (DEBUG_AP_INT_PACK == 1)
	ap_in_type in_buf0[BS_R * BS_I];
	ap_in_type in_buf1[BS_R * BS_I];
#else
	vec_input_t in_buf0[BS_R * BS_I];
	vec_input_t in_buf1[BS_R * BS_I];
#endif

#pragma HLS RESOURCE variable=in_buf0 core=RAM_2P_BRAM
#pragma HLS RESOURCE variable=in_buf1 core=RAM_2P_BRAM
//
//#pragma HLS ARRAY_PARTITION variable=in_buf0 dim=2 complete
//#pragma HLS ARRAY_PARTITION variable=in_buf1 dim=2 complete

	bool new_row_col_pair_feed = false; // mark for block write in last iteration
	bool new_row_col_pair_buffer = false;

//	bool buffer_id_flag = 1;

//	write_in_buf_last(feeder_id, in_buf0, ch_data_row_feeder_in, new_row_col_pair_buffer);
	new_row_col_pair_feed = new_row_col_pair_buffer;
	new_row_col_pair_buffer = false;

	for (char blk = 0; blk < TOTAL_NUM_BLOCKS + 1; blk++)
	{
	//	if (buffer_id_flag)
		if ((blk & 1) == 0)
		{
			write_in_buf_last(blk, in_buf0, ch_data_row_feeder_in, new_row_col_pair_buffer);
			read_in_buf(blk, in_buf1, ch_data_feeder_to_pe, new_row_col_pair_feed);
		}
		else
		{
			read_in_buf(blk, in_buf0, ch_data_feeder_to_pe, new_row_col_pair_feed);
			write_in_buf_last(blk, in_buf1, ch_data_row_feeder_in, new_row_col_pair_buffer);
		}

	//	buffer_id_flag = !buffer_id_flag;
		new_row_col_pair_feed = new_row_col_pair_buffer;
		new_row_col_pair_buffer = false;
	}

//	read_in_buf(in_buf1, ch_data_feeder_to_pe, new_row_col_pair_feed);
}
