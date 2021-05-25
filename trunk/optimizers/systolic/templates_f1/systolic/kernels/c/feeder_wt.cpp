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
static void write_wt_buf(int blk, int feeder_id, ap_in_type *wt_buf, hls::stream<wt_data_loader_to_feeder> &ch_data_col_feeder_in, hls::stream<wt_data_loader_to_feeder> &ch_data_col_feeder_out)
#else
static void write_wt_buf(int blk, int feeder_id, vec_input_t *wt_buf, hls::stream<wt_data_loader_to_feeder> &ch_data_col_feeder_in, hls::stream<wt_data_loader_to_feeder> &ch_data_col_feeder_out)
#endif
{
	struct wt_data_loader_to_feeder wt_data;
//	vec_input_t vec_wt;
	// write
//	for (int o2 = 0; o2 < (O - feeder_id); o2++)
	for (int o2 = 0; o2 < O; o2++)
	for (int o1 = 0; o1 < BS_O; o1++)
	for (int i1 = 0; i1 < BS_I; i1++)
//	int i1 = 0;
//	int o1 = 0;
//	int o2 = 0;
//	for (int i = 0; i < (O * BS_O * BS_I); i++)
	{
#pragma HLS PIPELINE

		if (blk < TOTAL_NUM_BLOCKS)
			wt_data = ch_data_col_feeder_in.read();

	//	if (0 == o2)
		if (feeder_id == o2)
		{
			// write buffer
			wt_buf[o1 * BS_I + i1] = wt_data.pe_data;
		//	ap_in_data_to_vec_in_data(wt_data.pe_data, vec_wt.data);
		//	wt_buf[o1 * BS_I + i1] = vec_wt;
		}
	//	else
		{
			// forward
			if (feeder_id < SYS_ARRAY_NUM_COLS - 1)
			{
				if (blk < TOTAL_NUM_BLOCKS)
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

#if (DEBUG_AP_INT_PACK == 1)
static void write_wt_buf_last(int blk, ap_in_type *wt_buf, hls::stream<wt_data_loader_to_feeder> &ch_data_col_feeder_in)
#else
static void write_wt_buf_last(int blk, vec_input_t *wt_buf, hls::stream<wt_data_loader_to_feeder> &ch_data_col_feeder_in)
#endif
{
	struct wt_data_loader_to_feeder wt_data;
//	vec_input_t vec_wt;
	// write
//	for (int o2 = 0; o2 < (O - feeder_id); o2++)
	for (int o2 = 0; o2 < O; o2++)
	for (int o1 = 0; o1 < BS_O; o1++)
	for (int i1 = 0; i1 < BS_I; i1++)
//	int i1 = 0;
//	int o1 = 0;
//	int o2 = 0;
//	for (int i = 0; i < (O * BS_O * BS_I); i++)
	{
#pragma HLS PIPELINE

		if (blk < TOTAL_NUM_BLOCKS)
			wt_data = ch_data_col_feeder_in.read();

	//	if (0 == o2)
	//	if (feeder_id == o2)
		if ((O - 1) == o2)
		{
			// write buffer
			wt_buf[o1 * BS_I + i1] = wt_data.pe_data;
		//	ap_in_data_to_vec_in_data(wt_data.pe_data, vec_wt.data);
		//	wt_buf[o1 * BS_I + i1] = vec_wt;
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

#if (DEBUG_AP_INT_PACK == 1)
static void read_wt_buf(int blk, ap_in_type *wt_buf, hls::stream<wt_data_feeder_to_pe> &ch_data_to_pe)
#else
static void read_wt_buf(int blk, vec_input_t *wt_buf, hls::stream<wt_data_feeder_to_pe> &ch_data_to_pe)
#endif
{
//	vec_input_t vec_wt;
	struct wt_data_feeder_to_pe wt_data;
	// read
	for (int i1 = 0; i1 < BS_I; i1++)
	for (int r1 = 0; r1 < BS_R; r1++) // interleaving
	for (int o1 = 0; o1 < BS_O; o1++)
//	int i1 = 0;
//	int r1 = 0;
//	int o1 = 0;
//	for (int it = 0; it < (BS_I * BS_R * BS_O); it++)
	{
#pragma HLS PIPELINE

		wt_data.pe_data = wt_buf[o1 * BS_I + i1];
	//	vec_wt = wt_buf[o1 * BS_I + i1];
	//	vec_in_data_to_ap_in_data(vec_wt.data, wt_data.pe_data);

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

static void feed_wt(hls::stream<wt_data_loader_to_feeder> &ch_data_col_feeder_in, hls::stream<wt_data_feeder_to_pe> &ch_data_feeder_to_pe, hls::stream<wt_data_loader_to_feeder> &ch_data_col_feeder_out, int feeder_id)	
{
#if (DEBUG_AP_INT_PACK == 1)
	ap_in_type wt_buf0[BS_O * BS_I];
	ap_in_type wt_buf1[BS_O * BS_I];
#else
	vec_input_t wt_buf0[BS_O * BS_I];
	vec_input_t wt_buf1[BS_O * BS_I];
#endif

#pragma HLS RESOURCE variable=wt_buf0 core=RAM_2P_BRAM
#pragma HLS RESOURCE variable=wt_buf1 core=RAM_2P_BRAM

//#pragma HLS ARRAY_PARTITION variable=wt_buf0 dim=2 complete
//#pragma HLS ARRAY_PARTITION variable=wt_buf1 dim=2 complete

//	bool buffer_id_flag = 1;

//	write_wt_buf(feeder_id, wt_buf0, ch_data_col_feeder_in, ch_data_col_feeder_out);

//	for (int blk = 1; blk < TOTAL_NUM_BLOCKS; blk++)
	for (int blk = 0; blk < TOTAL_NUM_BLOCKS + 1; blk++)
	{
	//	if (buffer_id_flag)
		if ((blk & 1) == 0)
		{
		//	if (feeder_id < O - 1)
				write_wt_buf(blk, feeder_id, wt_buf0, ch_data_col_feeder_in, ch_data_col_feeder_out);
		//	else
		//		write_wt_buf_last(blk, feeder_id, wt_buf0, ch_data_col_feeder_in);

			read_wt_buf(blk, wt_buf1, ch_data_feeder_to_pe);
		}
		else
		{
			read_wt_buf(blk, wt_buf0, ch_data_feeder_to_pe);
		//	if (feeder_id < O - 1)
				write_wt_buf(blk, feeder_id, wt_buf1, ch_data_col_feeder_in, ch_data_col_feeder_out);
		//	else
		//		write_wt_buf_last(blk, feeder_id, wt_buf1, ch_data_col_feeder_in);
		}

	//	buffer_id_flag = !buffer_id_flag;
	}

//	read_wt_buf(wt_buf1, ch_data_feeder_to_pe);
}

static void feed_wt_last(hls::stream<wt_data_loader_to_feeder> &ch_data_col_feeder_in, hls::stream<wt_data_feeder_to_pe> &ch_data_feeder_to_pe)
{
#if (DEBUG_AP_INT_PACK == 1)
	ap_in_type wt_buf0[BS_O * BS_I];
	ap_in_type wt_buf1[BS_O * BS_I];
#else
	vec_input_t wt_buf0[BS_O * BS_I];
	vec_input_t wt_buf1[BS_O * BS_I];
#endif

#pragma HLS RESOURCE variable=wt_buf0 core=RAM_2P_BRAM
#pragma HLS RESOURCE variable=wt_buf1 core=RAM_2P_BRAM

//#pragma HLS ARRAY_PARTITION variable=wt_buf0 dim=2 complete
//#pragma HLS ARRAY_PARTITION variable=wt_buf1 dim=2 complete

//	bool buffer_id_flag = 1;

//	write_wt_buf_last(feeder_id, wt_buf0, ch_data_col_feeder_in);

//	for (int blk = 1; blk < TOTAL_NUM_BLOCKS; blk++)
	for (int blk = 0; blk < TOTAL_NUM_BLOCKS + 1; blk++)
	{
	//	if (buffer_id_flag)
		if ((blk & 1) == 0)
		{
			write_wt_buf_last(blk, wt_buf0, ch_data_col_feeder_in);
			read_wt_buf(blk, wt_buf1, ch_data_feeder_to_pe);
		}
		else
		{
			read_wt_buf(blk, wt_buf0, ch_data_feeder_to_pe);
			write_wt_buf_last(blk, wt_buf1, ch_data_col_feeder_in);
		}

	//	buffer_id_flag = !buffer_id_flag;
	}

//	read_wt_buf(wt_buf1, ch_data_feeder_to_pe);
}
