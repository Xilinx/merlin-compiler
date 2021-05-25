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



#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "type_def.h"

#include "systolic_params.h"
#include "config.h"

#include "lbj.h"

#include "util.h"

static void conv2d_calc_tile(data_bitwidth *ifmap_buf,
		data_bitwidth *wt_buf,
		data_bitwidth *ofmap_buf, 
		OpCFG &cfg)
{
//	memset(ofmap_buf, 0, BS_R * R * BS_C * C * BS_O * O * sizeof(data_bitwidth));

	for (int r1 = 0; r1 < BS_R; r1++)
	for (int c1 = 0; c1 < BS_C; c1++)
	for (int o1 = 0; o1 < BS_O; o1++)
	for (int i1 = 0; i1 < BS_I; i1++)
	for (int p = 0; p < cfg.k1; p++)
	for (int q = 0; q < cfg.k2; q++)
	{
		for (int r2 = 0; r2 < R; r2++)
		for (int c2 = 0; c2 < C; c2++)
		for (int o2 = 0; o2 < O; o2++)
		for (int i2 = 0; i2 < I; i2++)
		{
			// (r0,c0,i0,r1*R+r2+p,c1*C+c2+q,i1,i2)
		//	int ifmap_idx = ((r0 * cfg.BN_C + c0) * cfg.BN_I + i0) * ((BS_R * R + cfg.k1 - 1) * (BS_C * C + cfg.k2 - 1) * BS_I * I) + ((r1 * R + r2 + p) * (BS_C * C + cfg.k2 - 1) + (c1 * C + c2 + q)) * BS_I * I + i1 * I + i2;
			int ifmap_idx = ((r1 * R + r2 + p) * (BS_C * C + cfg.k2 - 1) + (c1 * C + c2 + q)) * BS_I * I + i1 * I + i2;
			// (o0,i0,o2,o1,p,q,i1,i2)
		//	int wt_idx = (o0 * cfg.BN_I + i0) * O * BS_O * cfg.k1 * cfg.k2 * BS_I * I + (((o1 * O + o2) * cfg.k1 + p) * cfg.k2 + q) * BS_I * I + i1 * I + i2;
			int wt_idx = (((o1 * O + o2) * cfg.k1 + p) * cfg.k2 + q) * BS_I * I + i1 * I + i2;

			// (r0,c0,o0,r1,r2,c1,c2,o1,o2)
		//	int ofmap_idx = ((r0 * cfg.BN_C + c0) * cfg.BN_O + o0) * BS_R * R * BS_C * C * BS_O * O + ((r1 * R + r2) * BS_C * C + c1 * C + c2) * BS_O * O + o1 * O + o2;
			int ofmap_idx = ((r1 * R + r2) * BS_C * C + c1 * C + c2) * BS_O * O + o1 * O + o2;

	//		if (ifmap_idx > num_elem_input_fm || wt_idx > num_elem_weight || ofmap_idx > num_elem_output_fm)
	//		{
	//			printf("ifmap_idx=%d, wt_idx=%d, ofmap_idx=%d\n", ifmap_idx, wt_idx, ofmap_idx);
	//		}
		
		//	printf("buf_length = %d\n", BS_R * R * BS_C * C * BS_O * O);
		//	printf("ofmap_buf[%d]=%d\n", ofmap_idx, ofmap_buf[ofmap_idx]);

			ofmap_buf[ofmap_idx] += ifmap_buf[ifmap_idx] * wt_buf[wt_idx];

		//	printf("%d+=%d*%d\n", ofmap_buf[ofmap_idx], ifmap_buf[ifmap_idx], wt_buf[wt_idx]);
		}
	}
}

static void pooling(data_bitwidth *ofmap_buf2,
		data_bitwidth *ofmap_buf3, 
		OpCFG &cfg, 
		int r0,
		int c0,
		int o0)
{
	/*
	 * In C model, pooling only performs down scaling, output to a scaled smaller buffer, 
	 * block merging will be performed in zero padding.
	 * Constraint: (BS_R * R) and (BS_C * C) must be divided by cfg.pooling_stride.
	 * */
	int BS_R_R_scaled = (BS_R * R) / cfg.pooling_stride;
	int BS_C_C_scaled = (BS_C * C) / cfg.pooling_stride;

	for (int r1 = 0; r1 < BS_R_R_scaled; r1++)
	for (int c1 = 0; c1 < BS_C_C_scaled; c1++)
	for (int o1 = 0; o1 < O * BS_O; o1++) 
	{
		data_bitwidth max = -1;
		int ofmap_buf3_idx = (r1 * BS_C_C_scaled + c1) * O * BS_O + o1;

		for (int p = 0; p < cfg.pooling_stride; p++)
		for (int q = 0; q < cfg.pooling_stride; q++)
		{
			int ofmap_buf2_idx = ((r1 * cfg.pooling_stride + p) * BS_C * C + (c1 * cfg.pooling_stride + q)) * O * BS_O + o1;

			if ((max == -1) || (max - ofmap_buf2[ofmap_buf2_idx]) < 0)
				max = ofmap_buf2[ofmap_buf2_idx];
		}

		ofmap_buf3[ofmap_buf3_idx] = max;
	}
}


static void store_ofmap(int layer_idx,
		OpCFG &cfg,
		data_bitwidth *out3_buf, data_bitwidth *out3,
		int r0, int c0, int o0)
{
	int cfg_BN_R = (cfg_img_row[layer_idx] + R * BS_R - 1) / (R * BS_R);
	int cfg_BN_C = (cfg_img_col[layer_idx] + BS_C * C - 1) / (BS_C * C);
	int cfg_BN_O = (cfg_out_num[layer_idx] + O * BS_O - 1) / (O * BS_O);

	int pooled_BN_R = (cfg.BN_R + cfg_pooling_stride[layer_idx] - 1) / cfg_pooling_stride[layer_idx];
	int pooled_BN_C = (cfg_BN_C + cfg_pooling_stride[layer_idx] - 1) / cfg_pooling_stride[layer_idx];


	int BS_R_R_scaled = (R * BS_R) / cfg_pooling_stride[layer_idx];
	int BS_C_C_scaled = (BS_C * C) / cfg_pooling_stride[layer_idx];

	int blk_idx_r0 = r0 / cfg_pooling_stride[layer_idx];
	int blk_idx_c0 = c0 / cfg_pooling_stride[layer_idx];

	int subblk_idx_r0 = r0 % cfg_pooling_stride[layer_idx];
	int subblk_idx_c0 = c0 % cfg_pooling_stride[layer_idx];

//	printf("blk_idx_r0=%d, blk_idx_c0=%d\n", blk_idx_r0, blk_idx_c0);
//	printf("subblk_idx_r0=%d, subblk_idx_c0=%d\n", subblk_idx_r0, subblk_idx_c0);
//	printf("BS_R_R_scaled=%d, BS_C_C_scaled=%d\n", BS_R_R_scaled, BS_C_C_scaled);

	for (int rr = 0; rr < BS_R_R_scaled; rr++)
	for (int cc = 0; cc < BS_C_C_scaled; cc++)
	for (int oo = 0; oo < O * BS_O; oo++)
	{
		int out3_buf_idx = (rr * BS_C_C_scaled + cc) * O * BS_O + oo;
		int out3_idx = ((blk_idx_r0 * pooled_BN_C + blk_idx_c0) * cfg.BN_O + o0) * R * BS_R * BS_C * C * O * BS_O + ((subblk_idx_r0 * BS_R_R_scaled + rr) * BS_C * C + (subblk_idx_c0 * BS_C_C_scaled + cc)) * O * BS_O + oo;

		out3[out3_idx] = out3_buf[out3_buf_idx];
	}
}

static void zero_padding(int layer,
		data_bitwidth *out3_buf, data_bitwidth *out5, 
		int r0, int c0, int i0)
{
}

static void init_ofmap_buf(data_bitwidth *ofmap_buf, 
		OpCFG &cfg)
{
	memset(ofmap_buf, 0, (BS_R * R * BS_C * C * BS_O * O) * sizeof(data_bitwidth));
}

static void conv2d(OpCFG *cfg,
		int layer_idx, 
		data_bitwidth *ifmap,
		data_bitwidth *weights,
		data_bitwidth *ofmap,
		int wt_offset)
{
	data_bitwidth *weight = weights + wt_offset;

	data_bitwidth ifmap_buf[(BS_R * R + MK1 - 1) * (BS_C * C + MK2 - 1) * BS_I * I];
	data_bitwidth wt_buf[BS_O * O * MK1 * MK2 * BS_I * I];
	data_bitwidth ofmap_buf[BS_R * R * BS_C * C * BS_O * O];

//	int	num_elem_input_fm = cfg.BN_R * cfg.BN_C * cfg.BN_I * (R * BS_R + K1 - 1) * (C * BS_C + K2 - 1) * BS_I * I;
//	int num_elem_weight = cfg.BN_O * cfg.BN_I * O * BS_O * K1 * K2 * BS_I * I;
//	int	num_elem_output_fm = cfg.BN_R * cfg.BN_C * cfg.BN_O * R * BS_R * C * BS_C * O * BS_O;
//
//	printf("num_elem_input_fm=%d, num_elem_weight=%d, num_elem_output_fm=%d\n", num_elem_input_fm, num_elem_weight, num_elem_output_fm);
//	fflush(stdout);


	OpCFG cfg_a = cfg[layer_idx];

	for (int r0 = 0; r0 < cfg_a.BN_R; r0++)
	for (int c0 = 0; c0 < cfg_a.BN_C; c0++)
	for (int o0 = 0; o0 < cfg_a.BN_O; o0++)
	{
		init_ofmap_buf(ofmap_buf, cfg_a);

		for (int i0 = 0; i0 < cfg_a.BN_I; i0++)
		{
			int ifmap_tile_offset = ((r0 * cfg_a.BN_C + c0) * cfg_a.BN_I + i0) * (BS_R * R + cfg_a.k1 - 1) * (BS_C * C + cfg_a.k2 - 1) * BS_I * I;
			int ifmap_tile_size = (BS_R * R + cfg_a.k1 - 1) * (BS_C * C + cfg_a.k2 - 1) * BS_I * I;
			
			int wt_tile_offset = (o0 * cfg_a.BN_I + i0) * BS_O * O * cfg_a.k1 * cfg_a.k2 * BS_I * I;
			int wt_tile_size = BS_O * O * cfg_a.k1 * cfg_a.k2 * BS_I * I;

			memcpy(ifmap_buf, ifmap + ifmap_tile_offset, ifmap_tile_size * sizeof(data_bitwidth));
			memcpy(wt_buf, weight + wt_tile_offset,  wt_tile_size * sizeof(data_bitwidth));

			conv2d_calc_tile(ifmap_buf, wt_buf, ofmap_buf, cfg_a);
		}

	//	printf("(r0,c0,o0)=(%d,%d,%d)\n", r0, c0, o0);
	//	for (int rr = 0; rr < (BS_R * R); rr++)
	//	for (int cc = 0; cc < (BS_C * C); cc++)
	//	for (int oo = 0; oo < (BS_O * O); oo++)
	//	{
	//		int ofmap_buf_idx = rr * (BS_C * C * BS_O * O) + cc * (BS_O * O) + oo;

	//	//	if (ofmap_buf[ofmap_buf_idx] == 1)
	//		{
	//			printf("ofmap_buf[%d]=%d\n", ofmap_buf_idx, ofmap_buf[ofmap_buf_idx]);
	//		}
	//	}

		if (cfg_a.pooling_stride > 1)
		{
			pooling(ofmap_buf, ofmap_buf, cfg_a, r0, c0, o0);
		}

		if (layer_idx == (get_num_conv() - 1))
		{
			store_ofmap(layer_idx, cfg_a, ofmap_buf, ofmap, r0, c0, o0);
		}
		else
		{
			zero_padding(layer_idx, ofmap_buf, ofmap, r0, c0, o0);
		}

	//	for (int i = 0; i < 10; i++)
	//	{
	//		printf("%d\n", ofmap[i]);
	//	}

	//	memcpy(ofmap + ((r0 * cfg.BN_C + c0) * cfg.BN_O + o0) * (BS_R * R * BS_C * C * BS_O * O / (cfg.pooling_stride * cfg.pooling_stride)), ofmap_buf, (BS_R * R * BS_C * C * BS_O * O / cfg.pooling_stride) * sizeof(data_bitwidth));
	//	memcpy(ofmap + (((r0 / cfg.pooling_stride) * (cfg.BN_C / cfg.pooling_stride) + c0) * cfg.BN_O + o0) * (BS_R * R * BS_C * C * BS_O * O), ofmap_buf, (BS_R * R * BS_C * C * BS_O * O) * sizeof(data_bitwidth));
	}
}

static void resadd(data_bitwidth *ofmap_buf1, 
		data_bitwidth *ofmap_buf2,
		data_bitwidth *ofmap_buf3,
		int size)
{
	for (int i = 0; i < size; i++)
	{
		ofmap_buf3[i] = ofmap_buf1[i] + ofmap_buf2[i];
	}
}

void cnn_golden(data_bitwidth *ifmap,
		data_bitwidth *weights,
		data_bitwidth *ofmap,
		int start_block_idx,
		int end_block_idx,
		OpCFG *cfg)
{
	data_bitwidth ofmap_buf1[MAX_ACTIVATION_BUF_LEN]; 
	data_bitwidth ofmap_buf2[MAX_ACTIVATION_BUF_LEN];
	data_bitwidth ofmap_buf3[MAX_ACTIVATION_BUF_LEN];
	data_bitwidth ofmap_buf4[MAX_ACTIVATION_BUF_LEN];

	// read the feature map for the first layer
//	int	num_elem_input_fm = BN_R * BN_I * (R * BS_R + K1 - 1) * (C * BS_C + K2 - 1) * BS_I * I;
	int	num_elem_input_fm;
	int num_elem_weight;
	int	num_elem_output_fm;

	int wt_offset = 0;


#if 0
	num_elem_input_fm = (cfg[0].BN_R * cfg[0].BN_C * cfg[0].BN_I) * (BS_R * R + cfg[0].k1 - 1) * (BS_C * C + cfg[0].k2 - 1) * BS_I * I;

	printf("%s: num_elem_input_fm=%d\n", __FUNCTION__, num_elem_input_fm);
	fflush(stdout);

	memcpy(ofmap_buf4, ifmap, num_elem_input_fm * sizeof(data_bitwidth));

	printf("conv layer %d\n", 0);
	fflush(stdout);

	conv2d(ofmap_buf4, weights, ofmap_buf1, cfg[0], wt_offset);

#if (DEBUG_SINGLE_CONV_PERF == 1)
	num_elem_output_fm = ((cfg[0].BN_R / cfg[0].pooling_stride) * (cfg[0].BN_C / cfg[0].pooling_stride) * cfg[0].BN_O) * (BS_R * R) * (BS_C * C) * BS_O * O;

	printf("%s: num_elem_output_fm=%d\n", __FUNCTION__, num_elem_output_fm);

	memcpy(ofmap, ofmap_buf1, num_elem_output_fm * sizeof(data_bitwidth));

//	for (int i = 0; i < num_elem_output_fm; i++)
//	{
//		printf("%d\n", ofmap_buf4[i]);
//	}
#endif


//#if (DEBUG_SINGLE_CONV_PERF == 0)
	num_elem_input_fm = (cfg[1].BN_R * cfg[1].BN_C * cfg[1].BN_I) * (BS_R * R + cfg[1].k1 - 1) * (BS_C * C + cfg[1].k2 - 1) * BS_I * I;

	memcpy(ofmap_buf2, ofmap_buf1, num_elem_input_fm * sizeof(data_bitwidth));
	memcpy(ofmap_buf3, ofmap_buf1, num_elem_input_fm * sizeof(data_bitwidth));
#endif


	int layer_idx_base = 0;
//	int last_layer_idx = 0;
//	int building_block_idx;
	int layer_idx_within_building_block;

	for (int building_block_idx = 0; building_block_idx < start_block_idx; building_block_idx++)
	{
		layer_idx_base += building_block_content[building_block_idx];
	}

//	for (building_block_idx = 0; building_block_idx < BUILDING_BLOCK_NUM; building_block_idx++)
	for (int building_block_idx = start_block_idx; building_block_idx <= end_block_idx; building_block_idx++)
	{
	//	for (int layer_idx_within_building_block = 0; layer_idx_within_building_block < 3; layer_idx_within_building_block++)
		for (int layer_idx_within_building_block = 0; layer_idx_within_building_block < building_block_content[building_block_idx]; layer_idx_within_building_block++)
		{
			int layer_idx = layer_idx_base + layer_idx_within_building_block;

			printf("conv layer %d\n", layer_idx);
			fflush(stdout);

			if (layer_idx == 0)
			{
				num_elem_input_fm = (cfg[layer_idx].BN_R * cfg[layer_idx].BN_C * cfg[layer_idx].BN_I) * (BS_R * R + cfg[layer_idx].k1 - 1) * (BS_C * C + cfg[layer_idx].k2 - 1) * BS_I * I;

				memcpy(ofmap_buf4, ifmap, num_elem_input_fm * sizeof(data_bitwidth));
				conv2d(cfg, layer_idx, ofmap_buf4, weights, ofmap_buf1, wt_offset);

				if (building_block_idx == (BUILDING_BLOCK_NUM - 1))
				{
					// done
					num_elem_output_fm = cfg[layer_idx].BN_R * cfg[layer_idx].BN_O * R * BS_R * C * BS_C * O * BS_O;

					memcpy(ofmap, ofmap_buf1, num_elem_output_fm * sizeof(data_bitwidth));

				//	for (int i = 0; i < 10; i++)
				//	{
				//		printf("%d\n", ofmap_buf1[i]);
				//	}
				}
				else
				{
					num_elem_input_fm = (cfg[layer_idx + 1].BN_R * cfg[layer_idx + 1].BN_C * cfg[layer_idx + 1].BN_I) * (BS_R * R + cfg[layer_idx + 1].k1 - 1) * (BS_C * C + cfg[layer_idx + 1].k2 - 1) * BS_I * I;

					memcpy(ofmap_buf2, ofmap_buf1, num_elem_input_fm * sizeof(data_bitwidth));
					memcpy(ofmap_buf3, ofmap_buf1, num_elem_input_fm * sizeof(data_bitwidth));
				}
			}
			else
			{
				if ((layer_idx_within_building_block & 1) == 0)
				{
					conv2d(cfg, layer_idx, ofmap_buf1, weights, ofmap_buf2, wt_offset);
				}
				else
				{
					conv2d(cfg, layer_idx, ofmap_buf2, weights, ofmap_buf1, wt_offset);
				}

				if (layer_idx_within_building_block == 3)
				{
					// {conv2d and resadd} for the block containing 4 conv layers
					if (building_block_idx == (BUILDING_BLOCK_NUM - 1))
					{
						num_elem_output_fm = cfg[layer_idx].BN_R * cfg[layer_idx].BN_O * R * BS_R * C * BS_C * O * BS_O;

						conv2d(cfg, layer_idx, ofmap_buf3, weights, ofmap_buf4, wt_offset);
						resadd(ofmap_buf1, ofmap_buf4, ofmap_buf2, num_elem_output_fm);

						memcpy(ofmap, ofmap_buf2, num_elem_output_fm * sizeof(data_bitwidth));
					}
					else
					{
						num_elem_input_fm = (cfg[layer_idx + 1].BN_R * cfg[layer_idx + 1].BN_C * cfg[layer_idx + 1].BN_I) * (BS_R * R + cfg[layer_idx + 1].k1 - 1) * (BS_C * C + cfg[layer_idx + 1].k2 - 1) * BS_I * I;

						conv2d(cfg, layer_idx, ofmap_buf3, weights, ofmap_buf4, wt_offset);
						resadd(ofmap_buf1, ofmap_buf4, ofmap_buf2, num_elem_input_fm);

						memcpy(ofmap_buf1, ofmap_buf2, num_elem_input_fm * sizeof(data_bitwidth));
					}
				}
				else
				{
					// {resadd} only
					if (building_block_idx == (BUILDING_BLOCK_NUM - 1))
					{
						num_elem_output_fm = cfg[layer_idx].BN_R * cfg[layer_idx].BN_O * R * BS_R * C * BS_C * O * BS_O;

						resadd(ofmap_buf1, ofmap_buf3, ofmap_buf2, num_elem_output_fm);
						memcpy(ofmap, ofmap_buf2, num_elem_output_fm * sizeof(data_bitwidth));
					}
					else
					{
						num_elem_input_fm = (cfg[layer_idx + 1].BN_R * cfg[layer_idx + 1].BN_C * cfg[layer_idx + 1].BN_I) * (BS_R * R + cfg[layer_idx + 1].k1 - 1) * (BS_C * C + cfg[layer_idx + 1].k2 - 1) * BS_I * I;

						resadd(ofmap_buf1, ofmap_buf3, ofmap_buf2, num_elem_input_fm);
						memcpy(ofmap_buf1, ofmap_buf2, num_elem_input_fm * sizeof(data_bitwidth));
					}
				}
			}

			num_elem_weight = cfg[layer_idx].BN_O * cfg[layer_idx].BN_I * O * BS_O * cfg_k1[layer_idx] * cfg_k2[layer_idx] * BS_I * I;

			wt_offset += num_elem_weight;
		}

#if 0

		if (4 == building_block_content[building_block_idx])
		{
		//	int layer_idx = layer_idx_base + 3;

			if (building_block_idx == (BUILDING_BLOCK_NUM - 1))
			{
				num_elem_output_fm = cfg[layer_idx].BN_R * cfg[layer_idx].BN_O * R * BS_R * C * BS_C * O * BS_O;

				conv2d(ofmap_buf3, weights, ofmap_buf4, cfg[layer_idx], wt_offset);
				resadd(ofmap_buf1, ofmap_buf4, ofmap_buf2, num_elem_output_fm);

				memcpy(ofmap, ofmap_buf2, num_elem_output_fm * sizeof(data_bitwidth));
			}
			else
			{
				num_elem_input_fm = (cfg[layer_idx + 1].BN_R * cfg[layer_idx + 1].BN_C * cfg[layer_idx + 1].BN_I) * (BS_R * R + cfg[layer_idx + 1].k1 - 1) * (BS_C * C + cfg[layer_idx + 1].k2 - 1) * BS_I * I;

				conv2d(ofmap_buf3, weights, ofmap_buf4, cfg[layer_idx], wt_offset);
				resadd(ofmap_buf1, ofmap_buf4, ofmap_buf2, num_elem_input_fm);

				memcpy(ofmap_buf1, ofmap_buf2, num_elem_input_fm * sizeof(data_bitwidth));
			}
		}
		else
		{
		//	int layer_idx = layer_idx_base + 2;

			if (building_block_idx == (BUILDING_BLOCK_NUM - 1))
			{
				num_elem_output_fm = cfg[layer_idx].BN_R * cfg[layer_idx].BN_O * R * BS_R * C * BS_C * O * BS_O;

				resadd(ofmap_buf1, ofmap_buf3, ofmap_buf2, num_elem_output_fm);
				memcpy(ofmap, ofmap_buf2, num_elem_output_fm * sizeof(data_bitwidth));
			}
			else
			{
				num_elem_input_fm = (cfg[layer_idx + 1].BN_R * cfg[layer_idx + 1].BN_C * cfg[layer_idx + 1].BN_I) * (BS_R * R + cfg[layer_idx + 1].k1 - 1) * (BS_C * C + cfg[layer_idx + 1].k2 - 1) * BS_I * I;

				resadd(ofmap_buf1, ofmap_buf3, ofmap_buf2, num_elem_input_fm);

				memcpy(ofmap_buf1, ofmap_buf2, num_elem_input_fm * sizeof(data_bitwidth));
			}
		}
#endif

	//	memcpy(ofmap_buf1, ofmap_buf2, num_elem_output_fm * sizeof(data_bitwidth));


		layer_idx_base += building_block_content[building_block_idx];
	}
//#endif

}
