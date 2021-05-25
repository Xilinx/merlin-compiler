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


void block_wise_reformat_input(int layer, data_precision *input, data_precision *input_block_wise)
{
	int addr_in, addr_in_bw;

	for (int r0 = 0; r0 < cfg_BN_R[layer]; r0++)
	for (int c0 = 0; c0 < cfg_BN_C[layer]; c0++)
    for (int i0 = 0; i0 < cfg_BN_I[layer]; i0++) {
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
					addr_in = ((r + p) * (cfg_img_col[layer] + cfg_kernel[layer] - 1) + (c + q)) * cfg_in_num[layer] + i;
					addr_in_bw = (((((r0 * cfg_BN_C[layer] + c0) * cfg_BN_I[layer] + i0) * (R * BS_R + K - 1) + (r2 * BS_R + r1 + p)) * (BS_C + K - 1) + (c1 + q)) * BS_I + i1) * I + i2;

				//	if (addr_in_bw == 1984512)
				//		printf("layer=%d, r=%d\n", layer, r);

					if ((r + p) < (cfg_img_row[layer] + K - 1) && (c + q) < (cfg_img_col[layer] + K - 1) && i < cfg_in_num[layer])
					{
						input_block_wise[addr_in_bw] = input[addr_in];
					}
					else
					{
						// maginal zero padding
						input_block_wise[addr_in_bw] = 0;
					}
	}}}
}

void block_wise_reformat_weights(int layer, data_precision *weights, data_precision *weights_block_wise)
{
	int addr_w, addr_w_bw;

    for (int o0 = 0; o0 < cfg_BN_O[layer]; o0++)
    for (int i0 = 0; i0 < cfg_BN_I[layer]; i0++) {
        for (int p = 0; p < K; p++)
        for (int q = 0; q < K; q++)
        for (int i1 = 0; i1 < BS_I; i1++)
        for (int o1 = 0; o1 < BS_O; o1++) {
        for (int o2 = 0; o2 < O; o2++)
        for (int i2 = 0; i2 < I; i2++) {

			int o = o0 * BS_O * O + o2 * BS_O + o1;
			int i = i0 * BS_I * I + i1 * I + i2;

			addr_w = ((o * K + p) * K + q) * cfg_in_num[layer] + i;
			addr_w_bw = (o0 * cfg_BN_I[layer] + i0) * BS_O * O * K * K * BS_I * I + (((o2 * BS_O + o1) * K + p) * K + q) * BS_I * I + i1 * I + i2;

			// w1[o0][i0][o2][o1][p][q][i1][i2] = w0[o0][o2][o1][p][q][i0][i1][i2]
			if (o < cfg_out_num[layer] && i < cfg_in_num[layer])
			{
				weights_block_wise[addr_w_bw] = weights[addr_w];
			}
			else
			{
				weights_block_wise[addr_w_bw] = 0;
			}
	}}}
}

void block_wise_reformat_output(int layer, data_precision *output_block_wise, data_precision *output)
{
	const unsigned int pooled_img_row = cfg_img_row[layer] / cfg_pooling_size[layer];
	const unsigned int pooled_img_col = cfg_img_col[layer] / cfg_pooling_size[layer];

	const unsigned int cfg_BN_R = (pooled_img_row + R * BS_R - 1) / (R * BS_R);
	const unsigned int cfg_BN_C = (pooled_img_col + BS_C - 1) / BS_C;
	const unsigned int cfg_BN_O = (cfg_out_num[layer] + O * BS_O - 1) / (O * BS_O);

	for (int r0 = 0; r0 < cfg_BN_R; r0++)
	for (int r2 = 0; r2 < R; r2++) 
	for (int r1 = 0; r1 < BS_R; r1++) 
	{
		for (int c0 = 0; c0 < cfg_BN_C; c0++)
		for (int c1 = 0; c1 < BS_C; c1++)
		{
			for (int o0 = 0; o0 < cfg_BN_O; o0++)
			for (int o2 = 0; o2 < O; o2++)
			for (int o1 = 0; o1 < BS_O; o1++) 
			{
				int r = (r0 * R + r2) * BS_R + r1;
				int c = c0 * BS_C + c1;
				int o = (o0 * O + o2) * BS_O + o1;
				// out0[r0][r2][r1][c0][c1][o0][o2][o1] = out2[r0][c0][o0][r2][r1][c1][o2][o1]
				int addr_out_bw = ((r0 * cfg_BN_C + c0) * cfg_BN_O + o0) * R * BS_R * BS_C * O * BS_O + ((r2 * BS_R + r1) * BS_C + c1) * O * BS_O + o2 * BS_O + o1;
				int addr_out = (r * pooled_img_col + c) * cfg_out_num[layer] + o;

				if (r < pooled_img_row && c < pooled_img_col && o < cfg_out_num[layer])
				{
					output[addr_out] = output_block_wise[addr_out_bw];
				}
		}}}
}

/*
 * Functions for golden
 * */
void zero_padding(int layer, data_precision *out, data_precision *out_padded)
{
	const unsigned cfg_IMG_IN_ROW = cfg_img_row[layer + 1] + cfg_kernel[layer + 1] - 1;
	const unsigned cfg_IMG_IN_COL = cfg_img_col[layer + 1] + cfg_kernel[layer + 1] - 1;

	for (int r = 0; r < cfg_IMG_IN_ROW; r++)
	for (int c = 0; c < cfg_IMG_IN_COL; c++)
	for (int o = 0; o < cfg_out_num[layer]; o++)
	{
		int out_padded_idx = (r * cfg_IMG_IN_COL + c) * cfg_out_num[layer] + o;

		if ((r >= 0 && r < (cfg_kernel[layer + 1] - 1) / 2) || (r >= cfg_img_row[layer + 1] + (cfg_kernel[layer + 1] - 1) / 2 && r < cfg_IMG_IN_ROW) || (c >= 0 && c < (cfg_kernel[layer + 1] - 1) / 2) || (c >= cfg_img_col[layer + 1] + (K - 1) / 2 && c < cfg_IMG_IN_COL)) 
		{
			out_padded[out_padded_idx] = 0;
		}
		else 
		{
			int out_idx = ((r - ((cfg_kernel[layer + 1] - 1) / 2)) * cfg_img_col[layer + 1] + (c - ((cfg_kernel[layer + 1] - 1) / 2))) * cfg_out_num[layer] + o;
			out_padded[out_padded_idx] = out[out_idx];
		}
	}
}


static void conv(
     unsigned int layer,
	   data_precision *out,
	   data_precision *in,
	   data_precision *weight
	   )
{
    // Initialize output
    // TODO: Initialize with bias
	for (int r = 0; r < cfg_img_row[layer]; r++) {
		for (int c = 0; c < cfg_img_col[layer]; c++) {
			for (int o = 0; o < cfg_out_num[layer]; o++) {
				// out[r][c][o]
				out[r * (cfg_img_col[layer] * cfg_out_num[layer]) + c * cfg_out_num[layer] + o] = 0;
			}
		}
	}
    
  for (int o = 0; o < cfg_out_num[layer]; o++)
  for (int i = 0; i < cfg_in_num[layer]; i++)
  for (int c = 0; c < cfg_img_col[layer]; c++)
  for (int r = 0; r < cfg_img_row[layer]; r++)
  for (int p = 0; p < cfg_kernel[layer]; p++)
  for (int q = 0; q < cfg_kernel[layer]; q++) {
	// out[r][c][o] = in[r+p][c+q][i] * weight[o][p][q][i]
		int out_idx = r * (cfg_img_col[layer] * cfg_out_num[layer]) + c * cfg_out_num[layer] + o;
		int in_idx = (r + p) * (cfg_img_col[layer] + cfg_kernel[layer] - 1) * cfg_in_num[layer] + (c + q) * cfg_in_num[layer] + i;
		int w_idx = o * (cfg_in_num[layer] * cfg_kernel[layer] * cfg_kernel[layer]) + p * (cfg_in_num[layer] * cfg_kernel[layer]) + q * cfg_in_num[layer] + i;

        out[out_idx] += in[in_idx] * weight[w_idx];

//		if (out_idx == 3211968) 
//		{
//			printf("r=%d,c=%d,o=%d,out[%d]+=in[%d]*weights[%d]:%f+=%f*%f\n", r, c, o, out_idx, in_idx, w_idx, out[out_idx], in[in_idx], weight[w_idx]);
//		}
    }
}

void relu(int layer, data_precision *relu_in, data_precision *relu_out)
{
	int	num_elem_output_fm = cfg_out_num[layer] * cfg_img_row[layer] * cfg_img_col[layer];

	// workaround
	memcpy(relu_out, relu_in, sizeof(data_precision) * num_elem_output_fm);
}

void pooling(int layer, data_precision *pooling_in, data_precision *pooling_out)
{
	int pooled_img_row = cfg_img_row[layer] / cfg_pooling_size[layer];
	int pooled_img_col = cfg_img_col[layer] / cfg_pooling_size[layer];
//	int	num_elem_output_fm_new = cfg_out_num[layer] * next_img_row * next_img_col;

	for (int r = 0; r < pooled_img_row; r++)
	for (int c = 0; c < pooled_img_col; c++)
	for (int o = 0; o < cfg_out_num[layer]; o++) 
	{
		data_precision max = -1.0; // -INF
		int pooling_out_idx = (r * pooled_img_col + c) * cfg_out_num[layer] + o;

		for (int p = 0; p < cfg_pooling_size[layer]; p++)
		for (int q = 0; q < cfg_pooling_size[layer]; q++)
		{
			int pooling_in_idx = ((r * cfg_pooling_size[layer] + p) * cfg_img_col[layer] + (c * cfg_pooling_size[layer] + q)) * cfg_out_num[layer] + o;

			if (max == -1.0 || max < pooling_in[pooling_in_idx])
				max = pooling_in[pooling_in_idx];
		}
		pooling_out[pooling_out_idx] = max;
	}
}

void print_out_padded(int layer, data_precision *out_padded)
{
	printf("%s\n", __func__);
	FILE *f;

	f = fopen("pad.txt", "w");

//	fprintf(f, "layer=%d\n", layer);

	const unsigned int cfg_BN_R = ((cfg_img_row[layer] / cfg_pooling_size[layer]) + R * BS_R - 1) / (R * BS_R);
	const unsigned int cfg_BN_C = ((cfg_img_col[layer] / cfg_pooling_size[layer]) + BS_C - 1) / BS_C;
	const unsigned int cfg_BN_O = (cfg_out_num[layer] + O * BS_O - 1) / (O * BS_O);

	for (int r0 = 0; r0 < cfg_BN_R; r0++)
	for (int c0 = 0; c0 < cfg_BN_C; c0++)
	for (int i0 = 0; i0 < cfg_BN_O; i0++) 
	{
		for (int r_p = 0; r_p < R * BS_R + K - 1; r_p++)
		for (int c_q = 0; c_q < BS_C + K - 1; c_q++) 
		for (int i12 = 0; i12 < BS_I * I; i12++)
		{
			int r = r0 * R * BS_R + r_p;
			int c = c0 * BS_C + c_q;
			int i = i0 * BS_I * I + i12;

			int out_padded_idx = ((r0 * R * BS_R + r_p) * (cfg_BN_C * BS_C + K - 1) + c0 * BS_C + c_q) * cfg_BN_O * BS_I * I + (i0 * BS_I * I + i12);

		//	if (r0 == cfg_BN_R - 1 && c0 == cfg_BN_C - 1 && i0 == cfg_BN_O - 1)
			{
				if (r < cfg_img_row[layer] / cfg_pooling_size[layer] + K - 1 && c < cfg_img_col[layer] / cfg_pooling_size[layer] + K - 1 && i < cfg_out_num[layer])
				{
				//	printf("%.3f\n", out_padded[out_padded_idx]);
					fprintf(f, "%.3f\n", out_padded[out_padded_idx]);
				}
				else
				{
				//	printf("%.3f\n", 0);
					fprintf(f, "%.3f\n", 0);
				}
			}
		}
	}

	fclose(f);
}

void print_conv_out(int layer, data_precision *conv_out)
{
	printf("%s\n", __func__);

	const unsigned int cfg_BN_R = (cfg_img_row[layer] + R * BS_R - 1) / (R * BS_R);
	const unsigned int cfg_BN_C = (cfg_img_col[layer] + BS_C - 1) / BS_C;
	const unsigned int cfg_BN_O = (cfg_out_num[layer] + O * BS_O - 1) / (O * BS_O);

	FILE *f;

	f = fopen("conv_out.txt", "w");
//	printf("layer=%d\n", layer);
//	fprintf(f, "layer=%d\n", layer);

	for (int r0 = 0; r0 < cfg_BN_R; r0++)
	for (int c0 = 0; c0 < cfg_BN_C; c0++)
	for (int o0 = 0; o0 < cfg_BN_O; o0++)
	{
		for (int rr = 0; rr < R * BS_R; rr++)
		for (int cc = 0; cc < BS_C; cc++)
		for (int oo = 0; oo < O * BS_O; oo++)
		{
			int r = r0 * R * BS_R + rr;
			int c = c0 * BS_C + cc;
			int o = o0 * O * BS_O + oo;

			int out_idx = r * cfg_img_col[layer] * cfg_out_num[layer] + c * cfg_out_num[layer] + o;

		//	int out_idx_bw = ((r0 * cfg_BN_C + c0) * cfg_BN_O + o0) * R * BS_R * BS_C * BS_O * O + (rr * BS_C + cc) * O * BS_O + oo;


	//		if (r0 == cfg_BN_R - 1 && c0 == cfg_BN_C - 1 && o0 == cfg_BN_O - 1)
			{
				if (r < cfg_img_row[layer] && c < cfg_img_col[layer] && o < cfg_out_num[layer])
				{
					//	printf("%.3f\n", conv_out[out_idx]);
					fprintf(f, "%.3f\n", conv_out[out_idx]);
				}
				else
				{
					// FIXME: fprintf(f, "%.3f", 0) cannot write "0" to file, weird.
					data_precision aa = 0;
					fprintf(f, "%.3f\n", aa);

				//	if (out_idx == 3211968)
				//	{
				//		fprintf(f, "%d:%.3f\n", out_idx, aa);
				//	}
				//	else
				//	{
				//	//	fprintf(f, "%.3f\n", 0);
				//		fprintf(f, "%.3f\n", aa);
				//	}
				}
			}

//			if (out_idx == 3211968)
//			{
//				printf("out_idx_bw=%d\n", out_idx_bw);
//				printf("r=%d,c=%d,o=%d, conv_out[%d]=%.3f\n", r, c, o, out_idx, conv_out[out_idx]);
//			}
		}
	}

	fclose(f);
}

void print_pooling_out(int layer, data_precision *pooling_out)
{

	printf("%s\n", __func__);

	const unsigned int pooled_img_row = cfg_img_row[layer] / cfg_pooling_size[layer];
	const unsigned int pooled_img_col = cfg_img_col[layer] / cfg_pooling_size[layer];

	const unsigned int cfg_BN_R = (pooled_img_row + R * BS_R - 1) / (R * BS_R);
	const unsigned int cfg_BN_C = (pooled_img_col + BS_C - 1) / BS_C;
	const unsigned int cfg_BN_O = (cfg_out_num[layer] + O * BS_O - 1) / (O * BS_O);

//	printf("pooled_img_row=%d, pooled_img_col=%d, cfg_BN_R=%d, cfg_BN_C=%d, cfg_BN_O=%d\n", pooled_img_row, pooled_img_col, cfg_BN_R, cfg_BN_C, cfg_BN_O);

	FILE *f;

	f = fopen("pooling_out.txt", "w");

	for (int r0 = 0; r0 < cfg_BN_R; r0++)
	for (int c0 = 0; c0 < cfg_BN_C; c0++)
	for (int o0 = 0; o0 < cfg_BN_O; o0++)
	{
		for (int rr = 0; rr < R * BS_R; rr++)
		for (int cc = 0; cc < BS_C; cc++)
		for (int oo = 0; oo < O * BS_O; oo++)
		{
			int r = r0 * R * BS_R + rr;
			int c = c0 * BS_C + cc;
			int o = o0 * O * BS_O + oo;

			int out_idx = r * pooled_img_col * cfg_out_num[layer] + c * cfg_out_num[layer] + o;

		//	if (out_idx == 817472)
		//	{
		//		printf("r=%d, c=%d, o=%d\n", r, c, o);
		//	}

			if (r < pooled_img_row && c < pooled_img_col && o < cfg_out_num[layer])
			{
				//	printf("%.3f\n", pooling_out[out_idx]);
				fprintf(f, "%.3f\n", pooling_out[out_idx]);
			}
			else
			{
				fprintf(f, "%.3f\n", 0.0);
			}
		}
	}

	fclose(f);
}

void cnn(
		data_precision* out,
		data_precision* in,
		data_precision* weights
	   )
{
	unsigned int num_elem_input_fm = (cfg_img_row[0] + cfg_kernel[0] - 1) * 
		(cfg_img_col[0] + cfg_kernel[0] - 1) * cfg_in_num[0];

	// initiate with length of input feature map number of first layer, will change later
	data_precision *out_padded = (data_precision *)malloc(num_elem_input_fm * sizeof(data_precision));

	data_precision *weight = weights;

	for (int layer = 0; layer < NUM_CONV; layer++)
	{
		unsigned int num_elem_weight = cfg_out_num[layer] * cfg_kernel[layer] * cfg_kernel[layer] * cfg_in_num[layer];
		unsigned int num_elem_input_fm = (cfg_img_row[layer] + cfg_kernel[layer] - 1) * 
			(cfg_img_col[layer] + cfg_kernel[layer] - 1) * cfg_in_num[layer];
		unsigned int num_elem_output_fm = cfg_out_num[layer] * cfg_img_row[layer] * cfg_img_col[layer];
		unsigned int num_elem_output_fm_scaled = cfg_out_num[layer] * (cfg_img_row[layer] / cfg_pooling_size[layer]) * (cfg_img_col[layer] / cfg_pooling_size[layer]);

		data_precision *in_copy = (data_precision *)malloc(num_elem_input_fm * sizeof(data_precision));
		data_precision *conv_out = (data_precision *)malloc(num_elem_output_fm * sizeof(data_precision));
		data_precision *relu_out = (data_precision *)malloc(num_elem_output_fm * sizeof(data_precision));
		data_precision *pooling_out = (data_precision *)malloc(num_elem_output_fm_scaled * sizeof(data_precision));

		if (0 == layer)
		{
			memcpy(in_copy, in, sizeof(data_precision) * num_elem_input_fm);
		}
		else
		{
			memcpy(in_copy, out_padded, sizeof(data_precision) * num_elem_input_fm);
		}

		printf("%s: layer %d conv\n", __func__, layer);
		fflush(stdout);

		// convolutional layer
		conv(layer, conv_out, in_copy, weight);

#if DEBUG_CONV
		if (layer == TEST_LAYER)
			print_conv_out(layer, conv_out);
#endif

		printf("%s: layer %d relu\n", __func__, layer);
		fflush(stdout);

		// relu
		relu(layer, conv_out, relu_out);

		// pooling
		if (cfg_pooling_size[layer] > 1)
		{
			printf("%s: layer %d pooling\n", __func__, layer);
			fflush(stdout);

			pooling(layer, relu_out, pooling_out);

#if DEBUG_POOLING
			if (layer == TEST_LAYER)
				print_pooling_out(layer, pooling_out);
#endif

		}

		weight += num_elem_weight;

		// Assign output to the input for the next layer
		if (layer < NUM_CONV - 1)
		{
			unsigned int next_elem_input_fm = (cfg_img_row[layer + 1] + cfg_kernel[layer + 1] - 1) * 
			(cfg_img_col[layer + 1] + cfg_kernel[layer + 1] - 1) * cfg_in_num[layer + 1];

			out_padded = (data_precision *)realloc(out_padded, sizeof(data_precision) * next_elem_input_fm);

			printf("%s: layer %d zero-padding\n", __func__, layer);
			fflush(stdout);

			if (cfg_pooling_size[layer] > 1)
			{
				zero_padding(layer, pooling_out, out_padded);
			} 
			else
			{
				zero_padding(layer, relu_out, out_padded);
			}

#if DEBUG_ZERO_PADDING
			{
				if (layer == TEST_LAYER)
					print_out_padded(layer, out_padded);
			}
#endif
		}
		if (layer == NUM_CONV - 1)
		{
			if (cfg_pooling_size[layer] > 1)
				memcpy(out, pooling_out, sizeof(data_precision) * num_elem_output_fm_scaled);
			else
				memcpy(out, relu_out, sizeof(data_precision) * num_elem_output_fm);
		}

		free(in_copy);
		free(conv_out);
		free(relu_out);
		free(pooling_out);
	}

	free(out_padded);
}

/*
 * Functions for specified
 * */
static void conv_specified(
		unsigned int layer,
		data_precision* out,
		data_precision* in,
		data_precision* weight
	   )
{
	for (int r = 0; r < cfg_img_row[layer]; r++) {
		for (int c = 0; c < cfg_img_col[layer]; c++) {
			for (int o = 0; o < cfg_out_num[layer]; o++) {
				// out[r][c][o]
				out[r * (cfg_img_col[layer] * cfg_out_num[layer]) + c * cfg_out_num[layer] + o] = 0;
			}
		}
	}

	// TODO: Add more outer loops for different kernel and image size
//	const unsigned int cfg_BN_R = ceil(cfg_img_col[layer] / BS_R);
	const unsigned int cfg_BN_R = (cfg_img_row[layer] + R * BS_R - 1) / (R * BS_R);
//	const unsigned int cfg_BN_C = ceil(cfg_img_col[layer] / (BS_C * C));
	const unsigned int cfg_BN_C = (cfg_img_col[layer] + BS_C - 1) / BS_C;
//	const unsigned int cfg_BN_O = ceil(cfg_out_num[layer] / O);
	const unsigned int cfg_BN_O = (cfg_out_num[layer] + O * BS_O - 1) / (O * BS_O);
//	const unsigned int cfg_BN_I = ceil(cfg_in_num[layer] / (BS_I * I));
	const unsigned int cfg_BN_I = (cfg_in_num[layer] + BS_I * I - 1) / (BS_I * I);

	printf("cfg_BN_R=%d cfg_BN_C=%d cfg_BN_O=%d cfg_BN_I=%d\n", cfg_BN_R, cfg_BN_C, cfg_BN_O, cfg_BN_I);

	for (int r0 = 0; r0 < cfg_BN_R; r0++)
	for (int c0 = 0; c0 < cfg_BN_C; c0++)
	for (int o0 = 0; o0 < cfg_BN_O; o0++) 
	for (int i0 = 0; i0 < cfg_BN_I; i0++) {
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

				int out_idx = (r * cfg_img_col[layer] + c) * cfg_out_num[layer] + o; // [r0][r2][r1][c0][c1][o0][o2][o1]
				int in_idx = ((r + p) * (cfg_img_col[layer] + cfg_kernel[layer] - 1) + c + q) * cfg_in_num[layer] + i; // [r0 * R * BS_R + r2 * R + r1 + p][c0 * BS_C + c1 + q][i0][i1][i2]
				int w_idx = ((o * K + p) * K + q) * cfg_in_num[layer] + i; // [o0][o2][p][q][i0][i1][i2]

				if (r < cfg_img_row[layer] && c < cfg_img_col[layer] && o < cfg_out_num[layer] && i < cfg_in_num[layer]) {
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
		data_precision* out,
		data_precision* in,
		data_precision* weights
		)
{
	data_precision *weight = weights;

	for (int layer = 0; layer < NUM_CONV; layer++) {
		unsigned int num_elem_weight = cfg_out_num[layer] * cfg_kernel[layer] * cfg_kernel[layer] * cfg_in_num[layer];
		unsigned int num_elem_input_fm = (cfg_img_row[layer] + cfg_kernel[layer] - 1) * 
			(cfg_img_col[layer] + cfg_kernel[layer] - 1) * cfg_in_num[layer];
		unsigned int num_elem_output_fm = cfg_out_num[layer] * cfg_img_row[layer] * cfg_img_col[layer];
		conv_specified(layer, out, in, weight);

		weight += num_elem_weight;
	}
}

/*
 * Functions for block wise
 * */
static void conv_specified_block_wise(
		int layer,
		data_precision *out_block_wise,
		data_precision *in_block_wise,
		data_precision *weight_block_wise
		)
{
	const unsigned int cfg_BN_R = (cfg_img_row[layer] + R * BS_R - 1) / (R * BS_R);
	const unsigned int cfg_BN_C = (cfg_img_col[layer] + BS_C - 1) / BS_C;
	const unsigned int cfg_BN_O = (cfg_out_num[layer] + O * BS_O - 1) / (O * BS_O);
	const unsigned int cfg_BN_I = (cfg_in_num[layer] + BS_I * I - 1) / (BS_I * I);

	int num_elem_output_bw = cfg_BN_R * cfg_BN_C * cfg_BN_O * R * BS_R * BS_C * O;
	memset(out_block_wise, 0, sizeof(data_precision) * num_elem_output_bw);

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

	for (int r0 = 0; r0 < cfg_BN_R; r0++)
	for (int c0 = 0; c0 < cfg_BN_C; c0++)
	for (int o0 = 0; o0 < cfg_BN_O; o0++) 
	for (int i0 = 0; i0 < cfg_BN_I; i0++) 
	{
		for (int p = 0; p < K; p++)
		for (int q = 0; q < K; q++)
		for (int i1 = 0; i1 < BS_I; i1++)
		for (int r1 = 0; r1 < BS_R; r1++)
		for (int c1 = 0; c1 < BS_C; c1++)
		for (int o1 = 0; o1 < BS_O; o1++)
		{
			for (int r2 = 0; r2 < R; r2++)
			for (int o2 = 0; o2 < O; o2++) 
			{
				int out_idx_bw = ((r0 * cfg_BN_C + c0) * cfg_BN_O + o0) * R * BS_R * BS_C * O * BS_O + (r2 * BS_R * BS_C + r1 * BS_C + c1) * O * BS_O + o2 * BS_O + o1; // [c0][r0][o0][c2][c1][r1][o2][o1]

				for (int i2 = 0; i2 < I; i2++)
				{
				//	int out_idx_bw = ((r0 * cfg_BN_C + c0) * cfg_BN_O + o0) * R * BS_R * BS_C * O * BS_O + (r2 * BS_R * BS_C + r1 * BS_C + c1) * O * BS_O + o2 * BS_O + o1; // [c0][r0][o0][c2][c1][r1][o2][o1]
					int in_idx_bw = (((((r0 * cfg_BN_C + c0) * cfg_BN_I + i0) * (R * BS_R + K - 1) + (r2 * BS_R + r1 + p)) * (BS_C + K - 1) + (c1 + q)) * BS_I + i1) * I + i2;
					int w_idx_bw = (o0 * cfg_BN_I + i0) * BS_O * O * K * K * BS_I * I + ((o2 * K + p) * K + q) * BS_I * I + i1 * I + i2;

					out_block_wise[out_idx_bw] += in_block_wise[in_idx_bw] * weight_block_wise[w_idx_bw];

				}

#if DEBUG_CONV
				if (layer == TEST_LAYER)
				{
					int r = r0 * R * BS_R + r2 * R + r1;
					int c = c0 * BS_C + c1;
					int o = o0 * O * BS_O + o2 * BS_O + o1;
					int i = i0 * BS_I + i1;

					if (out_idx_bw == 1438080)
					{
						printf("out_block_wise[%d]=%.3f<-{", out_idx_bw, out_block_wise[out_idx_bw]);
						for (int i2 = 0; i2 < I; i2++)
						{
							int in_idx_bw = (((((r0 * cfg_BN_C + c0) * cfg_BN_I + i0) * (R * BS_R + K - 1) + (r2 * BS_R + r1 + p)) * (BS_C + K - 1) + (c1 + q)) * BS_I + i1) * I + i2;

							printf("%.3f ", in_block_wise[in_idx_bw]);
						}
						printf("}.{");
						for (int i2 = 0; i2 < I; i2++)
						{
							int w_idx_bw = (o0 * cfg_BN_I + i0) * BS_O * O * K * K * BS_I * I + ((o2 * K + p) * K + q) * BS_I * I + i1 * I + i2;

							printf("%.3f ", weight_block_wise[w_idx_bw]);
						}
						printf("} r0=%d,c0=%d,o0=%d,i0=%d,r2=%d,r1=%d,c1=%d,o2=%d,o1=%d,i1=%d,r=%d,c=%d,o=%d,i=%d\n", r0, c0, o0, i0, r2, r1, c1, o2, o1, i1, r, c, o, i);
					}
				}
#endif
			}
		}
	}

	// important for multi-layer connection: filter the cases where r and c are beyond output boundaries but input is within boundaries (input[r+p][c+q][i])
	for (int r0 = 0; r0 < cfg_BN_R; r0++)
	for (int c0 = 0; c0 < cfg_BN_C; c0++)
	for (int o0 = 0; o0 < cfg_BN_O; o0++) 
	{
		for (int r1 = 0; r1 < BS_R; r1++)
		for (int c1 = 0; c1 < BS_C; c1++)
		for (int o1 = 0; o1 < BS_O; o1++)
		{
			for (int r2 = 0; r2 < R; r2++)
			for (int o2 = 0; o2 < O; o2++) 
			{
				int r = r0 * R * BS_R + r2 * BS_R + r1;
				int c = c0 * BS_C + c1;
				int o = o0 * O * BS_O + o2 * BS_O + o1;

				int out_idx_bw = ((r0 * cfg_BN_C + c0) * cfg_BN_O + o0) * R * BS_R * BS_C * O * BS_O + (r2 * BS_R * BS_C + r1 * BS_C + c1) * O * BS_O + o2 * BS_O + o1; // [c0][r0][o0][c2][c1][r1][o2][o1]

				if (r >= cfg_img_row[layer] || c >= cfg_img_col[layer] || o >= cfg_out_num[layer])
				{
					out_block_wise[out_idx_bw] = 0;
				}
			}
		}
	}
}

void relu_block_wise(int layer, data_precision *relu_in, data_precision *relu_out)
{
	int	num_elem_output_fm_bw = cfg_BN_R[layer] * cfg_BN_C[layer] * cfg_BN_O[layer] * R * BS_R * BS_C * O * BS_O;

	memcpy(relu_out, relu_in, sizeof(data_precision) * num_elem_output_fm_bw);
}

void pooling_block_wise(int layer, data_precision *pooling_in, data_precision *pooling_out)
{
	int cfg_BN_R = (cfg_img_row[layer] + R * BS_R - 1) / (R * BS_R); // ceil()
	int cfg_BN_C = (cfg_img_col[layer] + BS_C - 1) / BS_C;
	int cfg_BN_O = (cfg_out_num[layer] + O * BS_O - 1) / (O * BS_O);

	// step 1: down scaling (scaling_buffer<-pooing_in)
	int next_BN_R = ((cfg_img_row[layer] / cfg_pooling_size[layer]) + R * BS_R - 1) / (R * BS_R); // ceil()
	int next_BN_C = ((cfg_img_col[layer] / cfg_pooling_size[layer]) + BS_C - 1) / BS_C; // ceil()
	int next_BN_O = cfg_BN_O;

//	printf("%s: next_BN_R=%d next_BN_C=%d next_BN_O=%d\n", __func__, next_BN_R, next_BN_C, next_BN_O);
//	fflush(stdout);

	int BS_R_R_scaled = (R * BS_R) / cfg_pooling_size[layer];
	int BS_C_C_scaled = BS_C / (cfg_pooling_size[layer]);

	int scaling_buffer_length = next_BN_R * next_BN_C * next_BN_O * R * BS_R * BS_C * O * BS_O;
	data_precision *scaling_buffer = (data_precision *)malloc(sizeof(data_precision) * scaling_buffer_length);
	memset(scaling_buffer, 0, sizeof(data_precision) * scaling_buffer_length);

	for (int r0 = 0; r0 < cfg_BN_R; r0++)
	for (int c0 = 0; c0 < cfg_BN_C; c0++)
	for (int o0 = 0; o0 < cfg_BN_O; o0++)
	{
		for (int r21 = 0; r21 < BS_R_R_scaled; r21++)
		for (int c21 = 0; c21 < BS_C_C_scaled; c21++)
		for (int o21 = 0; o21 < O * BS_O; o21++)
		{

			data_precision max = -1.0;
			int scaling_buf_idx = ((r0 * cfg_BN_C + c0) * cfg_BN_O + o0) * BS_R_R_scaled * BS_C_C_scaled * O * BS_O + (r21 * BS_C_C_scaled + c21) * O * BS_O + o21;

			for (int p = 0; p < cfg_pooling_size[layer]; p++)
			for (int q = 0; q < cfg_pooling_size[layer]; q++)
			{
				int pooling_in_idx = ((r0 * cfg_BN_C + c0) * cfg_BN_O + o0) * R * BS_R * BS_C * O * BS_O + ((r21 * cfg_pooling_size[layer] + p) * BS_C + (c21 * cfg_pooling_size[layer] + q)) * O * BS_O + o21;

			//	if (r0 == 0 && c0 == 2 && o0 == 0 && r21 == 0 && c21 == 0 && o21 == 0)
			//		printf("%.3f\n", pooling_in[pooling_in_idx]);

				if ((max + 1.0) < 1e-6 || (max - pooling_in[pooling_in_idx]) < 0)
					max = pooling_in[pooling_in_idx];
			}
			scaling_buffer[scaling_buf_idx] = max;

		//	if (r0 == 0 && c0 == 2 && o0 == 0 && r21 == 0 && c21 == 0 && o21 == 0)
		//		printf("scaling_buffer[%d]=%.3f\n", scaling_buf_idx, scaling_buffer[scaling_buf_idx]);
		}

	//	if (r0 == 0 && c0 == 2 && o0 == 0)
	//	{
	//		for (int rr = 0; rr < R * BS_R; rr++)
	//		for (int cc = 0; cc < BS_C; cc++)
	//		for (int oo = 0; oo < O * BS_O; oo++) 
	//		{
	//			int pooling_in_idx = ((r0 * cfg_BN_C + c0) * cfg_BN_O + o0) * R * BS_R * BS_C * O * BS_O + (rr * BS_C + cc) * O * BS_O + oo;
	//
	//			printf("%.3f\n", pooling_in[pooling_in_idx]);
	//		}
	//	}
	}

	// step 2: block merging (pooling_out<-scaling_buffer)
	// Attention: after block merging, number of sub-blocks may be larger than 
	// (cfg_BN_R, cfg_BN_C, cfg_BN_O)
	int next_BN_R_scaled = next_BN_R * cfg_pooling_size[layer];
	int next_BN_C_scaled = next_BN_C * cfg_pooling_size[layer];
	int next_BN_O_scaled = next_BN_O;

	for (int r0 = 0; r0 < next_BN_R_scaled; r0++)
	for (int c0 = 0; c0 < next_BN_C_scaled; c0++)
	for (int o0 = 0; o0 < next_BN_O_scaled; o0++) {
		for (int r21 = 0; r21 < BS_R_R_scaled; r21++)
		for (int c21 = 0; c21 < BS_C_C_scaled; c21++)
		for (int o21 = 0; o21 < O * BS_O; o21++) {
			int scaling_buf_idx = ((((r0 * next_BN_C_scaled + c0) * next_BN_O_scaled + o0) * BS_R_R_scaled + r21) * BS_C_C_scaled + c21) * O * BS_O + o21;
			int pooling_out_idx = (((r0 / cfg_pooling_size[layer]) * next_BN_C + (c0 / cfg_pooling_size[layer])) * next_BN_O + o0) * R * BS_R * BS_C * O * BS_O + (((r0 % cfg_pooling_size[layer]) * BS_R_R_scaled + r21) * BS_C + ((c0 % cfg_pooling_size[layer]) * BS_C_C_scaled + c21)) * O * BS_O + o21;

			pooling_out[pooling_out_idx] = scaling_buffer[scaling_buf_idx];
		}}
	
	free(scaling_buffer);
}

void zero_padding_block_wise(int layer, data_precision *out, data_precision *out_padded)
{
	const unsigned int cfg_BN_R = (cfg_img_row[layer + 1] + R * BS_R - 1) / (R * BS_R);
	const unsigned int cfg_BN_C = (cfg_img_col[layer + 1] + BS_C - 1) / BS_C;
	const unsigned int cfg_BN_O = (cfg_in_num[layer + 1] + O * BS_O - 1) / (O * BS_O);

//	printf("%s: cfg_BN_R=%d cfg_BN_C=%d cfg_BN_O=%d\n", __func__, cfg_BN_R, cfg_BN_C, cfg_BN_O);
//	fflush(stdout);

	// FIXME: for now, O*BS_O is equal to next layer's BS_I*I.
	for (int r0 = 0; r0 < cfg_BN_R; r0++)
	for (int c0 = 0; c0 < cfg_BN_C; c0++)
	for (int o0 = 0; o0 < cfg_BN_O; o0++)
	{
		for (int r21 = 0; r21 < R * BS_R + K - 1; r21++)
		for (int c1 = 0; c1 < BS_C + K - 1; c1++)
		for (int o21 = 0; o21 < O * BS_O; o21++) 
		{
			int out_padded_idx = (r0 * cfg_BN_C * cfg_BN_O + c0 * cfg_BN_O + o0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_O * O + (r21 * (BS_C + K - 1) + c1) * O * BS_O + o21;

			if ((r21 >= 0 && r21 < (K - 1) / 2) && (c1 >= 0 && c1 < (K - 1) / 2))
			{
				// part 1
				if (0 == r0 || 0 == c0)
					out_padded[out_padded_idx] = 0;
				else
				{
					int r21_p = r21 + (R * BS_R - ((K - 1) / 2));
					int c1_p = c1 + (BS_C - (K - 1) / 2);
					int out_idx = ((r0 - 1) * cfg_BN_C * cfg_BN_O + (c0 - 1) * cfg_BN_O + o0) * (R * BS_R) * BS_C * BS_O * O + (r21_p * BS_C + c1_p) * O * BS_O + o21;

					out_padded[out_padded_idx] = out[out_idx];
				}
			}
			else if ((r21 >= 0 && r21 < (K - 1) / 2) && (c1 >= (K - 1) / 2 && c1 < BS_C + (K - 1) / 2))
			{
				// part 2
				if (0 == r0) 
				{
					out_padded[out_padded_idx] = 0;
				}
				else
				{
					int r21_p = r21 + (R * BS_R - ((K - 1) / 2));
					int c1_p = c1 - (K - 1) / 2;
					int out_idx = ((r0 - 1) * cfg_BN_C * cfg_BN_O + c0 * cfg_BN_O + o0) * (R * BS_R) * BS_C * BS_O * O + (r21_p * BS_C + c1_p) * O * BS_O + o21;

					out_padded[out_padded_idx] = out[out_idx];
				}
			}
			else if ((r21 >= 0 && r21 < (K - 1) / 2) && (c1 >= BS_C + (K - 1) / 2 && c1 < BS_C + (K - 1))) 
			{
				// part 3
				if (0 == r0 || c0 == (cfg_BN_C - 1))
				{
					out_padded[out_padded_idx] = 0;
				}
				else
				{
					int r21_p = r21 + (R * BS_R - ((K - 1) / 2));
					int c1_p = c1 - (BS_C + ((K - 1) / 2));
					int out_idx = ((r0 - 1) * cfg_BN_C * cfg_BN_O + (c0 + 1) * cfg_BN_O + o0) * (R * BS_R) * BS_C * BS_O * O + (r21_p * BS_C + c1_p) * O * BS_O + o21;

					out_padded[out_padded_idx] = out[out_idx];
				}
			}
			else if ((r21 >= (K - 1) / 2 && r21 < (K - 1) / 2 + R * BS_R) && (c1 >= 0 && c1 < (K - 1) / 2)) 
			{
				// part 4
				if (c0 == 0) 
				{
					out_padded[out_padded_idx] = 0;
				}
				else 
				{
					int r21_p = r21 - ((K - 1) / 2);
					int c1_p = c1 + (BS_C - (K - 1) / 2);
					int out_idx = (r0 * cfg_BN_C * cfg_BN_O + (c0 - 1) * cfg_BN_O + o0) * (R * BS_R) * BS_C * BS_O * O + (r21_p * BS_C + c1_p) * O * BS_O + o21;

					out_padded[out_padded_idx] = out[out_idx];
				}
			}
			else if ((r21 >= (K - 1) / 2 && r21 < (K - 1) / 2 + R * BS_R) && (c1 >= (K - 1) / 2 && c1 < BS_C + (K - 1) / 2)) 
			{
				// part 5
				int r21_p = r21 - ((K - 1) / 2);
				int c1_p = c1 - (K - 1) / 2;
				int out_idx = (r0 * cfg_BN_C * cfg_BN_O + c0 * cfg_BN_O + o0) * (R * BS_R) * BS_C * BS_O * O + (r21_p * BS_C + c1_p) * O * BS_O + o21;

				out_padded[out_padded_idx] = out[out_idx];
			}
			else if ((r21 >= (K - 1) / 2 && r21 < (K - 1) / 2 + R * BS_R) && (c1 >= BS_C + (K - 1) / 2 && c1 < BS_C + (K - 1)))
			{
				// part 6
				if (c0 == cfg_BN_C - 1)
				{
					out_padded[out_padded_idx] = 0;
				}
				else 
				{
					int r21_p = r21 - ((K - 1) / 2);
					int c1_p = c1 - (BS_C + (K - 1) / 2);
					int out_idx = (r0 * cfg_BN_C * cfg_BN_O + (c0 + 1) * cfg_BN_O + o0) * (R * BS_R) * BS_C * BS_O * O + (r21_p * BS_C + c1_p) * O * BS_O + o21;

					out_padded[out_padded_idx] = out[out_idx];
				}
			}
			else if ((r21 >= (K - 1) / 2 + R * BS_R && r21 < R * BS_R + (K - 1)) && (c1 >= 0 && c1 < (K - 1) / 2))
			{
				// part 7
				if (r0 == cfg_BN_R - 1 || c0 == 0)
				{
					out_padded[out_padded_idx] = 0;
				}
				else
				{
					int r21_p = r21 - (R * BS_R + ((K - 1) / 2));
					int c1_p = c1 + BS_C - ((K - 1) / 2);
					int out_idx = ((r0 + 1) * cfg_BN_C * cfg_BN_O + (c0 - 1) * cfg_BN_O + o0) * (R * BS_R) * BS_C * BS_O * O + (r21_p * BS_C + c1_p) * O * BS_O + o21;

					out_padded[out_padded_idx] = out[out_idx];
				}
			}
			else if ((r21 >= (K - 1) / 2 + R * BS_R && r21 < R * BS_R + (K - 1)) && (c1 >= (K - 1) / 2 && c1 < BS_C + (K - 1) / 2)) 
			{
				// part 8
				if (r0 == cfg_BN_R - 1)
					out_padded[out_padded_idx] = 0;
				else
				{
					int r21_p = r21 - (R * BS_R + ((K - 1) / 2));
					int c1_p = c1 - (K - 1) / 2;
					int out_idx = ((r0 + 1) * cfg_BN_C * cfg_BN_O + c0 * cfg_BN_O + o0) * (R * BS_R) * BS_C * BS_O * O + (r21_p * BS_C + c1_p) * O * BS_O + o21;

					out_padded[out_padded_idx] = out[out_idx];
				}
			}
			else if ((r21 >= (K - 1) / 2 + R * BS_R && r21 < R * BS_R + (K - 1)) && (c1 >= BS_C + (K - 1) / 2 && c1 < BS_C + (K - 1))) 
			{
				// part 9
				if (r0 == (cfg_BN_R - 1) || c0 == (cfg_BN_C - 1))
					out_padded[out_padded_idx] = 0;
				else 
				{
					int r21_p = r21 - (R * BS_R + ((K - 1) / 2));
					int c1_p = c1 - (BS_C + (K - 1) / 2);
					int out_idx = ((r0 + 1) * cfg_BN_C * cfg_BN_O + (c0 + 1) * cfg_BN_O + o0) * (R * BS_R) * BS_C * BS_O * O + (r21_p * BS_C + c1_p) * O * BS_O + o21;

					out_padded[out_padded_idx] = out[out_idx];
				}
			}
		}}
}

void print_out_block_wise_padded(int layer, data_precision *out_block_wise_padded)
{
	printf("%s\n", __func__);
	FILE *f;

	f = fopen("pad_bw.txt", "w");

//	fprintf(f, "layer=%d\n", layer);

	const unsigned int cfg_BN_R = ((cfg_img_row[layer] / cfg_pooling_size[layer]) + R * BS_R - 1) / (R * BS_R);
	const unsigned int cfg_BN_C = ((cfg_img_col[layer] / cfg_pooling_size[layer]) + BS_C - 1) / BS_C;
	const unsigned int cfg_BN_O = (cfg_out_num[layer] + O * BS_O - 1) / (O * BS_O);

	for (int r0 = 0; r0 < cfg_BN_R; r0++)
	for (int c0 = 0; c0 < cfg_BN_C; c0++)
	for (int i0 = 0; i0 < cfg_BN_O; i0++)
	{
		for (int r_p = 0; r_p < R * BS_R + K - 1; r_p++)
		for (int c_q = 0; c_q < BS_C + K - 1; c_q++)
		for (int i12 = 0; i12 < BS_I * I; i12++)
		{
			int out_block_wise_padded_idx = ((r0 * cfg_BN_C + c0) * cfg_BN_O + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (r_p * (BS_C + K - 1) + c_q) * BS_I * I + i12;

	//		if (r0 == 0 && c0 == 0 && i0 == 0)
			{
			//	printf("%.3f\n", out_block_wise_padded[out_block_wise_padded_idx]);
				fprintf(f, "%.3f\n", out_block_wise_padded[out_block_wise_padded_idx]);
			}
		//	if (out_block_wise_padded_idx == 360928)
		//	{
		//		printf("r_p=%d,c_q=%d,i12=%d\n", r_p, c_q, i12);
		//	}
		}
	}

	fclose(f);


	FILE *f1, *f2;
	int err = 0;

	if ((f1 = fopen("pad.txt", "r")) == NULL)
	{
		printf("Open file %s for reading fails.\n", "pad.txt");
		exit(1);
	}
	if ((f2 = fopen("pad_bw.txt", "r")) == NULL)
	{
		printf("Open file %s for reading fails.\n", "pad_bw.txt");
		exit(1);
	}
	for (int r0 = 0; r0 < cfg_BN_R; r0++)
	for (int c0 = 0; c0 < cfg_BN_C; c0++)
	for (int i0 = 0; i0 < cfg_BN_O; i0++)
	{
		for (int r_p = 0; r_p < R * BS_R + K - 1; r_p++)
		for (int c_q = 0; c_q < BS_C + K - 1; c_q++)
		for (int i12 = 0; i12 < BS_I * I; i12++)
		{
			data_precision out0, out1;

			int r = r0 * R * BS_R + r_p;
			int c = c0 * BS_C + c_q;
			int i = i0 * BS_I * I + i12;

			int out_idx = ((r0 * R * BS_R + r_p) * (cfg_BN_C * BS_C + K - 1) + c0 * BS_C + c_q) * cfg_BN_O * BS_I * I + (i0 * BS_I * I + i12);
			int out_idx_bw = ((r0 * cfg_BN_C + c0) * cfg_BN_O + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (r_p * (BS_C + K - 1) + c_q) * BS_I * I + i12;

			fscanf(f1, "%f", &out0);
			fscanf(f2, "%f", &out1);

			if (fabs(out0 - out1) > 0.01)
			{
				if (err < 10)
				{
					printf("out[%d]=%.3f, out_padded[%d]=%.3f\n", out_idx, out0, out_idx_bw, out1);
				}
				else if (err == 10)
				{
					printf("...\n");
				}
				err++;
			}
		}
	}

	fclose(f1);
	fclose(f2);
}

void print_conv_out_block_wise(int layer, data_precision *conv_out_block_wise)
{
	printf("%s\n", __func__);

	const unsigned int cfg_BN_R = (cfg_img_row[layer] + R * BS_R - 1) / (R * BS_R);
	const unsigned int cfg_BN_C = (cfg_img_col[layer] + BS_C - 1) / BS_C;
	const unsigned int cfg_BN_O = (cfg_out_num[layer] + O * BS_O - 1) / (O * BS_O);

	FILE *f;

	f = fopen("conv_out_block_wise.txt", "w");

//	printf("layer=%d\n", layer);
//	fprintf(f, "layer=%d\n", layer);
//	block_wise_reformat_output(layer, pooling_out_block_wise, out);

	for (int r0 = 0; r0 < cfg_BN_R; r0++)
	for (int c0 = 0; c0 < cfg_BN_C; c0++)
	for (int o0 = 0; o0 < cfg_BN_O; o0++)
	{
		for (int rr = 0; rr < R * BS_R; rr++)
		for (int cc = 0; cc < BS_C; cc++)
		for (int oo = 0; oo < O * BS_O; oo++)
		{
			int r = r0 * R * BS_R + rr;
			int c = c0 * BS_C + cc;
			int o = o0 * O * BS_O + oo;

			int out_idx = ((r0 * cfg_BN_C + c0) * cfg_BN_O + o0) * R * BS_R * BS_C * BS_O * O + (rr * BS_C + cc) * O * BS_O + oo;

	//		if (r0 == cfg_BN_R - 1 && c0 == cfg_BN_C - 1 && o0 == cfg_BN_O - 1)
			{
			//	if (r < cfg_img_row[layer] && c < cfg_img_col[layer] && o < cfg_out_num[layer])
				{
				//	printf("%.3f\n", conv_out_block_wise[out_idx]);
					fprintf(f, "%.3f\n", conv_out_block_wise[out_idx]);
				}
			}
		}
	}

	fclose(f);

	FILE *f1, *f2;
	int err = 0;

	if ((f1 = fopen("conv_out.txt", "r")) == NULL)
	{
		printf("Open file %s for reading fails.\n", "conv_out.txt");
		exit(1);
	}
	if ((f2 = fopen("conv_out_block_wise.txt", "r")) == NULL)
	{
		printf("Open file %s for reading fails.\n", "conv_out_block_wise.txt");
		exit(1);
	}

	for (int r0 = 0; r0 < cfg_BN_R; r0++)
	for (int c0 = 0; c0 < cfg_BN_C; c0++)
	for (int o0 = 0; o0 < cfg_BN_O; o0++) 
	{
		for (int r1 = 0; r1 < BS_R; r1++)
		for (int c1 = 0; c1 < BS_C; c1++)
		for (int o1 = 0; o1 < BS_O; o1++)
		{
			for (int r2 = 0; r2 < R; r2++)
			for (int o2 = 0; o2 < O; o2++) 
			{
				int r = r0 * R * BS_R + r2 * BS_R + r1;
				int c = c0 * BS_C + c1;
				int o = o0 * O * BS_O + o2 * BS_O + o1;

				data_precision out0, out1;

				int out_idx = r * cfg_img_col[layer] * cfg_out_num[layer] + c * cfg_out_num[layer] + o;
				int out_idx_bw = ((r0 * cfg_BN_C + c0) * cfg_BN_O + o0) * R * BS_R * BS_C * O * BS_O + (r2 * BS_R * BS_C + r1 * BS_C + c1) * O * BS_O + o2 * BS_O + o1; // [c0][r0][o0][c2][c1][r1][o2][o1]

				fscanf(f1, "%f", &out0);
				fscanf(f2, "%f", &out1);

	//			if (out_idx == 3211968)
	//			{
	//				printf("%s:r=%d,c=%d,o=%d, conv_out[%d]=%.3f\n", __func__, r, c, o, out_idx, out0);
	//			}

				if (fabs(out0 - out1) > 0.01)
				{
					if (err < 10)
					{
						printf("conv_out[%d]=%.3f, conv_out_block_wise[%d]=%.3f\n", out_idx, out0, out_idx_bw, out1);
					}
					else if (err == 10)
					{
						printf("...\n");
					}
					err++;
				}
			}
		}
	}
	
	fclose(f1);
	fclose(f2);
}

void print_pooling_out_block_wise(int layer, data_precision *pooling_out_block_wise)
{
	printf("%s\n", __func__);

	const unsigned int pooled_img_row = cfg_img_row[layer] / cfg_pooling_size[layer];
	const unsigned int pooled_img_col = cfg_img_col[layer] / cfg_pooling_size[layer];

	const unsigned int cfg_BN_R = (pooled_img_row + R * BS_R - 1) / (R * BS_R);
	const unsigned int cfg_BN_C = (pooled_img_col + BS_C - 1) / BS_C;
	const unsigned int cfg_BN_O = (cfg_out_num[layer] + O * BS_O - 1) / (O * BS_O);

	FILE *f;

	f = fopen("pooling_out_block_wise.txt", "w");

//	printf("layer=%d\n", layer);
//	fprintf(f, "layer=%d\n", layer);
//	block_wise_reformat_output(layer, pooling_out_block_wise, out);

	for (int r0 = 0; r0 < cfg_BN_R; r0++)
	for (int c0 = 0; c0 < cfg_BN_C; c0++)
	for (int o0 = 0; o0 < cfg_BN_O; o0++)
	{
		for (int rr = 0; rr < R * BS_R; rr++)
		for (int cc = 0; cc < BS_C; cc++)
		for (int oo = 0; oo < O * BS_O; oo++)
		{
			int r = r0 * R * BS_R + rr;
			int c = c0 * BS_C + cc;
			int o = o0 * O * BS_O + oo;

			int out_idx = ((r0 * cfg_BN_C + c0) * cfg_BN_O + o0) * R * BS_R * BS_C * BS_O * O + (rr * BS_C + cc) * O * BS_O + oo;

	//		if (r0 == cfg_BN_R - 1 && c0 == cfg_BN_C - 1 && o0 == cfg_BN_O - 1)
			{
			//	if (r < cfg_img_row[layer] && c < cfg_img_col[layer] && o < cfg_out_num[layer])
				{
				//	printf("%.3f\n", conv_out_block_wise[out_idx]);
					fprintf(f, "%.3f\n", pooling_out_block_wise[out_idx]);
				}
			}
		}
	}

	fclose(f);

	FILE *f1, *f2;
	int err = 0;

	if ((f1 = fopen("pooling_out.txt", "r")) == NULL)
	{
		printf("Open file %s for reading fails.\n", "pooling_out.txt");
		exit(1);
	}
	if ((f2 = fopen("pooling_out_block_wise.txt", "r")) == NULL)
	{
		printf("Open file %s for reading fails.\n", "pooling_out_block_wise.txt");
		exit(1);
	}

	for (int r0 = 0; r0 < cfg_BN_R; r0++)
	for (int c0 = 0; c0 < cfg_BN_C; c0++)
	for (int o0 = 0; o0 < cfg_BN_O; o0++) 
	{
		for (int r1 = 0; r1 < BS_R; r1++)
		for (int c1 = 0; c1 < BS_C; c1++)
		for (int o1 = 0; o1 < BS_O; o1++)
		{
			for (int r2 = 0; r2 < R; r2++)
			for (int o2 = 0; o2 < O; o2++) 
			{
				int r = r0 * R * BS_R + r2 * BS_R + r1;
				int c = c0 * BS_C + c1;
				int o = o0 * O * BS_O + o2 * BS_O + o1;

				data_precision out0, out1;

				int out_idx = r * pooled_img_col * cfg_out_num[layer] + c * cfg_out_num[layer] + o;
				int out_idx_bw = ((r0 * cfg_BN_C + c0) * cfg_BN_O + o0) * R * BS_R * BS_C * O * BS_O + (r2 * BS_R * BS_C + r1 * BS_C + c1) * O * BS_O + o2 * BS_O + o1; // [c0][r0][o0][c2][c1][r1][o2][o1]

				fscanf(f1, "%f", &out0);
				fscanf(f2, "%f", &out1);

	//			if (out_idx == 3211968)
	//			{
	//				printf("%s:r=%d,c=%d,o=%d, conv_out[%d]=%.3f\n", __func__, r, c, o, out_idx, out0);
	//			}

				if (fabs(out0 - out1) > 0.1)
				{
					if (err < 10)
					{
						printf("pooling_out[%d]=%.3f, pooling_out_block_wise[%d]=%.3f\n", out_idx, out0, out_idx_bw, out1);
					}
					else if (err == 10)
					{
						printf("...\n");
					}
					err++;
				}
			}
		}
	}
	
	fclose(f1);
	fclose(f2);
}

void cnn_specified_block_wise(
		data_precision* out,
		data_precision* in,
		data_precision* weights
		)
{
	data_precision *weight = weights;

	// block wise reformat for input and weight
//	unsigned int num_elem_weight = cfg_out_num[0] * cfg_kernel[0] * cfg_kernel[0] * cfg_in_num[0];
	unsigned int num_elem_input_fm = (cfg_img_row[0] + cfg_kernel[0] - 1) * 
		(cfg_img_col[0] + cfg_kernel[0] - 1) * cfg_in_num[0];
//	unsigned int num_elem_output_fm = cfg_out_num[0] * cfg_img_row[0] * cfg_img_col[0];

	data_precision *out_block_wise_padded = (data_precision *)malloc(num_elem_input_fm * sizeof(data_precision));

	for (int layer = 0; layer < NUM_CONV; layer++) 
	{
		int num_elem_input_fm_bw = cfg_BN_R[layer] * cfg_BN_C[layer] * cfg_BN_I[layer] * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I;
		int num_elem_weight = cfg_out_num[layer] * cfg_kernel[layer] * cfg_kernel[layer] * cfg_in_num[layer];
		int num_elem_weight_bw = cfg_BN_O[layer] * cfg_BN_I[layer] * O * K * K * BS_I * I;
		int	num_elem_output_fm_bw = cfg_BN_R[layer] * cfg_BN_C[layer] * cfg_BN_O[layer] * R * BS_R * BS_C * O * BS_O;
		int num_elem_output_fm_bw_scaled = (cfg_BN_R[layer] + cfg_pooling_size[layer] - 1 / cfg_pooling_size[layer]) * (cfg_BN_C[layer] + cfg_pooling_size[layer] - 1 / cfg_pooling_size[layer]) * cfg_BN_O[layer] * R * BS_R * BS_C * O * BS_O;

		data_precision *in_block_wise = (data_precision *)malloc(num_elem_input_fm_bw * sizeof(data_precision));
		data_precision *weight_block_wise = (data_precision *)malloc(num_elem_weight_bw * sizeof(data_precision));
		data_precision *conv_out_block_wise = (data_precision *)malloc(num_elem_output_fm_bw * sizeof(data_precision));
		data_precision *relu_out_block_wise = (data_precision *)malloc(num_elem_output_fm_bw * sizeof(data_precision));
		data_precision *pooling_out_block_wise = (data_precision *)malloc(num_elem_output_fm_bw_scaled * sizeof(data_precision));
		data_precision *out_block_wise = (data_precision *)malloc(num_elem_output_fm_bw_scaled * sizeof(data_precision));

		if (0 == layer)
		{
			block_wise_reformat_input(layer, in, in_block_wise);
		} 
		else
		{
			memcpy(in_block_wise, out_block_wise_padded, sizeof(data_precision) * num_elem_input_fm_bw);
		}
		block_wise_reformat_weights(layer, weight, weight_block_wise);

		printf("%s: layer %d conv\n", __func__, layer);
		fflush(stdout);

		// convolutional layer
		conv_specified_block_wise(layer, conv_out_block_wise, in_block_wise, weight_block_wise);

#if DEBUG_CONV
		if (layer == TEST_LAYER)
			print_conv_out_block_wise(layer, conv_out_block_wise);
#endif

		printf("%s: layer %d relu\n", __func__, layer);
		fflush(stdout);

		// relu
		relu_block_wise(layer, conv_out_block_wise, relu_out_block_wise);

		// pooling
		if (cfg_pooling_size[layer] > 1)
		{
			printf("%s: layer %d pooling\n", __func__, layer);
			fflush(stdout);

			pooling_block_wise(layer, relu_out_block_wise, pooling_out_block_wise);

#if DEBUG_POOLING
			if (layer == TEST_LAYER)
				print_pooling_out_block_wise(layer, pooling_out_block_wise);
#endif
		}

		weight += num_elem_weight;

		// Assign output to the input for the next layer
		if (layer < NUM_CONV - 1)
		{
			int next_elem_input_fm_bw = cfg_BN_R[layer + 1] * cfg_BN_C[layer + 1] * cfg_BN_I[layer + 1] * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I;

	//		data_precision *out_padded_block_wise = (data_precision *)malloc(next_elem_input_fm * sizeof(data_precision));
			out_block_wise_padded = (data_precision *)realloc(out_block_wise_padded, next_elem_input_fm_bw * sizeof(data_precision));

			printf("%s: layer %d zero-padding\n", __func__, layer);
			fflush(stdout);

			if (cfg_pooling_size[layer] > 1)
				zero_padding_block_wise(layer, pooling_out_block_wise, out_block_wise_padded);
			else
				zero_padding_block_wise(layer, relu_out_block_wise, out_block_wise_padded);

#if DEBUG_ZERO_PADDING
			if (layer == TEST_LAYER)
				print_out_block_wise_padded(layer, out_block_wise_padded);
#endif

		}
		if (layer == NUM_CONV - 1)
		{
			if (cfg_pooling_size[layer] > 1)
				block_wise_reformat_output(layer, pooling_out_block_wise, out);
			else
				block_wise_reformat_output(layer, relu_out_block_wise, out);
		}

		free(in_block_wise);
		free(weight_block_wise);
		free(conv_out_block_wise);
		free(relu_out_block_wise);
		free(pooling_out_block_wise);
		free(out_block_wise);
	}

	free(out_block_wise_padded);
}

void cnn_gold(
       data_precision *weights,
       data_precision *input_fm,
       data_precision *output_fm
		)
{
    printf("CNN golden ... \n");
    fflush(stdout);

	unsigned int num_elem_input_fm = cfg_in_num[0] * (cfg_img_row[0] + cfg_kernel[0] - 1) * (cfg_img_col[0] + cfg_kernel[0] - 1);
	unsigned int num_elem_output_fm = cfg_out_num[NUM_CONV - 1] * cfg_img_row[NUM_CONV - 1] * cfg_img_col[NUM_CONV - 1];

	data_precision *output_fm_specified = (data_precision *)malloc(num_elem_output_fm * sizeof(data_precision));
	data_precision *output_fm_specified_block_wise = (data_precision *)malloc(num_elem_output_fm * sizeof(data_precision));

	cnn(output_fm, input_fm, weights);

//	cnn_specified(output_fm_specified, input_fm, weights);

	cnn_specified_block_wise(output_fm_specified_block_wise, input_fm, weights);

	memcpy(output_fm, output_fm_specified_block_wise, sizeof(data_precision) * num_elem_output_fm);

	int err = 0;
	for (int i = 0; i < num_elem_output_fm; i++) 
	{
	//	if (fabs(output_fm[i] - output_fm_specified_block_wise[i])/* / output_fm[i]*/ > /*0.01*/ 20) 
	//	{
	//		if (err < 10) printf("out[%d]=%f, out1[%d]=%f\n", i, output_fm[i], i, output_fm_specified_block_wise[i]);
	//		else if (err == 10) printf("...\n");
	//		err++;
	//	}
		if (fabs(output_fm[i] - output_fm_specified_block_wise[i])/* / output_fm[i]*/ > 0.01) 
		{
			printf("out[%d]=%f, out1[%d]=%f\n", i, output_fm[i], i, output_fm_specified_block_wise[i]);
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
