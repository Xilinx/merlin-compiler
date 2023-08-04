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
void needwun_kernel(char *SEQA,char *SEQB,char *alignedA,char *alignedB);
static void __merlin_dummy_extern_int_merlin_include_L_();
#include "__merlinhead_kernel_top.h"
static void __merlin_dummy_extern_int_merlin_include_L_();

void __merlinwrapper_needwun_kernel(char *SEQA,char *SEQB,char *alignedA,char *alignedB)
{
  __CMOST_COMMENT__ commands;
{
    
#pragma ACCEL wrapper variable=SEQA port=SEQA depth=131072 max_depth=131072 data_type=char io=RO copy=false
    
#pragma ACCEL wrapper variable=SEQB port=SEQB depth=131072 max_depth=131072 data_type=char io=RO copy=false
    
#pragma ACCEL wrapper variable=alignedA port=alignedA depth=262144 max_depth=262144 data_type=char io=WO copy=false
    
#pragma ACCEL wrapper variable=alignedB port=alignedB depth=262144 max_depth=262144 data_type=char io=WO copy=false
  }
  static __CMOST_COMMENT__ __m_SEQA;
  static __CMOST_COMMENT__ __m_SEQB;
  static __CMOST_COMMENT__ __m_alignedA;
  static __CMOST_COMMENT__ __m_alignedB;
{
/*
    static char __m_SEQA[131072];
    static char __m_SEQB[131072];
    static char __m_alignedA[262144];
    static char __m_alignedB[262144];
*/
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "SEQA", "131072 * sizeof(char )");
      
#pragma ACCEL debug fflush(stdout);
      if (SEQA != 0) {
        __CMOST_COMMENT__ __needwun_kernel__SEQA_buffer;
        opencl_write_buffer(__needwun_kernel__SEQA_buffer,commands[0],0 * sizeof(char ),(char *)SEQA,((unsigned long )131072) * sizeof(char ));
      }
       else {
        printf("Error! Detected null pointer 'SEQA' for external memory.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "SEQA");
      
#pragma ACCEL debug fflush(stdout);
    }
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "SEQB", "131072 * sizeof(char )");
      
#pragma ACCEL debug fflush(stdout);
      if (SEQB != 0) {
        __CMOST_COMMENT__ __needwun_kernel__SEQB_buffer;
        opencl_write_buffer(__needwun_kernel__SEQB_buffer,commands[0],0 * sizeof(char ),(char *)SEQB,((unsigned long )131072) * sizeof(char ));
      }
       else {
        printf("Error! Detected null pointer 'SEQB' for external memory.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "SEQB");
      
#pragma ACCEL debug fflush(stdout);
    }
  }
  
#pragma ACCEL kernel
{
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "needwun_kernel");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__needwun_kernel_kernel, 0, sizeof(opencl_mem), &__needwun_kernel__SEQA_buffer);
	opencl_set_kernel_arg(__needwun_kernel_kernel, 1, sizeof(opencl_mem), &__needwun_kernel__SEQB_buffer);
	opencl_set_kernel_arg(__needwun_kernel_kernel, 2, sizeof(opencl_mem), &__needwun_kernel__alignedA_buffer);
	opencl_set_kernel_arg(__needwun_kernel_kernel, 3, sizeof(opencl_mem), &__needwun_kernel__alignedB_buffer);
	#pragma ACCEL debug printf("[Merlin Info] Finish set kernel %s argument....\n", "needwun_kernel");
	#pragma ACCEL debug fflush(stdout);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start execute kernel %s....\n", "needwun_kernel");
	#pragma ACCEL debug fflush(stdout);
	size_t __gid[1];
	__gid[0] = 1;
	opencl_enqueue_kernel(__needwun_kernel_kernel, commands[0], 1, __gid, &__event_needwun_kernel);
}
{
	clWaitForEvents(1, &__event_needwun_kernel);
	if(__event_needwun_kernel) {
		opencl_release_event(__event_needwun_kernel);
	}
	#pragma ACCEL debug printf("[Merlin Info] Finish execute kernel %s....\n", "needwun_kernel");
	#pragma ACCEL debug fflush(stdout);
}
{
/*
    static char __m_SEQA[131072];
    static char __m_SEQB[131072];
    static char __m_alignedA[262144];
    static char __m_alignedB[262144];
*/
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "alignedA", "262144 * sizeof(char )");
      
#pragma ACCEL debug fflush(stdout);
      if (alignedA != 0) {
        __CMOST_COMMENT__ __needwun_kernel__alignedA_buffer;
        opencl_read_buffer(__needwun_kernel__alignedA_buffer,commands[0],0 * sizeof(char ),(char *)alignedA,((unsigned long )262144) * sizeof(char ));
      }
       else {
        printf("Error! Detected null pointer 'alignedA' for external memory.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "alignedA");
      
#pragma ACCEL debug fflush(stdout);
    }
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "alignedB", "262144 * sizeof(char )");
      
#pragma ACCEL debug fflush(stdout);
      if (alignedB != 0) {
        __CMOST_COMMENT__ __needwun_kernel__alignedB_buffer;
        opencl_read_buffer(__needwun_kernel__alignedB_buffer,commands[0],0 * sizeof(char ),(char *)alignedB,((unsigned long )262144) * sizeof(char ));
      }
       else {
        printf("Error! Detected null pointer 'alignedB' for external memory.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "alignedB");
      
#pragma ACCEL debug fflush(stdout);
    }
  }
}

void __merlin_needwun_kernel(char *SEQA,char *SEQB,char *alignedA,char *alignedB)
{
  
#pragma ACCEL string __merlin_check_opencl();
  
#pragma ACCEL string __merlin_init_needwun_kernel();
  __merlinwrapper_needwun_kernel(SEQA,SEQB,alignedA,alignedB);
  
#pragma ACCEL string __merlin_release_needwun_kernel();
}

void __merlin_write_buffer_needwun_kernel(char *SEQA,char *SEQB,char *alignedA,char *alignedB)
{
  static char __m_SEQA[131072];
  static char __m_SEQB[131072];
  static char __m_alignedA[262144];
  static char __m_alignedB[262144];
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "SEQA", "131072 * sizeof(char )");
    
#pragma ACCEL debug fflush(stdout);
    if (SEQA != 0) {
      __CMOST_COMMENT__ __needwun_kernel__SEQA_buffer;
      __CMOST_COMMENT__ __event___needwun_kernel__SEQA_buffer;
// __MERLIN_EVENT_WRITE__ __event___needwun_kernel__SEQA_buffer
      int __MERLIN_EVENT_WRITE____event___needwun_kernel__SEQA_buffer;
      opencl_write_buffer_nb(__needwun_kernel__SEQA_buffer,commands[0],0 * sizeof(char ),(char *)SEQA,((unsigned long )131072) * sizeof(char ),&__event___needwun_kernel__SEQA_buffer);
    }
     else {
      printf("Error! Detected null pointer 'SEQA' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "SEQA");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "SEQB", "131072 * sizeof(char )");
    
#pragma ACCEL debug fflush(stdout);
    if (SEQB != 0) {
      __CMOST_COMMENT__ __needwun_kernel__SEQB_buffer;
      __CMOST_COMMENT__ __event___needwun_kernel__SEQB_buffer;
// __MERLIN_EVENT_WRITE__ __event___needwun_kernel__SEQB_buffer
      int __MERLIN_EVENT_WRITE____event___needwun_kernel__SEQB_buffer;
      opencl_write_buffer_nb(__needwun_kernel__SEQB_buffer,commands[0],0 * sizeof(char ),(char *)SEQB,((unsigned long )131072) * sizeof(char ),&__event___needwun_kernel__SEQB_buffer);
    }
     else {
      printf("Error! Detected null pointer 'SEQB' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "SEQB");
    
#pragma ACCEL debug fflush(stdout);
  }
}

void __merlin_read_buffer_needwun_kernel(char *SEQA,char *SEQB,char *alignedA,char *alignedB)
{
  static char __m_SEQA[131072];
  static char __m_SEQB[131072];
  static char __m_alignedA[262144];
  static char __m_alignedB[262144];
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "alignedA", "262144 * sizeof(char )");
    
#pragma ACCEL debug fflush(stdout);
    if (alignedA != 0) {
      __CMOST_COMMENT__ __needwun_kernel__alignedA_buffer;
      __CMOST_COMMENT__ __event___needwun_kernel__alignedA_buffer;
// __MERLIN_EVENT_READ__ __event___needwun_kernel__alignedA_buffer
      int __MERLIN_EVENT_READ____event___needwun_kernel__alignedA_buffer;
      opencl_read_buffer_nb(__needwun_kernel__alignedA_buffer,commands[0],0 * sizeof(char ),(char *)alignedA,((unsigned long )262144) * sizeof(char ),&__event___needwun_kernel__alignedA_buffer);
    }
     else {
      printf("Error! Detected null pointer 'alignedA' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "alignedA");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "alignedB", "262144 * sizeof(char )");
    
#pragma ACCEL debug fflush(stdout);
    if (alignedB != 0) {
      __CMOST_COMMENT__ __needwun_kernel__alignedB_buffer;
      __CMOST_COMMENT__ __event___needwun_kernel__alignedB_buffer;
// __MERLIN_EVENT_READ__ __event___needwun_kernel__alignedB_buffer
      int __MERLIN_EVENT_READ____event___needwun_kernel__alignedB_buffer;
      opencl_read_buffer_nb(__needwun_kernel__alignedB_buffer,commands[0],0 * sizeof(char ),(char *)alignedB,((unsigned long )262144) * sizeof(char ),&__event___needwun_kernel__alignedB_buffer);
    }
     else {
      printf("Error! Detected null pointer 'alignedB' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "alignedB");
    
#pragma ACCEL debug fflush(stdout);
  }
}

void __merlin_execute_needwun_kernel(char *SEQA,char *SEQB,char *alignedA,char *alignedB)
{
  int i;
{
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "needwun_kernel");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__needwun_kernel_kernel, 0, sizeof(opencl_mem), &__needwun_kernel__SEQA_buffer);
	opencl_set_kernel_arg(__needwun_kernel_kernel, 1, sizeof(opencl_mem), &__needwun_kernel__SEQB_buffer);
	opencl_set_kernel_arg(__needwun_kernel_kernel, 2, sizeof(opencl_mem), &__needwun_kernel__alignedA_buffer);
	opencl_set_kernel_arg(__needwun_kernel_kernel, 3, sizeof(opencl_mem), &__needwun_kernel__alignedB_buffer);
	#pragma ACCEL debug printf("[Merlin Info] Finish set kernel %s argument....\n", "needwun_kernel");
	#pragma ACCEL debug fflush(stdout);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start execute kernel %s....\n", "needwun_kernel");
	#pragma ACCEL debug fflush(stdout);
	size_t __gid[1];
	__gid[0] = 1;
	opencl_enqueue_kernel(__needwun_kernel_kernel, commands[0], 1, __gid, &__event_needwun_kernel);
}
}
