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
opencl_kernel _____begin____kernel;
cl_event __event____begin___;
opencl_mem _____begin_____a_buffer;
cl_event __event______begin_____a_buffer;
opencl_mem _____begin_____b_buffer;
cl_event __event______begin_____b_buffer;
opencl_mem _____begin_____c_buffer;
cl_event __event______begin_____c_buffer;
int __merlin_init____begin___(){
	#pragma ACCEL debug printf("[Merlin Info] Start create kernel for %s....\n", "___begin___");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_kernel(&_____begin____kernel, (char *)"begin_merlinkernel0");
	#pragma ACCEL debug printf("[Merlin Info] Successful create kernel for %s....\n", "___begin___");
	#pragma ACCEL debug fflush(stdout);
	#pragma ACCEL debug printf("[Merlin Info] Start create buffer for %s....\n", "a");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_buffer(&_____begin_____a_buffer, (int64_t)64, 2, NULL);
	#pragma ACCEL debug printf("[Merlin Info] Successful create buffer for %s....\n", "a");
	#pragma ACCEL debug fflush(stdout);
	#pragma ACCEL debug printf("[Merlin Info] Start create buffer for %s....\n", "b");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_buffer(&_____begin_____b_buffer, (int64_t)64, 2, NULL);
	#pragma ACCEL debug printf("[Merlin Info] Successful create buffer for %s....\n", "b");
	#pragma ACCEL debug fflush(stdout);
	#pragma ACCEL debug printf("[Merlin Info] Start create buffer for %s....\n", "c");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_buffer(&_____begin_____c_buffer, (int64_t)64, 2, NULL);
	#pragma ACCEL debug printf("[Merlin Info] Successful create buffer for %s....\n", "c");
	#pragma ACCEL debug fflush(stdout);
	return 0;
}
int opencl_init_kernel_buffer(){
	int __merlin_init____begin___();
	return 0;
}
int __merlin_release____begin___(){
	if(_____begin____kernel) {
		opencl_release_kernel(_____begin____kernel);
	}
	if(__event____begin___) {
		opencl_release_event(__event____begin___);
	}
	if(_____begin_____a_buffer) {
		opencl_release_mem_object(_____begin_____a_buffer);
	}
	if(_____begin_____b_buffer) {
		opencl_release_mem_object(_____begin_____b_buffer);
	}
	if(_____begin_____c_buffer) {
		opencl_release_mem_object(_____begin_____c_buffer);
	}
	return 0;
}
int __merlin_release_kernel_buffer(){
	int __merlin_release____begin___();
	return 0;
}
int __merlin_wait_kernel____begin___(){
	opencl_wait_event(__event____begin___);
	return 0;
}
int __merlin_wait_write____begin___(){
	return 0;
}
int __merlin_wait_read____begin___(){
	return 0;
}
