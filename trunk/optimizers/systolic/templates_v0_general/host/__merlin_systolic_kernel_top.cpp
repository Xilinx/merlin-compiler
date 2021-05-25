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

// System includes 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include <sys/time.h>

//#include <unistd.h>

//#include "systolic_params.h"

#include "__merlin_golden_if.h"
#include "__merlin_opencl_if.h"

#include "__merlinhead_kernel_top.h"

#include "block_wise_reformat.h"

#define GOLDEN_MODEL 1
#define OPENCL_MODEL 1

// problem data
//data_precision *weight;
//data_precision *weight_block_wise;

//data_precision *input_fm;
//data_precision *input_fm_block_wise;

//data_precision *output_fm;
//data_precision *output_fm_c;
//data_precision *output_fm_opencl;
//data_precision *output_fm_block_wise;

// for debug
//data_precision *output_fm_c_block_wise;
//data_precision *output_fm_opencl_block_wise;

// PE info
int *t_idx_addr; // address from drain
int *t_idx_idx; // index from PE(i,j)
int	*t_idx_i; // PE index i
int	*t_idx_j; // PE index j

////////////////////////////////////////
// Check and print out the parameters //
////////////////////////////////////////
void print_params()
{
	printf("\n===== Host-CPU checking the systolic array CNN parameters ======\n\n");

	fflush(stdout);
}


////////////////////////////////////////
// Data preparation for loader IN //
////////////////////////////////////////
void load_a_prepare(data_precision *a, unsigned int num_elem_a, 
		data_precision **p_a_bw, unsigned int num_elem_a_bw)
{
//	int num_elem_input_fm = `$expr_ub_h`;
//	int num_elem_input_fm = BN_R * BN_C * BN_I * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I;

//	if ((input_fm = (data_precision*)acl_aligned_malloc(num_elem_input_fm * sizeof(data_precision))) == NULL) 
//	{
//		perror("Failed malloc of input_fm");
//	}
//	one_array(input_fm, num_elem_input_fm);
//	idx_array(input_fm, num_elem_input_fm);
//	randomize_array(input_fm, num_elem_input_fm);

//	int num_elem_input_fm_bw = BN_I * BN_K * BS_I * PE_ROWS * BS_K * PE_DSIZE;
//	int num_elem_input_fm_bw = `$expr_ub_h`;
//	int num_elem_input_fm_bw = BN_R * BN_C * BN_I * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I;

	if ((*p_a_bw = (data_precision*)acl_aligned_malloc(num_elem_a_bw * sizeof(data_precision))) == NULL) 
	{
		perror("Failed malloc of a_bw");
	}

	printf("Block-wise reformatting of input feature maps!\n");
	fflush(stdout);

	block_wise_reformat_input(a, *p_a_bw);
}

////////////////////////////////////////
// Data preparation for loader W //
////////////////////////////////////////
void load_b_prepare(data_precision *b, unsigned int num_elem_b, 
		data_precision **p_b_bw, unsigned int num_elem_b_bw)
{
//	int num_elem_weight = cfg_out_num[i] * cfg_kernel[i] * cfg_kernel[i] * cfg_in_num[i];
//	int num_elem_weight_bw = cfg_BN_O[i] * cfg_BN_I[i] * O * K * K * BS_I * I;
//	int num_elem_weight = `$expr_ub_v`;
//	int num_elem_weight_bw = `$expr_ub_v`;

//	if ((weight = (data_precision*)acl_aligned_malloc(num_elem_weight * sizeof(data_precision))) == NULL) {
//		perror("Failed malloc of weight");
//	}

	if ((*p_b_bw = (data_precision *)acl_aligned_malloc(num_elem_b_bw * sizeof(data_precision))) == NULL) 
	{
		perror("Failed malloc of weights_block_wise");
	}


//		one_array(weight, num_elem_weight);
//		idx_array(weight, num_elem_weight);
//	randomize_array(weight, num_elem_weight);


	printf("Block-wise reformatting of weight!\n");
	fflush(stdout);
	block_wise_reformat_weight(b, *p_b_bw);
}

////////////////////////////////////////
// Data preparation for drain OUT //
////////////////////////////////////////
void drain_c_prepare(data_precision **p_o_golden,
		data_precision **p_o_c, 
		unsigned int num_elem_o, 
		data_precision **p_o_bw_golden, 
		data_precision **p_o_bw_c, 
		data_precision **p_o_bw_opencl, 
		unsigned int num_elem_o_bw)
{
	if ((*p_o_golden = (data_precision*)acl_aligned_malloc(num_elem_o * sizeof(data_precision))) == NULL) 
	{
		perror("Failed malloc of o_golden");
	}
	if ((*p_o_c = (data_precision*)acl_aligned_malloc(num_elem_o * sizeof(data_precision))) == NULL) 
	{
		perror("Failed malloc of o_c");
	}

	if ((*p_o_bw_golden = (data_precision*)acl_aligned_malloc(num_elem_o_bw * sizeof(data_precision))) == NULL) 
	{
		perror("Failed malloc of o_bw_golden");
	}
	if ((*p_o_bw_c = (data_precision*)acl_aligned_malloc(num_elem_o_bw * sizeof(data_precision))) == NULL) 
	{
		perror("Failed malloc of o_bw_c");
	}
	if ((*p_o_bw_opencl = (data_precision*)acl_aligned_malloc(num_elem_o_bw * sizeof(data_precision))) == NULL) 
	{
		perror("Failed malloc of o_bw_opencl");
	}

	t_idx_addr = (int *)malloc(sizeof(int) * num_elem_o_bw);
	t_idx_idx = (int *)malloc(sizeof(int) * num_elem_o_bw);
	t_idx_i = (int *)malloc(sizeof(int) * num_elem_o_bw);
	t_idx_j = (int *)malloc(sizeof(int) * num_elem_o_bw);
}

void run_opencl_model(data_precision *a_bw,
		data_precision *b_bw,
		data_precision *o_bw_opencl,
		data_precision *o_opencl,
		unsigned int num_elem_o_bw)
{
printf("Starting %s\n", __FUNCTION__);
//	int	num_elem_output_fm = cfg_out_num[NUM_CONV - 1] * cfg_img_row[NUM_CONV - 1] * cfg_img_col[NUM_CONV - 1];
//	int	num_elem_output_fm_bw = cfg_BN_R[NUM_CONV - 1] * cfg_BN_C[NUM_CONV - 1] * cfg_BN_O[NUM_CONV - 1] * R * BS_R * BS_C * O * BS_O;
//	int	num_elem_output_fm_bw = `$expr_ub_o`;

	data_precision *tmp_o_drain = (data_precision *)malloc(sizeof(data_precision) * num_elem_o_bw);

//	memset(output_fm_block_wise, 0, num_elem_output_fm_bw*sizeof(data_precision));
	memset(o_bw_opencl, 0, num_elem_o_bw * sizeof(data_precision));

	opencl_model(
			a_bw,
			b_bw,
		//	output_fm_block_wise
		//	output_fm_opencl_block_wise
			tmp_o_drain	
			);

	// address mapping after drain
//	for (int r0 = 0; r0 < BN_R; r0++)
//	for (int c0 = 0; c0 < BN_C; c0++)
//	for (int o0 = 0; o0 < BN_O; o0++) {
//		for (int r2 = 0; r2 < R; r2++)
//		for (int r1 = 0; r1 < BS_R; r1++)
//		for (int c1 = 0; c1 < BS_C; c1++)
//		for (int o2 = 0; o2 < O; o2++)
//		for (int o1 = 0; o1 < BS_O; o1++) {
//			int out0_idx = (r0 * BN_C * BN_O + c0 * BN_O + o0) * R * BS_R * BS_C * BS_O * O + (r2 * BS_R * BS_C + r1 * BS_C + c1) * BS_O * O + o1 * O + o2;
//			int out1_idx = (r0 * BN_C * BN_O + c0 * BN_O + o0) * R * BS_R * BS_C * BS_O * O + (r2 * BS_R * BS_C + r1 * BS_C + c1) * O * BS_O + o2 * BS_O + o1;
//
//			output_fm_opencl_block_wise[out1_idx] = output_fm_drain[out0_idx];
//		}}
//
	data_reorganization(tmp_o_drain, o_bw_opencl);

	free(tmp_o_drain);

	block_wise_reformat_output(o_bw_opencl, o_opencl);
//	memcpy(output_fm_opencl_block_wise, output_fm_block_wise, sizeof(data_precision) * num_elem_output_fm_bw);
}

void opencl_model_verify(const char *case_name, 
		data_precision *o_bw_c,
		data_precision *o_bw_opencl,
		unsigned int num_elem_o_bw)
{
printf("Starting %s\n", __FUNCTION__);
//	int	num_elem_output_fm = cfg_out_num[NUM_CONV - 1] * cfg_img_row[NUM_CONV - 1] * cfg_img_col[NUM_CONV - 1];
//	int	num_elem_output_fm_bw = cfg_BN_R[NUM_CONV - 1] * cfg_BN_C[NUM_CONV - 1] * cfg_BN_O[NUM_CONV - 1] * R * BS_R * BS_C * O * BS_O;
//	int	num_elem_output_fm_bw = `$expr_ub_o`;

	int err = 0;
	printf("******************************************\n");
//	for (int t = 0; t  < num_elem_output_fm; t++)
	for (int t = 0; t  < num_elem_o_bw; t++)
	{
#if DEBUG_PE == 1
		int t_i = t_idx_i[t];
		int t_j = t_idx_j[t];
		int t_idx = t_idx_idx[t];
		int t_addr = t_idx_addr[t];

	//	if (fabs(output_fm_c[t] - output_fm_opencl[t]) > 0.01)
		if (fabs(o_bw_c[t] - o_bw_opencl[t]) > 0.01)
		{
//			if (err < 10) printf("ERROR: idx=%d addr=%d pe(%d,%d)@%d exp=%lf, out=%lf\n",
//					t, t_addr, t_i, t_j, t_idx, output_fm_c[t], output_fm_opencl[t]);
			if (err < 10) printf("ERROR: idx=%d addr=%d pe(%d,%d)@%d exp=%lf, out=%lf\n",
					t, t_addr, t_i, t_j, t_idx, o_bw_c[t], o_bw_opencl[t]);
			else if (err == 10) printf("...\n");
			err ++;
		}
#else
	//	if (fabs(output_fm_c[t] - output_fm_opencl[t]) > 0.001)
		if (fabs(o_bw_c[t] - o_bw_opencl[t]) > 0.01)
		{
		//	if (err < 10) printf("OPENCL-MODEL ERROR: idx=%d exp=%lf, out=%lf\n",
		//			t, output_fm_c[t], output_fm_opencl[t]);
		//	if (err < 10) printf("OPENCL-MODEL ERROR: idx=%d exp=%lf, out=%lf\n",
		//			t, output_fm_block_wise[t], output_fm_opencl_block_wise[t]);
			if (err < 10) printf("OPENCL-MODEL ERROR: idx=%d exp=%f, out=%f\n",
					t, o_bw_c[t], o_bw_opencl[t]);
			else if (err == 10) printf("...\n");
			err ++;
		}
#endif
	}
	if (err) printf("[OpenCL-model Verification] %d errors\n", err);
	else     printf("[OpenCL-model Verification] PASSED!\n");
	printf("******************************************\n");

	// unit_test error info
	std::ofstream err_log("err.log");
	if (err)
	{
		err_log << case_name << ": " << "Failed" << std::endl;
	}
	err_log.close();

//	dump_data_precision_into_file(output_fm_opencl, num_elem_output_fm , "c_out.dat");
//	dump_data_precision_into_file(output_fm_c, num_elem_output_fm , "c_ref.dat");
}


void run_golden_model(data_precision *a, 
		data_precision *b, 
		unsigned int num_elem_o,
		data_precision *o_golden,
		data_precision *o_golden_bw, 
		unsigned int num_elem_o_bw)
{
printf("Starting %s\n", __FUNCTION__);
	memset(o_golden, 0, num_elem_o * sizeof(data_precision));
	memset(o_golden_bw, 0, num_elem_o_bw * sizeof(data_precision));

	cnn_gold(a, b,
			o_golden,
			o_golden_bw,
			num_elem_o);
}

void cleanup(data_precision *a_bw, 
		data_precision *b_bw, 
		data_precision *o_golden, 
		data_precision *o_bw_golden, 
		data_precision *o_c, 
		data_precision *o_bw_c, 
		data_precision *o_bw_opencl 
		)
{
	// Host resources
//	acl_aligned_free(input_fm);
//	acl_aligned_free(input_fm_block_wise);
//	acl_aligned_free(weight);
//	acl_aligned_free(weight_block_wise);
//	acl_aligned_free(output_fm);
//	acl_aligned_free(output_fm_block_wise);
////	acl_aligned_free(output_fm_c);
//	acl_aligned_free(output_fm_c_block_wise);
//	acl_aligned_free(output_fm_opencl_block_wise);

	acl_aligned_free(a_bw);
	acl_aligned_free(b_bw);
//	acl_aligned_free(c_bw);
	acl_aligned_free(o_golden);
	acl_aligned_free(o_bw_golden);
	acl_aligned_free(o_c);
	acl_aligned_free(o_bw_c);
	acl_aligned_free(o_bw_opencl);

	free(t_idx_addr);
	free(t_idx_idx);
	free(t_idx_i);
	free(t_idx_j);
}

//int main(int argc, const char** argv) {
//void __merlin_systolic_kernel_top(const char *case_name)
void __merlin_systolic_kernel_top(const char *case_name, 
		data_precision *a, unsigned int num_elem_a,
		data_precision *b, unsigned int num_elem_b,
		data_precision *o_opencl, unsigned int num_elem_o)
{
printf("Starting %s\n", __FUNCTION__);
//	printf("%s Starting...\n\n", argv[0]); 
//	print_params();

	data_precision *a_bw;
	data_precision *b_bw;
	data_precision *o_bw;

	// for debug
	data_precision *o_golden;
	data_precision *o_bw_golden;
	// c model output
	data_precision *o_c;
	data_precision *o_bw_c;
	// opencl model output
	data_precision *o_bw_opencl; 

	unsigned int num_elem_a_bw = `$expr_ub_h`;
	unsigned int num_elem_b_bw = `$expr_ub_v`;
	unsigned int num_elem_o_bw = `$expr_ub_o`;


	// loader IN
	load_a_prepare(a, num_elem_a, &a_bw, num_elem_a_bw);

	// loader W
	load_b_prepare(b, num_elem_b, &b_bw, num_elem_b_bw);

	// drain OUT
	drain_c_prepare(&o_golden, &o_c, num_elem_o, 
			&o_bw_golden, &o_bw_c, &o_bw_opencl, num_elem_o_bw);

#if GOLDEN_MODEL
	// golden cnn
	run_golden_model(a, b, 
			num_elem_o, 
			o_golden, o_bw_golden, 
			num_elem_o_bw);
#endif

#if (OPENCL_MODEL == 1)
	// OpenCL model
	run_opencl_model(a_bw, b_bw,
			o_bw_opencl,
			o_opencl,
			num_elem_o_bw);
#endif

//	opencl_model_verify(argv[0]);
#if (TEST_GOLDEN_BW == 1)
	opencl_model_verify(case_name, o_bw_golden, o_bw_opencl, num_elem_o_bw);
#else
	opencl_model_verify(case_name, o_golden, o_opencl, num_elem_o);
#endif

	cleanup(a_bw, b_bw, 
			o_golden, o_bw_golden, 
			o_c, o_bw_c, o_bw_opencl);
}

