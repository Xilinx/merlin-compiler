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
#include "altera_const.h"
#include <string.h>

#include "merlin_type_define.h"
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_kernel_pragma();
// Original: #pragma ACCEL kernel

__kernel void vec_add_kernel(__global int *restrict a, __global int *restrict b,
                             __global int *restrict c, int inc) {
  int _memcpy_i2_1;
  int _memcpy_i2_0;
  int _memcpy_i1_1;
  int _memcpy_i1_0;
  int _memcpy_i0_1;
  int _memcpy_i0_0;
  int b_buf_0[625][16];
  int a_buf_0[625][16];
  int c_buf_0[625][16];

#pragma ACCEL interface variable = a max_depth = 10000 depth = 10000

#pragma ACCEL interface variable = b max_depth = 10000 depth = 10000

#pragma ACCEL interface variable = c max_depth = 10000 depth = 10000
  int j;
  int j0;
  int jj;
  for (_memcpy_i0_1 = 0; _memcpy_i0_1 < 625; ++_memcpy_i0_1) {

#pragma HLS COALESCING_WIDTH = 512

#pragma unroll
    for (_memcpy_i0_0 = 0; _memcpy_i0_0 < 16; ++_memcpy_i0_0) {
      a_buf_0[_memcpy_i0_1][_memcpy_i0_0] =
          a[0 + ((0 * 625 + _memcpy_i0_1) * 16 + _memcpy_i0_0)];
    }
  }
  for (_memcpy_i1_1 = 0; _memcpy_i1_1 < 625; ++_memcpy_i1_1) {

#pragma HLS COALESCING_WIDTH = 512

#pragma unroll
    for (_memcpy_i1_0 = 0; _memcpy_i1_0 < 16; ++_memcpy_i1_0) {
      b_buf_0[_memcpy_i1_1][_memcpy_i1_0] =
          b[0 + ((0 * 625 + _memcpy_i1_1) * 16 + _memcpy_i1_0)];
    }
  }
  for (j0 = 0; j0 < 10000 / 16; j0++) {

#pragma unroll 8
    for (jj = 0; jj < 16; jj++)
    // Original: #pragma ACCEL parallel
    // Original: #pragma ACCEL PARALLEL COMPLETE
    {
      j = j0 * 16 + jj;
      c_buf_0[j0][jj] = a_buf_0[j0][jj] + b_buf_0[j0][jj] + inc;
    }
    break;
  }
  for (_memcpy_i2_1 = 0; _memcpy_i2_1 < 625; ++_memcpy_i2_1) {

#pragma HLS COALESCING_WIDTH = 512

#pragma unroll
    for (_memcpy_i2_0 = 0; _memcpy_i2_0 < 16; ++_memcpy_i2_0) {
      c[0 + ((0 * 625 + _memcpy_i2_1) * 16 + _memcpy_i2_0)] =
          c_buf_0[_memcpy_i2_1][_memcpy_i2_0];
    }
  }
}
