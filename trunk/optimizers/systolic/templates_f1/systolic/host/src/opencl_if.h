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



#ifndef __OPENCL_IF_H_
#define __OPENCL_IF_H_

#define NUM_QUEUES 1

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


void opencl_model(
       data_bitwidth *weights_block_wise,
       data_bitwidth *ifmap_block_wise,
       data_bitwidth *ofmap_block_wise
	   );

void kernel_top(
		data_bitwidth *weights_block_wise,
		data_bitwidth *ifmap_block_wise,
		data_bitwidth *ofmap_block_wise
		) ;

#endif // __OPENCL_IF_H_
