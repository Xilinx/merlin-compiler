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


#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <CL/opencl.h>

#include <iostream>

#include "systolic.h"

////////////////////////////////////////////////////////////////////////////////

void prepare_data(data_bitwidth *data, int length)
{
	for (int i = 0; i < length; i++)
	{
		data[i] = 1.0;
	}
}

void systolic_golden(data_bitwidth *ifmap, data_bitwidth *weight, data_bitwidth *ofmap)
{
	for (int o2 = 0; o2 < O; o2++)
	for (int o1 = 0; o1 < BS_O; o1++)
	for (int r1 = 0; r1 < BS_R; r1++)
	{
		int ofmap_idx = (o2 * BS_O + o1) * BS_R + r1;

		ofmap[ofmap_idx] = 0;
		for (int i1 = 0; i1 < BS_I; i1++)
		{
			int ifmap_idx = r1 * BS_I + i1;
			int wt_idx = (o2 * BS_O + o1) * BS_I + i1;

			ofmap[ofmap_idx] += weight[wt_idx] * ifmap[ifmap_idx];
		}
	}
}

	int
load_file_to_memory(const char *filename, char **result)
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

int main(int argc, char** argv)
{
#if defined(SDX_PLATFORM) && !defined(TARGET_DEVICE)
#define STR_VALUE(arg)      #arg
#define GET_STRING(name) STR_VALUE(name)
#define TARGET_DEVICE GET_STRING(SDX_PLATFORM)
#endif

	//TARGET_DEVICE macro needs to be passed from gcc command line
	const char *target_device_name = TARGET_DEVICE;

	int err;                            // error code returned from api calls

	data_bitwidth ifmap[BS_R * BS_I];
	data_bitwidth weight[O * BS_O * BS_I];
	data_bitwidth ofmap[O * BS_R * BS_O];
	data_bitwidth ofmap2[O * BS_R * BS_O];

	data_bitwidth ofmap_sw[O * BS_R * BS_O];

	int ifmap_length = BS_R * BS_I;
	int wt_length = O * BS_O * BS_I;
	int ofmap_length = O * BS_R * BS_O;

	unsigned int correct;               // number of correct results returned

	size_t global[2];                   // global domain size for our calculation
	size_t local[2];                    // local domain size for our calculation

	cl_platform_id platforms[16];       // platform id
	cl_platform_id platform_id;         // platform id
	cl_uint platform_count;
	cl_device_id device_id;             // compute device id 
	cl_context context;                 // compute context
	cl_command_queue commands;          // compute command queue
	cl_command_queue commands2;          // compute command queue
	cl_program program;                 // compute program
	cl_kernel kernel;                   // compute kernel

	cl_kernel kernel2;                   // compute kernel2

	char cl_platform_vendor[1001];

	cl_mem d_ifmap;                     // device memory used for the input array
	cl_mem d_weight;                     // device memory used for the input array
	cl_mem d_ofmap;                      // device memory used for the output array

	cl_mem d_ifmap2;
	cl_mem d_weight2;
	cl_mem d_ofmap2;

	if (argc != 2)
	{
		printf("%s <inputfile>\n", argv[0]);
		return EXIT_FAILURE;
	}

	// Fill our data sets with pattern
	//
	prepare_data(ifmap, ifmap_length);
	prepare_data(weight, wt_length);

	// 
	// Get all platforms and then select Xilinx platform
	err = clGetPlatformIDs(16, platforms, &platform_count);
	if (err != CL_SUCCESS)
	{
		printf("Error: Failed to find an OpenCL platform!\n");
		printf("Test failed\n");
		return EXIT_FAILURE;
	}
	printf("INFO: Found %d platforms\n", platform_count);

	// Find Xilinx Plaftorm
	int platform_found = 0;
	for (unsigned int iplat=0; iplat<platform_count; iplat++) 
	{
		err = clGetPlatformInfo(platforms[iplat], CL_PLATFORM_VENDOR, 1000, (void *)cl_platform_vendor,NULL);
		if (err != CL_SUCCESS)
		{
			printf("Error: clGetPlatformInfo(CL_PLATFORM_VENDOR) failed!\n");
			printf("Test failed\n");
			return EXIT_FAILURE;
		}
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
		return EXIT_FAILURE;
	}

	// Connect to a compute device
	// find all devices and then select the target device
	cl_device_id devices[16];  // compute device id 
	cl_uint device_count;
	unsigned int device_found = 0;
	char cl_device_name[1001];
	err = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ACCELERATOR,
			16, devices, &device_count);
	if (err != CL_SUCCESS)
	{
		printf("Error: Failed to create a device group!\n");
		printf("Test failed\n");
		return EXIT_FAILURE;
	}

	//iterate all devices to select the target device. 
	for (int i=0; i<device_count; i++)
	{
		err = clGetDeviceInfo(devices[i], CL_DEVICE_NAME, 1024, cl_device_name, 0);
		if (err != CL_SUCCESS)
		{
			printf("Error: Failed to get device name for device %d!\n", i);
			printf("Test failed\n");
			return EXIT_FAILURE;
		}
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
		return EXIT_FAILURE;
	}


	err = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ACCELERATOR,
			1, &device_id, NULL);
	if (err != CL_SUCCESS)
	{
		printf("Error: Failed to create a device group!\n");
		printf("Test failed\n");
		return EXIT_FAILURE;
	}

	// Create a compute context 
	//
	context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
	if (!context)
	{
		printf("Error: Failed to create a compute context!\n");
		printf("Test failed\n");
		return EXIT_FAILURE;
	}

	// Create a command commands
	//
	commands = clCreateCommandQueue(context, device_id, 0, &err);
	if (!commands)
	{
		printf("Error: Failed to create a command commands!\n");
		printf("Error: code %i\n",err);
		printf("Test failed\n");
		return EXIT_FAILURE;
	}

	commands2 = clCreateCommandQueue(context, device_id, 0, &err);
	if (!commands2)
	{
		printf("Error: Failed to create a command commands!\n");
		printf("Error: code %i\n",err);
		printf("Test failed\n");
		return EXIT_FAILURE;
	}

	int status;

	// Create Program Objects
	//

	// Load binary from disk
	unsigned char *kernelbinary;
	char *xclbin=argv[1];
	printf("INFO: Loading %s\n", xclbin);
	int n_i = load_file_to_memory(xclbin, (char **) &kernelbinary);
	if (n_i < 0)
	{
		printf("failed to load kernel from xclbin: %s\n", xclbin);
		printf("Test failed\n");
		return EXIT_FAILURE;
	}
	size_t n = n_i;
	// Create the compute program from offline
	program = clCreateProgramWithBinary(context, 1, &device_id, &n,
			(const unsigned char **) &kernelbinary, &status, &err);
	if ((!program) || (err!=CL_SUCCESS))
	{
		printf("Error: Failed to create compute program from binary %d!\n", err);
		printf("Test failed\n");
		return EXIT_FAILURE;
	}

	// Build the program executable
	//
	err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	if (err != CL_SUCCESS)
	{
		size_t len;
		char buffer[2048];

		printf("Error: Failed to build program executable!\n");
		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
		printf("%s\n", buffer);
		printf("Test failed\n");
		return EXIT_FAILURE;
	}


	// Create the compute kernel in the program we wish to run
	//
	kernel = clCreateKernel(program, "systolic_top", &err);
	if (!kernel || err != CL_SUCCESS)
	{
		printf("Error: Failed to create compute kernel!\n");
		printf("Test failed\n");
		return EXIT_FAILURE;
	}

	kernel2 = clCreateKernel(program, "systolic_top2", &err);
	if (!kernel || err != CL_SUCCESS)
	{
		printf("Error: Failed to create compute kernel!\n");
		printf("Test failed\n");
		return EXIT_FAILURE;
	}

	// Create the input and output arrays in device memory for our calculation
	//
	d_ifmap = clCreateBuffer(context,  CL_MEM_READ_ONLY,  sizeof(data_bitwidth) * ifmap_length, NULL, NULL);
	d_weight = clCreateBuffer(context,  CL_MEM_READ_ONLY,  sizeof(data_bitwidth) * wt_length, NULL, NULL);
	d_ofmap = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(data_bitwidth) *ofmap_length,  NULL, NULL);
	if (!d_ifmap || !d_weight || !d_ofmap)
	{
		printf("Error: Failed to allocate device memory!\n");
		printf("Test failed\n");
		return EXIT_FAILURE;
	}    

	d_ifmap2 = clCreateBuffer(context,  CL_MEM_READ_ONLY,  sizeof(data_bitwidth) * ifmap_length, NULL, NULL);
	d_weight2 = clCreateBuffer(context,  CL_MEM_READ_ONLY,  sizeof(data_bitwidth) * wt_length, NULL, NULL);
	d_ofmap2 = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(data_bitwidth) *ofmap_length,  NULL, NULL);
	if (!d_ifmap2 || !d_weight2 || !d_ofmap2)
	{
		printf("Error: Failed to allocate device memory!\n");
		printf("Test failed\n");
		return EXIT_FAILURE;
	}    

	// Write our data set into the input array in device memory 
	//
	err = clEnqueueWriteBuffer(commands, d_ifmap, CL_TRUE, 0, sizeof(data_bitwidth) * ifmap_length, ifmap, 0, NULL, NULL);
	if (err != CL_SUCCESS)
	{
		printf("Error: Failed to write to source array a!\n");
		printf("Test failed\n");
		return EXIT_FAILURE;
	}
	err = clEnqueueWriteBuffer(commands2, d_ifmap2, CL_TRUE, 0, sizeof(data_bitwidth) * ifmap_length, ifmap, 0, NULL, NULL);
	if (err != CL_SUCCESS)
	{
		printf("Error: Failed to write to source array a!\n");
		printf("Test failed\n");
		return EXIT_FAILURE;
	}

	// Write our data set into the input array in device memory 
	//
	err = clEnqueueWriteBuffer(commands, d_weight, CL_TRUE, 0, sizeof(data_bitwidth) * wt_length, weight, 0, NULL, NULL);
	if (err != CL_SUCCESS)
	{
		printf("Error: Failed to write to source array b!\n");
		printf("Test failed\n");
		return EXIT_FAILURE;
	}
	err = clEnqueueWriteBuffer(commands2, d_weight2, CL_TRUE, 0, sizeof(data_bitwidth) * wt_length, weight, 0, NULL, NULL);
	if (err != CL_SUCCESS)
	{
		printf("Error: Failed to write to source array b!\n");
		printf("Test failed\n");
		return EXIT_FAILURE;
	}

	// Set the arguments to our compute kernel
	//
	err = 0;
	err  = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_ifmap);
	err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_weight);
	err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_ofmap);
	if (err != CL_SUCCESS)
	{
		printf("Error: Failed to set kernel arguments! %d\n", err);
		printf("Test failed\n");
		return EXIT_FAILURE;
	}

	err = 0;
	err  = clSetKernelArg(kernel2, 0, sizeof(cl_mem), &d_ifmap2);
	err |= clSetKernelArg(kernel2, 1, sizeof(cl_mem), &d_weight2);
	err |= clSetKernelArg(kernel2, 2, sizeof(cl_mem), &d_ofmap2);
	if (err != CL_SUCCESS)
	{
		printf("Error: Failed to set kernel arguments! %d\n", err);
		printf("Test failed\n");
		return EXIT_FAILURE;
	}


	// Execute the kernel over the entire range of our 1d input data set
	// using the maximum number of work group items for this device
	//

#ifdef C_KERNEL
	err = clEnqueueTask(commands, kernel, 0, NULL, NULL);
	err = clEnqueueTask(commands2, kernel2, 0, NULL, NULL);
#else
	global[0] = 1;
	global[1] = 1;
	local[0] = 1;
	local[1] = 1;
	err = clEnqueueNDRangeKernel(commands, kernel, 2, NULL, 
			(size_t*)&global, (size_t*)&local, 0, NULL, NULL);
	err = clEnqueueNDRangeKernel(commands2, kernel2, 2, NULL, 
			(size_t*)&global, (size_t*)&local, 0, NULL, NULL);
#endif
	if (err)
	{
		printf("Error: Failed to execute kernel! %d\n", err);
		printf("Test failed\n");
		return EXIT_FAILURE;
	}
	printf("#line %d: %d\n", __LINE__, err);
	fflush(stdout);

	// Read back the results from the device to verify the output
	//
	cl_event readevent;
	err = clEnqueueReadBuffer(commands, d_ofmap, CL_TRUE, 0, sizeof(data_bitwidth) * ofmap_length, ofmap, 0, NULL, &readevent);  
	if (err != CL_SUCCESS)
	{
		printf("Error: Failed to read output array! %d\n", err);
		printf("Test failed\n");
		return EXIT_FAILURE;
	}

	clWaitForEvents(1, &readevent);

	cl_event readevent2;
	err = clEnqueueReadBuffer(commands2, d_ofmap2, CL_TRUE, 0, sizeof(data_bitwidth) * ofmap_length, ofmap2, 0, NULL, &readevent2);  
	if (err != CL_SUCCESS)
	{
		printf("Error: Failed to read output array! %d\n", err);
		printf("Test failed\n");
		return EXIT_FAILURE;
	}

	clWaitForEvents(1, &readevent2);

	// Validate our results
	//
	systolic_golden(ifmap, weight, ofmap_sw);

    int match = 0;
    for (int i = 0 ; i < ofmap_length; i++)
	{
	//	if (ofmap[i] != ofmap_sw[i])
	//	{
	//		std::cout << "Error: Result mismatch" << std::endl;
	//		std::cout << "i = " << i << " CPU result = " << ofmap_sw[i]
	//			<< " Device result = " << ofmap[i] << std::endl;
	//		match = 1;
	//		break;
	//	}
		printf("ofmap_sw[%d]=%.3f. ofmap[%d]=%.3f, ofmap2[%d]=%.3f\n", i, ofmap_sw[i], i, ofmap[i], i, ofmap2[i]);
    }

   // std::cout << "TEST " << (match ? "FAILED" : "PASSED") << std::endl; 

	// Shutdown and cleanup
	//
	clReleaseMemObject(d_ifmap);
	clReleaseMemObject(d_weight);
	clReleaseMemObject(d_ofmap);

	clReleaseMemObject(d_ifmap2);
	clReleaseMemObject(d_weight2);
	clReleaseMemObject(d_ofmap2);

	clReleaseKernel(kernel);
	clReleaseKernel(kernel2);

	clReleaseProgram(program);

	clReleaseCommandQueue(commands);
	clReleaseCommandQueue(commands2);

	clReleaseContext(context);

}
