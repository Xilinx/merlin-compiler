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



#include "systolic_params.h"

#include "cnn.h"

#include "util.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define DEBUG_ZERO_PADDING 0
#define DEBUG_ZERO_PADDING_BW 0
#define WITHIN_BLOCK_IDX 2000

void block_wise_reformat_input(float *input, float *input_block_wise)
{
	int addr_in, addr_in_bw;

	int img_row = BN_R * BS_R * R;
	int img_col = BN_C * BS_C;
	int in_num = BN_I * BS_I * I;

	for (int r0 = 0; r0 < BN_R; r0++)
	for (int c0 = 0; c0 < BN_C; c0++)
    for (int i0 = 0; i0 < BN_I; i0++) {
        for (int p = 0; p < K; p++)
        for (int q = 0; q < K; q++)
        for (int i1 = 0; i1 < BS_I; i1++)
		for (int r1 = 0; r1 < BS_R; r1++)
		for (int c1 = 0; c1 < BS_C; c1++) {
				for (int r2 = 0; r2 < R; r2++)
				for (int i2 = 0; i2 < I; i2++) {

					int r = r0 * R * BS_R + r2 * BS_R + r1;
					int c = c0 * BS_C + c1;
					int i = (i0 * BS_I + i1) * I + i2;

					// in1[r0][c0][i0][r2*BS_R+r1+q][c1+p][i1[i2]]=in0[r0*R*BS_R+r2*BS_R+r1+p][c0*BS_C+c1+q][i0][i1][i2]
					addr_in = ((r + p) * (img_col + K - 1) + (c + q)) * in_num + i;
					addr_in_bw = (((((r0 * BN_C + c0) * BN_I + i0) * (R * BS_R + K - 1) + (r2 * BS_R + r1 + p)) * (BS_C + K - 1) + (c1 + q)) * BS_I + i1) * I + i2;

					if (r < img_row && c < img_col && i < in_num) {
						input_block_wise[addr_in_bw] = input[addr_in];
					} else {
						// maginal zero padding
						input_block_wise[addr_in_bw] = 0;
					}
	}}}
}

void block_wise_reformat_weight(float *weight, float *weight_block_wise)
{
	int addr_w, addr_w_bw;

	int out_num = BN_O * BS_O * O;
	int in_num = BN_I * BS_I * I;

    for (int o0 = 0; o0 < BN_O; o0++)
    for (int i0 = 0; i0 < BN_I; i0++) {
        for (int p = 0; p < K; p++)
        for (int q = 0; q < K; q++)
        for (int i1 = 0; i1 < BS_I; i1++)
        for (int o1 = 0; o1 < BS_O; o1++) {
        for (int o2 = 0; o2 < O; o2++)
        for (int i2 = 0; i2 < I; i2++) {

			int o = o0 * BS_O * O + o2 * BS_O + o1;
			int i = i0 * BS_I * I + i1 * I + i2;

			addr_w = ((o * K + p) * K + q) * in_num + i;
			addr_w_bw = (o0 * BN_I + i0) * BS_O * O * K * K * BS_I * I + (((o2 * BS_O + o1) * K + p) * K + q) * BS_I * I + i1 * I + i2;

			// w1[o0][i0][o2][o1][p][q][i1][i2] = w0[o0][o2][o1][p][q][i0][i1][i2]
			if (o < out_num && i < in_num) {
				weight_block_wise[addr_w_bw] = weight[addr_w];
			} else {
				weight_block_wise[addr_w_bw] = 0;
			}
	}}}
}

void block_wise_reformat_output(float *output_block_wise, float *output)
{
	int addr_out_bw, addr_out;

	int img_row = BN_R * BS_R * R;
	int img_col = BN_C * BS_C;
	int out_num = BN_O * BS_O * O;

		for (int r0 = 0; r0 < BN_R; r0++)
		for (int r2 = 0; r2 < R; r2++) 
		for (int r1 = 0; r1 < BS_R; r1++) {
			for (int c0 = 0; c0 < BN_C; c0++)
			for (int c1 = 0; c1 < BS_C; c1++) {
				for (int o0 = 0; o0 < BN_O; o0++)
				for (int o2 = 0; o2 < O; o2++)
				for (int o1 = 0; o1 < BS_O; o1++) {

					int r = (r0 * R + r2) * BS_R + r1;
					int c = c0 * BS_C + c1;
					int o = (o0 * O + o2) * BS_O + o1;
					// out0[r0][r2][r1][c0][c1][o0][o2][o1] = out2[r0][c0][o0][r2][r1][c1][o2][o1]
					addr_out_bw = ((r0 * BN_C + c0) * BN_O + o0) * R * BS_R * BS_C * O * BS_O + ((r2 * BS_R + r1) * BS_C + c1) * O * BS_O + o2 * BS_O + o1;
					addr_out = (r * img_col + c) * out_num + o;

					if (r < img_row && c < img_col && o < out_num) {
						output[addr_out] = output_block_wise[addr_out_bw];
					}
			}}}
}


static void conv(
	   float *out,
	   float *in,
	   float *weight
	   )
{
	int img_row = BN_R * BS_R * R;
	int img_col = BN_C * BS_C;
	int in_num = BN_I * BS_I * I;
	int out_num = BN_O * BS_O * O;

    // Initialize output
    // TODO: Initialize with bias
	for (int r = 0; r < img_row; r++) {
		for (int c = 0; c < img_col; c++) {
			for (int o = 0; o < out_num; o++) {
				// out[r][c][o]
				out[r * (img_col * out_num) + c * out_num + o] = 0;
			}
		}
	}
    
  for (int o = 0; o < out_num; o++)
  for (int i = 0; i < in_num; i++)
  for (int c = 0; c < img_col; c++)
  for (int r = 0; r < img_row; r++)
  for (int p = 0; p < K; p++)
  for (int q = 0; q < K; q++) {
	// out[r][c][o] = in[r+p][c+q][i] * weight[o][p][q][i]
		int out_idx = r * (img_col * out_num) + c * out_num + o;
		int in_idx = (r + p) * (img_col + K - 1) * in_num + (c + q) * in_num + i;
		int w_idx = o * (in_num * K * K) + p * (in_num * K) + q * in_num + i;

        out[out_idx] += in[in_idx] * weight[w_idx];

//		if (out_idx == 1) {
//			printf("out[%d]+=in[%d]*weights[%d]:%f+=%f*%f\n", out_idx, in_idx, w_idx, out[out_idx], in[in_idx], weight[w_idx]);
//		}
    }
}

void cnn(
		float* out,
		float* in,
		float* weight
	   )
{
	int img_row = BN_R * BS_R * R;
	int img_col = BN_C * BS_C;
	int in_num = BN_I * BS_I * I;
	int out_num = BN_O * BS_O * O;

	unsigned int num_elem_input_fm = (img_row + K - 1) * 
		(img_col + K - 1) * in_num;

	unsigned int num_elem_weight = out_num * K * K * in_num;
	unsigned int num_elem_output_fm = out_num * img_row * img_col;

	// convolutional layer
	conv(out, in, weight);
}

/*
 * Functions for specified
 * */
static void conv_specified(
		float* out,
		float* in,
		float* weight
	   )
{
	int img_row = BN_R * BS_R * R;
	int img_col = BN_C * BS_C;
	int in_num = BN_I * BS_I * I;
	int out_num = BN_O * BS_O * O;

	for (int r = 0; r < img_row; r++) {
		for (int c = 0; c < img_col; c++) {
			for (int o = 0; o < out_num; o++) {
				// out[r][c][o]
				out[r * (img_col * out_num) + c * out_num + o] = 0;
			}
		}
	}

	for (int r0 = 0; r0 < BN_R; r0++)
	for (int c0 = 0; c0 < BN_C; c0++)
	for (int o0 = 0; o0 < BN_O; o0++) 
	for (int i0 = 0; i0 < BN_I; i0++) {
		for (int p = 0; p < K; p++)
		for (int q = 0; q < K; q++)
		for (int i1 = 0; i1 < BS_I; i1++)
		for (int r1 = 0; r1 < BS_R; r1++)
		for (int c1 = 0; c1 < BS_C; c1++)
		for (int o1 = 0; o1 < BS_O; o1++) {
			for (int r2 = 0; r2 < R; r2++)
			for (int o2 = 0; o2 < O; o2++) 
			for (int i2 = 0; i2 < I; i2++) {

				int r = r0 * R * BS_R + r2 * BS_R + r1;
				int c = c0 * BS_C + c1;
				int o = o0 * O * BS_O + o2 * BS_O + o1;
				int i = i0 * BS_I * I + i1 * I + i2;

				int out_idx = (r * img_col + c) * out_num + o; // [r0][r2][r1][c0][c1][o0][o2][o1]
				int in_idx = ((r + p) * (img_col + K - 1) + c + q) * in_num + i; // [r0 * R * BS_R + r2 * R + r1 + p][c0 * BS_C + c1 + q][i0][i1][i2]
				int w_idx = ((o * K + p) * K + q) * in_num + i; // [o0][o2][p][q][i0][i1][i2]

				if (r < img_row && c < img_col && o < out_num && i < in_num) {
					out[out_idx] += in[in_idx] * weight[w_idx];
				//	if (out_idx == 0)
					//	printf("%f\n", out[out_idx]);
				}

//				if (out_idx == 80) {
//					printf("out[%d]+=in[%d]*weights[%d]:%f+=%f*%f\n", out_idx, in_idx, w_idx, out[out_idx], in[in_idx], weight[w_idx]);
//				}

			}}}
}

void cnn_specified(
		float* out,
		float* in,
		float* weight
		)
{
	int img_row = BN_R * BS_R * R;
	int img_col = BN_C * BS_C;
	int in_num = BN_I * BS_I * I;
	int out_num = BN_O * BS_O * O;

	unsigned int num_elem_weight = out_num * K * K * in_num;
	unsigned int num_elem_input_fm = (img_row + K - 1) * 
		(img_col + K - 1) * in_num;
	unsigned int num_elem_output_fm = out_num * img_row * img_col;
	conv_specified(out, in, weight);
}

/*
 * Functions for block wise
 * */
static void conv_specified_block_wise(
		float *out_block_wise,
		float *in_block_wise,
		float *weight_block_wise
		)
{
	int num_elem_output_bw = BN_R * BN_C * BN_O * R * BS_R * BS_C * O;
	memset(out_block_wise, 0, sizeof(float) * num_elem_output_bw);

//	for (int r0 = 0; r0 < cfg_BN_R; r0++)
//	for (int c0 = 0; c0 < cfg_BN_C; c0++)
//	for (int o0 = 0; o0 < cfg_BN_O; o0++) {
//		for (int r1 = 0; r1 < BS_R; r1++)
//		for (int c1 = 0; c1 < BS_C; c1++) {
//			for (int c2 = 0; c2 < C; c2++)
//			for (int o2 = 0; o2 < O; o2++) {
//				int out_idx = ((r0 * cfg_BN_C + c0) * cfg_BN_O + o0) * BS_R * C * BS_C * O + ((r1 * C + c2) * BS_C + c1) * O + o2; // [r0][c0][o0][r1][c2][c1][o2]
//
//				out_block_wise[out_idx] = 0;
//			}}}

	for (int r0 = 0; r0 < BN_R; r0++)
	for (int c0 = 0; c0 < BN_C; c0++)
	for (int o0 = 0; o0 < BN_O; o0++) 
	for (int i0 = 0; i0 < BN_I; i0++) {
		for (int p = 0; p < K; p++)
		for (int q = 0; q < K; q++)
		for (int i1 = 0; i1 < BS_I; i1++)
		for (int r1 = 0; r1 < BS_R; r1++)
		for (int c1 = 0; c1 < BS_C; c1++)
		for (int o1 = 0; o1 < BS_O; o1++) {
			for (int r2 = 0; r2 < R; r2++)
			for (int o2 = 0; o2 < O; o2++) 
			for (int i2 = 0; i2 < I; i2++) {
				int out_idx_bw = ((r0 * BN_C + c0) * BN_O + o0) * R * BS_R * BS_C * O * BS_O + (r2 * BS_R * BS_C + r1 * BS_C + c1) * O * BS_O + o2 * BS_O + o1; // [c0][r0][o0][c2][c1][r1][o2][o1]
				int in_idx_bw = (((((r0 * BN_C + c0) * BN_I + i0) * (R * BS_R + K - 1) + (r2 * BS_R + r1 + p)) * (BS_C + K - 1) + (c1 + q)) * BS_I + i1) * I + i2;
				int w_idx_bw = (o0 * BN_I + i0) * BS_O * O * K * K * BS_I * I + ((o2 * K + p) * K + q) * BS_I * I + i1 * I + i2;

				out_block_wise[out_idx_bw] += in_block_wise[in_idx_bw] * weight_block_wise[w_idx_bw];

//				if (out_idx_bw == 80) {
//					printf("out_block_wise[%d]+=in_block_wise[%d]*weight_block_wise[%d]:%f+=%f*%f\n", out_idx_bw, in_idx_bw, w_idx_bw, out_block_wise[out_idx_bw], in_block_wise[in_idx_bw], weight_block_wise[w_idx_bw]);
//				}

			}}}
}

void cnn_specified_block_wise(
		float* out,
		float* in,
		float* weight
		)
{
	int img_row = BN_R * BS_R * R;
	int img_col = BN_C * BS_C;
	int in_num = BN_I * BS_I * I;
	int out_num = BN_O * BS_O * O;

	// block wise reformat for input and weight
//	unsigned int num_elem_weight = cfg_out_num[0] * cfg_kernel[0] * cfg_kernel[0] * cfg_in_num[0];
	unsigned int num_elem_input_fm = (img_row + K - 1) * (img_col + K - 1) * in_num;
//	unsigned int num_elem_output_fm = cfg_out_num[0] * cfg_img_row[0] * cfg_img_col[0];

//	float *out_block_wise_padded = (float *)malloc(num_elem_input_fm * sizeof(float));

	int num_elem_input_fm_bw = BN_R * BN_C * BN_I * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I;
	int num_elem_weight = out_num * K * K * in_num;
	int num_elem_weight_bw = BN_O * BN_I * O * K * K * BS_I * I;
	int	num_elem_output_fm_bw = BN_R * BN_C * BN_O * R * BS_R * BS_C * O * BS_O;
	//	int num_elem_output_fm_bw_scaled = (cfg_BN_R[layer] + cfg_pooling_size[layer] - 1 / cfg_pooling_size[layer]) * (cfg_BN_C[layer] + cfg_pooling_size[layer] - 1 / cfg_pooling_size[layer]) * cfg_BN_O[layer] * R * BS_R * BS_C * O * BS_O;

	float *in_block_wise = (float *)malloc(num_elem_input_fm_bw * sizeof(float));
	float *weight_block_wise = (float *)malloc(num_elem_weight_bw * sizeof(float));
	float *out_block_wise = (float *)malloc(num_elem_output_fm_bw * sizeof(float));
	//	float *relu_out_block_wise = (float *)malloc(num_elem_output_fm_bw * sizeof(float));
	//	float *pooling_out_block_wise = (float *)malloc(num_elem_output_fm_bw_scaled * sizeof(float));
	//	float *out_block_wise = (float *)malloc(num_elem_output_fm_bw_scaled * sizeof(float));

	//	conv_specified_block_wise(i, out, in, weight, num_elem_output_fm, num_elem_input_fm, num_elem_weight);

	block_wise_reformat_input(in, in_block_wise);
	block_wise_reformat_weight(weight, weight_block_wise);

	// convolutional layer
	conv_specified_block_wise(out_block_wise, in_block_wise, weight_block_wise);

	block_wise_reformat_output(out_block_wise, out);

	free(in_block_wise);
	free(weight_block_wise);
	free(out_block_wise);
}

void cnn_gold(
       float *weight,
       float *input_fm,
       float *output_fm
		)
{
    printf("CNN golden ... \n");
    fflush(stdout);

	int img_row = BN_R * BS_R * R;
	int img_col = BN_C * BS_C;
	int in_num = BN_I * BS_I * I;
	int out_num = BN_O * BS_O * O;

	unsigned int num_elem_input_fm = in_num * (img_row + K - 1) * (img_col + K - 1);
	unsigned int num_elem_output_fm = out_num * img_row * img_col;

	float *output_fm_specified = (float *)malloc(num_elem_output_fm * sizeof(float));
	float *output_fm_specified_block_wise = (float *)malloc(num_elem_output_fm * sizeof(float));

	cnn(output_fm, input_fm, weight);

	cnn_specified(output_fm_specified, input_fm, weight);

	cnn_specified_block_wise(output_fm_specified_block_wise, input_fm, weight);

//	memcpy(output_fm, output_fm_specified_block_wise, sizeof(float) * num_elem_output_fm);

	int err = 0;
	for (int i = 0; i < num_elem_output_fm; i++) {
		if (fabs(output_fm[i] - output_fm_specified_block_wise[i]) > 0.01) {
			if (err < 10) printf("out[%d]=%f, out1[%d]=%f\n", i, output_fm[i], i, output_fm_specified_block_wise[i]);
			else if (err == 10) printf("...\n");
			err++;
		}
//		if (fabs(output_fm[i] - output_fm_specified[i]) > 0.01) {
//			if (err < 10) printf("out[%d]=%f, out1[%d]=%f\n", i, output_fm[i], i, output_fm_specified[i]);
//			else if (err == 10) printf("...\n");
//			err++;
//		}
	}

	free(output_fm_specified);
	free(output_fm_specified_block_wise);

	printf("CNN golden finished \n");
}
