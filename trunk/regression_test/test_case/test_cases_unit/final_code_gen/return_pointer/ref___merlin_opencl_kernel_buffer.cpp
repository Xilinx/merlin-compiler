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
#include "__merlin_opencl_kernel_buffer.h"
opencl_kernel __CompressFragment_kernel_kernel;
cl_event __event_CompressFragment_kernel;
opencl_mem __CompressFragment_kernel__merlin_input_buffer;
cl_event __event___CompressFragment_kernel__merlin_input_buffer;
opencl_mem __CompressFragment_kernel__merlin_return_value_buffer;
cl_event __event___CompressFragment_kernel__merlin_return_value_buffer;
opencl_mem __CompressFragment_kernel__op_buffer;
cl_event __event___CompressFragment_kernel__op_buffer;
opencl_mem __CompressFragment_kernel__table_buffer;
cl_event __event___CompressFragment_kernel__table_buffer;
int __merlin_init_CompressFragment_kernel(){
	#pragma ACCEL debug printf("[Merlin Info] Start create kernel for %s....\n", "CompressFragment_kernel");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_kernel(&__CompressFragment_kernel_kernel, (char *)"CompressFragment_kernel");
	#pragma ACCEL debug printf("[Merlin Info] Successful create kernel for %s....\n", "CompressFragment_kernel");
	#pragma ACCEL debug fflush(stdout);
	#pragma ACCEL debug printf("[Merlin Info] Start create buffer for %s....\n", "merlin_input");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_buffer(&__CompressFragment_kernel__merlin_input_buffer, (int64_t)1*65536, 2, NULL);
	#pragma ACCEL debug printf("[Merlin Info] Successful create buffer for %s....\n", "merlin_input");
	#pragma ACCEL debug fflush(stdout);
	#pragma ACCEL debug printf("[Merlin Info] Start create buffer for %s....\n", "merlin_return_value");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_buffer(&__CompressFragment_kernel__merlin_return_value_buffer, (int64_t)4*1, 2, NULL);
	#pragma ACCEL debug printf("[Merlin Info] Successful create buffer for %s....\n", "merlin_return_value");
	#pragma ACCEL debug fflush(stdout);
	#pragma ACCEL debug printf("[Merlin Info] Start create buffer for %s....\n", "op");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_buffer(&__CompressFragment_kernel__op_buffer, (int64_t)1*65536, 2, NULL);
	#pragma ACCEL debug printf("[Merlin Info] Successful create buffer for %s....\n", "op");
	#pragma ACCEL debug fflush(stdout);
	#pragma ACCEL debug printf("[Merlin Info] Start create buffer for %s....\n", "table");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_buffer(&__CompressFragment_kernel__table_buffer, (int64_t)2*256, 2, NULL);
	#pragma ACCEL debug printf("[Merlin Info] Successful create buffer for %s....\n", "table");
	#pragma ACCEL debug fflush(stdout);
	return 0;
}
int opencl_init_kernel_buffer(){
	int __merlin_init_CompressFragment_kernel();
	return 0;
}
int __merlin_release_CompressFragment_kernel(){
	if(__CompressFragment_kernel_kernel) {
		opencl_release_kernel(__CompressFragment_kernel_kernel);
	}
	if(__event_CompressFragment_kernel) {
		opencl_release_event(__event_CompressFragment_kernel);
	}
	if(__CompressFragment_kernel__merlin_input_buffer) {
		opencl_release_mem_object(__CompressFragment_kernel__merlin_input_buffer);
	}
	if(__CompressFragment_kernel__merlin_return_value_buffer) {
		opencl_release_mem_object(__CompressFragment_kernel__merlin_return_value_buffer);
	}
	if(__CompressFragment_kernel__op_buffer) {
		opencl_release_mem_object(__CompressFragment_kernel__op_buffer);
	}
	if(__CompressFragment_kernel__table_buffer) {
		opencl_release_mem_object(__CompressFragment_kernel__table_buffer);
	}
	return 0;
}
int __merlin_release_kernel_buffer(){
	int __merlin_release_CompressFragment_kernel();
	return 0;
}
int __merlin_wait_kernel_CompressFragment_kernel(){
	opencl_wait_event(__event_CompressFragment_kernel);
	return 0;
}
int __merlin_wait_write_CompressFragment_kernel(){
	return 0;
}
int __merlin_wait_read_CompressFragment_kernel(){
	return 0;
}
