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
#define TILE_SIZE 100

#pragma ACCEL kernel
void vec_add_kernel(int a[VEC_SIZE],int b[VEC_SIZE],int c[VEC_SIZE],int inc)
{
  int i, j;
  for (i = 0; i < VEC_SIZE / TILE_SIZE; i++) {
      // Test condition with tiling
#pragma ACCEL PIPELINE auto{options:I0=["off","","flatten"]}
#pragma ACCEL TILE FACTOR=auto{options: T0 =[2**x for x in range(0,10)]}
      if (i > 10) { 
          // Test if traversed even within other blocks
          // Test condition with pipeline shadowing
          for (j = 0; j < TILE_SIZE; j++) {
#pragma ACCEL TILE FACTOR=auto{options:T0_0_0=[2**x for x in range(0,7)]}
            int idx = i * TILE_SIZE + j;
            c[idx] = a[idx] + b[idx] + inc;
          }
      }
  }

  // Test if ignored due to conflict
  for (j = 0; j < VEC_SIZE; j++) {
#pragma ACCEL PARALLEL FACTOR=4
    c[j] += a[j] + b[j] + inc;
  }

  // Test if ignored due to unbound loop
  for (j = 0; j < inc; j++) {
      c[j] += j;
  }

  // Test if fully unrolled when tripcount < 10
  for (j = 0; j < 3; j++) {
      c[j] += a[j];
  }
}
