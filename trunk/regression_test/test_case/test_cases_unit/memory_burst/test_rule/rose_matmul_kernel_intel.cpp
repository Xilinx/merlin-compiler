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
#include<assert.h>
#define VEC_SIZE 512 
#define TS 64
#define aa(p, n) (a[(p)*VEC_SIZE + (n)])
#define bb(p, n) (b[(p)*VEC_SIZE + (n)])
#define cc(p, n) (c[(p)*VEC_SIZE + (n)])
extern int __merlin_include__GB_string_h_0;
#pragma ACCEL kernel

void matmul_kernel(float *a,float *b,float *c)
{
  
#pragma ACCEL skip_sync variable=c_buf
  
#pragma ACCEL skip_sync variable=c
  
#pragma ACCEL skip_sync variable=c_buf
  
#pragma ACCEL skip_sync variable=c
  float c_buf[262144];
  float b_buf[262144];
  float a_buf[262144];
  memcpy((void *)(&a_buf[0]),(const void *)(&a[0]),sizeof(float ) * 262144);
  memcpy((void *)(&b_buf[0]),(const void *)(&b[0]),sizeof(float ) * 262144);
  memcpy((void *)(&c_buf[0]),(const void *)(&c[0]),sizeof(float ) * 262144);
  int i;
  int j;
  int k;
  int j0;
  int jj;
  for (k = 0; k < 512; k++) {
    for (i = 0; i < 512; i++) {
      for (j0 = 0; j0 < 512 / 64; j0++) 
// Original: #pragma ACCEL pipeline flatten
{
        
#pragma ACCEL PIPELINE II=1
        for (jj = 0; jj < 64; jj++) {
          
#pragma ACCEL PARALLEL COMPLETE
          j = j0 * 64 + jj;
          c_buf[i * 512 + j0 * 64 + jj] += a_buf[k * 512 + i] * b_buf[j0 * 64 + k * 512 + jj];
        }
      }
    }
  }
  memcpy((void *)(&c[0]),(const void *)(&c_buf[0]),sizeof(float ) * 262144);
}
