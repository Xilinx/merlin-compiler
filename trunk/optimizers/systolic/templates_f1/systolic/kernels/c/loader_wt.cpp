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



//static void	load_wt_and_forward(data_bitwidth *weight, hls::stream<wt_data_to_feeder> &ch_data_loader_wt_to_first_feeder)
static void	load_wt_and_forward(vec_input_t *weight, hls::stream<wt_data_loader_to_feeder> &ch_data_loader_wt_to_first_feeder)
{
	struct wt_data_loader_to_feeder wt_data;
	vec_input_t ddr_data;

	for (int r0 = 0; r0 < BN_R; r0++)
	{
		for (int o0 = 0; o0 < BN_O; o0++)
		{
			for (int i0 = 0; i0 < BN_I; i0++)
			{
				for (int vector_id_in_block = 0; vector_id_in_block < (O * BS_O * BS_I); vector_id_in_block++)
				{
#pragma HLS PIPELINE

#if (DEBUG_AP_INT_PACK == 1)
					ddr_data = weight[(o0 * BN_I + i0) * (O * BS_O * BS_I) + vector_id_in_block];
					vec_in_data_to_ap_in_data(ddr_data.data, wt_data.pe_data);
#else
					wt_data.pe_data = weight[(o0 * BN_I + i0) * (O * BS_O * BS_I) + vector_id_in_block];
#endif


					ch_data_loader_wt_to_first_feeder << wt_data;
				}
			}
		}
	}

	for (int i0 = 0; i0 < BN_I; i0++)
	{
		for (int vector_id_in_block = 0; vector_id_in_block < (O * BS_O * BS_I); vector_id_in_block++)
		{
#pragma HLS PIPELINE

#if (DEBUG_AP_INT_PACK == 1)
			wt_data.pe_data = 0;
#else
			for (int i2 = 0; i2 < I; i2++)
			{
#pragma HLS UNROLL
			//	wt_data.pe_simd[i2] = 0;
				wt_data.pe_data.data[i2] = 0;
			}
#endif

			ch_data_loader_wt_to_first_feeder << wt_data;
		}
	}
}

