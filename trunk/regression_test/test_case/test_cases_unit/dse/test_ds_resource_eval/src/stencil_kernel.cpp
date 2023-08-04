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
#define SIZE_1 640
#include "memcpy_512_2d.h"
#undef SIZE_1
#define SIZE_1 20
#define SIZE_2 640
#include "memcpy_512_3d.h"
#undef SIZE_1
#undef SIZE_2
#include<assert.h>
#include <string.h> 

#include "merlin_type_define.h"







// Original: #pragma ACCEL kernel

void stencil_kernel_L_1_1_para_sub(int filter_buf_0[11],int r0_sub,int r0,int sol_buf_0[20][640],int orig_buf_0[22][640])
// Original: #pragma ACCEL parallel factor=8
// Original: #pragma ACCEL PARALLEL FACTOR=8
// Original: #pragma ACCEL PARALLEL
{
  
#pragma HLS function_instantiate variable=r0_sub
  
#pragma HLS inline off
  for (int rr = 0; rr < 20; rr++) {
    int r = (r0 * 8 + r0_sub) * 20 + rr;
    if (r < 1280 - 2) {
      int orig_buf[4][4];
      
#pragma HLS array_partition variable=orig_buf complete dim=2
      
#pragma HLS array_partition variable=orig_buf complete dim=1
// added for shifting register
      for (int k1 = 0; k1 < 3; k1++) 
// Original: #pragma ACCEL PARALLEL COMPLETE
{
        
#pragma HLS unroll
        for (int k2 = 0; k2 < 4; k2++) 
// Original: #pragma ACCEL PARALLEL COMPLETE
{
          
#pragma HLS unroll
          orig_buf[k1][k2] = orig_buf_0[k1 + rr][k2];
        }
      }
      for (int c = 0; c < 640 - 2; c++) 
// Original: #pragma ACCEL pipeline flatten
// Original: #pragma ACCEL PIPELINE
// Original: #pragma ACCEL PIPELINE
{
        
#pragma HLS pipeline
        int temp;
        int mul;
        temp = ((int )0);
        for (int k1 = 0; k1 < 3; k1++) 
// Original: #pragma ACCEL PARALLEL COMPLETE
// Original: #pragma ACCEL PARALLEL COMPLETE
{
          
#pragma HLS unroll
          for (int k2 = 0; k2 < 3; k2++) 
// Original: #pragma ACCEL PARALLEL COMPLETE
// Original: #pragma ACCEL PARALLEL COMPLETE
{
            
#pragma HLS unroll
//orig[(r+k1)*col_size + c+k2];
            mul = filter_buf_0[k1 * 4 + k2] * orig_buf[k1][k2];
            temp += mul;
          }
        }
        sol_buf_0[rr][c] = temp;
// added for shifting register
        for (int k1 = 0; k1 < 3; k1++) 
// Original: #pragma ACCEL PARALLEL COMPLETE
// Original: #pragma ACCEL PARALLEL COMPLETE
{
          
#pragma HLS unroll
          for (int k2 = 0; k2 < 3; k2++) 
// Original: #pragma ACCEL PARALLEL COMPLETE
// Original: #pragma ACCEL PARALLEL COMPLETE
{
            
#pragma HLS unroll
            orig_buf[k1][k2] = orig_buf[k1][1 + k2];
          }
          if (c < 640 - 4) {
            orig_buf[k1][3] = orig_buf_0[k1 + rr][4 + c];
          }
        }
      }
      sol_buf_0[rr][638] = 0;
      sol_buf_0[rr][639] = 0;
    }
     else {
      for (int c = 0; c < 640; c++) 
// Original: #pragma ACCEL PIPELINE AUTO
{
        
#pragma HLS pipeline
        sol_buf_0[rr][c] = 0;
      }
    }
  }
}

void stencil_kernel_L_1_1_para(int filter_buf_0[8][11],int r0,int sol_buf_0[8][20][640],int orig_buf_0[8][22][640])
{
  
#pragma HLS inline off
  int r0_sub;
  for (r0_sub = 0; r0_sub < 8; ++r0_sub) {
    
#pragma HLS unroll complete
    stencil_kernel_L_1_1_para_sub(filter_buf_0[r0_sub],r0_sub,r0,sol_buf_0[r0_sub],orig_buf_0[r0_sub]);
  }
}
static int stencil_kernel_dummy_pos;

static void merlin_memcpy_0(int dst[11],int dst_idx_1,int src[16],int src_idx_0,unsigned int len,unsigned int max_len)
{
  
#pragma HLS inline off
  
#pragma HLS function_instantiate variable=dst_idx_1,src_idx_0
  long long i;
  long long total_offset1 = 0 * 11 + dst_idx_1;
  long long total_offset2 = 0 * 16 + src_idx_0;
  for (i = 0; i < len / 4; ++i) {
    
#pragma HLS PIPELINE II=1
    
#pragma HLS LOOP_TRIPCOUNT max=11
    dst[total_offset1 + i] = src[total_offset2 + i];
  }
}
extern "C" { 

__kernel void stencil_kernel(merlin_uint_512 *orig,merlin_uint_512 *sol,int filter[16])
{
  
#pragma HLS INTERFACE m_axi port=filter offset=slave depth=16
  
#pragma HLS INTERFACE m_axi port=orig offset=slave depth=51200
  
#pragma HLS INTERFACE m_axi port=sol offset=slave depth=51200
  
#pragma HLS INTERFACE s_axilite port=filter bundle=control
  
#pragma HLS INTERFACE s_axilite port=orig bundle=control
  
#pragma HLS INTERFACE s_axilite port=sol bundle=control
  
#pragma HLS INTERFACE s_axilite port=return bundle=control
  
#pragma HLS DATA_PACK VARIABLE=sol
  
#pragma HLS DATA_PACK VARIABLE=orig
  int filter_buf_0[8][11];
// Existing HLS partition: #pragma HLS array_partition variable=filter_buf_0 dim=1 complete
  
#pragma HLS array_partition variable=filter_buf_0 complete dim=2
  
#pragma HLS array_partition variable=filter_buf_0 complete dim=1
  int r0_sub;
{
    int r0_sub;
    for (r0_sub = 0; r0_sub < 8; ++r0_sub) {
      
#pragma HLS UNROLL
      merlin_memcpy_0(filter_buf_0[r0_sub],0,filter,0,sizeof(int ) * ((unsigned long )11),44UL);
    }
  }
  
#pragma ACCEL interface variable=filter max_depth=16 depth=16
  
#pragma ACCEL interface variable=sol max_depth=819200 depth=819200
  
#pragma ACCEL interface variable=orig max_depth=819200 depth=819200
#define TS_R 20
//    #pragma ACCEL pipeline
// Original pramga:  ACCEL PARALLEL FACTOR=8 
  for (int r0 = 0; r0 < 8; r0++) 
// Original: #pragma ACCEL parallel factor=8
// Original: #pragma ACCEL PARALLEL FACTOR=8
{
    int sol_buf_0[8][20][640];
// Existing HLS partition: #pragma HLS array_partition variable=sol_buf_0 dim=1 complete
    
#pragma HLS array_partition variable=sol_buf_0 cyclic factor=8 dim=3
    
#pragma HLS array_partition variable=sol_buf_0 complete dim=1
    int orig_buf_0[8][22][640];
// Existing HLS partition: #pragma HLS array_partition variable=orig_buf_0 dim=1 complete
    
#pragma HLS array_partition variable=orig_buf_0 cyclic factor=8 dim=3
    
#pragma HLS array_partition variable=orig_buf_0 cyclic factor=3 dim=2
    
#pragma HLS array_partition variable=orig_buf_0 complete dim=1
{
      int r0_sub;
      for (r0_sub = 0; r0_sub < 8; ++r0_sub) {
        
#pragma HLS UNROLL
        unsigned long merlin_len = (14080 <= 819200 + r0 * - 102400 + r0_sub * - 12800?14080 : 819200 + r0 * - 102400 + r0_sub * - 12800);
        merlin_len <= 14080LL?(static_cast < void  >  (0)) : __assert_fail("merlin_len <= 14080LL","__merlinkernel_stencil_kernel.cpp",47,__PRETTY_FUNCTION__);
// Existing HLS partition: #pragma HLS array_partition variable=orig_buf_0 cyclic factor = 16 dim=3
        memcpy_wide_bus_read_int_2d_640_512(orig_buf_0[r0_sub],0,0,(merlin_uint_512 *)orig,((r0 * 102400 + r0_sub * 12800) * 4),sizeof(int ) * merlin_len,14080LL);
      }
    }
    stencil_kernel_L_1_1_para(filter_buf_0,r0,sol_buf_0,orig_buf_0);
    for (r0_sub = 0; r0_sub < 8; ++r0_sub) 
// Original: #pragma ACCEL parallel factor=8
// Original: #pragma ACCEL PARALLEL FACTOR=8
// Original: #pragma ACCEL PARALLEL
{
    }{
      memcpy_wide_bus_write_int_3d_20_640_512((merlin_uint_512 *)sol,sol_buf_0,0,0,0,(4 * (r0 * 102400)),sizeof(int ) * ((unsigned long )102400),102400LL);
    }
  }
// Existing HLS partition: #pragma HLS array_partition variable=sol_buf_0 cyclic factor = 16 dim=3
}
}
