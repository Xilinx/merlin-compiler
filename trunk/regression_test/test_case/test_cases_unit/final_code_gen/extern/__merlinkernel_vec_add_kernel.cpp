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
#define SIZE_1 16
#include "memcpy_512_2d.h"
#undef SIZE_1
#include <string.h> 
#include "cmost.h"
#include "merlin_type_define.h"
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_kernel_pragma();
// Original: #pragma ACCEL kernel
#pragma ACCEL string 1234
void vec_add_kernel(class ap_uint< 512 > *a,class ap_uint< 512 > *b,class ap_uint< 512 > *c,int inc)
{
  int b_buf_0[12500][16];
// Existing HLS partition: #pragma HLS array_partition variable=b_buf_0 cyclic factor = 16 dim=2
  
#pragma HLS array_partition variable=b_buf_0 complete dim=2
  memcpy_wide_bus_read_int_2d_16_512(b_buf_0,0,0,((class ap_uint< 512 > *)b),(0 * 4),sizeof(int ) * ((unsigned long )200000),200000L);
  int a_buf_0[12500][16];
// Existing HLS partition: #pragma HLS array_partition variable=a_buf_0 cyclic factor = 16 dim=2
  
#pragma HLS array_partition variable=a_buf_0 complete dim=2
  memcpy_wide_bus_read_int_2d_16_512(a_buf_0,0,0,((class ap_uint< 512 > *)a),(0 * 4),sizeof(int ) * ((unsigned long )200000),200000L);
  int c_buf_0[12500][16];
  
#pragma HLS array_partition variable=c_buf_0 complete dim=2
  int j_sub;
  __merlin_access_range(c,0,199999UL);
  __merlin_access_range(b,0,199999UL);
  __merlin_access_range(a,0,199999UL);
  
#pragma ACCEL interface variable=c max_depth=200000 depth=200000
  
#pragma ACCEL interface variable=b max_depth=200000 depth=200000
  
#pragma ACCEL interface variable=a max_depth=200000 depth=200000
  int j;
// Original pramga:  ACCEL PARALLEL FACTOR=16 
  for (j = 0; j < 12500; j++) 
// Original: #pragma ACCEL parallel factor=16
// Original: #pragma ACCEL PARALLEL FACTOR=16
// Original: #pragma ACCEL PIPELINE AUTO
{
    
#pragma HLS pipeline
    for (j_sub = 0; j_sub < 16; ++j_sub) 
// Original: #pragma ACCEL parallel factor=16
// Original: #pragma ACCEL PARALLEL FACTOR=16
// Original: #pragma ACCEL PARALLEL
{
      
#pragma HLS unroll
      c_buf_0[j][j_sub] = a_buf_0[j][j_sub] + b_buf_0[j][j_sub] + inc;
    }
  }
// Existing HLS partition: #pragma HLS array_partition variable=c_buf_0 cyclic factor = 16 dim=2
  memcpy_wide_bus_write_int_2d_16_512(((class ap_uint< 512 > *)c),c_buf_0,0,0,(4 * 0),sizeof(int ) * ((unsigned long )200000),200000L);
}
