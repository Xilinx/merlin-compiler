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
#include "memcpy_512.h"
#include <string.h> 
#include <stdio.h> 
#include <stdlib.h> 
void vec_add_kernel(class ap_uint< 512 > a[625],class ap_uint< 512 > b[625],class ap_uint< 512 > c[625],int inc);
#include "__merlinhead_kernel_top.h"

void __merlinwrapper_vec_add_kernel(int a[10000],int b[10000],int c[10000],int inc)
{
	cl_mem v_cl_mem_vec_add_kernel_i[4096];
	int v_cl_mem_vec_add_kernel_i_p=0;
	cl_mem v_cl_mem_vec_add_kernel_o[4096];
	int v_cl_mem_vec_add_kernel_o_p=0;
  __CMOST_COMMENT__ commands;
  
#pragma ACCEL wrapper VARIABLE=a port=a data_type=int depth=10000 max_depth=10000 io=RO copy=false
  
#pragma ACCEL wrapper VARIABLE=b port=b data_type=int depth=10000 max_depth=10000 io=RO copy=false
  
#pragma ACCEL wrapper VARIABLE=c port=c data_type=int depth=10000 max_depth=10000 io=WO copy=false
  
#pragma ACCEL wrapper VARIABLE=inc port=inc data_type=int
	cl_mem __vec_add_kernel__a_buffer__migrate;
  static int *__m_a;
	posix_memalign((void**)&__m_a, 256, 10000 * 4);
	opencl_create_buffer(&__vec_add_kernel__a_buffer__migrate, 10000 * 4, 2, __m_a);
	v_cl_mem_vec_add_kernel_i[v_cl_mem_vec_add_kernel_i_p++]=__vec_add_kernel__a_buffer__migrate;
	cl_mem __vec_add_kernel__b_buffer__migrate;
  static int *__m_b;
	posix_memalign((void**)&__m_b, 256, 10000 * 4);
	opencl_create_buffer(&__vec_add_kernel__b_buffer__migrate, 10000 * 4, 2, __m_b);
	v_cl_mem_vec_add_kernel_i[v_cl_mem_vec_add_kernel_i_p++]=__vec_add_kernel__b_buffer__migrate;
	cl_mem __vec_add_kernel__c_buffer__migrate;
  static int *__m_c;
	posix_memalign((void**)&__m_c, 256, 10000 * 4);
	opencl_create_buffer(&__vec_add_kernel__c_buffer__migrate, 10000 * 4, 2, __m_c);
	v_cl_mem_vec_add_kernel_o[v_cl_mem_vec_add_kernel_o_p++]=__vec_add_kernel__c_buffer__migrate;
  if (a != 0) {
    __CMOST_COMMENT__ __vec_add_kernel__a_buffer;
    memcpy((void *)(__m_a + 0),(const void *)a,10000 * sizeof(int ));
  }
   else {
    printf("Error! Detected null pointer 'a' for external memory.\n");
    exit(1);
  }
  if (b != 0) {
    __CMOST_COMMENT__ __vec_add_kernel__b_buffer;
    memcpy((void *)(__m_b + 0),(const void *)b,10000 * sizeof(int ));
  }
   else {
    printf("Error! Detected null pointer 'b' for external memory.\n");
    exit(1);
  }
  
#pragma ACCEL kernel
	#pragma ACCEL debug printf("[Merlin Info] Start migrate buffer in.\n");
	#pragma ACCEL debug fflush(stdout);
	opencl_migrate_buffer(v_cl_mem_vec_add_kernel_i_p, v_cl_mem_vec_add_kernel_i, commands[0], 1);
	#pragma ACCEL debug printf("[Merlin Info] Finish migrate buffer.\n");
{
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "vec_add_kernel");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__vec_add_kernel_kernel, 0, sizeof(opencl_mem), &__vec_add_kernel__a_buffer__migrate);
	opencl_set_kernel_arg(__vec_add_kernel_kernel, 1, sizeof(opencl_mem), &__vec_add_kernel__b_buffer__migrate);
	opencl_set_kernel_arg(__vec_add_kernel_kernel, 2, sizeof(opencl_mem), &__vec_add_kernel__c_buffer__migrate);
	opencl_set_kernel_arg(__vec_add_kernel_kernel, 3, sizeof(int), &inc);
	#pragma ACCEL debug printf("[Merlin Info] Finish set kernel %s argument....\n", "vec_add_kernel");
	#pragma ACCEL debug fflush(stdout);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start execute kernel %s....\n", "vec_add_kernel");
	#pragma ACCEL debug fflush(stdout);
	size_t __gid[1];
	__gid[0] = 1;
	opencl_enqueue_kernel(__vec_add_kernel_kernel, commands[0], 1, __gid, &__event_vec_add_kernel);
}
{
	clWaitForEvents(1, &__event_vec_add_kernel);
	#pragma ACCEL debug printf("[Merlin Info] Finish execute kernel %s....\n", "vec_add_kernel");
	#pragma ACCEL debug fflush(stdout);
}
	#pragma ACCEL debug printf("[Merlin Info] Start migrate buffer out.\n");
	#pragma ACCEL debug fflush(stdout);
	opencl_migrate_buffer(v_cl_mem_vec_add_kernel_o_p, v_cl_mem_vec_add_kernel_o, commands[0], 0);
	#pragma ACCEL debug printf("[Merlin Info] Finish migrate buffer.\n");
	if(__vec_add_kernel__a_buffer__migrate) {
		opencl_release_mem_object(__vec_add_kernel__a_buffer__migrate);
	}
	if(__vec_add_kernel__b_buffer__migrate) {
		opencl_release_mem_object(__vec_add_kernel__b_buffer__migrate);
	}
	if(__vec_add_kernel__c_buffer__migrate) {
		opencl_release_mem_object(__vec_add_kernel__c_buffer__migrate);
	}
  if (c != ((int *)0)) {
    __CMOST_COMMENT__ __vec_add_kernel__c_buffer;
    memcpy((void *)c,(const void *)(__m_c + 0),((unsigned long )10000) * sizeof(int ));
  }
   else {
    printf("Error! Detected null pointer 'c' for external memory.\n");
    exit(1);
  }
}

void __merlin_vec_add_kernel(int a[10000],int b[10000],int c[10000],int inc)
{
  
#pragma ACCEL string __merlin_check_opencl();
  
#pragma ACCEL string __merlin_init_vec_add_kernel();
  __merlinwrapper_vec_add_kernel(a,b,c,inc);
  
#pragma ACCEL string __merlin_release_vec_add_kernel();
}

void __merlin_write_buffer_vec_add_kernel(int a[10000],int b[10000],int c[10000],int inc)
{
  static int __m_a[10000];
  static int __m_b[10000];
  static int __m_c[10000];
{
    if (a != 0) {
      __CMOST_COMMENT__ __vec_add_kernel__a_buffer;
      __CMOST_COMMENT__ __event___vec_add_kernel__a_buffer;
// __MERLIN_EVENT_WRITE__ __event___vec_add_kernel__a_buffer
      int __MERLIN_EVENT_WRITE____event___vec_add_kernel__a_buffer;
      opencl_write_buffer_nb(__vec_add_kernel__a_buffer,commands[0],0 * sizeof(int ),(int *)a,((unsigned long )10000) * sizeof(int ),&__event___vec_add_kernel__a_buffer);
    }
     else {
      printf("Error! Detected null pointer 'a' for external memory.\n");
      exit(1);
    }
  }
{
    if (b != 0) {
      __CMOST_COMMENT__ __vec_add_kernel__b_buffer;
      __CMOST_COMMENT__ __event___vec_add_kernel__b_buffer;
// __MERLIN_EVENT_WRITE__ __event___vec_add_kernel__b_buffer
      int __MERLIN_EVENT_WRITE____event___vec_add_kernel__b_buffer;
      opencl_write_buffer_nb(__vec_add_kernel__b_buffer,commands[0],0 * sizeof(int ),(int *)b,((unsigned long )10000) * sizeof(int ),&__event___vec_add_kernel__b_buffer);
    }
     else {
      printf("Error! Detected null pointer 'b' for external memory.\n");
      exit(1);
    }
  }
}

void __merlin_read_buffer_vec_add_kernel(int a[10000],int b[10000],int c[10000],int inc)
{
  static int __m_a[10000];
  static int __m_b[10000];
  static int __m_c[10000];
{
    if (c != 0) {
      __CMOST_COMMENT__ __vec_add_kernel__c_buffer;
      __CMOST_COMMENT__ __event___vec_add_kernel__c_buffer;
// __MERLIN_EVENT_READ__ __event___vec_add_kernel__c_buffer
      int __MERLIN_EVENT_READ____event___vec_add_kernel__c_buffer;
      opencl_read_buffer_nb(__vec_add_kernel__c_buffer,commands[0],0 * sizeof(int ),(int *)c,((unsigned long )10000) * sizeof(int ),&__event___vec_add_kernel__c_buffer);
    }
     else {
      printf("Error! Detected null pointer 'c' for external memory.\n");
      exit(1);
    }
  }
}

void __merlin_execute_vec_add_kernel(int a[10000],int b[10000],int c[10000],int inc)
{
  int i;
{
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "vec_add_kernel");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__vec_add_kernel_kernel, 0, sizeof(opencl_mem), &__vec_add_kernel__a_buffer);
	opencl_set_kernel_arg(__vec_add_kernel_kernel, 1, sizeof(opencl_mem), &__vec_add_kernel__b_buffer);
	opencl_set_kernel_arg(__vec_add_kernel_kernel, 2, sizeof(opencl_mem), &__vec_add_kernel__c_buffer);
	opencl_set_kernel_arg(__vec_add_kernel_kernel, 3, sizeof(int), &inc);
	#pragma ACCEL debug printf("[Merlin Info] Finish set kernel %s argument....\n", "vec_add_kernel");
	#pragma ACCEL debug fflush(stdout);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start execute kernel %s....\n", "vec_add_kernel");
	#pragma ACCEL debug fflush(stdout);
	size_t __gid[1];
	__gid[0] = 1;
	opencl_enqueue_kernel(__vec_add_kernel_kernel, commands[0], 1, __gid, &__event_vec_add_kernel);
}
}
