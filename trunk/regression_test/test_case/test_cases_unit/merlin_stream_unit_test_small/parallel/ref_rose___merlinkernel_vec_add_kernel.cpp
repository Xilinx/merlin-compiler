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
#define get_compute_id(x) x
extern int __merlin_include__GB__0;
#pragma OPENCL EXTENSION cl_intel_channels : enable
#pragma channel int ch_c[2] __attribute__((depth((unsigned long )10240)));
typedef int __merlin_channel_typedef;
#include "channel_def.h"
__merlin_channel_typedef ch_c[2];
#pragma channel int ch_b[2] __attribute__((depth((unsigned long )10240)));
typedef int __merlin_channel_typedef;
#include "channel_def.h"
::__merlin_channel_typedef ch_b[2];
#pragma channel int ch_a[2] __attribute__((depth((unsigned long )10240)));
typedef int __merlin_channel_typedef;
#include "channel_def.h"
::__merlin_channel_typedef ch_a[2];
#include "cmost.h"
#include <merlin_stream.h>
#include "merlin_type_define.h"
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();

static void __merlin_org_compute(int id)
{
  int j;
  for (j = 0; j < 10240; j++) {
    int tmp_a;
    int tmp_b;
    int tmp_c;
    tmp_a = read_channel_altera_ch_a__merlinalterafuncend(ch_a[id]);
    tmp_b = read_channel_altera_ch_b__merlinalterafuncend(ch_b[id]);
    tmp_c = tmp_a + tmp_b + 1;
    write_channel_altera_ch_c__merlinalterafuncend(ch_c[id],tmp_c);
  }
}
 __attribute__ (( autorun))
 __attribute__ (( num_compute_units(2)))
 __attribute__ (( max_global_work_dim(0)))

__kernel void compute()
{
  int id = get_compute_id(0);
  while(1){
    __merlin_org_compute(id);
  }
}
static void __merlin_dummy_kernel_pragma();
// Original: #pragma ACCEL kernel

__kernel void vec_add_kernel(__global int * restrict a,__global int * restrict b,__global int * restrict c,int inc)
{
  
#pragma ACCEL interface variable=c max_depth=2048000 depth=2048000
  
#pragma ACCEL interface variable=b max_depth=2048000 depth=2048000
  
#pragma ACCEL interface variable=a max_depth=2048000 depth=2048000
  int i1;
  int i2;
  int j;
  for (i1 = 0; i1 < 100; i1++) {
    for (i2 = 0; i2 < 10240; i2++) {
      
#pragma unroll
      for (j = 0; j < 2; j++) 
// Original: #pragma ACCEL parallel
// Original: #pragma ACCEL PARALLEL COMPLETE
{
        write_channel_altera_ch_a__merlinalterafuncend(ch_a[j],a[i1 * 2 * 10240 + i2 * 2 + j]);
      }
    }
    for (i2 = 0; i2 < 10240; i2++) {
      
#pragma unroll
      for (j = 0; j < 2; j++) 
// Original: #pragma ACCEL parallel
// Original: #pragma ACCEL PARALLEL COMPLETE
{
        write_channel_altera_ch_b__merlinalterafuncend(ch_b[j],b[i1 * 2 * 10240 + i2 * 2 + j]);
      }
    }
    
#pragma ACCEL spawn-position kernel=compute factor=2
    for (i2 = 0; i2 < 10240; i2++) {
      
#pragma unroll
      for (j = 0; j < 2; j++) 
// Original: #pragma ACCEL parallel
// Original: #pragma ACCEL PARALLEL COMPLETE
{
        c[i1 * 2 * 10240 + i2 * 2 + j] = read_channel_altera_ch_c__merlinalterafuncend(ch_c[j]);
      }
    }
  }
}
