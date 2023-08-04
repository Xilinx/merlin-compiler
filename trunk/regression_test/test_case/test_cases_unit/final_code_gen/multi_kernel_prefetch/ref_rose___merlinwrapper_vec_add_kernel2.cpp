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
static void __merlin_dummy_431();
void vec_add_kernel2(int *d,int *b,int *c,int inc);
static void __merlin_dummy_extern_int_merlin_include_L_();
#include "__merlinhead_kernel_top.h"
static void __merlin_dummy_extern_int_merlin_include_L_();
void msm_port_d_vec_add_kernel2_0(int *d);
void msm_port_b_vec_add_kernel2_0(int *b);
void msm_port_c_vec_add_kernel2_0(int *c);

void __merlinwrapper_vec_add_kernel2(int *d,int *b,int *c,int inc)
{
  __CMOST_COMMENT__ commands;
{
    
#pragma ACCEL wrapper variable=d port=d depth=10000 max_depth=10000 data_type=int io=RO copy=false
    
#pragma ACCEL wrapper variable=b port=b depth=10000 max_depth=10000 data_type=int io=RO copy=false
    
#pragma ACCEL wrapper variable=c port=c depth=10000 max_depth=10000 data_type=int io=WO copy=false
    
#pragma ACCEL wrapper variable=inc port=inc
  }
  static __CMOST_COMMENT__ __m_d;
  static __CMOST_COMMENT__ __m_b;
  static __CMOST_COMMENT__ __m_c;
{
/*
    static int __m_d[10000];
    static int __m_b[10000];
    static int __m_c[10000];
*/
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "d", "10000 * sizeof(int )");
      
#pragma ACCEL debug fflush(stdout);
      if (d != 0) {
        __CMOST_COMMENT__ __vec_add_kernel2__d_buffer;
        opencl_write_buffer(__vec_add_kernel2__d_buffer,commands[0],0 * sizeof(int ),(int *)d,((unsigned long )10000) * sizeof(int ));
      }
       else {
        printf("Error! Detected null pointer 'd' for external memory.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "d");
      
#pragma ACCEL debug fflush(stdout);
    }
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "b", "10000 * sizeof(int )");
      
#pragma ACCEL debug fflush(stdout);
      if (b != 0) {
        __CMOST_COMMENT__ __vec_add_kernel2__b_buffer;
        opencl_write_buffer(__vec_add_kernel2__b_buffer,commands[0],0 * sizeof(int ),(int *)b,((unsigned long )10000) * sizeof(int ));
      }
       else {
        printf("Error! Detected null pointer 'b' for external memory.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "b");
      
#pragma ACCEL debug fflush(stdout);
    }
  }
  
#pragma ACCEL sub_kernel parent=vec_add_kernel2
  
#pragma ACCEL sub_kernel parent=vec_add_kernel2
{
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "vec_add_kernel2");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__vec_add_kernel2_kernel, 0, sizeof(opencl_mem), &__vec_add_kernel2__d_buffer);
	opencl_set_kernel_arg(__vec_add_kernel2_kernel, 1, sizeof(opencl_mem), &__vec_add_kernel2__b_buffer);
	opencl_set_kernel_arg(__vec_add_kernel2_kernel, 2, sizeof(opencl_mem), &__vec_add_kernel2__c_buffer);
	opencl_set_kernel_arg(__vec_add_kernel2_kernel, 3, sizeof(int), &inc);
	#pragma ACCEL debug printf("[Merlin Info] Finish set kernel %s argument....\n", "vec_add_kernel2");
	#pragma ACCEL debug fflush(stdout);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "msm_port_d_vec_add_kernel2_0");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__msm_port_d_vec_add_kernel2_0_kernel, 0, sizeof(opencl_mem), &__vec_add_kernel2__d_buffer);
	#pragma ACCEL debug printf("[Merlin Info] Finish set kernel %s argument....\n", "msm_port_d_vec_add_kernel2_0");
	#pragma ACCEL debug fflush(stdout);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "msm_port_b_vec_add_kernel2_0");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__msm_port_b_vec_add_kernel2_0_kernel, 0, sizeof(opencl_mem), &__vec_add_kernel2__b_buffer);
	#pragma ACCEL debug printf("[Merlin Info] Finish set kernel %s argument....\n", "msm_port_b_vec_add_kernel2_0");
	#pragma ACCEL debug fflush(stdout);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "msm_port_c_vec_add_kernel2_0");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__msm_port_c_vec_add_kernel2_0_kernel, 0, sizeof(opencl_mem), &__vec_add_kernel2__c_buffer);
	#pragma ACCEL debug printf("[Merlin Info] Finish set kernel %s argument....\n", "msm_port_c_vec_add_kernel2_0");
	#pragma ACCEL debug fflush(stdout);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start execute kernel %s....\n", "vec_add_kernel2");
	#pragma ACCEL debug fflush(stdout);
	size_t __gid[1];
	__gid[0] = 1;
	opencl_enqueue_kernel(__vec_add_kernel2_kernel, commands[0], 1, __gid, &__event_vec_add_kernel2);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start execute kernel %s....\n", "msm_port_d_vec_add_kernel2_0");
	#pragma ACCEL debug fflush(stdout);
	size_t __gid[1];
	__gid[0] = 1;
	opencl_enqueue_kernel(__msm_port_d_vec_add_kernel2_0_kernel, commands[1], 1, __gid, &__event_msm_port_d_vec_add_kernel2_0);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start execute kernel %s....\n", "msm_port_b_vec_add_kernel2_0");
	#pragma ACCEL debug fflush(stdout);
	size_t __gid[1];
	__gid[0] = 1;
	opencl_enqueue_kernel(__msm_port_b_vec_add_kernel2_0_kernel, commands[2], 1, __gid, &__event_msm_port_b_vec_add_kernel2_0);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start execute kernel %s....\n", "msm_port_c_vec_add_kernel2_0");
	#pragma ACCEL debug fflush(stdout);
	size_t __gid[1];
	__gid[0] = 1;
	opencl_enqueue_kernel(__msm_port_c_vec_add_kernel2_0_kernel, commands[3], 1, __gid, &__event_msm_port_c_vec_add_kernel2_0);
}
{
	clWaitForEvents(1, &__event_vec_add_kernel2);
	if(__event_vec_add_kernel2) {
		opencl_release_event(__event_vec_add_kernel2);
	}
	#pragma ACCEL debug printf("[Merlin Info] Finish execute kernel %s....\n", "vec_add_kernel2");
	#pragma ACCEL debug fflush(stdout);
}
{
	clWaitForEvents(1, &__event_msm_port_d_vec_add_kernel2_0);
	if(__event_msm_port_d_vec_add_kernel2_0) {
		opencl_release_event(__event_msm_port_d_vec_add_kernel2_0);
	}
	#pragma ACCEL debug printf("[Merlin Info] Finish execute kernel %s....\n", "msm_port_d_vec_add_kernel2_0");
	#pragma ACCEL debug fflush(stdout);
}
{
	clWaitForEvents(1, &__event_msm_port_b_vec_add_kernel2_0);
	if(__event_msm_port_b_vec_add_kernel2_0) {
		opencl_release_event(__event_msm_port_b_vec_add_kernel2_0);
	}
	#pragma ACCEL debug printf("[Merlin Info] Finish execute kernel %s....\n", "msm_port_b_vec_add_kernel2_0");
	#pragma ACCEL debug fflush(stdout);
}
{
	clWaitForEvents(1, &__event_msm_port_c_vec_add_kernel2_0);
	if(__event_msm_port_c_vec_add_kernel2_0) {
		opencl_release_event(__event_msm_port_c_vec_add_kernel2_0);
	}
	#pragma ACCEL debug printf("[Merlin Info] Finish execute kernel %s....\n", "msm_port_c_vec_add_kernel2_0");
	#pragma ACCEL debug fflush(stdout);
}
  
#pragma ACCEL sub_kernel parent=vec_add_kernel2
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "c", "10000 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (c != 0) {
      __CMOST_COMMENT__ __vec_add_kernel2__c_buffer;
      opencl_read_buffer(__vec_add_kernel2__c_buffer,commands[0],0 * sizeof(int ),(int *)c,((unsigned long )10000) * sizeof(int ));
    }
     else {
      printf("Error! Detected null pointer 'c' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "c");
    
#pragma ACCEL debug fflush(stdout);
  }
}

void __merlin_vec_add_kernel2(int *d,int *b,int *c,int inc)
{
  
#pragma ACCEL string __merlin_check_opencl();
  
#pragma ACCEL string __merlin_init_vec_add_kernel2();
  __merlinwrapper_vec_add_kernel2(d,b,c,inc);
  
#pragma ACCEL string __merlin_release_vec_add_kernel2();
}

void __merlin_write_buffer_vec_add_kernel2(int *d,int *b,int *c,int inc)
{
  static int __m_d[10000];
  static int __m_b[10000];
  static int __m_c[10000];
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "d", "10000 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (d != 0) {
      __CMOST_COMMENT__ __vec_add_kernel2__d_buffer;
      __CMOST_COMMENT__ __event___vec_add_kernel2__d_buffer;
// __MERLIN_EVENT_WRITE__ __event___vec_add_kernel2__d_buffer
      int __MERLIN_EVENT_WRITE____event___vec_add_kernel2__d_buffer;
      opencl_write_buffer_nb(__vec_add_kernel2__d_buffer,commands[0],0 * sizeof(int ),(int *)d,((unsigned long )10000) * sizeof(int ),&__event___vec_add_kernel2__d_buffer);
    }
     else {
      printf("Error! Detected null pointer 'd' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "d");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "b", "10000 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (b != 0) {
      __CMOST_COMMENT__ __vec_add_kernel2__b_buffer;
      __CMOST_COMMENT__ __event___vec_add_kernel2__b_buffer;
// __MERLIN_EVENT_WRITE__ __event___vec_add_kernel2__b_buffer
      int __MERLIN_EVENT_WRITE____event___vec_add_kernel2__b_buffer;
      opencl_write_buffer_nb(__vec_add_kernel2__b_buffer,commands[0],0 * sizeof(int ),(int *)b,((unsigned long )10000) * sizeof(int ),&__event___vec_add_kernel2__b_buffer);
    }
     else {
      printf("Error! Detected null pointer 'b' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "b");
    
#pragma ACCEL debug fflush(stdout);
  }
}

void __merlin_read_buffer_vec_add_kernel2(int *d,int *b,int *c,int inc)
{
  static int __m_d[10000];
  static int __m_b[10000];
  static int __m_c[10000];
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "c", "10000 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (c != 0) {
      __CMOST_COMMENT__ __vec_add_kernel2__c_buffer;
      __CMOST_COMMENT__ __event___vec_add_kernel2__c_buffer;
// __MERLIN_EVENT_READ__ __event___vec_add_kernel2__c_buffer
      int __MERLIN_EVENT_READ____event___vec_add_kernel2__c_buffer;
      opencl_read_buffer_nb(__vec_add_kernel2__c_buffer,commands[0],0 * sizeof(int ),(int *)c,((unsigned long )10000) * sizeof(int ),&__event___vec_add_kernel2__c_buffer);
    }
     else {
      printf("Error! Detected null pointer 'c' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "c");
    
#pragma ACCEL debug fflush(stdout);
  }
}

void __merlin_execute_vec_add_kernel2(int *d,int *b,int *c,int inc)
{
  int i;
{
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "vec_add_kernel2");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__vec_add_kernel2_kernel, 0, sizeof(opencl_mem), &__vec_add_kernel2__d_buffer);
	opencl_set_kernel_arg(__vec_add_kernel2_kernel, 1, sizeof(opencl_mem), &__vec_add_kernel2__b_buffer);
	opencl_set_kernel_arg(__vec_add_kernel2_kernel, 2, sizeof(opencl_mem), &__vec_add_kernel2__c_buffer);
	opencl_set_kernel_arg(__vec_add_kernel2_kernel, 3, sizeof(int), &inc);
	#pragma ACCEL debug printf("[Merlin Info] Finish set kernel %s argument....\n", "vec_add_kernel2");
	#pragma ACCEL debug fflush(stdout);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "msm_port_d_vec_add_kernel2_0");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__msm_port_d_vec_add_kernel2_0_kernel, 0, sizeof(opencl_mem), &__vec_add_kernel2__d_buffer);
	#pragma ACCEL debug printf("[Merlin Info] Finish set kernel %s argument....\n", "msm_port_d_vec_add_kernel2_0");
	#pragma ACCEL debug fflush(stdout);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "msm_port_b_vec_add_kernel2_0");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__msm_port_b_vec_add_kernel2_0_kernel, 0, sizeof(opencl_mem), &__vec_add_kernel2__b_buffer);
	#pragma ACCEL debug printf("[Merlin Info] Finish set kernel %s argument....\n", "msm_port_b_vec_add_kernel2_0");
	#pragma ACCEL debug fflush(stdout);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "msm_port_c_vec_add_kernel2_0");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__msm_port_c_vec_add_kernel2_0_kernel, 0, sizeof(opencl_mem), &__vec_add_kernel2__c_buffer);
	#pragma ACCEL debug printf("[Merlin Info] Finish set kernel %s argument....\n", "msm_port_c_vec_add_kernel2_0");
	#pragma ACCEL debug fflush(stdout);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start execute kernel %s....\n", "vec_add_kernel2");
	#pragma ACCEL debug fflush(stdout);
	size_t __gid[1];
	__gid[0] = 1;
	opencl_enqueue_kernel(__vec_add_kernel2_kernel, commands[0], 1, __gid, &__event_vec_add_kernel2);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start execute kernel %s....\n", "msm_port_d_vec_add_kernel2_0");
	#pragma ACCEL debug fflush(stdout);
	size_t __gid[1];
	__gid[0] = 1;
	opencl_enqueue_kernel(__msm_port_d_vec_add_kernel2_0_kernel, commands[1], 1, __gid, &__event_msm_port_d_vec_add_kernel2_0);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start execute kernel %s....\n", "msm_port_b_vec_add_kernel2_0");
	#pragma ACCEL debug fflush(stdout);
	size_t __gid[1];
	__gid[0] = 1;
	opencl_enqueue_kernel(__msm_port_b_vec_add_kernel2_0_kernel, commands[2], 1, __gid, &__event_msm_port_b_vec_add_kernel2_0);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start execute kernel %s....\n", "msm_port_c_vec_add_kernel2_0");
	#pragma ACCEL debug fflush(stdout);
	size_t __gid[1];
	__gid[0] = 1;
	opencl_enqueue_kernel(__msm_port_c_vec_add_kernel2_0_kernel, commands[3], 1, __gid, &__event_msm_port_c_vec_add_kernel2_0);
}
}
