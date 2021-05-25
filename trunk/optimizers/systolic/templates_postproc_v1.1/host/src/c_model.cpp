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



//;#  my $debug_pe_idx = "on";
//;  my $debug_pe_idx = "off";

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "systolic_params.h"

#include "cnn.h"

#include "util.h"

// load a block from input and distribute it among feeders
// feeder(r2)
static void loader_and_feeder_in(
		int layer,
		float *in1,	float *in1_buf,
		int r0, int c0, int i0)
{
	// TODO: eliminate redundant reads
//	for (int p = 0; p < K; p++)
//	for (int q = 0; q < K; q++)
//	for (int i1 = 0; i1 < BS_I; i1++)
//	for (int r1 = 0; r1 < BS_R; r1++)
//	for (int c1 = 0; c1 < BS_C; c1++) {
//		for (int r2 = 0; r2 < R; r2++)
//		for (int i2 = 0; i2 < I; i2++) {
//			// in1_buf[r2][r1+p][c1+q][i1][i2]=in1[c0][i0][r2*BS_R+r1+p][c1+q][i1][i2]
//			int in1_idx = (c0 * BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + ((r2 * BS_R + r1 + p) * (BS_C + K - 1) + c1 + q) * BS_I * I + i1 * I + i2;
//			int in1_buf_idx = r2 * (BS_R + K - 1) * (BS_C + K - 1) * BS_I * I  + ((r1 + p) * (BS_C + K - 1) + (c1 + q)) * BS_I * I + i1 * I + i2;
//
//			in1_buf[in1_buf_idx] = in1[in1_idx];
//		}}
	
	const unsigned int cfg_BN_R = (cfg_img_row[layer] + R * BS_R - 1) / (R * BS_R);
	const unsigned int cfg_BN_C = (cfg_img_col[layer] + BS_C - 1) / BS_C;
	const unsigned int cfg_BN_I = (cfg_in_num[layer] + BS_I * I - 1) / (BS_I * I);

	for (int r2 = 0; r2 < R; r2++) 
	{
		for (int r1_p = 0; r1_p < (BS_R + K - 1); r1_p++)
		for (int c1_q = 0; c1_q < (BS_C + K - 1); c1_q++)
		for (int i1 = 0; i1 < BS_I; i1++)
		for (int i2 = 0; i2 < I; i2++)
		{
			int r = r0 * R * BS_R + r2 * BS_R + r1_p;
			int c = c0 * BS_C + c1_q;
			int i = i0 * BS_I * I + i1 * I + i2;

			int in1_idx = (r * (cfg_img_col[layer] + cfg_kernel[layer] - 1) + c) * cfg_in_num[layer] + i;

			int in1_idx_bw = (r0 * cfg_BN_C * cfg_BN_I + c0 * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + ((r2 * BS_R + r1_p) * (BS_C + K - 1) + c1_q) * BS_I * I + i1 * I + i2;
			int in1_buf_idx = r2 * (BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (r1_p * (BS_C + K - 1) + c1_q) * BS_I * I + i1 * I + i2;

			if (r < cfg_img_row[layer] + cfg_kernel[layer] - 1 && c < cfg_img_col[layer] + cfg_kernel[layer] - 1 && i < cfg_in_num[layer])
			{
				in1_buf[in1_buf_idx] = in1[in1_idx_bw];
			}
			else
			{
				in1_buf[in1_buf_idx] = 0;
			}
		}
	}
	/*
	for (int r2 = 0; r2 < R; r2++) {
		for (int r1_p = 0; r1_p < (BS_R + K - 1); r1_p++)
		for (int c1_q = 0; c1_q < (BS_C + K - 1); c1_q++)
		for (int i1 = 0; i1 < BS_I; i1++)
			if (r2 == 0) {
				printf("{");
				for (int i2 = 0; i2 < I; i2++) {
					int in1_buf_idx = r2 * (BS_R + K - 1) * (BS_C + K - 1) * BS_I * I  + (r1_p * (BS_C + K - 1) + c1_q) * BS_I * I + i1 * I + i2;

					printf(" %.2lf", in1_buf[in1_buf_idx]);
				}
				printf("}\n");
			}
	}
	*/
}

// load a block from weights and distribute it among feeders
// feeder(o2)
static void loader_and_feeder_weights(
		int layer,
		float *w1, float *w1_buf,
		int o0, int i0)
{
	const unsigned int cfg_BN_O = (cfg_out_num[layer] + O * BS_O - 1) / (O * BS_O);
	const unsigned int cfg_BN_I = (cfg_in_num[layer] + BS_I * I - 1) / (BS_I * I);

	for (int o2 = 0; o2 < O; o2++) 
	{
		for (int p = 0; p < K; p++)
		for (int q = 0; q < K; q++)
		for (int i1 = 0; i1 < BS_I; i1++)
		for (int o1 = 0; o1 < BS_O; o1++)
		{
			for (int i2 = 0; i2 < I; i2++) 
			{
				int o = o0 * BS_O * O + o2 * BS_O + o1;
				int i = i0 * BS_I * I + i1 * I + i2;

				int w1_idx = ((o * K + p) * K + q) * cfg_in_num[layer] + i;

				// w1_buf[o2][o1][p][q][i1][i2]=w1[o0][i0][o2][o1][p][q][i1][i2]
				int w1_idx_bw = (o0 * cfg_BN_I + i0) * O * BS_O * K * K * BS_I * I + (((o2 * BS_O + o1) * K + p) * K + q) * BS_I * I + i1 * I + i2;
				int w1_buf_idx = o2 * BS_O * K * K * BS_I * I + ((o1 * K + p) * K + q) * BS_I * I + i1 * I + i2;
	
				if (o < cfg_out_num[layer] && i < cfg_in_num[layer])
				{
					w1_buf[w1_buf_idx] = w1[w1_idx_bw];
				}
				else
				{
					w1_buf[w1_buf_idx] = 0;
				}
			}
		}
	}

	/*
	for (int o2 = 0; o2 < O; o2++) {
	for (int o1 = 0; o1 < BS_O; o1++)
	for (int p = 0; p < K; p++)
	for (int q = 0; q < K; q++)
	for (int i1 = 0; i1 < BS_I; i1++)
		if (o2 == 0) {
			printf("{");
			for (int i2 = 0; i2 < I; i2++) {
				int w1_buf_idx = o2 * BS_O * K * K * BS_I * I + ((o1 * K + p) * K + q) * BS_I * I + i1 * I + i2;
				printf(" %.2lf", w1_buf[w1_buf_idx]);
			}
			printf("}\n");
		}
	}
	*/
}

// PE(r2,o2)
static void PE(
		int layer,
		float *in1_buf, float *w1_buf, 
		float *PE_accum_shift_reg, 
		int r0,	int c0, int o0, int i0)
{
	for (int r2 = 0; r2 < R; r2++)
	for (int o2 = 0; o2 < O; o2++) {
		for (int p = 0; p < K; p++)
		for (int q = 0; q < K; q++)
		for (int i1 = 0; i1 < BS_I; i1++) {
			for (int r1 = 0; r1 < BS_R; r1++)
			for (int c1 = 0; c1 < BS_C; c1++)
			for (int o1 = 0; o1 < BS_O; o1++) {
				int reg_idx = (((r2 * O + o2) * BS_R + r1) * BS_C + c1) * BS_O + o1;
				for (int i2 = 0; i2 < I; i2++) {
					int in1_buf_idx = r2 * (BS_R + K - 1) * (BS_C + K - 1) * BS_I * I  + ((r1 + p) * (BS_C + K - 1) + (c1 + q)) * BS_I * I + i1 * I + i2;
					int w1_buf_idx = o2 * BS_O * K * K * BS_I * I + ((o1 * K + p) * K + q) * BS_I * I + i1 * I + i2;

					PE_accum_shift_reg[reg_idx] += in1_buf[in1_buf_idx] * w1_buf[w1_buf_idx];

				}

#if DEBUG_CONV
				if (layer == TEST_LAYER)
				{
					int r = r0 * R * BS_R + r2 * R + r1;
					int c = c0 * BS_C + c1;
					int o = o0 * O * BS_O + o2 * BS_O + o1;
					int i = i0 * BS_I + i1;

					int out1_idx_bw = ((r0 * cfg_BN_C[layer] + c0) * cfg_BN_O[layer] + o0) * R * BS_R * BS_C * O * BS_O + ((r2 * BS_R + r1) * BS_C + c1) * O * BS_O + o2 * BS_O + o1;
					if (out1_idx_bw == 1438080)
					{
						printf("out1[%d]=%.3f<-{", out1_idx_bw, PE_accum_shift_reg[reg_idx]);
						for (int i2 = 0; i2 < I; i2++)
						{
							int in1_buf_idx = r2 * (BS_R + K - 1) * (BS_C + K - 1) * BS_I * I  + ((r1 + p) * (BS_C + K - 1) + (c1 + q)) * BS_I * I + i1 * I + i2;

							printf("%.3f ", in1_buf[in1_buf_idx]);
						}
						printf("}.{");
						for (int i2 = 0; i2 < I; i2++)
						{
							int w1_buf_idx = o2 * BS_O * K * K * BS_I * I + ((o1 * K + p) * K + q) * BS_I * I + i1 * I + i2;
							printf("%.3f ", w1_buf[w1_buf_idx]);
						}
						printf("} r0=%d,c0=%d,o0=%d,i0=%d,r2=%d,r1=%d,c1=%d,o2=%d,o1=%d,i1=%d,r=%d,c=%d,o=%d,i=%d\n", r0, c0, o0, i0, r2, r1, c1, o2, o1, i1, r, c, o, i);
					}
				}
#endif

				//; if ($debug_pe_idx eq "on") {
				const unsigned int cfg_BN_R = (cfg_img_row[layer] + R * BS_R - 1) / (BS_R * R);
				const unsigned int cfg_BN_C = (cfg_img_col[layer] + BS_C - 1) / BS_C;
				const unsigned int cfg_BN_O = (cfg_out_num[layer] + O - 1) / O;

				int row = r2;
				int col = o2;
				// (r0,c0,o0,r1,c1,o1)
				int pe_idx = (r0 * cfg_BN_C * cfg_BN_O + c0 * cfg_BN_O + o0) * BS_R * BS_C * BS_O + (r1 * BS_C + c1) * BS_O + o1;
				// (r0,c0,o0,r2,r1,c1,o1,o2)
				int addr = (r0 * cfg_BN_C * cfg_BN_O + c0 * cfg_BN_O + o0) * R * BS_R * BS_C * BS_O * O + (r2 * BS_R * BS_C + r1 * BS_C + c1) * BS_O * O + o1 * O + o2; // address from drain

				if (row == CPU_PE_DEBUG_I && 
						col == CPU_PE_DEBUG_J &&
						pe_idx == CPU_PE_DEBUG_IDX) {

					printf("CPU PE[%d,%d]@%d addr=%d %.2lf= <+ {", row, col, pe_idx, addr, PE_accum_shift_reg[reg_idx]);

					for (int i2 = 0; i2 < I; i2++) {
						int in1_buf_idx = r2 * (BS_R + K - 1) * (BS_C + K - 1) * BS_I * I  + ((r1 + p) * (BS_C + K - 1) + (c1 + q)) * BS_I * I + i1 * I + i2;
						printf(" %.1f", in1_buf[in1_buf_idx]);
					}
					printf("}.{");
					for (int i2 = 0; i2 < I; i2++) {
						int w1_buf_idx = o2 * BS_O * K * K * BS_I * I + ((o1 * K + p) * K + q) * BS_I * I + i1 * I + i2;
						printf(" %.1f", w1_buf[w1_buf_idx]);
					}
					printf("} r0=%d c0=%d o0=%d i0=%d p=%d q=%d i1=%d r1=%d c1=%d o1=%d\n", r0, c0, o0, i0, p, q, i1, r1, c1, o1);
				}
				//;}
			}
		}
	}
}

// drain_O for C model
static void drain_O(int layer, 
		float *PE_accum_shift_reg, float *out0_buf, 
		int r0, int c0, int o0)
{

	for (int r2 = 0; r2 < R; r2++)
	for (int r1 = 0; r1 < BS_R; r1++)
	for (int c1 = 0; c1 < BS_C; c1++)
	for (int o1 = 0; o1 < BS_O; o1++)
	for (int o2 = 0; o2 < O; o2++)
	{
		int reg_idx = (((r2 * O + o2) * BS_R + r1) * BS_C + c1) * BS_O + o1;
		int out0_buf_idx = ((r2 * BS_R + r1) * BS_C + c1) * BS_O * O + o1 * O + o2;

		out0_buf[out0_buf_idx] = PE_accum_shift_reg[reg_idx];
	}
}

static void fast_conv(int layer, float *out5, float *out0_buf,
		int r0, int c0, int o0)
{
	for (int r2 = 0; r2 < R; r2++)
	for (int r1 = 0; r1 < BS_R; r1++)
	for (int c1 = 0; c1 < BS_C; c1++)
	for (int o2 = 0; o2 < /*O*/ 1; o2++)
	for (int o1 = 0; o1 < /*BS_O*/ 1; o1++)
	{
		int out0_buf_idx = ((r2 * BS_R + r1) * BS_C + c1) * O * BS_O + o2 * BS_O + o1;

		out0_buf[out0_buf_idx] = 0.0;
		for (int i0 = 0; i0 < cfg_BN_I[layer]; i0++)
		for (int p = 0; p < cfg_kernel[layer]; p++)
		for (int q = 0; q < cfg_kernel[layer]; q++)
		for (int i1 = 0; i1 < BS_I; i1++)
		for (int i2 = 0; i2 < I; i2++)
		{
			int out5_idx = ((r0 * cfg_BN_C[layer] + c0) * cfg_BN_I[i0]) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + ((r2 * BS_R + r1 + p) * (BS_C + K - 1) + (c1 + q)) * BS_I * I + i1 * I + i2;
			out0_buf[out0_buf_idx] += out5[out5_idx];
		}
	}
}

static void addr_trans(int layer, 
		float *out0_buf, float *out1_buf,
		int r0, int c0, int o0)
{
//	const unsigned int cfg_BN_R = (cfg_img_row[layer] + R * BS_R - 1) / (R * BS_R);
//	const unsigned int cfg_BN_C = (cfg_img_col[layer] + BS_C - 1) / BS_C;
//	const unsigned int cfg_BN_O = (cfg_out_num[layer] + O * BS_O - 1) / (O * BS_O);

	for (int r2 = 0; r2 < R; r2++)
	for (int r1 = 0; r1 < BS_R; r1++)
	for (int c1 = 0; c1 < BS_C; c1++)
	for (int o2 = 0; o2 < O; o2++)
	for (int o1 = 0; o1 < BS_O; o1++)
	{
		int r = r0 * R * BS_R + r2 * BS_R + r1;
		int c = c0 * BS_C + c1;
		int o = o0 * O * BS_O + o2 * BS_O + o1;

		int out1_buf_idx = ((r2 * BS_R + r1) * BS_C + c1) * O * BS_O + o2 * BS_O + o1;
		int out0_buf_idx = ((r2 * BS_R + r1) * BS_C + c1) * BS_O * O + o1 * O + o2;

		if (r < cfg_img_row[layer] && c < cfg_img_col[layer] && o < cfg_out_num[layer])
		{
			out1_buf[out1_buf_idx] = out0_buf[out0_buf_idx];
		}
		else
		{
			out1_buf[out1_buf_idx] = 0;
		}
	}
}

static void relu_sw(int layer, float *out1_buf, float *out2_buf, 
		int r0, int c0, int o0)
{
	for (int r2 = 0; r2 < R; r2++)
	for (int r1 = 0; r1 < BS_R; r1++)
	for (int c1 = 0; c1 < BS_C; c1++)
	for (int o2 = 0; o2 < O; o2++)
	for (int o1 = 0; o1 < BS_O; o1++)
	{
		int out1_buf_idx = ((r2 * BS_R + r1) * BS_C + c1) * O * BS_O + o2 * BS_O + o1;
		int out2_buf_idx = out1_buf_idx;

		int out0_idx = ((r0 * cfg_BN_C[layer] + c0) * cfg_BN_O[layer] + o0) * R * BS_R * BS_C * O * BS_O + ((r2 * BS_R + r1) * BS_C + c1) * O * BS_O + o2 * BS_O + o1;

	//	out2_buf[out2_buf_idx] = out1_buf[out1_buf_idx];
	//	out2_buf[out2_buf_idx] = 1.0;
		out2_buf[out2_buf_idx] = out0_idx;
	}
}

static void pooling_sw(int layer,
		float *out2_buf, float *out3_buf, 
		int r0, int c0, int o0)
{
	/*
	 * In C model, pooling only performs down scaling, output to a scaled smaller buffer, 
	 * block merging will be performed in zero padding.
	 * */
	int BS_R_R_scaled = (R * BS_R) / cfg_pooling_size[layer];
	int BS_C_scaled = BS_C / cfg_pooling_size[layer];

	for (int r1 = 0; r1 < BS_R_R_scaled; r1++)
	for (int c1 = 0; c1 < BS_C_scaled; c1++)
	for (int o1 = 0; o1 < O * BS_O; o1++) 
	{
		float max = -1.0;
		int out3_buf_idx = (r1 * BS_C_scaled + c1) * O * BS_O + o1;

		for (int p = 0; p < cfg_pooling_size[layer]; p++)
		for (int q = 0; q < cfg_pooling_size[layer]; q++)
		{
			int out2_buf_idx = ((r1 * cfg_pooling_size[layer] + p) * BS_C + (c1 * cfg_pooling_size[layer] + q)) * O * BS_O + o1;

		//	if (r0 == 0 && c0 == 2 && o0 == 0 && r1 == 0 && c1 == 0 && o1 == 0)
		//	{
		//		printf("%.3f\n", out2_buf[out2_buf_idx]);
		//	}

			if ((max + 1.0) < 1e-6 || (max - out2_buf[out2_buf_idx]) < 0)
				max = out2_buf[out2_buf_idx];
		}

		out3_buf[out3_buf_idx] = max;

	//	int out3_idx = ((r0 * cfg_BN_C[layer] + c0) * cfg_BN_O[layer] + o0) * BS_R_R_scaled * BS_C_scaled * O * BS_O + out3_buf_idx;
	//	if (r0 == 0 && c0 == 2 && o0 == 0 && r1 == 0 && c1 == 0 && o1 == 0)
	//	{
	//		printf("out3_buf[%d]=%.3f\n", out3_idx, out3_buf[out3_buf_idx]);
	//	}
	}

//	if (r0 == 0 && c0 == 2 && o0 == 0)
//	{
//		for (int rr = 0; rr < R * BS_R; rr++)
//		for (int cc = 0; cc < BS_C; cc++)
//		for (int oo = 0; oo < O * BS_O; oo++) 
//		{
//			int out2_buf_idx = (rr * BS_C + cc) * O * BS_O + oo;
//
//			printf("%.3f\n", out2_buf[out2_buf_idx]);
//		}
//	}
}

static void zero_padding_sw_big(int layer,
		float *out2_buf, float *out5, 
		int r0, int c0, int i0)
{
	const unsigned int cfg_BN_R = (cfg_img_row[layer + 1] + R * BS_R - 1) / (R * BS_R);
	const unsigned int cfg_BN_C = (cfg_img_col[layer + 1] + BS_C - 1) / BS_C;
	const unsigned int cfg_BN_I = (cfg_in_num[layer + 1] + BS_I * I - 1) / (BS_I * I);

	// part 1
	for (int rr = 0; rr < (K - 1) / 2; rr++)
	for (int cc = 0; cc < (K - 1) / 2; cc++)
	for (int ii = 0; ii < (BS_I * I); ii++)
	{
		if (r0 > 0 && c0 > 0)
		{
			int out2_buf_idx = (rr * BS_C + cc) * BS_I * I + ii;
			int out5_idx = (((r0 - 1) * cfg_BN_C + c0 - 1) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (((K - 1) / 2 + R * BS_R + rr) * (BS_C + K - 1) + ((K - 1) / 2 + BS_C + cc)) * BS_I * I + ii;

			out5[out5_idx] = out2_buf[out2_buf_idx];
		}
		else
		{
			int out5_idx = ((r0 * cfg_BN_C + c0) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (rr * (BS_C + K - 1) + cc) * BS_I * I + ii;

			out5[out5_idx] = 0;
		}
	}

	// part 2
	for (int rr = 0; rr < (K - 1) / 2; rr++)
	for (int cc = 0; cc < BS_C; cc++)
	for (int ii = 0; ii < (BS_I * I); ii++)
	{
		if (r0 > 0)
		{
			int out2_buf_idx = (rr * BS_C + cc) * BS_I * I + ii;
			int out5_idx = (((r0 - 1) * cfg_BN_C + c0) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (((K - 1) / 2 + R * BS_R + rr) * (BS_C + K - 1) + ((K - 1) / 2 + cc)) * BS_I * I + ii;

			out5[out5_idx] = out2_buf[out2_buf_idx];
		}
		else
		{
			int out5_idx = ((r0 * cfg_BN_C + c0) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (rr * (BS_C + K - 1) + ((K - 1) / 2 + cc)) * BS_I * I + ii;
			
			out5[out5_idx] = 0;
		}
	}

	// part 3
	for (int rr = 0; rr < (K - 1) / 2; rr++)
	for (int cc = BS_C - (K - 1) / 2; cc < BS_C; cc++)
	for (int ii = 0; ii < (BS_I * I); ii++)
	{
		if (r0 > 0 && c0 < cfg_BN_C - 1)
		{
			int out2_buf_idx = (rr * BS_C + cc) * BS_I * I + ii;
			int out5_idx = (((r0 - 1) * cfg_BN_C + c0 + 1) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (((K - 1) / 2 + R * BS_R + rr) * (BS_C + K - 1) + (cc - (BS_C - (K - 1)/ 2))) * BS_I * I + ii;

			out5[out5_idx] = out2_buf[out2_buf_idx];
		}
		else
		{
			int out5_idx = ((r0 * cfg_BN_C + c0) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (rr * (BS_C + K - 1) + (cc + (K - 1))) * BS_I * I + ii;

			out5[out5_idx] = 0;
		}
	}

	// part 4
	for (int rr = 0; rr < R * BS_R; rr++)
	for (int cc = BS_C - (K - 1) / 2; cc < BS_C; cc++)
	for (int ii = 0; ii < (BS_I * I); ii++)
	{
		if (c0 < cfg_BN_C - 1)
		{
			int out2_buf_idx = (rr * BS_C + cc) * BS_I * I + ii;
			int out5_idx = ((r0 * cfg_BN_C + c0 + 1) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (((K - 1) / 2 + rr) * (BS_C + K - 1) + (cc - (BS_C - (K - 1)/ 2))) * BS_I * I + ii;

			out5[out5_idx] = out2_buf[out2_buf_idx];
		}
		else
		{
			int out5_idx = ((r0 * cfg_BN_C + c0) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + ((rr + (K - 1) / 2) * (BS_C + K - 1) + (cc + (K - 1))) * BS_I * I + ii;

			out5[out5_idx] = 0;
		}
	}

	// part 5
	for (int rr = R * BS_R - (K - 1) / 2; rr < R * BS_R; rr++)
	for (int cc = BS_C - (K - 1) / 2; cc < BS_C; cc++)
	for (int ii = 0; ii < (BS_I * I); ii++)
	{
		if (r0 < cfg_BN_R - 1 && c0 < cfg_BN_C - 1)
		{
			int out2_buf_idx = (rr * BS_C + cc) * BS_I * I + ii;
			int rr_p = rr - (R * BS_R - (K - 1) / 2);
			int cc_p = cc - (BS_C - (K - 1) / 2);
			int out5_idx = (((r0 + 1) * cfg_BN_C + c0 + 1) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (rr_p * (BS_C + K - 1) + cc_p) * BS_I * I + ii;

			out5[out5_idx] = out2_buf[out2_buf_idx];
		}
		else
		{
			int out5_idx = ((r0 * cfg_BN_C + c0) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + ((rr + K - 1) * (BS_C + K - 1) + (cc + (K - 1))) * BS_I * I + ii;

			out5[out5_idx] = 0;
		}
	}

	// part 6
	for (int rr = R * BS_R - (K - 1) / 2; rr < R * BS_R; rr++)
	for (int cc = 0; cc < BS_C; cc++)
	for (int ii = 0; ii < (BS_I * I); ii++)
	{
		if (r0 < cfg_BN_R - 1)
		{
			int out2_buf_idx = (rr * BS_C + cc) * BS_I * I + ii;
			int out5_idx = (((r0 + 1) * cfg_BN_C + c0) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + ((rr - (R * BS_R - (K - 1) / 2)) * (BS_C + K - 1) + (cc + (K - 1) / 2)) * BS_I * I + ii;

			out5[out5_idx] = out2_buf[out2_buf_idx];
		}
		else
		{
			int out5_idx = ((r0 * cfg_BN_C + c0) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + ((rr + K - 1) * (BS_C + K - 1) + (cc + (K - 1) / 2)) * BS_I * I + ii;

			out5[out5_idx] = 0;
		}
	}

	// part 7
	for (int rr = R * BS_R - (K - 1) / 2; rr < R * BS_R; rr++)
	for (int cc = 0; cc < (K - 1) / 2; cc++)
	for (int ii = 0; ii < (BS_I * I); ii++)
	{
		if (r0 < cfg_BN_R - 1 && c0 > 0)
		{
			int out2_buf_idx = (rr * BS_C + cc) * BS_I * I + ii;
			int out5_idx = (((r0 + 1) * cfg_BN_C + c0 - 1) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + ((rr - (R * BS_R - (K - 1) / 2)) * (BS_C + K - 1) + (cc + BS_C + (K - 1) / 2)) * BS_I * I + ii;

			out5[out5_idx] = out2_buf[out2_buf_idx];
		}
		else
		{
			int out5_idx = ((r0 * cfg_BN_C + c0) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + ((rr + K - 1) * (BS_C + K - 1) + cc) * BS_I * I + ii;

			out5[out5_idx] = 0;
		}
	}

	// part 8
	for (int rr = 0; rr < R * BS_R; rr++)
	for (int cc = 0; cc < (K - 1) / 2; cc++)
	for (int ii = 0; ii < (BS_I * I); ii++)
	{
		if (c0 > 0)
		{
			int out2_buf_idx = (rr * BS_C + cc) * BS_I * I + ii;
			int rr_p = rr + ((K - 1) / 2);
			int cc_p = cc + BS_C + (K - 1) / 2;
			int out5_idx = ((r0 * cfg_BN_C + c0 - 1) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (rr_p * (BS_C + K - 1) + cc_p) * BS_I * I + ii;

		//	if (out5_idx == 992)
		//	{
		//		printf("r0=%d, c0=%d, i0=%d, rr_p=%d, cc_p=%d, ii=%d\n", r0, c0, i0, rr_p, cc_p, ii);
		//		printf("out2_buf[%d]=%.3f\n", out2_buf_idx, out2_buf[out2_buf_idx]);
		//	}

			out5[out5_idx] = out2_buf[out2_buf_idx];
		}
		else
		{
			int rr_p = rr + ((K - 1) / 2);
			int cc_p = cc;
			int out5_idx = ((r0 * cfg_BN_C + c0) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (rr_p * (BS_C + K - 1) + cc_p) * BS_I * I + ii;

			out5[out5_idx] = 0;
		}
	}
	
	// part 9
	for (int rr = 0; rr < (R * BS_R); rr++)
	for (int cc = 0; cc < BS_C; cc++)
	for (int ii = 0; ii < (BS_I * I); ii++)
	{
		int out2_buf_idx = (rr * BS_C + cc) * BS_I * I + ii;
		int out5_idx = ((r0 * cfg_BN_C + c0) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (((K - 1) / 2 + rr) * (BS_C + K - 1) + ((K - 1) / 2 + cc)) * BS_I * I + ii;

		out5[out5_idx] = out2_buf[out2_buf_idx];
	}
}

static void zero_padding_sw_little(int layer,
		float *out3_buf, float *out5, 
		int r0, int c0, int i0)
{
	const unsigned int cfg_BN_R = (cfg_img_row[layer + 1] + R * BS_R - 1) / (R * BS_R);
	const unsigned int cfg_BN_C = (cfg_img_col[layer + 1] + BS_C - 1) / BS_C;
	const unsigned int cfg_BN_I = (cfg_in_num[layer + 1] + BS_I * I - 1) / (BS_I * I);

	int BS_R_R_scaled = (R * BS_R) / cfg_pooling_size[layer];
	int BS_C_scaled = BS_C / cfg_pooling_size[layer];

	int blk_idx_r0 = r0 / cfg_pooling_size[layer];
	int blk_idx_c0 = c0 / cfg_pooling_size[layer];
	int subblk_idx_r0 = r0 % cfg_pooling_size[layer];
	int subblk_idx_c0 = c0 % cfg_pooling_size[layer];

#define ZERO_PADDING_SW_LITTLE_OUT5_IDX(r0_p, c0_p, i0, rr_p, cc_p, ii) ((((r0_p) * cfg_BN_C + (c0_p)) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + ((rr_p) * (BS_C + K - 1) + (cc_p)) * BS_I * I + ii)

	// part 1, 2, 3, 4
	for (int rr = 0; rr < BS_R_R_scaled; rr++)
	for (int cc = 0; cc < BS_C_scaled; cc++)
	for (int ii = 0; ii < (BS_I * I); ii++) 
	{
		int out3_buf_idx = (rr * BS_C_scaled + cc) * BS_I * I + ii;
		int rr_p = (K - 1) / 2 + subblk_idx_r0 * BS_R_R_scaled + rr;
		int cc_p = (K - 1) / 2 + subblk_idx_c0 * BS_C_scaled + cc;
		int out5_idx = ((blk_idx_r0 * cfg_BN_C + blk_idx_c0) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (rr_p * (BS_C + K - 1) + cc_p) * BS_I * I + ii;

		out5[out5_idx] = out3_buf[out3_buf_idx];
	}

	// part 5
	if (subblk_idx_r0 == 0 && subblk_idx_c0 == 0)
	{
		for (int rr = 0; rr < (K - 1) / 2; rr++)
		for (int cc = 0; cc < (K - 1) / 2; cc++)
		for (int ii = 0; ii < (BS_I * I); ii++) 
		{
			if (blk_idx_r0 > 0 && blk_idx_c0 > 0)
			{
				int out3_buf_idx = (rr * BS_C_scaled + cc) * BS_I * I + ii;
				int rr_p = (K - 1) / 2 + R * BS_R + rr;
				int cc_p = (K - 1) / 2 + BS_C + cc;
				int out5_idx = (((blk_idx_r0 - 1) * cfg_BN_C + blk_idx_c0 - 1) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (rr_p * (BS_C + K - 1) + cc_p) * BS_I * I + ii;

				out5[out5_idx] = out3_buf[out3_buf_idx];
			}
			else
			{
				int rr_p = rr;
				int cc_p = cc;
				int out5_idx = ((blk_idx_r0 * cfg_BN_C + blk_idx_c0) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (rr_p * (BS_C + K - 1) + cc_p) * BS_I * I + ii;

				out5[out5_idx] = 0;
			}
		}

	}

	// part 6.l, 6.r
	if (subblk_idx_r0 == 0)
	{
		for (int rr = 0; rr < (K - 1) / 2; rr++)
		for (int cc = 0; cc < BS_C_scaled; cc++)
		for (int ii = 0; ii < (BS_I * I); ii++) 
		{
			if (blk_idx_r0 > 0)
			{
				int out3_buf_idx = (rr * BS_C_scaled + cc) * BS_I * I + ii;
				int rr_p = (K - 1) / 2 + R * BS_R + rr;
				int cc_p = (K - 1) / 2 + subblk_idx_c0 * BS_C_scaled + cc;
				int out5_idx = (((blk_idx_r0 - 1) * cfg_BN_C + blk_idx_c0) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (rr_p * (BS_C + K - 1) + cc_p) * BS_I * I + ii;

				out5[out5_idx] = out3_buf[out3_buf_idx];
			}
			else
			{
				int rr_p = rr;
				int cc_p = cc + ((K - 1) / 2) + subblk_idx_c0 * BS_C_scaled;
				int out5_idx = ((blk_idx_r0 * cfg_BN_C + blk_idx_c0) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (rr_p * (BS_C + K - 1) + cc_p) * BS_I * I + ii;

				out5[out5_idx] = 0;
			}
		}
	}

	// part 7
	if (subblk_idx_r0 == 0 && subblk_idx_c0 == cfg_pooling_size[layer] - 1)
	{
		for (int rr = 0; rr < (K - 1) / 2; rr++)
		for (int cc = BS_C_scaled - ((K - 1) / 2); cc < BS_C_scaled; cc++)
		for (int ii = 0; ii < (BS_I * I); ii++) 
		{
			if (blk_idx_r0 > 0 && blk_idx_c0 < cfg_BN_C - 1)
			{
				int out3_buf_idx = (rr * BS_C_scaled + cc) * BS_I * I + ii;
				int rr_p = (K - 1) / 2 + R * BS_R + rr;
				int cc_p = cc - (BS_C_scaled - ((K - 1) / 2));
				int out5_idx = (((blk_idx_r0 - 1) * cfg_BN_C + blk_idx_c0 + 1) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (rr_p * (BS_C + K - 1) + cc_p) * BS_I * I + ii;

				out5[out5_idx] = out3_buf[out3_buf_idx];
			}
			else
			{
				int rr_p = rr;
				int cc_p = cc + BS_C - BS_C_scaled + (K - 1);
				int out5_idx = ((blk_idx_r0 * cfg_BN_C + blk_idx_c0) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (rr_p * (BS_C + K - 1) + cc_p) * BS_I * I + ii;


				out5[out5_idx] = 0;
			}
		}
	}

	// part 8.u, 8.d
	if (subblk_idx_c0 == 0)
	{
		for (int rr = 0; rr < BS_R_R_scaled; rr++)
		for (int cc = 0; cc < (K - 1) / 2; cc++)
		for (int ii = 0; ii < (BS_I * I); ii++) 
		{
			if (blk_idx_c0 > 0)
			{
				int out3_buf_idx = (rr * BS_C_scaled + cc) * BS_I * I + ii;
				int rr_p = rr + (K - 1) / 2 + subblk_idx_r0 * BS_R_R_scaled;
				int cc_p = cc + ((K - 1) / 2) + BS_C;
				int out5_idx = ((blk_idx_r0 * cfg_BN_C + blk_idx_c0 - 1) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (rr_p * (BS_C + K - 1) + cc_p) * BS_I * I + ii;
			if (r0 == 0 && c0 == 2 && i0 == 0 && rr == 0 && cc == 0 && ii == 0)
				printf("out3_buf[%d]=%.3f\n", out3_buf_idx, out3_buf[out3_buf_idx]);

				out5[out5_idx] = out3_buf[out3_buf_idx];

				if (out5_idx == 992)
					printf("8: out5[%d]=%.3f\n", out5_idx, out5[out5_idx]);
			}
			else
			{
				int rr_p = rr + (K - 1) / 2 + subblk_idx_r0 * BS_R_R_scaled;
				int cc_p = cc;
				int out5_idx = ((blk_idx_r0 * cfg_BN_C + blk_idx_c0) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (rr_p * (BS_C + K - 1) + cc_p) * BS_I * I + ii;

				out5[out5_idx] = 0;
			}
		}
	}

	// part 9
	if (subblk_idx_r0 == cfg_pooling_size[layer] - 1 && subblk_idx_c0 == 0)
	{
		for (int rr = BS_R_R_scaled - ((K - 1) / 2); rr < BS_R_R_scaled; rr++)
		for (int cc = 0; cc < (K - 1) / 2; cc++)
		for (int ii = 0; ii < (BS_I * I); ii++) 
		{
			if (blk_idx_r0 < cfg_BN_R - 1 && blk_idx_c0 > 0)
			{
				int out3_buf_idx = (rr * BS_C_scaled + cc) * BS_I * I + ii;
				int rr_p = rr + (K - 1) / 2 - BS_R_R_scaled;
				int cc_p = cc + BS_C + ((K - 1) / 2);
				int out5_idx = (((blk_idx_r0 + 1) * cfg_BN_C + blk_idx_c0 - 1) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (rr_p * (BS_C + K - 1) + cc_p) * BS_I * I + ii;

				out5[out5_idx] = out3_buf[out3_buf_idx];
			}
			else
			{
				int rr_p = rr + R * BS_R + (K - 1) - BS_R_R_scaled;
				int cc_p = cc;
			//	int out5_idx = ((blk_idx_r0 * cfg_BN_C + blk_idx_c0) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (rr_p * (BS_C + K - 1) + cc_p) * BS_I * I + ii;
				int out5_idx = ZERO_PADDING_SW_LITTLE_OUT5_IDX(blk_idx_r0, blk_idx_c0, i0, rr_p, cc_p, ii);

				out5[out5_idx] = 0;
			}
		}
	}

	// part 10.l, 10.r
	if (subblk_idx_r0 == cfg_pooling_size[layer] - 1)
	{
		for (int rr = BS_R_R_scaled - ((K - 1) / 2); rr < BS_R_R_scaled; rr++)
		for (int cc = 0; cc < BS_C_scaled; cc++)
		for (int ii = 0; ii < (BS_I * I); ii++) 
		{
			if (blk_idx_r0 < cfg_BN_R - 1)
			{
				int out3_buf_idx = (rr * BS_C_scaled + cc) * BS_I * I + ii;
				int rr_p = rr - (BS_R_R_scaled - (K - 1) / 2);
				int cc_p = cc + (K - 1) / 2 + subblk_idx_c0 * BS_C_scaled;
				int out5_idx = ZERO_PADDING_SW_LITTLE_OUT5_IDX(blk_idx_r0 + 1, blk_idx_c0, i0, rr_p, cc_p, ii);

				out5[out5_idx] = out3_buf[out3_buf_idx];
			}
			else
			{
				int rr_p = rr + R * BS_R + (K - 1) - BS_R_R_scaled;
				int cc_p = cc + (K - 1) / 2 + subblk_idx_c0 * BS_C_scaled;
				int out5_idx = ZERO_PADDING_SW_LITTLE_OUT5_IDX(blk_idx_r0, blk_idx_c0, i0, rr_p, cc_p, ii);

				out5[out5_idx] = 0;
			}
		}
	}

	// part 11
	if (subblk_idx_r0 == cfg_pooling_size[layer] - 1 && subblk_idx_c0 == cfg_pooling_size[layer] - 1)
	{
		for (int rr = BS_R_R_scaled - ((K - 1) / 2); rr < BS_R_R_scaled; rr++)
		for (int cc = BS_C_scaled - ((K - 1) / 2); cc < BS_C_scaled; cc++)
		for (int ii = 0; ii < (BS_I * I); ii++) 
		{
			if (blk_idx_r0 < cfg_BN_R - 1 && blk_idx_c0 < cfg_BN_C - 1)
			{
				int out3_buf_idx = (rr * BS_C_scaled + cc) * BS_I * I + ii;
				int rr_p = rr - (BS_R_R_scaled - (K - 1) / 2);
				int cc_p = cc + (K - 1) / 2 - BS_C_scaled;
		//		int out5_idx = ZERO_PADDING_SW_LITTLE_OUT5_IDX(blk_idx_r0 + 1, blk_idx_c0 + 1, i0, rr_p, cc_p, ii);
				int out5_idx = (((blk_idx_r0 + 1) * cfg_BN_C + blk_idx_c0 + 1) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (rr_p * (BS_C + K - 1) + cc_p) * BS_I * I + ii;

				out5[out5_idx] = out3_buf[out3_buf_idx];
			}
			else
			{
				int rr_p = rr + R * BS_R + (K - 1) - BS_R_R_scaled;
				int cc_p = cc + BS_C + (K - 1) - BS_C_scaled;
				int out5_idx = ZERO_PADDING_SW_LITTLE_OUT5_IDX(blk_idx_r0, blk_idx_c0, i0, rr_p, cc_p, ii);

				out5[out5_idx] = 0;
			}
		}
	}

	// part 12.u, 12.d
	if (subblk_idx_c0 == cfg_pooling_size[layer] - 1)
	{
		for (int rr = 0; rr < BS_R_R_scaled; rr++)
		for (int cc = BS_C_scaled - ((K - 1) / 2); cc < BS_C_scaled; cc++)
		for (int ii = 0; ii < (BS_I * I); ii++) 
		{
			if (blk_idx_c0 < cfg_BN_C - 1)
			{
				int out3_buf_idx = (rr * BS_C_scaled + cc) * BS_I * I + ii;
				int rr_p = rr + (K - 1) / 2 + subblk_idx_r0 * BS_R_R_scaled;
				int cc_p = cc + (K - 1) / 2 - BS_C_scaled;
			//	int out5_idx = ZERO_PADDING_SW_LITTLE_OUT5_IDX(blk_idx_r0, blk_idx_c0 + 1, i0, rr_p, cc_p, ii);
				int out5_idx = ((blk_idx_r0 * cfg_BN_C + blk_idx_c0 + 1) * cfg_BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (rr_p * (BS_C + K - 1) + cc_p) * BS_I * I + ii;


				out5[out5_idx] = out3_buf[out3_buf_idx];
			}
			else
			{
				int rr_p = rr + (K - 1) / 2 + subblk_idx_r0 * BS_R_R_scaled;
				int cc_p = cc + BS_C + (K - 1) - BS_C_scaled;
				int out5_idx = ZERO_PADDING_SW_LITTLE_OUT5_IDX(blk_idx_r0, blk_idx_c0, i0, rr_p, cc_p, ii);

				out5[out5_idx] = 0;
			}
		}
	}
}

static void write_to_ddr_big(int layer,
		float *out2_buf, float *out3,
		int r0, int c0, int o0)
{
	const unsigned int cfg_BN_R = (cfg_img_row[layer] + R * BS_R - 1) / (R * BS_R);
	const unsigned int cfg_BN_C = (cfg_img_col[layer] + BS_C - 1) / BS_C;
	const unsigned int cfg_BN_O = (cfg_out_num[layer] + O * BS_O - 1) / (O * BS_O);

	for (int r2 = 0; r2 < R; r2++)
	for (int r1 = 0; r1 < BS_R; r1++)
	for (int c1 = 0; c1 < BS_C; c1++)
	for (int o2 = 0; o2 < O; o2++)
	for (int o1 = 0; o1 < BS_O; o1++)
	{
		int out2_buf_idx = ((r2 * BS_R + r1) * BS_C + c1) * O * BS_O + o2 * BS_O + o1;
		int out3_idx = ((r0 * cfg_BN_C + c0) * cfg_BN_O + o0) * R * BS_R * BS_C * O * BS_O + out2_buf_idx;

		out3[out3_idx] = out2_buf[out2_buf_idx];

	}
}

static void write_to_ddr_little(int layer,
		float *out3_buf, float *out3,
		int r0, int c0, int o0)
{
	int cfg_BN_R = (cfg_img_row[layer] + R * BS_R - 1) / (R * BS_R);
	int cfg_BN_C = (cfg_img_col[layer] + BS_C - 1) / BS_C;
	int cfg_BN_O = (cfg_out_num[layer] + O * BS_O - 1) / (O * BS_O);

	int pooled_BN_R = (cfg_BN_R + cfg_pooling_size[layer] - 1) / cfg_pooling_size[layer];
	int pooled_BN_C = (cfg_BN_C + cfg_pooling_size[layer] - 1) / cfg_pooling_size[layer];


	int BS_R_R_scaled = (R * BS_R) / cfg_pooling_size[layer];
	int BS_C_scaled = BS_C / cfg_pooling_size[layer];

	int blk_idx_r0 = r0 / cfg_pooling_size[layer];
	int blk_idx_c0 = c0 / cfg_pooling_size[layer];

	int subblk_idx_r0 = r0 % cfg_pooling_size[layer];
	int subblk_idx_c0 = c0 % cfg_pooling_size[layer];

	for (int rr = 0; rr < BS_R_R_scaled; rr++)
	for (int cc = 0; cc < BS_C_scaled; cc++)
	for (int oo = 0; oo < O * BS_O; oo++)
	{
		int out3_buf_idx = (rr * BS_C_scaled + cc) * O * BS_O + oo;
		int out3_idx = ((blk_idx_r0 * pooled_BN_C + blk_idx_c0) * cfg_BN_O + o0) * R * BS_R * BS_C * O * BS_O + ((subblk_idx_r0 * BS_R_R_scaled + rr) * BS_C + (subblk_idx_c0 * BS_C_scaled + cc)) * O * BS_O + oo;

		out3[out3_idx] = out3_buf[out3_buf_idx];
	}
}

static void print_out5(int layer, float *out5)
{
	const unsigned int cfg_BN_R = ((cfg_img_row[layer] / cfg_pooling_size[layer]) + R * BS_R - 1) / (R * BS_R);
	const unsigned int cfg_BN_C = ((cfg_img_col[layer] / cfg_pooling_size[layer]) + BS_C - 1) / BS_C;
	const unsigned int cfg_BN_O = (cfg_out_num[layer] + O * BS_O - 1) / (O * BS_O);

	FILE *f;

	f = fopen("pad_sw.txt", "w");

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

			int out5_idx_bw = ((r0 * cfg_BN_C + c0) * cfg_BN_O + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (r_p * (BS_C + K - 1) + c_q) * BS_I * I + i12;

			//	printf("%.3f\n", out_block_wise_padded[out_block_wise_padded_idx]);
			fprintf(f, "%.3f\n", out5[out5_idx_bw]);

			if (out5_idx_bw == 992)
			{
				printf("r0=%d, c0=%d, i0=%d, r_p=%d, c_q=%d, i12=%d\n", r0, c0, i0, r_p, c_q, i12);
			}
		}
	}

	fclose(f);

	/*
	FILE *f1, *f2;
	char s1[] = "pad_bw.txt";
	char s2[] = "pad_sw.txt";
	int err = 0;

	if ((f1 = fopen(s1, "r")) == NULL)
	{
		printf("Open file %s for reading fails.\n", s1);
		exit(1);
	}
	if ((f2 = fopen(s2, "r")) == NULL)
	{
		printf("Open file %s for reading fails.\n", s2);
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
			float out0, out1;

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
					printf("out_bw[%d]=%.3f, out_sw[%d]=%.3f\n", out_idx_bw, out0, out_idx_bw, out1);
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
	*/
}

static void print_out3(int layer, float *out3)
{
	printf("%s\n", __func__);

	const unsigned int pooled_img_row = cfg_img_row[layer] / cfg_pooling_size[layer];
	const unsigned int pooled_img_col = cfg_img_col[layer] / cfg_pooling_size[layer];

	const unsigned int cfg_BN_R = (pooled_img_row + R * BS_R - 1) / (R * BS_R);
	const unsigned int cfg_BN_C = (pooled_img_col + BS_C - 1) / BS_C;
	const unsigned int cfg_BN_O = (cfg_out_num[layer] + O * BS_O - 1) / (O * BS_O);

	FILE *f;

	f = fopen("out3.txt", "w");

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

			//	printf("%.3f\n", conv_out_block_wise[out_idx]);
			fprintf(f, "%.3f\n", out3[out_idx]);

		//	if (out_idx == 4512)
		//	{
		//		printf("r0=%d, c0=%d, o0=%d, rr=%d, cc=%d, oo=%d\n", r0, c0, o0, rr, cc, oo);
		//	}
		}
	}

	fclose(f);

	FILE *f1, *f2;
	char s1[] = "pooling_out_block_wise.txt";
	char s2[] = "out3.txt";
	int err = 0;

	if ((f1 = fopen(s1, "r")) == NULL)
	{
		printf("Open file %s for reading fails.\n", s1);
		exit(1);
	}
	if ((f2 = fopen(s2, "r")) == NULL)
	{
		printf("Open file %s for reading fails.\n", s2);
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

				float out0, out1;

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
						printf("pooling_out_block_wise[%d]=%.3f, out3[%d]=%.3f\n", out_idx_bw, out0, out_idx_bw, out1);
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

void print_out1(int layer)
{
	printf("%s\n", __func__);

	const unsigned int cfg_BN_R = (cfg_img_row[layer] + R * BS_R - 1) / (R * BS_R);
	const unsigned int cfg_BN_C = (cfg_img_col[layer] + BS_C - 1) / BS_C;
	const unsigned int cfg_BN_O = (cfg_out_num[layer] + O * BS_O - 1) / (O * BS_O);

	FILE *f1, *f2;
	char s1[] = "conv_out_block_wise.txt";
	char s2[] = "out1.txt";
	int err = 0;

	if ((f1 = fopen(s1, "r")) == NULL)
	{
		printf("Open file %s for reading fails.\n", s1);
		exit(1);
	}
	if ((f2 = fopen(s2, "r")) == NULL)
	{
		printf("Open file %s for reading fails.\n", s2);
		exit(1);
	}

	for (int r0 = 0; r0 < cfg_BN_R; r0++)
	for (int c0 = 0; c0 < cfg_BN_C; c0++)
	for (int o0 = 0; o0 < cfg_BN_O; o0++) 
	{
		for (int rr = 0; rr < R * BS_R; rr++)
		for (int cc = 0; cc < BS_C; cc++)
		for (int oo = 0; oo < O * BS_O; oo++)
		{
			{
				int r = r0 * R * BS_R + rr;
				int c = c0 * BS_C + cc;
				int o = o0 * O * BS_O + oo;

				float out0, out1;

				int out_idx = r * cfg_img_col[layer] * cfg_out_num[layer] + c * cfg_out_num[layer] + o;
				int out_idx_bw = ((r0 * cfg_BN_C + c0) * cfg_BN_O + o0) * R * BS_R * BS_C * O * BS_O + (rr * BS_C + cc) * O * BS_O + oo;

				fscanf(f1, "%f", &out0);
				fscanf(f2, "%f", &out1);

				if (out_idx_bw == 1438080)
				{
					printf("%s:r=%d,c=%d,o=%d,r0=%d,c0=%d,o0=%d,conv_out_block_wise[%d]=%.3f, out1[%d]=%.3f\n", __func__, r, c, o, r0, c0, o0, out_idx_bw, out0, out_idx_bw, out1);
				}

				if (fabs(out0 - out1) > 0.01)
				{
					if (err < 10)
					{
						printf("conv_out_block_wise[%d]=%.3f, out1[%d]=%.3f\n", out_idx_bw, out0, out_idx_bw, out1);
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

static void kernel_sw_full(
       float *w1,
       float *in1,
       float *out3
       ) 
{
	float in1_buf[R * (BS_R + K - 1) * (BS_C + K - 1) * BS_I * I];
	float w1_buf[O * BS_O * K * K * BS_I * I];
	float PE_accum_shift_reg[R * O * BS_R * BS_C * BS_O];
	float out0_buf[R * BS_R * BS_C * BS_O * O];
	float out1_buf[R * BS_R * BS_C * O * BS_O];
	float out2_buf[R * BS_R * BS_C * O * BS_O];
	float out3_buf[R * BS_R * BS_C * O * BS_O];

	const unsigned int cfg_BN_R_max = (cfg_img_row[0] + R * BS_R - 1) / (R * BS_R);
	const unsigned int cfg_BN_C_max = (cfg_img_col[0] + BS_C - 1) / BS_C;
	const unsigned int cfg_BN_O_max = (cfg_out_num[NUM_CONV - 1] + O * BS_O - 1) / (O * BS_O);
	const unsigned int cfg_BN_I_0 = (cfg_in_num[0] + O * BS_O - 1) / (O * BS_O);

	int num_elem_input_fm_bw_0 = cfg_BN_R_max * cfg_BN_C_max * cfg_BN_I_0 * (R * BS_R + K - 1) * (BS_C + K - 1) * O * BS_O;
	int num_elem_input_fm_bw_max = cfg_BN_R_max * cfg_BN_C_max * cfg_BN_O_max * (R * BS_R + K - 1) * (BS_C + K - 1) * O * BS_O;

	float *out5 = (float *)malloc(sizeof(float) * num_elem_input_fm_bw_max);
	float *out5_copy = (float *)malloc(sizeof(float) * num_elem_input_fm_bw_max);

	memcpy(out5, in1, sizeof(float) * num_elem_input_fm_bw_0);

	for (int layer = 0; layer < NUM_CONV; layer++)
	{
		printf("C model: layer %d\n", layer);
		fflush(stdout);

		const unsigned int cfg_BN_R = (cfg_img_row[layer] + R * BS_R - 1) / (R * BS_R);
		const unsigned int cfg_BN_C = (cfg_img_col[layer] + BS_C - 1) / BS_C;
		const unsigned int cfg_BN_O = (cfg_out_num[layer] + O * BS_O - 1) / (O * BS_O);
		const unsigned int cfg_BN_I = (cfg_in_num[layer] + BS_I * I - 1) / (BS_I * I);

		printf("cfg_BN_R=%d, cfg_BN_C=%d, cfg_BN_O=%d, cfg_BN_I=%d\n", cfg_BN_R, cfg_BN_C, cfg_BN_O, cfg_BN_I);
		fflush(stdout);

		int num_elem_input_fm_bw = cfg_BN_R * cfg_BN_C * cfg_BN_I * (R * BS_R + K - 1) * (BS_C + K - 1) * O * BS_O;
		int num_elem_weight_bw = cfg_BN_O * cfg_BN_I * O * BS_O * K * K * BS_I * I;

//		int blk_idx = 0;

//		if (0 == layer)
//		{
//			memcpy(out5, in1, sizeof(float) * num_elem_input_fm_bw);
//		}

		for (int r0 = 0; r0 < cfg_BN_R; r0++)
		for (int c0 = 0; c0 < cfg_BN_C; c0++)
		for (int o0 = 0; o0 < cfg_BN_O; o0++)
		{
#if 0
			// initialize shift register in PE(r2,o2)
			{
				for (int r2 = 0; r2 < R; r2++)
				for (int o2 = 0; o2 < O; o2++) {
					for (int r1 = 0; r1 < BS_R; r1++)
					for (int c1 = 0; c1 < BS_C; c1++)
					for (int o1 = 0; o1 < BS_O; o1++) {
						// PE_accum_shift_reg[r2][o2][r1][c1][o1]
						int reg_idx = (((r2 * O + o2) * BS_R + r1) * BS_C + c1) * BS_O + o1;
	
						PE_accum_shift_reg[reg_idx] = 0.0;
					}
				}
			}
	
			// systolic array
			for (int i0 = 0; i0 < cfg_BN_I; i0++) 
			{
			//	loader_and_feeder_in(layer, in1, in1_buf, r0, c0, i0);
				if ((layer & 1) == 0)
				{
					loader_and_feeder_in(layer, out5, in1_buf, r0, c0, i0);
				}
				else
				{
					loader_and_feeder_in(layer, out5_copy, in1_buf, r0, c0, i0);
				}
				loader_and_feeder_weights(layer, w1, w1_buf, o0, i0);
		
				PE(layer, in1_buf, w1_buf, PE_accum_shift_reg, r0, c0, o0, i0);
			}
			// drain_O
			drain_O(layer, PE_accum_shift_reg, out0_buf, r0, c0, o0);
	
			// address mapping (out0->out1)
			addr_trans(layer, out0_buf, out1_buf, r0, c0, o0);

#if DEBUG_CONV
			if (layer == TEST_LAYER)
			{
				FILE *f_out1;

				f_out1 = fopen("out1.txt", "a");

				if ((r0 + c0 + o0) == 0)
				{
					fprintf(f_out1, "");
				}

				for (int r2 = 0; r2 < R; r2++)
				for (int r1 = 0; r1 < BS_R; r1++)
				for (int c1 = 0; c1 < BS_C; c1++)
				for (int o2 = 0; o2 < O; o2++)
				for (int o1 = 0; o1 < BS_O; o1++)
				{
					int out1_buf_idx = ((r2 * BS_R + r1) * BS_C + c1) * O * BS_O + o2 * BS_O + o1;
		
					fprintf(f_out1, "%.3f\n", out1_buf[out1_buf_idx]);
				}

				fclose(f_out1);
			}
#endif
#endif

		//	if ((layer & 1) == 0)
		//	{
		//		fast_conv(layer, out5, out0_buf, r0, c0, o0);
		//	}
		//	else
		//	{
		//		fast_conv(layer, out5_copy, out0_buf, r0, c0, o0);
		//	}
	
			// relu (out1->out2)
		//	relu_sw(layer, out1_buf, out2_buf);
			relu_sw(layer, out0_buf, out0_buf, r0, c0, o0);

			// pooling (out2->out3)
			if (cfg_pooling_size[layer] > 1)
			{
			//	pooling_sw(layer, out2_buf, out3_buf, r0, c0, o0);
				pooling_sw(layer, out0_buf, out0_buf, r0, c0, o0);
			}

			if (layer < NUM_CONV - 1)
			{
				// boundary padding
			//	if (cfg_pooling_size[layer] > 1)
			//	{
					if ((layer & 1) == 0)
					{
						zero_padding_sw_little(layer, out0_buf, out5_copy, r0, c0, o0);
					}
					else
					{
						zero_padding_sw_little(layer, out0_buf, out5, r0, c0, o0);
					}
			//	}
			//	else
			//	{
			//		if ((layer & 1) == 0)
			//			zero_padding_sw_big(layer, out2_buf, out5_copy, r0, c0, o0);
			//		else
			//			zero_padding_sw_big(layer, out2_buf, out5, r0, c0, o0);
			//	}

			}
			if (layer == NUM_CONV - 1)
			{
				// write to DDR
			//	if (cfg_pooling_size[layer] > 1)
			//	{
				//	write_to_ddr_little(layer, out3_buf, out3, r0, c0, o0);
					write_to_ddr_little(layer, out0_buf, out3, r0, c0, o0);
			//	}
			//	else
			//		write_to_ddr_big(layer, out2_buf, out3, r0, c0, o0);
			}
	
		}

#if DEBUG_ZERO_PADDING
		if (layer == TEST_LAYER)
		{
			if ((layer & 1) == 0)
				print_out5(layer, out5_copy);
			else
				print_out5(layer, out5);
		}
#endif

#if DEBUG_CONV
		if (layer == TEST_LAYER)
		{
			print_out1(layer);
		}
#endif

#if DEBUG_POOLING
		if (layer == NUM_CONV - 1)
			print_out3(layer, out3);
#endif

		w1 += num_elem_weight_bw;
	}

	free(out5);
	free(out5_copy);
}

void c_model(
       float *weights_block_wise,
       float *input_fm_block_wise,
       float *output_fm_block_wise,
	   int *t_idx_addr,
	   int *t_idx_idx,
	   int *t_idx_i,
	   int *t_idx_j
		)
{

	kernel_sw_full(
			weights_block_wise,
			input_fm_block_wise,
			output_fm_block_wise
			);
}

