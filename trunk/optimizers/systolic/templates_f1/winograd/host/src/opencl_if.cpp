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
       data_bitwidth *weight,
       data_bitwidth *ifmap,
       data_bitwidth *ofmap
       )
{
	cl_mem d_ifmap;
	cl_mem d_weight;
	cl_mem d_ofmap;

	int ifmap_length = BN_R * BN_I * (R * BS_R + K1 - 1) * (C * BS_C + K2 - 1) * BS_I * I;
	int wt_length = BN_O * BN_I * O * BS_O * K1 * K2 * BS_I * I;
	int ofmap_length = BN_R * BN_O * R * BS_R * C * BS_C * O * BS_O;

	cl_int status;

	cl_kernel kernel;
	cl_event kernel_exec_event[NUM_QUEUES];

	// Create the compute kernel in the program we wish to run
	//
	kernel = clCreateKernel(program, "systolic_top", &status);
	CHECK(status);

	// Create the input and output arrays in device memory for our calculation
	//
	d_ifmap = clCreateBuffer(context,  CL_MEM_READ_ONLY,  sizeof(data_bitwidth) * ifmap_length, NULL, &status);
	CHECK(status);
	d_weight = clCreateBuffer(context,  CL_MEM_READ_ONLY,  sizeof(data_bitwidth) * wt_length, NULL, &status);
	CHECK(status);
	d_ofmap = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(data_bitwidth) * ofmap_length, NULL, &status);
	CHECK(status);

	// Write our data set into the input array in device memory 
	//
	status = clEnqueueWriteBuffer(commands, d_ifmap, CL_TRUE, 0, sizeof(data_bitwidth) * ifmap_length, ifmap, 0, NULL, NULL);
	CHECK(status);

	// Write our data set into the input array in device memory 
	//
	status = clEnqueueWriteBuffer(commands, d_weight, CL_TRUE, 0, sizeof(data_bitwidth) * wt_length, weight, 0, NULL, NULL);
	CHECK(status);

	// Set the arguments to our compute kernel
	//
	status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_ifmap);
	CHECK(status);
	status = clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_weight);
	CHECK(status);
	status = clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_ofmap);
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
//	err = clEnqueueReadBuffer(commands, d_ofmap, CL_TRUE, 0, sizeof(data_bitwidth) * ofmap_length, ofmap, 0, NULL, &readevent);  
	status = clEnqueueReadBuffer(commands, d_ofmap, CL_TRUE, 0, sizeof(data_bitwidth) * ofmap_length, ofmap, 0, NULL, NULL);  
	CHECK(status);

//	clWaitForEvents(1, &readevent);

	// Shutdown and cleanup
	//
	clReleaseMemObject(d_ifmap);
	clReleaseMemObject(d_weight);
	clReleaseMemObject(d_ofmap);

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

	double num_operations = 0.0; 
	
	for (int i = 0; i < 1; i++) 
	{
		num_operations += (2.0 * BN_R * BS_R * R * BS_C * C * BN_O * BS_O * O * K1 * K2 * BN_I * BS_I * I);
	}

	printf("  # operations = %.0f\n", num_operations);     
	printf("  Throughput: %.5f GFLOPS\n", (double)1.0e-9 * num_operations / k_overall_exec_time);     
	printf("\n");
	printf("DONE\n");
}

void opencl_model(
       data_bitwidth *weights_block_wise,
       data_bitwidth *ifmap_block_wise,
       data_bitwidth *ofmap_block_wise
	   )
{
	opencl_setup();

	kernel_top(
			weights_block_wise,
			ifmap_block_wise,
			ofmap_block_wise
			) ;

	report_measurement();
}
