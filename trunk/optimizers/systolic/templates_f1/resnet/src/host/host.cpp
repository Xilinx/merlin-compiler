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


#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifndef VIVADO_HLS
#include <CL/opencl.h>
#endif

#include <iostream>


#ifndef VIVADO_HLS
#include "opencl_if.h"
#else
#include "cnn_top.h"
#endif

#include "cnn.h"
#include "c_model.h"
#include "util.h"

#include "config.h"
#include "type_def.h"

#include "systolic_params.h"

#ifndef VIVADO_HLS
char *xclbin_filename;
#endif

////////////////////////////////////////////////////////////////////////////////

//data_bitwidth ifmap[BN_R * BN_I * (R * BS_R + K1 - 1) * (C * BS_C + K2 - 1) * BS_I * I];
//data_bitwidth weight[BN_O * BN_I * O * BS_O * K1 * K2 * BS_I * I];
//data_bitwidth ofmap_opencl[BN_R * BN_O * R * BS_R * C * BS_C * O * BS_O];
//
//data_bitwidth ofmap_sw[BN_R * BN_O * R * BS_R * C * BS_C * O * BS_O];
//data_bitwidth ofmap_c[BN_R * BN_O * R * BS_R * C * BS_C * O * BS_O];

data_bitwidth *ifmap;
data_bitwidth *ifmap_bw;
data_bitwidth *weights;
data_bitwidth *weights_bw;

data_bitwidth *ofmap_opencl_bw;

data_bitwidth *ofmap_golden_bw;
data_bitwidth *ofmap_c_bw;

#ifdef VIVADO_HLS
//dram_trans_t *ifmap_hls;
//dram_trans_t *weights_hls;
//dram_trans_t *ofmap_hls;
data_bitwidth *ofmap_hls_bw;
#endif


OpCFG *cfg;

void config_op()
{
	int num_conv = get_num_conv();

	printf("num_conv = %d\n", num_conv);

	cfg = (OpCFG *)malloc(num_conv * sizeof(OpCFG));

	for (int i = 0; i < num_conv; i++)
	{
		printf("layer_idx=%d\n", i);

		cfg[i].BN_R = (cfg_img_row[i] + (BS_R * R) - 1) / (BS_R * R);
		cfg[i].BN_C = (cfg_img_col[i] + (BS_C * C) - 1) / (BS_C * C);
		cfg[i].BN_O = (cfg_out_num[i] + (BS_O * O) - 1) / (BS_O * O);
		cfg[i].BN_I = (cfg_in_num[i] + (BS_I * I) - 1) / (BS_I * I);
		cfg[i].k1 = cfg_k1[i];
		cfg[i].k2 = cfg_k2[i];
		cfg[i].conv_stride = cfg_conv_stride[i];
		cfg[i].pooling_stride = cfg_pooling_stride[i];

		printf("BN_R=%d, BN_C=%d, BN_O=%d, BN_I=%d\n", cfg[i].BN_R, cfg[i].BN_C, cfg[i].BN_O, cfg[i].BN_I);

		fflush(stdout);
	}
}

void block_wise_reformat_ifmap(int layer, data_bitwidth *ifmap, data_bitwidth *ifmap_bw)
{

	for (int r0 = 0; r0 < cfg[layer].BN_R; r0++)
	for (int c0 = 0; c0 < cfg[layer].BN_C; c0++)
    for (int i0 = 0; i0 < cfg[layer].BN_I; i0++) {
        for (int p = 0; p < cfg[layer].k1; p++)
        for (int q = 0; q < cfg[layer].k2; q++)
        for (int i1 = 0; i1 < BS_I; i1++)
		for (int r1 = 0; r1 < BS_R; r1++)
		for (int c1 = 0; c1 < BS_C; c1++) {
				for (int r2 = 0; r2 < R; r2++)
				for (int c2 = 0; c2 < C; c2++)
				for (int i2 = 0; i2 < I; i2++) {

					int r = r0 * BS_R * R + r1 * R + r2;
					int c = c0 * BS_C * C + c1 * C + c2;
					int i = (i0 * BS_I + i1) * I + i2;

					// in1[r0][c0][i0][r1*R+r2+q][c1*C+c2+p][i1[i2]]=in0[r0*BS_R*R+r1*R+r2+p][c0*BS_C*C+c1*C+c2+q][i0][i1][i2]
					int ifmap_idx = ((r + p) * (cfg_img_col[layer] + cfg_k2[layer] - 1) + (c + q)) * cfg_in_num[layer] + i;
					int ifmap_idx_bw = (((((r0 * cfg[layer].BN_C + c0) * cfg[layer].BN_I + i0) * (R * BS_R + cfg[layer].k1 - 1) + (r1 * R + r2 + p)) * (BS_C * C + cfg[layer].k2 - 1) + (c1 * C + c2 + q)) * BS_I + i1) * I + i2;

				//	printf("I am here@%s:%d\n", __func__, __LINE__);
				//	fflush(stdout);

				//	printf("ifmap_dix=%d, ifmap_idx_bw=%d\n", ifmap_idx, ifmap_idx_bw);
				//	fflush(stdout);

					if ((r + p) < (cfg_img_row[layer] + cfg_k1[layer] - 1) && (c + q) < (cfg_img_col[layer] + cfg_k2[layer] - 1) && i < cfg_in_num[layer])
					{
						ifmap_bw[ifmap_idx_bw] = ifmap[ifmap_idx];
					}
					else
					{
						// maginal zero padding
						ifmap_bw[ifmap_idx_bw] = 0;
					}

	}}}
}

void block_wise_reformat_weight(int layer, data_bitwidth *wt, data_bitwidth *wt_bw)
{
    for (int o0 = 0; o0 < cfg[layer].BN_O; o0++)
    for (int i0 = 0; i0 < cfg[layer].BN_I; i0++) {
        for (int p = 0; p < cfg[layer].k1; p++)
        for (int q = 0; q < cfg[layer].k2; q++)
        for (int i1 = 0; i1 < BS_I; i1++)
        for (int o1 = 0; o1 < BS_O; o1++) {
        for (int o2 = 0; o2 < O; o2++)
        for (int i2 = 0; i2 < I; i2++) {

			int o = o0 * BS_O * O + o1 * O + o2;
			int i = i0 * BS_I * I + i1 * I + i2;

			int wt_idx = ((o * cfg[layer].k1 + p) * cfg[layer].k2 + q) * cfg_in_num[layer] + i;
			int wt_idx_bw = (o0 * cfg[layer].BN_I + i0) * BS_O * O * cfg[layer].k1 * cfg[layer].k2 * BS_I * I + (((o1 * O + o2) * cfg[layer].k1 + p) * cfg[layer].k2 + q) * BS_I * I + i1 * I + i2;

			// w1[o0][i0][o1][o2][p][q][i1][i2] = w0[o0][o1][o2][p][q][i0][i1][i2]
			if (o < cfg_out_num[layer] && i < cfg_in_num[layer])
			{
				wt_bw[wt_idx_bw] = wt[wt_idx];
			}
			else
			{
				wt_bw[wt_idx_bw] = 0;
			}
	}}}
}

////////////////////////////////////////
// Data preparation for loader ifmap //
////////////////////////////////////////
void load_ifmap_prepare()
{
	int	num_elem_input_fm = (cfg_img_row[0] + cfg_k1[0] - 1) * (cfg_img_col[0] + cfg_k2[0] - 1) * cfg_in_num[0];

	if ((ifmap = (data_bitwidth *)malloc(num_elem_input_fm * sizeof(data_bitwidth))) == NULL)
	{
		perror("Failed malloc of ifmap");
	}


//	randomize_array(ifmap, num_elem_input_fm);
	one_array(ifmap, num_elem_input_fm);

	int	num_elem_input_fm_bw = cfg[0].BN_R * cfg[0].BN_C * cfg[0].BN_I * (R * BS_R + cfg_k1[0] - 1) * (C * BS_C + cfg_k2[0] - 1) * BS_I * I;

	if ((ifmap_bw = (data_bitwidth *)malloc(num_elem_input_fm_bw * sizeof(data_bitwidth))) == NULL) 
	{
		perror("Failed malloc of ifmap_bw");
	}

	printf("Block-wise reformatting of input feature maps!\n");
	block_wise_reformat_ifmap(0, ifmap, ifmap_bw);


#if 0
#ifdef VIVADO_HLS
	if ((ifmap_hls = (dram_trans_t *)malloc(num_elem_input_fm * sizeof(data_bitwidth))) == NULL) 
	{
		perror("Failed malloc of ifmap_hls");
	}

	// TODO: prepare data for ifmap_hls
#endif
#endif
}

////////////////////////////////////////
// Data preparation for loader weights //
////////////////////////////////////////
void load_wt_prepare()
{
	int num_elem_weights = 0;
	int num_elem_weights_bw = 0;

	int num_conv = get_num_conv();

	for (int i = 0; i < num_conv; i++) 
	{
		int num_elem_weight = cfg_out_num[i] * cfg_k1[i] * cfg_k2[i] * cfg_in_num[i];
		int num_elem_weight_bw = cfg[i].BN_O * cfg[i].BN_I * O * BS_O * cfg_k1[i] * cfg_k2[i] * BS_I * I;

		num_elem_weights += num_elem_weight;
		num_elem_weights_bw += num_elem_weight_bw;
	}

	if ((weights = (data_bitwidth *)malloc(num_elem_weights * sizeof(data_bitwidth))) == NULL)
	{
		perror("Failed malloc of weights");
	}
	if ((weights_bw = (data_bitwidth *)malloc(num_elem_weights_bw * sizeof(data_bitwidth))) == NULL) {
		perror("Failed malloc of weights_bw");
	}

#if 0
#ifdef VIVADO_HLS
	if ((weights_hls = (dram_trans_t *)malloc(num_elem_weights * sizeof(data_bitwidth))) == NULL)
	{
		perror("Failed malloc of weights_hls");
	}
#endif
#endif

	data_bitwidth *weight = weights;
	data_bitwidth *weight_bw = weights_bw;

	for (int i = 0; i < num_conv; i++)
	{
		int num_elem_weight = cfg_out_num[i] * cfg_k1[i] * cfg_k2[i] * cfg_in_num[i];
		int num_elem_weight_bw = cfg[i].BN_O * cfg[i].BN_I * O * BS_O * cfg_k1[i] * cfg_k2[i] * BS_I * I;

	//	randomize_array(weight, num_elem_weight);
		one_array(weight, num_elem_weight);

		printf("Block-wise reformatting of weights[%d]!\n", i);
		block_wise_reformat_weight(i, weight, weight_bw);

		weight += num_elem_weight;
		weight_bw += num_elem_weight_bw;
	}
}

////////////////////////////////////////
// Data preparation for drain ofmap //
////////////////////////////////////////
void drain_ofmap_prepare()
{
	int num_conv = get_num_conv();

	int	num_elem_output_fm = cfg_out_num[num_conv - 1] * cfg_img_row[num_conv - 1] * cfg_img_col[num_conv - 1];
	int	num_elem_output_fm_bw = cfg[num_conv - 1].BN_R * cfg[num_conv - 1].BN_C * cfg[num_conv - 1].BN_O * R * BS_R * C * BS_C * O * BS_O;

	printf("%s: num_elem_output_fm=%d\n", __FUNCTION__, num_elem_output_fm);
	printf("%s: num_elem_output_fm_bw=%d\n", __FUNCTION__, num_elem_output_fm_bw);

	if ((ofmap_golden_bw = (data_bitwidth *)malloc(num_elem_output_fm_bw * sizeof(data_bitwidth))) == NULL)
	{
		perror("Failed malloc of ofmap_golden_bw");
	}
//	memset(output_fm_block_wise, 0, num_elem_output_fm*sizeof(float));

	if ((ofmap_c_bw = (data_bitwidth *)malloc(num_elem_output_fm_bw * sizeof(data_bitwidth))) == NULL) 
	{
		perror("Failed malloc of ofmap_c_bw");
	}
//	memset(output_fm, 0, num_elem_output_fm*sizeof(float));

//	if((ofmap_opencl = (data_bitwidth *)malloc(num_elem_output_fm * sizeof(data_bitwidth))) == NULL)
//	{
//		perror("Failed malloc of ofmap_opencl");
//	}
	ofmap_opencl_bw = (data_bitwidth *)malloc(num_elem_output_fm_bw * sizeof(data_bitwidth));
	if (ofmap_opencl_bw == NULL)
	{
		perror("Failed malloc of ofmap_opencl_bw");
	}

#ifdef VIVADO_HLS
	if((ofmap_hls_bw = (data_bitwidth *)malloc(num_elem_output_fm_bw * sizeof(data_bitwidth))) == NULL)
	{
		perror("Failed malloc of ofmap_hls_bw");
	}
#endif
//	memset(output_fm_opencl, 0, num_elem_output_fm*sizeof(float));

}

void run_model(void (*cnn)(data_bitwidth *, data_bitwidth *, data_bitwidth *,
			int, int,
			OpCFG *),
		data_bitwidth *ifmap,
		data_bitwidth *weights,
		data_bitwidth *ofmap, 
		int start_block_idx,
		int end_block_idx,
		OpCFG *cfg)
{
//	int	num_elem_output_fm = cfg_BN_R[NUM_CONV - 1] * cfg_BN_O[NUM_CONV - 1] * R * cfg_BS_R[NUM_CONV - 1] * C * cfg_BS_C[NUM_CONV - 1] * O * cfg_BS_O[NUM_CONV - 1];

	cnn(ifmap, weights, ofmap,
			start_block_idx, end_block_idx,
			cfg);
}

#ifdef VIVADO_HLS
void run_hls_model(
	//	dram_trans_t *ifmap,
	//	dram_trans_t *weights,
	//	dram_trans_t *ofmap, 
		data_bitwidth *ifmap,
		data_bitwidth *weights,
		data_bitwidth *ofmap, 
		int start_block_idx,
		int end_block_idx,
		OpCFG *cfg)
{
	int DEBUG_msg[DEBUG_MSG_LEN];

//	cnn_top(ifmap, weights, ofmap);
	cnn_top((dram_trans_t *)ifmap,
			(dram_trans_t *)weights,
			(dram_trans_t *)ofmap, 
			start_block_idx,
			end_block_idx,
			cfg,
			DEBUG_msg);
//	cnn_top((vec_input_t *)ifmap, (vec_input_t *)weights, (vec_input_t *)weights, (vec_input_t *)ofmap);
//	cnn_top(ifmap, weights, ofmap);
}
#endif

int model_verify(data_bitwidth *ofmap_golden, data_bitwidth *ofmap_test)
{
	int err_no = 0;

	int num_conv = get_num_conv();

	int	num_elem_output_fm = cfg[num_conv - 1].BN_R * cfg[num_conv - 1].BN_C * cfg[num_conv - 1].BN_O * R * BS_R * C * BS_C * O * BS_O;

	for (int i = 0 ; i < num_elem_output_fm; i++)
	{
#if (DATA_BITWIDTH == 32)
		if (fabs(ofmap_test[i] - ofmap_golden[i]) > 1e-3)
#else
			if (ofmap_test[i] != ofmap_golden[i])
#endif
			{
				err_no++;
				if (err_no == 10)
				{
					printf("...\n");
					break;
				}
				else
				{
				//	std::cout << "Error: Result mismatch" << std::endl;
#if (DATA_BITWIDTH == 32)
				printf("ofmap_golden[%d]=%.3f. ofmap_test[%d]=%.3f\n", i, ofmap_golden[i], i, ofmap_test[i]);
#else
				printf("ofmap_golden[%d]=%d. ofmap_test[%d]=%d\n", i, ofmap_golden[i], i, ofmap_test[i]);
#endif
				}
			}
	}

	return err_no;
}

void cleanup()
{
	free(ifmap);
	free(ifmap_bw);

	free(weights);
	free(weights_bw);

	free(ofmap_golden_bw);
	free(ofmap_c_bw);
	free(ofmap_opencl_bw);

#ifdef VIVADO_HLS
	free(ofmap_hls_bw);
#endif

	free(cfg);
}

int main(int argc, char **argv)
{
//	int ifmap_length = BN_R * BN_I * (R * BS_R + K1 - 1) * (C * BS_C + K2 - 1) * BS_I * I;
//	int wt_length = BN_O * BN_I * O * BS_O * K1 * K2 * BS_I * I;
//	int ofmap_length = BN_R * BN_O * R * BS_R * C * BS_C * O * BS_O;

#ifndef VIVADO_HLS
	if (argc != 2)
	{
		printf("%s <inputfile>\n", argv[0]);
		return EXIT_FAILURE;
	}

	xclbin_filename = argv[1];
#endif

	int err_no;

	int start_block_idx = 0;
	int end_block_idx = BUILDING_BLOCK_NUM - 1;

	// config operators
	config_op();

	// Fill our data sets with pattern
	//
	load_ifmap_prepare();

	load_wt_prepare();

	drain_ofmap_prepare();

	run_model(cnn_golden,
			ifmap_bw, weights_bw, ofmap_golden_bw,
			start_block_idx, end_block_idx,
			cfg);

//	run_model(c_model, ifmap, weights, ofmap_c, cfg);
//	match = model_verify(ofmap_golden, ofmap_c);
//	std::cout << "C Model TEST: " << (match ? "PASSED" : "FAILED") << std::endl; 

#ifdef VIVADO_HLS
	run_hls_model(ifmap_bw, weights_bw, ofmap_hls_bw,
			start_block_idx, end_block_idx,
			cfg);
	err_no = model_verify(ofmap_golden_bw, ofmap_hls_bw);
	std::cout << "HLS Model TEST: " << (err_no ? "FAILED" : "PASSED") << std::endl; 
#else
	run_model(opencl_model,
			ifmap_bw, weights_bw, ofmap_opencl_bw,
			start_block_idx, end_block_idx,
			cfg);
	err_no = model_verify(ofmap_golden_bw, ofmap_opencl_bw);
	std::cout << "OpenCL Model TEST: " << (err_no ? "FAILED" : "PASSED") << std::endl; 
#endif

	cleanup();

	return 0;
}
