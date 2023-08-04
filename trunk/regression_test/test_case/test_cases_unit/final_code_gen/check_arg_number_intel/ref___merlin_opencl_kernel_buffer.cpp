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
opencl_kernel __vec_add_kernel_kernel;
cl_event __event_vec_add_kernel;
opencl_mem __vec_add_kernel__a_buffer;
cl_event __event___vec_add_kernel__a_buffer;
opencl_mem __vec_add_kernel__b_buffer;
cl_event __event___vec_add_kernel__b_buffer;
opencl_mem __vec_add_kernel__c_buffer;
cl_event __event___vec_add_kernel__c_buffer;
int __merlin_init_vec_add_kernel(){
	#pragma ACCEL debug printf("[Merlin Info] Start create kernel for %s....\n", "vec_add_kernel");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_kernel(&__vec_add_kernel_kernel, (char *)"vec_add_kernel");
	#pragma ACCEL debug printf("[Merlin Info] Successful create kernel for %s....\n", "vec_add_kernel");
	#pragma ACCEL debug fflush(stdout);
	#pragma ACCEL debug printf("[Merlin Info] Start create buffer for %s....\n", "a");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_buffer(&__vec_add_kernel__a_buffer, (int64_t)4*10000, 2, NULL);
	#pragma ACCEL debug printf("[Merlin Info] Successful create buffer for %s....\n", "a");
	#pragma ACCEL debug fflush(stdout);
	#pragma ACCEL debug printf("[Merlin Info] Start create buffer for %s....\n", "b");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_buffer(&__vec_add_kernel__b_buffer, (int64_t)4*10000, 2, NULL);
	#pragma ACCEL debug printf("[Merlin Info] Successful create buffer for %s....\n", "b");
	#pragma ACCEL debug fflush(stdout);
	#pragma ACCEL debug printf("[Merlin Info] Start create buffer for %s....\n", "c");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_buffer(&__vec_add_kernel__c_buffer, (int64_t)4*10000, 2, NULL);
	#pragma ACCEL debug printf("[Merlin Info] Successful create buffer for %s....\n", "c");
	#pragma ACCEL debug fflush(stdout);
	return 0;
}
int opencl_init_kernel_buffer(){
	int __merlin_init_vec_add_kernel();
	return 0;
}
int __merlin_release_vec_add_kernel(){
	if(__vec_add_kernel_kernel) {
		opencl_release_kernel(__vec_add_kernel_kernel);
	}
	if(__event_vec_add_kernel) {
		opencl_release_event(__event_vec_add_kernel);
	}
	if(__vec_add_kernel__a_buffer) {
		opencl_release_mem_object(__vec_add_kernel__a_buffer);
	}
	if(__vec_add_kernel__b_buffer) {
		opencl_release_mem_object(__vec_add_kernel__b_buffer);
	}
	if(__vec_add_kernel__c_buffer) {
		opencl_release_mem_object(__vec_add_kernel__c_buffer);
	}
	return 0;
}
int __merlin_release_kernel_buffer(){
	int __merlin_release_vec_add_kernel();
	return 0;
}
int __merlin_wait_kernel_vec_add_kernel(){
	opencl_wait_event(__event_vec_add_kernel);
	return 0;
}
int __merlin_wait_write_vec_add_kernel(){
		opencl_wait_event(__event___vec_add_kernel__a_buffer);
	if(__vec_add_kernel__a_buffer) {
		opencl_release_mem_object(__vec_add_kernel__a_buffer);
	}
		opencl_wait_event(__event___vec_add_kernel__b_buffer);
	if(__vec_add_kernel__b_buffer) {
		opencl_release_mem_object(__vec_add_kernel__b_buffer);
	}
	return 0;
}
int __merlin_wait_read_vec_add_kernel(){
		opencl_wait_event(__event___vec_add_kernel__c_buffer);
	if(__vec_add_kernel__c_buffer) {
		opencl_release_mem_object(__vec_add_kernel__c_buffer);
	}
	return 0;
}
