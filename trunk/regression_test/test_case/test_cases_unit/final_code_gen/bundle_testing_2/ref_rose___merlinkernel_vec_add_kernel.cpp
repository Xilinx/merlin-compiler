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
#include "merlin_type_define.h"
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_kernel_pragma();
// Original: #pragma ACCEL kernel
static int vec_add_kernel_dummy_pos;
extern "C" { 

__kernel void vec_add_kernel(int a[200000],int b[200000],int c[200000],int d[200000],long e[200000],int f[200000],int g[200000],long h[200000],long i[200000])
{
  
#pragma HLS INTERFACE m_axi port=d offset=slave depth=200000 bundle=merlin_gmem_vec_add_kernel_32_00
  
#pragma HLS INTERFACE m_axi port=e offset=slave depth=200000 bundle=merlin_gmem_vec_add_kernel_64_00
  
#pragma HLS INTERFACE m_axi port=f offset=slave depth=200000 bundle=merlin_gmem_vec_add_kernel_32_00
  
#pragma HLS INTERFACE m_axi port=g offset=slave depth=200000 bundle=merlin_gmem_vec_add_kernel_32_1
  
#pragma HLS INTERFACE m_axi port=h offset=slave depth=200000 bundle=merlin_gmem_vec_add_kernel_64_1
  
#pragma HLS INTERFACE s_axilite port=d bundle=control
  
#pragma HLS INTERFACE s_axilite port=e bundle=control
  
#pragma HLS INTERFACE s_axilite port=f bundle=control
  
#pragma HLS INTERFACE s_axilite port=g bundle=control
  
#pragma HLS INTERFACE s_axilite port=h bundle=control
  
#pragma HLS INTERFACE s_axilite port=return bundle=control
  
#pragma HLS INTERFACE m_axi port=a offset=slave depth=200000 bundle=merlin_gmem_vec_add_kernel_32_0
  
#pragma HLS INTERFACE m_axi port=b offset=slave depth=200000 bundle=merlin_gmem_vec_add_kernel_32_0
  
#pragma HLS INTERFACE m_axi port=c offset=slave depth=200000 bundle=c0
  
#pragma HLS INTERFACE m_axi port=i offset=slave depth=200000 bundle=merlin_gmem_vec_add_kernel_64_0
  
#pragma ACCEL interface variable=i depth=200000 max_depth=200000
  
#pragma ACCEL interface variable=h depth=200000 max_depth=200000
  
#pragma ACCEL interface variable=g depth=200000 max_depth=200000
  
#pragma ACCEL interface variable=f depth=200000 max_depth=200000
  
#pragma ACCEL interface variable=e depth=200000 max_depth=200000
  
#pragma ACCEL interface variable=d depth=200000 max_depth=200000
  
#pragma ACCEL interface variable=c depth=200000 max_depth=200000
  
#pragma ACCEL interface variable=b depth=200000 max_depth=200000
  
#pragma ACCEL interface variable=a depth=200000 max_depth=200000
  b[0] = a[0];
  c[0] += a[0];
  d[0] = a[0];
  e[0] = ((long )a[0]);
  g[0] = f[0];
  h[0] = ((long )a[0]);
  h[1] = i[1];
}
}
