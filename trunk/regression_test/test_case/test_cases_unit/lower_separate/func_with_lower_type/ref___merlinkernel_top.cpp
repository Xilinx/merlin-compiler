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

static void p_ZN6A_int_3setIEEi_1(struct A_int_ *this_,int val)
{
  this_ -> a = val;
}

static void p_ZN6A_int_4incrIEEv_1(struct A_int_ *this_)
{
  this_ -> a++;
}

static void sub_1(struct A_int_ *a)
{
  p_ZN6A_int_4incrIEEv_1(&a[0]);
}

static int p_ZNK6A_int_3getIEEv_1(const struct A_int_ *this_)
{
  return this_ -> a;
}
#pragma ACCEL kernel

void top(int *a)
{
  struct A_int_ tmp;
  p_ZN6A_int_3setIEEi_1(&tmp,a[1]);
  sub_1(&tmp);
  a[2] = p_ZNK6A_int_3getIEEv_1(&tmp);
}
