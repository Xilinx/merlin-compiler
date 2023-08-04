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
static const int _ZN2ns2FCE = 10;

static void p_ZN2ns1AC2Ev_1(struct ns_A *this_)
{
// the following 'a' should be updated into "this_->a"
  
#pragma HLS array_patition variable=this_->a factor=_ZN2ns2FCE
  this_ -> a[0] = 1;
}

static void p_ZN2ns1A3setEii_1(struct ns_A *this_,int i,int val)
{
  this_ -> a[i] = val;
}
#pragma ACCEL kernel

void top(int *a)
{
  struct ns_A tmp;
  p_ZN2ns1AC2Ev_1(&tmp);
  ;
  for (int i = 0; i < 10; ++i) {
    p_ZN2ns1A3setEii_1(&tmp,i,a[i]);
  }
}
