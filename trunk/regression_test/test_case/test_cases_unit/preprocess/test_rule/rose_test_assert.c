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
#include<assert.h>
#pragma ACCEL kernel

void func_top(int *a,int *b,int N)
{
  int c = 10;
  c <= 10?((void )0) : __assert_fail("c <= 10","test_assert.c",6,__PRETTY_FUNCTION__);
  N < 100?((void )0) : __assert_fail("N < 100","test_assert.c",7,__PRETTY_FUNCTION__);
  int i;
  for (i = 0; i < N; ++i) {
    a[i] = b[i];
  }
  for (i = 0; i < c; ++i) {
    a[i] = b[i];
  }
}

void sub(int *a,int c)
{
  c < 100?((void )0) : __assert_fail("c < 100","test_assert.c",17,__PRETTY_FUNCTION__);
  for (int i = 0; i < c; ++i) {
    a[i] = i;
  }
}
#pragma ACCEL kernel

void func_top_1(int *a)
{
  int c = 10;
  sub(a,c);
}
