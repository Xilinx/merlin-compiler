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
// Original: #pragma ACCEL array_partition type=channel variable=ch_c_5 dim=1 factor=16
// Original: #pragma ACCEL array_partition type=channel variable=ch_b_4 dim=1 factor=16
// Original: #pragma ACCEL array_partition type=channel variable=ch_d_3 dim=1 factor=16
#include "merlin_stream.h" 
static void __merlin_dummy_extern_int_merlin_include_G_();
#pragma ACCEL STREAM_CHANNEL CHANNEL_DEPTH=32 NODE=vec_add_kernel2 PORT=msm_port_c_vec_add_kernel2_0
merlin_stream ch_c_5_p[16];
#pragma ACCEL STREAM_CHANNEL CHANNEL_DEPTH=32 NODE=vec_add_kernel2 PORT=msm_port_b_vec_add_kernel2_0
merlin_stream ch_b_4_p[16];
#pragma ACCEL STREAM_CHANNEL CHANNEL_DEPTH=32 NODE=vec_add_kernel2 PORT=msm_port_d_vec_add_kernel2_0
merlin_stream ch_d_3_p[16];
#include "cmost.h"
#include "merlin_type_define.h"
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_kernel_pragma();
// Original: #pragma ACCEL kernel die=SLR1

void msm_port_d_vec_add_kernel2_0(int *d)
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
      merlin_stream_write(&ch_d_3_p[_jj_8],(&d_sp_tmp_0));
    }
  }
}

void msm_port_b_vec_add_kernel2_0(int *b)
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
      merlin_stream_write(&ch_b_4_p[_jj_10],(&b_sp_tmp_0));
    }
  }
}

void msm_port_c_vec_add_kernel2_0(int *c)
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
      merlin_stream_read(&ch_c_5_p[_jj_12],(&c_sp_tmp_0));
      c[_j0_11 * 16 + _jj_12] = c_sp_tmp_0;
    }
  }
}

void vec_add_kernel2(int *d,int *b,int *c,int inc)
{
  int _jj_12;
  
#pragma unroll
  for (_jj_12 = 0; _jj_12 <= 15; ++_jj_12) {
    merlin_stream_init(&ch_c_5_p[_jj_12],(unsigned long )0,(unsigned long )4);
  }
  int _jj_10;
  
#pragma unroll
  for (_jj_10 = 0; _jj_10 <= 15; ++_jj_10) {
    merlin_stream_init(&ch_b_4_p[_jj_10],(unsigned long )0,(unsigned long )4);
  }
  
#pragma ACCEL STREAM_PORT ACCESS_TYPE=read DIM_NUM=1 NODE=vec_add_kernel2 VARIABLE=b
  msm_port_b_vec_add_kernel2_0(b);
  int _jj_8;
  
#pragma unroll
  for (_jj_8 = 0; _jj_8 <= 15; ++_jj_8) {
    merlin_stream_init(&ch_d_3_p[_jj_8],(unsigned long )0,(unsigned long )4);
  }
  
#pragma ACCEL STREAM_PORT ACCESS_TYPE=read DIM_NUM=1 NODE=vec_add_kernel2 VARIABLE=d
  msm_port_d_vec_add_kernel2_0(d);
  __merlin_access_range(c,0,9999UL);
  __merlin_access_range(b,0,9999UL);
  __merlin_access_range(d,0,9999UL);
  
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
      merlin_stream_read(&ch_d_3_p[jj],(&d_sn_tmp_0));
      merlin_stream_read(&ch_b_4_p[jj],(&b_sn_tmp_0));
      c_sn_tmp_0 = d_sn_tmp_0 + b_sn_tmp_0 + inc + 10000;
      merlin_stream_write(&ch_c_5_p[jj],(&c_sn_tmp_0));
    }
  }
  int _jj_8_0;
  
#pragma unroll
  for (_jj_8_0 = 0; _jj_8_0 <= 15; ++_jj_8_0) {
    merlin_stream_reset(&ch_d_3_p[_jj_8_0]);
  }
  int _jj_10_0;
  
#pragma unroll
  for (_jj_10_0 = 0; _jj_10_0 <= 15; ++_jj_10_0) {
    merlin_stream_reset(&ch_b_4_p[_jj_10_0]);
  }
  
#pragma ACCEL STREAM_PORT ACCESS_TYPE=write DIM_NUM=1 NODE=vec_add_kernel2 VARIABLE=c
  msm_port_c_vec_add_kernel2_0(c);
  int _jj_12_0;
  
#pragma unroll
  for (_jj_12_0 = 0; _jj_12_0 <= 15; ++_jj_12_0) {
    merlin_stream_reset(&ch_c_5_p[_jj_12_0]);
  }
}
