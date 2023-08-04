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
opencl_kernel __align_kernel_fpga_kernel;
cl_event __event_align_kernel_fpga;
opencl_mem __align_kernel_fpga__profbuf_size_vec_buffer;
cl_event __event___align_kernel_fpga__profbuf_size_vec_buffer;
opencl_mem __align_kernel_fpga__profbuf_vec_data_buffer;
cl_event __event___align_kernel_fpga__profbuf_vec_data_buffer;
opencl_mem __align_kernel_fpga__results_buffer;
cl_event __event___align_kernel_fpga__results_buffer;
opencl_mem __align_kernel_fpga__rf_len_vec_buffer;
cl_event __event___align_kernel_fpga__rf_len_vec_buffer;
opencl_mem __align_kernel_fpga__rf_vec_buffer;
cl_event __event___align_kernel_fpga__rf_vec_buffer;
opencl_kernel __msm_port_rf_vec_align_kernel_fpga_0_kernel;
cl_event __event_msm_port_rf_vec_align_kernel_fpga_0;
opencl_kernel __msm_port_rf_len_vec_align_kernel_fpga_0_kernel;
cl_event __event_msm_port_rf_len_vec_align_kernel_fpga_0;
opencl_kernel __msm_port_profbuf_size_vec_align_kernel_fpga_0_kernel;
cl_event __event_msm_port_profbuf_size_vec_align_kernel_fpga_0;
opencl_kernel __msm_port_profbuf_vec_data_align_kernel_fpga_0_kernel;
cl_event __event_msm_port_profbuf_vec_data_align_kernel_fpga_0;
opencl_kernel __msm_port_results_align_kernel_fpga_0_kernel;
cl_event __event_msm_port_results_align_kernel_fpga_0;
int __merlin_init_align_kernel_fpga(){
	#pragma ACCEL debug printf("[Merlin Info] Start create kernel for %s....\n", "align_kernel_fpga");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_kernel(&__align_kernel_fpga_kernel, (char *)"align_kernel_fpga");
	#pragma ACCEL debug printf("[Merlin Info] Successful create kernel for %s....\n", "align_kernel_fpga");
	#pragma ACCEL debug fflush(stdout);
	#pragma ACCEL debug printf("[Merlin Info] Start create buffer for %s....\n", "profbuf_size_vec");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_buffer(&__align_kernel_fpga__profbuf_size_vec_buffer, (int64_t)4*10000, 2, NULL);
	#pragma ACCEL debug printf("[Merlin Info] Successful create buffer for %s....\n", "profbuf_size_vec");
	#pragma ACCEL debug fflush(stdout);
	#pragma ACCEL debug printf("[Merlin Info] Start create buffer for %s....\n", "profbuf_vec_data");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_buffer(&__align_kernel_fpga__profbuf_vec_data_buffer, (int64_t)1*1280000*16, 2, NULL);
	#pragma ACCEL debug printf("[Merlin Info] Successful create buffer for %s....\n", "profbuf_vec_data");
	#pragma ACCEL debug fflush(stdout);
	#pragma ACCEL debug printf("[Merlin Info] Start create buffer for %s....\n", "results");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_buffer(&__align_kernel_fpga__results_buffer, (int64_t)8*10000, 2, NULL);
	#pragma ACCEL debug printf("[Merlin Info] Successful create buffer for %s....\n", "results");
	#pragma ACCEL debug fflush(stdout);
	#pragma ACCEL debug printf("[Merlin Info] Start create buffer for %s....\n", "rf_len_vec");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_buffer(&__align_kernel_fpga__rf_len_vec_buffer, (int64_t)4*10000, 2, NULL);
	#pragma ACCEL debug printf("[Merlin Info] Successful create buffer for %s....\n", "rf_len_vec");
	#pragma ACCEL debug fflush(stdout);
	#pragma ACCEL debug printf("[Merlin Info] Start create buffer for %s....\n", "rf_vec");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_buffer(&__align_kernel_fpga__rf_vec_buffer, (int64_t)1*20480000, 2, NULL);
	#pragma ACCEL debug printf("[Merlin Info] Successful create buffer for %s....\n", "rf_vec");
	#pragma ACCEL debug fflush(stdout);
	#pragma ACCEL debug printf("[Merlin Info] Start create kernel for %s....\n", "msm_port_rf_vec_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_kernel(&__msm_port_rf_vec_align_kernel_fpga_0_kernel, (char *)"msm_port_rf_vec_align_kernel_fpga_0");
	#pragma ACCEL debug printf("[Merlin Info] Successful create kernel for %s....\n", "msm_port_rf_vec_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
	#pragma ACCEL debug printf("[Merlin Info] Start create kernel for %s....\n", "msm_port_rf_len_vec_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_kernel(&__msm_port_rf_len_vec_align_kernel_fpga_0_kernel, (char *)"msm_port_rf_len_vec_align_kernel_fpga_0");
	#pragma ACCEL debug printf("[Merlin Info] Successful create kernel for %s....\n", "msm_port_rf_len_vec_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
	#pragma ACCEL debug printf("[Merlin Info] Start create kernel for %s....\n", "msm_port_profbuf_size_vec_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_kernel(&__msm_port_profbuf_size_vec_align_kernel_fpga_0_kernel, (char *)"msm_port_profbuf_size_vec_align_kernel_fpga_0");
	#pragma ACCEL debug printf("[Merlin Info] Successful create kernel for %s....\n", "msm_port_profbuf_size_vec_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
	#pragma ACCEL debug printf("[Merlin Info] Start create kernel for %s....\n", "msm_port_profbuf_vec_data_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_kernel(&__msm_port_profbuf_vec_data_align_kernel_fpga_0_kernel, (char *)"msm_port_profbuf_vec_data_align_kernel_fpga_0");
	#pragma ACCEL debug printf("[Merlin Info] Successful create kernel for %s....\n", "msm_port_profbuf_vec_data_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
	#pragma ACCEL debug printf("[Merlin Info] Start create kernel for %s....\n", "msm_port_results_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
	opencl_create_kernel(&__msm_port_results_align_kernel_fpga_0_kernel, (char *)"msm_port_results_align_kernel_fpga_0");
	#pragma ACCEL debug printf("[Merlin Info] Successful create kernel for %s....\n", "msm_port_results_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
	return 0;
}
int opencl_init_kernel_buffer(){
	int __merlin_init_align_kernel_fpga();
	return 0;
}
int __merlin_release_align_kernel_fpga(){
	if(__align_kernel_fpga_kernel) {
		opencl_release_kernel(__align_kernel_fpga_kernel);
	}
	if(__event_align_kernel_fpga) {
		opencl_release_event(__event_align_kernel_fpga);
	}
	if(__align_kernel_fpga__profbuf_size_vec_buffer) {
		opencl_release_mem_object(__align_kernel_fpga__profbuf_size_vec_buffer);
	}
	if(__align_kernel_fpga__profbuf_vec_data_buffer) {
		opencl_release_mem_object(__align_kernel_fpga__profbuf_vec_data_buffer);
	}
	if(__align_kernel_fpga__results_buffer) {
		opencl_release_mem_object(__align_kernel_fpga__results_buffer);
	}
	if(__align_kernel_fpga__rf_len_vec_buffer) {
		opencl_release_mem_object(__align_kernel_fpga__rf_len_vec_buffer);
	}
	if(__align_kernel_fpga__rf_vec_buffer) {
		opencl_release_mem_object(__align_kernel_fpga__rf_vec_buffer);
	}
	return 0;
}
int __merlin_release_kernel_buffer(){
	int __merlin_release_align_kernel_fpga();
	return 0;
}
int __merlin_wait_kernel_align_kernel_fpga(){
	opencl_wait_event(__event_align_kernel_fpga);
	opencl_wait_event(__event_msm_port_rf_vec_align_kernel_fpga_0);
	opencl_wait_event(__event_msm_port_rf_len_vec_align_kernel_fpga_0);
	opencl_wait_event(__event_msm_port_profbuf_size_vec_align_kernel_fpga_0);
	opencl_wait_event(__event_msm_port_profbuf_vec_data_align_kernel_fpga_0);
	opencl_wait_event(__event_msm_port_results_align_kernel_fpga_0);
	return 0;
}
int __merlin_wait_write_align_kernel_fpga(){
		opencl_wait_event(__event___align_kernel_fpga__profbuf_size_vec_buffer);
	if(__align_kernel_fpga__profbuf_size_vec_buffer) {
		opencl_release_mem_object(__align_kernel_fpga__profbuf_size_vec_buffer);
	}
		opencl_wait_event(__event___align_kernel_fpga__profbuf_vec_data_buffer);
	if(__align_kernel_fpga__profbuf_vec_data_buffer) {
		opencl_release_mem_object(__align_kernel_fpga__profbuf_vec_data_buffer);
	}
		opencl_wait_event(__event___align_kernel_fpga__rf_vec_buffer);
	if(__align_kernel_fpga__rf_vec_buffer) {
		opencl_release_mem_object(__align_kernel_fpga__rf_vec_buffer);
	}
	return 0;
}
int __merlin_wait_read_align_kernel_fpga(){
		opencl_wait_event(__event___align_kernel_fpga__profbuf_vec_data_buffer);
	if(__align_kernel_fpga__profbuf_vec_data_buffer) {
		opencl_release_mem_object(__align_kernel_fpga__profbuf_vec_data_buffer);
	}
		opencl_wait_event(__event___align_kernel_fpga__results_buffer);
	if(__align_kernel_fpga__results_buffer) {
		opencl_release_mem_object(__align_kernel_fpga__results_buffer);
	}
	return 0;
}
