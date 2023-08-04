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
#define SIZE_1 2
#define SIZE_2 16
#define SIZE_3 8
#include "memcpy_512_4d.h"
#undef SIZE_1
#undef SIZE_2
#undef SIZE_3
#define SIZE_1 8
#define SIZE_2 16
#define SIZE_3 8
#include "memcpy_512_4d.h"
#undef SIZE_1
#undef SIZE_2
#undef SIZE_3
#include <string.h> 

#include "merlin_type_define.h"






// Original: #pragma ACCEL kernel
static int bbgemm_kernel_dummy_pos;
extern "C" { 

__kernel void bbgemm_kernel(merlin_uint_512 *m1,merlin_uint_512 *m2,merlin_uint_512 *prod)
{
  
#pragma HLS INTERFACE m_axi port=m1 offset=slave depth=2048
  
#pragma HLS INTERFACE m_axi port=m2 offset=slave depth=2048
  
#pragma HLS INTERFACE m_axi port=prod offset=slave depth=2048
  
#pragma HLS INTERFACE s_axilite port=m1 bundle=control
  
#pragma HLS INTERFACE s_axilite port=m2 bundle=control
  
#pragma HLS INTERFACE s_axilite port=prod bundle=control
  
#pragma HLS INTERFACE s_axilite port=return bundle=control
  
#pragma HLS DATA_PACK VARIABLE=prod
  
#pragma HLS DATA_PACK VARIABLE=m2
  
#pragma HLS DATA_PACK VARIABLE=m1
  double m1_buf_0[64][2][16][8];
// Existing HLS partition: #pragma HLS array_partition variable=m1_buf_0 cyclic factor = 8 dim=4
  
#pragma HLS array_partition variable=m1_buf_0 complete dim=4
  
#pragma HLS array_partition variable=m1_buf_0 complete dim=2
  memcpy_wide_bus_read_double_4d_2_16_8_512(m1_buf_0,0,0,0,0,(merlin_uint_512 *)m1,(0 * 8),sizeof(double ) * ((unsigned long )16384),16384LL);
  double prod_buf_0[64][2][16][8];
// Existing HLS partition: #pragma HLS array_partition variable=prod_buf_0 cyclic factor = 8 dim=4
  
#pragma HLS array_partition variable=prod_buf_0 complete dim=4
  
#pragma HLS array_partition variable=prod_buf_0 complete dim=2
  memcpy_wide_bus_read_double_4d_2_16_8_512(prod_buf_0,0,0,0,0,(merlin_uint_512 *)prod,(0 * 8),sizeof(double ) * ((unsigned long )16384),16384LL);
  double m2_buf_0[16][8][16][8];
// Existing HLS partition: #pragma HLS array_partition variable=m2_buf_0 cyclic factor = 8 dim=4
  
#pragma HLS array_partition variable=m2_buf_0 complete dim=4
  
#pragma HLS array_partition variable=m2_buf_0 complete dim=2
  memcpy_wide_bus_read_double_4d_8_16_8_512(m2_buf_0,0,0,0,0,(merlin_uint_512 *)m2,(0 * 8),sizeof(double ) * ((unsigned long )16384),16384LL);
  int i_sub;
  int i;
  int k;
  int j;
  int jj;
  int kk;
  int i_row;
  int k_row;
  double temp_x;
  double mul;
  
#pragma ACCEL interface variable=m1 max_depth=16384 depth=16384
  
#pragma ACCEL interface variable=m2 max_depth=16384 depth=16384
  
#pragma ACCEL interface variable=prod max_depth=16384 depth=16384
// Standardize from: for(jj = 0;jj < 128;jj += 8) {...}
  for (jj = 0; jj <= 15; jj++) {
    int _in_jj = 0 + ((int )8LL) * jj;
// Standardize from: for(kk = 0;kk < 128;kk += 8) {...}
    for (kk = 0; kk <= 15; kk++) {
      int _in_kk = 0 + ((int )8LL) * kk;
// Original pramga:  ACCEL PARALLEL FACTOR=2 
      for (i = 0; i < 64; ++i) 
// Original: #pragma ACCEL parallel factor=2
// Original: #pragma ACCEL PARALLEL FACTOR=2
// Original: #pragma ACCEL PIPELINE AUTO
{
        
#pragma HLS pipeline
        for (i_sub = 0; i_sub < 2; ++i_sub) 
// Original: #pragma ACCEL parallel factor=2
// Original: #pragma ACCEL PARALLEL FACTOR=2
// Original: #pragma ACCEL PARALLEL
{
          
#pragma HLS unroll
          for (k = 0; k < 8; ++k) 
// Original: #pragma ACCEL parallel
// Original: #pragma ACCEL PARALLEL COMPLETE
// Original: #pragma ACCEL PARALLEL COMPLETE
{
            
#pragma HLS unroll
            i_row = (i * 2 + i_sub) * 128;
            k_row = (k + _in_kk) * 128;
            temp_x = m1_buf_0[i][i_sub][kk][k];
            for (j = 0; j < 8; ++j) 
// Original: #pragma ACCEL parallel
// Original: #pragma ACCEL PARALLEL COMPLETE
// Original: #pragma ACCEL PARALLEL COMPLETE
{
              
#pragma HLS unroll
              mul = temp_x * m2_buf_0[kk][k][jj][j];
              prod_buf_0[i][i_sub][jj][j] += mul;
            }
          }
        }
      }
//#pragma ACCEL pipeline
//#pragma ACCEL tiling factor=8
    }
    kk = 120 + ((int )8LL);
  }
  jj = 120 + ((int )8LL);
// Existing HLS partition: #pragma HLS array_partition variable=prod_buf_0 cyclic factor = 8 dim=4
  memcpy_wide_bus_write_double_4d_2_16_8_512((merlin_uint_512 *)prod,prod_buf_0,0,0,0,0,(8 * 0),sizeof(double ) * ((unsigned long )16384),16384LL);
}
}
