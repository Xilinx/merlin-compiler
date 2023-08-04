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
#include "memcpy_512_1d.h"
#include<hls_stream.h>
#include "cmost.h"
#include "cmost.h"
static class hls::stream< int  > __merlinvec_add_kernel_a_ch[16];
static class hls::stream< int  > __merlinvec_add_kernel_b_ch[16];
static class hls::stream< int  > __merlinvec_add_kernel_c_ch[16];

void __merlinvec_add_kernel_a_streaming(class ap_uint< 512 > a[625])
{
  
#pragma HLS array_partition variable=__merlinvec_add_kernel_a_ch complete dim=1
  
#pragma HLS INLINE off
  long merlin_it_0;
  merlinL1:
  for (merlin_it_0 = 0L; merlin_it_0 < 625L; ++merlin_it_0) 
// Original: #pragma ACCEL pipeline
{
    
#pragma HLS pipeline
    int a_buf[16];
// Existing HLS partition: #pragma HLS array_partition variable=a_buf cyclic factor = 16 dim=1
    
#pragma HLS array_partition variable=a_buf complete dim=1
    memcpy_wide_bus_read_int_512(&a_buf[0],(class ap_uint< 512 > *)a,(::size_t )(merlin_it_0 * ((long )16) * ((long )4)),sizeof(int ) * ((unsigned long )16L),(::size_t )16L);
    long merlin_it_1;
    merlinL0:
    for (merlin_it_1 = 0L; merlin_it_1 < ((long )16); ++merlin_it_1) 
// Original: #pragma ACCEL parallel
{
      
#pragma HLS unroll
      int merlin_tmp_2;
      merlin_tmp_2 = a_buf[merlin_it_1];
      __merlinvec_add_kernel_a_ch[merlin_it_1] . write(merlin_tmp_2);
    }
  }
}

void __merlinvec_add_kernel_b_streaming(class ap_uint< 512 > b[625])
{
  
#pragma HLS array_partition variable=__merlinvec_add_kernel_b_ch complete dim=1
  
#pragma HLS INLINE off
  long merlin_it_0;
  merlinL3:
  for (merlin_it_0 = 0L; merlin_it_0 < 625L; ++merlin_it_0) 
// Original: #pragma ACCEL pipeline
{
    
#pragma HLS pipeline
    int b_buf[16];
// Existing HLS partition: #pragma HLS array_partition variable=b_buf cyclic factor = 16 dim=1
    
#pragma HLS array_partition variable=b_buf complete dim=1
    memcpy_wide_bus_read_int_512(&b_buf[0],(class ap_uint< 512 > *)b,(::size_t )(merlin_it_0 * ((long )16) * ((long )4)),sizeof(int ) * ((unsigned long )16L),(::size_t )16L);
    long merlin_it_1;
    merlinL2:
    for (merlin_it_1 = 0L; merlin_it_1 < ((long )16); ++merlin_it_1) 
// Original: #pragma ACCEL parallel
{
      
#pragma HLS unroll
      int merlin_tmp_2;
      merlin_tmp_2 = b_buf[merlin_it_1];
      __merlinvec_add_kernel_b_ch[merlin_it_1] . write(merlin_tmp_2);
    }
  }
}

void __merlinvec_add_kernel_c_streaming(class ap_uint< 512 > c[625])
{
  
#pragma HLS array_partition variable=__merlinvec_add_kernel_c_ch complete dim=1
  
#pragma HLS INLINE off
  long merlin_it_0;
  merlinL7:
  for (merlin_it_0 = 0L; merlin_it_0 < 625L; ++merlin_it_0) 
// Original: #pragma ACCEL pipeline
{
    
#pragma HLS dependence variable=c array inter false
    
#pragma HLS pipeline
    int c_buf[16];
    
#pragma HLS array_partition variable=c_buf complete dim=1
    long merlin_it_1;
    merlinL6:
    for (merlin_it_1 = 0L; merlin_it_1 < ((long )16); ++merlin_it_1) 
// Original: #pragma ACCEL parallel
{
      
#pragma HLS unroll
      int merlin_tmp_2;
      merlin_tmp_2 = __merlinvec_add_kernel_c_ch[merlin_it_1] . read();
      c_buf[merlin_it_1] = merlin_tmp_2;
    }
// Existing HLS partition: #pragma HLS array_partition variable=c_buf cyclic factor = 16 dim=1
    memcpy_wide_bus_write_int_512((class ap_uint< 512 > *)c,&c_buf[0],(::size_t )(((long )4) * (merlin_it_0 * ((long )16))),sizeof(int ) * ((unsigned long )16L),(::size_t )16L);
  }
}

void __merlinvec_add_kernel_computation(int a[10000],int b[10000],int c[10000],int inc)
{
  
#pragma HLS inline off
  int j;
  merlinL5:
  for (j = 0; j < 625; j++) 
// Original: #pragma ACCEL parallel factor=16
// Original: #pragma ACCEL PARALLEL FACTOR=16
// Original: #pragma ACCEL PIPELINE AUTO
{
    
#pragma HLS pipeline
    merlinL4:
    for (int j_sub = 0; j_sub < 16; ++j_sub) 
// Original: #pragma ACCEL parallel factor=16
// Original: #pragma ACCEL PARALLEL FACTOR=16
// Original: #pragma ACCEL PARALLEL
{
      
#pragma HLS unroll
      __merlinvec_add_kernel_c_ch[j_sub] . write((__merlinvec_add_kernel_a_ch[j_sub] . read() + __merlinvec_add_kernel_b_ch[j_sub] . read() + inc));
    }
  }
// Original pragma:  ACCEL PARALLEL FACTOR=16 
}
static int vec_add_kernel_dummy_pos;
extern "C" { 

__kernel void vec_add_kernel(class ap_uint< 512 > a[625],class ap_uint< 512 > b[625],class ap_uint< 512 > c[625],int inc)
{
  
#pragma HLS INTERFACE m_axi port=a offset=slave depth=625 bundle=merlin_gmem_vec_add_kernel_512_0
  
#pragma HLS INTERFACE m_axi port=b offset=slave depth=625 bundle=merlin_gmem_vec_add_kernel_512_1
  
#pragma HLS INTERFACE m_axi port=c offset=slave depth=625 bundle=merlin_gmem_vec_add_kernel_512_0
  
#pragma HLS INTERFACE s_axilite port=a bundle=control
  
#pragma HLS INTERFACE s_axilite port=b bundle=control
  
#pragma HLS INTERFACE s_axilite port=c bundle=control
  
#pragma HLS INTERFACE s_axilite port=inc bundle=control
  
#pragma HLS INTERFACE s_axilite port=return bundle=control
  
#pragma HLS DATA_PACK VARIABLE=c
  
#pragma HLS DATA_PACK VARIABLE=b
  
#pragma HLS DATA_PACK VARIABLE=a
  
#pragma ACCEL interface variable=a depth=10000 max_depth=10000 BURST_OFF EXPLICIT_BUNDLE BUS_BITWIDTH=512
  
#pragma ACCEL interface variable=b depth=10000 max_depth=10000 BURST_OFF EXPLICIT_BUNDLE BUS_BITWIDTH=512
  
#pragma ACCEl interface variable=c depth=10000 max_depth=10000 BURST_OFF EXPLICIT_BUNDLE BUS_BITWIDTH=512
// Original pragma:  ACCEL PARALLEL FACTOR=16 
  
#pragma HLS dataflow
  
#pragma HLS stream variable=__merlinvec_add_kernel_a_ch
  __merlinvec_add_kernel_a_streaming(a);
  
#pragma HLS stream variable=__merlinvec_add_kernel_b_ch
  __merlinvec_add_kernel_b_streaming(b);
  __merlinvec_add_kernel_computation(0,0,0,inc);
  
#pragma HLS stream variable=__merlinvec_add_kernel_c_ch
  __merlinvec_add_kernel_c_streaming(c);
}
}
