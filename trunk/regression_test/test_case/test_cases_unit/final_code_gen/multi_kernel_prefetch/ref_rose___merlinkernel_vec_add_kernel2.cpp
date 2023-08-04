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
// Original: #pragma ACCEL array_partition type=channel variable=ch_c_5 dim=1 factor=16
// Original: #pragma ACCEL array_partition type=channel variable=ch_b_4 dim=1 factor=16
// Original: #pragma ACCEL array_partition type=channel variable=ch_d_3 dim=1 factor=16
#include "merlin_stream.h" 
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy__pragma_ACCEL_STREAM_CHANNEL_C_();
#ifndef __defined_ch_c_5_p
#define __defined_ch_c_5_p
#pragma channel int ch_c_5_p[16] __attribute__((depth(32)));
typedef int __merlin_channel_typedef;
#include "channel_def.h"
__merlin_channel_typedef ch_c_5_p[16];
#endif
static void __merlin_dummy__pragma_ACCEL_STREAM_CHANNEL_C_();
#ifndef __defined_ch_b_4_p
#define __defined_ch_b_4_p
#pragma channel int ch_b_4_p[16] __attribute__((depth(32)));
typedef int __merlin_channel_typedef;
#include "channel_def.h"
::__merlin_channel_typedef ch_b_4_p[16];
#endif
static void __merlin_dummy__pragma_ACCEL_STREAM_CHANNEL_C_();
#ifndef __defined_ch_d_3_p
#define __defined_ch_d_3_p
#pragma channel int ch_d_3_p[16] __attribute__((depth(32)));
typedef int __merlin_channel_typedef;
#include "channel_def.h"
::__merlin_channel_typedef ch_d_3_p[16];
#endif
#include "cmost.h"
#include "merlin_type_define.h"
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_kernel_pragma();
// Original: #pragma ACCEL kernel die=SLR1

__kernel void msm_port_d_vec_add_kernel2_0(__global int * restrict d)
{
  int _j0_7;
  int _jj_8;
  for (_j0_7 = 0; _j0_7 < 10000 / 16; _j0_7++) {
    
#pragma unroll
    for (_jj_8 = 0; _jj_8 < 16; _jj_8++) 
// Original: #pragma ACCEL parallel
// Original: #pragma ACCEL PARALLEL COMPLETE
{
      int d_sp_tmp_0;
      d_sp_tmp_0 = d[_j0_7 * 16 + _jj_8];
      write_channel_altera_ch_d_3_p__merlinalterafuncend(ch_d_3_p[_jj_8],d_sp_tmp_0);
    }
  }
}

__kernel void msm_port_b_vec_add_kernel2_0(__global int * restrict b)
{
  int _j0_9;
  int _jj_10;
  for (_j0_9 = 0; _j0_9 < 10000 / 16; _j0_9++) {
    
#pragma unroll
    for (_jj_10 = 0; _jj_10 < 16; _jj_10++) 
// Original: #pragma ACCEL parallel
// Original: #pragma ACCEL PARALLEL COMPLETE
{
      int b_sp_tmp_0;
      b_sp_tmp_0 = b[_j0_9 * 16 + _jj_10];
      write_channel_altera_ch_b_4_p__merlinalterafuncend(ch_b_4_p[_jj_10],b_sp_tmp_0);
    }
  }
}

__kernel void msm_port_c_vec_add_kernel2_0(__global int * restrict c)
{
  int _j0_11;
  int _jj_12;
  for (_j0_11 = 0; _j0_11 < 10000 / 16; _j0_11++) {
    
#pragma unroll
    for (_jj_12 = 0; _jj_12 < 16; _jj_12++) 
// Original: #pragma ACCEL parallel
// Original: #pragma ACCEL PARALLEL COMPLETE
{
      int c_sp_tmp_0;
      c_sp_tmp_0 = ((int )(read_channel_altera_ch_c_5_p__merlinalterafuncend(ch_c_5_p[_jj_12])));
      c[_j0_11 * 16 + _jj_12] = c_sp_tmp_0;
    }
  }
}

__kernel void vec_add_kernel2(__global int * restrict d,__global int * restrict b,__global int * restrict c,int inc)
{
  int _jj_12;
  int _jj_10;
  
#pragma ACCEL STREAM_PORT ACCESS_TYPE=read DIM_NUM=1 NODE=vec_add_kernel2 VARIABLE=b
  int _jj_8;
  
#pragma ACCEL STREAM_PORT ACCESS_TYPE=read DIM_NUM=1 NODE=vec_add_kernel2 VARIABLE=d
  
#pragma ACCEL interface variable=d stream_prefetch=on max_depth=10000 depth=10000 memory_burst=off
  
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
      int d_sn_tmp_0;
      j = j0 * 16 + jj;
      d_sn_tmp_0 = ((int )(read_channel_altera_ch_d_3_p__merlinalterafuncend(ch_d_3_p[jj])));
      b_sn_tmp_0 = ((int )(read_channel_altera_ch_b_4_p__merlinalterafuncend(ch_b_4_p[jj])));
      c_sn_tmp_0 = d_sn_tmp_0 + b_sn_tmp_0 + inc + 10000;
      write_channel_altera_ch_c_5_p__merlinalterafuncend(ch_c_5_p[jj],c_sn_tmp_0);
    }
  }
  int _jj_8_0;
  int _jj_10_0;
  
#pragma ACCEL STREAM_PORT ACCESS_TYPE=write DIM_NUM=1 NODE=vec_add_kernel2 VARIABLE=c
  int _jj_12_0;
}
