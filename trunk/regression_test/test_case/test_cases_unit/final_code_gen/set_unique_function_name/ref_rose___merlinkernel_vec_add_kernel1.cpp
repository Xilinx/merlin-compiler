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
int test1_merlin_clone1 = 0;
static int test1(int a,int b,int c);

static int test1(int a,int b,int c)
{
  c = a + b;
  double test1_merlin_clone5 = 0;
}
static int test1_merlin_clone0(int a,int b,int c);

static int test1_merlin_clone0(int a,int b,int c)
{
  c = a + b;
}

void vec_add_kernel1_merlin_clone2(int m,int n)
{
}

void vec_add_kernel1_merlin_clone1(int m)
{
}

void vec_add_kernel1()
{
}

void vec_add_kernel2_merlin_clone2(int m,int n)
{
}

void vec_add_kernel2_merlin_clone0(int m)
{
}

void vec_add_kernel2()
{
}

__kernel void vec_add_kernel1_merlin_clone0(__global int * restrict a,__global int * restrict b,__global int * restrict c,int inc)
{
  test1_merlin_clone1 = 0;
  test1(1,2,3);
  test1_merlin_clone0(1,2,3);
  float test1_merlin_clone4 = 0;
  int vec_add_kernel1_merlin_clone0 = 1;
  vec_add_kernel1();
  vec_add_kernel1_merlin_clone1(1);
  vec_add_kernel1_merlin_clone2(1,2);
  vec_add_kernel2();
  vec_add_kernel2_merlin_clone0(1);
  vec_add_kernel2_merlin_clone2(1,2);
}
