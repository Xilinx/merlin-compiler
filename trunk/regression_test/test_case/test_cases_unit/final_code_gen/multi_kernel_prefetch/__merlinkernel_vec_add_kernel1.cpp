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
// Original: #pragma ACCEL array_partition type=channel variable=ch_c_2 dim=1 factor=16
// Original: #pragma ACCEL array_partition type=channel variable=ch_b_1 dim=1 factor=16
// Original: #pragma ACCEL array_partition type=channel variable=ch_a_0 dim=1 factor=16
#include "merlin_stream.h" 
static void __merlin_dummy_extern_int_merlin_include_G_();
#pragma ACCEL STREAM_CHANNEL CHANNEL_DEPTH=32 NODE=vec_add_kernel1 PORT=msm_port_c_vec_add_kernel1_0
merlin_stream ch_c_2_p[16];
#pragma ACCEL STREAM_CHANNEL CHANNEL_DEPTH=32 NODE=vec_add_kernel1 PORT=msm_port_b_vec_add_kernel1_0
merlin_stream ch_b_1_p[16];
#pragma ACCEL STREAM_CHANNEL CHANNEL_DEPTH=32 NODE=vec_add_kernel1 PORT=msm_port_a_vec_add_kernel1_0
merlin_stream ch_a_0_p[16];
#include "cmost.h"
#include "merlin_type_define.h"
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_kernel_pragma();
// Original: #pragma ACCEL kernel die=SLR0

void msm_port_a_vec_add_kernel1_0(int *a)
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
      merlin_stream_write(&ch_a_0_p[_jj_2],(&a_sp_tmp_0));
    }
  }
}

void msm_port_b_vec_add_kernel1_0(int *b)
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
      merlin_stream_write(&ch_b_1_p[_jj_4],(&b_sp_tmp_0));
    }
  }
}

void msm_port_c_vec_add_kernel1_0(int *c)
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
      merlin_stream_read(&ch_c_2_p[_jj_6],(&c_sp_tmp_0));
      c[_j0_5 * 16 + _jj_6] = c_sp_tmp_0;
    }
  }
}

void vec_add_kernel1(int *a,int *b,int *c,int inc)
{
  int _jj_6;
  
#pragma unroll
  for (_jj_6 = 0; _jj_6 <= 15; ++_jj_6) {
    merlin_stream_init(&ch_c_2_p[_jj_6],(unsigned long )0,(unsigned long )4);
  }
  int _jj_4;
  
#pragma unroll
  for (_jj_4 = 0; _jj_4 <= 15; ++_jj_4) {
    merlin_stream_init(&ch_b_1_p[_jj_4],(unsigned long )0,(unsigned long )4);
  }
  
#pragma ACCEL STREAM_PORT ACCESS_TYPE=read DIM_NUM=1 NODE=vec_add_kernel1 VARIABLE=b
  msm_port_b_vec_add_kernel1_0(b);
  int _jj_2;
  
#pragma unroll
  for (_jj_2 = 0; _jj_2 <= 15; ++_jj_2) {
    merlin_stream_init(&ch_a_0_p[_jj_2],(unsigned long )0,(unsigned long )4);
  }
  
#pragma ACCEL STREAM_PORT ACCESS_TYPE=read DIM_NUM=1 NODE=vec_add_kernel1 VARIABLE=a
  msm_port_a_vec_add_kernel1_0(a);
  __merlin_access_range(c,0,9999UL);
  __merlin_access_range(b,0,9999UL);
  __merlin_access_range(a,0,9999UL);
  
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
      merlin_stream_read(&ch_a_0_p[jj],(&a_sn_tmp_0));
      merlin_stream_read(&ch_b_1_p[jj],(&b_sn_tmp_0));
      c_sn_tmp_0 = a_sn_tmp_0 + b_sn_tmp_0 + inc;
      merlin_stream_write(&ch_c_2_p[jj],(&c_sn_tmp_0));
    }
  }
  int _jj_2_0;
  
#pragma unroll
  for (_jj_2_0 = 0; _jj_2_0 <= 15; ++_jj_2_0) {
    merlin_stream_reset(&ch_a_0_p[_jj_2_0]);
  }
  int _jj_4_0;
  
#pragma unroll
  for (_jj_4_0 = 0; _jj_4_0 <= 15; ++_jj_4_0) {
    merlin_stream_reset(&ch_b_1_p[_jj_4_0]);
  }
  
#pragma ACCEL STREAM_PORT ACCESS_TYPE=write DIM_NUM=1 NODE=vec_add_kernel1 VARIABLE=c
  msm_port_c_vec_add_kernel1_0(c);
  int _jj_6_0;
  
#pragma unroll
  for (_jj_6_0 = 0; _jj_6_0 <= 15; ++_jj_6_0) {
    merlin_stream_reset(&ch_c_2_p[_jj_6_0]);
  }
}
