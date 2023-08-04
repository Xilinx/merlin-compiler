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
#include <merlin_stream.h>
#include "merlin_type_define.h"
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_439();
void vec_add_kernel(int a[1048576],int b[1048576],int c[1048576],int inc);
static void __merlin_dummy_extern_int_merlin_include_L_();
#include "__merlinhead_kernel_top.h"
static void __merlin_dummy_extern_int_merlin_include_L_();
static void prefetch_c(int c[1048576]);
static void prefetch_b(int b[1048576]);
static void prefetch_a(int a[1048576]);

void __merlinwrapper_vec_add_kernel(int a[1048576],int b[1048576],int c[1048576],int inc)
{
  __CMOST_COMMENT__ commands;
{
    
#pragma ACCEL wrapper variable=a port=a depth=1048576 max_depth=1048576 data_type=int io=RW copy=false
    
#pragma ACCEL wrapper variable=b port=b depth=1048576 max_depth=1048576 data_type=int io=RW copy=false
    
#pragma ACCEL wrapper variable=c port=c depth=1048576 max_depth=1048576 data_type=int io=RW copy=false
    
#pragma ACCEL wrapper variable=inc port=inc
  }
  static __CMOST_COMMENT__ __m_a;
  static __CMOST_COMMENT__ __m_b;
  static __CMOST_COMMENT__ __m_c;
{
/*
    static int __m_a[1048576];
    static int __m_b[1048576];
    static int __m_c[1048576];
*/
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "a", "1048576 * sizeof(int )");
      
#pragma ACCEL debug fflush(stdout);
      if (a != 0) {
        __CMOST_COMMENT__ __vec_add_kernel__a_buffer;
        opencl_write_buffer(__vec_add_kernel__a_buffer,commands[0],0 * sizeof(int ),(int *)a,((unsigned long )1048576) * sizeof(int ));
      }
       else {
        printf("Error! Detected null pointer 'a' for external memory.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "a");
      
#pragma ACCEL debug fflush(stdout);
    }
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "b", "1048576 * sizeof(int )");
      
#pragma ACCEL debug fflush(stdout);
      if (b != 0) {
        __CMOST_COMMENT__ __vec_add_kernel__b_buffer;
        opencl_write_buffer(__vec_add_kernel__b_buffer,commands[0],0 * sizeof(int ),(int *)b,((unsigned long )1048576) * sizeof(int ));
      }
       else {
        printf("Error! Detected null pointer 'b' for external memory.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "b");
      
#pragma ACCEL debug fflush(stdout);
    }
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "c", "1048576 * sizeof(int )");
      
#pragma ACCEL debug fflush(stdout);
      if (c != 0) {
        __CMOST_COMMENT__ __vec_add_kernel__c_buffer;
        opencl_write_buffer(__vec_add_kernel__c_buffer,commands[0],0 * sizeof(int ),(int *)c,((unsigned long )1048576) * sizeof(int ));
      }
       else {
        printf("Error! Detected null pointer 'c' for external memory.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "c");
      
#pragma ACCEL debug fflush(stdout);
    }
  }
  
#pragma ACCEL kernel
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
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "prefetch_c");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__prefetch_c_kernel, 0, sizeof(opencl_mem), &__vec_add_kernel__c_buffer);
	#pragma ACCEL debug printf("[Merlin Info] Finish set kernel %s argument....\n", "prefetch_c");
	#pragma ACCEL debug fflush(stdout);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "prefetch_b");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__prefetch_b_kernel, 0, sizeof(opencl_mem), &__vec_add_kernel__b_buffer);
	#pragma ACCEL debug printf("[Merlin Info] Finish set kernel %s argument....\n", "prefetch_b");
	#pragma ACCEL debug fflush(stdout);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "prefetch_a");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__prefetch_a_kernel, 0, sizeof(opencl_mem), &__vec_add_kernel__a_buffer);
	#pragma ACCEL debug printf("[Merlin Info] Finish set kernel %s argument....\n", "prefetch_a");
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
	#pragma ACCEL debug printf("[Merlin Info] Start execute kernel %s....\n", "prefetch_c");
	#pragma ACCEL debug fflush(stdout);
	size_t __gid[1];
	__gid[0] = 1;
	opencl_enqueue_kernel(__prefetch_c_kernel, commands[1], 1, __gid, &__event_prefetch_c);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start execute kernel %s....\n", "prefetch_b");
	#pragma ACCEL debug fflush(stdout);
	size_t __gid[1];
	__gid[0] = 1;
	opencl_enqueue_kernel(__prefetch_b_kernel, commands[2], 1, __gid, &__event_prefetch_b);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start execute kernel %s....\n", "prefetch_a");
	#pragma ACCEL debug fflush(stdout);
	size_t __gid[1];
	__gid[0] = 1;
	opencl_enqueue_kernel(__prefetch_a_kernel, commands[3], 1, __gid, &__event_prefetch_a);
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
	clWaitForEvents(1, &__event_prefetch_c);
	if(__event_prefetch_c) {
		opencl_release_event(__event_prefetch_c);
	}
	#pragma ACCEL debug printf("[Merlin Info] Finish execute kernel %s....\n", "prefetch_c");
	#pragma ACCEL debug fflush(stdout);
}
{
	clWaitForEvents(1, &__event_prefetch_b);
	if(__event_prefetch_b) {
		opencl_release_event(__event_prefetch_b);
	}
	#pragma ACCEL debug printf("[Merlin Info] Finish execute kernel %s....\n", "prefetch_b");
	#pragma ACCEL debug fflush(stdout);
}
{
	clWaitForEvents(1, &__event_prefetch_a);
	if(__event_prefetch_a) {
		opencl_release_event(__event_prefetch_a);
	}
	#pragma ACCEL debug printf("[Merlin Info] Finish execute kernel %s....\n", "prefetch_a");
	#pragma ACCEL debug fflush(stdout);
}
{
/*
    static int __m_a[1048576];
    static int __m_b[1048576];
    static int __m_c[1048576];
*/
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "a", "1048576 * sizeof(int )");
      
#pragma ACCEL debug fflush(stdout);
      if (a != 0) {
        __CMOST_COMMENT__ __vec_add_kernel__a_buffer;
        opencl_read_buffer(__vec_add_kernel__a_buffer,commands[0],0 * sizeof(int ),(int *)a,((unsigned long )1048576) * sizeof(int ));
      }
       else {
        printf("Error! Detected null pointer 'a' for external memory.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "a");
      
#pragma ACCEL debug fflush(stdout);
    }
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "b", "1048576 * sizeof(int )");
      
#pragma ACCEL debug fflush(stdout);
      if (b != 0) {
        __CMOST_COMMENT__ __vec_add_kernel__b_buffer;
        opencl_read_buffer(__vec_add_kernel__b_buffer,commands[0],0 * sizeof(int ),(int *)b,((unsigned long )1048576) * sizeof(int ));
      }
       else {
        printf("Error! Detected null pointer 'b' for external memory.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "b");
      
#pragma ACCEL debug fflush(stdout);
    }
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "c", "1048576 * sizeof(int )");
      
#pragma ACCEL debug fflush(stdout);
      if (c != 0) {
        __CMOST_COMMENT__ __vec_add_kernel__c_buffer;
        opencl_read_buffer(__vec_add_kernel__c_buffer,commands[0],0 * sizeof(int ),(int *)c,((unsigned long )1048576) * sizeof(int ));
      }
       else {
        printf("Error! Detected null pointer 'c' for external memory.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "c");
      
#pragma ACCEL debug fflush(stdout);
    }
  }
}

void __merlin_vec_add_kernel(int a[1048576],int b[1048576],int c[1048576],int inc)
{
  
#pragma ACCEL string __merlin_check_opencl();
  
#pragma ACCEL string __merlin_init_vec_add_kernel();
  __merlinwrapper_vec_add_kernel(a,b,c,inc);
  
#pragma ACCEL string __merlin_release_vec_add_kernel();
}

void __merlin_write_buffer_vec_add_kernel(int a[1048576],int b[1048576],int c[1048576],int inc)
{
  static int __m_a[1048576];
  static int __m_b[1048576];
  static int __m_c[1048576];
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "a", "1048576 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (a != 0) {
      __CMOST_COMMENT__ __vec_add_kernel__a_buffer;
      __CMOST_COMMENT__ __event___vec_add_kernel__a_buffer;
// __MERLIN_EVENT_WRITE__ __event___vec_add_kernel__a_buffer
      int __MERLIN_EVENT_WRITE____event___vec_add_kernel__a_buffer;
      opencl_write_buffer_nb(__vec_add_kernel__a_buffer,commands[0],0 * sizeof(int ),(int *)a,((unsigned long )1048576) * sizeof(int ),&__event___vec_add_kernel__a_buffer);
    }
     else {
      printf("Error! Detected null pointer 'a' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "a");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "b", "1048576 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (b != 0) {
      __CMOST_COMMENT__ __vec_add_kernel__b_buffer;
      __CMOST_COMMENT__ __event___vec_add_kernel__b_buffer;
// __MERLIN_EVENT_WRITE__ __event___vec_add_kernel__b_buffer
      int __MERLIN_EVENT_WRITE____event___vec_add_kernel__b_buffer;
      opencl_write_buffer_nb(__vec_add_kernel__b_buffer,commands[0],0 * sizeof(int ),(int *)b,((unsigned long )1048576) * sizeof(int ),&__event___vec_add_kernel__b_buffer);
    }
     else {
      printf("Error! Detected null pointer 'b' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "b");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "c", "1048576 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (c != 0) {
      __CMOST_COMMENT__ __vec_add_kernel__c_buffer;
      __CMOST_COMMENT__ __event___vec_add_kernel__c_buffer;
// __MERLIN_EVENT_WRITE__ __event___vec_add_kernel__c_buffer
      int __MERLIN_EVENT_WRITE____event___vec_add_kernel__c_buffer;
      opencl_write_buffer_nb(__vec_add_kernel__c_buffer,commands[0],0 * sizeof(int ),(int *)c,((unsigned long )1048576) * sizeof(int ),&__event___vec_add_kernel__c_buffer);
    }
     else {
      printf("Error! Detected null pointer 'c' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "c");
    
#pragma ACCEL debug fflush(stdout);
  }
}

void __merlin_read_buffer_vec_add_kernel(int a[1048576],int b[1048576],int c[1048576],int inc)
{
  static int __m_a[1048576];
  static int __m_b[1048576];
  static int __m_c[1048576];
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "a", "1048576 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (a != 0) {
      __CMOST_COMMENT__ __vec_add_kernel__a_buffer;
      __CMOST_COMMENT__ __event___vec_add_kernel__a_buffer;
// __MERLIN_EVENT_READ__ __event___vec_add_kernel__a_buffer
      int __MERLIN_EVENT_READ____event___vec_add_kernel__a_buffer;
      opencl_read_buffer_nb(__vec_add_kernel__a_buffer,commands[0],0 * sizeof(int ),(int *)a,((unsigned long )1048576) * sizeof(int ),&__event___vec_add_kernel__a_buffer);
    }
     else {
      printf("Error! Detected null pointer 'a' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "a");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "b", "1048576 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (b != 0) {
      __CMOST_COMMENT__ __vec_add_kernel__b_buffer;
      __CMOST_COMMENT__ __event___vec_add_kernel__b_buffer;
// __MERLIN_EVENT_READ__ __event___vec_add_kernel__b_buffer
      int __MERLIN_EVENT_READ____event___vec_add_kernel__b_buffer;
      opencl_read_buffer_nb(__vec_add_kernel__b_buffer,commands[0],0 * sizeof(int ),(int *)b,((unsigned long )1048576) * sizeof(int ),&__event___vec_add_kernel__b_buffer);
    }
     else {
      printf("Error! Detected null pointer 'b' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "b");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "c", "1048576 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (c != 0) {
      __CMOST_COMMENT__ __vec_add_kernel__c_buffer;
      __CMOST_COMMENT__ __event___vec_add_kernel__c_buffer;
// __MERLIN_EVENT_READ__ __event___vec_add_kernel__c_buffer
      int __MERLIN_EVENT_READ____event___vec_add_kernel__c_buffer;
      opencl_read_buffer_nb(__vec_add_kernel__c_buffer,commands[0],0 * sizeof(int ),(int *)c,((unsigned long )1048576) * sizeof(int ),&__event___vec_add_kernel__c_buffer);
    }
     else {
      printf("Error! Detected null pointer 'c' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "c");
    
#pragma ACCEL debug fflush(stdout);
  }
}

void __merlin_execute_vec_add_kernel(int a[1048576],int b[1048576],int c[1048576],int inc)
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
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "prefetch_c");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__prefetch_c_kernel, 0, sizeof(opencl_mem), &__vec_add_kernel__c_buffer);
	#pragma ACCEL debug printf("[Merlin Info] Finish set kernel %s argument....\n", "prefetch_c");
	#pragma ACCEL debug fflush(stdout);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "prefetch_b");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__prefetch_b_kernel, 0, sizeof(opencl_mem), &__vec_add_kernel__b_buffer);
	#pragma ACCEL debug printf("[Merlin Info] Finish set kernel %s argument....\n", "prefetch_b");
	#pragma ACCEL debug fflush(stdout);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "prefetch_a");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__prefetch_a_kernel, 0, sizeof(opencl_mem), &__vec_add_kernel__a_buffer);
	#pragma ACCEL debug printf("[Merlin Info] Finish set kernel %s argument....\n", "prefetch_a");
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
	#pragma ACCEL debug printf("[Merlin Info] Start execute kernel %s....\n", "prefetch_c");
	#pragma ACCEL debug fflush(stdout);
	size_t __gid[1];
	__gid[0] = 1;
	opencl_enqueue_kernel(__prefetch_c_kernel, commands[1], 1, __gid, &__event_prefetch_c);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start execute kernel %s....\n", "prefetch_b");
	#pragma ACCEL debug fflush(stdout);
	size_t __gid[1];
	__gid[0] = 1;
	opencl_enqueue_kernel(__prefetch_b_kernel, commands[2], 1, __gid, &__event_prefetch_b);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start execute kernel %s....\n", "prefetch_a");
	#pragma ACCEL debug fflush(stdout);
	size_t __gid[1];
	__gid[0] = 1;
	opencl_enqueue_kernel(__prefetch_a_kernel, commands[3], 1, __gid, &__event_prefetch_a);
}
}
