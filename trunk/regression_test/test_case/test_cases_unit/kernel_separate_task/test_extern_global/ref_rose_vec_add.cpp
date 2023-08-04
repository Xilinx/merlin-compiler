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
extern const int a;
const int a = 10;
extern const int b = 10;
static const char c = 20;
static short d = 10;
extern const int f;
const int f = 40;
int g;
extern int h;
int h;
extern int i;
static void __merlin_dummy_kernel_pragma();

int vec_add_kernel1()
{
  return a + b + c + d + f + g + h + i;
}
static void __merlin_dummy_kernel_pragma();

void vec_add_kernel2(int *a,int *b,int *c,int inc)
{
}
static void __merlin_dummy_kernel_pragma();

void vec_add(int *a,int *b,int *c,int inc)
{
  vec_add_kernel1();
  vec_add_kernel2(a,b,c,inc);
}
