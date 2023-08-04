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
const int m = 1;
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_kernel_pragma();
/* Original: #pragma ACCEL kernel */
const int aa[2][2] = {1, 2, 3, 4};
const int bb[2][2] = {1, 2, 3, 4};
const int cc[2] = {1, 2};
const int n = 1;
const int * x;
const dd = 1;
int * y;
//int * aaa;
//int * bbb = aaa + 1;
//int * ccc = aaa - 1;
//int * ddd = &aaa[0];
//long * eee = (long *)aaa;
const int adder(int a, int b) {
    return a + b;
}
const int adder_2(int *a, int *b) {
  int j;
  for (j = 0; j < 10000; j++) {
    b[j] = 2 * a[j];
  }
  return dd;
}
void vec_add_kernel(const int *a,const int b[10000],int *c,int inc)
{
  y=x;
  int b_buf_0[10000];
  long _memcpy_i_1;
  
#pragma unroll 16
  for (_memcpy_i_1 = 0; _memcpy_i_1 < (sizeof(int ) * ((unsigned long )10000) / 4); ++_memcpy_i_1) {
    long total_offset1 = (0 * 10000 + 0);
    long total_offset2 = (0 * 0 + 0);
    b_buf_0[total_offset1 + _memcpy_i_1] = b[total_offset2 + _memcpy_i_1];
  }
  int a_buf_0[10000];
  long _memcpy_i_0;
  
#pragma unroll 16
  for (_memcpy_i_0 = 0; _memcpy_i_0 < (sizeof(int ) * ((unsigned long )10000) / 4); ++_memcpy_i_0) {
    long total_offset1 = (0 * 10000 + 0);
    long total_offset2 = (0 * 0 + 0);
    a_buf_0[total_offset1 + _memcpy_i_0] = a[total_offset2 + _memcpy_i_0];
  }
  int c_buf_0[10000];
  __merlin_access_range(c,0,9999UL);
  __merlin_access_range(b,0,9999UL);
  __merlin_access_range(a,0,9999UL);
  
#pragma ACCEL interface variable=a max_depth=10000 depth=10000
  
#pragma ACCEL interface variable=b max_depth=10000 depth=10000
  
#pragma ACCEL interface variable=c max_depth=10000 depth=10000
  int j;
  for (j = 0; j < 10000; j++) {
    c_buf_0[j] = a_buf_0[j] + b_buf_0[j] + inc + m + n + aa[1][1] + adder(1, bb[0][0]);
  }
  adder_2(a_buf_0, cc);
  long _memcpy_i_2;
  
#pragma unroll 16
  for (_memcpy_i_2 = 0; _memcpy_i_2 < (sizeof(int ) * ((unsigned long )10000) / 4); ++_memcpy_i_2) {
    long total_offset1 = (0 * 0 + 0);
    long total_offset2 = (0 * 10000 + 0);
    c[total_offset1 + _memcpy_i_2] = c_buf_0[total_offset2 + _memcpy_i_2];
  }
  const int * ee = a;
  return ee;
}
