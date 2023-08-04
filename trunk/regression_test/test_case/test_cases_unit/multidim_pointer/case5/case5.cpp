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



static int vec_add_kernel_dummy_pos;
extern "C" { 

#pragma ACCEL kernel
__kernel void vec_add_kernel(int inc)
{
int a[10240];
int c[20480];  
  
#pragma HLS INTERFACE s_axilite port=inc bundle=control
  
#pragma HLS INTERFACE s_axilite port=return bundle=control
  
  int *B[10];
  
#pragma HLS array_partition variable=B complete dim=1
  for (int i = 0; i < 10; i++) 
{
    
#pragma HLS unroll
    B[i] = a + i * 1024;
  }
  int **C = B;
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 1024; j++) 
{
      
#pragma HLS pipeline
      C[i][j] += 1;
    }
  }
#pragma HLS array_partition variable=B complete dim=1
  for (int i = 0; i < 10; i++) 
{
    
#pragma HLS unroll
    B[i] = c + i * 2048;
  }
  C[1][1] ++;
}
}
