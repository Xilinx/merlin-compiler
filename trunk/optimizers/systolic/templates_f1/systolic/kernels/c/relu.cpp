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



//static void write_relu_buf(int it, vec_output_t *relu_buf, hls::stream<vec_output_t> &ch_drain_to_relu)
static void write_relu_buf(int it, o_data_from_pe *relu_buf, hls::stream<o_data_from_pe> &ch_drain_to_relu)
{
	for (int r2 = 0; r2 < R; r2++)
	for (int r1 = 0; r1 < BS_R; r1++)
	{
		for (int o1 = 0; o1 < BS_O; o1++)
		{
#pragma HLS PIPELINE
		//	struct vec_output_t v;
			o_data_from_pe ap_out;
			int relu_buf_idx = ((R - r2 - 1) * BS_R + r1) * BS_O + o1;
			
			if (it < BN_R * BN_O)
			{
			//	v = ch_drain_to_relu.read();
				ap_out = ch_drain_to_relu.read();
			}

		//	relu_buf[relu_buf_idx] = v;
			relu_buf[relu_buf_idx] = ap_out;
		}
	}
}

//static void read_relu_buf(int it, vec_output_t *relu_buf, hls::stream<vec_output_t> &ch_relu_to_mem)
static void read_relu_buf(int it, o_data_from_pe *relu_buf, hls::stream<o_data_from_pe> &ch_relu_to_mem)
{
	for (int r2 = 0; r2 < R; r2++)
	for (int r1 = 0; r1 < BS_R; r1++)
	{
		for (int o1 = 0; o1 < BS_O; o1++)
		{
#pragma HLS PIPELINE
			int relu_buf_idx = (r2 * BS_R + r1) * BS_O + o1;

			if (it > 0)
				ch_relu_to_mem << relu_buf[relu_buf_idx];
		}
	}
}

//static void relu(hls::stream<vec_output_t> &ch_drain_to_relu, hls::stream<vec_output_t> &ch_relu_to_mem)
static void relu(hls::stream<o_data_from_pe> &ch_drain_to_relu, hls::stream<o_data_from_pe> &ch_relu_to_mem)
{
//	vec_output_t relu_buf0[R * BS_R * BS_O];
//	vec_output_t relu_buf1[R * BS_R * BS_O];
	o_data_from_pe relu_buf0[R * BS_R * BS_O];
	o_data_from_pe relu_buf1[R * BS_R * BS_O];

	for (int it = 0; it < (BN_R * BN_O) + 1; it++)
	{
		if ((it & 1) == 0)
		{
			write_relu_buf(it, relu_buf0, ch_drain_to_relu);
			read_relu_buf(it, relu_buf1, ch_relu_to_mem);
		}
		else
		{
			read_relu_buf(it, relu_buf0, ch_relu_to_mem);
			write_relu_buf(it, relu_buf1, ch_drain_to_relu);
		}
	}
}
