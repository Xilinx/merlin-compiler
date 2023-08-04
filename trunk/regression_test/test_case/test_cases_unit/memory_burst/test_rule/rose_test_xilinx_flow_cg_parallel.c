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
#include "cmost.h"
#include <assert.h>
extern int __merlin_include__GB_assert_h_1;
extern int __merlin_include__GB_string_h_0;
#pragma ACCEL kernel

void top(int a[100],int b[10])
{
  int b_buf[10];
  int a_buf[10][10];
  int i;
{
    memcpy((void *)b_buf,(const void *)(&b[0]),sizeof(int ) * 10);
  }
  for (i = 0; i < 10; ++i) {
    
#pragma ACCEL parallel
    for (int j = 0; j < 10; ++j) {
      a_buf[i - 0L][j] = b_buf[j];
    }
  }
{
    memcpy((void *)(&a[0]),(const void *)a_buf[0],sizeof(int ) * 100);
  }
  return ;
}
#pragma ACCEL kernel

void top2(int a[1000000],int b[10])
{
  int b_buf[10];
  int i;
  int j;
  memcpy((void *)(&b_buf[0]),(const void *)(&b[0]),sizeof(int ) * 10);
  for (i = 0; i < 100000; ++i) 
/* Original: #pragma ACCEL parallel */
{
    for (j = 0; j < 10; ++j) {
      a[i * 10 + j] = b_buf[j];
    }
  }
  return ;
}
#pragma ACCEL kernel

void top3(int a[1000000],int b[10])
{
  int b_buf_0[10];
  int b_buf[10];
  int i;
  int j;
  int k;
{
    memcpy((void *)b_buf,(const void *)(&b[0]),sizeof(int ) * 10);
  }
  memcpy((void *)(&b_buf_0[0]),(const void *)(&b[0]),sizeof(int ) * 10);
  for (k = 0; k < 10; ++k) {
    int a_buf_0[10];
    int a_buf[100][10];
    for (i = 0; i < 100; ++i) {
      
#pragma ACCEL parallel
      for (j = 0; j < 10; ++j) {
        a_buf[i - 0L][j] = b_buf[j] + 1;
      }
    }
{
      memcpy((void *)(&a[0]),(const void *)a_buf[0],sizeof(int ) * 1000);
    }
    for (i = 0; i < 10; ++i) {
      a_buf_0[i] = b_buf_0[i] + 2;
    }
    memcpy((void *)(&a[1000]),(const void *)(&a_buf_0[0]),sizeof(int ) * 10);
  }
  return ;
}
#pragma ACCEL kernel

void top4(int a[100],int b[10],int size)
{
  int b_buf[9];
  int a_buf[10][9];
  int i;
  size < 10?((void )0) : __assert_fail("size < 10","test_xilinx_flow_cg_parallel.c",47,__PRETTY_FUNCTION__);
{
    if (size >= 0) {
      memcpy((void *)b_buf,(const void *)(&b[0]),sizeof(int ) * size);
    }
  }
  for (i = 0; i < 10; ++i) {
    
#pragma ACCEL parallel
    for (int j = 0; j < size; ++j) {
      a_buf[i - 0L][j] = b_buf[j];
    }
  }
{
    int i;
    for (i = 0; i < 10; ++i) {
      unsigned long merlin_len = size;
      if (size >= 0) {
        assert(merlin_len <= 9L);
        memcpy((void *)(&a[i * 10]),(const void *)a_buf[i - 0L],sizeof(int ) * merlin_len);
      }
    }
  }
  return ;
}
