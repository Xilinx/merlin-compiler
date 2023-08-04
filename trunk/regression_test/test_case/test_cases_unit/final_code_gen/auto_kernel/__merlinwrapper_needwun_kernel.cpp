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
{
    
#pragma ACCEL wrapper variable=SEQA port=SEQA depth=131072 max_depth=131072 data_type=char io=RO copy=false
    
#pragma ACCEL wrapper variable=SEQB port=SEQB depth=131072 max_depth=131072 data_type=char io=RO copy=false
    
#pragma ACCEL wrapper variable=alignedA port=alignedA depth=262144 max_depth=262144 data_type=char io=WO copy=false
    
#pragma ACCEL wrapper variable=alignedB port=alignedB depth=262144 max_depth=262144 data_type=char io=WO copy=false
  }
  static char __m_SEQA[131072];
  static char __m_SEQB[131072];
  static char __m_alignedA[262144];
  static char __m_alignedB[262144];
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
        memcpy((void *)(__m_SEQA + 0),(const void *)SEQA,131072 * sizeof(char ));
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
        memcpy((void *)(__m_SEQB + 0),(const void *)SEQB,131072 * sizeof(char ));
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
  needwun_kernel(__m_SEQA,__m_SEQB,__m_alignedA,__m_alignedB);
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
        memcpy((void *)alignedA,(const void *)(__m_alignedA + 0),262144 * sizeof(char ));
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
        memcpy((void *)alignedB,(const void *)(__m_alignedB + 0),262144 * sizeof(char ));
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
      memcpy((void *)(__m_SEQA + 0),(const void *)SEQA,131072 * sizeof(char ));
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
      memcpy((void *)(__m_SEQB + 0),(const void *)SEQB,131072 * sizeof(char ));
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
      memcpy((void *)alignedA,(const void *)(__m_alignedA + 0),262144 * sizeof(char ));
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
      memcpy((void *)alignedB,(const void *)(__m_alignedB + 0),262144 * sizeof(char ));
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
}
