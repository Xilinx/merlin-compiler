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
#define FACTOR 100
const int delta = 100;
void func(int *a,int *b,int *c)
{
  static const int delta2 = 100;
  //int i;
  for (int i0 = 0; i0 < 10; ++i0) {
    for (int ii = 0; ii < 10; ++ii) {
#pragma ACCEL parallel
      int i = i0 * 10 + ii;
    c[i] = a[i] + b[i] + delta + delta2;
    }
  }
}
#pragma ACCEL kernel

void top(int *a,int *b,int *c)
{
  for (int i = 0; i < 10000 / 100; ++i) {
    int c_buf_0[100][100];
    int b_buf_0[100][100];
    int a_buf_0[100][100];
{
      for (int j = 0; j < 100; ++j) {
        
#pragma HLS UNROLL
        memcpy(a_buf_0[j],((const void *)(&a[i * 10000 + j * 100])),sizeof(int ) * 100);
      }
    }
{
      for (int j = 0; j < 100; ++j) {
        
#pragma HLS UNROLL
        memcpy(b_buf_0[j],((const void *)(&b[i * 10000 + j * 100])),sizeof(int ) * 100);
      }
    }
    for (int j = 0; j < 100; ++j) {
      
#pragma ACCEL parallel
      int offset = i * 100 + j;
      func(a_buf_0[j],b_buf_0[j],c_buf_0[j]);
    }
{
      for (int j = 0; j < 100; ++j) {
        
#pragma HLS UNROLL
        memcpy(((void *)(&c[i * 10000 + j * 100])),c_buf_0[j],sizeof(int ) * 100);
      }
    }
  }
}
