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
// Original: #pragma ACCEL array_partition type=channel variable=ch_s_0 dim=1 factor=4
// Original: #pragma ACCEL array_partition type=channel variable=ch_c_2 dim=1 factor=4
// Original: #pragma ACCEL array_partition type=channel variable=ch_b_1 dim=1 factor=4
// Original: #pragma ACCEL array_partition type=channel variable=ch_a_0 dim=1 factor=4
#include "merlin_stream.h" 
static void __merlin_dummy_extern_int_merlin_include_G_();
#pragma ACCEL STREAM_CHANNEL CHANNEL_DEPTH=32 NODE=msm_node_0 PORT=msm_port_s_msm_node_0_0
merlin_stream ch_s_0_p[4];
#include <string.h> 
#include "merlin_stream.h" 
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
#pragma ACCEL STREAM_CHANNEL CHANNEL_DEPTH=32 NODE=vec_add PORT=msm_port_c_vec_add_0
merlin_stream ch_c_2_p[4];
#pragma ACCEL STREAM_CHANNEL CHANNEL_DEPTH=32 NODE=vec_add PORT=msm_port_b_vec_add_0
merlin_stream ch_b_1_p[4];
#pragma ACCEL STREAM_CHANNEL CHANNEL_DEPTH=32 NODE=vec_add PORT=msm_port_a_vec_add_0
merlin_stream ch_a_0_p[4];
#include "cmost.h"
#include <math.h>
#include "merlin_type_define.h"
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_kernel_pragma();
// Original: #pragma ACCEL kernel

void msm_port_a_vec_add_0(int a[1024])
{
  int _memcpy_i1_1;
  int _memcpy_i1_0;
  int a_buf_0[256][4];
  
#pragma ivdep array(a)
  
#pragma ivdep array(a_buf_0)
  
#pragma unroll 4
  for (_memcpy_i1_1 = 0; _memcpy_i1_1 < 256; ++_memcpy_i1_1) {
    
#pragma HLS COALESCING_WIDTH=512
    
#pragma ivdep array(a)
    
#pragma ivdep array(a_buf_0)
    
#pragma unroll
    for (_memcpy_i1_0 = 0; _memcpy_i1_0 < 4; ++_memcpy_i1_0) {
      a_buf_0[_memcpy_i1_1][_memcpy_i1_0] = a[0 + ((0 * 256 + _memcpy_i1_1) * 4 + _memcpy_i1_0)];
    }
  }
  int _i_sub;
// Original pramga:  ACCEL PARALLEL FACTOR=4 
  
#pragma ivdep array(a_buf_0)
  
#pragma unroll 1
  for (int i = 0; i < 256; i++) 
// Original: #pragma ACCEL parallel factor=4
// Original: #pragma ACCEL PARALLEL FACTOR=4
{
    
#pragma ivdep array(a_buf_0)
    
#pragma unroll
    for (_i_sub = 0; _i_sub < 4; ++_i_sub) 
// Original: #pragma ACCEL parallel factor=4
// Original: #pragma ACCEL PARALLEL FACTOR=4
{
      int a_sp_tmp_0;
      a_sp_tmp_0 = a_buf_0[i][_i_sub];
      merlin_stream_write(&ch_a_0_p[_i_sub],(&a_sp_tmp_0));
    }
  }
}

void msm_port_b_vec_add_0(int b[1024])
{
  int _memcpy_i2_1;
  int _memcpy_i2_0;
  int b_buf_0[256][4];
  
#pragma ivdep array(b)
  
#pragma ivdep array(b_buf_0)
  
#pragma unroll 4
  for (_memcpy_i2_1 = 0; _memcpy_i2_1 < 256; ++_memcpy_i2_1) {
    
#pragma HLS COALESCING_WIDTH=512
    
#pragma ivdep array(b)
    
#pragma ivdep array(b_buf_0)
    
#pragma unroll
    for (_memcpy_i2_0 = 0; _memcpy_i2_0 < 4; ++_memcpy_i2_0) {
      b_buf_0[_memcpy_i2_1][_memcpy_i2_0] = b[0 + ((0 * 256 + _memcpy_i2_1) * 4 + _memcpy_i2_0)];
    }
  }
  int _i_sub;
// Original pramga:  ACCEL PARALLEL FACTOR=4 
  
#pragma ivdep array(b_buf_0)
  
#pragma unroll 1
  for (int i = 0; i < 256; i++) 
// Original: #pragma ACCEL parallel factor=4
// Original: #pragma ACCEL PARALLEL FACTOR=4
{
    
#pragma ivdep array(b_buf_0)
    
#pragma unroll
    for (_i_sub = 0; _i_sub < 4; ++_i_sub) 
// Original: #pragma ACCEL parallel factor=4
// Original: #pragma ACCEL PARALLEL FACTOR=4
{
      int b_sp_tmp_0;
      b_sp_tmp_0 = b_buf_0[i][_i_sub];
      merlin_stream_write(&ch_b_1_p[_i_sub],(&b_sp_tmp_0));
    }
  }
}

void msm_port_c_vec_add_0(int c[1024])
{
  int _memcpy_i0_1;
  int _memcpy_i0_0;
  int c_buf_0[256][4];
  int _i_sub;
// Original pramga:  ACCEL PARALLEL FACTOR=4 
  
#pragma ivdep array(c_buf_0)
  
#pragma unroll 1
  for (int i = 0; i < 256; i++) 
// Original: #pragma ACCEL parallel factor=4
// Original: #pragma ACCEL PARALLEL FACTOR=4
{
    
#pragma ivdep array(c_buf_0)
    
#pragma unroll
    for (_i_sub = 0; _i_sub < 4; ++_i_sub) 
// Original: #pragma ACCEL parallel factor=4
// Original: #pragma ACCEL PARALLEL FACTOR=4
{
      int c_sp_tmp_0;
      merlin_stream_read(&ch_c_2_p[_i_sub],(&c_sp_tmp_0));
      c_buf_0[i][_i_sub] = c_sp_tmp_0;
    }
  }
  
#pragma ivdep array(c)
  
#pragma ivdep array(c_buf_0)
  
#pragma unroll 4
  for (_memcpy_i0_1 = 0; _memcpy_i0_1 < 256; ++_memcpy_i0_1) {
    
#pragma HLS COALESCING_WIDTH=512
    
#pragma ivdep array(c)
    
#pragma ivdep array(c_buf_0)
    
#pragma unroll
    for (_memcpy_i0_0 = 0; _memcpy_i0_0 < 4; ++_memcpy_i0_0) {
      c[0 + ((0 * 256 + _memcpy_i0_1) * 4 + _memcpy_i0_0)] = c_buf_0[_memcpy_i0_1][_memcpy_i0_0];
    }
  }
}

void msm_node_0(int s,int i_sub)
// Original: #pragma ACCEL parallel factor=4
// Original: #pragma ACCEL PARALLEL FACTOR=4
{
  int s_sn_tmp_0;
  int _s;
  merlin_stream_read(&ch_s_0_p[i_sub],(&s_sn_tmp_0));
  _s = s_sn_tmp_0;
  int c_sn_tmp_0;
  int b_sn_tmp_0;
  int a_sn_tmp_0;
  merlin_stream_read(&ch_a_0_p[i_sub],(void *)(&a_sn_tmp_0));
  int a0 = a_sn_tmp_0;
  merlin_stream_read(&ch_b_1_p[i_sub],(void *)(&b_sn_tmp_0));
  int b0 = b_sn_tmp_0;
  int c0 = 0;
  for (int j = 0; j < 1024 * 10; j++) {
    c0 += ((int )(pow((double )(a0 * b0 + j),1.5))) % 7 + _s;
  }
  c_sn_tmp_0 = c0;
  merlin_stream_write(&ch_c_2_p[i_sub],(void *)(&c_sn_tmp_0));
}

void msm_port_s_msm_node_0_0(int s,int i_sub)
{
  int s_sp_tmp_0;
  int _s;
  s_sp_tmp_0 = s;
  merlin_stream_write(&ch_s_0_p[i_sub],(&s_sp_tmp_0));
  _s = s;
  _s;
}

void vec_add(int s,int a[1024],int b[1024],int c[1024])
{
  int _i_sub;
  
#pragma unroll
  for (_i_sub = 0; _i_sub <= 3; ++_i_sub) {
    merlin_stream_init(&ch_c_2_p[_i_sub],(unsigned long )1024,(unsigned long )4);
  }
  int _i_sub_1;
  
#pragma unroll
  for (_i_sub_1 = 0; _i_sub_1 <= 3; ++_i_sub_1) {
    merlin_stream_init(&ch_b_1_p[_i_sub_1],(unsigned long )1024,(unsigned long )4);
  }
  
#pragma ACCEL STREAM_PORT ACCESS_TYPE=read DIM_NUM=1 NODE=vec_add VARIABLE=b
  msm_port_b_vec_add_0(b);
  int _i_sub_3;
  
#pragma unroll
  for (_i_sub_3 = 0; _i_sub_3 <= 3; ++_i_sub_3) {
    merlin_stream_init(&ch_a_0_p[_i_sub_3],(unsigned long )1024,(unsigned long )4);
  }
  
#pragma ACCEL STREAM_PORT ACCESS_TYPE=read DIM_NUM=1 NODE=vec_add VARIABLE=a
  msm_port_a_vec_add_0(a);
  int i_sub;
  __merlin_access_range(c,0,1023UL);
  __merlin_access_range(b,0,1023UL);
  __merlin_access_range(a,0,1023UL);
  
#pragma ACCEL interface variable=a stream_prefetch=on max_depth=1024 depth=1024
  
#pragma ACCEL interface variable=b stream_prefetch=on max_depth=1024 depth=1024
  
#pragma ACCEL interface variable=c stream_prefetch=on max_depth=1024 depth=1024
// Original pramga:  ACCEL PARALLEL FACTOR=4 
  
#pragma unroll 1
  for (int i = 0; i < 256; i++) 
// Original: #pragma ACCEL parallel factor=4
// Original: #pragma ACCEL PARALLEL FACTOR=4
{
    
#pragma unroll
    for (i_sub = 0; i_sub < 4; ++i_sub) {
      merlin_stream_init(&ch_s_0_p[i_sub],1,4);
// Original pragma: ACCEL STREAM_PORT ACCESS_TYPE=read DIM_NUM=0 NODE=msm_node_0 VARIABLE=s
      
#pragma ACCEL STREAM_PORT ACCESS_TYPE=read DIM_NUM=0 NODE=msm_node_0 PARALLEL_ITERATOR=1 VARIABLE=s
      msm_port_s_msm_node_0_0(s,i_sub);
// Original: #pragma ACCEL parallel factor=4
// Original: #pragma ACCEL PARALLEL FACTOR=4
// Original pragma: ACCEL STREAM_NODE 
// Original pragma: ACCEL STREAM_NODE PARALLEL_FACTOR=4
      
#pragma ACCEL STREAM_NODE PARALLEL_FACTOR=4 PARALLEL_ITERATOR=1
      msm_node_0(s,i_sub);
      merlin_stream_reset(&ch_s_0_p[i_sub]);
    }
  }
  int _i_sub_4;
  
#pragma unroll
  for (_i_sub_4 = 0; _i_sub_4 <= 3; ++_i_sub_4) {
    merlin_stream_reset(&ch_a_0_p[_i_sub_4]);
  }
  int _i_sub_2;
  
#pragma unroll
  for (_i_sub_2 = 0; _i_sub_2 <= 3; ++_i_sub_2) {
    merlin_stream_reset(&ch_b_1_p[_i_sub_2]);
  }
  
#pragma ACCEL STREAM_PORT ACCESS_TYPE=write DIM_NUM=1 NODE=vec_add VARIABLE=c
  msm_port_c_vec_add_0(c);
  int _i_sub_0;
  
#pragma unroll
  for (_i_sub_0 = 0; _i_sub_0 <= 3; ++_i_sub_0) {
    merlin_stream_reset(&ch_c_2_p[_i_sub_0]);
  }
}
