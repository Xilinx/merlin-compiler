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
#include <string.h> 
#include "cmost.h"
#include "merlin_type_define.h"
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_kernel_pragma();
#include "channel_def.h"
#pragma channel float _ch_Cin_buf_0[16] __attribute__((depth(32)));
typedef int __merlin_channel_typedef;
/* Original: #pragma ACCEL kernel */
__merlin_channel_typedef _ch_Cin_buf_0[16];

#pragma accel kernel
void cnn_kernel(float *Cout,float *Cin,float *weight)
{
  float Cin_buf_0_rf1[3][8][16];
  float weight_buf_0_rf0[3][3][8][16][8][16];
  int _memcpy_i3_5;
  int _memcpy_i3_4;
  int _memcpy_i3_3;
  int _memcpy_i3_2;
  int _memcpy_i3_1;
  int _memcpy_i3_0;
  int _memcpy_i2_2;
  int _memcpy_i2_1;
  int _memcpy_i2_0;
  int _memcpy_i1_3;
  int _memcpy_i1_2;
  int _memcpy_i1_1;
  int _memcpy_i1_0;
  int _memcpy_i0_3;
  int _memcpy_i0_2;
  int _memcpy_i0_1;
  int _memcpy_i0_0;
  float weight_buf_0[3][3][8][16][8][16];
  float Cout_buf_0[224][224][8][16];
  for (_memcpy_i0_3 = 0; _memcpy_i0_3 < 224; ++_memcpy_i0_3) {
    
#pragma HLS COALESCING_WIDTH=512
    for (_memcpy_i0_2 = 0; _memcpy_i0_2 < 224; ++_memcpy_i0_2) {
      for (_memcpy_i0_1 = 0; _memcpy_i0_1 < 8; ++_memcpy_i0_1) {
        for (_memcpy_i0_0 = 0; _memcpy_i0_0 < 16; ++_memcpy_i0_0) {
          
#pragma ACCEL PARALLEL SKIP COMPLETE
          Cout_buf_0[_memcpy_i0_3][_memcpy_i0_2][_memcpy_i0_1][_memcpy_i0_0] = Cout[0 + ((((0 * 224 + _memcpy_i0_3) * 224 + _memcpy_i0_2) * 8 + _memcpy_i0_1) * 16 + _memcpy_i0_0)];
        }
      }
    }
  }
  for (_memcpy_i3_5 = 0; _memcpy_i3_5 < 3; ++_memcpy_i3_5) {
    
#pragma HLS COALESCING_WIDTH=512
    for (_memcpy_i3_4 = 0; _memcpy_i3_4 < 3; ++_memcpy_i3_4) {
      for (_memcpy_i3_1 = 0; _memcpy_i3_1 < 8; ++_memcpy_i3_1) {
        for (_memcpy_i3_3 = 0; _memcpy_i3_3 < 8; ++_memcpy_i3_3) {
          for (_memcpy_i3_0 = 0; _memcpy_i3_0 < 16; ++_memcpy_i3_0) {
            for (_memcpy_i3_2 = 0; _memcpy_i3_2 < 16; ++_memcpy_i3_2) {
              
#pragma ACCEL PARALLEL SKIP COMPLETE
              weight_buf_0[_memcpy_i3_5][_memcpy_i3_4][_memcpy_i3_3][_memcpy_i3_2][_memcpy_i3_1][_memcpy_i3_0] = weight[0 + ((((((0 * 3 + _memcpy_i3_5) * 3 + _memcpy_i3_4) * 8 + _memcpy_i3_3) * 16 + _memcpy_i3_2) * 8 + _memcpy_i3_1) * 16 + _memcpy_i3_0)];
            }
          }
        }
      }
    }
  }
  
#pragma ACCEL skip_sync variable=Cout_buf_0
  
#pragma ACCEL skip_sync variable=Cout
  
#pragma ACCEL skip_sync variable=Cout_buf_0
  
#pragma ACCEL skip_sync variable=Cout
  
#pragma ACCEL skip_sync variable=Cout_buf_0
  
#pragma ACCEL skip_sync variable=Cout
  
#pragma ACCEL skip_sync variable=Cout_buf_0
  
#pragma ACCEL skip_sync variable=Cout
  
#pragma ACCEL skip_sync variable=Cout_buf_0
  
#pragma ACCEL skip_sync variable=Cout
  
#pragma ACCEL skip_sync variable=Cout_buf_0
  
#pragma ACCEL skip_sync variable=Cout
  __merlin_access_range(weight,0,147455);
  __merlin_access_range(Cin,0,6537727);
  __merlin_access_range(Cout,0,6422527);
  
#pragma ACCEL interface variable=weight max_depth=3,3,128,128 depth=3,3,128,128
  
#pragma ACCEL interface variable=Cin max_depth=226,226,128 depth=226,226,128
  
#pragma ACCEL interface variable=Cout max_depth=224,224,128 depth=224,224,128
  for (int r = 0; r < 224; r++) {
    for (int c = 0; c < 224; c++) {
      for (int p = 0; p < 3; p++) {
        float Cin_buf_0[3][8][16];
        for (_memcpy_i2_2 = 0; _memcpy_i2_2 < 3; ++_memcpy_i2_2) {
          
#pragma HLS COALESCING_WIDTH=512
          for (_memcpy_i2_1 = 0; _memcpy_i2_1 < 8; ++_memcpy_i2_1) {
            for (_memcpy_i2_0 = 0; _memcpy_i2_0 < 16; ++_memcpy_i2_0) {
              
#pragma ACCEL PARALLEL SKIP COMPLETE
              write_channel_altera__ch_Cin_buf_0__merlinalterafuncend(_ch_Cin_buf_0[_memcpy_i2_0],Cin[c * 128 + p * 28928 + r * 28928 + (((0 * 3 + _memcpy_i2_2) * 8 + _memcpy_i2_1) * 16 + _memcpy_i2_0)]);
            }
          }
        }
        for (int q = 0; q < 3; q++) {
/* Original pramga:  ACCEL PARALLEL FACTOR=16  */
          for (int i = 0; i < 8; i++) 
/* Original: #pragma ACCEL parallel factor=16 */
/* Original: #pragma ACCEL PARALLEL FACTOR=16 */
{
            
#pragma ACCEL PARALLEL FACTOR=1 SKIP
/* Original pramga:  ACCEL PARALLEL FACTOR=16  */
            for (int o = 0; o < 8; o++) 
/* Original: #pragma ACCEL parallel factor=16 */
/* Original: #pragma ACCEL PARALLEL FACTOR=16 */
/* Original: #pragma ACCEL reduction */
{
              if (r == 0 && c == 0) {
                for (int o_sub = 0; o_sub < 16; ++o_sub) {
                  
#pragma ACCEL parallel
                  for (int i_sub = 0; i_sub < 16; ++i_sub) {
                    
#pragma ACCEL parallel
                    weight_buf_0_rf0[p][q][o][o_sub][i][i_sub] = weight_buf_0[p][q][o][o_sub][i][i_sub];
                  }
                }
              }
              if (o == 0) {
                for (int i_sub = 0; i_sub < 16; ++i_sub) {
                  
#pragma ACCEL parallel
                  Cin_buf_0_rf1[q][i][i_sub] = ((float )(read_channel_altera__ch_Cin_buf_0__merlinalterafuncend(_ch_Cin_buf_0[i_sub])));
                }
              }
              
#pragma ACCEL PARALLEL FACTOR=1 SKIP
              
#pragma ACCEL REDUCTION
              for (int i_sub = 0; i_sub < 16; ++i_sub) {
                
#pragma ACCEL PARALLEL
                for (int o_sub = 0; o_sub < 16; ++o_sub) 
/* Original: #pragma ACCEL parallel factor=16 */
/* Original: #pragma ACCEL PARALLEL FACTOR=16 */
/* Original: #pragma ACCEL reduction */
{
                  
#pragma ACCEL PARALLEL
                  
#pragma ACCEL REDUCTION
                  int one_sub;
                  one_sub = ((int )(weight_buf_0_rf0[p][q][o][o_sub][i][i_sub] * Cin_buf_0_rf1[q][i][i_sub]));
                  Cout_buf_0[r][c][o][o_sub] += ((float )one_sub);
                }
              }
            }
          }
        }
      }
    }
  }
  for (_memcpy_i1_3 = 0; _memcpy_i1_3 < 224; ++_memcpy_i1_3) {
    
#pragma HLS COALESCING_WIDTH=512
    for (_memcpy_i1_2 = 0; _memcpy_i1_2 < 224; ++_memcpy_i1_2) {
      for (_memcpy_i1_1 = 0; _memcpy_i1_1 < 8; ++_memcpy_i1_1) {
        for (_memcpy_i1_0 = 0; _memcpy_i1_0 < 16; ++_memcpy_i1_0) {
          
#pragma ACCEL PARALLEL SKIP COMPLETE
          Cout[0 + ((((0 * 224 + _memcpy_i1_3) * 224 + _memcpy_i1_2) * 8 + _memcpy_i1_1) * 16 + _memcpy_i1_0)] = Cout_buf_0[_memcpy_i1_3][_memcpy_i1_2][_memcpy_i1_1][_memcpy_i1_0];
        }
      }
    }
  }
}
