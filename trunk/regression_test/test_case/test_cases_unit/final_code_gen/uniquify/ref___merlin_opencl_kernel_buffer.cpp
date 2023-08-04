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
opencl_kernel __aes256_encrypt_ecb_kernel_kernel;
cl_event __event_aes256_encrypt_ecb_kernel;
opencl_mem __aes256_encrypt_ecb_kernel__buf_buffer;
cl_event __event___aes256_encrypt_ecb_kernel__buf_buffer;
opencl_mem __aes256_encrypt_ecb_kernel__ctx_deckey_buffer;
cl_event __event___aes256_encrypt_ecb_kernel__ctx_deckey_buffer;
opencl_mem __aes256_encrypt_ecb_kernel__ctx_enckey_buffer;
cl_event __event___aes256_encrypt_ecb_kernel__ctx_enckey_buffer;
opencl_mem __aes256_encrypt_ecb_kernel__ctx_key_buffer;
cl_event __event___aes256_encrypt_ecb_kernel__ctx_key_buffer;
opencl_mem __aes256_encrypt_ecb_kernel__k_buffer;
cl_event __event___aes256_encrypt_ecb_kernel__k_buffer;
int __merlin_init_aes256_encrypt_ecb_kernel(){
	#pragma ACCEL debug printf("[Merlin Info] Start create kernel for %s....\n", "aes256_encrypt_ecb_kernel");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_kernel(&__aes256_encrypt_ecb_kernel_kernel, (char *)"aes256_encrypt_ecb_kernel");
	#pragma ACCEL debug printf("[Merlin Info] Successful create kernel for %s....\n", "aes256_encrypt_ecb_kernel");
	#pragma ACCEL debug fflush(stdout);
	#pragma ACCEL debug printf("[Merlin Info] Start create buffer for %s....\n", "buf");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_buffer(&__aes256_encrypt_ecb_kernel__buf_buffer, (int64_t)1*16, 2, NULL);
	#pragma ACCEL debug printf("[Merlin Info] Successful create buffer for %s....\n", "buf");
	#pragma ACCEL debug fflush(stdout);
	#pragma ACCEL debug printf("[Merlin Info] Start create buffer for %s....\n", "ctx_deckey");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_buffer(&__aes256_encrypt_ecb_kernel__ctx_deckey_buffer, (int64_t)1*1*32, 2, NULL);
	#pragma ACCEL debug printf("[Merlin Info] Successful create buffer for %s....\n", "ctx_deckey");
	#pragma ACCEL debug fflush(stdout);
	#pragma ACCEL debug printf("[Merlin Info] Start create buffer for %s....\n", "ctx_enckey");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_buffer(&__aes256_encrypt_ecb_kernel__ctx_enckey_buffer, (int64_t)1*1*32, 2, NULL);
	#pragma ACCEL debug printf("[Merlin Info] Successful create buffer for %s....\n", "ctx_enckey");
	#pragma ACCEL debug fflush(stdout);
	#pragma ACCEL debug printf("[Merlin Info] Start create buffer for %s....\n", "ctx_key");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_buffer(&__aes256_encrypt_ecb_kernel__ctx_key_buffer, (int64_t)1*1*32, 2, NULL);
	#pragma ACCEL debug printf("[Merlin Info] Successful create buffer for %s....\n", "ctx_key");
	#pragma ACCEL debug fflush(stdout);
	#pragma ACCEL debug printf("[Merlin Info] Start create buffer for %s....\n", "k");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_buffer(&__aes256_encrypt_ecb_kernel__k_buffer, (int64_t)1*32, 2, NULL);
	#pragma ACCEL debug printf("[Merlin Info] Successful create buffer for %s....\n", "k");
	#pragma ACCEL debug fflush(stdout);
	return 0;
}
int opencl_init_kernel_buffer(){
	int __merlin_init_aes256_encrypt_ecb_kernel();
	return 0;
}
int __merlin_release_aes256_encrypt_ecb_kernel(){
	if(__aes256_encrypt_ecb_kernel_kernel) {
		opencl_release_kernel(__aes256_encrypt_ecb_kernel_kernel);
	}
	if(__event_aes256_encrypt_ecb_kernel) {
		opencl_release_event(__event_aes256_encrypt_ecb_kernel);
	}
	if(__aes256_encrypt_ecb_kernel__buf_buffer) {
		opencl_release_mem_object(__aes256_encrypt_ecb_kernel__buf_buffer);
	}
	if(__aes256_encrypt_ecb_kernel__ctx_deckey_buffer) {
		opencl_release_mem_object(__aes256_encrypt_ecb_kernel__ctx_deckey_buffer);
	}
	if(__aes256_encrypt_ecb_kernel__ctx_enckey_buffer) {
		opencl_release_mem_object(__aes256_encrypt_ecb_kernel__ctx_enckey_buffer);
	}
	if(__aes256_encrypt_ecb_kernel__ctx_key_buffer) {
		opencl_release_mem_object(__aes256_encrypt_ecb_kernel__ctx_key_buffer);
	}
	if(__aes256_encrypt_ecb_kernel__k_buffer) {
		opencl_release_mem_object(__aes256_encrypt_ecb_kernel__k_buffer);
	}
	return 0;
}
int __merlin_release_kernel_buffer(){
	int __merlin_release_aes256_encrypt_ecb_kernel();
	return 0;
}
int __merlin_wait_kernel_aes256_encrypt_ecb_kernel(){
	opencl_wait_event(__event_aes256_encrypt_ecb_kernel);
	return 0;
}
int __merlin_wait_write_aes256_encrypt_ecb_kernel(){
		opencl_wait_event(__event___aes256_encrypt_ecb_kernel__buf_buffer);
	if(__aes256_encrypt_ecb_kernel__buf_buffer) {
		opencl_release_mem_object(__aes256_encrypt_ecb_kernel__buf_buffer);
	}
		opencl_wait_event(__event___aes256_encrypt_ecb_kernel__ctx_deckey_buffer);
	if(__aes256_encrypt_ecb_kernel__ctx_deckey_buffer) {
		opencl_release_mem_object(__aes256_encrypt_ecb_kernel__ctx_deckey_buffer);
	}
		opencl_wait_event(__event___aes256_encrypt_ecb_kernel__ctx_enckey_buffer);
	if(__aes256_encrypt_ecb_kernel__ctx_enckey_buffer) {
		opencl_release_mem_object(__aes256_encrypt_ecb_kernel__ctx_enckey_buffer);
	}
		opencl_wait_event(__event___aes256_encrypt_ecb_kernel__ctx_key_buffer);
	if(__aes256_encrypt_ecb_kernel__ctx_key_buffer) {
		opencl_release_mem_object(__aes256_encrypt_ecb_kernel__ctx_key_buffer);
	}
		opencl_wait_event(__event___aes256_encrypt_ecb_kernel__k_buffer);
	if(__aes256_encrypt_ecb_kernel__k_buffer) {
		opencl_release_mem_object(__aes256_encrypt_ecb_kernel__k_buffer);
	}
	return 0;
}
int __merlin_wait_read_aes256_encrypt_ecb_kernel(){
		opencl_wait_event(__event___aes256_encrypt_ecb_kernel__buf_buffer);
	if(__aes256_encrypt_ecb_kernel__buf_buffer) {
		opencl_release_mem_object(__aes256_encrypt_ecb_kernel__buf_buffer);
	}
		opencl_wait_event(__event___aes256_encrypt_ecb_kernel__ctx_deckey_buffer);
	if(__aes256_encrypt_ecb_kernel__ctx_deckey_buffer) {
		opencl_release_mem_object(__aes256_encrypt_ecb_kernel__ctx_deckey_buffer);
	}
		opencl_wait_event(__event___aes256_encrypt_ecb_kernel__ctx_enckey_buffer);
	if(__aes256_encrypt_ecb_kernel__ctx_enckey_buffer) {
		opencl_release_mem_object(__aes256_encrypt_ecb_kernel__ctx_enckey_buffer);
	}
		opencl_wait_event(__event___aes256_encrypt_ecb_kernel__ctx_key_buffer);
	if(__aes256_encrypt_ecb_kernel__ctx_key_buffer) {
		opencl_release_mem_object(__aes256_encrypt_ecb_kernel__ctx_key_buffer);
	}
	return 0;
}
