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
#define VEC_SIZE 10000
#define TILE 16
#pragma ACCEL kernel
void vec_add_kernel(int *a,int *b,int *c,int inc)
{
  int _memcpy_i2_1;
  int _memcpy_i2_0;
  int _memcpy_i1_1;
  int _memcpy_i1_0;
  int _memcpy_i0_1;
  int _memcpy_i0_0;
  int c_buf_0[625][16];
  int b_buf_0[625][16];
  int a_buf_0_0[625][16];
  int j_0;
  int j0_0;
  int jj_0;
//for(int i=0; i<1;i++) {
  for (_memcpy_i0_1 = 0; _memcpy_i0_1 < 625; ++_memcpy_i0_1) {
    for (_memcpy_i0_0 = 0; _memcpy_i0_0 < 16; ++_memcpy_i0_0) {
// Parallel pragma comes from SgInitializedName:b_buf_0
      
#pragma ACCEL parallel
      b_buf_0[_memcpy_i0_1][_memcpy_i0_0] = b[0 + ((0 * 625 + _memcpy_i0_1) * 16 + _memcpy_i0_0)];
    }
  }
  for (_memcpy_i1_1 = 0; _memcpy_i1_1 < 625; ++_memcpy_i1_1) {
    for (_memcpy_i1_0 = 0; _memcpy_i1_0 < 16; ++_memcpy_i1_0) {
// Parallel pragma comes from SgInitializedName:a_buf_0_0
      
#pragma ACCEL parallel
      a_buf_0_0[_memcpy_i1_1][_memcpy_i1_0] = a[0 + ((0 * 625 + _memcpy_i1_1) * 16 + _memcpy_i1_0)];
    }
  }
  for (j0_0 = 0; j0_0 < 10000 / 16; j0_0++) {
    for (jj_0 = 0; jj_0 < 16; jj_0++) 
/* Original: #pragma ACCEL parallel */
{
      
#pragma ACCEL PARALLEL COMPLETE
      j_0 = j0_0 * 16 + jj_0;
      c_buf_0[j0_0][jj_0] = a_buf_0_0[j0_0][jj_0] + b_buf_0[j0_0][jj_0] + inc;
    }
  }
  for (_memcpy_i2_1 = 0; _memcpy_i2_1 < 625; ++_memcpy_i2_1) {
    for (_memcpy_i2_0 = 0; _memcpy_i2_0 < 16; ++_memcpy_i2_0) {
// Parallel pragma comes from SgInitializedName:c_buf_0
      
#pragma ACCEL parallel
      c[0 + ((0 * 625 + _memcpy_i2_1) * 16 + _memcpy_i2_0)] = c_buf_0[_memcpy_i2_1][_memcpy_i2_0];
    }
  }
//}
}
