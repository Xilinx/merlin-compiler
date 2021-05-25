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



static void	load_in_and_forward(vec_input_t *ifmap, hls::stream<in_data_loader_to_feeder> &ch_data_row_loader_to_first_feeder)
{
	bool new_row_col_pair = false;
	struct in_data_loader_to_feeder in_data;
#pragma HLS DATA_PACK variable=in_data.pe_data

#if (DEBUG_AP_INT_PACK == 1)
	vec_input_t ddr_data;
#endif

	for (int r0 = 0; r0 < BN_R; r0++)
	{
		for (int o0 = 0; o0 < BN_O; o0++)
		{
			for (int i0 = 0; i0 < BN_I; i0++)
			{
				for (int vector_id_in_block = 0; vector_id_in_block < (R * BS_R * BS_I); vector_id_in_block++)
				{
#pragma HLS PIPELINE

#if (DEBUG_AP_INT_PACK == 1)
					ddr_data = ifmap[(r0 * BN_I + i0) * (R * BS_R * BS_I) + vector_id_in_block];
					vec_in_data_to_ap_in_data(ddr_data.data, in_data.pe_data);
#else
					in_data.pe_data = ifmap[(r0 * BN_I + i0) * (R * BS_R * BS_I) + vector_id_in_block];
#endif
				
					in_data.new_row_col_pair = new_row_col_pair;

					ch_data_row_loader_to_first_feeder << in_data;
				}

				new_row_col_pair = false;
			}
			new_row_col_pair = true;
		}
	}

	for (int i0 = 0; i0 < BN_I; i0++)
	{
		for (int vector_id_in_block = 0; vector_id_in_block < (R * BS_R * BS_I); vector_id_in_block++)
		{
#pragma HLS PIPELINE

			// in_data.data = 0;
#if (DEBUG_AP_INT_PACK == 1)
			in_data.pe_data = 0;
#else
			for (int i2 = 0; i2 < I; i2++)
			{
#pragma HLS UNROLL
			//	in_data.pe_simd[i2] = 0;
				in_data.pe_data.data[i2] = 0;
			}
#endif

			in_data.new_row_col_pair = new_row_col_pair;

			ch_data_row_loader_to_first_feeder << in_data;
		}
		new_row_col_pair = false;
	}
}

