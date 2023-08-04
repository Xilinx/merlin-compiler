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
void ellpack_kernel(double nzval[2097152],int cols[2097152],double vec[4096],double out[4096]);
static void __merlin_dummy_extern_int_merlin_include_L_();
#include "__merlinhead_kernel_top.h"
static void __merlin_dummy_extern_int_merlin_include_L_();

void __merlinwrapper_ellpack_kernel(double nzval[2097152],int cols[2097152],double vec[4096],double out[4096])
{
  __CMOST_COMMENT__ commands;
{
    
#pragma ACCEL wrapper variable=nzval port=nzval depth=2097152 max_depth=2097152 data_type=double io=RO copy=false
    
#pragma ACCEL wrapper variable=cols port=cols depth=2097152 max_depth=2097152 data_type=int io=RO copy=false
    
#pragma ACCEL wrapper variable=vec port=vec depth=4096 max_depth=4096 data_type=double io=RO copy=false
    
#pragma ACCEL wrapper variable=out depth=4096 max_depth=4096 data_type=double io=RW copy=false port=merlin_out
  }
  static __CMOST_COMMENT__ __m_nzval;
  static __CMOST_COMMENT__ __m_cols;
  static __CMOST_COMMENT__ __m_vec;
  static __CMOST_COMMENT__ __m_merlin_out;
{
/*
    static double __m_nzval[2097152];
    static int __m_cols[2097152];
    static double __m_vec[4096];
    static double __m_merlin_out[4096];
*/
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "nzval", "2097152 * sizeof(double )");
      
#pragma ACCEL debug fflush(stdout);
      if (nzval != 0) {
        __CMOST_COMMENT__ __ellpack_kernel__nzval_buffer;
        opencl_write_buffer(__ellpack_kernel__nzval_buffer,commands[0],0 * sizeof(double ),(double *)nzval,((unsigned long )2097152) * sizeof(double ));
      }
       else {
        printf("Error! The external memory pointed by 'nzval' is invalid.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "nzval");
      
#pragma ACCEL debug fflush(stdout);
    }
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "cols", "2097152 * sizeof(int )");
      
#pragma ACCEL debug fflush(stdout);
      if (cols != 0) {
        __CMOST_COMMENT__ __ellpack_kernel__cols_buffer;
        opencl_write_buffer(__ellpack_kernel__cols_buffer,commands[0],0 * sizeof(int ),(int *)cols,((unsigned long )2097152) * sizeof(int ));
      }
       else {
        printf("Error! The external memory pointed by 'cols' is invalid.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "cols");
      
#pragma ACCEL debug fflush(stdout);
    }
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "vec", "4096 * sizeof(double )");
      
#pragma ACCEL debug fflush(stdout);
      if (vec != 0) {
        __CMOST_COMMENT__ __ellpack_kernel__vec_buffer;
        opencl_write_buffer(__ellpack_kernel__vec_buffer,commands[0],0 * sizeof(double ),(double *)vec,((unsigned long )4096) * sizeof(double ));
      }
       else {
        printf("Error! The external memory pointed by 'vec' is invalid.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "vec");
      
#pragma ACCEL debug fflush(stdout);
    }
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "merlin_out", "4096 * sizeof(double )");
      
#pragma ACCEL debug fflush(stdout);
      if (out != 0) {
        __CMOST_COMMENT__ __ellpack_kernel__merlin_out_buffer;
        opencl_write_buffer(__ellpack_kernel__merlin_out_buffer,commands[0],0 * sizeof(double ),(double *)out,((unsigned long )4096) * sizeof(double ));
      }
       else {
        printf("Error! The external memory pointed by 'out' is invalid.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "merlin_out");
      
#pragma ACCEL debug fflush(stdout);
    }
  }
  
#pragma ACCEL kernel
{
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "ellpack_kernel");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__ellpack_kernel_kernel, 0, sizeof(opencl_mem), &__ellpack_kernel__nzval_buffer);
	opencl_set_kernel_arg(__ellpack_kernel_kernel, 1, sizeof(opencl_mem), &__ellpack_kernel__cols_buffer);
	opencl_set_kernel_arg(__ellpack_kernel_kernel, 2, sizeof(opencl_mem), &__ellpack_kernel__vec_buffer);
	opencl_set_kernel_arg(__ellpack_kernel_kernel, 3, sizeof(opencl_mem), &__ellpack_kernel__merlin_out_buffer);
	#pragma ACCEL debug printf("[Merlin Info] Finish set kernel %s argument....\n", "ellpack_kernel");
	#pragma ACCEL debug fflush(stdout);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start execute kernel %s....\n", "ellpack_kernel");
	#pragma ACCEL debug fflush(stdout);
	size_t __gid[1];
	__gid[0] = 1;
	opencl_enqueue_kernel(__ellpack_kernel_kernel, commands[0], 1, __gid, &__event_ellpack_kernel);
}
{
	clWaitForEvents(1, &__event_ellpack_kernel);
	#pragma ACCEL debug printf("[Merlin Info] Finish execute kernel %s....\n", "ellpack_kernel");
	#pragma ACCEL debug fflush(stdout);
}
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "merlin_out", "4096 * sizeof(double )");
    
#pragma ACCEL debug fflush(stdout);
    if (out != 0) {
      __CMOST_COMMENT__ __ellpack_kernel__merlin_out_buffer;
      opencl_read_buffer(__ellpack_kernel__merlin_out_buffer,commands[0],0 * sizeof(double ),(double *)out,((unsigned long )4096) * sizeof(double ));
    }
     else {
      printf("Error! The external memory pointed by 'out' is invalid.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "merlin_out");
    
#pragma ACCEL debug fflush(stdout);
  }
}

void __merlin_ellpack_kernel(double nzval[2097152],int cols[2097152],double vec[4096],double out[4096])
{
  
#pragma ACCEL string __merlin_check_opencl();
  
#pragma ACCEL string __merlin_init_ellpack_kernel();
  __merlinwrapper_ellpack_kernel(nzval,cols,vec,out);
  
#pragma ACCEL string __merlin_release_ellpack_kernel();
}

void __merlin_write_buffer_ellpack_kernel(double nzval[2097152],int cols[2097152],double vec[4096],double out[4096])
{
  static double __m_nzval[2097152];
  static int __m_cols[2097152];
  static double __m_vec[4096];
  static double __m_merlin_out[4096];
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "nzval", "2097152 * sizeof(double )");
    
#pragma ACCEL debug fflush(stdout);
    if (nzval != 0) {
      __CMOST_COMMENT__ __ellpack_kernel__nzval_buffer;
      __CMOST_COMMENT__ __event___ellpack_kernel__nzval_buffer;
// __MERLIN_EVENT_WRITE__ __event___ellpack_kernel__nzval_buffer
      int __MERLIN_EVENT_WRITE____event___ellpack_kernel__nzval_buffer;
      opencl_write_buffer_nb(__ellpack_kernel__nzval_buffer,commands[0],0 * sizeof(double ),(double *)nzval,((unsigned long )2097152) * sizeof(double ),&__event___ellpack_kernel__nzval_buffer);
    }
     else {
      printf("Error! The external memory pointed by 'nzval' is invalid.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "nzval");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "cols", "2097152 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (cols != 0) {
      __CMOST_COMMENT__ __ellpack_kernel__cols_buffer;
      __CMOST_COMMENT__ __event___ellpack_kernel__cols_buffer;
// __MERLIN_EVENT_WRITE__ __event___ellpack_kernel__cols_buffer
      int __MERLIN_EVENT_WRITE____event___ellpack_kernel__cols_buffer;
      opencl_write_buffer_nb(__ellpack_kernel__cols_buffer,commands[0],0 * sizeof(int ),(int *)cols,((unsigned long )2097152) * sizeof(int ),&__event___ellpack_kernel__cols_buffer);
    }
     else {
      printf("Error! The external memory pointed by 'cols' is invalid.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "cols");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "vec", "4096 * sizeof(double )");
    
#pragma ACCEL debug fflush(stdout);
    if (vec != 0) {
      __CMOST_COMMENT__ __ellpack_kernel__vec_buffer;
      __CMOST_COMMENT__ __event___ellpack_kernel__vec_buffer;
// __MERLIN_EVENT_WRITE__ __event___ellpack_kernel__vec_buffer
      int __MERLIN_EVENT_WRITE____event___ellpack_kernel__vec_buffer;
      opencl_write_buffer_nb(__ellpack_kernel__vec_buffer,commands[0],0 * sizeof(double ),(double *)vec,((unsigned long )4096) * sizeof(double ),&__event___ellpack_kernel__vec_buffer);
    }
     else {
      printf("Error! The external memory pointed by 'vec' is invalid.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "vec");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "merlin_out", "4096 * sizeof(double )");
    
#pragma ACCEL debug fflush(stdout);
    if (out != 0) {
      __CMOST_COMMENT__ __ellpack_kernel__merlin_out_buffer;
      __CMOST_COMMENT__ __event___ellpack_kernel__merlin_out_buffer;
// __MERLIN_EVENT_WRITE__ __event___ellpack_kernel__merlin_out_buffer
      int __MERLIN_EVENT_WRITE____event___ellpack_kernel__merlin_out_buffer;
      opencl_write_buffer_nb(__ellpack_kernel__merlin_out_buffer,commands[0],0 * sizeof(double ),(double *)out,((unsigned long )4096) * sizeof(double ),&__event___ellpack_kernel__merlin_out_buffer);
    }
     else {
      printf("Error! The external memory pointed by 'out' is invalid.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "merlin_out");
    
#pragma ACCEL debug fflush(stdout);
  }
}

void __merlin_read_buffer_ellpack_kernel(double nzval[2097152],int cols[2097152],double vec[4096],double out[4096])
{
  static double __m_nzval[2097152];
  static int __m_cols[2097152];
  static double __m_vec[4096];
  static double __m_merlin_out[4096];
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "merlin_out", "4096 * sizeof(double )");
    
#pragma ACCEL debug fflush(stdout);
    if (out != 0) {
      __CMOST_COMMENT__ __ellpack_kernel__merlin_out_buffer;
      __CMOST_COMMENT__ __event___ellpack_kernel__merlin_out_buffer;
// __MERLIN_EVENT_READ__ __event___ellpack_kernel__merlin_out_buffer
      int __MERLIN_EVENT_READ____event___ellpack_kernel__merlin_out_buffer;
      opencl_read_buffer_nb(__ellpack_kernel__merlin_out_buffer,commands[0],0 * sizeof(double ),(double *)out,((unsigned long )4096) * sizeof(double ),&__event___ellpack_kernel__merlin_out_buffer);
    }
     else {
      printf("Error! The external memory pointed by 'out' is invalid.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "merlin_out");
    
#pragma ACCEL debug fflush(stdout);
  }
}

void __merlin_execute_ellpack_kernel(double nzval[2097152],int cols[2097152],double vec[4096],double out[4096])
{
  int i;
{
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "ellpack_kernel");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__ellpack_kernel_kernel, 0, sizeof(opencl_mem), &__ellpack_kernel__nzval_buffer);
	opencl_set_kernel_arg(__ellpack_kernel_kernel, 1, sizeof(opencl_mem), &__ellpack_kernel__cols_buffer);
	opencl_set_kernel_arg(__ellpack_kernel_kernel, 2, sizeof(opencl_mem), &__ellpack_kernel__vec_buffer);
	opencl_set_kernel_arg(__ellpack_kernel_kernel, 3, sizeof(opencl_mem), &__ellpack_kernel__merlin_out_buffer);
	#pragma ACCEL debug printf("[Merlin Info] Finish set kernel %s argument....\n", "ellpack_kernel");
	#pragma ACCEL debug fflush(stdout);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start execute kernel %s....\n", "ellpack_kernel");
	#pragma ACCEL debug fflush(stdout);
	size_t __gid[1];
	__gid[0] = 1;
	opencl_enqueue_kernel(__ellpack_kernel_kernel, commands[0], 1, __gid, &__event_ellpack_kernel);
}
}
