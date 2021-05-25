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

#include "systolic_params.h"

#include "cnn.h"
#include "c_model.h"
#include "opencl_if.h"

#include "util.h"

#define GOLDEN_MODEL 0
#define C_MODEL 0
#define OPENCL_MODEL 1

// problem data
data_precision *weights;
data_precision *weights_block_wise;

data_precision *input_fm;
data_precision *input_fm_block_wise;

data_precision *output_fm;
data_precision *output_fm_c;
data_precision *output_fm_opencl;
data_precision *output_fm_block_wise;

// for debug
data_precision *output_fm_c_block_wise;
data_precision *output_fm_opencl_block_wise;

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
void load_IN_prepare()
{
	int	num_elem_input_fm = (cfg_img_row[0] + cfg_kernel[0] - 1) * (cfg_img_col[0] + cfg_kernel[0] - 1) * cfg_in_num[0];

	if ((input_fm = (data_precision*)acl_aligned_malloc(num_elem_input_fm * sizeof(data_precision))) == NULL) {
		perror("Failed malloc of input_fm");
	}
//	one_array(input_fm, num_elem_input_fm);
//	idx_array(input_fm, num_elem_input_fm);
	randomize_array(input_fm, num_elem_input_fm);

	int num_elem_input_fm_bw = cfg_BN_R[0] * cfg_BN_C[0] * cfg_BN_I[0] * (R * BS_R + K - 1) * (BS_C + K - 1)  * BS_I * I;

	if ((input_fm_block_wise = (data_precision*)acl_aligned_malloc(num_elem_input_fm_bw * sizeof(data_precision))) == NULL) {
		perror("Failed malloc of input_fm");
	}

	printf("Block-wise reformatting of input feature maps!\n");
	block_wise_reformat_input(0, input_fm, input_fm_block_wise);
}

////////////////////////////////////////
// Data preparation for loader W //
////////////////////////////////////////
void load_W_prepare()
{
	int num_elem_weights = 0;
	int num_elem_weights_bw = 0;

	for (int i = 0; i < NUM_CONV; i++) 
	{
		int num_elem_weight = cfg_out_num[i] * cfg_kernel[i] * cfg_kernel[i] * cfg_in_num[i];
		int num_elem_weight_bw = cfg_BN_O[i] * cfg_BN_I[i] * O * K * K * BS_I * I;

		num_elem_weights += num_elem_weight;
		num_elem_weights_bw += num_elem_weight_bw;
	}

	if ((weights = (data_precision*)acl_aligned_malloc(num_elem_weights * sizeof(data_precision))) == NULL) {
		perror("Failed malloc of weights");
	}
	if ((weights_block_wise = (data_precision*)acl_aligned_malloc(num_elem_weights_bw * sizeof(data_precision))) == NULL) {
		perror("Failed malloc of weights_block_wise");
	}

	data_precision *weight = weights, *weight_block_wise = weights_block_wise;

	for (int i = 0; i < NUM_CONV; i++)
	{
		int num_elem_weight = cfg_out_num[i] * cfg_kernel[i] * cfg_kernel[i] * cfg_in_num[i];
		int num_elem_weight_bw = cfg_BN_O[i] * cfg_BN_I[i] * O * K * K * BS_I * I;
	//	one_array(weight, num_elem_weight);
	//	idx_array(weight, num_elem_weight);
		randomize_array(weight, num_elem_weight);

		printf("Block-wise reformatting of weights[%d]!\n", i);

		//	block_wise_reformat_weights(weights, weights_block_wise);
		block_wise_reformat_weights(i, weight, weight_block_wise);

		weight += num_elem_weight;
		weight_block_wise += num_elem_weight_bw;
	}
}

////////////////////////////////////////
// Data preparation for drain OUT //
////////////////////////////////////////
void drain_OUT_prepare()
{
	int	num_elem_output_fm = cfg_out_num[NUM_CONV - 1] * cfg_img_row[NUM_CONV - 1] * cfg_img_col[NUM_CONV - 1];
	int	num_elem_output_fm_bw = cfg_BN_R[NUM_CONV - 1] * cfg_BN_C[NUM_CONV - 1] * cfg_BN_O[NUM_CONV - 1] * R * BS_R * BS_C * O * BS_O;

//	printf("num_elem_output_fm=%d, num_elem_output_fm_bw=%d\n", num_elem_output_fm, num_elem_output_fm_bw);
//	fflush(stdout);
	if ((output_fm_block_wise = (data_precision*)acl_aligned_malloc(num_elem_output_fm_bw * sizeof(data_precision))) == NULL) {
		perror("Failed malloc of output_fm_block_wise");
	}
//	memset(output_fm_block_wise, 0, num_elem_output_fm*sizeof(data_precision));

	if ((output_fm = (data_precision*)acl_aligned_malloc(num_elem_output_fm*sizeof(data_precision))) == NULL) {
		perror("Failed malloc of drain_C");
	}
//	memset(output_fm, 0, num_elem_output_fm*sizeof(data_precision));

	if((output_fm_c = (data_precision*)acl_aligned_malloc(num_elem_output_fm*sizeof(data_precision))) == NULL) {
		perror("Failed malloc of drain_C_1");
	}
//	memset(output_fm_c, 0, num_elem_output_fm*sizeof(data_precision));

	if((output_fm_opencl = (data_precision*)acl_aligned_malloc(num_elem_output_fm*sizeof(data_precision))) == NULL) {
		perror("Failed malloc of drain_C_2");
	}
//	memset(output_fm_opencl, 0, num_elem_output_fm*sizeof(data_precision));

	output_fm_c_block_wise = (data_precision *)malloc(sizeof(data_precision) * num_elem_output_fm_bw);
	output_fm_opencl_block_wise = (data_precision *)malloc(sizeof(data_precision) * num_elem_output_fm_bw);

	t_idx_addr = (int *)malloc(sizeof(int) * num_elem_output_fm_bw);
	t_idx_idx = (int *)malloc(sizeof(int) * num_elem_output_fm_bw);
	t_idx_i = (int *)malloc(sizeof(int) * num_elem_output_fm_bw);
	t_idx_j = (int *)malloc(sizeof(int) * num_elem_output_fm_bw);
}

void run_opencl_model()
{
//	int	num_elem_output_fm = cfg_out_num[NUM_CONV - 1] * cfg_img_row[NUM_CONV - 1] * cfg_img_col[NUM_CONV - 1];
	int	num_elem_output_fm_bw = cfg_BN_R[NUM_CONV - 1] * cfg_BN_C[NUM_CONV - 1] * cfg_BN_O[NUM_CONV - 1] * R * BS_R * BS_C * O * BS_O;

	memset(output_fm_block_wise, 0, num_elem_output_fm_bw*sizeof(data_precision));

	opencl_model(
			weights_block_wise,
			input_fm_block_wise,
			output_fm_block_wise
			);

	block_wise_reformat_output(NUM_CONV - 1, output_fm_block_wise, output_fm_opencl);
	memcpy(output_fm_opencl_block_wise, output_fm_block_wise, sizeof(data_precision) * num_elem_output_fm_bw);
}

void opencl_model_verify()
{
	int	num_elem_output_fm = cfg_out_num[NUM_CONV - 1] * cfg_img_row[NUM_CONV - 1] * cfg_img_col[NUM_CONV - 1];
	int	num_elem_output_fm_bw = cfg_BN_R[NUM_CONV - 1] * cfg_BN_C[NUM_CONV - 1] * cfg_BN_O[NUM_CONV - 1] * R * BS_R * BS_C * O * BS_O;

	int err = 0;
	printf("******************************************\n");
//	for (int t = 0; t  < num_elem_output_fm; t++)
	for (int t = 0; t  < num_elem_output_fm_bw; t++)
	{
#if DEBUG_PE == 1
		int t_i = t_idx_i[t];
		int t_j = t_idx_j[t];
		int t_idx = t_idx_idx[t];
		int t_addr = t_idx_addr[t];

	//	if (fabs(output_fm_c[t] - output_fm_opencl[t]) > 0.01)
		if (fabs(output_fm_c_block_wise[t] - output_fm_opencl_block_wise[t]) > 0.01)
		{
//			if (err < 10) printf("ERROR: idx=%d addr=%d pe(%d,%d)@%d exp=%lf, out=%lf\n",
//					t, t_addr, t_i, t_j, t_idx, output_fm_c[t], output_fm_opencl[t]);
			if (err < 10) printf("ERROR: idx=%d addr=%d pe(%d,%d)@%d exp=%lf, out=%lf\n",
					t, t_addr, t_i, t_j, t_idx, output_fm_c_block_wise[t], output_fm_opencl_block_wise[t]);
			else if (err == 10) printf("...\n");
			err ++;
		}
#else
		if (fabs(output_fm_c[t] - output_fm_opencl[t]) > 0.001)
		{
			if (err < 10) printf("OPENCL-MODEL ERROR: idx=%d exp=%lf, out=%lf\n",
					t, output_fm_c[t], output_fm_opencl[t]);
			else if (err == 10) printf("...\n");
			err ++;
		}
#endif
	}
	if (err) printf("[OpenCL-model Verification] %d errors\n", err);
	else     printf("[OpenCL-model Verification] PASSED!\n");
	printf("******************************************\n");

//	dump_data_precision_into_file(output_fm_opencl, num_elem_output_fm , "c_out.dat");
//	dump_data_precision_into_file(output_fm_c, num_elem_output_fm , "c_ref.dat");
}

void run_c_model()
{
//	int	num_elem_output_fm = cfg_out_num[NUM_CONV - 1] * cfg_img_row[NUM_CONV - 1] * cfg_img_col[NUM_CONV - 1];
	int	num_elem_output_fm_bw = cfg_BN_R[NUM_CONV - 1] * cfg_BN_C[NUM_CONV - 1] * cfg_BN_O[NUM_CONV - 1] * R * BS_R * BS_C * O * BS_O;

	memset(output_fm_block_wise, 0, num_elem_output_fm_bw*sizeof(data_precision));

	c_model(
			weights_block_wise,
			input_fm_block_wise,
			output_fm_block_wise,
			t_idx_addr,
			t_idx_idx,
			t_idx_i,
			t_idx_j
			);

	block_wise_reformat_output(NUM_CONV - 1, output_fm_block_wise, output_fm_c);
	memcpy(output_fm_c_block_wise, output_fm_block_wise, sizeof(data_precision) * num_elem_output_fm_bw);
}

void c_model_verify()
{
	int	num_elem_output_fm = cfg_out_num[NUM_CONV - 1] * cfg_img_row[NUM_CONV - 1] * cfg_img_col[NUM_CONV - 1];

	int err = 0;
	printf("******************************************\n");
	for (int t = 0; t  < num_elem_output_fm; t++)
	{
		if (fabs(output_fm[t] - output_fm_c[t]) > 0.001)
		{
			if (err < 10) printf("C-MODEL ERROR: idx=%d exp=%lf, out=%lf\n",
					t, output_fm[t], output_fm_c[t]);
			else if (err == 10) printf("...\n");
			err ++;
		}
	//	if (fabs(output_fm[t] - output_fm_c[t]) > 0.001)
	//	{
	//		printf("C-MODEL ERROR: idx=%d exp=%lf, out=%lf\n",
	//				t, output_fm[t], output_fm_c[t]);
	//	}
	}
	if (err) printf("[C-model Verification] %d errors\n", err);
	else     printf("[C-model Verification] PASSED!\n");
	printf("******************************************\n");

}

void run_golden_model()
{
	int	num_elem_output_fm = cfg_out_num[NUM_CONV - 1] * cfg_img_row[NUM_CONV - 1] * cfg_img_col[NUM_CONV - 1];

	memset(output_fm, 0, num_elem_output_fm*sizeof(data_precision));

	cnn_gold(weights,
			input_fm,
			output_fm);
}

void cleanup()
{
	// Host resources
	acl_aligned_free(weights);
	acl_aligned_free(input_fm);
	acl_aligned_free(output_fm);
	acl_aligned_free(output_fm_c);
	acl_aligned_free(output_fm_opencl);

	// Release the OpenCL resources
	//----------------------------------------------

//	for(int i=0; i<NUM_QUEUES_TO_CREATE; i++) {
//		clReleaseCommandQueue(cmdQueue[i]);
//	}


//	clReleaseProgram(program);
//	clReleaseContext(context);
//
//	acl_aligned_free(platforms);
//	acl_aligned_free(devices);

	free(output_fm_c_block_wise);
	free(output_fm_opencl_block_wise);

	free(t_idx_addr);
	free(t_idx_idx);
	free(t_idx_i);
	free(t_idx_j);
}

int main(int argc, const char** argv) {

	printf("%s Starting...\n\n", argv[0]); 

//	print_params();

	// loader IN
	load_IN_prepare();

	// loader W
	load_W_prepare();

	// drain OUT
	drain_OUT_prepare();

#if C_MODEL
	// golden cnn
	run_golden_model();
#endif

#if C_MODEL
  // C model
	run_c_model();

	c_model_verify();
#endif

#if OPENCL_MODEL
	// OpenCL model
	run_opencl_model();

	opencl_model_verify();
#endif

	cleanup();
}

