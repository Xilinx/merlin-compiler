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

void top(int *a)
{
  int i;
  int j;
  int k;
  int a_buf_0[20][30];
  for (i = 0; i < 10; ++i) {
    for (k = 0; k < 20; ++k) {
      for (j = 0; j < 30; ++j) {
        a_buf_0[k - 0][j] = 0;
      }
    }
  }
{
    int k;
    for (k = 0; k < 20; ++k) {
      memcpy((void *)(&a[k * 10000]),(const void *)a_buf_0[k - 0],sizeof(int ) * 30);
    }
  }
}
#pragma ACCEL kernel

void top2(int *a,int *b)
{
  int b_1_0_buf[1];
  int i;
  int j;
  int k;
  int b_buf_0[20][30];
{
    int k;
    for (k = 0; k < 20; ++k) {
      memcpy((void *)b_buf_0[k - 0],(const void *)(&b[k * 10000]),sizeof(int ) * 30);
    }
  }
{
    b_1_0_buf[0] = b[0];
  }
  for (i = 0; i < 10; ++i) {
    for (k = 0; k < 20; ++k) {
      int a_buf_0[30];
      for (j = 0; j < 30; ++j) {
        a_buf_0[j] = b_buf_0[k - 0][j] + 2;
      }
      memcpy((void *)(&a[k * 10000]),(const void *)(&a_buf_0[0]),sizeof(int ) * 30);
    }
    a[0] += b_1_0_buf[0];
  }
}
