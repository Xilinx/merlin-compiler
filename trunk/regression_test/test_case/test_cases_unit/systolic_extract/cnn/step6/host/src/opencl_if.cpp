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

#pragma message ("* Compiling for ALTERA CL")
#include "CL/opencl.h"

#include "opencl_if.h"

#include "systolic_params.h"

#ifndef EMULATOR
  #define AOCX_FILE "systolic_array_4x4_dot8.aocx"
#else
  #define AOCX_FILE "kernel_top.sim.aocx"
#endif

// set the kernel names (kernel functions)
#include "host_kernel_strings.h"

double compute_kernel_execution_time(cl_event &event, double &start_d, double &end_d)
{
  cl_ulong start, end;
    
  clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, 		sizeof(cl_ulong), &end, 	NULL);
  clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, 	sizeof(cl_ulong), &start, 	NULL);
    
  start_d = (double)1.0e-9 * start;
  end_d   = (double)1.0e-9 * end;

  return 	(double)1.0e-9 * (end - start); // nanoseconds to seconds
}



void matmul_kernel_top(cl_int & status,
       cl_command_queue cmdQueue[NUM_QUEUES_TO_CREATE+1],
   	   cl_program &program,
       cl_context &context,
       unsigned int num_elem_A,
       unsigned int num_elem_B,
       unsigned int num_elem_C,
//       int BNUM_I, int BNUM_J, int BNUM_K,
       float * matrix_mul_inputA_block_wise,
       float * matrix_mul_inputB_block_wise,
       float * matrix_mul_outputC,
	double k_start_time[NUM_QUEUES_TO_FINISH],
	double k_end_time[NUM_QUEUES_TO_FINISH],
	double k_exec_time[NUM_QUEUES_TO_FINISH]
       ) 
{


//	printf("%d %s\n",__LINE__, __func__);
//	fflush(stdout);

        cl_event kernel_exec_event[NUM_QUEUES];
	cl_kernel kernel[NUM_KERNELS_TO_CREATE];

	cl_mem d_matrix_mul_outputC;
	cl_mem d_matrix_mul_inputA;
	cl_mem d_matrix_mul_inputB;

	
//        printf("\n===== Host-CPU transferring matrices A,B to the FPGA device global memory (DDR4) via PCIe ======\n\n");


	d_matrix_mul_inputA = clCreateBuffer(
		context,
		//CL_MEM_READ_ONLY | CL_MEM_BANK_1_ALTERA,
		CL_MEM_READ_ONLY,
		num_elem_A*sizeof(cl_float),
		NULL,
		&status); CHECK(status);

	d_matrix_mul_inputB = clCreateBuffer(
		context,
		//CL_MEM_READ_ONLY | CL_MEM_BANK_2_ALTERA,
		CL_MEM_READ_ONLY,
		num_elem_B*sizeof(cl_float),
		NULL,
		&status); CHECK(status);

	d_matrix_mul_outputC = clCreateBuffer(
		context,
		//CL_MEM_WRITE_ONLY | CL_MEM_BANK_1_ALTERA,
		CL_MEM_WRITE_ONLY,
		num_elem_C*sizeof(cl_float),
		NULL,
		&status); CHECK(status);


        // blocking writes
	status = clEnqueueWriteBuffer(
		cmdQueue[KID_FEED_MAT_A],
		d_matrix_mul_inputA,
		CL_TRUE,
		0,
		num_elem_A*sizeof(cl_float),
		matrix_mul_inputA_block_wise,
		//matrix_mul_inputA,
		0,
		NULL,
		NULL); CHECK(status);

	status = clEnqueueWriteBuffer(
		cmdQueue[KID_FEED_MAT_B],
		d_matrix_mul_inputB,
		CL_TRUE,
		0,
		num_elem_B*sizeof(cl_float),
		matrix_mul_inputB_block_wise,
		//matrix_mul_inputB,
		0,
		NULL,
		NULL); CHECK(status);

	status = clEnqueueWriteBuffer(
		cmdQueue[KID_DRAIN_MAT_C],
		d_matrix_mul_outputC,
		CL_TRUE,
		0,
		num_elem_C*sizeof(cl_float),
		matrix_mul_outputC,
		//matrix_mul_inputB,
		0,
		NULL,
		NULL); CHECK(status);


   

    for(int j=0; j<NUM_KERNELS_TO_CREATE; j++) {
//        printf("Creating kernel[%d]: %s\n", j,kernel_name[j]);
        kernel[j] = clCreateKernel(program, (const char*)kernel_name[j], &status); 
        CHECK(status);
    }

	fflush(stdout);

	unsigned int mat_a_num_vectors_in_row_of_blocks = //MAT_A_NUM_VECTORS_IN_ROW_OF_BLOCKS;
                   BNUM_K * MAT_A_BLOCK_NUM_VECTORS;
	unsigned char mat_a_num_blocks_in_col = BNUM_I; //MAT_A_NUM_BLOCKS_IN_COL;
	unsigned char mat_b_num_blocks_in_row = BNUM_J; //MAT_B_NUM_BLOCKS_IN_ROW;

        status = clSetKernelArg(
		kernel[KID_FEED_MAT_A],
		0,
		sizeof(cl_mem),
		(void*)&d_matrix_mul_inputA); CHECK(status);

        status = clSetKernelArg(
		kernel[KID_FEED_MAT_A],
		1,
		sizeof(unsigned int),
		(void*)&mat_a_num_vectors_in_row_of_blocks); CHECK(status);

        status = clSetKernelArg(
		kernel[KID_FEED_MAT_A],
		2,
		sizeof(unsigned char),
		(void*)&mat_a_num_blocks_in_col); CHECK(status);

        status = clSetKernelArg(
		kernel[KID_FEED_MAT_A],
		3,
		sizeof(unsigned char),
		(void*)&mat_b_num_blocks_in_row); CHECK(status);

	unsigned int mat_b_num_vectors_in_col_of_blocks = //MAT_B_NUM_VECTORS_IN_COL_OF_BLOCKS;
                    BNUM_J * MAT_B_BLOCK_NUM_VECTORS;
	unsigned int mat_b_num_vectors_in_matrix = BNUM_J * BNUM_K * MAT_B_BLOCK_NUM_VECTORS; //MAT_B_NUM_VECTORS_IN_MATRIX;

        status = clSetKernelArg(
		kernel[KID_FEED_MAT_B],
		0,
		sizeof(cl_mem),
		(void*)&d_matrix_mul_inputB); CHECK(status);

        status = clSetKernelArg(
		kernel[KID_FEED_MAT_B],
		1,
		sizeof(unsigned int),
		(void*)&mat_b_num_vectors_in_col_of_blocks); CHECK(status);

        status = clSetKernelArg(
		kernel[KID_FEED_MAT_B],
		2,
		sizeof(unsigned int),
		(void*)&mat_b_num_vectors_in_matrix); CHECK(status);

        status = clSetKernelArg(
		kernel[KID_FEED_MAT_B],
		3,
		sizeof(unsigned char),
		(void*)&mat_a_num_blocks_in_col); CHECK(status);

	int mat_c_num_coalesced_words = //WC * HC / SYS_ARRAY_NUM_COLS;
        //BNUM_I * BNUM_J * MAT_A_BLOCK_HEIGHT* MAT_B_BLOCK_WIDTH / SYS_ARRAY_NUM_COLS;
        BNUM_I * BNUM_J * MAT_A_BLOCK_HEIGHT* (COLUMNS_INTERLEAVED * SYS_ARRAY_NUM_COLS) / SYS_ARRAY_NUM_COLS;

	status = clSetKernelArg(
		kernel[KID_DRAIN_MAT_C],
		0,
		sizeof(cl_mem),
		(void*)&d_matrix_mul_outputC); CHECK(status);

	status = clSetKernelArg(
		kernel[KID_DRAIN_MAT_C],
		1,
		sizeof(int),
		(void*)&mat_c_num_coalesced_words); CHECK(status);

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
	LOAD_MAT_A,
	LOAD_MAT_B, 
	DRAIN_C_WRITE_TREE, 
	DRAIN_C_CHAIN,
	FEED_MAT_A,
	FEED_MAT_B,
	PE
};
        sleep(nsec);
//	status = clFinish(cmdQueue[LOAD_MAT_A]); CHECK(status);
//	status = clFinish(cmdQueue[LOAD_MAT_B]); CHECK(status);
//	status = clFinish(cmdQueue[DRAIN_C_WRITE_TREE]); CHECK(status);
//	status = clFinish(cmdQueue[DRAIN_C_CHAIN]); CHECK(status);
//	status = clFinish(cmdQueue[FEED_MAT_A]); CHECK(status);
//	status = clFinish(cmdQueue[FEED_MAT_B]); CHECK(status);
//	status = clFinish(cmdQueue[PE]); CHECK(status);

#endif

        for(i=0; i < NUM_QUEUES_TO_FINISH; i++) {
#ifndef KERNEL_TIMEOUT
           status = clFinish(cmdQueue[i]); CHECK(status);
#endif
	}

        printf(" *** FPGA execution finished!\n");

	fflush(stdout);
//	exit(1);


	for (i=0; i<NUM_QUEUES_TO_FINISH; i++) {
          k_exec_time[i] = compute_kernel_execution_time(kernel_exec_event[i], k_start_time[i], k_end_time[i]);     
        }     
//        printf("\n\n");
    
//        printf("\n===== Host-CPU transferring result matrix C from the FPGA device global memory (DDR4) via PCIe ======\n\n");
	
      // Read the results back from the device, blocking read
        clEnqueueReadBuffer(
              //cmdQueue[KID_DRAIN_MAT_C],
              cmdQueue[NUM_KERNELS_TO_CREATE], // using a special queue for reading buffer C
              d_matrix_mul_outputC,
              CL_TRUE,
              0,
              num_elem_C*sizeof(cl_float),
              matrix_mul_outputC,
              0,
              NULL,
              NULL); CHECK(status);

 	// printf("Matrix C (FPGA result)\n");
	// print_matrix(matrix_mul_outputC, HC, WC);
	// printf("\n");

//    printf("1.0e-5f=%f\n", 1.0e-5f);
//    printf("1.0e-6f=%f\n", 1.0e-6f);


	for(i=0; i<NUM_KERNELS_TO_CREATE; i++) {
          clReleaseKernel(kernel[i]);
	}

	for(i=0; i<NUM_QUEUES_TO_FINISH; i++) {
         clReleaseEvent(kernel_exec_event[i]);
        }

	clReleaseMemObject(d_matrix_mul_inputA);
	clReleaseMemObject(d_matrix_mul_inputB);
	clReleaseMemObject(d_matrix_mul_outputC);

}
