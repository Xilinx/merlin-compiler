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
#include "merlin_type_define.h"
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_kernel_pragma();
/* Original: #pragma ACCEL kernel */

__kernel void top_kernel(enum vec_add_flag flag_a,__global enum vec_add_flag * restrict arr,__global int * restrict merlin_return_value)
{
  
#pragma ACCEL auto_register variable=arr_buf_0
  int arr_buf_0[100];
  long _memcpy_i_0;
  
#pragma unroll 50
  for (_memcpy_i_0 = 0; _memcpy_i_0 < sizeof(int ) * ((unsigned long )100) / 4; ++_memcpy_i_0) {
    long total_offset1 = (0 * 100 + 0);
    long total_offset2 = (0 * 0 + 0);
    arr_buf_0[total_offset1 + _memcpy_i_0] = arr[total_offset2 + _memcpy_i_0];
  }
  
#pragma ACCEL interface variable=merlin_return_value io=WO scope_type=return max_depth=1 depth=1
  
#pragma ACCEL interface variable=arr max_depth=100 depth=100
  int i;
  int accu = 0;
  for (i = 0; i < 100; ++i) {
    if (((unsigned int )yes) == ((unsigned int )flag_a)) {
      accu += arr_buf_0[i];
    }
     else {
      accu -= arr_buf_0[i];
    }
  }
  merlin_return_value[0] = accu;
  return ;
}
