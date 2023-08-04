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
#include "merlin_stream.h"
//#include <stdio.h>
#include "_merlin_stream.h"
//NODE=vec_add_kernel PORT=prefetch
extern int __merlin_include__GB_merlin_stream_h_1;
extern int __merlin_include__GB_merlin_type_define_h_0;
#pragma ACCEL STREAM_CHANNEL CHANNEL_DEPTH=32
struct merlin_stream ch_a_0;
#pragma ACCEL kernel

void vec_add_kernel_s0(int a[10000])
{
  
#pragma ACCEL interface variable=a depth=10000
  for (int j = 0; j < 10000; j++) {
    int tmp = a[j];
    merlin_stream_write(&ch_a_0,(void *)(&tmp));
  }
}
