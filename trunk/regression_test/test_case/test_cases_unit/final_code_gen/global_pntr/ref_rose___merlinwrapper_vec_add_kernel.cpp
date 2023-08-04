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
#include <string.h> 
#include <stdio.h> 
#include <stdlib.h> 
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_431();
void vec_add_kernel(int a[10240],int inc);
static void __merlin_dummy_extern_int_merlin_include_L_();
#include "__merlinhead_kernel_top.h"
static void __merlin_dummy_extern_int_merlin_include_L_();

void __merlinwrapper_vec_add_kernel(int a[10240],int b[10240],int c[10240],int inc,char *str)
{
  __CMOST_COMMENT__ commands;
{
    
#pragma ACCEL wrapper variable=a port=a depth=10240 max_depth=10240 data_type=int io=RW copy=false
    
#pragma ACCEL wrapper variable=inc port=inc
  }
  static __CMOST_COMMENT__ __m_a;
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "a", "10240 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (a != 0) {
      __CMOST_COMMENT__ __vec_add_kernel__a_buffer;
      opencl_write_buffer(__vec_add_kernel__a_buffer,commands[0],0 * sizeof(int ),(int *)a,((unsigned long )10240) * sizeof(int ));
    }
     else {
      printf("Error! Detected null pointer 'a' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "a");
    
#pragma ACCEL debug fflush(stdout);
  }
  
#pragma ACCEL kernel
{
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "vec_add_kernel");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__vec_add_kernel_kernel, 0, sizeof(opencl_mem), &__vec_add_kernel__a_buffer);
	opencl_set_kernel_arg(__vec_add_kernel_kernel, 1, sizeof(int), &inc);
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
	if(__event_vec_add_kernel) {
		opencl_release_event(__event_vec_add_kernel);
	}
	#pragma ACCEL debug printf("[Merlin Info] Finish execute kernel %s....\n", "vec_add_kernel");
	#pragma ACCEL debug fflush(stdout);
}
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "a", "10240 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (a != 0) {
      __CMOST_COMMENT__ __vec_add_kernel__a_buffer;
      opencl_read_buffer(__vec_add_kernel__a_buffer,commands[0],0 * sizeof(int ),(int *)a,((unsigned long )10240) * sizeof(int ));
    }
     else {
      printf("Error! Detected null pointer 'a' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "a");
    
#pragma ACCEL debug fflush(stdout);
  }
}

void __merlin_vec_add_kernel(int a[10240],int b[10240],int c[10240],int inc,char *str)
{
  
#pragma ACCEL string __merlin_check_opencl();
  
#pragma ACCEL string __merlin_init_vec_add_kernel();
  __merlinwrapper_vec_add_kernel(a,b,c,inc,str);
  
#pragma ACCEL string __merlin_release_vec_add_kernel();
}

void __merlin_write_buffer_vec_add_kernel(int a[10240],int b[10240],int c[10240],int inc,char *str)
{
  static int __m_a[10240];
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "a", "10240 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (a != 0) {
      __CMOST_COMMENT__ __vec_add_kernel__a_buffer;
      __CMOST_COMMENT__ __event___vec_add_kernel__a_buffer;
// __MERLIN_EVENT_WRITE__ __event___vec_add_kernel__a_buffer
      int __MERLIN_EVENT_WRITE____event___vec_add_kernel__a_buffer;
      opencl_write_buffer_nb(__vec_add_kernel__a_buffer,commands[0],0 * sizeof(int ),(int *)a,((unsigned long )10240) * sizeof(int ),&__event___vec_add_kernel__a_buffer);
    }
     else {
      printf("Error! Detected null pointer 'a' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "a");
    
#pragma ACCEL debug fflush(stdout);
  }
}

void __merlin_read_buffer_vec_add_kernel(int a[10240],int b[10240],int c[10240],int inc,char *str)
{
  static int __m_a[10240];
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "a", "10240 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (a != 0) {
      __CMOST_COMMENT__ __vec_add_kernel__a_buffer;
      __CMOST_COMMENT__ __event___vec_add_kernel__a_buffer;
// __MERLIN_EVENT_READ__ __event___vec_add_kernel__a_buffer
      int __MERLIN_EVENT_READ____event___vec_add_kernel__a_buffer;
      opencl_read_buffer_nb(__vec_add_kernel__a_buffer,commands[0],0 * sizeof(int ),(int *)a,((unsigned long )10240) * sizeof(int ),&__event___vec_add_kernel__a_buffer);
    }
     else {
      printf("Error! Detected null pointer 'a' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "a");
    
#pragma ACCEL debug fflush(stdout);
  }
}

void __merlin_execute_vec_add_kernel(int a[10240],int b[10240],int c[10240],int inc,char *str)
{
  int i;
{
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "vec_add_kernel");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__vec_add_kernel_kernel, 0, sizeof(opencl_mem), &__vec_add_kernel__a_buffer);
	opencl_set_kernel_arg(__vec_add_kernel_kernel, 1, sizeof(int), &inc);
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
