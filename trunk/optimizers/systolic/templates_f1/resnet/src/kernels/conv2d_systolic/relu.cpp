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



static void write_relu_buf(int it, 
		vec_output_t *relu_buf,
		hls::stream<vec_output_t> &ch_drain_to_relu, 
		OpCFG &cfg)
{
	for (int r1 = 0; r1 < BS_R; r1++)
	for (int o1 = 0; o1 < BS_O; o1++)
	for (int r2 = 0; r2 < R; r2++)
	for (int c1 = 0; c1 < BS_C; c1++)
	for (int c2 = 0; c2 < C; c2++)
	{
#pragma HLS PIPELINE
		struct vec_output_t v;
		int relu_buf_idx = (((r1 * R + r2) * BS_C + c1) * C + c2) * BS_O + o1;

	//	if (it < BN_R * BN_O)
		if (it < cfg.BN_R * cfg.BN_O)
			v = ch_drain_to_relu.read();

		relu_buf[relu_buf_idx] = v;
	}
}

static void read_relu_buf(int it,
		vec_output_t *relu_buf,
		hls::stream<vec_output_t> &ch_relu_to_mem)
{
	for (int r1 = 0; r1 < BS_R; r1++)
	for (int r2 = 0; r2 < R; r2++)
	for (int o1 = 0; o1 < BS_O; o1++)
	for (int c1 = 0; c1 < BS_C; c1++)
	for (int c2 = 0; c2 < C; c2++)
	{
#pragma HLS PIPELINE
		int relu_buf_idx = (((r1 * R + r2) * BS_C + c1) * C + c2) * BS_O + o1;

		if (it > 0)
			ch_relu_to_mem << relu_buf[relu_buf_idx];
	}
}

static void relu(hls::stream<vec_output_t> &ch_drain_to_relu,
		hls::stream<vec_output_t> &ch_relu_to_mem, 
		OpCFG &cfg)
{
	vec_output_t relu_buf0[BS_R * R * BS_C * C * BS_O];
	vec_output_t relu_buf1[BS_R * R * BS_C * C * BS_O];

//	for (int it = 0; it < (BN_R * BN_O + 1); it++)
	for (int it = 0; it < (cfg.BN_R * cfg.BN_O + 1); it++)
	{
		if ((it & 1) == 0)
		{
			write_relu_buf(it, relu_buf0, ch_drain_to_relu, cfg);
			read_relu_buf(it, relu_buf1, ch_relu_to_mem);
		}
		else
		{
			read_relu_buf(it, relu_buf0, ch_relu_to_mem);
			write_relu_buf(it, relu_buf1, ch_drain_to_relu, cfg);
		}
	}
}
