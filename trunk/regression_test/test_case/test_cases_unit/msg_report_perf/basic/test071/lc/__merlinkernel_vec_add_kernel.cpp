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
#define SIZE_1 16
#define SIZE_2 16
#define SIZE_3 16
#include "memcpy_512_4d.h"
#undef SIZE_1
#undef SIZE_2
#undef SIZE_3
#include <string.h> 

#include "merlin_type_define.h"
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_kernel_pragma();
// Original: #pragma ACCEL kernel
static int vec_add_kernel_dummy_pos;
extern "C" { 

__kernel void vec_add_kernel(merlin_uint_512 *a,merlin_uint_512 *b,merlin_uint_512 *c,int inc)
{
  
#pragma HLS INTERFACE m_axi port=a offset=slave depth=4096
  
#pragma HLS INTERFACE m_axi port=b offset=slave depth=4096
  
#pragma HLS INTERFACE m_axi port=c offset=slave depth=4096
  
#pragma HLS INTERFACE s_axilite port=a bundle=control
  
#pragma HLS INTERFACE s_axilite port=b bundle=control
  
#pragma HLS INTERFACE s_axilite port=c bundle=control
  
#pragma HLS INTERFACE s_axilite port=inc bundle=control
  
#pragma HLS INTERFACE s_axilite port=return bundle=control
  
#pragma HLS DATA_PACK VARIABLE=c
  
#pragma HLS DATA_PACK VARIABLE=b
  
#pragma HLS DATA_PACK VARIABLE=a
  int b_buf_0[16][16][16][16];
  
#pragma HLS array_partition variable=b_buf_0 complete dim=4
  int a_buf_0[16][16][16][16];
  
#pragma HLS array_partition variable=a_buf_0 complete dim=4
  int c_buf_0[16][16][16][16];
  
#pragma HLS array_partition variable=c_buf_0 complete dim=4
// Existing HLS partition: #pragma HLS array_partition variable=c_buf_0 cyclic factor = 16 dim=4
  memcpy_wide_bus_read_int_4d_16_16_16_512(c_buf_0,0,0,0,0,(merlin_uint_512 *)c,(0 * 4),sizeof(int ) * ((unsigned long )65536),65536LL);
// Existing HLS partition: #pragma HLS array_partition variable=a_buf_0 cyclic factor = 16 dim=4
  memcpy_wide_bus_read_int_4d_16_16_16_512(a_buf_0,0,0,0,0,(merlin_uint_512 *)a,(0 * 4),sizeof(int ) * ((unsigned long )65536),65536LL);
// Existing HLS partition: #pragma HLS array_partition variable=b_buf_0 cyclic factor = 16 dim=4
  memcpy_wide_bus_read_int_4d_16_16_16_512(b_buf_0,0,0,0,0,(merlin_uint_512 *)b,(0 * 4),sizeof(int ) * ((unsigned long )65536),65536LL);
  
#pragma ACCEL interface variable=a max_depth=16,16,16,16 depth=16,16,16,16
  
#pragma ACCEL interface variable=b max_depth=16,16,16,16 depth=16,16,16,16
  
#pragma ACCEL interface variable=c max_depth=16,16,16,16 depth=16,16,16,16
  for (int p = 0; p < 16; p++) {
    for (int q = 0; q < 16; q++) {
      for (int n = 0; n < 16; n++) {
        for (int m = 0; m < 16; m++) {
          for (int j = 0; j < 16; j++) 
// Original: #pragma ACCEL PIPELINE AUTO
// Original: #pragma ACCEL LOOP_FLATTEN OFF
{
            
#pragma HLS pipeline
            
#pragma ACCEL unroll 8
            for (int i = 0; i < 16; i++) 
// Original: #pragma ACCEL PARALLEL COMPLETE
{
              
#pragma HLS unroll
              c_buf_0[n][m][j][i] += a_buf_0[n][m][j][i] + b_buf_0[n][m][j][i];
            }
          }
        }
      }
    }
  }
// Existing HLS partition: #pragma HLS array_partition variable=c_buf_0 cyclic factor = 16 dim=4
  memcpy_wide_bus_write_int_4d_16_16_16_512((merlin_uint_512 *)c,c_buf_0,0,0,0,0,(4 * 0),sizeof(int ) * ((unsigned long )65536),65536LL);
}
}
