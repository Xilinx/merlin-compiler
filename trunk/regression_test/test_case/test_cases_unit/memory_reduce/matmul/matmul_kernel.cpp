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
#include "channel_def.h"
#pragma channel float __ch_c_buf_0[128] __attribute__((depth(32)));
typedef int __merlin_channel_typedef;
#include <string.h> 
#define VEC_SIZE 256 
#define TILE  128
#define aa(p, n) (a[(p)*VEC_SIZE + (n)])
#define bb(p, n) (b[(p)*VEC_SIZE + (n)])
#define cc(p, n) (c[(p)*VEC_SIZE + (n)])
__merlin_channel_typedef __ch_c_buf_0[128];
#include "channel_def.h"
#pragma channel int __ch_token_0 __attribute__((depth(32)));
typedef int __merlin_channel_typedef;
__merlin_channel_typedef __ch_token_0;
#include "channel_def.h"
#pragma channel float __ch_b_buf_0_0[128] __attribute__((depth(32)));
typedef int __merlin_channel_typedef;
__merlin_channel_typedef __ch_b_buf_0_0[128];
#include "channel_def.h"
#pragma channel float __ch_a_buf_0_0 __attribute__((depth(32)));
typedef int __merlin_channel_typedef;
__merlin_channel_typedef __ch_a_buf_0_0;
#include "channel_def.h"
#pragma channel float __ch_c_buf_0_rn[128] __attribute__((depth(32)));
typedef int __merlin_channel_typedef;
__merlin_channel_typedef __ch_c_buf_0_rn[128];
#include "channel_def.h"
#pragma channel float __ch_c_buf_0_rf0_dep_1[128] __attribute__((depth(512)));
typedef int __merlin_channel_typedef;
__merlin_channel_typedef __ch_c_buf_0_rf0_dep_1[128];
#pragma ACCEL kernel
void matmul_kernel(float *a,float *b,float *c)
{
  int token_1;
  int token_0;
  float c_buf_0_rn_rf3[256][2][128];
  float a_buf_0_0_rf2[256][256];
  float b_buf_0_0_rf1[256][2][128];
  float c_buf_0_rf0[256][2][128];
  float c_buf_0_rn[256][2][128];
  int _memcpy_i3_2;
  int _memcpy_i3_1;
  int _memcpy_i3_0;
  int _memcpy_i2_1;
  int _memcpy_i2_0;
  int _memcpy_i1_2;
  int _memcpy_i1_1;
  int _memcpy_i1_0;
  int _memcpy_i0_2;
  int _memcpy_i0_1;
  int _memcpy_i0_0;
  float c_buf_0[256][2][128];
  float b_buf_0_0[256][2][128];
  float a_buf_0_0[256][256];
  int i_0;
  int j_0;
  int k_0;
  int j0_0;
  int jj_0;
  for (_memcpy_i0_2 = 0; _memcpy_i0_2 < 256; ++_memcpy_i0_2) {
    for (_memcpy_i0_1 = 0; _memcpy_i0_1 < 2; ++_memcpy_i0_1) {
      for (_memcpy_i0_0 = 0; _memcpy_i0_0 < 128; ++_memcpy_i0_0) {
// Parallel pragma comes from SgInitializedName:c_buf_0
        
#pragma ACCEL parallel
        write_channel_altera___ch_c_buf_0__merlinalterafuncend(__ch_c_buf_0[_memcpy_i0_0],c[0 + (((0 * 256 + _memcpy_i0_2) * 2 + _memcpy_i0_1) * 128 + _memcpy_i0_0)]);
      }
      if (_memcpy_i0_1 == 1 && _memcpy_i0_2 == 255) {
        write_channel_altera___ch_token_0__merlinalterafuncend(__ch_token_0,1);
      }
    }
  }
  for (_memcpy_i1_2 = 0; _memcpy_i1_2 < 256; ++_memcpy_i1_2) {
    for (_memcpy_i1_1 = 0; _memcpy_i1_1 < 2; ++_memcpy_i1_1) {
      for (_memcpy_i1_0 = 0; _memcpy_i1_0 < 128; ++_memcpy_i1_0) {
// Parallel pragma comes from SgInitializedName:b_buf_0_0
        
#pragma ACCEL parallel
        write_channel_altera___ch_b_buf_0_0__merlinalterafuncend(__ch_b_buf_0_0[_memcpy_i1_0],b[0 + (((0 * 256 + _memcpy_i1_2) * 2 + _memcpy_i1_1) * 128 + _memcpy_i1_0)]);
      }
    }
  }
  for (_memcpy_i2_1 = 0; _memcpy_i2_1 < 256; ++_memcpy_i2_1) {
    for (_memcpy_i2_0 = 0; _memcpy_i2_0 < 256; ++_memcpy_i2_0) {
      write_channel_altera___ch_a_buf_0_0__merlinalterafuncend(__ch_a_buf_0_0,a[0 + ((0 * 256 + _memcpy_i2_1) * 256 + _memcpy_i2_0)]);
    }
  }
  int __merlin_iteration_0;
  float __ch_read_c_buf_0_rf0_1[128];
  int __ch_is_c_buf_0_rf0_init_1;
  float __ch_c_buf_0_rf0_init_1[128];
  for (k_0 = 0; k_0 < 256; k_0++) {
    float merlin_dummy_r_1[128];
    float merlin_dummy_w_1[128];
    for (i_0 = 0; i_0 < 256; i_0++) {
      for (j0_0 = 0; j0_0 < 256 / 128; j0_0++) 
// Original: #pragma ACCEL pipeline flatten
{
        __ch_is_c_buf_0_rf0_init_1 = 0;
        if (k_0 > 0) {
          for (__merlin_iteration_0 = 0; __merlin_iteration_0 < 127 + 1; ++__merlin_iteration_0) {
#pragma ACCEL PARALLEL COMPLETE
            __ch_read_c_buf_0_rf0_1[__merlin_iteration_0] = read_channel_altera___ch_c_buf_0_rf0_dep_1__merlinalterafuncend(__ch_c_buf_0_rf0_dep_1[__merlin_iteration_0]);
          }
        }
        if (j0_0 == 0) {
          a_buf_0_0_rf2[k_0][i_0] = read_channel_altera___ch_a_buf_0_0__merlinalterafuncend(__ch_a_buf_0_0);
        }
        if (i_0 == 0) {
          for (jj_0 = 0; jj_0 < 128; ++jj_0) {
            
#pragma ACCEL parallel
            b_buf_0_0_rf1[k_0][j0_0][jj_0] = read_channel_altera___ch_b_buf_0_0__merlinalterafuncend(__ch_b_buf_0_0[jj_0]);
          }
        }
        if (k_0 == 0) {
          for (jj_0 = 0; jj_0 < 128; ++jj_0) {
            
#pragma ACCEL parallel
            __ch_c_buf_0_rf0_init_1[jj_0] = read_channel_altera___ch_c_buf_0__merlinalterafuncend(__ch_c_buf_0[jj_0]);
            __ch_is_c_buf_0_rf0_init_1 = 1;
          }
        }
        
#pragma ACCEL PIPELINE II=1
        for (jj_0 = 0; jj_0 < 128; ++jj_0) {
          
#pragma ACCEL PARALLEL COMPLETE
          j_0 = j0_0 * 128 + jj_0;
          c_buf_0_rf0[i_0][j0_0][jj_0] = ((__ch_is_c_buf_0_rf0_init_1?__ch_c_buf_0_rf0_init_1[jj_0] : __ch_read_c_buf_0_rf0_1[jj_0])) + a_buf_0_0_rf2[k_0][i_0] * b_buf_0_0_rf1[k_0][j0_0][jj_0];
          c_buf_0_rn_rf3[i_0][j0_0][jj_0] = c_buf_0_rf0[i_0][j0_0][jj_0];
        }
        if (k_0 == 255) {
          for (jj_0 = 0; jj_0 < 128; ++jj_0) {
            
#pragma ACCEL parallel
            write_channel_altera___ch_c_buf_0_rn__merlinalterafuncend(__ch_c_buf_0_rn[jj_0],c_buf_0_rn_rf3[i_0][j0_0][jj_0]);
          }
        }
        if (k_0 < 255) {
          for (__merlin_iteration_0 = 0; __merlin_iteration_0 < 127 + 1; ++__merlin_iteration_0) {
#pragma ACCEL PARALLEL COMPLETE
            write_channel_altera___ch_c_buf_0_rf0_dep_1__merlinalterafuncend(__ch_c_buf_0_rf0_dep_1[__merlin_iteration_0],c_buf_0_rf0[i_0][j0_0][__merlin_iteration_0]);
          }
        }
      }
    }
  }
  for (_memcpy_i3_2 = 0; _memcpy_i3_2 < 256; ++_memcpy_i3_2) {
    for (_memcpy_i3_1 = 0; _memcpy_i3_1 < 2; ++_memcpy_i3_1) {
      if (_memcpy_i3_1 == 0 && _memcpy_i3_2 == 0) {
        read_channel_altera___ch_token_0__merlinalterafuncend(__ch_token_0);
      }
      for (_memcpy_i3_0 = 0; _memcpy_i3_0 < 128; ++_memcpy_i3_0) {
// Parallel pragma comes from SgInitializedName:c_buf_0_rn
        
#pragma ACCEL parallel
        c[0 + (((0 * 256 + _memcpy_i3_2) * 2 + _memcpy_i3_1) * 128 + _memcpy_i3_0)] = read_channel_altera___ch_c_buf_0_rn__merlinalterafuncend(__ch_c_buf_0_rn[_memcpy_i3_0]);
      }
      if (_memcpy_i3_1 == 1 && _memcpy_i3_2 == 255) {
        token_1 = 1;
      }
    }
  }
}
