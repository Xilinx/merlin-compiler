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
#include "memcpy_512.h"
#include <string.h> 
#include <stdio.h> 
#include <stdlib.h> 
void vec_add_kernel(class ap_uint< 512 > a[625],class ap_uint< 512 > b[625],class ap_uint< 512 > c[625],int inc);
#include "__merlinhead_kernel_top.h"

void __merlinwrapper_vec_add_kernel(int a[10000],int b[10000],int c[10000],int inc) {
#pragma ACCEL wrapper VARIABLE=a port=a data_type=int depth=10000 max_depth=10000 io=RO copy=false
#pragma ACCEL wrapper VARIABLE=b port=b data_type=int depth=10000 max_depth=10000 io=RO copy=false
#pragma ACCEL wrapper VARIABLE=c port=c data_type=int depth=10000 max_depth=10000 io=WO copy=false
#pragma ACCEL wrapper VARIABLE=inc port=inc data_type=int
  static int __m_a[10000];
  static int __m_b[10000];
  static int __m_c[10000];
  if (a != 0) {
    memcpy((void *)(__m_a + 0),(const void *)a,10000 * sizeof(int ));
  } else {
    printf("Error! Detected null pointer 'a' for external memory.\n");
    exit(1);
  }
  if (b != 0) {
    memcpy((void *)(__m_b + 0),(const void *)b,10000 * sizeof(int ));
  } else {
    printf("Error! Detected null pointer 'b' for external memory.\n");
    exit(1);
  }
  
  #pragma ACCEL kernel
  vec_add_kernel((class ap_uint< 512 > *)__m_a,(class ap_uint< 512 > *)__m_b,(class ap_uint< 512 > *)__m_c,inc);
  if (c != ((int *)0)) {
    memcpy((void *)c,(const void *)(__m_c + 0),((unsigned long )10000) * sizeof(int ));
  } else {
    printf("Error! Detected null pointer 'c' for external memory.\n");
    exit(1);
  }
}

void __merlin_vec_add_kernel(int a[10000],int b[10000],int c[10000],int inc)
{
  
#pragma ACCEL string __merlin_check_opencl();
  
#pragma ACCEL string __merlin_init_vec_add_kernel();
  __merlinwrapper_vec_add_kernel(a,b,c,inc);
  
#pragma ACCEL string __merlin_release_vec_add_kernel();
}

void __merlin_write_buffer_vec_add_kernel(int a[10000],int b[10000],int c[10000],int inc)
{
  static int __m_a[10000];
  static int __m_b[10000];
  static int __m_c[10000];
{
    if (a != 0) {
      memcpy((void *)(__m_a + 0),(const void *)a,10000 * sizeof(int ));
    }
     else {
      printf("Error! Detected null pointer 'a' for external memory.\n");
      exit(1);
    }
  }
{
    if (b != 0) {
      memcpy((void *)(__m_b + 0),(const void *)b,10000 * sizeof(int ));
    }
     else {
      printf("Error! Detected null pointer 'b' for external memory.\n");
      exit(1);
    }
  }
}

void __merlin_read_buffer_vec_add_kernel(int a[10000],int b[10000],int c[10000],int inc)
{
  static int __m_a[10000];
  static int __m_b[10000];
  static int __m_c[10000];
{
    if (c != 0) {
      memcpy((void *)c,(const void *)(__m_c + 0),10000 * sizeof(int ));
    }
     else {
      printf("Error! Detected null pointer 'c' for external memory.\n");
      exit(1);
    }
  }
}

void __merlin_execute_vec_add_kernel(int a[10000],int b[10000],int c[10000],int inc)
{
}
