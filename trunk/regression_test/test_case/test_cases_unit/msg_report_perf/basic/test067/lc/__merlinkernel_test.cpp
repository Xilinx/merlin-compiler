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
#define __constant
#define __kernel
#define __global
#include "memcpy_512_1d.h"
#include <string.h> 

#include "merlin_type_define.h"
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();

static void func1(int *a,long *b)
{
  for (int i = 0; i < 4096; i++) 
// Original: #pragma ACCEL PIPELINE AUTO
{
    
#pragma HLS pipeline
    b[i] = ((long )a[i]);
  }
}

static void func2(int *c)
{
  for (int i = 0; i < 1024; i++) 
// Original: #pragma ACCEL PIPELINE AUTO
{
    
#pragma HLS pipeline
    c[i] = 1;
  }
}
static void __merlin_dummy_kernel_pragma();
// Original: #pragma ACCEL kernel
static int test_dummy_pos;
extern "C" { 

__kernel void test(merlin_uint_512 *a,merlin_uint_512 *b,int *c)
{
  
#pragma HLS INTERFACE m_axi port=a offset=slave depth=256
  
#pragma HLS INTERFACE m_axi port=b offset=slave depth=128
  
#pragma HLS INTERFACE m_axi port=c offset=slave depth=1024
  
#pragma HLS INTERFACE s_axilite port=a bundle=control
  
#pragma HLS INTERFACE s_axilite port=b bundle=control
  
#pragma HLS INTERFACE s_axilite port=c bundle=control
  
#pragma HLS INTERFACE s_axilite port=return bundle=control
  
#pragma HLS DATA_PACK VARIABLE=b
  
#pragma HLS DATA_PACK VARIABLE=a
  int a_buf_0[4096];
  
#pragma HLS array_partition variable=a_buf_0 cyclic factor=8 dim=1
  long b_buf_0[1024];
// Existing HLS partition: #pragma HLS array_partition variable=a_buf_0 cyclic factor = 16 dim=1
  
#pragma HLS array_partition variable=b_buf_0 cyclic factor=4 dim=1
  memcpy_wide_bus_read_int_512(&a_buf_0[0],(merlin_uint_512 *)a,(0 * 4),sizeof(int ) * ((unsigned long )4096),4096LL);
  
#pragma ACCEL interface variable=a max_depth=4096 depth=4096
  
#pragma ACCEL interface variable=b max_depth=1024 depth=1024
  
#pragma ACCEL interface variable=c memory_burst=off max_depth=1024 depth=1024
  func1((int *)a_buf_0,(long *)b_buf_0);
  func2(c);
// Existing HLS partition: #pragma HLS array_partition variable=b_buf_0 cyclic factor = 8 dim=1
  memcpy_wide_bus_write_long_512((merlin_uint_512 *)b,&b_buf_0[0],(8 * 0),sizeof(long ) * ((unsigned long )1024),1024LL);
  return ;
}
}
