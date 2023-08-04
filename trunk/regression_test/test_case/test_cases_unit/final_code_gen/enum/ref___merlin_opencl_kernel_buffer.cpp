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
opencl_kernel __top_kernel_kernel;
cl_event __event_top_kernel;
opencl_mem __top_kernel__arr_buffer;
cl_event __event___top_kernel__arr_buffer;
opencl_mem __top_kernel__merlin_return_value_buffer;
cl_event __event___top_kernel__merlin_return_value_buffer;
int __merlin_init_top_kernel(){
	#pragma ACCEL debug printf("[Merlin Info] Start create kernel for %s....\n", "top_kernel");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_kernel(&__top_kernel_kernel, (char *)"top_kernel");
	#pragma ACCEL debug printf("[Merlin Info] Successful create kernel for %s....\n", "top_kernel");
	#pragma ACCEL debug fflush(stdout);
	#pragma ACCEL debug printf("[Merlin Info] Start create buffer for %s....\n", "arr");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_buffer(&__top_kernel__arr_buffer, (int64_t)4*100, 2, NULL);
	#pragma ACCEL debug printf("[Merlin Info] Successful create buffer for %s....\n", "arr");
	#pragma ACCEL debug fflush(stdout);
	#pragma ACCEL debug printf("[Merlin Info] Start create buffer for %s....\n", "merlin_return_value");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_buffer(&__top_kernel__merlin_return_value_buffer, (int64_t)4*1, 2, NULL);
	#pragma ACCEL debug printf("[Merlin Info] Successful create buffer for %s....\n", "merlin_return_value");
	#pragma ACCEL debug fflush(stdout);
	return 0;
}
int opencl_init_kernel_buffer(){
	int __merlin_init_top_kernel();
	return 0;
}
int __merlin_release_top_kernel(){
	if(__top_kernel_kernel) {
		opencl_release_kernel(__top_kernel_kernel);
	}
	if(__event_top_kernel) {
		opencl_release_event(__event_top_kernel);
	}
	if(__top_kernel__arr_buffer) {
		opencl_release_mem_object(__top_kernel__arr_buffer);
	}
	if(__top_kernel__merlin_return_value_buffer) {
		opencl_release_mem_object(__top_kernel__merlin_return_value_buffer);
	}
	return 0;
}
int __merlin_release_kernel_buffer(){
	int __merlin_release_top_kernel();
	return 0;
}
int __merlin_wait_kernel_top_kernel(){
	opencl_wait_event(__event_top_kernel);
	return 0;
}
int __merlin_wait_write_top_kernel(){
		opencl_wait_event(__event___top_kernel__arr_buffer);
	if(__top_kernel__arr_buffer) {
		opencl_release_mem_object(__top_kernel__arr_buffer);
	}
	return 0;
}
int __merlin_wait_read_top_kernel(){
		opencl_wait_event(__event___top_kernel__merlin_return_value_buffer);
	if(__top_kernel__merlin_return_value_buffer) {
		opencl_release_mem_object(__top_kernel__merlin_return_value_buffer);
	}
	return 0;
}
