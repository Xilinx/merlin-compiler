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

#define NUM_NON_AUTORUN_KERNELS 3
#define NUM_KERNELS 	7 //( SYS_ARRAY_NUM_ROWS*SYS_ARRAY_NUM_COLS + SYS_ARRAY_NUM_ROWS + SYS_ARRAY_NUM_COLS + (SYS_ARRAY_NUM_COLS-1) + (1 + 1 + 1) )
#define NUM_QUEUES 	NUM_KERNELS

#ifdef EMULATOR
  // emulator needs to create both non-autorun and autorun kernels
  #define NUM_KERNELS_TO_CREATE   NUM_KERNELS
  #define NUM_QUEUES_TO_CREATE    NUM_QUEUES
#else
  // only create non-autorun kernels for the HW run
  #define NUM_KERNELS_TO_CREATE   NUM_NON_AUTORUN_KERNELS
  #define NUM_QUEUES_TO_CREATE    NUM_NON_AUTORUN_KERNELS
#endif

#define NUM_QUEUES_TO_FINISH    NUM_NON_AUTORUN_KERNELS


// Check the status returned by the OpenCL API functions
#define CHECK(status) 								\
	if (status != CL_SUCCESS)						\
{									\
	fprintf(stderr, "error %d in line %d.\n", status, __LINE__);	\
	exit(1);							\
}									\

// Check the status returned by the OpenCL API functions, don't exit on error
#define CHECK_NO_EXIT(status) 								\
	if (status != CL_SUCCESS)						\
{									\
	fprintf(stderr, "error %d in line %d.\n", status, __LINE__);	\
}									\


#define   KID_FEED_MAT_A 	0
#define   KID_FEED_MAT_B        1
#define   KID_DRAIN_MAT_C 	2


