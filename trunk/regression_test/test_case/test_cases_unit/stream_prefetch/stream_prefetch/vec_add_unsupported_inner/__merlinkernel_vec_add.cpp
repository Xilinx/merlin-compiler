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
#include "cmost.h"
static void __merlin_dummy_extern_int_merlin_include_G_();

static void func(int *a)
{
//int i;
  for (int i0 = 0; i0 < 10; ++i0) {
    for (int ii = 0; ii < 10; ++ii) 
// Original: #pragma ACCEL parallel
// Original: #pragma ACCEL PARALLEL COMPLETE
{
      
#pragma ACCEL PARALLEL COMPLETE 
      int i = i0 * 10 + ii;
      if (i0 > 10) {
        i = 0;
      }
      a[i] = 0;
    }
  }
}
static void __merlin_dummy_kernel_pragma();
// Original: #pragma ACCEL kernel

void top(int *a)
{
  __merlin_access_range(a,0,999999UL);
  
#pragma ACCEL interface variable=a stream_prefetch=on max_depth=1000000 depth=1000000
  int i;
  int j;
  for (i = 0; i < 10000 / 100; ++i) {
    for (j = 0; j < 100; ++j) 
// Original: #pragma ACCEL parallel
// Original: #pragma ACCEL PARALLEL COMPLETE
{
      
#pragma ACCEL PARALLEL COMPLETE 
      func(a);
    }
  }
}
