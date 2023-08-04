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
#pragma ACCEL kernel
void func1_kernel(int *a,int *b,int *c)
{
  for (int i = 0; i < 64; ++i) {
    for (int j = 0; j < 16; ++j) {
      
#pragma ACCEL parallel
      a[i * 16 +  j] = b[i * 16 + j] + c[i * 16 + j];
    }
  }
}
#pragma ACCEL kernel

void func2_kernel(int *a,int *b,int *c)
{
  
  for (int i = 0; i < 64; ++i) {
    a[i * 16 + 0] = b[i * 16 + 0] + c[i * 16 + 0];
    a[i * 16 + 1] = b[i * 16 + 1] + c[i * 16 + 1];
    a[i * 16 + 2] = b[i * 16 + 2] + c[i * 16 + 2];
    a[i * 16 + 3] = b[i * 16 + 3] + c[i * 16 + 3];
    a[i * 16 + 4] = b[i * 16 + 4] + c[i * 16 + 4];
    a[i * 16 + 5] = b[i * 16 + 5] + c[i * 16 + 5];
    a[i * 16 + 6] = b[i * 16 + 6] + c[i * 16 + 6];
    a[i * 16 + 7] = b[i * 16 + 7] + c[i * 16 + 7];
    a[i * 16 + 8] = b[i * 16 + 8] + c[i * 16 + 8];
    a[i * 16 + 9] = b[i * 16 + 9] + c[i * 16 + 9];
    a[i * 16 + 10] = b[i * 16 + 10] + c[i * 16 + 10];
    a[i * 16 + 11] = b[i * 16 + 11] + c[i * 16 + 11];
    a[i * 16 + 12] = b[i * 16 + 12] + c[i * 16 + 12];
    a[i * 16 + 13] = b[i * 16 + 13] + c[i * 16 + 13];
    a[i * 16 + 14] = b[i * 16 + 14] + c[i * 16 + 14];
    a[i * 16 + 15] = b[i * 16 + 15] + c[i * 16 + 15];
  }
}

//auto lifting 
#pragma ACCEL kernel
void top(int *a, int *b) {
  for (int i = 0; i < 100; ++i) {
    for (int j = 0; j < 16; ++j) {
#pragma ACCEL parallel
      a[i * 16 + j] = b[j] + 2;
    }
  }
}
