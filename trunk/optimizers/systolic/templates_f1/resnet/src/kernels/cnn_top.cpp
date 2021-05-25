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



#include <ap_int.h>
#include <string.h>

#include "config.h"

#include "cnn_top.h"

#include "systolic_params.h"
#include "type_def.h"

#define TEST_TENSOR_TYPE 1

#include "./conv2d/conv2d.cpp"
//#include "conv2d_systolic.cpp"


#if (TEST_TENSOR_TYPE == 1)
void _memcpy(dram_trans_t *dst, dram_trans_t *src, int size)
#else
//void _memcpy(data_bitwidth *dst, data_bitwidth *src, int size)
void _memcpy(data_bitwidth dst[][I], data_bitwidth src[][I], int size)
#endif
{
	for (int i = 0; i < (size / I); i++)
	{
#pragma HLS PIPELINE
		for (int i2 = 0; i2 < I; i2++)
		{
#pragma HLS UNROLL
#if (TEST_TENSOR_TYPE == 1)
			dst[i](i2 * DATA_BITWIDTH, i2 * DATA_BITWIDTH + DATA_BITWIDTH - 1) = src[i](i2 * DATA_BITWIDTH, i2 * DATA_BITWIDTH + DATA_BITWIDTH - 1);
#else
			dst[i][i2] = src[i][i2];
#endif
		}
	}
}

#if (TEST_TENSOR_TYPE == 1)
void _memcpy1(dram_trans_t *dst, dram_trans_t *src, int size)
#else
void _memcpy1(data_bitwidth dst[][I], dram_trans_t *src, int size)
#endif
{
#if (TEST_TENSOR_TYPE == 1)
	for (int i = 0; i < (size / NUM_PACKED_DATA); i++)
	{
		dst[i] = src[i];
	}
#else
//	dram_trans_t temp;

//	for (int i = 0; i < (size / NUM_PACKED_DATA); i++)
//	{
//		temp = src[i];
//
//		for (int j = 0; j < (NUM_PACKED_DATA / IN_PACK_SIZE); j++)
//		{
//#pragma HLS PIPELINE
//			for (int i2 = 0; i2 < IN_PACK_SIZE; i2++)
//			{
//#pragma HLS UNROLL
//				dst[i * (NUM_PACKED_DATA / IN_PACK_SIZE) + j][i2] = temp.range((j * IN_PACK_SIZE + i2) * DATA_BITWIDTH + DATA_BITWIDTH - 1, (j * IN_PACK_SIZE + i2) * DATA_BITWIDTH);
//			//	dst[i * NUM_PACKED_DATA + j][i2] = temp((j * IN_PACK_SIZE + i2) * DATA_BITWIDTH + DATA_BITWIDTH - 1, (j * IN_PACK_SIZE + i2) * DATA_BITWIDTH);
//			}
//		}

//		dst[i] = src[i];
//	}
#endif
}

#if (TEST_TENSOR_TYPE == 1)
void _memcpy2(dram_trans_t *dst, dram_trans_t *src, int size)
#else
void _memcpy2(dram_trans_t *dst, data_bitwidth src[][I], int size)
#endif
{
#if (TEST_TENSOR_TYPE == 1)
	for (int i = 0; i < (size / NUM_PACKED_DATA); i++)
	{
		dst[i] = src[i];
	}
#else
//	dram_trans_t temp;

//	for (int i = 0; i < (size / NUM_PACKED_DATA); i++)
//	{
//		for (int j = 0; j < (NUM_PACKED_DATA / IN_PACK_SIZE); j++)
//		{
//#pragma HLS PIPELINE
//			for (int i2 = 0; i2 < IN_PACK_SIZE; i2++)
//			{
//#pragma HLS UNROLL
//			//	temp((j * IN_PACK_SIZE + i2) * DATA_BITWIDTH + DATA_BITWIDTH - 1, (j * IN_PACK_SIZE + i2) * DATA_BITWIDTH) = src[i * (NUM_PACKED_DATA / IN_PACK_SIZE) + j][i2];
//				temp.range((j * IN_PACK_SIZE + i2) * DATA_BITWIDTH + DATA_BITWIDTH - 1, (j * IN_PACK_SIZE + i2) * DATA_BITWIDTH) = src[i * (NUM_PACKED_DATA / IN_PACK_SIZE) + j][i2];
//			}
//		}
//
//		dst[i] = temp;
//	}
#endif
}

//void _memcpy2(data_bitwidth *dst, data_bitwidth *src, int size)
//{
//#pragma HLS inline off
//	for (int i = 0; i < size; i++)
//	{
//		dst[i] = src[i];
//	}
//}

#if (TEST_TENSOR_TYPE == 1)
static void resadd(dram_trans_t *ofmap_buf1, 
		dram_trans_t *ofmap_buf2,
		dram_trans_t *ofmap_buf3,
		int size)
#else
static void resadd(data_bitwidth ofmap_buf1[][I], 
		data_bitwidth ofmap_buf2[][I],
		data_bitwidth ofmap_buf3[][I],
		int size)
#endif
{
	for (int i = 0; i < (size / I); i++)
	{
		for (int i2 = 0; i2 < I; i2++)
		{
#if (TEST_TENSOR_TYPE == 1)
			ofmap_buf3[i](i2 * DATA_BITWIDTH, i2 * DATA_BITWIDTH + DATA_BITWIDTH - 1) = ofmap_buf1[i](i2 * DATA_BITWIDTH, i2 * DATA_BITWIDTH + DATA_BITWIDTH - 1) + ofmap_buf2[i](i2 * DATA_BITWIDTH, i2 * DATA_BITWIDTH + DATA_BITWIDTH - 1);
#else
			ofmap_buf3[i][i2] = ofmap_buf1[i][i2] + ofmap_buf2[i][i2];
#endif
		}
	}
}

void cnn_top(dram_trans_t *ifmap,
		dram_trans_t *weights,
		dram_trans_t *ofmap,
		int start_block_idx,
		int end_block_idx,
		OpCFG *cfg, 
	//	int *DEBUG_msg)
		int *debug_msg)
{
#pragma HLS INTERFACE m_axi port=ifmap offset=slave bundle=ifmap
#pragma HLS INTERFACE m_axi port=weights offset=slave bundle=weights
#pragma HLS INTERFACE m_axi port=ofmap offset=slave bundle=ofmap
#pragma HLS INTERFACE m_axi port=cfg offset=slave bundle=cfg
//#pragma HLS INTERFACE m_axi port=DEBUG_msg offset=slave bundle=DEBUG_msg
#pragma HLS INTERFACE m_axi port=debug_msg offset=slave bundle=debug_msg
#pragma HLS INTERFACE s_axilite port=ifmap bundle=control
#pragma HLS INTERFACE s_axilite port=weights bundle=control
#pragma HLS INTERFACE s_axilite port=ofmap bundle=control
#pragma HLS INTERFACE s_axilite port=start_block_idx bundle=control
#pragma HLS INTERFACE s_axilite port=end_block_idx bundle=control
#pragma HLS INTERFACE s_axilite port=cfg bundle=control
//#pragma HLS INTERFACE s_axilite port=DEBUG_msg bundle=control
#pragma HLS INTERFACE s_axilite port=debug_msg bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

//#pragma HLS DATA_PACK variable=ifmap
//#pragma HLS DATA_PACK variable=weights
//#pragma HLS DATA_PACK variable=ofmap
#pragma HLS DATA_PACK variable=cfg

//#pragma HLS ALLOCATION instances=conv limit=1 function

#if (TEST_TENSOR_TYPE == 1)
	/*
	 * FIXME: II cannot be 1 if not manually 
	 * dividing buffers into 2 dimensions explicitly.
	 * */
	dram_trans_t ofmap_buf1[MAX_ACTIVATION_BUF_LEN / IN_PACK_SIZE];
	dram_trans_t ofmap_buf2[MAX_ACTIVATION_BUF_LEN / IN_PACK_SIZE];
	dram_trans_t ofmap_buf3[MAX_ACTIVATION_BUF_LEN / IN_PACK_SIZE]; 
	dram_trans_t ofmap_buf4[MAX_ACTIVATION_BUF_LEN / IN_PACK_SIZE]; 
#else
	data_bitwidth ofmap_buf1[MAX_ACTIVATION_BUF_LEN / I][I]; 
	data_bitwidth ofmap_buf2[MAX_ACTIVATION_BUF_LEN / I][I]; 
	data_bitwidth ofmap_buf3[MAX_ACTIVATION_BUF_LEN / I][I]; 
	data_bitwidth ofmap_buf4[MAX_ACTIVATION_BUF_LEN / I][I]; 
#endif

#pragma HLS RESOURCE variable=ofmap_buf1 core=XPM_MEMORY uram
#pragma HLS RESOURCE variable=ofmap_buf2 core=XPM_MEMORY uram
#pragma HLS RESOURCE variable=ofmap_buf3 core=XPM_MEMORY uram
#pragma HLS RESOURCE variable=ofmap_buf4 core=XPM_MEMORY uram

#if (TEST_TENSOR_TYPE == 0)
#pragma HLS ARRAY_PARTITION variable=ofmap_buf1 complete dim=2
#pragma HLS ARRAY_PARTITION variable=ofmap_buf2 complete dim=2
#pragma HLS ARRAY_PARTITION variable=ofmap_buf3 complete dim=2
#pragma HLS ARRAY_PARTITION variable=ofmap_buf4 complete dim=2
#endif

	int	num_elem_input_fm;
	int num_elem_weight;
	int	num_elem_output_fm;

	int wt_offset = 0;


	int layer_idx_base = 0;
//	int last_layer_idx = 0;
//	int building_block_idx;
	int layer_idx_within_building_block;

//	DEBUG_msg[0] = start_block_idx;
//	DEBUG_msg[1] = end_block_idx;
	debug_msg[0] = start_block_idx;
	debug_msg[1] = end_block_idx;

	for (int building_block_idx = 0; building_block_idx < start_block_idx; building_block_idx++)
	{
		layer_idx_base += building_block_content[building_block_idx];
	}

	for (int building_block_idx = start_block_idx; building_block_idx <= end_block_idx; building_block_idx++)
	{
		for (int layer_idx_within_building_block = 0; layer_idx_within_building_block < building_block_content[building_block_idx]; layer_idx_within_building_block++)
		{
			int layer_idx = layer_idx_base + layer_idx_within_building_block;

			if (layer_idx == 0)
			{
				num_elem_input_fm = (cfg[layer_idx].BN_R * cfg[layer_idx].BN_C * cfg[layer_idx].BN_I) * (BS_R * R + cfg[layer_idx].k1 - 1) * (BS_C * C + cfg[layer_idx].k2 - 1) * BS_I * I;

				_memcpy1(ofmap_buf4, ifmap, num_elem_input_fm);
				conv2d(cfg, layer_idx, ofmap_buf4, weights, ofmap_buf1, wt_offset);

				if (building_block_idx == (BUILDING_BLOCK_NUM - 1))
				{
					// done
					num_elem_output_fm = cfg[layer_idx].BN_R * cfg[layer_idx].BN_O * R * BS_R * C * BS_C * O * BS_O;

					_memcpy2(ofmap, ofmap_buf1, num_elem_output_fm);
				}
//#if 0
				else
				{
					num_elem_input_fm = (cfg[layer_idx + 1].BN_R * cfg[layer_idx + 1].BN_C * cfg[layer_idx + 1].BN_I) * (BS_R * R + cfg[layer_idx + 1].k1 - 1) * (BS_C * C + cfg[layer_idx + 1].k2 - 1) * BS_I * I;

					_memcpy(ofmap_buf2, ofmap_buf1, num_elem_input_fm);
					_memcpy(ofmap_buf3, ofmap_buf1, num_elem_input_fm);
				}
//#endif
			}
//#if 0
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

						_memcpy2(ofmap, ofmap_buf2, num_elem_output_fm);
					}
					else
					{
						num_elem_input_fm = (cfg[layer_idx + 1].BN_R * cfg[layer_idx + 1].BN_C * cfg[layer_idx + 1].BN_I) * (BS_R * R + cfg[layer_idx + 1].k1 - 1) * (BS_C * C + cfg[layer_idx + 1].k2 - 1) * BS_I * I;

						conv2d(cfg, layer_idx, ofmap_buf3, weights, ofmap_buf4, wt_offset);
						resadd(ofmap_buf1, ofmap_buf4, ofmap_buf2, num_elem_input_fm);

						_memcpy(ofmap_buf1, ofmap_buf2, num_elem_input_fm);
					}
				}
				else
				{
					// {resadd} only
					if (building_block_idx == (BUILDING_BLOCK_NUM - 1))
					{
						num_elem_output_fm = cfg[layer_idx].BN_R * cfg[layer_idx].BN_O * R * BS_R * C * BS_C * O * BS_O;

						resadd(ofmap_buf1, ofmap_buf3, ofmap_buf2, num_elem_output_fm);
						_memcpy2(ofmap, ofmap_buf2, num_elem_output_fm);
					}
					else
					{
						num_elem_input_fm = (cfg[layer_idx + 1].BN_R * cfg[layer_idx + 1].BN_C * cfg[layer_idx + 1].BN_I) * (BS_R * R + cfg[layer_idx + 1].k1 - 1) * (BS_C * C + cfg[layer_idx + 1].k2 - 1) * BS_I * I;

						resadd(ofmap_buf1, ofmap_buf3, ofmap_buf2, num_elem_input_fm);
						_memcpy(ofmap_buf1, ofmap_buf2, num_elem_input_fm);
					}
				}
			}
//#endif

			num_elem_weight = cfg[layer_idx].BN_O * cfg[layer_idx].BN_I * O * BS_O * cfg_k1[layer_idx] * cfg_k2[layer_idx] * BS_I * I;

			wt_offset += num_elem_weight;
		}

		layer_idx_base += building_block_content[building_block_idx];
	}

}
