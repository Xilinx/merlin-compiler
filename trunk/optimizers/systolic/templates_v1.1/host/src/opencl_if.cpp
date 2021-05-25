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

#pragma message ("* Compiling for ALTERA CL")
#include "CL/opencl.h"

#include "opencl_if.h"

#include "systolic_params.h"

#include "util.h"

#include "cnn.h"

// set the kernel names (kernel functions)
#include "host_kernel_strings.h"

#ifndef EMULATOR
  #define AOCX_FILE "kernel_top.aocx"
#else
  #define AOCX_FILE "kernel_top.sim.aocx"
#endif

// OpenCL environment
//typedef cl_char cl_data_precision;
typedef cl_float cl_data_precision;

cl_int status;
cl_uint numPlatforms = 0;
cl_platform_id* platforms = NULL;
cl_uint numDevices = 0;
cl_device_id* devices = NULL;
cl_context context = NULL;
cl_command_queue cmdQueue[NUM_QUEUES_TO_CREATE + 1]; // extra queue for reading buffer C
cl_program program;

// test and measurement
double k_start_time[NUM_QUEUES_TO_FINISH];
double k_end_time[NUM_QUEUES_TO_FINISH];
double k_exec_time[NUM_QUEUES_TO_FINISH];
//int * t_idx_addr;
//int * t_idx_idx;
//int * t_idx_i;
////int * t_idx_j = new int [num_elem_output_fm];
//int * t_idx_j;

void opencl_setup()
{
	FILE *f_out = stdout;
	unsigned int i;

	printf("\n===== Host-CPU setting up the OpenCL platform and device ======\n\n");


	status = clGetPlatformIDs(0, NULL, &numPlatforms);
	fprintf(stdout,"Number of platforms = %d\n", numPlatforms);

	// Allocate enough space for each platform
	platforms = (cl_platform_id*) acl_aligned_malloc (numPlatforms * sizeof(cl_platform_id));
	printf("Allocated space for Platform\n");

	// Fill in platforms with clGetPlatformIDs()
	status = clGetPlatformIDs(numPlatforms, platforms, NULL); CHECK(status);
	printf("Filled in platforms\n");    

	//----------------------------------------------
	// Discover and initialize the devices 
	//----------------------------------------------

	// Device info
	char buffer[4096];
	unsigned int buf_uint;
	int device_found = 0;

	printf("Initializing IDs\n");    
	for (i=0; i<numPlatforms; i++) {
		status = clGetDeviceIDs(platforms[i],
				CL_DEVICE_TYPE_ALL,
				0,
				NULL,
				&numDevices); 

		if(status == CL_SUCCESS){
			clGetPlatformInfo(platforms[i], 
					CL_PLATFORM_VENDOR,
					4096,
					buffer,
					NULL);
			if(strstr(buffer, "Altera") != NULL){
				device_found = 1;
			}
		//	if(strstr(buffer, "Intel") != NULL){
		//		device_found = 1;
		//	}
			printf("%s\n", buffer);

			if(device_found){
				// Allocate enough space for each device
				devices = (cl_device_id*)
					acl_aligned_malloc (numDevices * sizeof(cl_device_id));

				// Fill in devices with clGetDeviceIDs()
				status = clGetDeviceIDs(platforms[i],
						CL_DEVICE_TYPE_ALL,
						numDevices,
						devices,
						NULL);
				break;
			}
		}
	}

	if(!device_found) {
		printf("failed to find a OpenCL device\n");
		exit(-1);
	}

	for (i = 0; i < numDevices; i++) {
		clGetDeviceInfo(devices[i],
				CL_DEVICE_NAME,
				4096,
				buffer,
				NULL);
		fprintf(f_out, "\nDevice Name: %s\n", buffer);

		clGetDeviceInfo(devices[i],
				CL_DEVICE_VENDOR,
				4096,
				buffer,
				NULL);
		fprintf(f_out, "Device Vendor: %s\n", buffer);

		clGetDeviceInfo(devices[i],
				CL_DEVICE_MAX_COMPUTE_UNITS,
				sizeof(buf_uint),
				&buf_uint,
				NULL);
		fprintf(f_out, "Device Computing Units: %u\n", buf_uint);

		clGetDeviceInfo(devices[i],
				CL_DEVICE_GLOBAL_MEM_SIZE,
				sizeof(unsigned long),
				&buffer,
				NULL);
		fprintf(f_out, "Global Memory Size: %i\n", *((unsigned long*)buffer));

		clGetDeviceInfo(devices[i],
				CL_DEVICE_MAX_MEM_ALLOC_SIZE,
				sizeof(unsigned long),
				&buffer,
				NULL);
		fprintf(f_out, "Global Memory Allocation Size: %i\n\n", *((unsigned long*)buffer));
	}



	//----------------------------------------------
	// Create a context 
	//----------------------------------------------

	printf("\n===== Host-CPU setting up the OpenCL command queues ======\n\n");


	// Create a context using clCreateContext() and
	// associate it with the device

	context = clCreateContext(
			NULL,
			1,
			devices,
			NULL,
			NULL,
			&status); CHECK(status);

	//----------------------------------------------
	// Create command queues
	//---------------------------------------------


	// Create a command queue using clCreateCommandQueue(),
	// and associate it with the device you want to execute on
	for(i=0; i<NUM_QUEUES_TO_CREATE; i++) {
		//fprintf(stdout,"cmdQueue i = %d, kernel name = %s\n", i, kernel_name[i]);
		cmdQueue[i] = clCreateCommandQueue(
				context,
				devices[0],
				CL_QUEUE_PROFILING_ENABLE,
				&status); CHECK(status);
	}

	//fprintf(stdout,"cmdQueue i = %d, a queue for reading the C buffer\n", i);
	cmdQueue[i] = clCreateCommandQueue(
			context,
			devices[0],
			CL_QUEUE_PROFILING_ENABLE,
			&status); CHECK(status);

	//----------------------------------------------
	// Create device buffers
	//----------------------------------------------

	//----------------------------------------------
	// Write host data to device buffers
	//----------------------------------------------


	//----------------------------------------------
	// Create the program from binaries
	//----------------------------------------------
	printf("\n===== Host-CPU setting up OpenCL program and kernels ======\n\n");


	size_t binary_length;
	const unsigned char *binary;

	printf("\nAOCX file: %s\n\n", AOCX_FILE);
	// create the program using binary already compiled offline using aoc (i.e. the .aocx file)
	FILE *fp = fopen(AOCX_FILE, "rb");

	if (fp == NULL) {
		printf("Failed to open the AOCX file (fopen).\n");
		exit(1);
	}


	fseek(fp, 0, SEEK_END);
	binary_length = ftell(fp);
	binary = (unsigned char*) malloc(sizeof(unsigned char) * binary_length);
	assert(binary && "Malloc failed");
	rewind(fp);

	if (fread((void*)binary, binary_length, 1, fp) == 0) {
		printf("Failed to read from the AOCX file (fread).\n");
		exit(1);
	}
	fclose(fp);


	// Create a program using clCreateProgramWithBinary()
	program = clCreateProgramWithBinary(
			context,
			1,
			devices,
			&binary_length,
			(const unsigned char **)&binary,
			&status,
			NULL); 
	CHECK(status);


	//----------------------------------------------
	// Create the kernel
	//----------------------------------------------

	status = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	if(status != CL_SUCCESS) {
		char log[128*1024] = {0};
		clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, 128*1024, log, NULL);
		printf("%s\n", log);
		CHECK(status);
	}

}

double compute_kernel_execution_time(cl_event &event, double &start_d, double &end_d)
{
  cl_ulong start, end;
    
  clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, 		sizeof(cl_ulong), &end, 	NULL);
  clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, 	sizeof(cl_ulong), &start, 	NULL);
    
  start_d = (double)1.0e-9 * start;
  end_d   = (double)1.0e-9 * end;

  return 	(double)1.0e-9 * (end - start); // nanoseconds to seconds
}


void kernel_top(
       data_precision *weights_block_wise,
       data_precision *input_fm_block_wise,
       data_precision *output_fm_block_wise
       )
{

//	printf("%d %s\n",__LINE__, __func__);
//	fflush(stdout);

	int num_elem_input_fm_bw_0 = cfg_BN_R[0] * cfg_BN_C[0] * cfg_BN_I[0] * (R * BS_R + K - 1) * (BS_C + K - 1) * BS_I * I;
	int num_elem_weights_bw = 0;
	int	num_elem_output_fm_bw = cfg_BN_R[NUM_CONV - 1] * cfg_BN_C[NUM_CONV - 1] * cfg_BN_O[NUM_CONV - 1] * R * BS_R * BS_C * O * BS_O;

	int cfg_BN_R_max = (cfg_img_row[0] + R * BS_R - 1) / (R * BS_R);
	int cfg_BN_C_max = (cfg_img_col[0] + BS_C - 1) / BS_C;
	int cfg_BN_O_max = (cfg_out_num[NUM_CONV - 1] + O * BS_O - 1) / (O * BS_O);

	int num_elem_input_fm_bw_max = cfg_BN_R_max * cfg_BN_C_max * cfg_BN_O_max * (R * BS_R + K - 1) * (BS_C + K - 1) * O * BS_O;

	int num_conv = NUM_CONV;

	for (int i = 0; i < num_conv; i++)
	{
		int num_elem_weight_bw = cfg_BN_O[i] * cfg_BN_I[i] * O * cfg_kernel[i] * cfg_kernel[i] * BS_I * I;

		num_elem_weights_bw += num_elem_weight_bw;
	}

	data_precision *out5, *out5_copy;
//	data_precision *out5 = (data_precision *)malloc(sizeof(data_precision) * num_elem_input_fm_bw_max);
//	data_precision *out5_copy = (data_precision *)malloc(sizeof(data_precision) * num_elem_input_fm_bw_max);
	if ((out5 = (data_precision*)acl_aligned_malloc(num_elem_input_fm_bw_max * sizeof(data_precision))) == NULL) 
	{
		perror("Failed malloc of out5");
	}
	if ((out5_copy = (data_precision*)acl_aligned_malloc(num_elem_input_fm_bw_max * sizeof(data_precision))) == NULL) 
	{
		perror("Failed malloc of out5_copy");
	}

	memcpy(out5, input_fm_block_wise, sizeof(data_precision) * num_elem_input_fm_bw_0);

	cl_event kernel_exec_event[NUM_QUEUES];
	cl_kernel kernel[NUM_KERNELS_TO_CREATE];

	cl_mem d_weights_block_wise;
//	cl_mem d_input_fm_block_wise;
	cl_mem d_output_fm_block_wise;
	cl_mem d_out5;
	cl_mem d_out5_copy;
	
//        printf("\n===== Host-CPU transferring weights and input feature map to the FPGA device global memory (DDR4) via PCIe ======\n\n");

	d_out5 = clCreateBuffer(
		context,
		//CL_MEM_READ_ONLY | CL_MEM_BANK_1_ALTERA,
		CL_MEM_READ_ONLY,
		num_elem_input_fm_bw_max * sizeof(cl_data_precision),
		NULL,
		&status); CHECK(status);

	d_out5_copy = clCreateBuffer(
		context,
		//CL_MEM_READ_ONLY | CL_MEM_BANK_1_ALTERA,
		CL_MEM_READ_ONLY,
		num_elem_input_fm_bw_max * sizeof(cl_data_precision),
		NULL,
		&status); CHECK(status);

	d_weights_block_wise = clCreateBuffer(
		context,
		//CL_MEM_READ_ONLY | CL_MEM_BANK_2_ALTERA,
		CL_MEM_READ_ONLY,
		num_elem_weights_bw * sizeof(cl_data_precision),
		NULL,
		&status); CHECK(status);

	d_output_fm_block_wise = clCreateBuffer(
		context,
		//CL_MEM_WRITE_ONLY | CL_MEM_BANK_1_ALTERA,
		CL_MEM_WRITE_ONLY,
		num_elem_output_fm_bw * sizeof(cl_data_precision),
		NULL,
		&status); CHECK(status);

        // blocking writes
//	status = clEnqueueWriteBuffer(
//		cmdQueue[KID_FEED_IN],
//		d_input_fm_block_wise,
//		CL_TRUE,
//		0,
//		num_elem_input_fm_bw * sizeof(cl_data_precision),
//		input_fm_block_wise,
//		0,
//		NULL,
//		NULL); CHECK(status);

	status = clEnqueueWriteBuffer(
		cmdQueue[KID_FEED_IN],
		d_out5,
		CL_TRUE,
		0,
		num_elem_input_fm_bw_max * sizeof(cl_data_precision),
		out5,
		0,
		NULL,
		NULL); CHECK(status);

	status = clEnqueueWriteBuffer(
		cmdQueue[KID_FEED_IN],
		d_out5_copy,
		CL_TRUE,
		0,
		num_elem_input_fm_bw_max * sizeof(cl_data_precision),
		out5,
		0,
		NULL,
		NULL); CHECK(status);

	status = clEnqueueWriteBuffer(
		cmdQueue[KID_FEED_W],
		d_weights_block_wise,
		CL_TRUE,
		0,
		num_elem_weights_bw * sizeof(cl_data_precision),
		weights_block_wise,
		0,
		NULL,
		NULL); CHECK(status);

//	status = clEnqueueWriteBuffer(
//		cmdQueue[KID_POSTPROC],
//		d_output_fm_block_wise,
//		CL_TRUE,
//		0,
//		num_elem_output_fm_bw * sizeof(cl_data_precision),
//		output_fm_block_wise,
//		0,
//		NULL,
//		NULL); CHECK(status);

//	status = clEnqueueWriteBuffer(
//		cmdQueue[KID_POSTPROC],
//		d_out5,
//		CL_TRUE,
//		0,
//		num_elem_input_fm_bw_max * sizeof(cl_data_precision),
//		out5,
//		0,
//		NULL,
//		NULL); CHECK(status);
//
//	status = clEnqueueWriteBuffer(
//		cmdQueue[KID_POSTPROC],
//		d_out5_copy,
//		CL_TRUE,
//		0,
//		num_elem_input_fm_bw_max * sizeof(cl_data_precision),
//		out5_copy,
//		0,
//		NULL,
//		NULL); CHECK(status);

	printf("CL_INVALID_PROGRAM=%d\n", CL_INVALID_PROGRAM);
	printf("CL_INVALID_PROGRAM_EXECUTABLE=%d\n", CL_INVALID_PROGRAM_EXECUTABLE);
	printf("CL_INVALID_KERNEL_NAME=%d\n", CL_INVALID_KERNEL_NAME);
	printf("CL_INVALID_KERNEL_DEFINITION=%d\n", CL_INVALID_KERNEL_DEFINITION);
	printf("CL_INVALID_VALUE=%d\n", CL_INVALID_VALUE);
	printf("CL_OUT_OF_HOST_MEMORY=%d\n", CL_OUT_OF_HOST_MEMORY);
   
    for(int j=0; j<NUM_KERNELS_TO_CREATE; j++) {
        printf("Creating kernel[%d]: %s\n", j,kernel_name[j]);
		fflush(stdout);
        kernel[j] = clCreateKernel(program, (const char*)kernel_name[j], &status); 
        CHECK(status);
    }


	fflush(stdout);

        status = clSetKernelArg(
		kernel[KID_FEED_IN],
		0,
		sizeof(cl_mem),
		(void*)&d_out5);
		CHECK(status);

        status = clSetKernelArg(
		kernel[KID_FEED_IN],
		1,
		sizeof(cl_mem),
		(void*)&d_out5_copy);
		CHECK(status);

        status = clSetKernelArg(
		kernel[KID_FEED_IN],
		2,
	//	sizeof(unsigned char),
		sizeof(int),
		(void*)&num_conv);
		CHECK(status);

        status = clSetKernelArg(
		kernel[KID_FEED_W],
		0,
		sizeof(cl_mem),
		(void*)&d_weights_block_wise);
		CHECK(status);

        status = clSetKernelArg(
		kernel[KID_FEED_W],
		1,
	//	sizeof(unsigned char),
		sizeof(int),
		(void*)&num_conv);
		CHECK(status);


        status = clSetKernelArg(
		kernel[KID_DRAIN_OUT],
		0,
	//	sizeof(unsigned char),
		sizeof(int),
		(void*)&num_conv);
		CHECK(status);

//		status = clSetKernelArg(
//		kernel[KID_POSTPROC],
//		0,
//		sizeof(cl_mem),
//		(void*)&d_out5); CHECK(status);
//
//		status = clSetKernelArg(
//		kernel[KID_POSTPROC],
//		1,
//		sizeof(cl_mem),
//		(void*)&d_out5_copy); CHECK(status);
//
//        status = clSetKernelArg(
//		kernel[KID_POSTPROC],
//		2,
//		sizeof(int),
//		(void*)&num_conv);
//		CHECK(status);

        status = clSetKernelArg(
		kernel[KID_RELU],
		0,
	//	sizeof(unsigned char),
		sizeof(int),
		(void*)&num_conv);
		CHECK(status);

        status = clSetKernelArg(
		kernel[KID_POOLING],
		0,
	//	sizeof(unsigned char),
		sizeof(int),
		(void*)&num_conv);
		CHECK(status);

        status = clSetKernelArg(
		kernel[KID_ZERO_PADDING],
		0,
		sizeof(cl_mem),
		(void*)&d_out5);
		CHECK(status);

        status = clSetKernelArg(
		kernel[KID_ZERO_PADDING],
		1,
		sizeof(cl_mem),
		(void*)&d_out5_copy);
		CHECK(status);

        status = clSetKernelArg(
		kernel[KID_ZERO_PADDING],
		2,
	//	sizeof(unsigned char),
		sizeof(int),
		(void*)&num_conv);
		CHECK(status);

        status = clSetKernelArg(
		kernel[KID_WRITE_DDR],
		0,
		sizeof(cl_mem),
		(void*)&d_output_fm_block_wise);
		CHECK(status);

        status = clSetKernelArg(
		kernel[KID_WRITE_DDR],
		1,
	//	sizeof(unsigned char),
		sizeof(int),
		(void*)&num_conv);
		CHECK(status);

#if DEBUG_DDR_BANDWIDTH
		status = clSetKernelArg(
		kernel[KID_WRITE_DDR_DEBUG],
		0,
		sizeof(cl_mem),
		(void*)&d_output_fm_block_wise); CHECK(status);

        status = clSetKernelArg(
		kernel[KID_WRITE_DDR_DEBUG],
		1,
		sizeof(unsigned char),
		(void*)&num_conv);
		CHECK(status);

		status = clSetKernelArg(
		kernel[KID_ZERO_PADDING_DEBUG],
		0,
		sizeof(cl_mem),
		(void*)&d_out5);
		CHECK(status);

        status = clSetKernelArg(
		kernel[KID_ZERO_PADDING_DEBUG],
		1,
		sizeof(cl_mem),
		(void*)&d_out5_copy);
		CHECK(status);

        status = clSetKernelArg(
		kernel[KID_ZERO_PADDING_DEBUG],
		2,
		sizeof(unsigned char),
		(void*)&num_conv);
		CHECK(status);
#endif

	//----------------------------------------------
	// Configure the work-item structure (using only tasks atm)
	//----------------------------------------------

	// Define the number of threads that will be created 
	// as well as the number of work groups 
	size_t globalWorkSize[1];
	size_t localWorkSize[1];


	//----------------------------------------------
	// Enqueue the kernel for execution
	//----------------------------------------------


        // all kernels are always tasks
	globalWorkSize[0] = 1;
	localWorkSize[0]  = 1;

    printf("\n===== Host-CPU enqeuing the OpenCL kernels to the FPGA device ======\n\n");
	fflush(stdout);


    int i;
    for(i=0; i<NUM_KERNELS_TO_CREATE; i++) {
		// Alternatively, can use clEnqueueTaskKernel
		printf("clEnqueueNDRangeKernel[%d]: %s!\n", i,kernel_name[i]);
		status = clEnqueueNDRangeKernel(
				cmdQueue[i],
				kernel[i],
				1,
				NULL,
				globalWorkSize,
				localWorkSize,
				0,
				NULL,
                &kernel_exec_event[i]
                );
		CHECK(status);
	}

        printf("\n *** FPGA execution started ... \n");
        fflush(stdout);

#ifdef KERNEL_TIMEOUT

        int nsec = KERNEL_TIMEOUT;
        printf(" Debugging stuck mode enabled, wait for %d seconds ... \n", nsec);
        fflush(stdout);
#endif

	for(i=0; i < NUM_KERNELS_TO_CREATE ; i++) {
		status = clFlush(cmdQueue[i]); 
                CHECK(status);
	}


#ifdef KERNEL_TIMEOUT
enum kernels { 
	LOAD_IN,
	LOAD_W, 
	DRAIN_OUT_WRITE_TREE, 
	DRAIN_OUT_CHAIN,
	FEED_IN,
	FEED_W,
	POSTPROC,
	PE
};
        sleep(nsec);

//	status = clFinish(cmdQueue[LOAD_IN]); CHECK(status);
//	status = clFinish(cmdQueue[LOAD_W]); CHECK(status);
//	status = clFinish(cmdQueue[DRAIN_OUT_WRITE_TREE]); CHECK(status);
//	status = clFinish(cmdQueue[DRAIN_OUT_CHAIN]); CHECK(status);
//	status = clFinish(cmdQueue[FEED_IN]); CHECK(status);
//	status = clFinish(cmdQueue[FEED_W]); CHECK(status);
//	status = clFinish(cmdQueue[POSTPROC]); CHECK(status);
//	status = clFinish(cmdQueue[PE]); CHECK(status);

#endif

        for(i=0; i < NUM_QUEUES_TO_FINISH; i++) {
#ifndef KERNEL_TIMEOUT
           status = clFinish(cmdQueue[i]); CHECK(status);
#endif
	}

        printf(" *** FPGA execution finished!\n");

	fflush(stdout);


	for (i=0; i<NUM_QUEUES_TO_FINISH; i++)
{
          k_exec_time[i] = compute_kernel_execution_time(kernel_exec_event[i], k_start_time[i], k_end_time[i]);     
        }     
    
//        printf("\n===== Host-CPU transferring result matrix C from the FPGA device global memory (DDR4) via PCIe ======\n\n");
	
      // Read the results back from the device, blocking read
        clEnqueueReadBuffer(
              //cmdQueue[KID_DRAIN_MAT_C],
              cmdQueue[NUM_KERNELS_TO_CREATE], // using a special queue for reading buffer C
              d_output_fm_block_wise,
              CL_TRUE,
              0,
              num_elem_output_fm_bw * sizeof(cl_data_precision),
              output_fm_block_wise,
              0,
              NULL,
              NULL);
CHECK(status);


	for(i=0; i<NUM_KERNELS_TO_CREATE; i++) {
          clReleaseKernel(kernel[i]);
	}

	for(i=0; i<NUM_QUEUES_TO_FINISH; i++) {
         clReleaseEvent(kernel_exec_event[i]);
        }

//	clReleaseMemObject(d_input_fm_block_wise);
	clReleaseMemObject(d_weights_block_wise);
	clReleaseMemObject(d_output_fm_block_wise);
	clReleaseMemObject(d_out5);
	clReleaseMemObject(d_out5_copy);

//	free(out5);
//	free(out5_copy);
	acl_aligned_free(out5);
	acl_aligned_free(out5_copy);
}

void report_measurement()
{

	printf("\n===== Reporting measured throughput ======\n\n");
	double k_earliest_start_time = k_start_time[0];
	double k_latest_end_time     = k_end_time[0];	

	for (int i=1; i<NUM_QUEUES_TO_FINISH; i++) {

		if (k_start_time[i] < k_earliest_start_time) 	
			k_earliest_start_time 	= k_start_time[i];

		if (k_end_time[i]   > k_latest_end_time) 		
			k_latest_end_time 		= k_end_time[i];
	} 

	// IMPORTANT: we care about the finish time of drain_C, once data is drained we are done
	k_latest_end_time 		= k_end_time[KID_WRITE_DDR];


	double k_overall_exec_time = k_latest_end_time - k_earliest_start_time;

	printf("\n");
	printf("  Loader kernels start time\t\t= %.5f s\n", k_earliest_start_time);     
	printf("  Drainer kernels end time\t\t= %.5f s\n", k_latest_end_time);     
	printf("  FPGA CNN exec time\t\t= %.5f s\n", k_overall_exec_time);     


	// multiplied by 1.0e-9 to get G-FLOPs
	printf("\n");

	double num_operations = 0.0; 
	
	for (int i = 0; i < NUM_CONV; i++) {
		num_operations += (double) 2.0 * cfg_in_num[i] * cfg_out_num[i] *
			cfg_img_row[i] * cfg_img_col[i] * 
			cfg_kernel[i] * cfg_kernel[i];
	}

	printf("  # operations = %.0f\n", num_operations);     
	printf("  overall execution time = %fs\n", k_overall_exec_time);     
	printf("  # clock cycles = %.0f\n", 200 * 1.0e6 * k_overall_exec_time);     
	printf("  Throughput: %.5f GFLOPS\n", (double)1.0e-9 * num_operations / k_overall_exec_time);     
	printf("\n");
	printf("DONE\n");
}

void opencl_model(
       data_precision *weights_block_wise,
       data_precision *input_fm_block_wise,
       data_precision *output_fm_block_wise
	   )
{
	opencl_setup();

	kernel_top(
//			status,
//			cmdQueue,
//			program,
//			context,
			weights_block_wise,
			input_fm_block_wise,
			output_fm_block_wise
//			k_start_time,
//			k_end_time,
//			k_exec_time
			) ;

	report_measurement();
}
