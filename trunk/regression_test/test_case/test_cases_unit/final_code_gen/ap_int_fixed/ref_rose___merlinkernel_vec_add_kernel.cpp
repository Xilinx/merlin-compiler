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
extern int __merlin_include__GB_string_h_0;
#include<ap_int.h>
static int top_dummy_pos;

static void merlin_memcpy_0(class ap_ufixed< 10 , 20 , AP_TRN , AP_WRAP , 0 > *dst,int dst_idx_0,class ap_ufixed< 10 , 20 , AP_TRN , AP_WRAP , 0 > src[100],int src_idx_0,unsigned int len,unsigned int max_len)
{
  
#pragma HLS inline off
  
#pragma HLS function_instantiate variable=dst_idx_0,src_idx_0
  unsigned long i;
  unsigned long total_offset1 = 0 * 0 + dst_idx_0;
  unsigned long total_offset2 = 0 * 100 + src_idx_0;
  merlinL0:
  for (i = 0; i < len / 2; ++i) {
    
#pragma HLS PIPELINE II=1
    
#pragma HLS LOOP_TRIPCOUNT max=100
    dst[total_offset1 + i] = src[total_offset2 + i];
  }
}

static void merlin_memcpy_1(class ap_fixed< 10 , 20 , AP_TRN , AP_WRAP , 0 > *dst,int dst_idx_0,class ap_fixed< 10 , 20 , AP_TRN , AP_WRAP , 0 > src[100],int src_idx_0,unsigned int len,unsigned int max_len)
{
  
#pragma HLS inline off
  
#pragma HLS function_instantiate variable=dst_idx_0,src_idx_0
  unsigned long i;
  unsigned long total_offset1 = 0 * 0 + dst_idx_0;
  unsigned long total_offset2 = 0 * 100 + src_idx_0;
  merlinL1:
  for (i = 0; i < len / 2; ++i) {
    
#pragma HLS PIPELINE II=1
    
#pragma HLS LOOP_TRIPCOUNT max=100
    dst[total_offset1 + i] = src[total_offset2 + i];
  }
}

static void merlin_memcpy_2(class ap_uint< 10 > *dst,int dst_idx_0,class ap_uint< 10 > src[100],int src_idx_0,unsigned int len,unsigned int max_len)
{
  
#pragma HLS inline off
  
#pragma HLS function_instantiate variable=dst_idx_0,src_idx_0
  unsigned long i;
  unsigned long total_offset1 = 0 * 0 + dst_idx_0;
  unsigned long total_offset2 = 0 * 100 + src_idx_0;
  merlinL2:
  for (i = 0; i < len / 2; ++i) {
    
#pragma HLS PIPELINE II=1
    
#pragma HLS LOOP_TRIPCOUNT max=100
    dst[total_offset1 + i] = src[total_offset2 + i];
  }
}

static void merlin_memcpy_3(class ap_int< 10 > *dst,int dst_idx_0,class ap_int< 10 > src[100],int src_idx_0,unsigned int len,unsigned int max_len)
{
  
#pragma HLS inline off
  
#pragma HLS function_instantiate variable=dst_idx_0,src_idx_0
  unsigned long i;
  unsigned long total_offset1 = 0 * 0 + dst_idx_0;
  unsigned long total_offset2 = 0 * 100 + src_idx_0;
  merlinL3:
  for (i = 0; i < len / 2; ++i) {
    
#pragma HLS PIPELINE II=1
    
#pragma HLS LOOP_TRIPCOUNT max=100
    dst[total_offset1 + i] = src[total_offset2 + i];
  }
}
#pragma ACCEL kernel
extern "C" { 

__kernel void top(class ap_int< 10 > *a,class ap_uint< 10 > *b,class ap_fixed< 10 , 20 , AP_TRN , AP_WRAP , 0 > *c,class ap_ufixed< 10 , 20 , AP_TRN , AP_WRAP , 0 > *d)
{
  
#pragma HLS INTERFACE m_axi port=a offset=slave  bundle=merlin_gmem_top_10_0
  
#pragma HLS INTERFACE m_axi port=b offset=slave  bundle=merlin_gmem_top_10_1
  
#pragma HLS INTERFACE m_axi port=c offset=slave  bundle=merlin_gmem_top_10_2
  
#pragma HLS INTERFACE m_axi port=d offset=slave  bundle=merlin_gmem_top_10_3
  
#pragma HLS INTERFACE s_axilite port=a bundle=control
  
#pragma HLS INTERFACE s_axilite port=b bundle=control
  
#pragma HLS INTERFACE s_axilite port=c bundle=control
  
#pragma HLS INTERFACE s_axilite port=d bundle=control
  
#pragma HLS INTERFACE s_axilite port=return bundle=control
  class ap_ufixed< 10 , 20 , AP_TRN , AP_WRAP , 0 > d_buf_0[100];
  class ap_fixed< 10 , 20 , AP_TRN , AP_WRAP , 0 > c_buf_0[100];
  class ap_uint< 10 > b_buf_0[100];
  class ap_int< 10 > a_buf_0[100];
  merlinL4:
  for (int i = 0; i < 100; ++i) {
    a_buf_0[i] = i;
    b_buf_0[i] = i;
    c_buf_0[i] = i;
    d_buf_0[i] = i;
  }
  merlin_memcpy_0(d,0,d_buf_0,0,sizeof(class ap_ufixed< 10 , 20 , AP_TRN , AP_WRAP , 0 > ) * ((unsigned long )100),200UL);
  merlin_memcpy_1(c,0,c_buf_0,0,sizeof(class ap_fixed< 10 , 20 , AP_TRN , AP_WRAP , 0 > ) * ((unsigned long )100),200UL);
  merlin_memcpy_2(b,0,b_buf_0,0,sizeof(class ap_uint< 10 > ) * ((unsigned long )100),200UL);
  merlin_memcpy_3(a,0,a_buf_0,0,sizeof(class ap_int< 10 > ) * ((unsigned long )100),200UL);
}
}

void __merlinwrapper_top(class ap_int< 10 > *a,class ap_uint< 10 > *b,class ap_fixed< 10 , 20 , AP_TRN , AP_WRAP , 0 > *c,class ap_ufixed< 10 , 20 , AP_TRN , AP_WRAP , 0 > *d)
{
  top(a,b,c,d);
}
