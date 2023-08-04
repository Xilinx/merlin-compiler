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
#include <math.h>
#include "merlin_type_define.h"
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_893();
void vec_add(int s,int a[1024],int b[1024],int c[1024]);
static void __merlin_dummy_extern_int_merlin_include_L_();
#include "__merlinhead_kernel_top.h"
static void __merlin_dummy_extern_int_merlin_include_L_();

void __merlinwrapper_vec_add(int s,int a[1024],int b[1024],int c[1024])
{
{
    
#pragma ACCEL wrapper variable=s port=s
    
#pragma ACCEL wrapper variable=a port=a depth=1024 max_depth=1024 data_type=int io=RO copy=false
    
#pragma ACCEL wrapper variable=b port=b depth=1024 max_depth=1024 data_type=int io=RO copy=false
    
#pragma ACCEL wrapper variable=c port=c depth=1024 max_depth=1024 data_type=int io=WO copy=false
  }
  static int __m_a[1024];
  static int __m_b[1024];
  static int __m_c[1024];
{
/*
    static int __m_a[1024];
    static int __m_b[1024];
    static int __m_c[1024];
*/
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "a", "1024 * sizeof(int )");
      
#pragma ACCEL debug fflush(stdout);
      if (a != 0) {
        memcpy((void *)(__m_a + 0),(const void *)a,1024 * sizeof(int ));
      }
       else {
        printf("Error! The external memory pointed by 'a' is invalid.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "a");
      
#pragma ACCEL debug fflush(stdout);
    }
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "b", "1024 * sizeof(int )");
      
#pragma ACCEL debug fflush(stdout);
      if (b != 0) {
        memcpy((void *)(__m_b + 0),(const void *)b,1024 * sizeof(int ));
      }
       else {
        printf("Error! The external memory pointed by 'b' is invalid.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "b");
      
#pragma ACCEL debug fflush(stdout);
    }
  }
  
#pragma ACCEL kernel
  vec_add(s,__m_a,__m_b,__m_c);
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "c", "1024 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (c != 0) {
      memcpy((void *)c,(const void *)(__m_c + 0),1024 * sizeof(int ));
    }
     else {
      printf("Error! The external memory pointed by 'c' is invalid.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "c");
    
#pragma ACCEL debug fflush(stdout);
  }
}

void __merlin_vec_add(int s,int a[1024],int b[1024],int c[1024])
{
  
#pragma ACCEL string __merlin_check_opencl();
  
#pragma ACCEL string __merlin_init_vec_add();
  __merlinwrapper_vec_add(s,a,b,c);
  
#pragma ACCEL string __merlin_release_vec_add();
}

void __merlin_write_buffer_vec_add(int s,int a[1024],int b[1024],int c[1024])
{
  static int __m_a[1024];
  static int __m_b[1024];
  static int __m_c[1024];
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "a", "1024 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (a != 0) {
      memcpy((void *)(__m_a + 0),(const void *)a,1024 * sizeof(int ));
    }
     else {
      printf("Error! The external memory pointed by 'a' is invalid.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "a");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "b", "1024 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (b != 0) {
      memcpy((void *)(__m_b + 0),(const void *)b,1024 * sizeof(int ));
    }
     else {
      printf("Error! The external memory pointed by 'b' is invalid.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "b");
    
#pragma ACCEL debug fflush(stdout);
  }
}

void __merlin_read_buffer_vec_add(int s,int a[1024],int b[1024],int c[1024])
{
  static int __m_a[1024];
  static int __m_b[1024];
  static int __m_c[1024];
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "c", "1024 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (c != 0) {
      memcpy((void *)c,(const void *)(__m_c + 0),1024 * sizeof(int ));
    }
     else {
      printf("Error! The external memory pointed by 'c' is invalid.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "c");
    
#pragma ACCEL debug fflush(stdout);
  }
}

void __merlin_execute_vec_add(int s,int a[1024],int b[1024],int c[1024])
{
}
