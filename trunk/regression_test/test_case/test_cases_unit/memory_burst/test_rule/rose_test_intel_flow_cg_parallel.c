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
#include "cmost.h"
extern int __merlin_include__GB_string_h_0;

void sub_0(int *a,int *b,int c)
{
  for (int j = 0; j < 10; ++j) {
    a[j] = b[j];
  }
}
#pragma ACCEL kernel

void top(int a[100],int b[10],int c[10])
{
  int c_3_0_buf[10];
  int b_buf[10];
  int a_buf[10][10];
  int i;
{
    memcpy((void *)b_buf,(const void *)(&b[0]),sizeof(int ) * 10);
  }
  for (i = 0; i < 10; ++i) {
    
#pragma ACCEL parallel
    sub_0(a_buf[i - 0L],b_buf,c_3_0_buf[i]);
  }
{
    memcpy((void *)(&a[0]),(const void *)a_buf[0],sizeof(int ) * 100);
  }
  return ;
}

void sub(int *a,int *b)
{
  __merlin_access_range(a,0,9);
  __merlin_access_range(b,0,9);
  int j = 0;
  while(j < 10){
    a[j] = b[j];
    j++;
  }
}
#pragma ACCEL kernel

void top2(int a[100],int b[10])
{
  int b_buf[10];
  int a_buf[100];
  int i;
  int j;
  memcpy((void *)(&a_buf[0]),(const void *)(&a[0]),sizeof(int ) * 100);
  memcpy((void *)(&b_buf[0]),(const void *)(&b[0]),sizeof(int ) * 10);
  for (i = 0; i < 10; ++i) 
/* Original: #pragma ACCEL parallel */
{
    sub(&a_buf[i * 10],b_buf);
  }
  memcpy((void *)(&a[0]),(const void *)(&a_buf[0]),sizeof(int ) * 100);
  return ;
}
