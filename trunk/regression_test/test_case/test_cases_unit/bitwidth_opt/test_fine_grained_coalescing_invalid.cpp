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
extern int __merlin_include__GB_string_h_0;
#pragma ACCEL kernel

void func1_kernel(int a[1024],int b[1024],int c[1024])
{
  for (int i = 0; i < 64; ++i) {
    for (int j = 0; j < 16; ++j) {
      a[i * 16 + j] = b[i * 16 + j] + c[i * 16 + j];
    }
  }
}
#pragma ACCEL kernel

void func2_kernel(int a[1024],int b[1024],int c[1024])
{
  for (int i = 0; i < 64; ++i) {
    int c_buf_0[17];
    int b_buf_0[17];
    int a_buf_0[17];
    for (int j = 0; j < 17; ++j) {
      
#pragma ACCEL parallel
      a[i * 17 + j] = b[i * 17 + j] + c[i * 17 + j];
    }
  }
}
#pragma ACCEL kernel

void func3_kernel(int a[1024],int b[1024],int c[1024])
{
  for (int i = 0; i < 64; ++i) {
    a[i * 17 + 0] = b[i * 17 + 0] + c[i * 17 + 0];
    a[i * 17 + 1] = b[i * 17 + 1] + c[i * 17 + 1];
    a[i * 17 + 2] = b[i * 17 + 2] + c[i * 17 + 2];
    a[i * 17 + 3] = b[i * 17 + 3] + c[i * 17 + 3];
    a[i * 17 + 4] = b[i * 17 + 4] + c[i * 17 + 4];
    a[i * 17 + 5] = b[i * 17 + 5] + c[i * 17 + 5];
    a[i * 17 + 6] = b[i * 17 + 6] + c[i * 17 + 6];
    a[i * 17 + 7] = b[i * 17 + 7] + c[i * 17 + 7];
    a[i * 17 + 8] = b[i * 17 + 8] + c[i * 17 + 8];
    a[i * 17 + 9] = b[i * 17 + 9] + c[i * 17 + 9];
    a[i * 17 + 10] = b[i * 17 + 10] + c[i * 17 + 10];
    a[i * 17 + 11] = b[i * 17 + 11] + c[i * 17 + 11];
    a[i * 17 + 12] = b[i * 17 + 12] + c[i * 17 + 12];
    a[i * 17 + 13] = b[i * 17 + 13] + c[i * 17 + 13];
    a[i * 17 + 14] = b[i * 17 + 14] + c[i * 17 + 14];
    a[i * 17 + 15] = b[i * 17 + 15] + c[i * 17 + 15];
  }
}
#pragma ACCEL kernel
void func4_kernel(int *a) {
  for (int j = 0; j < 16; ++j) {
    for (int i = 0; i < 4; ++i) {
#pragma ACCEL parallel
      for (int r = 0; r < 4; ++r) {
#pragma ACCEL parallel
        a[i * 64 + r * 16 + j] = i * r + j;
      }
    }
  }
}
