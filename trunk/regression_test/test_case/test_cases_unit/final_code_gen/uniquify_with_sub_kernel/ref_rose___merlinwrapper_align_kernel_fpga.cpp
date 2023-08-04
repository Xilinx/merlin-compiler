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
#include <assert.h>
#include "merlin_type_define.h"
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_442();
static const unsigned long NWORDS_PER_REG = 16;
static const unsigned long NBYTES_PER_REG = 16;
static const unsigned long NBYTES_PER_WORD = 1;
void align_kernel_fpga(char rf_vec[20480000],int rf_len_vec[10000],int profbuf_size_vec[10000],unsigned char *profbuf_vec_data,long results[10000],int N);
static void __merlin_dummy_extern_int_merlin_include_L_();
#include "__merlinhead_kernel_top.h"
static void __merlin_dummy_extern_int_merlin_include_L_();
void msm_port_rf_vec_align_kernel_fpga_0(char rf_vec[20480000]);
void msm_port_rf_len_vec_align_kernel_fpga_0(int rf_len_vec[10000]);
void msm_port_profbuf_size_vec_align_kernel_fpga_0(int profbuf_size_vec[10000]);
void msm_port_profbuf_vec_data_align_kernel_fpga_0(unsigned char *profbuf_vec_data);
void msm_port_results_align_kernel_fpga_0(long results[10000]);

void __merlinwrapper_align_kernel_fpga(char rf_vec[20480000],int rf_len_vec[10000],int profbuf_size_vec[10000],struct m128i profbuf_vec[1280000],long results[10000],int N)
{
  __CMOST_COMMENT__ commands;
{
    
#pragma ACCEL wrapper variable=rf_vec port=rf_vec depth=20480000 max_depth=20480000 data_type=char io=RO copy=false
    
#pragma ACCEL wrapper variable=rf_len_vec port=rf_len_vec depth=10000 max_depth=10000 data_type=int io=NO copy=false
    
#pragma ACCEL wrapper variable=profbuf_size_vec port=profbuf_size_vec depth=10000 max_depth=10000 data_type=int io=RO copy=false
    
#pragma ACCEL wrapper variable=profbuf_vec->data port=profbuf_vec_data depth=1280000,16 max_depth=1280000,16 data_type="unsigned char" io=RW continue=1 copy=true
    
#pragma ACCEL wrapper variable=results port=results depth=10000 max_depth=10000 data_type=long io=WO copy=false
    
#pragma ACCEL wrapper variable=N port=N
  }
  static __CMOST_COMMENT__ __m_rf_vec;
  static __CMOST_COMMENT__ __m_rf_len_vec;
  static __CMOST_COMMENT__ __m_profbuf_size_vec;
  static __CMOST_COMMENT__ __m_profbuf_vec_data;
  static __CMOST_COMMENT__ __m_results;
{
/*
    static char __m_rf_vec[20480000];
    static int __m_rf_len_vec[10000];
    static int __m_profbuf_size_vec[10000];
    static unsigned char __m_profbuf_vec_data[20480000];
    static long __m_results[10000];
*/
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "rf_vec", "20480000 * sizeof(char )");
      
#pragma ACCEL debug fflush(stdout);
      if (rf_vec != 0) {
        __CMOST_COMMENT__ __align_kernel_fpga__rf_vec_buffer;
        opencl_write_buffer(__align_kernel_fpga__rf_vec_buffer,commands[0],0 * sizeof(char ),(char *)rf_vec,((unsigned long )20480000) * sizeof(char ));
      }
       else {
        printf("Error! The external memory pointed by 'rf_vec' is invalid.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "rf_vec");
      
#pragma ACCEL debug fflush(stdout);
    }
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "profbuf_size_vec", "10000 * sizeof(int )");
      
#pragma ACCEL debug fflush(stdout);
      if (profbuf_size_vec != 0) {
        __CMOST_COMMENT__ __align_kernel_fpga__profbuf_size_vec_buffer;
        opencl_write_buffer(__align_kernel_fpga__profbuf_size_vec_buffer,commands[0],0 * sizeof(int ),(int *)profbuf_size_vec,((unsigned long )10000) * sizeof(int ));
      }
       else {
        printf("Error! The external memory pointed by 'profbuf_size_vec' is invalid.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "profbuf_size_vec");
      
#pragma ACCEL debug fflush(stdout);
    }
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "profbuf_vec_data", "1280000 * 16 * sizeof(unsigned char )");
      
#pragma ACCEL debug fflush(stdout);
      if (profbuf_vec != 0 && profbuf_vec[0] . data != 0) {
        __CMOST_COMMENT__ __align_kernel_fpga__profbuf_vec_data_buffer;
        opencl_write_buffer(__align_kernel_fpga__profbuf_vec_data_buffer,commands[0],0 * sizeof(unsigned char ),(unsigned char *)profbuf_vec[0] . data,((unsigned long )(1280000 * 16)) * sizeof(unsigned char ));
      }
       else {
        printf("Error! The external memory pointed by 'profbuf_vec[0] . data' is invalid.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "profbuf_vec_data");
      
#pragma ACCEL debug fflush(stdout);
    }
  }
  
#pragma ACCEL sub_kernel parent=align_kernel_fpga
  
#pragma ACCEL sub_kernel parent=align_kernel_fpga
  
#pragma ACCEL sub_kernel parent=align_kernel_fpga
  
#pragma ACCEL sub_kernel parent=align_kernel_fpga
{
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "align_kernel_fpga");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__align_kernel_fpga_kernel, 0, sizeof(opencl_mem), &__align_kernel_fpga__rf_vec_buffer);
	opencl_set_kernel_arg(__align_kernel_fpga_kernel, 1, sizeof(opencl_mem), &__align_kernel_fpga__rf_len_vec_buffer);
	opencl_set_kernel_arg(__align_kernel_fpga_kernel, 2, sizeof(opencl_mem), &__align_kernel_fpga__profbuf_size_vec_buffer);
	opencl_set_kernel_arg(__align_kernel_fpga_kernel, 3, sizeof(opencl_mem), &__align_kernel_fpga__profbuf_vec_data_buffer);
	opencl_set_kernel_arg(__align_kernel_fpga_kernel, 4, sizeof(opencl_mem), &__align_kernel_fpga__results_buffer);
	opencl_set_kernel_arg(__align_kernel_fpga_kernel, 5, sizeof(int), &N);
	#pragma ACCEL debug printf("[Merlin Info] Finish set kernel %s argument....\n", "align_kernel_fpga");
	#pragma ACCEL debug fflush(stdout);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "msm_port_rf_vec_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__msm_port_rf_vec_align_kernel_fpga_0_kernel, 0, sizeof(opencl_mem), &__align_kernel_fpga__rf_vec_buffer);
	#pragma ACCEL debug printf("[Merlin Info] Finish set kernel %s argument....\n", "msm_port_rf_vec_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "msm_port_rf_len_vec_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__msm_port_rf_len_vec_align_kernel_fpga_0_kernel, 0, sizeof(opencl_mem), &__align_kernel_fpga__rf_len_vec_buffer);
	#pragma ACCEL debug printf("[Merlin Info] Finish set kernel %s argument....\n", "msm_port_rf_len_vec_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "msm_port_profbuf_size_vec_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__msm_port_profbuf_size_vec_align_kernel_fpga_0_kernel, 0, sizeof(opencl_mem), &__align_kernel_fpga__profbuf_size_vec_buffer);
	#pragma ACCEL debug printf("[Merlin Info] Finish set kernel %s argument....\n", "msm_port_profbuf_size_vec_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "msm_port_profbuf_vec_data_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__msm_port_profbuf_vec_data_align_kernel_fpga_0_kernel, 0, sizeof(opencl_mem), &__align_kernel_fpga__profbuf_vec_data_buffer);
	#pragma ACCEL debug printf("[Merlin Info] Finish set kernel %s argument....\n", "msm_port_profbuf_vec_data_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "msm_port_results_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__msm_port_results_align_kernel_fpga_0_kernel, 0, sizeof(opencl_mem), &__align_kernel_fpga__results_buffer);
	#pragma ACCEL debug printf("[Merlin Info] Finish set kernel %s argument....\n", "msm_port_results_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start execute kernel %s....\n", "align_kernel_fpga");
	#pragma ACCEL debug fflush(stdout);
	size_t __gid[1];
	__gid[0] = 1;
	opencl_enqueue_kernel(__align_kernel_fpga_kernel, commands[0], 1, __gid, &__event_align_kernel_fpga);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start execute kernel %s....\n", "msm_port_rf_vec_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
	size_t __gid[1];
	__gid[0] = 1;
	opencl_enqueue_kernel(__msm_port_rf_vec_align_kernel_fpga_0_kernel, commands[1], 1, __gid, &__event_msm_port_rf_vec_align_kernel_fpga_0);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start execute kernel %s....\n", "msm_port_rf_len_vec_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
	size_t __gid[1];
	__gid[0] = 1;
	opencl_enqueue_kernel(__msm_port_rf_len_vec_align_kernel_fpga_0_kernel, commands[2], 1, __gid, &__event_msm_port_rf_len_vec_align_kernel_fpga_0);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start execute kernel %s....\n", "msm_port_profbuf_size_vec_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
	size_t __gid[1];
	__gid[0] = 1;
	opencl_enqueue_kernel(__msm_port_profbuf_size_vec_align_kernel_fpga_0_kernel, commands[3], 1, __gid, &__event_msm_port_profbuf_size_vec_align_kernel_fpga_0);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start execute kernel %s....\n", "msm_port_profbuf_vec_data_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
	size_t __gid[1];
	__gid[0] = 1;
	opencl_enqueue_kernel(__msm_port_profbuf_vec_data_align_kernel_fpga_0_kernel, commands[4], 1, __gid, &__event_msm_port_profbuf_vec_data_align_kernel_fpga_0);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start execute kernel %s....\n", "msm_port_results_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
	size_t __gid[1];
	__gid[0] = 1;
	opencl_enqueue_kernel(__msm_port_results_align_kernel_fpga_0_kernel, commands[5], 1, __gid, &__event_msm_port_results_align_kernel_fpga_0);
}
{
	clWaitForEvents(1, &__event_align_kernel_fpga);
	if(__event_align_kernel_fpga) {
		opencl_release_event(__event_align_kernel_fpga);
	}
	#pragma ACCEL debug printf("[Merlin Info] Finish execute kernel %s....\n", "align_kernel_fpga");
	#pragma ACCEL debug fflush(stdout);
}
{
	clWaitForEvents(1, &__event_msm_port_rf_vec_align_kernel_fpga_0);
	if(__event_msm_port_rf_vec_align_kernel_fpga_0) {
		opencl_release_event(__event_msm_port_rf_vec_align_kernel_fpga_0);
	}
	#pragma ACCEL debug printf("[Merlin Info] Finish execute kernel %s....\n", "msm_port_rf_vec_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
}
{
	clWaitForEvents(1, &__event_msm_port_rf_len_vec_align_kernel_fpga_0);
	if(__event_msm_port_rf_len_vec_align_kernel_fpga_0) {
		opencl_release_event(__event_msm_port_rf_len_vec_align_kernel_fpga_0);
	}
	#pragma ACCEL debug printf("[Merlin Info] Finish execute kernel %s....\n", "msm_port_rf_len_vec_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
}
{
	clWaitForEvents(1, &__event_msm_port_profbuf_size_vec_align_kernel_fpga_0);
	if(__event_msm_port_profbuf_size_vec_align_kernel_fpga_0) {
		opencl_release_event(__event_msm_port_profbuf_size_vec_align_kernel_fpga_0);
	}
	#pragma ACCEL debug printf("[Merlin Info] Finish execute kernel %s....\n", "msm_port_profbuf_size_vec_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
}
{
	clWaitForEvents(1, &__event_msm_port_profbuf_vec_data_align_kernel_fpga_0);
	if(__event_msm_port_profbuf_vec_data_align_kernel_fpga_0) {
		opencl_release_event(__event_msm_port_profbuf_vec_data_align_kernel_fpga_0);
	}
	#pragma ACCEL debug printf("[Merlin Info] Finish execute kernel %s....\n", "msm_port_profbuf_vec_data_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
}
{
	clWaitForEvents(1, &__event_msm_port_results_align_kernel_fpga_0);
	if(__event_msm_port_results_align_kernel_fpga_0) {
		opencl_release_event(__event_msm_port_results_align_kernel_fpga_0);
	}
	#pragma ACCEL debug printf("[Merlin Info] Finish execute kernel %s....\n", "msm_port_results_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
}
  
#pragma ACCEL sub_kernel parent=align_kernel_fpga
{
/*
    static char __m_rf_vec[20480000];
    static int __m_rf_len_vec[10000];
    static int __m_profbuf_size_vec[10000];
    static unsigned char __m_profbuf_vec_data[20480000];
    static long __m_results[10000];
*/
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "profbuf_vec_data", "1280000 * 16 * sizeof(unsigned char )");
      
#pragma ACCEL debug fflush(stdout);
      if (profbuf_vec != 0 && profbuf_vec[0] . data != 0) {
        __CMOST_COMMENT__ __align_kernel_fpga__profbuf_vec_data_buffer;
        opencl_read_buffer(__align_kernel_fpga__profbuf_vec_data_buffer,commands[0],0 * sizeof(unsigned char ),(unsigned char *)profbuf_vec[0] . data,((unsigned long )(1280000 * 16)) * sizeof(unsigned char ));
      }
       else {
        printf("Error! The external memory pointed by 'profbuf_vec[0] . data' is invalid.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "profbuf_vec_data");
      
#pragma ACCEL debug fflush(stdout);
    }
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "results", "10000 * sizeof(long )");
      
#pragma ACCEL debug fflush(stdout);
      if (results != 0) {
        __CMOST_COMMENT__ __align_kernel_fpga__results_buffer;
        opencl_read_buffer(__align_kernel_fpga__results_buffer,commands[0],0 * sizeof(long ),(long *)results,((unsigned long )10000) * sizeof(long ));
      }
       else {
        printf("Error! The external memory pointed by 'results' is invalid.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "results");
      
#pragma ACCEL debug fflush(stdout);
    }
  }
}

void __merlin_align_kernel_fpga(char rf_vec[20480000],int rf_len_vec[10000],int profbuf_size_vec[10000],struct m128i profbuf_vec[1280000],long results[10000],int N)
{
  
#pragma ACCEL string __merlin_check_opencl();
  
#pragma ACCEL string __merlin_init_align_kernel_fpga();
  __merlinwrapper_align_kernel_fpga(rf_vec,rf_len_vec,profbuf_size_vec,profbuf_vec,results,N);
  
#pragma ACCEL string __merlin_release_align_kernel_fpga();
}

void __merlin_write_buffer_align_kernel_fpga(char rf_vec[20480000],int rf_len_vec[10000],int profbuf_size_vec[10000],struct m128i profbuf_vec[1280000],long results[10000],int N)
{
  static char __m_rf_vec[20480000];
  static int __m_rf_len_vec[10000];
  static int __m_profbuf_size_vec[10000];
  static unsigned char __m_profbuf_vec_data[20480000];
  static long __m_results[10000];
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "rf_vec", "20480000 * sizeof(char )");
    
#pragma ACCEL debug fflush(stdout);
    if (rf_vec != 0) {
      __CMOST_COMMENT__ __align_kernel_fpga__rf_vec_buffer;
      __CMOST_COMMENT__ __event___align_kernel_fpga__rf_vec_buffer;
// __MERLIN_EVENT_WRITE__ __event___align_kernel_fpga__rf_vec_buffer
      int __MERLIN_EVENT_WRITE____event___align_kernel_fpga__rf_vec_buffer;
      opencl_write_buffer_nb(__align_kernel_fpga__rf_vec_buffer,commands[0],0 * sizeof(char ),(char *)rf_vec,((unsigned long )20480000) * sizeof(char ),&__event___align_kernel_fpga__rf_vec_buffer);
    }
     else {
      printf("Error! The external memory pointed by 'rf_vec' is invalid.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "rf_vec");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "profbuf_size_vec", "10000 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (profbuf_size_vec != 0) {
      __CMOST_COMMENT__ __align_kernel_fpga__profbuf_size_vec_buffer;
      __CMOST_COMMENT__ __event___align_kernel_fpga__profbuf_size_vec_buffer;
// __MERLIN_EVENT_WRITE__ __event___align_kernel_fpga__profbuf_size_vec_buffer
      int __MERLIN_EVENT_WRITE____event___align_kernel_fpga__profbuf_size_vec_buffer;
      opencl_write_buffer_nb(__align_kernel_fpga__profbuf_size_vec_buffer,commands[0],0 * sizeof(int ),(int *)profbuf_size_vec,((unsigned long )10000) * sizeof(int ),&__event___align_kernel_fpga__profbuf_size_vec_buffer);
    }
     else {
      printf("Error! The external memory pointed by 'profbuf_size_vec' is invalid.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "profbuf_size_vec");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "profbuf_vec_data", "1280000 * 16 * sizeof(unsigned char )");
    
#pragma ACCEL debug fflush(stdout);
    if (profbuf_vec != 0 && profbuf_vec[0] . data != 0) {
      __CMOST_COMMENT__ __align_kernel_fpga__profbuf_vec_data_buffer;
      __CMOST_COMMENT__ __event___align_kernel_fpga__profbuf_vec_data_buffer;
// __MERLIN_EVENT_WRITE__ __event___align_kernel_fpga__profbuf_vec_data_buffer
      int __MERLIN_EVENT_WRITE____event___align_kernel_fpga__profbuf_vec_data_buffer;
      opencl_write_buffer_nb(__align_kernel_fpga__profbuf_vec_data_buffer,commands[0],0 * sizeof(unsigned char ),(unsigned char *)profbuf_vec[0] . data,((unsigned long )(1280000 * 16)) * sizeof(unsigned char ),&__event___align_kernel_fpga__profbuf_vec_data_buffer);
    }
     else {
      printf("Error! The external memory pointed by 'profbuf_vec[0] . data' is invalid.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "profbuf_vec_data");
    
#pragma ACCEL debug fflush(stdout);
  }
}

void __merlin_read_buffer_align_kernel_fpga(char rf_vec[20480000],int rf_len_vec[10000],int profbuf_size_vec[10000],struct m128i profbuf_vec[1280000],long results[10000],int N)
{
  static char __m_rf_vec[20480000];
  static int __m_rf_len_vec[10000];
  static int __m_profbuf_size_vec[10000];
  static unsigned char __m_profbuf_vec_data[20480000];
  static long __m_results[10000];
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "profbuf_vec_data", "1280000 * 16 * sizeof(unsigned char )");
    
#pragma ACCEL debug fflush(stdout);
    if (profbuf_vec != 0 && profbuf_vec[0] . data != 0) {
      __CMOST_COMMENT__ __align_kernel_fpga__profbuf_vec_data_buffer;
      __CMOST_COMMENT__ __event___align_kernel_fpga__profbuf_vec_data_buffer;
// __MERLIN_EVENT_READ__ __event___align_kernel_fpga__profbuf_vec_data_buffer
      int __MERLIN_EVENT_READ____event___align_kernel_fpga__profbuf_vec_data_buffer;
      opencl_read_buffer_nb(__align_kernel_fpga__profbuf_vec_data_buffer,commands[0],0 * sizeof(unsigned char ),(unsigned char *)profbuf_vec[0] . data,((unsigned long )(1280000 * 16)) * sizeof(unsigned char ),&__event___align_kernel_fpga__profbuf_vec_data_buffer);
    }
     else {
      printf("Error! The external memory pointed by 'profbuf_vec[0] . data' is invalid.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "profbuf_vec_data");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "results", "10000 * sizeof(long )");
    
#pragma ACCEL debug fflush(stdout);
    if (results != 0) {
      __CMOST_COMMENT__ __align_kernel_fpga__results_buffer;
      __CMOST_COMMENT__ __event___align_kernel_fpga__results_buffer;
// __MERLIN_EVENT_READ__ __event___align_kernel_fpga__results_buffer
      int __MERLIN_EVENT_READ____event___align_kernel_fpga__results_buffer;
      opencl_read_buffer_nb(__align_kernel_fpga__results_buffer,commands[0],0 * sizeof(long ),(long *)results,((unsigned long )10000) * sizeof(long ),&__event___align_kernel_fpga__results_buffer);
    }
     else {
      printf("Error! The external memory pointed by 'results' is invalid.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "results");
    
#pragma ACCEL debug fflush(stdout);
  }
}

void __merlin_execute_align_kernel_fpga(char rf_vec[20480000],int rf_len_vec[10000],int profbuf_size_vec[10000],struct m128i profbuf_vec[1280000],long results[10000],int N)
{
  int i;
{
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "align_kernel_fpga");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__align_kernel_fpga_kernel, 0, sizeof(opencl_mem), &__align_kernel_fpga__rf_vec_buffer);
	opencl_set_kernel_arg(__align_kernel_fpga_kernel, 1, sizeof(opencl_mem), &__align_kernel_fpga__rf_len_vec_buffer);
	opencl_set_kernel_arg(__align_kernel_fpga_kernel, 2, sizeof(opencl_mem), &__align_kernel_fpga__profbuf_size_vec_buffer);
	opencl_set_kernel_arg(__align_kernel_fpga_kernel, 3, sizeof(opencl_mem), &__align_kernel_fpga__profbuf_vec_data_buffer);
	opencl_set_kernel_arg(__align_kernel_fpga_kernel, 4, sizeof(opencl_mem), &__align_kernel_fpga__results_buffer);
	opencl_set_kernel_arg(__align_kernel_fpga_kernel, 5, sizeof(int), &N);
	#pragma ACCEL debug printf("[Merlin Info] Finish set kernel %s argument....\n", "align_kernel_fpga");
	#pragma ACCEL debug fflush(stdout);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "msm_port_rf_vec_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__msm_port_rf_vec_align_kernel_fpga_0_kernel, 0, sizeof(opencl_mem), &__align_kernel_fpga__rf_vec_buffer);
	#pragma ACCEL debug printf("[Merlin Info] Finish set kernel %s argument....\n", "msm_port_rf_vec_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "msm_port_rf_len_vec_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__msm_port_rf_len_vec_align_kernel_fpga_0_kernel, 0, sizeof(opencl_mem), &__align_kernel_fpga__rf_len_vec_buffer);
	#pragma ACCEL debug printf("[Merlin Info] Finish set kernel %s argument....\n", "msm_port_rf_len_vec_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "msm_port_profbuf_size_vec_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__msm_port_profbuf_size_vec_align_kernel_fpga_0_kernel, 0, sizeof(opencl_mem), &__align_kernel_fpga__profbuf_size_vec_buffer);
	#pragma ACCEL debug printf("[Merlin Info] Finish set kernel %s argument....\n", "msm_port_profbuf_size_vec_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "msm_port_profbuf_vec_data_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__msm_port_profbuf_vec_data_align_kernel_fpga_0_kernel, 0, sizeof(opencl_mem), &__align_kernel_fpga__profbuf_vec_data_buffer);
	#pragma ACCEL debug printf("[Merlin Info] Finish set kernel %s argument....\n", "msm_port_profbuf_vec_data_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "msm_port_results_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__msm_port_results_align_kernel_fpga_0_kernel, 0, sizeof(opencl_mem), &__align_kernel_fpga__results_buffer);
	#pragma ACCEL debug printf("[Merlin Info] Finish set kernel %s argument....\n", "msm_port_results_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start execute kernel %s....\n", "align_kernel_fpga");
	#pragma ACCEL debug fflush(stdout);
	size_t __gid[1];
	__gid[0] = 1;
	opencl_enqueue_kernel(__align_kernel_fpga_kernel, commands[0], 1, __gid, &__event_align_kernel_fpga);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start execute kernel %s....\n", "msm_port_rf_vec_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
	size_t __gid[1];
	__gid[0] = 1;
	opencl_enqueue_kernel(__msm_port_rf_vec_align_kernel_fpga_0_kernel, commands[1], 1, __gid, &__event_msm_port_rf_vec_align_kernel_fpga_0);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start execute kernel %s....\n", "msm_port_rf_len_vec_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
	size_t __gid[1];
	__gid[0] = 1;
	opencl_enqueue_kernel(__msm_port_rf_len_vec_align_kernel_fpga_0_kernel, commands[2], 1, __gid, &__event_msm_port_rf_len_vec_align_kernel_fpga_0);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start execute kernel %s....\n", "msm_port_profbuf_size_vec_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
	size_t __gid[1];
	__gid[0] = 1;
	opencl_enqueue_kernel(__msm_port_profbuf_size_vec_align_kernel_fpga_0_kernel, commands[3], 1, __gid, &__event_msm_port_profbuf_size_vec_align_kernel_fpga_0);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start execute kernel %s....\n", "msm_port_profbuf_vec_data_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
	size_t __gid[1];
	__gid[0] = 1;
	opencl_enqueue_kernel(__msm_port_profbuf_vec_data_align_kernel_fpga_0_kernel, commands[4], 1, __gid, &__event_msm_port_profbuf_vec_data_align_kernel_fpga_0);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start execute kernel %s....\n", "msm_port_results_align_kernel_fpga_0");
	#pragma ACCEL debug fflush(stdout);
	size_t __gid[1];
	__gid[0] = 1;
	opencl_enqueue_kernel(__msm_port_results_align_kernel_fpga_0_kernel, commands[5], 1, __gid, &__event_msm_port_results_align_kernel_fpga_0);
}
}
