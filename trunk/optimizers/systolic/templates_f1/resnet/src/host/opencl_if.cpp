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

#include <unistd.h>

#include <CL/opencl.h>

#pragma message ("* Compiling for Xilinx CL")

#include "systolic_params.h"
#include "host.h"
#include "opencl_if.h"
#include "util.h"
#include "cnn.h"

#if defined(SDX_PLATFORM) && !defined(TARGET_DEVICE)
#define STR_VALUE(arg)      #arg
#define GET_STRING(name) STR_VALUE(name)
#define TARGET_DEVICE GET_STRING(SDX_PLATFORM)
#endif

//TARGET_DEVICE macro needs to be passed from gcc command line
const char *target_device_name = TARGET_DEVICE;

// OpenCL environment
cl_platform_id platforms[16];       // platform id
cl_platform_id platform_id;         // platform id
cl_uint platform_count;
cl_device_id device_id;             // compute device id 
cl_context context;                 // compute context
cl_command_queue commands;          // compute command queue
cl_program program;                 // compute program



// test and measurement
double k_start_time[NUM_QUEUES];
double k_end_time[NUM_QUEUES];
double k_exec_time[NUM_QUEUES];

int load_file_to_memory(const char *filename, char **result)
{ 
	size_t size = 0;
	FILE *f = fopen(filename, "rb");
	if (f == NULL) 
	{ 
		*result = NULL;
		return -1; // -1 means file opening fail 
	} 
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);
	*result = (char *)malloc(size+1);
	if (size != fread(*result, sizeof(char), size, f)) 
	{ 
		free(*result);
		return -2; // -2 means file reading fail 
	} 
	fclose(f);
	(*result)[size] = 0;
	return size;
}

/////////////////////////////////////////////////////////////////////////////////
//opencl_setup
//Create context for Xilinx platform, Accelerator device
//Create single command queue for accelerator device
//Create program object with clCreateProgramWithBinary using given xclbin file name
//Return value
void opencl_setup()
{
	cl_int status;

	char cl_platform_vendor[1001];

	// Get all platforms and then select Xilinx platform
	status = clGetPlatformIDs(16, platforms, &platform_count);
	CHECK(status);
	printf("INFO: Found %d platforms\n", platform_count);

	// Find Xilinx Plaftorm
	int platform_found = 0;
	for (unsigned int iplat=0; iplat<platform_count; iplat++) 
	{
		status = clGetPlatformInfo(platforms[iplat], CL_PLATFORM_VENDOR, 1000, (void *)cl_platform_vendor,NULL);
		CHECK(status);
		if (strcmp(cl_platform_vendor, "Xilinx") == 0)
		{
			printf("INFO: Selected platform %d from %s\n", iplat, cl_platform_vendor);
			platform_id = platforms[iplat];
			platform_found = 1;
		}
	}
	if (!platform_found) 
	{
		printf("ERROR: Platform Xilinx not found. Exit.\n");
		exit(-1);
	}

	// Connect to a compute device
	// find all devices and then select the target device
	cl_device_id devices[16];  // compute device id 
	cl_uint device_count;
	unsigned int device_found = 0;
	char cl_device_name[1001];
	status = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ACCELERATOR,
			16, devices, &device_count);
	CHECK(status);

	//iterate all devices to select the target device. 
	for (int i=0; i<device_count; i++)
	{
		status = clGetDeviceInfo(devices[i], CL_DEVICE_NAME, 1024, cl_device_name, 0);
		CHECK(status);
		//printf("CL_DEVICE_NAME %s\n", cl_device_name);
		if(strcmp(cl_device_name, target_device_name) == 0) 
		{
			device_id = devices[i];
			device_found = 1;
			printf("INFO: Selected %s as the target device\n", cl_device_name);
		}
	}

	if (!device_found)
	{
		printf("ERROR: Target device %s not found. Exit.\n", target_device_name);
		exit(-1);
	}


	status = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ACCELERATOR,
			1, &device_id, NULL);
	CHECK(status);

	// Create a compute context 
	//
	context = clCreateContext(0, 1, &device_id, NULL, NULL, &status);
	CHECK(status);

	// Create a command commands
	//
//	commands = clCreateCommandQueue(context, device_id, 0, &status);
	commands = clCreateCommandQueue(context, device_id, CL_QUEUE_PROFILING_ENABLE, &status);
	CHECK(status);

	// Create Program Objects
	//

	// Load binary from disk
	unsigned char *kernelbinary;
	char *xclbin = xclbin_filename;
	printf("INFO: Loading %s\n", xclbin);
	int n_i = load_file_to_memory(xclbin, (char **) &kernelbinary);
	if (n_i < 0)
	{
		printf("failed to load kernel from xclbin: %s\n", xclbin);
		printf("Test failed\n");
		exit(-1);
	}
	size_t n = n_i;
	// Create the compute program from offline
	program = clCreateProgramWithBinary(context, 1, &device_id, &n,
			(const unsigned char **) &kernelbinary, &status, &status);
	CHECK(status);

	// Build the program executable
	//
	status = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	if (status != CL_SUCCESS)
	{
		size_t len;
		char buffer[2048];

		printf("Error: Failed to build program executable!\n");
		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
		printf("%s\n", buffer);
		printf("Test failed\n");
		exit(-1);
	}

}


double compute_kernel_execution_time(cl_event &event, double &start_d, double &end_d)
{
	cl_ulong start, end;

	clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, 	sizeof(cl_ulong), &start, 	NULL);
	clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, 		sizeof(cl_ulong), &end, 	NULL);

	start_d = (double)1.0e-9 * start;
	end_d   = (double)1.0e-9 * end;

	return 	(double)1.0e-9 * (end - start); // nanoseconds to seconds
}



void kernel_top(
       data_bitwidth *ifmap,
       data_bitwidth *weights,
       data_bitwidth *ofmap,
	   int start_block_idx,
	   int end_block_idx,
	   OpCFG *cfg
       )
{
	cl_mem d_ifmap;
	cl_mem d_weights;
	cl_mem d_ofmap;
	cl_mem d_cfg;

	cl_mem d_DEBUG_msg;

	cl_int status;

	cl_kernel kernel;
	cl_event kernel_exec_event[NUM_QUEUES];

	int num_conv;

	int num_elem_ifmap;
	int num_elem_weights;
	int num_elem_ofmap;

	int DEBUG_msg[DEBUG_MSG_LEN];

	num_elem_ifmap = cfg[0].BN_R * cfg[0].BN_C * cfg[0].BN_I * (R * BS_R + cfg_k1[0] - 1) * (C * BS_C + cfg_k2[0] - 1) * BS_I * I;

	num_conv = get_num_conv();

	num_elem_weights = 0;
	for (int i = 0; i < num_conv; i++)
	{
		int num_elem_weight = cfg[i].BN_O * cfg[i].BN_I * O * BS_O * cfg_k1[i] * cfg_k2[i] * BS_I * I;

		num_elem_weights += num_elem_weight;
	}

	num_elem_ofmap = cfg[num_conv - 1].BN_R * cfg[num_conv - 1].BN_C * cfg[num_conv - 1].BN_O * R * BS_R * C * BS_C * O * BS_O;

	printf("%s: num_elem_ifmap=%d\n", __FUNCTION__, num_elem_ifmap);
	printf("%s: num_elem_ofmap=%d\n", __FUNCTION__, num_elem_ofmap);
	printf("%s: num_elem_weights=%d\n", __FUNCTION__, num_elem_weights);
	fflush(stdout);

	// Create the compute kernel in the program we wish to run
	//
	kernel = clCreateKernel(program, "cnn_top", &status);
	CHECK(status);

	// Create the input and output arrays in device memory for our calculation
	//
	d_ifmap = clCreateBuffer(context,  CL_MEM_READ_ONLY,  sizeof(data_bitwidth) * num_elem_ifmap, NULL, &status);
	CHECK(status);
	d_weights = clCreateBuffer(context,  CL_MEM_READ_ONLY,  sizeof(data_bitwidth) * num_elem_weights, NULL, &status);
	CHECK(status);
	d_ofmap = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(data_bitwidth) * num_elem_ofmap, NULL, &status);
//	d_ofmap = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(data_bitwidth) * 1000000, NULL, &status);
	CHECK(status);
	d_cfg = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(OpCFG) * num_conv, NULL, &status);
	CHECK(status);

	d_DEBUG_msg = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(int) * DEBUG_MSG_LEN, NULL, &status);
	CHECK(status);

	// Write our data set into the input array in device memory 
	//
	status = clEnqueueWriteBuffer(commands, d_ifmap, CL_TRUE, 0, sizeof(data_bitwidth) * num_elem_ifmap, ifmap, 0, NULL, NULL);
	CHECK(status);

	// Write our data set into the input array in device memory 
	//
	status = clEnqueueWriteBuffer(commands, d_weights, CL_TRUE, 0, sizeof(data_bitwidth) * num_elem_weights, weights, 0, NULL, NULL);
	CHECK(status);


	status = clEnqueueWriteBuffer(commands, d_cfg, CL_TRUE, 0, sizeof(OpCFG) * num_conv, cfg, 0, NULL, NULL);
	CHECK(status);

	// Set the arguments to our compute kernel
	//
	status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_ifmap);
	CHECK(status);
	status = clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_weights);
	CHECK(status);
	status = clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_ofmap);
	CHECK(status);
	status = clSetKernelArg(kernel, 3, sizeof(cl_int), &start_block_idx);
	CHECK(status);
	status = clSetKernelArg(kernel, 4, sizeof(cl_int), &end_block_idx);
	CHECK(status);
	status = clSetKernelArg(kernel, 5, sizeof(cl_mem), &d_cfg);
	CHECK(status);
	status = clSetKernelArg(kernel, 6, sizeof(cl_mem), &d_DEBUG_msg);
	CHECK(status);

	// Execute the kernel over the entire range of our 1d input data set
	// using the maximum number of work group items for this device
	//

#ifdef C_KERNEL
	status = clEnqueueTask(commands, kernel, 0, NULL, &kernel_exec_event[0]);
#else
	global[0] = MATRIX_RANK;
	global[1] = MATRIX_RANK;
	local[0] = MATRIX_RANK;
	local[1] = MATRIX_RANK;
	status = clEnqueueNDRangeKernel(commands, kernel, 2, NULL, 
			(size_t*)&global, (size_t*)&local, 0, NULL, &kernel_exec_event[0]);
#endif
	CHECK(status);

	for (int i = 0; i < NUM_QUEUES; i++)
	{
		clFlush(commands);
	}
	for (int i = 0; i < NUM_QUEUES; i++)
	{
		clFinish(commands);
	}
	for (int i = 0; i < NUM_QUEUES; i++)
	{
		k_exec_time[i] = compute_kernel_execution_time(kernel_exec_event[i], k_start_time[i], k_end_time[i]);
	}

	// Read back the results from the device to verify the output
	//
//	cl_event readevent;
	status = clEnqueueReadBuffer(commands, d_ofmap, CL_TRUE, 0, sizeof(data_bitwidth) * num_elem_ofmap, ofmap, 0, NULL, NULL);  
	CHECK(status);

	status = clEnqueueReadBuffer(commands, d_DEBUG_msg, CL_TRUE, 0, sizeof(int) * DEBUG_MSG_LEN, DEBUG_msg, 0, NULL, NULL);  
	CHECK(status);

	for (int i = 0; i < DEBUG_MSG_LEN; i++)
	{
		printf("DEBUG_msg[%d]=%d\n", i, DEBUG_msg[i]);
	}
	fflush(stdout);

//	clWaitForEvents(1, &readevent);

	// Shutdown and cleanup
	//
	clReleaseMemObject(d_ifmap);
	clReleaseMemObject(d_weights);
	clReleaseMemObject(d_ofmap);
	clReleaseMemObject(d_cfg);
	clReleaseMemObject(d_DEBUG_msg);

	for (int i = 0; i < NUM_QUEUES; i++)
	{
		clReleaseEvent(kernel_exec_event[i]);
	}

	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseCommandQueue(commands);
	clReleaseContext(context);
}

void report_measurement()
{

	printf("\n===== Reporting measured throughput ======\n\n");
	double k_earliest_start_time = k_start_time[0];
	double k_latest_end_time     = k_end_time[0];	

	for (int i = 1; i < NUM_QUEUES; i++) 
	{

		if (k_start_time[i] < k_earliest_start_time) 	
			k_earliest_start_time 	= k_start_time[i];

		if (k_end_time[i]   > k_latest_end_time) 		
			k_latest_end_time 		= k_end_time[i];
	} 

	double k_overall_exec_time = k_latest_end_time - k_earliest_start_time;

	printf("\n");
	printf("  k_earliest_start_time\t\t= %.5f s\n", k_earliest_start_time);     
	printf("  k_latest_end_time\t\t= %.5f s\n", k_latest_end_time);     
	printf("  FPGA CNN exec time\t\t= %.5f s\n", k_overall_exec_time);     


	// multiplied by 1.0e-9 to get G-FLOPs
	printf("\n");

	int num_conv = get_num_conv();
	double num_operations = 0.0; 
	
	for (int i = 0; i < num_conv; i++) 
	{
		num_operations += (2.0 * cfg_in_num[i] * cfg_out_num[i] * cfg_img_row[i] * cfg_img_col[i] * cfg_k1[i] * cfg_k2[i]);
	}

	printf("  # operations = %.0f\n", num_operations);     
	printf("  Throughput: %.5f Gops\n", (double)1.0e-9 * num_operations / k_overall_exec_time);     
	printf("\n");
	printf("DONE\n");
}

void opencl_model(
       data_bitwidth *ifmap,
       data_bitwidth *weights,
       data_bitwidth *ofmap, 
	   int start_block_idx,
	   int end_block_idx,
	   OpCFG *cfg
	   )
{
	opencl_setup();

	kernel_top(ifmap,
			weights,
			ofmap, 
			start_block_idx,
			end_block_idx,
			cfg
			);

	report_measurement();
}
