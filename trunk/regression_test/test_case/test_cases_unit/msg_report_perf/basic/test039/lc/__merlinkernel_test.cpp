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
#include <string.h> 

#include "merlin_type_define.h"
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();

static void func1(char *a,int *b,long *c)
{
  for (int i = 0; i < 64; i++) 
// Original: #pragma ACCEL PIPELINE AUTO
{
    
#pragma HLS pipeline
    c[i] = ((long )(((int )a[i]) + b[i]));
  }
  return ;
}

static void func2(char *a,int *b,long *c)
{
  for (int i = 0; i < 32; i++) 
// Original: #pragma ACCEL PIPELINE AUTO
{
    
#pragma HLS pipeline
    c[i] = ((long )(((int )a[i]) + b[i]));
  }
  return ;
}
static void __merlin_dummy_kernel_pragma();
// Original: #pragma ACCEL kernel
static int test_dummy_pos;

static void merlin_memcpy_0(long dst[64],int dst_idx_0,long *src,int src_idx_0,unsigned int len,unsigned int max_len)
{
  
#pragma HLS inline off
  
#pragma HLS function_instantiate variable=dst_idx_0,src_idx_0
  long long i;
  long long total_offset1 = 0 * 64 + dst_idx_0;
  long long total_offset2 = 0 * 0 + src_idx_0;
  for (i = 0; i < len / 8; ++i) {
    
#pragma HLS PIPELINE II=1
    
#pragma HLS LOOP_TRIPCOUNT max=64
    dst[total_offset1 + i] = src[total_offset2 + i];
  }
}

static void merlin_memcpy_1(char dst[64],int dst_idx_0,char *src,int src_idx_0,unsigned int len,unsigned int max_len)
{
  
#pragma HLS inline off
  
#pragma HLS function_instantiate variable=dst_idx_0,src_idx_0
  long long i;
  long long total_offset1 = 0 * 64 + dst_idx_0;
  long long total_offset2 = 0 * 0 + src_idx_0;
  for (i = 0; i < len / 1; ++i) {
    
#pragma HLS PIPELINE II=1
    
#pragma HLS LOOP_TRIPCOUNT max=64
    dst[total_offset1 + i] = src[total_offset2 + i];
  }
}

static void merlin_memcpy_2(int dst[64],int dst_idx_0,int *src,int src_idx_0,unsigned int len,unsigned int max_len)
{
  
#pragma HLS inline off
  
#pragma HLS function_instantiate variable=dst_idx_0,src_idx_0
  long long i;
  long long total_offset1 = 0 * 64 + dst_idx_0;
  long long total_offset2 = 0 * 0 + src_idx_0;
  for (i = 0; i < len / 4; ++i) {
    
#pragma HLS PIPELINE II=1
    
#pragma HLS LOOP_TRIPCOUNT max=64
    dst[total_offset1 + i] = src[total_offset2 + i];
  }
}

static void merlin_memcpy_3(long *dst,int dst_idx_0,long src[64],int src_idx_0,unsigned int len,unsigned int max_len)
{
  
#pragma HLS inline off
  
#pragma HLS function_instantiate variable=dst_idx_0,src_idx_0
  long long i;
  long long total_offset1 = 0 * 0 + dst_idx_0;
  long long total_offset2 = 0 * 64 + src_idx_0;
  for (i = 0; i < len / 8; ++i) {
    
#pragma HLS PIPELINE II=1
    
#pragma HLS LOOP_TRIPCOUNT max=64
    dst[total_offset1 + i] = src[total_offset2 + i];
  }
}
extern "C" { 

__kernel void test(char *a,int *b,long *c,int m)
{
  
#pragma HLS INTERFACE m_axi port=a offset=slave depth=128
  
#pragma HLS INTERFACE m_axi port=b offset=slave depth=128
  
#pragma HLS INTERFACE m_axi port=c offset=slave depth=128
  
#pragma HLS INTERFACE s_axilite port=a bundle=control
  
#pragma HLS INTERFACE s_axilite port=b bundle=control
  
#pragma HLS INTERFACE s_axilite port=c bundle=control
  
#pragma HLS INTERFACE s_axilite port=m bundle=control
  
#pragma HLS INTERFACE s_axilite port=return bundle=control
  
#pragma ACCEL interface variable=a max_depth=128 depth=128
  
#pragma ACCEL interface variable=b max_depth=128 depth=128
  
#pragma ACCEL interface variable=c max_depth=128 depth=128
  if (m == 0) {
  }
   else {
    int b_buf_0[64];
    char a_buf_0[64];
    long c_buf_0[64];
    merlin_memcpy_0(c_buf_0,0,c,0,sizeof(long ) * ((unsigned long )64),512UL);
    merlin_memcpy_1(a_buf_0,0,a,0,sizeof(char ) * ((unsigned long )64),64UL);
    merlin_memcpy_2(b_buf_0,0,b,0,sizeof(int ) * ((unsigned long )64),256UL);
    if (m > 0) {
      func1((char *)a_buf_0,(int *)b_buf_0,(long *)c_buf_0);
    }
     else {
      func2((char *)a_buf_0,(int *)b_buf_0,(long *)c_buf_0);
    }
    merlin_memcpy_3(c,0,c_buf_0,0,sizeof(long ) * ((unsigned long )64),512UL);
  }
  return ;
}
}
