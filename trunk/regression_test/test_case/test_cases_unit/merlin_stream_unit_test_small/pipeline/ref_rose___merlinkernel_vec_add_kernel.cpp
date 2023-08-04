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
#pragma OPENCL EXTENSION cl_intel_channels : enable
#pragma channel int ch_c __attribute__((depth((unsigned long )1024)));
typedef int __merlin_channel_typedef;
#include "channel_def.h"
__merlin_channel_typedef ch_c;
#pragma channel int ch_b __attribute__((depth((unsigned long )1024)));
typedef int __merlin_channel_typedef;
#include "channel_def.h"
::__merlin_channel_typedef ch_b;
#pragma channel int ch_a __attribute__((depth((unsigned long )1024)));
typedef int __merlin_channel_typedef;
#include "channel_def.h"
::__merlin_channel_typedef ch_a;
#include <merlin_stream.h>
#include "merlin_type_define.h"
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();

static void prefetch_a_org(__global int a[1024])
{
  int j;
  for (j = 0; j < 1024; j++) {
    write_channel_altera_ch_a__merlinalterafuncend(ch_a,a[j]);
  }
}

__kernel void prefetch_a(__global int * restrict a)
{
  int i;
  for (i = 0; i < 1024; i++) {
    prefetch_a_org(a + i * 1024);
  }
}

static void prefetch_b_org(__global int b[1024])
{
  int j;
  for (j = 0; j < 1024; j++) {
    write_channel_altera_ch_b__merlinalterafuncend(ch_b,b[j]);
  }
}

__kernel void prefetch_b(__global int * restrict b)
{
  int i;
  for (i = 0; i < 1024; i++) {
    prefetch_b_org(b + i * 1024);
  }
}

static void compute_org()
{
  int j;
  for (j = 0; j < 1024; j++) {
    int tmp_a;
    int tmp_b;
    int tmp_c;
    tmp_a = read_channel_altera_ch_a__merlinalterafuncend(ch_a);
    tmp_b = read_channel_altera_ch_b__merlinalterafuncend(ch_b);
    tmp_c = tmp_a + tmp_b + 1;
    write_channel_altera_ch_c__merlinalterafuncend(ch_c,tmp_c);
  }
}

static void __merlin_org_compute()
{
  compute_org();
}
 __attribute__ (( autorun))
 __attribute__ (( max_global_work_dim(0)))

__kernel void compute()
{
  while(1){
    __merlin_org_compute();
  }
}

static void prefetch_c_org(__global int c[1024])
{
  int j;
  for (j = 0; j < 1024; j++) {
    c[j] = read_channel_altera_ch_c__merlinalterafuncend(ch_c);
  }
}

__kernel void prefetch_c(__global int * restrict c)
{
  int i;
  for (i = 0; i < 1024; i++) {
    prefetch_c_org(c + i * 1024);
  }
}
static void __merlin_dummy_kernel_pragma();
// Original: #pragma ACCEL kernel

__kernel void vec_add_kernel(__global int * restrict a,__global int * restrict b,__global int * restrict c,int inc)
{
  
#pragma ACCEL interface variable=c max_depth=1048576 depth=1048576
  
#pragma ACCEL interface variable=b max_depth=1048576 depth=1048576
  
#pragma ACCEL interface variable=a max_depth=1048576 depth=1048576
  
#pragma ACCEL spawn-position kernel=prefetch_a
  
#pragma ACCEL spawn-position kernel=prefetch_b
  int i;
  for (i = 0; i < 1024; i++) {
    
#pragma ACCEL spawn-position kernel=compute
  }
  
#pragma ACCEL spawn-position kernel=prefetch_c
}
