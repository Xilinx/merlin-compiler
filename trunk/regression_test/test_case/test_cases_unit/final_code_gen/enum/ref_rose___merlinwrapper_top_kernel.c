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
#include "merlin_type_define.h"
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_6();
void top_kernel(enum vec_add_flag flag_a,enum vec_add_flag *arr,int merlin_return_value[1]);
static void __merlin_dummy_extern_int_merlin_include_L_();
#include "__merlinhead_kernel_top.h"
static void __merlin_dummy_extern_int_merlin_include_L_();

int __merlinwrapper_top_kernel(enum vec_add_flag flag_a,enum vec_add_flag *arr)
{
  __CMOST_COMMENT__ commands;
  int merlin_return_value[1];
{
    
#pragma ACCEL wrapper variable=flag_a port=flag_a
    
#pragma ACCEL wrapper variable=arr port=arr depth=100 max_depth=100 data_type=int io=RO copy=true
    
#pragma ACCEL wrapper variable=merlin_return_value port=merlin_return_value depth=1 max_depth=1 data_type=int io=WO copy=true scope_type=return
  }
  static __CMOST_COMMENT__ __m_arr;
  static __CMOST_COMMENT__ __m_merlin_return_value;
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "arr", "100 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (arr != 0) {
      __CMOST_COMMENT__ __top_kernel__arr_buffer;
      opencl_write_buffer(__top_kernel__arr_buffer,commands[0],0 * sizeof(int ),(int *)arr,((unsigned long )100) * sizeof(int ));
    }
     else {
      printf("Error! The external memory pointed by 'arr' is invalid.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "arr");
    
#pragma ACCEL debug fflush(stdout);
  }
  
#pragma ACCEL kernel
{
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "top_kernel");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__top_kernel_kernel, 0, sizeof(enum vec_add_flag), &flag_a);
	opencl_set_kernel_arg(__top_kernel_kernel, 1, sizeof(opencl_mem), &__top_kernel__arr_buffer);
	opencl_set_kernel_arg(__top_kernel_kernel, 2, sizeof(opencl_mem), &__top_kernel__merlin_return_value_buffer);
	#pragma ACCEL debug printf("[Merlin Info] Finish set kernel %s argument....\n", "top_kernel");
	#pragma ACCEL debug fflush(stdout);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start execute kernel %s....\n", "top_kernel");
	#pragma ACCEL debug fflush(stdout);
	size_t __gid[1];
	__gid[0] = 1;
	opencl_enqueue_kernel(__top_kernel_kernel, commands[0], 1, __gid, &__event_top_kernel);
}
{
	clWaitForEvents(1, &__event_top_kernel);
	if(__event_top_kernel) {
		opencl_release_event(__event_top_kernel);
	}
	#pragma ACCEL debug printf("[Merlin Info] Finish execute kernel %s....\n", "top_kernel");
	#pragma ACCEL debug fflush(stdout);
}
{
    int merlin_return_value[1];
/*
    static int __m_arr[100];
    static int __m_merlin_return_value[1];
*/
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "merlin_return_value", "1 * sizeof(int )");
      
#pragma ACCEL debug fflush(stdout);
      if (merlin_return_value != 0) {
        __CMOST_COMMENT__ __top_kernel__merlin_return_value_buffer;
        opencl_read_buffer(__top_kernel__merlin_return_value_buffer,commands[0],0 * sizeof(int ),(int *)merlin_return_value,((unsigned long )1) * sizeof(int ));
      }
       else {
        printf("Error! The external memory pointed by 'merlin_return_value' is invalid.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "merlin_return_value");
      
#pragma ACCEL debug fflush(stdout);
    }
  }
  return  *merlin_return_value;
}

int __merlin_top_kernel(enum vec_add_flag flag_a,enum vec_add_flag *arr)
{
  
#pragma ACCEL string __merlin_check_opencl();
  
#pragma ACCEL string __merlin_init_top_kernel();
  __merlinwrapper_top_kernel(flag_a,arr);
  
#pragma ACCEL string __merlin_release_top_kernel();
}

int __merlin_write_buffer_top_kernel(enum vec_add_flag flag_a,enum vec_add_flag *arr)
{
  static int __m_arr[100];
  static int __m_merlin_return_value[1];
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "arr", "100 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (arr != 0) {
      __CMOST_COMMENT__ __top_kernel__arr_buffer;
      __CMOST_COMMENT__ __event___top_kernel__arr_buffer;
/* __MERLIN_EVENT_WRITE__ __event___top_kernel__arr_buffer */
      int __MERLIN_EVENT_WRITE____event___top_kernel__arr_buffer;
      opencl_write_buffer_nb(__top_kernel__arr_buffer,commands[0],0 * sizeof(int ),(int *)arr,((unsigned long )100) * sizeof(int ),&__event___top_kernel__arr_buffer);
    }
     else {
      printf("Error! The external memory pointed by 'arr' is invalid.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "arr");
    
#pragma ACCEL debug fflush(stdout);
  }
}

int __merlin_read_buffer_top_kernel(enum vec_add_flag flag_a,enum vec_add_flag *arr)
{
  int merlin_return_value[1];
  static int __m_arr[100];
  static int __m_merlin_return_value[1];
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "merlin_return_value", "1 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (merlin_return_value != 0) {
      __CMOST_COMMENT__ __top_kernel__merlin_return_value_buffer;
      __CMOST_COMMENT__ __event___top_kernel__merlin_return_value_buffer;
/* __MERLIN_EVENT_READ__ __event___top_kernel__merlin_return_value_buffer */
      int __MERLIN_EVENT_READ____event___top_kernel__merlin_return_value_buffer;
      opencl_read_buffer_nb(__top_kernel__merlin_return_value_buffer,commands[0],0 * sizeof(int ),(int *)merlin_return_value,((unsigned long )1) * sizeof(int ),&__event___top_kernel__merlin_return_value_buffer);
    }
     else {
      printf("Error! The external memory pointed by 'merlin_return_value' is invalid.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "merlin_return_value");
    
#pragma ACCEL debug fflush(stdout);
  }
  return  *merlin_return_value;
}

int __merlin_execute_top_kernel(enum vec_add_flag flag_a,enum vec_add_flag *arr)
{
  int i;
{
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "top_kernel");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__top_kernel_kernel, 0, sizeof(enum vec_add_flag), &flag_a);
	opencl_set_kernel_arg(__top_kernel_kernel, 1, sizeof(opencl_mem), &__top_kernel__arr_buffer);
	opencl_set_kernel_arg(__top_kernel_kernel, 2, sizeof(opencl_mem), &__top_kernel__merlin_return_value_buffer);
	#pragma ACCEL debug printf("[Merlin Info] Finish set kernel %s argument....\n", "top_kernel");
	#pragma ACCEL debug fflush(stdout);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start execute kernel %s....\n", "top_kernel");
	#pragma ACCEL debug fflush(stdout);
	size_t __gid[1];
	__gid[0] = 1;
	opencl_enqueue_kernel(__top_kernel_kernel, commands[0], 1, __gid, &__event_top_kernel);
}
}
