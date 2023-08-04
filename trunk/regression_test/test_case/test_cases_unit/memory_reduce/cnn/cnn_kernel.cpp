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
#pragma channel float __ch_weight_buf_0[8][8] __attribute__((depth(32)));
typedef int __merlin_channel_typedef;
#include <string.h> 
#include "config.h"
#include <string.h>
__merlin_channel_typedef __ch_weight_buf_0[8][8];
#include "channel_def.h"
#pragma channel float __ch_bias_buf_0_0[8] __attribute__((depth(32)));
typedef int __merlin_channel_typedef;
__merlin_channel_typedef __ch_bias_buf_0_0[8];
#include "channel_def.h"
#pragma channel float __ch_Cout_buf_0_0[8] __attribute__((depth(32)));
typedef int __merlin_channel_typedef;
__merlin_channel_typedef __ch_Cout_buf_0_0[8];
#include "channel_def.h"
#pragma channel float __ch_Cout_buf_0_0_rn[8] __attribute__((depth(32)));
typedef int __merlin_channel_typedef;
__merlin_channel_typedef __ch_Cout_buf_0_0_rn[8];
#include "channel_def.h"
#pragma channel float __ch_Cout_buf_0_0_rf2_dep_1[8] __attribute__((depth(51076)));
typedef int __merlin_channel_typedef;
__merlin_channel_typedef __ch_Cout_buf_0_0_rf2_dep_1[8];
#pragma ACCEl kernel
void CONV(float *Cout,float *Cin,float *weight,float *bias)
{
  float Cout_buf_0_0_rn_rf3[224][224][8];
  float Cout_buf_0_0_rf2[224][224][8];
  float bias_buf_0_0_rf1[4][8];
  float weight_buf_0_rf0[4][4][3][3][8][8];
  float Cout_buf_0_0_rn[224][224][8];
  int _memcpy_i2_2;
  int _memcpy_i2_1;
  int _memcpy_i2_0;
  int _memcpy_i1_1;
  int _memcpy_i1_0;
  int _memcpy_i0_5;
  int _memcpy_i0_4;
  int _memcpy_i0_3;
  int _memcpy_i0_2;
  int _memcpy_i0_1;
  int _memcpy_i0_0;
  float weight_buf_0[4][4][3][3][8][8];
  float bias_buf_0_0[4][8];
  float Cin_tmp_0_0[226][226][8];
  float Cout_buf_0_0[224][224][8];
  float tmp_0_0[8];
  int _i0_0_0;
  for (_memcpy_i0_5 = 0; _memcpy_i0_5 < 4; ++_memcpy_i0_5) {
    for (_memcpy_i0_4 = 0; _memcpy_i0_4 < 4; ++_memcpy_i0_4) {
      for (_memcpy_i0_3 = 0; _memcpy_i0_3 < 3; ++_memcpy_i0_3) {
        for (_memcpy_i0_2 = 0; _memcpy_i0_2 < 3; ++_memcpy_i0_2) {
          for (_memcpy_i0_1 = 0; _memcpy_i0_1 < 8; ++_memcpy_i0_1) {
// Parallel pragma comes from SgInitializedName:weight_buf_0
            
#pragma ACCEL parallel
            for (_memcpy_i0_0 = 0; _memcpy_i0_0 < 8; ++_memcpy_i0_0) {
// Parallel pragma comes from SgInitializedName:weight_buf_0
              
#pragma ACCEL parallel
              write_channel_altera___ch_weight_buf_0__merlinalterafuncend(__ch_weight_buf_0[_memcpy_i0_1][_memcpy_i0_0],weight[0 + ((((((0 * 4 + _memcpy_i0_5) * 4 + _memcpy_i0_4) * 3 + _memcpy_i0_3) * 3 + _memcpy_i0_2) * 8 + _memcpy_i0_1) * 8 + _memcpy_i0_0)]);
            }
          }
        }
      }
    }
  }
  for (_memcpy_i1_1 = 0; _memcpy_i1_1 < 4; ++_memcpy_i1_1) {
    for (_memcpy_i1_0 = 0; _memcpy_i1_0 < 8; ++_memcpy_i1_0) {
// Parallel pragma comes from SgInitializedName:bias_buf_0_0
      
#pragma ACCEL parallel
      write_channel_altera___ch_bias_buf_0_0__merlinalterafuncend(__ch_bias_buf_0_0[_memcpy_i1_0],bias[0 + ((0 * 4 + _memcpy_i1_1) * 8 + _memcpy_i1_0)]);
    }
  }
  int __merlin_iteration_0;
  float __ch_read_Cout_buf_0_0_rf2_1[8];
  int __ch_is_Cout_buf_0_0_rf2_init_1;
  float __ch_Cout_buf_0_0_rf2_init_1[8];
  for (int i0 = 0; i0 < 32 / 8; i0++) {
    float merlin_dummy_r_1[8];
    float merlin_dummy_w_1[8];
    for (int h = 0; h < 224; ++h) {
      for (int w = 0; w < 224; ++w) {
        for (int ii = 0; ii < 8; ++ii) {
// Parallel pragma comes from SgInitializedName:Cout_buf_0_0
          
#pragma ACCEL parallel
          if (h == 0 && w == 0) {
            bias_buf_0_0_rf1[i0][ii] = read_channel_altera___ch_bias_buf_0_0__merlinalterafuncend(__ch_bias_buf_0_0[ii]);
          }
          write_channel_altera___ch_Cout_buf_0_0__merlinalterafuncend(__ch_Cout_buf_0_0[ii],bias_buf_0_0_rf1[i0][ii]);
        }
      }
    }
    for (int j0 = 0; j0 < 32 / 8; j0++) {
      for (int p = 0; p < 3; p++) {
        for (int q = 0; q < 3; q++) {
          for (int h = 0; h < 226; h++) {
            for (int w = 0; w < 226; w++) 
// Original: #pragma ACCEL pipeline flatten
{
              __ch_is_Cout_buf_0_0_rf2_init_1 = 0;
              if (i0 > 0 || j0 > 0 || p > 0 || q > 0) {
                for (__merlin_iteration_0 = 0; __merlin_iteration_0 < 7 + 1; ++__merlin_iteration_0) {
#pragma ACCEL PARALLEL COMPLETE
                  __ch_read_Cout_buf_0_0_rf2_1[__merlin_iteration_0] = read_channel_altera___ch_Cout_buf_0_0_rf2_dep_1__merlinalterafuncend(__ch_Cout_buf_0_0_rf2_dep_1[__merlin_iteration_0]);
                }
              }
              
#pragma ACCEL PIPELINE II=1
              if (q == 0 && p == 0) {
                for (int ii = 0; ii < 8; ++ii) {
                  
#pragma ACCEL PARALLEL COMPLETE
                  Cin_tmp_0_0[h][w][ii] = Cin[j0 * 8 * 226 * 226 + h * 226 * 8 + w * 8 + ii];
                }
              }
              if (h >= 3 - 1 && w >= 3 - 1) {
                if (j0 == 0 && p == 0 && q == 0) {
                  for (int ii = 0; ii < 8; ++ii) {
                    
#pragma ACCEL parallel
                    __ch_Cout_buf_0_0_rf2_init_1[ii] = read_channel_altera___ch_Cout_buf_0_0__merlinalterafuncend(__ch_Cout_buf_0_0[ii]);
                    __ch_is_Cout_buf_0_0_rf2_init_1 = 1;
                  }
                }
                if (h == 2 && w == 2) {
                  for (int ii = 0; ii < 8; ii++) {
                    
#pragma ACCEL parallel
                    for (int jj = 0; jj < 8; jj++) {
                      
#pragma ACCEL parallel
                      weight_buf_0_rf0[i0][j0][p][q][jj][ii] = read_channel_altera___ch_weight_buf_0__merlinalterafuncend(__ch_weight_buf_0[jj][ii]);
                    }
                  }
                }
{
                  for (_i0_0_0 = 0; _i0_0_0 <= 7; ++_i0_0_0) {
                    
#pragma ACCEL PARALLEL COMPLETE
                    tmp_0_0[_i0_0_0] = 0;
                  }
                }
                for (int jj = 0; jj < 8; jj++) {
                  
#pragma ACCEL PARALLEL COMPLETE
                  for (int ii = 0; ii < 8; ii++) {
                    
#pragma ACCEL PARALLEL COMPLETE
                    tmp_0_0[ii] += weight_buf_0_rf0[i0][j0][p][q][jj][ii] * Cin_tmp_0_0[h + p - 3 + 1][w + q - 3 + 1][jj];
// weight[(i0 * TILE + ii) * NUM * KERNEL * KERNEL +
//       (j0 * TILE + jj) * KERNEL * KERNEL + p * KERNEL
//       +
//       q] *
// Cin[(j0 * TILE + jj) * INIMROW * INIMROW +
//    (h + p) * INIMROW + w + q];
                  }
                }
                for (int ii = 0; ii < 8; ++ii) {
                  
#pragma ACCEL PARALLEL COMPLETE
                  Cout_buf_0_0_rf2[h - 3 + 1][w - 3 + 1][ii] = ((__ch_is_Cout_buf_0_0_rf2_init_1?__ch_Cout_buf_0_0_rf2_init_1[ii] : __ch_read_Cout_buf_0_0_rf2_1[ii])) + tmp_0_0[ii];
                  Cout_buf_0_0_rn_rf3[h - 3 + 1][w - 3 + 1][ii] = Cout_buf_0_0_rf2[h - 3 + 1][w - 3 + 1][ii];
                }
                if (j0 == 3 && p == 2 && q == 2) {
                  for (int ii = 0; ii < 8; ++ii) {
                    
#pragma ACCEL parallel
                    write_channel_altera___ch_Cout_buf_0_0_rn__merlinalterafuncend(__ch_Cout_buf_0_0_rn[ii],Cout_buf_0_0_rn_rf3[h - 3 + 1][w - 3 + 1][ii]);
                  }
                }
              }
              if (i0 < 3 || j0 < 3 || p < 2 || q < 2) {
                for (__merlin_iteration_0 = 0; __merlin_iteration_0 < 7 + 1; ++__merlin_iteration_0) {
#pragma ACCEL PARALLEL COMPLETE
                  write_channel_altera___ch_Cout_buf_0_0_rf2_dep_1__merlinalterafuncend(__ch_Cout_buf_0_0_rf2_dep_1[__merlin_iteration_0],Cout_buf_0_0_rf2[h - 3 + 1][w - 3 + 1][__merlin_iteration_0]);
                }
              }
            }
          }
        }
      }
    }
    for (_memcpy_i2_2 = 0; _memcpy_i2_2 < 224; ++_memcpy_i2_2) {
      for (_memcpy_i2_1 = 0; _memcpy_i2_1 < 224; ++_memcpy_i2_1) {
        for (_memcpy_i2_0 = 0; _memcpy_i2_0 < 8; ++_memcpy_i2_0) {
// Parallel pragma comes from SgInitializedName:Cout_buf_0_0_rn
          
#pragma ACCEL parallel
          Cout[i0 * 8 * 224 * 224 + (((0 * 224 + _memcpy_i2_2) * 224 + _memcpy_i2_1) * 8 + _memcpy_i2_0)] = read_channel_altera___ch_Cout_buf_0_0_rn__merlinalterafuncend(__ch_Cout_buf_0_0_rn[_memcpy_i2_0]);
        }
      }
    }
  }
}
