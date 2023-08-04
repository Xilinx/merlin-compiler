//(C) Copyright 2016-2021 Xilinx, Inc.
//All Rights Reserved.
//
//Licensed to the Apache Software Foundation (ASF) under one
//or more contributor license agreements.  See the NOTICE file
//distributed with this work for additional information
//regarding copyright ownership.  The ASF licenses this file
//to you under the Apache License, Version 2.0 (the
//"License"); you may not use this file except in compliance
//with the License.  You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
//Unless required by applicable law or agreed to in writing,
//software distributed under the License is distributed on an
//"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
//KIND, either express or implied.  See the License for the
//specific language governing permissions and limitations
//under the License. (edited)
/************************************************************************************
 *  (c) Copyright 2014-2015 Xilinx, Inc. All rights reserved.
 *
 ************************************************************************************/


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

#include "__merlin_opencl_if.h"
#include "__merlin_opencl_kernel_buffer.h"

static int count_init=0;
cl_platform_id platform_id;         // platform id
cl_device_id device_id;             // compute device id 
cl_context context;                 // compute context
cl_command_queue commands[KERNEL_NUM];          // compute command queue
cl_program program;                 // compute program

//cl_kernel kernel;                   // compute kernel

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
  if (size != (int)fread(*result, sizeof(char), size, f)) 
  { 
    free(*result);
    return -2; // -2 means file reading fail 
  } 
  fclose(f);
  (*result)[size] = 0;
  return size;
}

int opencl_init_test_kernel(const char * bitstream)
{
  if(count_init == 0) {
    char cl_platform_vendor[1001];
    char cl_platform_name[1001];

    cl_platform_vendor[0] = 0;
    cl_platform_name[0] = 0;


    int err;
     
    // Connect to first platform
    //
    err = clGetPlatformIDs(1,&platform_id,NULL);
    if (err != CL_SUCCESS)
    {
      printf("Error: Failed to find an OpenCL platform!\n");
      printf("Test failed\n");
      exit(EXIT_FAILURE);
    }
    err = clGetPlatformInfo(platform_id,CL_PLATFORM_VENDOR,1000,(void *)cl_platform_vendor,NULL);
    if (err != CL_SUCCESS)
    {
      printf("Error: clGetPlatformInfo(CL_PLATFORM_VENDOR) failed!\n");
      printf("Test failed\n");
      exit(EXIT_FAILURE);
    }
    printf("CL_PLATFORM_VENDOR %s\n",cl_platform_vendor);
    err = clGetPlatformInfo(platform_id,CL_PLATFORM_NAME,1000,(void *)cl_platform_name,NULL);
    if (err != CL_SUCCESS)
    {
      printf("Error: clGetPlatformInfo(CL_PLATFORM_NAME) failed!\n");
      printf("Test failed\n");
      exit(EXIT_FAILURE);
    }
    printf("CL_PLATFORM_NAME %s\n",cl_platform_name);

    // Connect to a compute device
    //
    int fpga = 0;
    #if defined (FPGA_DEVICE)
    fpga = 1;
    #endif
    err = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ALL,
                         1, &device_id, NULL);
    if (err != CL_SUCCESS)
    {
      printf("Error: Failed to create a device group!\n");
      printf("Test failed\n");
      exit(EXIT_FAILURE);
    }
    
    // Create a compute context 
    //
    context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
    if (!context)
    {
      printf("Error: Failed to create a compute context!\n");
      printf("Test failed\n");
      exit(EXIT_FAILURE);
    }

    // Create a command commands
    //
    int i;
    for(i = 0; i < KERNEL_NUM; i++){
      commands[i] = clCreateCommandQueue(context, device_id, 0, &err);
      if (!commands[i])
      {
        printf("Error: Failed to create a command commands[%d]!\n",i);
        printf("Error: code %i\n",err);
        printf("Test failed\n");
        exit(EXIT_FAILURE);
      }
    }

    int status;

    // Create Program Objects
    //
    
    // Load binary from disk
    int n_i = 0;
    unsigned char *kernelbinary;

    #ifndef BACKEND_APOLLO_FLOW
    int flag = 0;
    if(strstr(bitstream, ".aocx")) flag = 1;


    const char * xclbin="kernel_top.aocx";
//    if(bitstream == NULL) {
    if(flag == 0) {
        xclbin="kernel_top.aocx";
    } else { 
        xclbin=bitstream; 
    }

    printf("loading %s\n", xclbin);
    n_i = load_file_to_memory(xclbin, (char **) &kernelbinary);
    if (n_i < 0) {
      printf("failed to load kernel from binary: %s\n", xclbin);
      printf("Test failed\n");
      exit(EXIT_FAILURE);
    }
    #endif
    size_t n = n_i;
    // Create the compute program from offline
    program = clCreateProgramWithBinary(context, 1, &device_id, &n,
                                        (const unsigned char **) &kernelbinary, &status, &err);
    if ((!program) || (err!=CL_SUCCESS)) {
      printf("Error: Failed to create compute program from binary %d!\n", err);
      printf("Test failed\n");
      exit(EXIT_FAILURE);
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
      exit(EXIT_FAILURE);
    }

    opencl_init_kernel_buffer();

    count_init++;
    return err;
  } else {
      return 0;
  }
}

int opencl_create_kernel(cl_kernel * kernel, char * kernel_name)
{
    int err;
//////////////////////////////////////////////////////////
// Create Kernels & Buffers
//////////////////////////////////////////////////////////
  // Create the compute kernel in the program we wish to run
  //
  //
  *kernel = clCreateKernel(program, kernel_name, &err);
  //if (!(*kernel) || err != CL_SUCCESS)
  //{
  //  printf("Error: Failed to create compute kernel!\n");
  //  printf("Test failed\n");
  //  exit(EXIT_FAILURE);
  //}

  return CL_SUCCESS;
}

int opencl_create_buffer(cl_mem * cl_buffer, size_t size, int type)
{
    //if (size >= (1UL<<32)) 
    //{
    //    printf("[opencl_create_buffer] WARNING: Invalid Size: size=%ld\n", size);
    //    exit(EXIT_FAILURE);
    //}

    size_t flag = 0;

    if (type == 0) flag = CL_MEM_READ_ONLY;
    if (type == 1) flag = CL_MEM_WRITE_ONLY;
    if (type == 2) flag = CL_MEM_READ_WRITE;

  *cl_buffer = clCreateBuffer(context,  flag, size, NULL, NULL);
  if (!(*cl_buffer))
  {
    printf("Error: Failed to allocate device memory!\n");
    printf("Test failed\n");
    exit(EXIT_FAILURE);
  }    

  return CL_SUCCESS;
}

// according to OpenCL standard, offset and size are in byte
int opencl_write_buffer(cl_mem cl_buffer, cl_command_queue commands, size_t offset, void * host_buffer, size_t size) {
    //if (offset < 0 || offset + size - 1 >= (1UL<<32)) 
    //{
    //    printf("[opencl_write_buffer] WARNING: offset or size overflow: offset=%ld size=%ld\n", offset, size);
    //    exit(EXIT_FAILURE);
    //}

  cl_event event;
//  int err = clEnqueueWriteBuffer(commands, cl_buffer, CL_TRUE, offset, size, host_buffer, 0, NULL, &event);
  int err = clEnqueueWriteBuffer(commands, cl_buffer, CL_FALSE, offset, size, host_buffer, 0, NULL, &event);
  clWaitForEvents(1, &event);
  if (err != CL_SUCCESS)
  {
    printf("Error: Failed to write to device buffer!\n");
    printf("Test failed\n");
    exit(EXIT_FAILURE);
  }

  return err;
}

// according to OpenCL standard, offset and size are in byte
int opencl_read_buffer(cl_mem cl_buffer, cl_command_queue commands, size_t offset, void * host_buffer, size_t size)
{
    //if (offset < 0 || offset + size - 1 >= (1UL<<32)) 
    //{
    //    printf("[opencl_read_buffer] WARNING: offset or size overflow: offset=%ld size=%ld\n", offset, size);
    //    exit(EXIT_FAILURE);
    //}

  cl_event readevent;
//  int err = clEnqueueReadBuffer(commands, cl_buffer, CL_TRUE, offset, size, host_buffer, 0, NULL, &readevent);
  int err = clEnqueueReadBuffer(commands, cl_buffer, CL_FALSE, offset, size, host_buffer, 0, NULL, &readevent);
  clWaitForEvents(1, &readevent);
  if (err != CL_SUCCESS)
  {
    printf("Error: Failed to read from device buffer!\n");
    printf("Test failed\n");
    exit(EXIT_FAILURE);
  }

  return err;
}

int opencl_set_kernel_arg(cl_kernel kernel, int index, size_t size, void * content)
{
    return clSetKernelArg(kernel, index, size, content);
}

//int opencl_enqueue_kernel(cl_kernel kernel, int dim, size_t global_in[])
int opencl_enqueue_kernel(cl_kernel kernel, cl_command_queue commands, int dim, size_t global_in[], cl_event *event_out)
{
    int i;
    size_t global[100], local[100];
    assert(dim < 100);

    for (i = 0; i < dim; i++)
    {
        global[i] = global_in[i];
        local[i] = 1;
    }

//    cl_event event;
//    int err = clEnqueueNDRangeKernel(commands, kernel, dim, NULL, (size_t*)&global, (size_t*)&local, 0, NULL, &event);
//    clWaitForEvents(1, &event);
    int err = clEnqueueNDRangeKernel(commands, kernel, dim, NULL, (size_t*)&global, (size_t*)&local, 0, NULL, event_out);

    return err;
}

int opencl_enqueue_task(cl_kernel kernel)
{

//    cl_event event;
//    int err = clEnqueueTask(commands, kernel, 0, NULL, &event);
//    clWaitForEvents(1, &event);
//
//    return err;
}

int opencl_flush()
{
//    return clFlush(commands);
}




