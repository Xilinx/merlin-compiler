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
#pragma OPENCL EXTENSION cl_altera_channels : enable
// Original: #pragma ACCEL array_partition type=channel variable=ch_c_2 dim=1 factor=16
// Original: #pragma ACCEL array_partition type=channel variable=ch_b_1 dim=1 factor=16
// Original: #pragma ACCEL array_partition type=channel variable=ch_a_0 dim=1 factor=16
#include "merlin_stream.h" 
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy__pragma_ACCEL_STREAM_CHANNEL_C_();
#ifndef __defined_ch_c_2_p
#define __defined_ch_c_2_p
#pragma channel int ch_c_2_p[16] __attribute__((depth(32)));
typedef int __merlin_channel_typedef;
#include "channel_def.h"
__merlin_channel_typedef ch_c_2_p[16];
#endif
static void __merlin_dummy__pragma_ACCEL_STREAM_CHANNEL_C_();
#ifndef __defined_ch_b_1_p
#define __defined_ch_b_1_p
#pragma channel int ch_b_1_p[16] __attribute__((depth(32)));
typedef int __merlin_channel_typedef;
#include "channel_def.h"
::__merlin_channel_typedef ch_b_1_p[16];
#endif
static void __merlin_dummy__pragma_ACCEL_STREAM_CHANNEL_C_();
#ifndef __defined_ch_a_0_p
#define __defined_ch_a_0_p
#pragma channel int ch_a_0_p[16] __attribute__((depth(32)));
typedef int __merlin_channel_typedef;
#include "channel_def.h"
::__merlin_channel_typedef ch_a_0_p[16];
#endif
#include "cmost.h"
#include "merlin_type_define.h"
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_kernel_pragma();
// Original: #pragma ACCEL kernel die=SLR0

__kernel void msm_port_a_vec_add_kernel1_0(__global int * restrict a)
{
  int _j0_1;
  int _jj_2;
  for (_j0_1 = 0; _j0_1 < 10000 / 16; _j0_1++) {
    
#pragma unroll
    for (_jj_2 = 0; _jj_2 < 16; _jj_2++) 
// Original: #pragma ACCEL parallel
// Original: #pragma ACCEL PARALLEL COMPLETE
{
      int a_sp_tmp_0;
      a_sp_tmp_0 = a[_j0_1 * 16 + _jj_2];
      write_channel_altera_ch_a_0_p__merlinalterafuncend(ch_a_0_p[_jj_2],a_sp_tmp_0);
    }
  }
}

__kernel void msm_port_b_vec_add_kernel1_0(__global int * restrict b)
{
  int _j0_3;
  int _jj_4;
  for (_j0_3 = 0; _j0_3 < 10000 / 16; _j0_3++) {
    
#pragma unroll
    for (_jj_4 = 0; _jj_4 < 16; _jj_4++) 
// Original: #pragma ACCEL parallel
// Original: #pragma ACCEL PARALLEL COMPLETE
{
      int b_sp_tmp_0;
      b_sp_tmp_0 = b[_j0_3 * 16 + _jj_4];
      write_channel_altera_ch_b_1_p__merlinalterafuncend(ch_b_1_p[_jj_4],b_sp_tmp_0);
    }
  }
}

__kernel void msm_port_c_vec_add_kernel1_0(__global int * restrict c)
{
  int _j0_5;
  int _jj_6;
  for (_j0_5 = 0; _j0_5 < 10000 / 16; _j0_5++) {
    
#pragma unroll
    for (_jj_6 = 0; _jj_6 < 16; _jj_6++) 
// Original: #pragma ACCEL parallel
// Original: #pragma ACCEL PARALLEL COMPLETE
{
      int c_sp_tmp_0;
      c_sp_tmp_0 = ((int )(read_channel_altera_ch_c_2_p__merlinalterafuncend(ch_c_2_p[_jj_6])));
      c[_j0_5 * 16 + _jj_6] = c_sp_tmp_0;
    }
  }
}

__kernel void vec_add_kernel1(__global int * restrict a,__global int * restrict b,__global int * restrict c,int inc)
{
  int _jj_6;
  int _jj_4;
  
#pragma ACCEL STREAM_PORT ACCESS_TYPE=read DIM_NUM=1 NODE=vec_add_kernel1 VARIABLE=b
  int _jj_2;
  
#pragma ACCEL STREAM_PORT ACCESS_TYPE=read DIM_NUM=1 NODE=vec_add_kernel1 VARIABLE=a
  
#pragma ACCEL interface variable=a stream_prefetch=on max_depth=10000 depth=10000 memory_burst=off
  
#pragma ACCEL interface variable=b stream_prefetch=on max_depth=10000 depth=10000 memory_burst=off
  
#pragma ACCEL interface variable=c stream_prefetch=on max_depth=10000 depth=10000 memory_burst=off
  int j;
  int j0;
  int jj;
  for (j0 = 0; j0 < 10000 / 16; j0++) {
    
#pragma unroll
    for (jj = 0; jj < 16; jj++) 
// Original: #pragma ACCEL parallel
// Original: #pragma ACCEL PARALLEL COMPLETE
{
      int c_sn_tmp_0;
      int b_sn_tmp_0;
      int a_sn_tmp_0;
      j = j0 * 16 + jj;
      a_sn_tmp_0 = ((int )(read_channel_altera_ch_a_0_p__merlinalterafuncend(ch_a_0_p[jj])));
      b_sn_tmp_0 = ((int )(read_channel_altera_ch_b_1_p__merlinalterafuncend(ch_b_1_p[jj])));
      c_sn_tmp_0 = a_sn_tmp_0 + b_sn_tmp_0 + inc;
      write_channel_altera_ch_c_2_p__merlinalterafuncend(ch_c_2_p[jj],c_sn_tmp_0);
    }
  }
  int _jj_2_0;
  int _jj_4_0;
  
#pragma ACCEL STREAM_PORT ACCESS_TYPE=write DIM_NUM=1 NODE=vec_add_kernel1 VARIABLE=c
  int _jj_6_0;
}
