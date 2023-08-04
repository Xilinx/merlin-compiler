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
void vec_add(int a[1024],int b[1024]);
static void __merlin_dummy_extern_int_merlin_include_L_();
#include "__merlinhead_kernel_top.h"
static void __merlin_dummy_extern_int_merlin_include_L_();

void __merlinwrapper_vec_add(int a[1024],int b[1024])
{
{
    
#pragma ACCEL wrapper variable=a port=a depth=1024 max_depth=1024 data_type=int io=WO copy=false
    
#pragma ACCEL wrapper variable=b port=b depth=1024 max_depth=1024 data_type=int io=RO copy=false
  }
  static int __m_a[1024];
  static int __m_b[1024];
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "b", "1024 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (b != 0) {
      memcpy((void *)(__m_b + 0),(const void *)b,1024 * sizeof(int ));
    }
     else {
      printf("Error! Detected null pointer 'b' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "b");
    
#pragma ACCEL debug fflush(stdout);
  }
  
#pragma ACCEL kernel
  vec_add(__m_a,__m_b);
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "a", "1024 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (a != 0) {
      memcpy((void *)a,(const void *)(__m_a + 0),1024 * sizeof(int ));
    }
     else {
      printf("Error! Detected null pointer 'a' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "a");
    
#pragma ACCEL debug fflush(stdout);
  }
}

void __merlin_vec_add(int a[1024],int b[1024])
{
  
#pragma ACCEL string __merlin_check_opencl();
  
#pragma ACCEL string __merlin_init_vec_add();
  __merlinwrapper_vec_add(a,b);
  
#pragma ACCEL string __merlin_release_vec_add();
}

void __merlin_write_buffer_vec_add(int a[1024],int b[1024])
{
  static int __m_a[1024];
  static int __m_b[1024];
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "b", "1024 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (b != 0) {
      memcpy((void *)(__m_b + 0),(const void *)b,1024 * sizeof(int ));
    }
     else {
      printf("Error! Detected null pointer 'b' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "b");
    
#pragma ACCEL debug fflush(stdout);
  }
}

void __merlin_read_buffer_vec_add(int a[1024],int b[1024])
{
  static int __m_a[1024];
  static int __m_b[1024];
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "a", "1024 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (a != 0) {
      memcpy((void *)a,(const void *)(__m_a + 0),1024 * sizeof(int ));
    }
     else {
      printf("Error! Detected null pointer 'a' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "a");
    
#pragma ACCEL debug fflush(stdout);
  }
}

void __merlin_execute_vec_add(int a[1024],int b[1024])
{
}
