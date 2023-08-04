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
#include "merlin_stream.h" 
static void __merlin_dummy_extern_int_merlin_include_G_();
#pragma ACCEL STREAM_CHANNEL CHANNEL_DEPTH=32 NODE=vec_add PORT=msm_port_c_vec_add_0
merlin_stream ch_c_2;
#pragma ACCEL STREAM_CHANNEL CHANNEL_DEPTH=32 NODE=vec_add PORT=msm_port_b_vec_add_0
merlin_stream ch_b_1;
#pragma ACCEL STREAM_CHANNEL CHANNEL_DEPTH=32 NODE=vec_add PORT=msm_port_a_vec_add_0
merlin_stream ch_a_0;
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_kernel_pragma();
// Original: #pragma ACCEL kernel

void msm_port_a_vec_add_0(int a[10000])
{
  for (int _i0_l1 = 0; _i0_l1 < 10; ++_i0_l1) {
    for (int _i1_l2 = 0; _i1_l2 < 10; ++_i1_l2) 
// Original: #pragma ACCEL parallel
// Original: #pragma ACCEL PARALLEL COMPLETE
{
      
#pragma ACCEL PARALLEL COMPLETE
      for (int _i2_l3 = 0; _i2_l3 < 100; ++_i2_l3) {
        int a_sp_tmp_0;
        a_sp_tmp_0 = a[_i0_l1 * 1000 + _i1_l2 * 100 + _i2_l3];
        merlin_stream_write(&ch_a_0,(&a_sp_tmp_0));
      }
    }
  }
}

void msm_port_b_vec_add_0(int b[10000])
{
  for (int _i0_l1 = 0; _i0_l1 < 10; ++_i0_l1) {
    for (int _i1_l2 = 0; _i1_l2 < 10; ++_i1_l2) 
// Original: #pragma ACCEL parallel
// Original: #pragma ACCEL PARALLEL COMPLETE
{
      
#pragma ACCEL PARALLEL COMPLETE
      for (int _i2_l3 = 0; _i2_l3 < 100; ++_i2_l3) {
        int b_sp_tmp_0;
        b_sp_tmp_0 = b[_i0_l1 * 1000 + _i1_l2 * 100 + _i2_l3];
        merlin_stream_write(&ch_b_1,(&b_sp_tmp_0));
      }
    }
  }
}

void msm_port_c_vec_add_0(int c[10000])
{
  for (int _i0_l1 = 0; _i0_l1 < 10; ++_i0_l1) {
    for (int _i1_l2 = 0; _i1_l2 < 10; ++_i1_l2) 
// Original: #pragma ACCEL parallel
// Original: #pragma ACCEL PARALLEL COMPLETE
{
      
#pragma ACCEL PARALLEL COMPLETE
      for (int _i2_l3 = 0; _i2_l3 < 100; ++_i2_l3) {
        int c_sp_tmp_0;
        merlin_stream_read(&ch_c_2,(&c_sp_tmp_0));
        c[_i0_l1 * 1000 + _i1_l2 * 100 + _i2_l3] = c_sp_tmp_0;
      }
    }
  }
}

#pragma ACCEL kernel
void vec_add(int a[10000],int b[10000],int c[10000])
{
  merlin_stream_init(&ch_c_2,10000,4);
  merlin_stream_init(&ch_b_1,10000,4);
  
#pragma ACCEL STREAM_PORT ACCESS_TYPE=read DIM_NUM=1 NODE=vec_add VARIABLE=b
  msm_port_b_vec_add_0(b);
  merlin_stream_init(&ch_a_0,10000,4);
  
#pragma ACCEL STREAM_PORT ACCESS_TYPE=read DIM_NUM=1 NODE=vec_add VARIABLE=a
  msm_port_a_vec_add_0(a);
  
#pragma ACCEL interface variable=c max_depth=10000 depth=10000 memory_burst=off
  
#pragma ACCEL interface variable=b max_depth=10000 depth=10000 memory_burst=off
  
#pragma ACCEL interface variable=a max_depth=10000 depth=10000 memory_burst=off
  for (int _i0_l1 = 0; _i0_l1 < 10; ++_i0_l1) {
    for (int _i1_l2 = 0; _i1_l2 < 10; ++_i1_l2) 
// Original: #pragma ACCEL parallel
// Original: #pragma ACCEL PARALLEL COMPLETE
{
      
#pragma ACCEL PARALLEL COMPLETE
      for (int _i2_l3 = 0; _i2_l3 < 100; ++_i2_l3) {
        int c_sn_tmp_0;
        int b_sn_tmp_0;
        int a_sn_tmp_0;
        int i = _i0_l1 * 1000 + _i1_l2 * 100 + _i2_l3;
        merlin_stream_read(&ch_a_0,(&a_sn_tmp_0));
        merlin_stream_read(&ch_b_1,(&b_sn_tmp_0));
        c_sn_tmp_0 = a_sn_tmp_0 + b_sn_tmp_0;
        merlin_stream_write(&ch_c_2,(&c_sn_tmp_0));
      }
    }
  }
  merlin_stream_reset(&ch_a_0);
  merlin_stream_reset(&ch_b_1);
  
#pragma ACCEL STREAM_PORT ACCESS_TYPE=write DIM_NUM=1 NODE=vec_add VARIABLE=c
  msm_port_c_vec_add_0(c);
  merlin_stream_reset(&ch_c_2);
}
