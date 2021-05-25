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



#include <hls_stream.h>
#include <ap_int.h>

#include "type_def.h"

#include "systolic_params.h"

#if (TEST_TENSOR_TYPE == 1)
static void load_ifmap(int i0, 
		dram_trans_t *ifmap,
		int ifmap_blk_offset,
		data_bitwidth ifmap_buf[BS_I][R * BS_R + MK1 - 1][C * BS_C + MK2 - 1][I], 
		OpCFG &cfg)
#else
//static void load_ifmap(data_bitwidth *ifmap,
static void load_ifmap(int i0, 
		data_bitwidth ifmap[][I],
		int ifmap_blk_offset,
		data_bitwidth ifmap_buf[BS_I][R * BS_R + MK1 - 1][C * BS_C + MK2 - 1][I], 
		OpCFG &cfg)
#endif
{
//#pragma HLS ARRAY_PARTITION variable=test_buf complete dim=2

	if (i0 < (cfg.BN_I))
	{
		int num_packed_data_in_tile = (R * BS_R + cfg.k1 - 1) * (C * BS_C + cfg.k2 - 1) * (BS_I * I) / NUM_PACKED_DATA;

		int r1_r2_p = 0;
		int c1_c2_q = 0;
		int i1 = 0;
	//	for (int r1_r2_p = 0; r1_r2_p < (R * BS_R + cfg.k1 - 1); r1_r2_p++)
	//	for (int c1_c2_q = 0; c1_c2_q < (C * BS_C + cfg.k2 - 1); c1_c2_q++)
	//	for (int i1 = 0; i1 < BS_I; i1++)
		for (int i = 0; i < num_packed_data_in_tile; i++)
		{
			for (int j = 0; j < (NUM_PACKED_DATA / I); j++)
			{
	#pragma HLS PIPELINE
				for (int i2 = 0; i2 < I; i2++)
				{
		#pragma HLS UNROLL
					int ifmap_buf_idx = (r1_r2_p * (C * BS_C + cfg.k2 - 1) + c1_c2_q) * BS_I * I + i1 * I + i2;
			//		int ifmap_idx = ifmap_blk_offset + ifmap_buf_idx;
		
				//	ifmap_buf[i1][r1_r2_p][c1_c2_q][i2] = (ifmap + ifmap_blk_offset)[ifmap_buf_idx];
				//	ifmap_buf[i1][r1_r2_p][c1_c2_q][i2] = ifmap[ifmap_buf_idx];
				//	ifmap_buf[i1][r1_r2_p][c1_c2_q][i2] = ifmap[ifmap_idx];
	#if (TEST_TENSOR_TYPE == 1)
					ifmap_buf[i1][r1_r2_p][c1_c2_q][i2] = ifmap[ifmap_blk_offset / NUM_PACKED_DATA + i]((j * I + i2) * DATA_BITWIDTH + DATA_BITWIDTH - 1, (j * I + i2) * DATA_BITWIDTH);
	#else
					ifmap_buf[i1][r1_r2_p][c1_c2_q][i2] = ifmap[ifmap_blk_offset / I + ifmap_buf_idx / I][i2];
	#endif
				}

				i1++;
				if (i1 == BS_I)
				{
					i1 = 0;
					r1_r2_p++;
					if (r1_r2_p == BS_R * R + cfg.k1 - 1)
					{
						r1_r2_p = 0;
						c1_c2_q++;
						if (c1_c2_q == BS_C * C + cfg.k2 - 1)
						{
							c1_c2_q = 0;
						}
					}
				}
			}
		}
	}
}

static void load_wt(int i0,
		dram_trans_t *weight,
		int wt_blk_offset, 
		data_bitwidth wt_buf[BS_O * MK1 * MK2 * BS_I][O][I], 
		OpCFG &cfg)
{
	if (i0 < cfg.BN_I)
	{
	//	for (int o1 = 0; o1 < BS_O; o1++)
	//	for (int o2 = 0; o2 < O; o1++)
	//	for (int p = 0; p < cfg.k1; p++)
	//	for (int q = 0; q < cfg.k2; q++)
	//	for (int i1 = 0; i1 < BS_I; i1++)
	
		int num_packed_data_in_tile = (BS_O * O * cfg.k1 * cfg.k2 * BS_I * I) / NUM_PACKED_DATA;
	
		int it = 0;
		int o2 = 0;
		for (int i = 0; i < num_packed_data_in_tile; i++)
		{
	#pragma HLS PIPELINE
		//	dram_trans_t temp = weight[(wt_blk_offset / I) + i];
	
			for (int j = 0; j < (NUM_PACKED_DATA / I); j++)
			{
	#pragma HLS PIPELINE
				for (int i2 = 0; i2 < I; i2++)
				{
	#pragma HLS UNROLL
				//	wt_buf[it][o2][i2] = temp.range((j * I + i2) * DATA_BITWIDTH + DATA_BITWIDTH - 1, (j * I + i2) * DATA_BITWIDTH);
				//	wt_buf[it][o2][i2] = temp((j * I + i2) * DATA_BITWIDTH + DATA_BITWIDTH - 1, (j * I + i2) * DATA_BITWIDTH);
					wt_buf[it][o2][i2] = weight[(wt_blk_offset / I) + i]((j * I + i2) * DATA_BITWIDTH + DATA_BITWIDTH - 1, (j * I + i2) * DATA_BITWIDTH);
				}
	
				o2++;
				if (o2 == O)
				{
					o2 = 0;
					it++;
					if (it == (BS_O * cfg.k1 * cfg.k2 * BS_I))
					{
						it = 0;
					}
				}
			}
		}

	}
}

static void conv2d_calc_tile(int i0,
		data_bitwidth ifmap_buf[BS_I][R * BS_R + MK1 - 1][C * BS_C + MK2 - 1][I], 
		data_bitwidth wt_buf[BS_O * MK1 * MK2 * BS_I][O][I], 
		data_bitwidth ofmap_buf[BS_R * BS_C * BS_O][R][C][O], 
		OpCFG &cfg)
{
#pragma HLS inline off

	if (i0 > 0)
	{

//	data_bitwidth ofmap_reg[R][C][O];
//#pragma HLS ARRAY_PARTITION variable=ofmap_reg complete dim=1
//#pragma HLS ARRAY_PARTITION variable=ofmap_reg complete dim=2
//#pragma HLS ARRAY_PARTITION variable=ofmap_reg complete dim=3
		data_bitwidth ofmap_reg[R * C * O];
	#pragma HLS ARRAY_PARTITION variable=ofmap_reg complete
	
		for (int p = 0; p < cfg.k1; p++)
		for (int q = 0; q < cfg.k2; q++)
		for (int i1 = 0; i1 < BS_I; i1++)
		for (int r1 = 0; r1 < BS_R; r1++)
		for (int c1 = 0; c1 < BS_C; c1++)
		for (int o1 = 0; o1 < BS_O; o1++)
		{
//	#pragma HLS DEPENDENCE variable=ofmap_buf inter false
	#pragma HLS PIPELINE
	
		//	int i = i0 * BS_I + i1;
	
			for (int r2 = 0; r2 < R; r2++)
	#pragma HLS UNROLL
			for (int c2 = 0; c2 < C; c2++)
	#pragma HLS UNROLL
			for (int o2 = 0; o2 < O; o2++)
			{
	#pragma HLS UNROLL
				ofmap_reg[(r2 * C + c2) * O + o2] = ((i0 == 1 && i1 == 0 && p == 0 && q == 0) ? 0 : ofmap_buf[(r1 * BS_C + c1) * BS_O + o1][r2][c2][o2]);
			}
	
			for (int r2 = 0; r2 < R; r2++)
	#pragma HLS UNROLL
			for (int c2 = 0; c2 < C; c2++)
			{
	#pragma HLS UNROLL
				for (int o2 = 0; o2 < O; o2++)
				{
	#pragma HLS UNROLL
					data_bitwidth temp = 0;
					for (int i2 = 0; i2 < I; i2++)
					{
	#pragma HLS UNROLL
						temp +=
							ifmap_buf[i1][r1 * R + r2 + p][c1 * C + c2 + q][i2] * wt_buf[(r1 * BS_C + c1) * BS_O + o1][o2][i2];
					}
					ofmap_reg[(r2 * C + c2) * O + o2] += temp;
				}
			}
	
			for (int r2 = 0; r2 < R; r2++)
	#pragma HLS UNROLL
			for (int c2 = 0; c2 < C; c2++)
	#pragma HLS UNROLL
			for (int o2 = 0; o2 < O; o2++)
			{
	#pragma HLS UNROLL
				ofmap_buf[(r1 * BS_C + c1) * BS_O + o1][r2][c2][o2]	= ofmap_reg[(r2 * C + c2) * O + o2];
			}
		}
	}
}

/*
static void init_ofmap_buf(data_bitwidth ofmap_buf[BS_R * BS_C * BS_O][R][C][O],
		OpCFG &cfg)
{
	for (int r1 = 0; r1 < BS_R; r1++)
	for (int c1 = 0; c1 < BS_C; c1++)
	for (int o1 = 0; o1 < BS_O; o1++)
#pragma HLS PIPELINE
	for (int r2 = 0; r2 < R; r2++)
#pragma HLS UNROLL
	for (int c2 = 0; c2 < C; c2++)
#pragma HLS UNROLL
	for (int o2 = 0; o2 < O; o2++)
	{
#pragma HLS UNROLL
		ofmap_buf[(r1 * BS_C + c1) * BS_O + o1][r2][c2][o2] = 0;
	}
}
*/

#if (TEST_TENSOR_TYPE == 1)
static void store_ofmap(dram_trans_t *ofmap, 
		int ofmap_blk_offset,
		data_bitwidth ofmap_buf[BS_R * BS_C * BS_O][R][C][O], 
		OpCFG &cfg)
#else
static void store_ofmap(data_bitwidth ofmap[][I], 
		int ofmap_blk_offset,
		data_bitwidth ofmap_buf[BS_R * BS_C * BS_O][R][C][O], 
		OpCFG &cfg) 
#endif
{
	/*
	 * FIXME: currently O must be divided by I
	 * */
	int num_packed_data_in_tile = (BS_R * R * BS_C * C * BS_O * O) / NUM_PACKED_DATA;
//	for (int r1 = 0; r1 < BS_R; r1++)
//	for (int c1 = 0; c1 < BS_C; c1++)
//	for (int o1 = 0; o1 < BS_O; o1++)
//	for (int r2 = 0; r2 < R; r2++)
//	for (int c2 = 0; c2 < C; c2++)
		
	int r1_c1_o1 = 0;
	int r2 = 0;
	int c2 = 0;
	int o2_i2 = 0;
	for (int i = 0; i < num_packed_data_in_tile; i++)
	{
	//	for (int o2_i2 = 0; o2_i2 < (O / I); o2_i2++)
		for (int j = 0; j < NUM_PACKED_DATA / I; j++)
		{
#pragma HLS PIPELINE
			for (int i2 = 0; i2 < I; i2++)
			{
#pragma HLS UNROLL

#if (TEST_TENSOR_TYPE == 1)
				ofmap[(ofmap_blk_offset / NUM_PACKED_DATA) + i]((j * I + i2) * DATA_BITWIDTH + DATA_BITWIDTH - 1, (j * I + i2) * DATA_BITWIDTH) = 
					ofmap_buf[r1_c1_o1][r2][c2][o2_i2 * I + i2];
#else
			//	// (r1,r2,c1,c2,o1,o2)
			//	int ofmap_buf_idx = ((((r1 * R + r2) * BS_C + c1) * C + c2) * BS_O + o1) * (O / I) + o2_i2;
				// (r0%cfg.pooling_stride,(r1*R+r2)/cfg.pooling_stride,c0%cfg.pooling_stride,(c1*C+c2)/cfg.pooling_stride,o1,o2)
				int ofmap_buf_idx = ((((r0 % cfg.pooling_stride) * (BS_R * R / cfg.pooling_stride) + (r1 * R + r2) / cfg.pooling_stride) * BS_C * C + (c0 % cfg.pooling_stride) * (BS_C * C / cfg.pooling_stride) + (c1 * C + c2) / cfg.pooling_stride) * BS_O + o1) * (O / I) + o2_i2;
				int ofmap_idx = (ofmap_blk_offset / I) + ofmap_buf_idx;

				ofmap[ofmap_idx][i2] = ofmap_buf[(r1 * BS_C + c1) * BS_O + o1][r2][c2][o2_i2 * I + i2];
#endif
			}

			o2_i2++;
			if (o2_i2 == (O / I))
			{
				o2_i2 = 0;
				c2++;
				if (c2 == C)
				{
					c2 = 0;
					r2++;
					if (r2 == R)
					{
						r2 = 0;
						r1_c1_o1++;
						if (r1_c1_o1 = BS_R * BS_C * BS_O)
						{
							r1_c1_o1 = 0;
						}
					}
				}
			}
		}
	}
}

#if (TEST_TENSOR_TYPE == 1)
void conv2d(OpCFG *cfg,
		int layer_idx,
		dram_trans_t *ifmap,
		dram_trans_t *weights,
		dram_trans_t *ofmap,
		int wt_offset)
#else
void conv2d(OpCFG *cfg,
		int layer_idx,
		data_bitwidth ifmap[][I],
		dram_trans_t *weights,
		data_bitwidth ofmap[][I],
		int wt_offset)
#endif
{
//#pragma HLS INTERFACE s_axilite port=ifmap bundle=control
//#pragma HLS INTERFACE m_axi port=weights offset=slave bundle=weights
//#pragma HLS INTERFACE s_axilite port=weights bundle=control
//#pragma HLS INTERFACE s_axilite port=ofmap bundle=control

//	data_bitwidth *weight = weights + wt_offset;
	dram_trans_t *weight = weights + (wt_offset / NUM_PACKED_DATA);

	data_bitwidth ifmap_buf0[BS_I][(R * BS_R + MK1 - 1)][(C * BS_C + MK2 - 1)][I];
	data_bitwidth ifmap_buf1[BS_I][(R * BS_R + MK1 - 1)][(C * BS_C + MK2 - 1)][I];

	data_bitwidth wt_buf0[BS_O * MK1 * MK2 * BS_I][O][I];
	data_bitwidth wt_buf1[BS_O * MK1 * MK2 * BS_I][O][I];

//	vec_input_t ofmap_buf0[BS_R * BS_C * BS_O / I][R][C][O];
	data_bitwidth ofmap_buf0[BS_R * BS_C * BS_O][R][C][O];
//	data_bitwidth ofmap_buf1[BS_R * BS_C * BS_O][R][C][O];

//	data_bitwidth test_buf[56 * 56 * 256 / I][I];
//#pragma HLS RESOURCE variable=test_buf core=XPM_MEMORY uram


#pragma HLS ARRAY_PARTITION variable=ifmap_buf0 factor=4 cyclic dim=2
#pragma HLS ARRAY_PARTITION variable=ifmap_buf0 factor=4 cyclic dim=3
#pragma HLS ARRAY_PARTITION variable=ifmap_buf0 complete dim=4
#pragma HLS ARRAY_PARTITION variable=ifmap_buf1 factor=4 cyclic dim=2
#pragma HLS ARRAY_PARTITION variable=ifmap_buf1 factor=4 cyclic dim=3
#pragma HLS ARRAY_PARTITION variable=ifmap_buf1 complete dim=4

#pragma HLS ARRAY_PARTITION variable=wt_buf0 factor=4 complete  dim=2
#pragma HLS ARRAY_PARTITION variable=wt_buf0 factor=4 complete  dim=3
#pragma HLS ARRAY_PARTITION variable=wt_buf1 factor=4 complete  dim=2
#pragma HLS ARRAY_PARTITION variable=wt_buf1 factor=4 complete  dim=3

#pragma HLS ARRAY_PARTITION variable=ofmap_buf0 factor=1 complete  dim=2
#pragma HLS ARRAY_PARTITION variable=ofmap_buf0 factor=4 complete  dim=3
#pragma HLS ARRAY_PARTITION variable=ofmap_buf0 factor=4 complete  dim=4
//#pragma HLS ARRAY_PARTITION variable=ofmap_buf1 factor=1 complete  dim=2
//#pragma HLS ARRAY_PARTITION variable=ofmap_buf1 factor=4 complete  dim=3
//#pragma HLS ARRAY_PARTITION variable=ofmap_buf1 factor=16 complete  dim=4

//#pragma HLS RESOURCE variable=ifmap_buf0 core=RAM_2P

//#pragma HLS stream variable=ifmap depth=1 
//#pragma HLS stream variable=ifmap_buf0 depth=1 
//#pragma HLS stream variable=wt_buf0 depth=1
//#pragma HLS stream variable=ofmap_buf0 depth=1

	OpCFG cfg_a = cfg[layer_idx];


//	int r0 = 0;
//	int c0 = 0;
//	int o0 = 0;
//	int i0 = 0;
	for (int r0 = 0; r0 < cfg_a.BN_R; r0++)
	for (int c0 = 0; c0 < cfg_a.BN_C; c0++)
	for (int o0 = 0; o0 < cfg_a.BN_O; o0++)
	{
		bool ping_pong_flag = 1;
		int ofmap_blk_offset = (((r0 / cfg_a.pooling_stride) * (cfg_a.BN_C / cfg_a.pooling_stride) + (c0 / cfg_a.pooling_stride)) * cfg_a.BN_O + o0) * (BS_R * R * BS_C * C * BS_O * O);
	//	int ofmap_blk_offset = ((r0 * cfg_a.BN_C + c0) * cfg_a.BN_O + o0) * (BS_R * R * BS_C * C * BS_O * O);

		for (int i0 = 0; i0 < (cfg_a.BN_I + 1); i0++)
//	for (int it = 0; it < (cfg_a.BN_R * cfg_a.BN_C * cfg_a.BN_O * cfg_a.BN_I + 2); it++)
//	{
		{
			int ifmap_blk_offset = ((r0 * cfg_a.BN_C + c0) * cfg_a.BN_I + i0) * (BS_R * R + cfg_a.k1 - 1) * (BS_C * C + cfg_a.k2 - 1) * BS_I * I;
			int wt_blk_offset = (o0 * cfg_a.BN_I + i0) * BS_O * O * cfg_a.k1 * cfg_a.k2 * BS_I * I;

			if (ping_pong_flag == 1)
			{
				load_ifmap(i0, ifmap, ifmap_blk_offset, ifmap_buf0, cfg_a);

				load_wt(i0, weight, wt_blk_offset, wt_buf0, cfg_a);

				conv2d_calc_tile(i0, ifmap_buf1, wt_buf1, ofmap_buf0, cfg_a);
			}
			else
			{
				load_ifmap(i0, ifmap, ifmap_blk_offset, ifmap_buf1, cfg_a);

				load_wt(i0, weight, wt_blk_offset, wt_buf1, cfg_a);

				conv2d_calc_tile(i0, ifmap_buf0, wt_buf0, ofmap_buf0, cfg_a);
			}

			ping_pong_flag = 1 - ping_pong_flag;
		}

		store_ofmap(ofmap, ofmap_blk_offset, ofmap_buf0, cfg_a);
	//	i0++;
	//	if (i0 == cfg_a.BN_I)
	//	{
	//		i0 = 0;
	//		o0++;
	//		if (o0 == cfg_a.BN_O)
	//		{
	//			o0 = 0;
	//			c0++;
	//			if (c0 == cfg_a.BN_C)
	//			{
	//				c0 = 0;
	//				r0++;
	//				if (r0 == cfg_a.BN_R)
	//				{
	//					r0 = 0;
	//				}
	//			}
	//		}
	//	}
	}
}
