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

int func(__global int *B[10])
{
  return 1;
}

int func2(__global const int *B[10])
{
  return 1;
}

__kernel void vec_add_kernel(__global int * restrict a,int inc)
{
  
#pragma ACCEL interface variable=a max_depth=10240 depth=10240
  __global int *B[10];
  int m = func(B);
  int n = func2((__global const int ** )B);
  
#pragma unroll
  for (int i = 0; i < 10; i++) {
    B[i] = a + i * 1024;
  }
  __global int **C = B;
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 1024; j++) {
      C[i][j] += 1;
    }
  }
}
