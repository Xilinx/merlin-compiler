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

static void p_ZN6float2C2Ev_1(struct float2 *this_)
{
  (this_ -> x = ((float )0) , this_ -> y = ((float )0));
}

static struct float2 &p_ZN6float2aSERK6float2_1(struct float2 *this_,const struct float2 &other)
{
  this_ -> x = other . x;
  this_ -> y = other . y;
  return  *this_;
}
#pragma ACCEL kernel

void foo()
{
  struct float2 a;
  p_ZN6float2C2Ev_1(&a);
  ;
  struct float2 b;
  p_ZN6float2C2Ev_1(&b);
  ;
  p_ZN6float2aSERK6float2_1(&b,a);
  struct float2x8 data;
  p_ZN6float2C2Ev_1(&data . i0);
  p_ZN6float2C2Ev_1(&data . i1);
  ;
  struct float2x8 data1;
  p_ZN6float2C2Ev_1(&data1 . i0);
  p_ZN6float2C2Ev_1(&data1 . i1);
  ;
  data1 = data;
  p_ZN6float2aSERK6float2_1(&data1 . i0,data . i0);
  p_ZN6float2aSERK6float2_1(&data1 . i1,data . i1);
}
