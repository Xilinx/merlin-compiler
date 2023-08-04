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
#include "cmost.h"
#include "merlin_type_define.h"
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_kernel_pragma();
// Original: #pragma ACCEL kernel
static int ellpack_kernel_dummy_pos;
extern "C" { 

__kernel void ellpack_kernel(double nzval[2097152],int cols[2097152],double vec[4096],double merlin_out[4096])
{
  
#pragma HLS INTERFACE m_axi port=cols offset=slave depth=2097152 bundle=merlin_gmem_ellpack_kernel_32_0
  
#pragma HLS INTERFACE m_axi port=merlin_out offset=slave depth=4096 bundle=merlin_gmem_ellpack_kernel_64_merlin_out
  
#pragma HLS INTERFACE m_axi port=nzval offset=slave depth=2097152 bundle=merlin_gmem_ellpack_kernel_64_0
  
#pragma HLS INTERFACE m_axi port=vec offset=slave depth=4096 bundle=merlin_gmem_ellpack_kernel_64_1
  
#pragma HLS INTERFACE s_axilite port=cols bundle=control
  
#pragma HLS INTERFACE s_axilite port=merlin_out bundle=control
  
#pragma HLS INTERFACE s_axilite port=nzval bundle=control
  
#pragma HLS INTERFACE s_axilite port=vec bundle=control
  
#pragma HLS INTERFACE s_axilite port=return bundle=control
//TYPE Si;
  
#pragma ACCEL interface variable=nzval max_depth=2097152 depth=2097152
  
#pragma ACCEL interface variable=cols max_depth=2097152 depth=2097152
  
#pragma ACCEL interface variable=vec max_depth=4096 depth=4096
  
#pragma ACCEL interface max_depth=4096 depth=4096 variable=merlin_out
#define TS 16
#define TS1 (8)
#define TS2 (32)
  ellpacku10:
//#pragma ACCEL pipeline
  for (int i0 = 0; i0 < (1 << 12) / 32; i0++) {
    merlinL1:
    for (int iii = 0; iii < 32; iii++) {
      int i = i0 * 32 + iii;
      int j0;
      int jj;
// = out[i];
      double sum = (double )0;
      ellpacku20:
      for (j0 = 0; j0 < (1 << 9) / 16; j0++) {
        merlinL0:
        for (jj = 0; jj < 16; jj++) {
          int j = j0 * 16 + jj;
          double Si = nzval[j + i * (1 << 9)] * vec[cols[j + i * (1 << 9)]];
          sum += Si;
        }
      }
      merlin_out[i] += sum;
    }
  }
}
}
