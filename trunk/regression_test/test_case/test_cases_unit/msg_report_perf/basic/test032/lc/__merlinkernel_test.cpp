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

static void merlin_memcpy_0(char dst[128],int dst_idx_0,char *src,int src_idx_0,unsigned int len,unsigned int max_len)
{
  
#pragma HLS inline off
  
#pragma HLS function_instantiate variable=dst_idx_0,src_idx_0
  long long i;
  long long total_offset1 = 0 * 128 + dst_idx_0;
  long long total_offset2 = 0 * 0 + src_idx_0;
  for (i = 0; i < len / 1; ++i) {
    
#pragma HLS PIPELINE II=1
    
#pragma HLS LOOP_TRIPCOUNT max=128
    dst[total_offset1 + i] = src[total_offset2 + i];
  }
}

static void merlin_memcpy_1(int dst[128],int dst_idx_0,int *src,int src_idx_0,unsigned int len,unsigned int max_len)
{
  
#pragma HLS inline off
  
#pragma HLS function_instantiate variable=dst_idx_0,src_idx_0
  long long i;
  long long total_offset1 = 0 * 128 + dst_idx_0;
  long long total_offset2 = 0 * 0 + src_idx_0;
  for (i = 0; i < len / 4; ++i) {
    
#pragma HLS PIPELINE II=1
    
#pragma HLS LOOP_TRIPCOUNT max=128
    dst[total_offset1 + i] = src[total_offset2 + i];
  }
}

static void func(char *a,int *b,merlin_uint_512 *c)
{
  int b_buf_0[128];
  char a_buf_0[128];
  long c_buf_0[128];
  
#pragma HLS array_partition variable=c_buf_0 cyclic factor=4 dim=1
  merlin_memcpy_0(a_buf_0,0,a,0,sizeof(char ) * ((unsigned long )128),128UL);
  merlin_memcpy_1(b_buf_0,0,b,0,sizeof(int ) * ((unsigned long )128),512UL);
  for (int i = 0; i < 128; i++) 
// Original: #pragma ACCEL PIPELINE AUTO
{
    
#pragma HLS pipeline
    c_buf_0[i] = ((long )(((int )a_buf_0[i]) + b_buf_0[i]));
  }
// Existing HLS partition: #pragma HLS array_partition variable=c_buf_0 cyclic factor = 8 dim=1
  memcpy_wide_bus_write_long_512((merlin_uint_512 *)c,&c_buf_0[0],(8 * 0),sizeof(long ) * ((unsigned long )128),128LL);
  return ;
}
static void __merlin_dummy_kernel_pragma();
// Original: #pragma ACCEL kernel
static int test_dummy_pos;
extern "C" { 

__kernel void test(char *a,int *b,merlin_uint_512 *c)
{
  
#pragma HLS INTERFACE m_axi port=a offset=slave depth=8192
  
#pragma HLS INTERFACE m_axi port=b offset=slave depth=8192
  
#pragma HLS INTERFACE m_axi port=c offset=slave depth=1024
  
#pragma HLS INTERFACE s_axilite port=a bundle=control
  
#pragma HLS INTERFACE s_axilite port=b bundle=control
  
#pragma HLS INTERFACE s_axilite port=c bundle=control
  
#pragma HLS INTERFACE s_axilite port=return bundle=control
  
#pragma HLS DATA_PACK VARIABLE=c
  
#pragma ACCEL interface variable=a max_depth=8192 depth=8192
  
#pragma ACCEL interface variable=b max_depth=8192 depth=8192
  
#pragma ACCEL interface variable=c max_depth=8192 depth=8192
  func(a,b,c);
  func(a,b,c);
  return ;
}
}
