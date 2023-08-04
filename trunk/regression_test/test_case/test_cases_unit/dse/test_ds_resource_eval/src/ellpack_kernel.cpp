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
#define SIZE_1 32
#define SIZE_2 32
#define SIZE_3 16
#include "memcpy_512_4d.h"
#undef SIZE_1
#undef SIZE_2
#undef SIZE_3
#define SIZE_1 32
#include "memcpy_512_2d.h"
#undef SIZE_1
#include <string.h> 

#include "merlin_type_define.h"






// Original: #pragma ACCEL kernel

void ellpack_kernel_L_1_0_para_sub(int i0_sub,double vec_buf_0[4096],double merlin_out_buf_0[32],int cols_buf_0[32][32][16],double nzval_buf_0[32][32][16])
// Original: #pragma ACCEL parallel factor=8
// Original: #pragma ACCEL PARALLEL FACTOR=8
// Original: #pragma ACCEL PARALLEL
{
  
#pragma HLS function_instantiate variable=i0_sub
  
#pragma HLS inline off
  for (int iii = 0; iii < 32; iii++) {
    int j0;
    int jj;
// = out[i];
    double sum = (double )0;
    ellpack_2:
{
      long long _j0_jj;
      j0 = 0;
      jj = 0;
// Flattened for(j0 = 0;j0 <(1 << 9) / 16;j0++) {...}
// Flattened for(jj = 0;jj < 16;jj++) {...}
      for (_j0_jj = (0 * 16); _j0_jj <= (32 * 16 - 1); ++_j0_jj) 
// Original: #pragma ACCEL parallel factor=1
// Original: #pragma ACCEL PARALLEL FACTOR=1
// Original: #pragma ACCEL PARALLEL FACTOR=1 
// Original: #pragma ACCEL PIPELINE AUTO
{
        
#pragma HLS pipeline
        double Si = nzval_buf_0[iii][j0][jj] * vec_buf_0[cols_buf_0[iii][j0][jj]];
        sum += Si;
{
          ++jj;
          if (jj > 15) {
            ++j0;
            if (j0 <= 31) 
              jj = 0;
          }
        }
      }
    }
    merlin_out_buf_0[iii] += sum;
  }
}

void ellpack_kernel_L_1_0_para(double vec_buf_0[8][4096],double merlin_out_buf_0[8][32],int cols_buf_0[8][32][32][16],double nzval_buf_0[8][32][32][16])
{
  
#pragma HLS inline off
  int i0_sub;
  for (i0_sub = 0; i0_sub < 8; ++i0_sub) {
    
#pragma HLS unroll complete
    ellpack_kernel_L_1_0_para_sub(i0_sub,vec_buf_0[i0_sub],merlin_out_buf_0[i0_sub],cols_buf_0[i0_sub],nzval_buf_0[i0_sub]);
  }
}
static int ellpack_kernel_dummy_pos;
extern "C" { 

__kernel void ellpack_kernel(merlin_uint_512 *nzval,merlin_uint_512 *cols,merlin_uint_512 *vec,merlin_uint_512 *merlin_out)
{
  
#pragma HLS INTERFACE m_axi port=cols offset=slave depth=131072
  
#pragma HLS INTERFACE m_axi port=merlin_out offset=slave depth=512
  
#pragma HLS INTERFACE m_axi port=nzval offset=slave depth=262144
  
#pragma HLS INTERFACE m_axi port=vec offset=slave depth=512
  
#pragma HLS INTERFACE s_axilite port=cols bundle=control
  
#pragma HLS INTERFACE s_axilite port=merlin_out bundle=control
  
#pragma HLS INTERFACE s_axilite port=nzval bundle=control
  
#pragma HLS INTERFACE s_axilite port=vec bundle=control
  
#pragma HLS INTERFACE s_axilite port=return bundle=control
  
#pragma HLS DATA_PACK VARIABLE=merlin_out
  
#pragma HLS DATA_PACK VARIABLE=vec
  
#pragma HLS DATA_PACK VARIABLE=cols
  
#pragma HLS DATA_PACK VARIABLE=nzval
  int i0_sub;
//TYPE Si;
  
#pragma ACCEL interface variable=nzval max_depth=2097152 depth=2097152
  
#pragma ACCEL interface variable=cols max_depth=2097152 depth=2097152
  
#pragma ACCEL interface variable=vec max_depth=4096 depth=4096
  
#pragma ACCEL interface max_depth=4096 depth=4096 variable=merlin_out
#define TS 16
#define TS1 (8)
#define TS2 (32)
  ellpack_1:
{
    double vec_buf_0[8][4096];
// Existing HLS partition: #pragma HLS array_partition variable=vec_buf_0 dim=1 complete
    
#pragma HLS array_partition variable=vec_buf_0 cyclic factor=4 dim=2
    
#pragma HLS array_partition variable=vec_buf_0 complete dim=1
{
      int i0_sub;
      for (i0_sub = 0; i0_sub < 8; ++i0_sub) {
        
#pragma HLS UNROLL
// Existing HLS partition: #pragma HLS array_partition variable=vec_buf_0 cyclic factor = 8 dim=2
        memcpy_wide_bus_read_double_512(vec_buf_0[i0_sub],(merlin_uint_512 *)vec,(0 * 8),sizeof(double ) * ((unsigned long )4096),4096LL);
      }
    }
// Original pramga:  ACCEL PARALLEL FACTOR=8 
    for (int i0 = 0; i0 < 16; i0++) 
// Original: #pragma ACCEL parallel factor=8
// Original: #pragma ACCEL PARALLEL FACTOR=8
{
      double merlin_out_buf_0[8][32];
// Existing HLS partition: #pragma HLS array_partition variable=merlin_out_buf_0 dim=1 complete
      
#pragma HLS array_partition variable=merlin_out_buf_0 cyclic factor=4 dim=2
      
#pragma HLS array_partition variable=merlin_out_buf_0 complete dim=1
      int cols_buf_0[8][32][32][16];
// Existing HLS partition: #pragma HLS array_partition variable=cols_buf_0 dim=1 complete
      
#pragma HLS array_partition variable=cols_buf_0 complete dim=4
      
#pragma HLS array_partition variable=cols_buf_0 complete dim=1
      double nzval_buf_0[8][32][32][16];
// Existing HLS partition: #pragma HLS array_partition variable=nzval_buf_0 dim=1 complete
      
#pragma HLS array_partition variable=nzval_buf_0 cyclic factor=4 dim=4
      
#pragma HLS array_partition variable=nzval_buf_0 complete dim=1
{
        memcpy_wide_bus_read_double_4d_32_32_16_512(nzval_buf_0,0,0,0,0,(merlin_uint_512 *)nzval,(i0 * 131072 * 8),sizeof(double ) * ((unsigned long )131072),131072LL);
      }
{
        memcpy_wide_bus_read_int_4d_32_32_16_512(cols_buf_0,0,0,0,0,(merlin_uint_512 *)cols,(i0 * 131072 * 4),sizeof(int ) * ((unsigned long )131072),131072LL);
      }
{
        memcpy_wide_bus_read_double_2d_32_512(merlin_out_buf_0,0,0,(merlin_uint_512 *)merlin_out,(i0 * 256 * 8),sizeof(double ) * ((unsigned long )256),256LL);
      }
      ellpack_kernel_L_1_0_para(vec_buf_0,merlin_out_buf_0,cols_buf_0,nzval_buf_0);
      for (i0_sub = 0; i0_sub < 8; ++i0_sub) 
// Original: #pragma ACCEL parallel factor=8
// Original: #pragma ACCEL PARALLEL FACTOR=8
// Original: #pragma ACCEL PARALLEL
{
      }{
        memcpy_wide_bus_write_double_2d_32_512((merlin_uint_512 *)merlin_out,merlin_out_buf_0,0,0,(8 * (i0 * 256)),sizeof(double ) * ((unsigned long )256),256LL);
      }
    }
// Existing HLS partition: #pragma HLS array_partition variable=merlin_out_buf_0 cyclic factor = 8 dim=2
// Existing HLS partition: #pragma HLS array_partition variable=merlin_out_buf_0 cyclic factor = 8 dim=2
// Existing HLS partition: #pragma HLS array_partition variable=cols_buf_0 cyclic factor = 16 dim=4
// Existing HLS partition: #pragma HLS array_partition variable=nzval_buf_0 cyclic factor = 8 dim=4
  }
//#pragma ACCEL pipeline
}
}
