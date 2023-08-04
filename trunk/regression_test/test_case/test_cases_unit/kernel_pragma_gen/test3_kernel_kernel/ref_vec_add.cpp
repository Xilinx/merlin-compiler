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
#include <stdio.h>
#define AOCL_ALIGNMENT 64
#define VEC_SIZE 10000
void vec_add_kernel1(int *a,int *b,int *c,int inc);
void vec_add_kernel2(int *a,int *b,int *c,int inc);

int main()
{
  int i;
  int inc = 1;
  int a[10000];
  int b[10000];
  int c1[10000];
  int c2[10000];
  for (i = 0; i < 10000; i++) {
    a[i] = i;
    b[i] = 2 * i;
    c1[i] = 1;
    c2[i] = 1;
  }
//#pragma ACCEL task name="vec_add1"
  vec_add_kernel1(a,b,c1,inc);
//#pragma ACCEL task name="vec_add2"
  vec_add_kernel2(a,b,c2,inc);
  for (i = 0; i < 10000; i++) {
    if (c1[i] != a[i] + b[i] + inc) {
      printf("Test1 failed\n");
      return 0;
    }
    if (c2[i] != a[i] + b[i] + inc + 10000) {
      printf("Test2 failed\n");
      return 0;
    }
  }
  printf("Test passed\n");
  return 1;
}
