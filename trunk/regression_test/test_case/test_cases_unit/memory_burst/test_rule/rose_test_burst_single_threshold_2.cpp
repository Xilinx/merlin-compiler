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
// 1. loop structure
// 2. function structure
// 3. access pattern
// 4. pragma modes
extern int __merlin_include__GB_string_h_0;
#pragma ACCEL kernel

void func_top_0(int *a,int *b)
{
  
#pragma ACCEL interface variable=a max_burst_size=1000
  int b_buf[100000];
  memcpy((void *)(&b_buf[0]),(const void *)(&b[0]),sizeof(int ) * 100000);
  for (int i = 0; i < 100000; ++i) {
    
#pragma ACCEL pipeline
    a[i] = b_buf[i] + 2;
  }
}
#pragma ACCEL kernel

void func_top_1(int *a,int *b)
{
  
#pragma ACCEL interface variable=a
  int b_buf[1000000];
  int a_buf[1000000];
  memcpy((void *)(&b_buf[0]),(const void *)(&b[0]),sizeof(int ) * 1000000);
  for (int i = 0; i < 1000000; ++i) {
    
#pragma ACCEL pipeline
    a_buf[i] = b_buf[i] + 2;
  }
  memcpy((void *)(&a[0]),(const void *)(&a_buf[0]),sizeof(int ) * 1000000);
}

int main()
{
  int *a;
  int *b;
  int *c;
  int *aa;
  int *bb;
  int *cc;
  int a_s;
  int b_s;
  int c_s;
  func_top_0(a,b);
  func_top_1(a,b);
  return 0;
}
