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
#define TILE_SIZE 10
#pragma ACCEL kernel

void vec_add_kernel(int a[10000],int b[10000],int c[512],int d[5000][2],int e[10000],int inc)
{
// Test if ignore inc due to scalar
// Test if insert b normally
// Test if insert d (2-D) normally
  
#pragma ACCEL interface variable=e bus_bitwidth=auto{options:bw2=[32,512]; default:32}
  
#pragma ACCEL interface variable=d bus_bitwidth=auto{options:bw1=[32,512]; default:32}
  
#pragma ACCEL interface variable=b bus_bitwidth=auto{options:bw0=[32,512]; default:32}
  
#pragma ACCEL interface variable=d max_depth=5000,2
// Test if ignore due to user-specified 
  
#pragma ACCEL interface variable=a bus_bitwidth=512
// Test if ignore due to short array
  
#pragma ACCEL interface variable=c max_depth=512
  int i;
  int j;
  for (i = 0; i < 10000 / 10; i++) {
    for (j = 0; j < 10; j++) {
      int idx = i * 10 + j;
      c[idx % 512] = a[idx] + b[idx] + inc;
    }
  }
  for (j = 0; j < 10000; j++) {
    c[j % 512] += a[j] + b[j] + inc;
  }
}
