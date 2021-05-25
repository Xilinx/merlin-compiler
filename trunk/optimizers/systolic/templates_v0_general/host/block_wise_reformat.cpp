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



#include <iostream>

#include "block_wise_reformat.h"


//#include <assert.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <math.h>


#if (TEST_CNN == 1)

#if (CNN_PE_ROW_SWITCH == 0)
void block_wise_reformat_input(data_precision *input, data_precision *input_block_wise)
{
	int addr_in, addr_in_bw;

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

					// in1[r0][c0][i0][r2*BS_R+r1+p][c1+q][i1[i2]]=in0[r0*R*BS_R+r2*BS_R+r1+p][c0*BS_C+c1+q][i0][i1][i2]
					addr_in = ((r + p) * (IMG_COL + K - 1) + (c + q)) * IMG_IN_NUM + i;
					addr_in_bw = (((((r0 * BN_C + c0) * BN_I + i0) * (R * BS_R + K - 1) + (r2 * BS_R + r1 + p)) * (BS_C + K - 1) + (c1 + q)) * BS_I + i1) * I + i2;

				//	if (r < IMG_ROW && c < IMG_COL && i < IMG_IN_NUM)
					if ((r + p) < (IMG_ROW + K - 1) && (c + q) < (IMG_COL + K - 1) && i < IMG_IN_NUM)
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

#else

void block_wise_reformat_input(data_precision *input, data_precision *input_block_wise)
{
	int addr_in, addr_in_bw;

	for (int r0 = 0; r0 < BN_R; r0++)
	for (int c0 = 0; c0 < BN_C; c0++)
    for (int i0 = 0; i0 < BN_I; i0++) {
        for (int p = 0; p < K; p++)
        for (int q = 0; q < K; q++)
        for (int i1 = 0; i1 < BS_I; i1++)
		for (int r1 = 0; r1 < BS_R; r1++)
		for (int c1 = 0; c1 < BS_C; c1++) {
				for (int c2 = 0; c2 < C; c2++)
				for (int i2 = 0; i2 < I; i2++) {

					int r = r0 * BS_R + r1;
					int c = c0 * BS_C * C + c2 * BS_C + c1;
					int i = (i0 * BS_I + i1) * I + i2;

					// in1[r0][c0][i0][r2*BS_R+r1+p][c1+q][i1[i2]]=in0[r0*R*BS_R+r2*BS_R+r1+p][c0*BS_C+c1+q][i0][i1][i2]
					addr_in = ((r + p) * (IMG_COL + K - 1) + (c + q)) * IMG_IN_NUM + i;
					addr_in_bw = (((((r0 * BN_C + c0) * BN_I + i0) * (BS_R + K - 1) + (r1 + p)) * (BS_C * C + K - 1) + (c2 * BS_C + c1 + q)) * BS_I + i1) * I + i2;

				//	if (r < IMG_ROW && c < IMG_COL && i < IMG_IN_NUM)
					if ((r + p) < (IMG_ROW + K - 1) && (c + q) < (IMG_COL + K - 1) && i < IMG_IN_NUM)
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
#endif // CNN_PE_ROW_SWITCH

void block_wise_reformat_weight(data_precision *weight, data_precision *weight_block_wise)
{
	int addr_w, addr_w_bw;

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

			addr_w = ((o * K + p) * K + q) * IMG_IN_NUM + i;
			addr_w_bw = (o0 * BN_I + i0) * BS_O * O * K * K * BS_I * I + (((o2 * BS_O + o1) * K + p) * K + q) * BS_I * I + i1 * I + i2;

		//	printf("addr_w=%d, addr_w_bw=%d\n", addr_w, addr_w_bw);
		//	fflush(stdout);

			// w1[o0][i0][o2][o1][p][q][i1][i2] = w0[o0][o2][o1][p][q][i0][i1][i2]
			if (o < IMG_OUT_NUM && i < IMG_IN_NUM) 
			{
				weight_block_wise[addr_w_bw] = weight[addr_w];
			}
			else
			{
				weight_block_wise[addr_w_bw] = 0;
			}
	}}}
}

#if (CNN_PE_ROW_SWITCH == 0)
void block_wise_reformat_output(data_precision *output_block_wise, data_precision *output)
{
	int addr_out_bw, addr_out;

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
				addr_out = (r * IMG_COL + c) * IMG_OUT_NUM + o;

				if (r < IMG_ROW && c < IMG_COL && o < IMG_OUT_NUM) {
					output[addr_out] = output_block_wise[addr_out_bw];
				}
		}}}
}

#else

void block_wise_reformat_output(data_precision *output_block_wise, data_precision *output)
{
	int addr_out_bw, addr_out;

	for (int r0 = 0; r0 < BN_R; r0++)
	for (int r1 = 0; r1 < BS_R; r1++) {
		for (int c0 = 0; c0 < BN_C; c0++)
		for (int c2 = 0; c2 < C; c2++) 
		for (int c1 = 0; c1 < BS_C; c1++) {
			for (int o0 = 0; o0 < BN_O; o0++)
			for (int o2 = 0; o2 < O; o2++)
			for (int o1 = 0; o1 < BS_O; o1++) {

				int r = r0 * BS_R + r1;
				int c = (c0 * C + c2) * BS_C + c1;
				int o = (o0 * O + o2) * BS_O + o1;
				// out0[r0][r2][r1][c0][c1][o0][o2][o1] = out2[r0][c0][o0][r2][r1][c1][o2][o1]
				addr_out_bw = ((r0 * BN_C + c0) * BN_O + o0) * BS_R * C * BS_C * O * BS_O + (r1 * C * BS_C + c2 * BS_C + c1) * O * BS_O + o2 * BS_O + o1;
				addr_out = (r * IMG_COL + c) * IMG_OUT_NUM + o;

				if (r < IMG_ROW && c < IMG_COL && o < IMG_OUT_NUM) {
					output[addr_out] = output_block_wise[addr_out_bw];
				}
		}}}
}
#endif // CNN_PE_ROW_SWITCH

// address mapping (out0->out1, in DDR for partial validation)

#if (CNN_PE_ROW_SWITCH == 0)
void data_reorganization(float *out0, float *output_fm_block_wise)
{
	for (int r0 = 0; r0 < BN_R; r0++)
	for (int c0 = 0; c0 < BN_C; c0++)
	for (int o0 = 0; o0 < BN_O; o0++) {
		for (int r2 = 0; r2 < R; r2++)
		for (int r1 = 0; r1 < BS_R; r1++)
		for (int c1 = 0; c1 < BS_C; c1++)
		for (int o2 = 0; o2 < O; o2++)
		for (int o1 = 0; o1 < BS_O; o1++) {
			int out0_idx = (r0 * BN_C * BN_O + c0 * BN_O + o0) * R * BS_R * BS_C * BS_O * O + (r2 * BS_R * BS_C + r1 * BS_C + c1) * BS_O * O + o1 * O + o2;
			int out1_idx = (r0 * BN_C * BN_O + c0 * BN_O + o0) * R * BS_R * BS_C * BS_O * O + (r2 * BS_R * BS_C + r1 * BS_C + c1) * O * BS_O + o2 * BS_O + o1;

			output_fm_block_wise[out1_idx] = out0[out0_idx];

		}}
}

#else

void data_reorganization(float *out0, float *output_fm_block_wise)
{
	for (int r0 = 0; r0 < BN_R; r0++)
	for (int c0 = 0; c0 < BN_C; c0++)
	for (int o0 = 0; o0 < BN_O; o0++) {
		for (int r1 = 0; r1 < BS_R; r1++)
		for (int c2 = 0; c2 < C; c2++)
		for (int c1 = 0; c1 < BS_C; c1++)
		for (int o2 = 0; o2 < O; o2++)
		for (int o1 = 0; o1 < BS_O; o1++) {
		//	int out0_idx = (r0 * BN_C * BN_O + c0 * BN_O + o0) * BS_R * C * BS_C * BS_O * O + (r1 * BS_C * C + c2 * BS_C + c1) * BS_O * O + o1 * O + o2;
			int out0_idx = (r0 * BN_C * BN_O + c0 * BN_O + o0) * BS_R * C * BS_C * BS_O * O + (c2 * BS_R * BS_C + r1 * BS_C + c1) * BS_O * O + o1 * O + o2;
			int out1_idx = (r0 * BN_C * BN_O + c0 * BN_O + o0) * BS_R * C * BS_C * BS_O * O + (r1 * BS_C * C + c2 * BS_C + c1) * O * BS_O + o2 * BS_O + o1;

			output_fm_block_wise[out1_idx] = out0[out0_idx];

		}}
}
#endif // CNN_PE_ROW_SWITCH


#else // MATMUL

void block_wise_reformat_input(data_precision *input, data_precision *input_block_wise)
{
	int in_idx, in_bw_idx;

	for (int i0 = 0; i0 < BN_M; i0++)
		for (int k0 = 0; k0 < BN_L; k0++) {
			for (int i1 = 0; i1 < BS_M; i1++)
		for (int k1 = 0; k1 < BS_L; k1++) {
				for (int i2 = 0; i2 < PE_ROWS; i2++)
				for (int k2 = 0; k2 < PE_DSIZE; k2++) {

					int i = i0 * BS_M * PE_ROWS + i2 * BS_M + i1;
					int k = k0 * BS_L * PE_DSIZE + k1 * PE_DSIZE + k2;

					// in1[i0][k0][i2][i1][k1][k2]=in0[i0][i2][i1][k0][k1][k2]
					// in_idx = (i0 * BS_M * PE_ROWS + i2 * BS_M + i1) * BN_L * BS_L * PE_DSIZE + (k0 * BS_L + k1) * PE_DSIZE + k2;
					in_idx = (i0 * BS_M * PE_ROWS + i2 * BS_M + i1) * L + (k0 * BS_L + k1) * PE_DSIZE + k2;
					in_bw_idx = (i0 * BN_L + k0) * BS_M * PE_ROWS * BS_L * PE_DSIZE + (i2 * BS_M + i1) * BS_L * PE_DSIZE + k1 * PE_DSIZE + k2;

				//	if (i < (BN_M * BS_M * PE_ROWS) && k < (BN_L * BS_L * PE_DSIZE))
					if ((i < M) && (k < L))
					{
						input_block_wise[in_bw_idx] = input[in_idx];
					} 
					else 
					{
						// maginal zero padding
						input_block_wise[in_bw_idx] = 0;
					}
	}}}
}

void block_wise_reformat_weight(data_precision *weight, data_precision *weight_block_wise)
{
	int w_idx, w_bw_idx;

    for (int j0 = 0; j0 < BN_N; j0++)
    for (int k0 = 0; k0 < BN_L; k0++) {
    for (int j1 = 0; j1 < BS_N; j1++)
    for (int k1 = 0; k1 < BS_L; k1++) {
    for (int j2 = 0; j2 < PE_COLS; j2++)
    for (int k2 = 0; k2 < PE_DSIZE; k2++) {

		int j = j0 * BS_N * PE_COLS + j2 * BS_N + j1;
		int k = k0 * BS_L * PE_DSIZE + k1 * PE_DSIZE + k2;

		// [j0][j2][j1][k0][k1][k2]
		w_idx = j * L + k;
		// [j0][k0][j2][j1][k1][k2]
		w_bw_idx = (j0 * BN_L + k0) * BS_N * PE_COLS * BS_L * PE_DSIZE + (j2 * BS_N + j1) * BS_L * PE_DSIZE + k1 * PE_DSIZE + k2;

		// w1[j0][k0][j2][j1][k1][k2] = w0[j0][j2][j1][k0][k1][k2]
	//	if (j < (BN_N * BS_N * PE_COLS) && k < (BN_L * BS_L * PE_DSIZE))
		if ((j < N) && (k < L))
		{
			weight_block_wise[w_bw_idx] = weight[w_idx];
		} 
		else
		{
			weight_block_wise[w_bw_idx] = 0;
		}
	}}}
}

void block_wise_reformat_output(data_precision *output_block_wise, data_precision *output)
//void bak_block_wise_reformat_output(data_precision *output_block_wise, data_precision *output)
{
	int out_bw_idx, out_idx;

	for (int i0 = 0; i0 < BN_M; i0++)
	for (int i2 = 0; i2 < PE_ROWS; i2++) 
	for (int i1 = 0; i1 < BS_M; i1++) {
		for (int j0 = 0; j0 < BN_N; j0++)
		for (int j2 = 0; j2 < PE_COLS; j2++)
		for (int j1 = 0; j1 < BS_N; j1++) {

			int i = i0 * BS_M * PE_ROWS + i2 * BS_M + i1;
			int j = j0 * BS_N * PE_COLS + j2 * BS_N + j1;

			// out0[i0][i2][i1][j0][j2][j1] = out2[i0][j0][i2][i1][j2][j1]
		//	out_idx = (i0 * BS_M * PE_ROWS + i2 * BS_M + i1) * BN_N * BS_N * PE_COLS + (j0 * BS_N * PE_COLS + j2 * BS_N + j1);
			out_idx = (i0 * BS_M * PE_ROWS + i2 * BS_M + i1) * N + (j0 * BS_N * PE_COLS + j2 * BS_N + j1);
		//	std::cout << "out_idx=" << out_idx << std::endl;
			out_bw_idx = (i0 * BN_N + j0) * BS_M * PE_ROWS * BS_N * PE_COLS + (i2 * BS_M + i1) * BS_N * PE_COLS + j2 * BS_N + j1;

		//	if (i < (BN_M * BS_M * PE_ROWS) && j < (BN_N * BS_N * PE_COLS))
			if ((i < M) && (j < N))
			{
				output[out_idx] = output_block_wise[out_bw_idx];

			//	if (out_idx == 32)
			//	{
			//		std::cout << "output[" << out_idx << "]" << output[out_idx] << std::endl;
			//	}
			}
		}}
}

void data_reorganization(data_precision *output_fm_drain, data_precision *output_fm_opencl_block_wise)
{
	for (int i0 = 0; i0 < BN_M; i0++)
	for (int i2 = 0; i2 < PE_ROWS; i2++) 
	for (int i1 = 0; i1 < BS_M; i1++) {
		for (int j0 = 0; j0 < BN_N; j0++)
		for (int j2 = 0; j2 < PE_COLS; j2++)
		for (int j1 = 0; j1 < BS_N; j1++) {

			// out1[i0][j0][i2][i1][j2][j1] = out0[i0][j0][i2][i1][j1][j2]
		int	out1_bw_idx = (i0 * BN_N + j0) * BS_M * PE_ROWS * BS_N * PE_COLS + (i2 * BS_M + i1) * BS_N * PE_COLS + j2 * BS_N + j1;
		int	out0_bw_idx = (i0 * BN_N + j0) * BS_M * PE_ROWS * BS_N * PE_COLS + (i2 * BS_M + i1) * BS_N * PE_COLS + j1 * PE_COLS + j2;

			output_fm_opencl_block_wise[out1_bw_idx] = output_fm_drain[out0_bw_idx];

		}}
}


#endif


