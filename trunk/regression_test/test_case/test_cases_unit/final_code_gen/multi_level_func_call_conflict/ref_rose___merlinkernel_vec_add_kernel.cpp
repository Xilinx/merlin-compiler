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

static int func4_1(int *a,int *b)
{
  for (int i = 0; i < 10000; i++) {
    b[i] = a[i];
  }
}

static int func4(__global int *a,int *b)
{
  for (int i = 0; i < 10000; i++) {
    b[i] = a[i];
  }
}

static int func3_1(int *a,int *b)
{
  return func4_1(a,b);
}

static int func3(__global int *a,int *b)
{
  return func4(a,b);
}

static int func2_1(int *a)
{
  int m[10000];
  int n[10000];
  for (int i = 0; i < 10000; i++) {
    m[i] = 1;
    n[i] = 1;
  }
  func3_1(a,n);
}

static int func2(__global int *a)
{
  int m[10000];
  int n[10000];
  for (int i = 0; i < 10000; i++) {
    m[i] = 1;
    n[i] = 1;
  }
  func3(a,n);
}

static int func1_1(__global int *a,int *b)
{
  for (int i = 0; i < 10000; i++) {
    b[i] = a[i];
  }
  func2(a);
}

static int func1_2(int *a,int *b)
{
  for (int i = 0; i < 10000; i++) {
    b[i] = a[i];
  }
  func2_1(a);
}

static int func1(__global int *a,__global int *b)
{
  for (int i = 0; i < 10000; i++) {
    b[i] = a[i];
  }
  func2(a);
}
// Original: #pragma ACCEL kernel

__kernel void vec_add_kernel(__global int * restrict a,__global int * restrict b)
{
  
#pragma ACCEL interface variable = a max_depth = 10000 depth = 10000
  
#pragma ACCEL interface variable = b max_depth = 10000 depth = 10000
  int m[10000];
  int n[10000];
  for (int i = 0; i < 10000; i++) {
    m[i] = 1;
    n[i] = 1;
  }
  func1(a,b);
  func1_2(m,n);
  func1_1(a,n);
}
