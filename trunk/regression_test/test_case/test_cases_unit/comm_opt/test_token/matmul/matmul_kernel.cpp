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
#define VEC_SIZE 256
#define TILE 64
#define aa(p, n) (a[(p)*VEC_SIZE + (n)])
#define bb(p, n) (b[(p)*VEC_SIZE + (n)])
#define cc(p, n) (c[(p)*VEC_SIZE + (n)])
#pragma ACCEL kernel
void matmul_kernel(float *a,float *b,float *c)
{
  float c_buf_0_rn_rf3[256][4][64];
  float a_buf_0_0_rf2[256][256];
  float b_buf_0_0_rf1[256][4][64];
  float c_buf_0_rf0[256][4][64];
  float c_buf_0_rn[256][4][64];
  int _memcpy_i3_2;
  int _memcpy_i3_1;
  int _memcpy_i3_0;
  int _memcpy_i2_1;
  int _memcpy_i2_0;
  int _memcpy_i1_2;
  int _memcpy_i1_1;
  int _memcpy_i1_0;
  int _memcpy_i0_2;
  int _memcpy_i0_1;
  int _memcpy_i0_0;
  float c_buf_0[256][4][64];
  float b_buf_0_0[256][4][64];
  float a_buf_0_0[256][256];
  int i_0;
  int j_0;
  int k_0;
  int j0_0;
  int jj_0;
 // for(int i=0; i<1; i++) {
  for (_memcpy_i0_2 = 0; _memcpy_i0_2 < 256; ++_memcpy_i0_2) {
    for (_memcpy_i0_1 = 0; _memcpy_i0_1 < 4; ++_memcpy_i0_1) {
      for (_memcpy_i0_0 = 0; _memcpy_i0_0 < 64; ++_memcpy_i0_0) {
        c_buf_0[_memcpy_i0_2][_memcpy_i0_1][_memcpy_i0_0] = c[0 + (((0 * 256 + _memcpy_i0_2) * 4 + _memcpy_i0_1) * 64 + _memcpy_i0_0)];
      }
    }
  }
  for (_memcpy_i1_2 = 0; _memcpy_i1_2 < 256; ++_memcpy_i1_2) {
    for (_memcpy_i1_1 = 0; _memcpy_i1_1 < 4; ++_memcpy_i1_1) {
      for (_memcpy_i1_0 = 0; _memcpy_i1_0 < 64; ++_memcpy_i1_0) {
        b_buf_0_0[_memcpy_i1_2][_memcpy_i1_1][_memcpy_i1_0] = b[0 + (((0 * 256 + _memcpy_i1_2) * 4 + _memcpy_i1_1) * 64 + _memcpy_i1_0)];
      }
    }
  }
  for (_memcpy_i2_1 = 0; _memcpy_i2_1 < 256; ++_memcpy_i2_1) {
    for (_memcpy_i2_0 = 0; _memcpy_i2_0 < 256; ++_memcpy_i2_0) {
      a_buf_0_0[_memcpy_i2_1][_memcpy_i2_0] = a[0 + ((0 * 256 + _memcpy_i2_1) * 256 + _memcpy_i2_0)];
    }
  }
  for (k_0 = 0; k_0 < 256; k_0++) {
    for (i_0 = 0; i_0 < 256; i_0++) {
      for (j0_0 = 0; j0_0 < 256 / 64; j0_0++) {
        for (jj_0 = 0; jj_0 < 64; ++jj_0) {
          if (k_0 == 0) {
            c_buf_0_rf0[i_0][j0_0][jj_0] = c_buf_0[i_0][j0_0][jj_0];
          }
          if (i_0 == 0) {
            b_buf_0_0_rf1[k_0][j0_0][jj_0] = b_buf_0_0[k_0][j0_0][jj_0];
          }
          if (j0_0 == 0 && jj_0 == 0) {
            a_buf_0_0_rf2[k_0][i_0] = a_buf_0_0[k_0][i_0];
          }
          j_0 = j0_0 * 64 + jj_0;
          c_buf_0_rf0[i_0][j0_0][jj_0] += a_buf_0_0_rf2[k_0][i_0] * b_buf_0_0_rf1[k_0][j0_0][jj_0];
          c_buf_0_rn_rf3[i_0][j0_0][jj_0] = c_buf_0_rf0[i_0][j0_0][jj_0];
          if (k_0 == 255) {
            c_buf_0_rn[i_0][j0_0][jj_0] = c_buf_0_rn_rf3[i_0][j0_0][jj_0];
          }
        }
      }
    }
  }
  for (_memcpy_i3_2 = 0; _memcpy_i3_2 < 256; ++_memcpy_i3_2) {
    for (_memcpy_i3_1 = 0; _memcpy_i3_1 < 4; ++_memcpy_i3_1) {
      for (_memcpy_i3_0 = 0; _memcpy_i3_0 < 64; ++_memcpy_i3_0) {
        c[0 + (((0 * 256 + _memcpy_i3_2) * 4 + _memcpy_i3_1) * 64 + _memcpy_i3_0)] = c_buf_0_rn[_memcpy_i3_2][_memcpy_i3_1][_memcpy_i3_0];
      }
    }
  }
 // }
}
