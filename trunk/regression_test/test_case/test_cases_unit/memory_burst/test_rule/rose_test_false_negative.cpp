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
#include<assert.h>
extern int __merlin_include__GB_assert_h_1;
extern int __merlin_include__GB_string_h_0;

void sub(int *a,int offset)
// Original: #pragma ACCEL cache
{
  int a_buf[100];
  memcpy((void *)(&a_buf[0]),(const void *)(&a[offset * 100]),sizeof(int ) * 100);
  for (int i = 0; i < 100; ++i) {
    a_buf[i] += 100;
  }
  memcpy((void *)(&a[offset * 100]),(const void *)(&a_buf[0]),sizeof(int ) * 100);
}
#pragma ACCEL kernel

void top(int *a)
{
  for (int i = 0; i < 100; ++i) {
    sub(a,i);
  }
}

void sub2(int *a,int offset)
// Original: #pragma ACCEL cache
{
  int a_buf[199];
{
    unsigned long merlin_len = offset;
    assert(merlin_len <= 199L);
    memcpy((void *)(&a_buf[0]),(const void *)(&a[0]),sizeof(int ) * merlin_len);
  }
  for (int i = 0; i < offset; ++i) {
    a_buf[i] += 100;
  }
{
    unsigned long merlin_len_0 = offset;
    assert(merlin_len_0 <= 199L);
    memcpy((void *)(&a[0]),(const void *)(&a_buf[0]),sizeof(int ) * merlin_len_0);
  }
}
#pragma ACCEL kernel

void top2(int *a)
{
  for (int i = 100; i < 200; ++i) {
    sub2(a,i);
  }
}
