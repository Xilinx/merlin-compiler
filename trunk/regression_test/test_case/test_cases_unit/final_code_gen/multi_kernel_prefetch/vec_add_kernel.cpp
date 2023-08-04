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
#define VEC_SIZE 10000
#define VEC_SIZE2 10000
#define TILE 16
void vec_add_kernel1(int *a,int *b,int *c,int inc);
void vec_add_kernel2(int *d,int *b,int *c,int inc);
#pragma ACCEL task parallel
static void __merlin_dummy_kernel_pragma();

void vec_add_kernel1(int *a,int *b,int *c,int inc)
{
  
#pragma ACCEL interface variable=a depth=10000 stream_prefetch=on
  
#pragma ACCEL interface variable=b depth=10000 stream_prefetch=on
  
#pragma ACCEL interface variable=c depth=10000 stream_prefetch=on
  int j;
  int j0;
  int jj;
  for (j0 = 0; j0 < 10000 / 16; j0++) {
    
#pragma ACCEL parallel
    for (jj = 0; jj < 16; jj++) {
      j = j0 * 16 + jj;
      c[j] = a[j] + b[j] + inc;
    }
  }
}
static void __merlin_dummy_kernel_pragma();

void vec_add_kernel2(int *d,int *b,int *c,int inc)
{
  
#pragma ACCEL interface variable=d depth=10000 stream_prefetch=on
  
#pragma ACCEL interface variable=b depth=10000 stream_prefetch=on
  
#pragma ACCEL interface variable=c depth=10000 stream_prefetch=on
  int j;
  int j0;
  int jj;
  for (j0 = 0; j0 < 10000 / 16; j0++) {
    
#pragma ACCEL parallel
    for (jj = 0; jj < 16; jj++) {
      j = j0 * 16 + jj;
      c[j] = d[j] + b[j] + inc + 10000;
    }
  }
}
