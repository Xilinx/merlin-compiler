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

#define C_MODEL 1
#define OPENCL_MODEL 1

// problem data
float *weight;
float *weight_block_wise;

float *input_fm;
float *input_fm_block_wise;

float *output_fm;
float *output_fm_c;
float *output_fm_opencl;
float *output_fm_block_wise;

// for debug
float *output_fm_c_block_wise;
float *output_fm_opencl_block_wise;

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
//	int	num_elem_input_fm = (cfg_img_row[0] + cfg_kernel[0] - 1) * (cfg_img_col[0] + cfg_kernel[0] - 1) * cfg_in_num[0];
	int	num_elem_input_fm = (BN_R * BS_R * R + K - 1) * (BN_C * BS_C + K - 1) * (BN_I * BS_I * I);

	if ((input_fm = (float*)acl_aligned_malloc(num_elem_input_fm * sizeof(float))) == NULL) {
		perror("Failed malloc of input_fm");
	}
//	one_array(input_fm, num_elem_input_fm);
//	idx_array(input_fm, num_elem_input_fm);
	randomize_array(input_fm, num_elem_input_fm);

//	int num_elem_input_fm_bw = cfg_BN_R[0] * cfg_BN_C[0] * cfg_BN_I[0] * (R * BS_R + K - 1) * (BS_C + K - 1)  * BS_I * I;
	int num_elem_input_fm_bw = BN_R * BN_C * BN_I * (R * BS_R + K - 1) * (BS_C + K - 1)  * BS_I * I;

	if ((input_fm_block_wise = (float*)acl_aligned_malloc(num_elem_input_fm_bw * sizeof(float))) == NULL) {
		perror("Failed malloc of input_fm");
	}

	printf("Block-wise reformatting of input feature maps!\n");
	block_wise_reformat_input(input_fm, input_fm_block_wise);
}

////////////////////////////////////////
// Data preparation for loader W //
////////////////////////////////////////
void load_W_prepare()
{
	int num_elem_weight = (BN_O * BS_O * O) * K * K * (BN_I * BS_I * I);
	int num_elem_weight_bw = BN_O * BN_I * BS_O * O * K * K * BS_I * I;

	if ((weight = (float*)acl_aligned_malloc(num_elem_weight * sizeof(float))) == NULL) 
	{
		perror("Failed malloc of weight");
	}
	if ((weight_block_wise = (float*)acl_aligned_malloc(num_elem_weight_bw * sizeof(float))) == NULL) 
	{
		perror("Failed malloc of weight_block_wise");
	}

	//	one_array(weight, num_elem_weight);
	//	idx_array(weight, num_elem_weight);
	randomize_array(weight, num_elem_weight);

	printf("Block-wise reformatting of weight!\n");

	block_wise_reformat_weight(weight, weight_block_wise);
}

////////////////////////////////////////
// Data preparation for drain OUT //
////////////////////////////////////////
void drain_OUT_prepare()
{
	int	num_elem_output_fm = (BN_R * BS_R * R) * (BN_C * BS_C) * (BN_O * BS_O * O);
	int	num_elem_output_fm_bw = BN_R * BN_C * BN_O * R * BS_R * BS_C * O * BS_O;

//	printf("num_elem_output_fm=%d, num_elem_output_fm_bw=%d\n", num_elem_output_fm, num_elem_output_fm_bw);
//	fflush(stdout);
	if ((output_fm_block_wise = (float*)acl_aligned_malloc(num_elem_output_fm_bw * sizeof(float))) == NULL) {
		perror("Failed malloc of output_fm_block_wise");
	}
//	memset(output_fm_block_wise, 0, num_elem_output_fm*sizeof(float));

	if ((output_fm = (float*)acl_aligned_malloc(num_elem_output_fm*sizeof(float))) == NULL) {
		perror("Failed malloc of drain_C");
	}
//	memset(output_fm, 0, num_elem_output_fm*sizeof(float));

	if((output_fm_c = (float*)acl_aligned_malloc(num_elem_output_fm*sizeof(float))) == NULL) {
		perror("Failed malloc of drain_C_1");
	}
//	memset(output_fm_c, 0, num_elem_output_fm*sizeof(float));

	if((output_fm_opencl = (float*)acl_aligned_malloc(num_elem_output_fm*sizeof(float))) == NULL) {
		perror("Failed malloc of drain_C_2");
	}
//	memset(output_fm_opencl, 0, num_elem_output_fm*sizeof(float));

	output_fm_c_block_wise = (float *)malloc(sizeof(float) * num_elem_output_fm_bw);
	output_fm_opencl_block_wise = (float *)malloc(sizeof(float) * num_elem_output_fm_bw);

	t_idx_addr = (int *)malloc(sizeof(int) * num_elem_output_fm_bw);
	t_idx_idx = (int *)malloc(sizeof(int) * num_elem_output_fm_bw);
	t_idx_i = (int *)malloc(sizeof(int) * num_elem_output_fm_bw);
	t_idx_j = (int *)malloc(sizeof(int) * num_elem_output_fm_bw);
}

void run_opencl_model()
{
	int	num_elem_output_fm_bw = BN_R * BN_C * BN_O * R * BS_R * BS_C * O * BS_O;

	memset(output_fm_block_wise, 0, num_elem_output_fm_bw*sizeof(float));

	opencl_model(
			weight_block_wise,
			input_fm_block_wise,
			output_fm_block_wise
			);

	block_wise_reformat_output(output_fm_block_wise, output_fm_opencl);
	memcpy(output_fm_opencl_block_wise, output_fm_block_wise, sizeof(float) * num_elem_output_fm_bw);
}

void opencl_model_verify()
{
	int	num_elem_output_fm = (BN_R * BS_R * R) * (BN_C * BS_C) * (BN_O * BS_O * O);
	int	num_elem_output_fm_bw = BN_R * BN_C * BN_O * R * BS_R * BS_C * O * BS_O;

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

//	dump_float_into_file(output_fm_opencl, num_elem_output_fm , "c_out.dat");
//	dump_float_into_file(output_fm_c, num_elem_output_fm , "c_ref.dat");
}

void run_c_model()
{
	int	num_elem_output_fm_bw = BN_R * BN_C * BN_O * R * BS_R * BS_C * O * BS_O;

	memset(output_fm_block_wise, 0, num_elem_output_fm_bw*sizeof(float));

	c_model(
			weight_block_wise,
			input_fm_block_wise,
			output_fm_block_wise,
			t_idx_addr,
			t_idx_idx,
			t_idx_i,
			t_idx_j
			);

	block_wise_reformat_output(output_fm_block_wise, output_fm_c);
	memcpy(output_fm_c_block_wise, output_fm_block_wise, sizeof(float) * num_elem_output_fm_bw);
}

void c_model_verify()
{
	int	num_elem_output_fm = (BN_R * BS_R * R) * (BN_C * BS_C) * (BN_O * BS_O * O);

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
	}
	if (err) printf("[C-model Verification] %d errors\n", err);
	else     printf("[C-model Verification] PASSED!\n");
	printf("******************************************\n");

}

void run_golden_model()
{
	int	num_elem_output_fm = (BN_R * BS_R * R) * (BN_C * BS_C) * (BN_O * BS_O * O);

	memset(output_fm, 0, num_elem_output_fm*sizeof(float));

	cnn_gold(weight,
			input_fm,
			output_fm);
}

void cleanup()
{
	// Host resources
	acl_aligned_free(weight);
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

	// golden cnn
	run_golden_model();

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

