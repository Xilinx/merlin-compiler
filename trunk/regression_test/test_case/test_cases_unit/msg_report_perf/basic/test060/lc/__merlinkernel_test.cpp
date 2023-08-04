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

static void func1(int *b)
{
  for (int i = 0; i < 1024; i++) 
// Original: #pragma ACCEL PIPELINE AUTO
{
    
#pragma HLS pipeline
    b[i] = 1;
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

__kernel void test(int *b,int *c)
{
  
#pragma HLS INTERFACE m_axi port=b offset=slave depth=1024
  
#pragma HLS INTERFACE m_axi port=c offset=slave depth=1024
  
#pragma HLS INTERFACE s_axilite port=b bundle=control
  
#pragma HLS INTERFACE s_axilite port=c bundle=control
  
#pragma HLS INTERFACE s_axilite port=return bundle=control
  
#pragma ACCEL interface variable=b max_depth=1024 depth=1024
  
#pragma ACCEL interface variable=c max_depth=1024 depth=1024
  func1(b);
  func2(c);
  return ;
}
}
