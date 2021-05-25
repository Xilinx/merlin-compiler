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

#include "systolic_params.h"

#include "cnn.h"

#include "util.h"

// load a block from input and distribute it among feeders
// feeder(r2)
static void loader_and_feeder_in(
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
	

	for (int r2 = 0; r2 < R; r2++) {
		for (int r1_p = 0; r1_p < (BS_R + K - 1); r1_p++)
		for (int c1_q = 0; c1_q < (BS_C + K - 1); c1_q++)
		for (int i1 = 0; i1 < BS_I; i1++)
		for (int i2 = 0; i2 < I; i2++) {
			int in1_idx = (r0 * BN_C * BN_I + c0 * BN_I + i0) * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + ((r2 * BS_R + r1_p) * (BS_C + K - 1) + c1_q) * BS_I * I + i1 * I + i2;
			int in1_buf_idx = r2 * (BS_R + K - 1) * (BS_C + K - 1) * BS_I * I + (r1_p * (BS_C + K - 1) + c1_q) * BS_I * I + i1 * I + i2;

			in1_buf[in1_buf_idx] = in1[in1_idx];
		}}
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
		float *w1, float *w1_buf,
		int o0, int i0)
{
	for (int o2 = 0; o2 < O; o2++) {
	for (int p = 0; p < K; p++)
	for (int q = 0; q < K; q++)
	for (int i1 = 0; i1 < BS_I; i1++)
	for (int o1 = 0; o1 < BS_O; o1++) {
	for (int i2 = 0; i2 < I; i2++) {
			// w1_buf[o2][o1][p][q][i1][i2]=w1[o0][i0][o2][o1][p][q][i1][i2]
			int w1_idx = (o0 * BN_I + i0) * O * BS_O * K * K * BS_I * I + (((o2 * BS_O + o1) * K + p) * K + q) * BS_I * I + i1 * I + i2;
			int w1_buf_idx = o2 * BS_O * K * K * BS_I * I + ((o1 * K + p) * K + q) * BS_I * I + i1 * I + i2;

			w1_buf[w1_buf_idx] = w1[w1_idx];
		}}}

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
		float *in1_buf, float *w1_buf, 
		float *PE_accum_shift_reg, 
		int r0,	int c0, int o0, int i0)
{
	for (int r2 = 0; r2 < R; r2++)
	for (int o2 = 0; o2 < O; o2++) {
		for (int p = 0; p < K; p++)
		for (int q = 0; q < K; q++)
		for (int i1 = 0; i1 < BS_I; i1++) {
			for (int c1 = 0; c1 < BS_C; c1++)
			for (int r1 = 0; r1 < BS_R; r1++)
			for (int o1 = 0; o1 < BS_O; o1++) {
				int reg_idx = (((r2 * O + o2) * BS_R + r1) * BS_C + c1) * BS_O + o1;
				for (int i2 = 0; i2 < I; i2++) {
					int in1_buf_idx = r2 * (BS_R + K - 1) * (BS_C + K - 1) * BS_I * I  + ((r1 + p) * (BS_C + K - 1) + (c1 + q)) * BS_I * I + i1 * I + i2;
					int w1_buf_idx = o2 * BS_O * K * K * BS_I * I + ((o1 * K + p) * K + q) * BS_I * I + i1 * I + i2;

					PE_accum_shift_reg[reg_idx] += in1_buf[in1_buf_idx] * w1_buf[w1_buf_idx];

				}

				//; if ($debug_pe_idx eq "on") {

				int row = r2;
				int col = o2;
				// (r0,c0,o0,r1,c1,o1)
				int pe_idx = (r0 * BN_C * BN_O + c0 * BN_O + o0) * BS_R * BS_C * BS_O + (r1 * BS_C + c1) * BS_O + o1;
				// (r0,c0,o0,r2,r1,c1,o1,o2)
				int addr = (r0 * BN_C * BN_O + c0 * BN_O + o0) * R * BS_R * BS_C * BS_O * O + (r2 * BS_R * BS_C + r1 * BS_C + c1) * BS_O * O + o1 * O + o2; // address from drain

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

// drain_O for partial validation
static void drain_O(
		float *PE_accum_shift_reg, float *out0, 
		int r0, int c0, int o0)
{
	for (int r2 = 0; r2 < R; r2++)
	for (int r1 = 0; r1 < BS_R; r1++)
	for (int c1 = 0; c1 < BS_C; c1++)
	for (int o1 = 0; o1 < BS_O; o1++)
	for (int o2 = 0; o2 < O; o2++) {
		int reg_idx = (((r2 * O + o2) * BS_R + r1) * BS_C + c1) * BS_O + o1;
		int out0_idx = (r0 * BN_C * BN_O + c0 * BN_O + o0) * R * BS_R * BS_C * BS_O * O + (r2 * BS_R * BS_C + r1 * BS_C + c1) * BS_O * O + o1 * O + o2;

		out0[out0_idx] = PE_accum_shift_reg[reg_idx];
	}
}

static void kernel_sw_partial(
       float *weight_block_wise,
       float *input_fm_block_wise,
	   float *output_fm_block_wise,
	   int	*t_idx_addr,
	   int	*t_idx_idx,
	   int	*t_idx_i,
	   int	*t_idx_j
       ) 
{
	printf("%s: %d\n", __func__, __LINE__);

	float in1_buf[R * (BS_R + K - 1) * (BS_C + K - 1) * BS_I * I];
	float w1_buf[O * BS_O * K * K * BS_I * I];
	float PE_accum_shift_reg[R * O * BS_R * BS_C * BS_O];

	float *w1 = weight_block_wise;
	float *in1 = input_fm_block_wise;

	int num_elem_input_fm_bw = BN_R * BN_C * BN_I * (R * BS_C + K - 1) * (BS_C + K - 1)  * BS_I * I;
	int num_elem_weight_bw = BN_O * BN_I * O * K * K * BS_I * I;
	int	num_elem_output_fm_bw = BN_R * BN_C * BN_O * R * BS_R * BS_C * O * BS_O;

	float *out0 = (float *)malloc(num_elem_output_fm_bw * sizeof(float));
	float *out1 = (float *)malloc(num_elem_output_fm_bw * sizeof(float));

	//	const unsigned int cfg_BN_R = (cfg_img_row[layer] + R * BS_R - 1) / (R * BS_R);
	//	const unsigned int cfg_BN_C = (cfg_img_col[layer] + BS_C - 1) / BS_C;
	//	const unsigned int cfg_BN_O = (cfg_out_num[layer] + O - 1) / O;
	//	const unsigned int cfg_BN_I = (cfg_in_num[layer] + BS_I * I - 1) / (BS_I * I);

		/* This scope supposes match with our DAC submission, that is,
		 * out0 stores original systolic array output from drain. */
		for (int r0 = 0; r0 < BN_R; r0++)
		for (int c0 = 0; c0 < BN_C; c0++)
		for (int o0 = 0; o0 < BN_O; o0++) {
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
	
		for (int i0 = 0; i0 < BN_I; i0++) {
	
			loader_and_feeder_in(in1, in1_buf, r0, c0, i0);
			loader_and_feeder_weights(w1, w1_buf, o0, i0);
	
			PE(in1_buf, w1_buf, PE_accum_shift_reg, r0, c0, o0, i0);
			
		}
		drain_O(PE_accum_shift_reg, out0, r0, c0, o0);
		/* End of partial validation*/
	
		}
	
		// address mapping (out0->out1, in DDR for partial validation)
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
	
				out1[out1_idx] = out0[out0_idx];
	
	//			t_idx_addr[out1_idx] = out0_idx;
	//			t_idx_i[out1_idx] = r2;
	//			t_idx_j[out1_idx] = o2;
	//			t_idx_idx[out1_idx] = (r0 * cfg_BN_C * cfg_BN_O + c0 * cfg_BN_O + o0) * BS_R * BS_C * BS_O + (r1 * BS_C + c1) * BS_O + o1;
	
				t_idx_addr[out0_idx] = out0_idx;
				t_idx_i[out0_idx] = r2;
				t_idx_j[out0_idx] = o2;
				t_idx_idx[out0_idx] = (r0 * BN_C * BN_O + c0 * BN_O + o0) * BS_R * BS_C * BS_O + (r1 * BS_C + c1) * BS_O + o1;
			}}
		
			// copy to final output
			memcpy(output_fm_block_wise, out1, num_elem_output_fm_bw * sizeof(float));
	//	}
	
		free(out0);
		free(out1);
}

void c_model(
       float *weight_block_wise,
       float *input_fm_block_wise,
       float *output_fm_block_wise,
	   int *t_idx_addr,
	   int *t_idx_idx,
	   int *t_idx_i,
	   int *t_idx_j
		)
{

	kernel_sw_partial(
			weight_block_wise,
			input_fm_block_wise,
			output_fm_block_wise,
			t_idx_addr,
			t_idx_idx,
			t_idx_i,
			t_idx_j
			);
}

