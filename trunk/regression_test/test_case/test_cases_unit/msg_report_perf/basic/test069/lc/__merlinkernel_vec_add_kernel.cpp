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
#define __constant
#define __kernel
#define __global
#include "altera_const.h"
#define get_compute_id(x) x
extern int __merlin_include__GB__0;
#pragma OPENCL EXTENSION cl_altera_channels : enable
// Original: #pragma ACCEL array_partition type=channel variable=ch__i_sub_l1_3 dim=1 factor=16
// Original: #pragma ACCEL array_partition type=channel variable=ch_c_buf_0_2 dim=1 factor=16
// Original: #pragma ACCEL array_partition type=channel variable=ch_b_buf_0_1 dim=1 factor=16
// Original: #pragma ACCEL array_partition type=channel variable=ch_a_buf_0_0 dim=1 factor=16
#include "merlin_stream.h" 
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy__pragma_ACCEL_STREAM_CHANNEL_C_();
#ifndef __defined_ch__i_sub_l1_3_p
#define __defined_ch__i_sub_l1_3_p
#pragma channel int ch__i_sub_l1_3_p[16] __attribute__((depth(32)));
typedef int __merlin_channel_typedef;
#include "channel_def.h"
__merlin_channel_typedef ch__i_sub_l1_3_p[16];
#endif
static void __merlin_dummy__pragma_ACCEL_STREAM_CHANNEL_C_();
#ifndef __defined_ch_c_buf_0_2_p
#define __defined_ch_c_buf_0_2_p
#pragma channel int ch_c_buf_0_2_p[16] __attribute__((depth(1000)));
typedef int __merlin_channel_typedef;
#include "channel_def.h"
__merlin_channel_typedef ch_c_buf_0_2_p[16];
#endif
static void __merlin_dummy__pragma_ACCEL_STREAM_CHANNEL_C_();
#ifndef __defined_ch_b_buf_0_1_p
#define __defined_ch_b_buf_0_1_p
#pragma channel int ch_b_buf_0_1_p[16] __attribute__((depth(1000)));
typedef int __merlin_channel_typedef;
#include "channel_def.h"
__merlin_channel_typedef ch_b_buf_0_1_p[16];
#endif
static void __merlin_dummy__pragma_ACCEL_STREAM_CHANNEL_C_();
#ifndef __defined_ch_a_buf_0_0_p
#define __defined_ch_a_buf_0_0_p
#pragma channel int ch_a_buf_0_0_p[16] __attribute__((depth(1000)));
typedef int __merlin_channel_typedef;
#include "channel_def.h"
__merlin_channel_typedef ch_a_buf_0_0_p[16];
#endif
#include <string.h> 

#include "merlin_type_define.h"
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();

void vec_add_core(int _i_sub_l1)
{
  int j;
  for (j = 0; j < 1000; j++) {
    int c_buf_0_sn_tmp_0;
    int b_buf_0_sn_tmp_0;
    int a_buf_0_sn_tmp_0;
    a_buf_0_sn_tmp_0 = ((int )(read_channel_altera_ch_a_buf_0_0_p__merlinalterafuncend(ch_a_buf_0_0_p[_i_sub_l1])));
    b_buf_0_sn_tmp_0 = ((int )(read_channel_altera_ch_b_buf_0_1_p__merlinalterafuncend(ch_b_buf_0_1_p[_i_sub_l1])));
    c_buf_0_sn_tmp_0 = a_buf_0_sn_tmp_0 + b_buf_0_sn_tmp_0;
    write_channel_altera_ch_c_buf_0_2_p__merlinalterafuncend(ch_c_buf_0_2_p[_i_sub_l1],c_buf_0_sn_tmp_0);
  }
}
static void __merlin_dummy_kernel_pragma();
// Original: #pragma ACCEL kernel
void __merlin_dummy_msm_node_0();
 __attribute__ (( autorun))
 __attribute__ (( num_compute_units(16)))
 __attribute__ (( max_global_work_dim(0)))

__kernel void msm_node_0()
{
  int id_0 = get_compute_id(0);
  while(1){
    int _i_sub_l1_sn_tmp_0;
    int __i_sub_l1;
    _i_sub_l1_sn_tmp_0 = ((int )(read_channel_altera_ch__i_sub_l1_3_p__merlinalterafuncend(ch__i_sub_l1_3_p[id_0])));
    __i_sub_l1 = _i_sub_l1_sn_tmp_0;
    vec_add_core(id_0);
  }
}

void msm_port_a_buf_0_msm_node_0_0(int a_buf_0[16][1000],int _i_sub_l1)
{
  int __i_sub_l1;
  __i_sub_l1 = _i_sub_l1;
  int _j_1;
  for (_j_1 = 0; _j_1 < 1000; _j_1++) {
    int _i_sub_l1_0;
{
      
#pragma unroll
      for (_i_sub_l1_0 = ((int )0); _i_sub_l1_0 <= ((int )15); ++_i_sub_l1_0) {
        int a_buf_0_sp_tmp_0;
        a_buf_0_sp_tmp_0 = a_buf_0[((long long )_i_sub_l1_0) - 0LL][_j_1];
        write_channel_altera_ch_a_buf_0_0_p__merlinalterafuncend(ch_a_buf_0_0_p[_i_sub_l1_0],a_buf_0_sp_tmp_0);
      }
    }
  }
}

void msm_port_b_buf_0_msm_node_0_0(int b_buf_0[16][1000],int _i_sub_l1)
{
  int __i_sub_l1;
  __i_sub_l1 = _i_sub_l1;
  int _j_2;
  for (_j_2 = 0; _j_2 < 1000; _j_2++) {
    int _i_sub_l1_0;
{
      
#pragma unroll
      for (_i_sub_l1_0 = ((int )0); _i_sub_l1_0 <= ((int )15); ++_i_sub_l1_0) {
        int b_buf_0_sp_tmp_0;
        b_buf_0_sp_tmp_0 = b_buf_0[((long long )_i_sub_l1_0) - 0LL][_j_2];
        write_channel_altera_ch_b_buf_0_1_p__merlinalterafuncend(ch_b_buf_0_1_p[_i_sub_l1_0],b_buf_0_sp_tmp_0);
      }
    }
  }
}

void msm_port_c_buf_0_msm_node_0_0(int c_buf_0[16][1000],int _i_sub_l1)
{
  int __i_sub_l1;
  __i_sub_l1 = _i_sub_l1;
  int _j_3;
  for (_j_3 = 0; _j_3 < 1000; _j_3++) {
    int _i_sub_l1_0;
{
      
#pragma unroll
      for (_i_sub_l1_0 = ((int )0); _i_sub_l1_0 <= ((int )15); ++_i_sub_l1_0) {
        int c_buf_0_sp_tmp_0;
        c_buf_0_sp_tmp_0 = ((int )(read_channel_altera_ch_c_buf_0_2_p__merlinalterafuncend(ch_c_buf_0_2_p[_i_sub_l1_0])));
        c_buf_0[((long long )_i_sub_l1_0) - 0LL][_j_3] = c_buf_0_sp_tmp_0;
      }
    }
  }
}

void msm_port_i_sub_l1_msm_node_0_0(int _i_sub_l1)
{
  int _i_sub_l1_0;
{
    
#pragma unroll
    for (_i_sub_l1_0 = ((int )0); _i_sub_l1_0 <= ((int )15); ++_i_sub_l1_0) {
      int _i_sub_l1_sp_tmp_0;
      int __i_sub_l1;
      _i_sub_l1_sp_tmp_0 = _i_sub_l1_0;
      write_channel_altera_ch__i_sub_l1_3_p__merlinalterafuncend(ch__i_sub_l1_3_p[_i_sub_l1_0],_i_sub_l1_sp_tmp_0);
      __i_sub_l1 = _i_sub_l1_0;
      __i_sub_l1;
    }
  }
}

__kernel void vec_add_kernel(__global int * restrict a,__global int * restrict b,__global int * restrict c)
{
  int _memcpy_i2_1;
  int _memcpy_i2_0;
  int _memcpy_i1_1;
  int _memcpy_i1_0;
  int _memcpy_i0_1;
  int _memcpy_i0_0;
  
#pragma ACCEL interface variable=a max_depth=2000000 depth=2000000
  
#pragma ACCEL interface variable=b max_depth=2000000 depth=2000000
  
#pragma ACCEL interface variable=c max_depth=2000000 depth=2000000
  int i;
// Original pramga:  ACCEL PARALLEL FACTOR=16 
  
#pragma unroll 1
  for (i = 0; i < 125; i++) 
// Original: #pragma ACCEL parallel factor=16
// Original: #pragma ACCEL PARALLEL FACTOR=16
{
    int c_buf_0[16][1000];
    int b_buf_0[16][1000];
    int a_buf_0[16][1000];
{
      for (_memcpy_i0_1 = 0; _memcpy_i0_1 < 16; ++_memcpy_i0_1) {
        
#pragma HLS COALESCING_WIDTH=1600
        
#pragma unroll 50
        for (_memcpy_i0_0 = 0; _memcpy_i0_0 < 1000; ++_memcpy_i0_0) {
          a_buf_0[_memcpy_i0_1][_memcpy_i0_0] = a[i * 16000 + ((0 * 16 + _memcpy_i0_1) * 1000 + _memcpy_i0_0)];
        }
      }
    }
{
      for (_memcpy_i1_1 = 0; _memcpy_i1_1 < 16; ++_memcpy_i1_1) {
        
#pragma HLS COALESCING_WIDTH=1600
        
#pragma unroll 50
        for (_memcpy_i1_0 = 0; _memcpy_i1_0 < 1000; ++_memcpy_i1_0) {
          b_buf_0[_memcpy_i1_1][_memcpy_i1_0] = b[i * 16000 + ((0 * 16 + _memcpy_i1_1) * 1000 + _memcpy_i1_0)];
        }
      }
    }
    msm_port_a_buf_0_msm_node_0_0(a_buf_0,0);
    msm_port_b_buf_0_msm_node_0_0(b_buf_0,0);
    msm_port_i_sub_l1_msm_node_0_0(0);
    
#pragma ACCEL mem_fence autokernel=msm_node_0 parent=vec_add_kernel depth=16
    msm_port_c_buf_0_msm_node_0_0(c_buf_0,0);
{
      for (_memcpy_i2_1 = 0; _memcpy_i2_1 < 16; ++_memcpy_i2_1) {
        
#pragma HLS COALESCING_WIDTH=1600
        
#pragma unroll 50
        for (_memcpy_i2_0 = 0; _memcpy_i2_0 < 1000; ++_memcpy_i2_0) {
          c[i * 16000 + ((0 * 16 + _memcpy_i2_1) * 1000 + _memcpy_i2_0)] = c_buf_0[_memcpy_i2_1][_memcpy_i2_0];
        }
      }
    }
  }
}
