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
opencl_kernel __ellpack_kernel_kernel;
cl_event __event_ellpack_kernel;
opencl_mem __ellpack_kernel__cols_buffer;
cl_event __event___ellpack_kernel__cols_buffer;
opencl_mem __ellpack_kernel__merlin_out_buffer;
cl_event __event___ellpack_kernel__merlin_out_buffer;
opencl_mem __ellpack_kernel__nzval_buffer;
cl_event __event___ellpack_kernel__nzval_buffer;
opencl_mem __ellpack_kernel__vec_buffer;
cl_event __event___ellpack_kernel__vec_buffer;
int __merlin_init_ellpack_kernel(){
	#pragma ACCEL debug printf("[Merlin Info] Start create kernel for %s....\n", "ellpack_kernel");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_kernel(&__ellpack_kernel_kernel, (char *)"ellpack_kernel");
	#pragma ACCEL debug printf("[Merlin Info] Successful create kernel for %s....\n", "ellpack_kernel");
	#pragma ACCEL debug fflush(stdout);
	#pragma ACCEL debug printf("[Merlin Info] Start create buffer for %s....\n", "cols");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_buffer(&__ellpack_kernel__cols_buffer, (int64_t)4*2097152, 2, NULL);
	#pragma ACCEL debug printf("[Merlin Info] Successful create buffer for %s....\n", "cols");
	#pragma ACCEL debug fflush(stdout);
	#pragma ACCEL debug printf("[Merlin Info] Start create buffer for %s....\n", "merlin_out");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_buffer(&__ellpack_kernel__merlin_out_buffer, (int64_t)8*4096, 2, NULL);
	#pragma ACCEL debug printf("[Merlin Info] Successful create buffer for %s....\n", "merlin_out");
	#pragma ACCEL debug fflush(stdout);
	#pragma ACCEL debug printf("[Merlin Info] Start create buffer for %s....\n", "nzval");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_buffer(&__ellpack_kernel__nzval_buffer, (int64_t)8*2097152, 2, NULL);
	#pragma ACCEL debug printf("[Merlin Info] Successful create buffer for %s....\n", "nzval");
	#pragma ACCEL debug fflush(stdout);
	#pragma ACCEL debug printf("[Merlin Info] Start create buffer for %s....\n", "vec");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_buffer(&__ellpack_kernel__vec_buffer, (int64_t)8*4096, 2, NULL);
	#pragma ACCEL debug printf("[Merlin Info] Successful create buffer for %s....\n", "vec");
	#pragma ACCEL debug fflush(stdout);
	return 0;
}
int opencl_init_kernel_buffer(){
	int __merlin_init_ellpack_kernel();
	return 0;
}
int __merlin_release_ellpack_kernel(){
	if(__ellpack_kernel_kernel) {
		opencl_release_kernel(__ellpack_kernel_kernel);
	}
	if(__event_ellpack_kernel) {
		opencl_release_event(__event_ellpack_kernel);
	}
	if(__ellpack_kernel__cols_buffer) {
		opencl_release_mem_object(__ellpack_kernel__cols_buffer);
	}
	if(__ellpack_kernel__merlin_out_buffer) {
		opencl_release_mem_object(__ellpack_kernel__merlin_out_buffer);
	}
	if(__ellpack_kernel__nzval_buffer) {
		opencl_release_mem_object(__ellpack_kernel__nzval_buffer);
	}
	if(__ellpack_kernel__vec_buffer) {
		opencl_release_mem_object(__ellpack_kernel__vec_buffer);
	}
	return 0;
}
int __merlin_release_kernel_buffer(){
	int __merlin_release_ellpack_kernel();
	return 0;
}
int __merlin_wait_kernel_ellpack_kernel(){
	opencl_wait_event(__event_ellpack_kernel);
	return 0;
}
int __merlin_wait_write_ellpack_kernel(){
		opencl_wait_event(__event___ellpack_kernel__cols_buffer);
	if(__ellpack_kernel__cols_buffer) {
		opencl_release_mem_object(__ellpack_kernel__cols_buffer);
	}
		opencl_wait_event(__event___ellpack_kernel__merlin_out_buffer);
	if(__ellpack_kernel__merlin_out_buffer) {
		opencl_release_mem_object(__ellpack_kernel__merlin_out_buffer);
	}
		opencl_wait_event(__event___ellpack_kernel__nzval_buffer);
	if(__ellpack_kernel__nzval_buffer) {
		opencl_release_mem_object(__ellpack_kernel__nzval_buffer);
	}
		opencl_wait_event(__event___ellpack_kernel__vec_buffer);
	if(__ellpack_kernel__vec_buffer) {
		opencl_release_mem_object(__ellpack_kernel__vec_buffer);
	}
	return 0;
}
int __merlin_wait_read_ellpack_kernel(){
		opencl_wait_event(__event___ellpack_kernel__merlin_out_buffer);
	if(__ellpack_kernel__merlin_out_buffer) {
		opencl_release_mem_object(__ellpack_kernel__merlin_out_buffer);
	}
	return 0;
}
