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



//; my $cfg_root = param_tree_define("dummy", "systolic_cfg.xml", "item_for_array");

//; my $expr_ub_h = $cfg_root->{Host_Params}->{IN_UB};
//; my $expr_ub_v = $cfg_root->{Host_Params}->{WT_UB};
//; my $expr_ub_o = $cfg_root->{Host_Params}->{OUT_UB};

//#include "systolic_params.h"

#include "__merlin_systolic_util.h"
#include "block_wise_reformat.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#if (TEST_CNN == 0)

void conv(
	   data_precision *out,
	   data_precision *in,
	   data_precision *weight
	   )
{
    // Initialize output
//	for (int i = 0; i < (BN_I * BS_I * PE_ROWS); i++) 
	for (int i = 0; i < M; i++) 
	//	for (int j = 0; j < (BN_J * BS_J * PE_COLS); j++) 
		for (int j = 0; j < N; j++) 
		{
			// out[i][j]
		//	int out_idx = i * (BN_J * BS_J * PE_COLS) + j;
			int out_idx = i * N + j;
			out[out_idx] = 0;
		}
    
//	for (int i = 0; i < (BN_I * BS_I * PE_ROWS); i++)
	for (int i = 0; i < M; i++)
//	for (int j = 0; j < (BN_J * BS_J * PE_COLS); j++)
	for (int j = 0; j < N; j++)
	{
		// out[i][j]
	//	int out_idx = i * (BN_J * BS_J * PE_COLS) + j;
		int out_idx = i * N + j;
		out[out_idx] = 0;
	//	for (int k = 0; k < (BN_K * BS_K * PE_DSIZE); k++)
		for (int k = 0; k < L; k++)
		{
		//	int in_idx = i * (BN_K * BS_K * PE_DSIZE) + k;
			int in_idx = i * L + k;
		//	int w_idx = j * (BN_K * BS_K * PE_DSIZE) + k;
			int w_idx = j * L + k;

			out[out_idx] += in[in_idx] * weight[w_idx];
		}
	}
}


/*
 * Functions for block wise
 * */
void conv_block_wise(
		data_precision *out_block_wise,
		data_precision *in_block_wise,
		data_precision *weight_block_wise
		)
{
	int num_elem_output_bw = `$expr_ub_o`;
	memset(out_block_wise, 0, sizeof(data_precision) * num_elem_output_bw);

	for (int i0 = 0; i0 < BN_M; i0++)
	for (int j0 = 0; j0 < BN_N; j0++)
	for (int k0 = 0; k0 < BN_L; k0++) 
		for (int i1 = 0; i1 < BS_M; i1++)
		for (int j1 = 0; j1 < BS_N; j1++)
		for (int k1 = 0; k1 < BS_L; k1++)
			for (int i2 = 0; i2 < PE_ROWS; i2++)
			for (int j2 = 0; j2 < PE_COLS; j2++) 
			for (int k2 = 0; k2 < PE_DSIZE; k2++) {
				// [i0][j0][i2][i1][j2][j1]
				int out_idx_bw = (i0 * BN_N + j0) * BS_M * PE_ROWS * BS_N * PE_COLS + (i2 * BS_M + i1) * BS_N * PE_COLS + j2 * BS_N + j1; 
				// [i0][k0][i2][i1][k1][k2]
				int in_idx_bw = (i0 * BN_L + k0) * BS_M * PE_ROWS * BS_L * PE_DSIZE + (i2 * BS_M + i1) * BS_L * PE_DSIZE + k1 * PE_DSIZE + k2; 
				// [j0][k0][j2][j1][k1][k2]
				int w_idx_bw = (j0 * BN_L + k0) * BS_N * PE_COLS * BS_L * PE_DSIZE + (j2 * BS_N + j1) * BS_L * PE_DSIZE + k1 * PE_DSIZE + k2; 

				out_block_wise[out_idx_bw] += in_block_wise[in_idx_bw] * weight_block_wise[w_idx_bw];

			}
}

#else

void conv(
	   data_precision *out,
	   data_precision *in,
	   data_precision *weight
	   )
{
    // Initialize output
    // TODO: Initialize with bias
	for (int r = 0; r < IMG_ROW; r++) {
		for (int c = 0; c < IMG_COL; c++) {
			for (int o = 0; o < IMG_OUT_NUM; o++) {
				// out[r][c][o]
				out[r * (IMG_COL * IMG_OUT_NUM) + c * IMG_OUT_NUM + o] = 0;
			}
		}
	}
    
  for (int o = 0; o < IMG_OUT_NUM; o++)
  for (int i = 0; i < IMG_IN_NUM; i++)
  for (int r = 0; r < IMG_ROW; r++)
  for (int c = 0; c < IMG_COL; c++)
  for (int p = 0; p < K; p++)
  for (int q = 0; q < K; q++) {
	// out[r][c][o] = in[r+p][c+q][i] * weight[o][p][q][i]
		int out_idx = r * (IMG_COL * IMG_OUT_NUM) + c * IMG_OUT_NUM + o;
		int in_idx = (r + p) * (IMG_COL + K - 1) * IMG_IN_NUM + (c + q) * IMG_IN_NUM + i;
		int w_idx = o * (IMG_IN_NUM * K * K) + p * (IMG_IN_NUM * K) + q * IMG_IN_NUM + i;

        out[out_idx] += in[in_idx] * weight[w_idx];
    }
}

/*
 * Functions for block wise
 * */
#if (CNN_PE_ROW_SWITCH == 0)
void conv_block_wise(
		data_precision *out_block_wise,
		data_precision *in_block_wise,
		data_precision *weight_block_wise
		)
{
	int num_elem_output_bw = `$expr_ub_o`;
	memset(out_block_wise, 0, sizeof(data_precision) * num_elem_output_bw);

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
				// [r0][c0][o0][r2][r1][c1][o2][o1]
				int out_idx_bw = ((r0 * BN_C + c0) * BN_O + o0) * R * BS_R * BS_C * O * BS_O + (r2 * BS_R * BS_C + r1 * BS_C + c1) * O * BS_O + o2 * BS_O + o1; 
				// [r0][c0][i0][r2*BS_R+r1+p][c1+q][i1][i2]
				int in_idx_bw = (((((r0 * BN_C + c0) * BN_I + i0) * (R * BS_R + K - 1) + (r2 * BS_R + r1 + p)) * (BS_C + K - 1) + (c1 + q)) * BS_I + i1) * I + i2;
				// [o0][i0][o2][o1][p][q][i1][i2]
				int w_idx_bw = (o0 * BN_I + i0) * BS_O * O * K * K * BS_I * I + (((o2 * BS_O + o1) * K + p) * K + q) * BS_I * I + i1 * I + i2;

				out_block_wise[out_idx_bw] += in_block_wise[in_idx_bw] * weight_block_wise[w_idx_bw];

			}}}
}
#else

void conv_block_wise(
		data_precision *out_block_wise,
		data_precision *in_block_wise,
		data_precision *weight_block_wise
		)
{
	int num_elem_output_bw = `$expr_ub_o`;
	memset(out_block_wise, 0, sizeof(data_precision) * num_elem_output_bw);

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
			for (int c2 = 0; c2 < C; c2++)
			for (int o2 = 0; o2 < O; o2++) 
			for (int i2 = 0; i2 < I; i2++) {
				// [r0][c0][o0][r2][r1][c1][o2][o1]
				int out_idx_bw = ((r0 * BN_C + c0) * BN_O + o0) * BS_R * C * BS_C * O * BS_O + (r1 * C * BS_C + c2 * BS_C + c1) * O * BS_O + o2 * BS_O + o1; 
				// [r0][c0][i0][r2*BS_R+r1+p][c1+q][i1][i2]
				int in_idx_bw = (((((r0 * BN_C + c0) * BN_I + i0) * (BS_R + K - 1) + (r1 + p)) * (C * BS_C + K - 1) + (c2 * BS_C + c1 + q)) * BS_I + i1) * I + i2;
				// [o0][i0][o2][o1][p][q][i1][i2]
				int w_idx_bw = (o0 * BN_I + i0) * BS_O * O * K * K * BS_I * I + (((o2 * BS_O + o1) * K + p) * K + q) * BS_I * I + i1 * I + i2;

				out_block_wise[out_idx_bw] += in_block_wise[in_idx_bw] * weight_block_wise[w_idx_bw];

			}}}
}

#endif // CNN_PE_ROW_SWITCH

#endif // TEST_CNN
