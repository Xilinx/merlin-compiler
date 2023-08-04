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
#include "merlin_type_define.h"
extern int __merlin_include__GB_merlin_type_define_h_0;

static void sub_function_3_1(int *a,int *b,int *c)
{
  int j;
  for (j = 0; j < 10000; j++) 
    c[j] = a[j] + b[j] + 3;
}

static void sub_function_2_1(int *a,int *b,int *c)
{
  int j;
  sub_function_3_1(a,b,c);
  for (j = 0; j < 10000; j++) 
    c[j] = a[j] + b[j] + 2;
}

static void sub_function_1_1(int *a,int *b,int *c)
{
  int j;
  sub_function_2_1(a,b,c);
  for (j = 0; j < 10000; j++) 
    c[j] = a[j] + b[j] + 1;
}

static void sub_function_4_1(int *a,int *b,int *c)
{
  int j;
  sub_function_1_1(a,b,c);
  for (j = 0; j < 10000; j++) 
    c[j] = a[j] + b[j] + 4;
}

static void sub_function_5_1(int *a,int *b,int *c)
{
  int j;
  sub_function_2_1(a,b,c);
  for (j = 0; j < 10000; j++) 
    c[j] = a[j] + b[j] + 5;
}

static void sub_function_6_1(int *a,int *b,int *c)
{
  int j;
  for (j = 0; j < 10000; j++) 
    c[j] = a[j] + b[j] + 6;
}

static void sub_function_7_1(int *a,int *b,int *c)
{
  int j;
  sub_function_6_1(a,b,c);
  for (j = 0; j < 10000; j++) 
    c[j] = a[j] + b[j] + 7;
}
#pragma ACCEL kernel

void vec_add_kernel(int *a,int *b,int *c,int inc)
{
  sub_function_1_1(a,b,c);
  sub_function_2_1(a,b,c);
  sub_function_4_1(a,b,c);
  sub_function_5_1(a,b,c);
  sub_function_6_1(a,b,c);
  sub_function_7_1(a,b,c);
  int j;
  
#pragma ACCEL pipeline
  
#pragma ACCEL parallel factor = 16
  for (j = 0; j < 10000; j++) 
    c[j] = a[j] + b[j] + inc;
}
