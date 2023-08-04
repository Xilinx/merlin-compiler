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
#include "merlin_type_define.h"
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_kernel_pragma();
/* Original: #pragma ACCEL kernel */

__kernel void vec_add_kernel(__global int * restrict a,__global int * restrict b,__global int * restrict c,int inc)
{
  int b_buf_0[inc];
  long _memcpy_i_1;
  
#pragma unroll 16
  for (_memcpy_i_1 = 0; _memcpy_i_1 < sizeof(int ) * ((unsigned long )inc) / 4; ++_memcpy_i_1) {
    b_buf_0[_memcpy_i_1] = b[_memcpy_i_1];
  }
  int a_buf_0[inc];
  long _memcpy_i_0;
  
#pragma unroll 16
  for (_memcpy_i_0 = 0; _memcpy_i_0 < sizeof(int ) * ((unsigned long )inc) / 4; ++_memcpy_i_0) {
    a_buf_0[_memcpy_i_0] = a[_memcpy_i_0];
  }
  int c_buf_0[inc];
  
#pragma ACCEL interface variable=a max_depth=inc depth=inc
  
#pragma ACCEL interface variable=b max_depth=inc depth=inc
  
#pragma ACCEL interface variable=c max_depth=inc depth=inc
  int j;
  for (j = inc; j < 2 * inc; j++) {
    c_buf_0[j] = a_buf_0[j] + b_buf_0[j] + inc;
  }
  long _memcpy_i_2;
  
#pragma unroll 16
  for (_memcpy_i_2 = 0; _memcpy_i_2 < sizeof(int ) * ((unsigned long )inc) / 4; ++_memcpy_i_2) {
    c[_memcpy_i_2] = c_buf_0[_memcpy_i_2];
  }
}
