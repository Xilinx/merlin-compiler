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
#include "cmost.h"
#include "merlin_type_define.h"
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_kernel_pragma();
/* Original: #pragma ACCEL kernel */

__kernel void vec_add_kernel(int *a,int *b,int *c,int inc,int *d,long *e,int *f,long *g,int *h)
{
  
#pragma HLS INTERFACE m_axi port=a offset=slave bundle=merlin_gmem_vec_add_kernel_a depth=10000
  
#pragma HLS INTERFACE m_axi port=b offset=slave bundle=merlin_gmem_vec_add_kernel_b depth=10000
  
#pragma HLS INTERFACE m_axi port=c offset=slave bundle=merlin_gmem_vec_add_kernel_c depth=10000
  
#pragma HLS INTERFACE m_axi port=d offset=slave bundle=merlin_gmem_vec_add_kernel_d 
  
#pragma HLS INTERFACE m_axi port=e offset=slave bundle=merlin_gmem_vec_add_kernel_e 
  
#pragma HLS INTERFACE m_axi port=f offset=slave bundle=merlin_gmem_vec_add_kernel_f 
  
#pragma HLS INTERFACE m_axi port=g offset=slave bundle=merlin_gmem_vec_add_kernel_g 
  
#pragma HLS INTERFACE m_axi port=h offset=slave bundle=merlin_gmem_vec_add_kernel_h 
  
#pragma HLS INTERFACE s_axilite port=a bundle=control
  
#pragma HLS INTERFACE s_axilite port=b bundle=control
  
#pragma HLS INTERFACE s_axilite port=c bundle=control
  
#pragma HLS INTERFACE s_axilite port=d bundle=control
  
#pragma HLS INTERFACE s_axilite port=e bundle=control
  
#pragma HLS INTERFACE s_axilite port=f bundle=control
  
#pragma HLS INTERFACE s_axilite port=g bundle=control
  
#pragma HLS INTERFACE s_axilite port=h bundle=control
  
#pragma HLS INTERFACE s_axilite port=inc bundle=control
  
#pragma HLS INTERFACE s_axilite port=return bundle=control
  d[0] = a[0];
  e[0] = a[0];
  c[0] = f[1];
  c[1] = g[2];
  h[0] = a[0];
  int b_buf_0[10000];
  long _memcpy_i_1;
  
#pragma unroll 16
  merlinL3:
  for (_memcpy_i_1 = ((long )0); ((unsigned long )_memcpy_i_1) < sizeof(int ) * ((unsigned long )10000) / ((unsigned long )4); ++_memcpy_i_1) {
    long total_offset1 = (long )(0 * 10000 + 0);
    long total_offset2 = (long )(0 * 0 + 0);
    b_buf_0[total_offset1 + _memcpy_i_1] = b[total_offset2 + _memcpy_i_1];
  }
  int a_buf_0[10000];
  long _memcpy_i_0;
  
#pragma unroll 16
  merlinL2:
  for (_memcpy_i_0 = ((long )0); ((unsigned long )_memcpy_i_0) < sizeof(int ) * ((unsigned long )10000) / ((unsigned long )4); ++_memcpy_i_0) {
    long total_offset1 = (long )(0 * 10000 + 0);
    long total_offset2 = (long )(0 * 0 + 0);
    a_buf_0[total_offset1 + _memcpy_i_0] = a[total_offset2 + _memcpy_i_0];
  }
  int c_buf_0[10000];
  
#pragma ACCEL interface variable=a max_depth=10000 depth=10000
  
#pragma ACCEL interface variable=b max_depth=10000 depth=10000
  
#pragma ACCEL interface variable=c max_depth=10000 depth=10000
  int j;
  merlinL1:
  for (j = 0; j < 10000; j++) {
    c_buf_0[j] = a_buf_0[j] + b_buf_0[j] + inc;
  }
  long _memcpy_i_2;
  
#pragma unroll 16
  merlinL0:
  for (_memcpy_i_2 = ((long )0); ((unsigned long )_memcpy_i_2) < sizeof(int ) * ((unsigned long )10000) / ((unsigned long )4); ++_memcpy_i_2) {
    long total_offset1 = (long )(0 * 0 + 0);
    long total_offset2 = (long )(0 * 10000 + 0);
    c[total_offset1 + _memcpy_i_2] = c_buf_0[total_offset2 + _memcpy_i_2];
  }
}
