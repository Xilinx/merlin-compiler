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
#define VEC_SIZE 64
#define VEC_flat_SIZE 4096
#define aa(p, n) (a[(p)*VEC_SIZE + (n)])
#define bb(p, n) (b[(p)*VEC_SIZE + (n)])
#define cc(p, n) (c[(p)*VEC_SIZE + (n)])
#define a_buf(p, n) (abuf[(p)*VEC_SIZE + (n)])

#pragma ACCEL kernel
void matmul_kernel(int *a,int *b,int *c)
{
  int c_2_0_buf[4096];
  int b_2_0_buf[64];
  int a_2_0_buf[4033];
  int i;
  int j;
  int k;
	int i0;

  memcpy(&c_2_0_buf[0],&c[0],sizeof(int ) * 4096);
  for (k = 0; k < 64; k++) {
    memcpy(&a_2_0_buf[0],&a[k],sizeof(int ) * 4033);
    memcpy(&b_2_0_buf[0],&b[64 * k],sizeof(int ) * 64);
    for (i0 = 0; i0 < 64; i0++) {
    for (i = 0; i < 64; i++) {
#pragma ACCEL pipeline 
      for (j = 0; j < 64; j++) {
        
#pragma ACCEL parallel complete
        c_2_0_buf[64 * k + j] += a_2_0_buf[64 * i] * b_2_0_buf[j];
      }
    }
  }
	}
  memcpy(&c[0],&c_2_0_buf[0],sizeof(int ) * 4096);
}
