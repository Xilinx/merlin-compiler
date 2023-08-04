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
#include <assert.h>
#if 0
#else
#pragma ACCEL kernel

void func5(int *b)
{
  
#pragma ACCEL interface variable=b depth=14 max_depth=14
  b;
}

void main5()
{
  int *a = (int *)(malloc(14 * sizeof(int )));
  func5(a);
}
#pragma ACCEL kernel

void func4(int *b)
{
  
#pragma ACCEL interface variable=b depth=1024 max_depth=1024
  b;
}

void main4()
{
// = (int*)malloc(14 * sizeof(int));
  int (*a)[1024];
  func4(a[0]);
}
#pragma ACCEL kernel

void func1(int *a)
{
  
#pragma ACCEL interface variable=a depth=10,1024,30 max_depth=10,1024,30
// assert( a != 0);
  int *f0 = &a[30];
  f0[0];
}

struct my_struct 
{
  
#pragma ACCEL attribute depth=50
  int *buf;
}
;

void func_sub(int *e)
{
  float *g = (float *)e;
  g[0];
}
#pragma ACCEL kernel

void func(int *a,int *b,int *cc,int n)
{
  
#pragma ACCEL interface variable=cc depth=3,5 max_depth=3,5
  
#pragma ACCEL interface variable=a depth=1024,n,128 max_depth=1024,100,128
  
#pragma ACCEL interface variable=b depth=10,10 max_depth=10,10
  n <= 100?((void )0) : __assert_fail("n <=100","test0.c",66,__PRETTY_FUNCTION__);
  int i;
  for (i = 0; i < 100; i++) {
    int *f = &b[i * 10L];
    int **f0;
    cc[i * 5L] = a[256 + i * 12800L] + b[3 + i * 10L] + f[3] + f0[0][0];
    cc[i * 5L] = f0[2][2];
    func_sub(&a[12800]);
    func_sub(&a[25600]);
  }
}
#if 0
#endif
#endif 
