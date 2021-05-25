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



//static void write_to_ddr(hls::stream<vec_output_t> &ch_drain_to_mem,
//		vec_output_t *ofmap)
static void write_to_ddr(hls::stream<vec_output_t> &ch_drain_to_mem,
		vec_input_t *ofmap, 
		OpCFG &cfg)
{
	for (int r0 = 0; r0 < cfg.BN_R; r0++)
	for (int o0 = 0; o0 < cfg.BN_O; o0++)
	{
	//	for (int r2 = 0; r2 < R; r2++)
	//	for (int r1 = 0; r1 < BS_R; r1++)
	//	for (int o2 = 0; o2 < O; o2++)
	//	for (int o1 = 0; o1 < BS_O; o1++)
		for (int o_buf_idx = 0; o_buf_idx < ((R * BS_R * C * BS_C * O * BS_O) / OUT_PACK_SIZE); o_buf_idx++)
		{
#pragma HLS PIPELINE
		//	int o_idx = (r0 * BN_O + o0) * (R * BS_R * C * BS_C * BS_O * O / OUT_PACK_SIZE) + o_buf_idx;

		//	data_bitwidth data = ch_drain_to_postproc.read();
			vec_output_t v = ch_drain_to_mem.read();

			for (int o2_i2 = 0; o2_i2 < (O / I); o2_i2++)
			{
#pragma HLS PIPELINE
				int o_idx = (r0 * cfg.BN_O + o0) * (R * BS_R * C * BS_C * BS_O * O / OUT_PACK_SIZE) + o_buf_idx * (O / I) + o2_i2;
				vec_input_t u;

				for (int i2 = 0; i2 < I; i2++)
				{
#pragma HLS UNROLL
					u.data[i2] = v.data[o2_i2 * I + i2];
				}
				ofmap[o_idx] = u;
			}
		}
	}
}
