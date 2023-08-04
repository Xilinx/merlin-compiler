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
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();

static void sub(int *a,int *b)
{
  int b_buf_0[512];
  long long _memcpy_i_1;
  
#pragma unroll 16
  for (_memcpy_i_1 = 0; _memcpy_i_1 < (sizeof(int ) * ((unsigned long )512) / 4); ++_memcpy_i_1) {
    long long total_offset1 = (0 * 512 + 0);
    long long total_offset2 = (0 * 0 + 0);
    b_buf_0[total_offset1 + _memcpy_i_1] = b[total_offset2 + _memcpy_i_1];
  }
  int a_buf_0[512];
  __merlin_access_range(b,0,1023UL);
  for (int _i_l1 = 0; _i_l1 < 512; ++_i_l1) {
    a_buf_0[_i_l1] = b_buf_0[_i_l1];
  }
  long long _memcpy_i_0;
  
#pragma unroll 16
  for (_memcpy_i_0 = 0; _memcpy_i_0 < (sizeof(int ) * ((unsigned long )512) / 4); ++_memcpy_i_0) {
    long long total_offset1 = (0 * 0 + 0);
    long long total_offset2 = (0 * 512 + 0);
    a[total_offset1 + _memcpy_i_0] = a_buf_0[total_offset2 + _memcpy_i_0];
  }
}
static void __merlin_dummy_kernel_pragma();
// Original: #pragma ACCEL kernel

void vec_add(int a[1024],int b[1024])
{
  __merlin_access_range(b,0,1023UL);
  __merlin_access_range(a,0,1023UL);
  
#pragma ACCEL interface variable=b max_depth=1024 depth=1024
  
#pragma ACCEL interface variable=a max_depth=1024 depth=1024
  sub(a + 512,b);
  sub(a,b);
}
