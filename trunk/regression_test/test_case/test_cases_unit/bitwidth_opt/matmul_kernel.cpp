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
#include<string.h>
#define VEC_SIZE 512 
#define aa(p, n) (a[(p)*VEC_SIZE + (n)])
#define bb(p, n) (b[(p)*VEC_SIZE + (n)])
#define cc(p, n) (c[(p)*VEC_SIZE + (n)])

#pragma ACCEL kernel
void matmul_kernel(float *a,float *b,float *c)
{
  float c_2_0_buf[262144];
  float b_2_0_buf[512];
  float a_2_0_buf[512];
  int i;
  int j;
  int k;
  memcpy(&c_2_0_buf[0],&c[0],sizeof(float ) * 262144);
  for (k = 0; k < 512; k++) {
    
#pragma ACCEL pipeline
    memcpy(&a_2_0_buf[0],&a[512 * k],sizeof(float ) * 512);
    memcpy(&b_2_0_buf[0],&b[512 * k],sizeof(float ) * 512);
    for (i = 0; i < 512; i++) {
      
#pragma ACCEL pipeline
      for (j = 0; j < 512; j++) {
        
#pragma ACCEL parallel complete
        c_2_0_buf[512 * i + j] += a_2_0_buf[i] * b_2_0_buf[j];
      }
    }
  }
  memcpy(&c[0],&c_2_0_buf[0],sizeof(float ) * 262144);
}
