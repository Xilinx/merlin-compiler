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



//static void	load_in_and_forward(vec_input_t *ifmap, hls::stream<in_data_loader_to_feeder> &ch_data_row_loader_to_first_feeder)
static void	load_in_and_forward(vec_input_t *ifmap,
		hls::stream<in_data_loader_to_feeder> &ch_data_row_loader_to_first_feeder, 
		OpCFG &cfg)
{
	bool new_row_col_pair = false;
	struct in_data_loader_to_feeder in_data;
	vec_input_t ddr_data;

	for (int r0 = 0; r0 < cfg.BN_R; r0++)
	for (int c0 = 0; c0 < cfg.BN_C; c0++)
	for (int o0 = 0; o0 < cfg.BN_O; o0++)
	{
		for (int i0 = 0; i0 < cfg.BN_I; i0++)
		{
			for (int vector_id_in_block = 0; vector_id_in_block < ((BS_R * R + K1 - 1) * (BS_C * C + K2 - 1) * BS_I); vector_id_in_block++)
			{
#pragma HLS PIPELINE

				ddr_data = ifmap[((r0 * cfg.BN_C + c0) * cfg.BN_I + i0) * ((R * BS_R + K1 - 1) * (C * BS_C + K2 - 1) * BS_I) + vector_id_in_block];
				vec_in_data_to_ap_in_data(ddr_data.data, in_data.pe_data);

				in_data.new_row_col_pair = new_row_col_pair;

				ch_data_row_loader_to_first_feeder << in_data;
			}
			new_row_col_pair = false;
		}
		new_row_col_pair = true;
	}

	for (int i0 = 0; i0 < cfg.BN_I; i0++)
	{
		for (int vector_id_in_block = 0; vector_id_in_block < ((BS_R * R  + K1 - 1) * (BS_C * C + K2 - 1) * BS_I); vector_id_in_block++)
		{
#pragma HLS PIPELINE

			// in_data.data = 0;
//			for (int i2 = 0; i2 < I; i2++)
//			{
//#pragma HLS UNROLL
//				in_data.pe_data.data[i2] = 0;
//			}
			in_data.pe_data = 0;

			in_data.new_row_col_pair = new_row_col_pair;

			ch_data_row_loader_to_first_feeder << in_data;
		}
		new_row_col_pair = false;
	}
}


