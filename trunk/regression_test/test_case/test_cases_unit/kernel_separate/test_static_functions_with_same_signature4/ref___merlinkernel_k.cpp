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

static void f(int a)
{
  a++;
}

static void g1_1(int x)
{
  f(x);
}

static void f_2(int a2)
{
  a2--;
}

static void f_1(int x)
{
  "to make conflict";
  f_2(x);
}

static void g2_1(int x)
{
  f_1(x);
}
#pragma ACCEL kernel

void k()
{
  g1_1(1);
  g2_1(2);
}
