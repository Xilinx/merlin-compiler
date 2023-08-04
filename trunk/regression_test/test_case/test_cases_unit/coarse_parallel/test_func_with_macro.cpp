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
#define A_BUF a_buf
#define A_BUF_DECL  int A_BUF[100];
void func(int *a)
{
  //int i;
  for (int i0 = 0; i0 < 10; ++i0) {
    for (int ii = 0; ii < 10; ++ii) {
#pragma ACCEL parallel
      int i = i0 * 10 + ii;
    a[i] = i;
    }
  }
}
#pragma ACCEL kernel

void top(int *a,int *b,int *c)
{
  for (int i = 0; i < 10000 / 100; ++i) {
    A_BUF_DECL
    for (int j = 0; j < 100; ++j) {
#pragma ACCEL parallel
      func(A_BUF);
    }
  }
}
