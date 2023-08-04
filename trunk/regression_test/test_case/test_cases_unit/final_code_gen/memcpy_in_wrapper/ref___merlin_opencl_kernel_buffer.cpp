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
opencl_kernel __md_kernel_kernel;
cl_event __event_md_kernel;
opencl_mem __md_kernel__force_x_buffer;
cl_event __event___md_kernel__force_x_buffer;
opencl_mem __md_kernel__force_y_buffer;
cl_event __event___md_kernel__force_y_buffer;
opencl_mem __md_kernel__force_z_buffer;
cl_event __event___md_kernel__force_z_buffer;
opencl_mem __md_kernel__n_points_buffer;
cl_event __event___md_kernel__n_points_buffer;
opencl_mem __md_kernel__position_x_buffer;
cl_event __event___md_kernel__position_x_buffer;
opencl_mem __md_kernel__position_y_buffer;
cl_event __event___md_kernel__position_y_buffer;
opencl_mem __md_kernel__position_z_buffer;
cl_event __event___md_kernel__position_z_buffer;
int __merlin_init_md_kernel(){
	#pragma ACCEL debug printf("[Merlin Info] Start create kernel for %s....\n", "md_kernel");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_kernel(&__md_kernel_kernel, (char *)"md_kernel");
	#pragma ACCEL debug printf("[Merlin Info] Successful create kernel for %s....\n", "md_kernel");
	#pragma ACCEL debug fflush(stdout);
	#pragma ACCEL debug printf("[Merlin Info] Start create buffer for %s....\n", "force_x");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_buffer(&__md_kernel__force_x_buffer, (int64_t)8*4*4*4*10, 2, NULL);
	#pragma ACCEL debug printf("[Merlin Info] Successful create buffer for %s....\n", "force_x");
	#pragma ACCEL debug fflush(stdout);
	#pragma ACCEL debug printf("[Merlin Info] Start create buffer for %s....\n", "force_y");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_buffer(&__md_kernel__force_y_buffer, (int64_t)8*4*4*4*10, 2, NULL);
	#pragma ACCEL debug printf("[Merlin Info] Successful create buffer for %s....\n", "force_y");
	#pragma ACCEL debug fflush(stdout);
	#pragma ACCEL debug printf("[Merlin Info] Start create buffer for %s....\n", "force_z");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_buffer(&__md_kernel__force_z_buffer, (int64_t)8*4*4*4*10, 2, NULL);
	#pragma ACCEL debug printf("[Merlin Info] Successful create buffer for %s....\n", "force_z");
	#pragma ACCEL debug fflush(stdout);
	#pragma ACCEL debug printf("[Merlin Info] Start create buffer for %s....\n", "n_points");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_buffer(&__md_kernel__n_points_buffer, (int64_t)4*4*4*4, 2, NULL);
	#pragma ACCEL debug printf("[Merlin Info] Successful create buffer for %s....\n", "n_points");
	#pragma ACCEL debug fflush(stdout);
	#pragma ACCEL debug printf("[Merlin Info] Start create buffer for %s....\n", "position_x");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_buffer(&__md_kernel__position_x_buffer, (int64_t)8*4*4*4*10, 2, NULL);
	#pragma ACCEL debug printf("[Merlin Info] Successful create buffer for %s....\n", "position_x");
	#pragma ACCEL debug fflush(stdout);
	#pragma ACCEL debug printf("[Merlin Info] Start create buffer for %s....\n", "position_y");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_buffer(&__md_kernel__position_y_buffer, (int64_t)8*4*4*4*10, 2, NULL);
	#pragma ACCEL debug printf("[Merlin Info] Successful create buffer for %s....\n", "position_y");
	#pragma ACCEL debug fflush(stdout);
	#pragma ACCEL debug printf("[Merlin Info] Start create buffer for %s....\n", "position_z");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_buffer(&__md_kernel__position_z_buffer, (int64_t)8*4*4*4*10, 2, NULL);
	#pragma ACCEL debug printf("[Merlin Info] Successful create buffer for %s....\n", "position_z");
	#pragma ACCEL debug fflush(stdout);
	return 0;
}
int opencl_init_kernel_buffer(){
	int __merlin_init_md_kernel();
	return 0;
}
int __merlin_release_md_kernel(){
	if(__md_kernel_kernel) {
		opencl_release_kernel(__md_kernel_kernel);
	}
	if(__event_md_kernel) {
		opencl_release_event(__event_md_kernel);
	}
	if(__md_kernel__force_x_buffer) {
		opencl_release_mem_object(__md_kernel__force_x_buffer);
	}
	if(__md_kernel__force_y_buffer) {
		opencl_release_mem_object(__md_kernel__force_y_buffer);
	}
	if(__md_kernel__force_z_buffer) {
		opencl_release_mem_object(__md_kernel__force_z_buffer);
	}
	if(__md_kernel__n_points_buffer) {
		opencl_release_mem_object(__md_kernel__n_points_buffer);
	}
	if(__md_kernel__position_x_buffer) {
		opencl_release_mem_object(__md_kernel__position_x_buffer);
	}
	if(__md_kernel__position_y_buffer) {
		opencl_release_mem_object(__md_kernel__position_y_buffer);
	}
	if(__md_kernel__position_z_buffer) {
		opencl_release_mem_object(__md_kernel__position_z_buffer);
	}
	return 0;
}
int __merlin_release_kernel_buffer(){
	int __merlin_release_md_kernel();
	return 0;
}
int __merlin_wait_kernel_md_kernel(){
	opencl_wait_event(__event_md_kernel);
	return 0;
}
int __merlin_wait_write_md_kernel(){
		opencl_wait_event(__event___md_kernel__force_x_buffer);
	if(__md_kernel__force_x_buffer) {
		opencl_release_mem_object(__md_kernel__force_x_buffer);
	}
		opencl_wait_event(__event___md_kernel__force_y_buffer);
	if(__md_kernel__force_y_buffer) {
		opencl_release_mem_object(__md_kernel__force_y_buffer);
	}
		opencl_wait_event(__event___md_kernel__force_z_buffer);
	if(__md_kernel__force_z_buffer) {
		opencl_release_mem_object(__md_kernel__force_z_buffer);
	}
		opencl_wait_event(__event___md_kernel__n_points_buffer);
	if(__md_kernel__n_points_buffer) {
		opencl_release_mem_object(__md_kernel__n_points_buffer);
	}
		opencl_wait_event(__event___md_kernel__position_x_buffer);
	if(__md_kernel__position_x_buffer) {
		opencl_release_mem_object(__md_kernel__position_x_buffer);
	}
		opencl_wait_event(__event___md_kernel__position_y_buffer);
	if(__md_kernel__position_y_buffer) {
		opencl_release_mem_object(__md_kernel__position_y_buffer);
	}
		opencl_wait_event(__event___md_kernel__position_z_buffer);
	if(__md_kernel__position_z_buffer) {
		opencl_release_mem_object(__md_kernel__position_z_buffer);
	}
	return 0;
}
int __merlin_wait_read_md_kernel(){
		opencl_wait_event(__event___md_kernel__force_x_buffer);
	if(__md_kernel__force_x_buffer) {
		opencl_release_mem_object(__md_kernel__force_x_buffer);
	}
		opencl_wait_event(__event___md_kernel__force_y_buffer);
	if(__md_kernel__force_y_buffer) {
		opencl_release_mem_object(__md_kernel__force_y_buffer);
	}
		opencl_wait_event(__event___md_kernel__force_z_buffer);
	if(__md_kernel__force_z_buffer) {
		opencl_release_mem_object(__md_kernel__force_z_buffer);
	}
		opencl_wait_event(__event___md_kernel__n_points_buffer);
	if(__md_kernel__n_points_buffer) {
		opencl_release_mem_object(__md_kernel__n_points_buffer);
	}
		opencl_wait_event(__event___md_kernel__position_x_buffer);
	if(__md_kernel__position_x_buffer) {
		opencl_release_mem_object(__md_kernel__position_x_buffer);
	}
		opencl_wait_event(__event___md_kernel__position_y_buffer);
	if(__md_kernel__position_y_buffer) {
		opencl_release_mem_object(__md_kernel__position_y_buffer);
	}
		opencl_wait_event(__event___md_kernel__position_z_buffer);
	if(__md_kernel__position_z_buffer) {
		opencl_release_mem_object(__md_kernel__position_z_buffer);
	}
	return 0;
}
