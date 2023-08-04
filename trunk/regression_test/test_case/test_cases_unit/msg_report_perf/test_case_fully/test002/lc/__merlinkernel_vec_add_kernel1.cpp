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

#include "merlin_type_define.h"
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_kernel_pragma();
// Original: #pragma ACCEL kernel
static int vec_add_kernel1_dummy_pos;
extern "C" { 

__kernel void vec_add_kernel1(int a[10000],int b[10000],int c[10000],int inc)
{
  
#pragma HLS INTERFACE m_axi port=a offset=slave depth=10000
  
#pragma HLS INTERFACE m_axi port=b offset=slave depth=10000
  
#pragma HLS INTERFACE m_axi port=c offset=slave depth=10000
  
#pragma HLS INTERFACE s_axilite port=a bundle=control
  
#pragma HLS INTERFACE s_axilite port=b bundle=control
  
#pragma HLS INTERFACE s_axilite port=c bundle=control
  
#pragma HLS INTERFACE s_axilite port=inc bundle=control
  
#pragma HLS INTERFACE s_axilite port=return bundle=control
  
#pragma ACCEL interface variable=c max_depth=10000 depth=10000
  
#pragma ACCEL interface variable=b max_depth=10000 depth=10000
  
#pragma ACCEL interface variable=a max_depth=10000 depth=10000
  int j;
  for (j = 0; j < 10000; j++) 
// Original: #pragma ACCEL PIPELINE AUTO
{
    
#pragma HLS pipeline
    c[j] = a[j] + b[j] + inc;
  }
}
}
