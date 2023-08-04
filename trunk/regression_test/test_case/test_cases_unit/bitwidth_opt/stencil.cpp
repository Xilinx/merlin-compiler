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
#include "stencil.h"
static void __merlin_dummy_0();
static void __merlin_dummy_kernel_pragma();
// Original: #pragma ACCEL kernel name="stencil"

void stencil_kernel_L_1_1_para(int filter_buf_0[3][3],int r0_sub,int r0,int sol_buf_0[20][640],int orig_buf_0[22][640])
// Original: #pragma ACCEL parallel
// Original: #pragma ACCEL PARALLEL COMPLETE
{
  
#pragma HLS inline off
  for (int rr = 0; rr < 20; rr++) {
    int r = (r0 * 16 + r0_sub) * 20 + rr;
    if (r < 1280 - 2) {
      int orig_buf[4][4];
// added for shifting register
      for (int k1 = 0; k1 < 3; k1++) {
        
#pragma ACCEL PIPELINE
        for (int k2 = 0; k2 < 4; k2++) {
          
#pragma ACCEL PARALLEL COMPLETE
          orig_buf[k1][k2] = orig_buf_0[rr + k1][k2];
        }
      }
      for (int c = 0; c < 640 - 2; c++) 
// Original: #pragma ACCEL pipeline flatten
{
        
#pragma ACCEL PIPELINE II=1
        int temp;
        int mul;
        temp = ((int )0);
        for (int k1 = 0; k1 < 3; k1++) {
          
#pragma ACCEL PARALLEL COMPLETE
          for (int k2 = 0; k2 < 3; k2++) {
            
#pragma ACCEL PARALLEL COMPLETE
//orig[(r+k1)*col_size + c+k2];
            mul = filter_buf_0[k1][k2] * orig_buf[k1][k2];
            temp += mul;
          }
        }
        sol_buf_0[rr][c] = temp;
// added for shifting register
        for (int k1 = 0; k1 < 3; k1++) {
          
#pragma ACCEL PARALLEL COMPLETE
          for (int k2 = 0; k2 < 3; k2++) {
            
#pragma ACCEL PARALLEL COMPLETE
            orig_buf[k1][k2] = orig_buf[k1][1 + k2];
          }
          if (c < 640 - 4) {
            orig_buf[k1][3] = orig_buf_0[rr + k1][4 + c];
          }
        }
      }
      sol_buf_0[rr][638] = 0;
      sol_buf_0[rr][639] = 0;
    }
     else {
      for (int c = 0; c < 640; c++) {
        
#pragma ACCEL PIPELINE
        sol_buf_0[rr][c] = 0;
      }
    }
  }
}

#pragma ACCEL kernel
void stencil_kernel(int orig[819200],int sol[819200],int filter[9])
{
  int filter_buf_0[16][3][3];
  
#pragma HLS array_partition variable=filter_buf_0 dim=1 complete
  __merlin_access_range(filter,0,8UL);
  __merlin_access_range(sol,0,819199UL);
  __merlin_access_range(orig,0,819199UL);
  
#pragma ACCEL interface variable=filter depth=9 max_depth=9
  
#pragma ACCEL interface variable=sol depth=819200 max_depth=819200
  
#pragma ACCEL interface variable=orig depth=819200 max_depth=819200
  int r0_sub;
{
    int r0_sub;
    for (r0_sub = 0; r0_sub < 16; ++r0_sub) {
      memcpy(filter_buf_0[r0_sub][0],((const void *)(&filter[0])),sizeof(int ) * 9);
    }
  }
  for (int r0 = 0; r0 < 4LL; r0++) {
    int sol_buf_0[16][20][640];
    
#pragma HLS array_partition variable=sol_buf_0 dim=1 complete
    int orig_buf_0[16][22][640];
    
#pragma HLS array_partition variable=orig_buf_0 dim=1 complete
{
      int r0_sub;
      for (r0_sub = 0; r0_sub < 16; ++r0_sub) {
        memcpy(orig_buf_0[r0_sub][0],((const void *)(&orig[r0 * 204800 + r0_sub * 12800])),sizeof(int ) * 14080);
      }
    }
    for (r0_sub = 0; r0_sub < 16; ++r0_sub) 
// Original: #pragma ACCEL parallel
// Original: #pragma ACCEL PARALLEL COMPLETE
{
      
#pragma HLS unroll complete
      stencil_kernel_L_1_1_para(filter_buf_0[r0_sub],r0_sub,r0,sol_buf_0[r0_sub],orig_buf_0[r0_sub]);
    }
{
      memcpy(((void *)(&sol[r0 * 204800])),sol_buf_0[0][0],sizeof(int ) * 12800 * 16);
    }
  }
}
#define TS_R 20
