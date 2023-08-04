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
#include "config.h"
#include <string.h>
#pragma ACCEL kernel
void CONV(float *Cout,float *Cin,float *weight,float *bias)
{
  float Cout_buf_0_0_rn_rf3[224][224][4];
  float Cin_buf_0_0_rf2[226][226][4];
  float weight_buf_0_0_rf1[8][8][3][3][4][4];
  float bias_buf_0_rf0[8][4];
  float Cout_buf_0_0_tmp;
  float Cout_buf_0_0_rn[224][224][4];
  int _memcpy_i4_2;
  int _memcpy_i4_1;
  int _memcpy_i4_0;
  int _memcpy_i3_2;
  int _memcpy_i3_1;
  int _memcpy_i3_0;
  int _memcpy_i2_2;
  int _memcpy_i2_1;
  int _memcpy_i2_0;
  int _memcpy_i1_5;
  int _memcpy_i1_4;
  int _memcpy_i1_3;
  int _memcpy_i1_2;
  int _memcpy_i1_1;
  int _memcpy_i1_0;
  int _memcpy_i0_1;
  int _memcpy_i0_0;
  float bias_buf_0[8][4];
  float weight_buf_0_0[8][8][3][3][4][4];
  float Cout_buf_tmp_0_0[4];
  float Cin_tmp_0_0[226][226][4];
  float Cout_buf_0_0[224][224][4];
  float Cin_buf_0_0[226][226][4];
  for (_memcpy_i0_1 = 0; _memcpy_i0_1 < 8; ++_memcpy_i0_1) {
    for (_memcpy_i0_0 = 0; _memcpy_i0_0 < 4; ++_memcpy_i0_0) {
// Parallel pragma comes from SgInitializedName:bias_buf_0
      
#pragma ACCEL parallel
      bias_buf_0[_memcpy_i0_1][_memcpy_i0_0] = bias[0 + ((0 * 8 + _memcpy_i0_1) * 4 + _memcpy_i0_0)];
    }
  }
  for (_memcpy_i1_5 = 0; _memcpy_i1_5 < 8; ++_memcpy_i1_5) {
    for (_memcpy_i1_4 = 0; _memcpy_i1_4 < 8; ++_memcpy_i1_4) {
      for (_memcpy_i1_3 = 0; _memcpy_i1_3 < 3; ++_memcpy_i1_3) {
        for (_memcpy_i1_2 = 0; _memcpy_i1_2 < 3; ++_memcpy_i1_2) {
          for (_memcpy_i1_1 = 0; _memcpy_i1_1 < 4; ++_memcpy_i1_1) {
// Parallel pragma comes from SgInitializedName:weight_buf_0_0
            
#pragma ACCEL parallel
            for (_memcpy_i1_0 = 0; _memcpy_i1_0 < 4; ++_memcpy_i1_0) {
// Parallel pragma comes from SgInitializedName:weight_buf_0_0
              
#pragma ACCEL parallel
              weight_buf_0_0[_memcpy_i1_5][_memcpy_i1_4][_memcpy_i1_3][_memcpy_i1_2][_memcpy_i1_1][_memcpy_i1_0] = weight[0 + ((((((0 * 8 + _memcpy_i1_5) * 8 + _memcpy_i1_4) * 3 + _memcpy_i1_3) * 3 + _memcpy_i1_2) * 4 + _memcpy_i1_1) * 4 + _memcpy_i1_0)];
            }
          }
        }
      }
    }
  }
  for (int i0 = 0; i0 < 32 / 4; i0++) {
    for (_memcpy_i2_2 = 0; _memcpy_i2_2 < 224; ++_memcpy_i2_2) {
      for (_memcpy_i2_1 = 0; _memcpy_i2_1 < 224; ++_memcpy_i2_1) {
        for (_memcpy_i2_0 = 0; _memcpy_i2_0 < 4; ++_memcpy_i2_0) {
          Cout_buf_0_0[_memcpy_i2_2][_memcpy_i2_1][_memcpy_i2_0] = Cout[i0 * 200704 + (((0 * 224 + _memcpy_i2_2) * 224 + _memcpy_i2_1) * 4 + _memcpy_i2_0)];
        }
      }
    }
    for (int j0 = 0; j0 < 32 / 4; j0++) {
      for (_memcpy_i3_2 = 0; _memcpy_i3_2 < 226; ++_memcpy_i3_2) {
        for (_memcpy_i3_1 = 0; _memcpy_i3_1 < 226; ++_memcpy_i3_1) {
          for (_memcpy_i3_0 = 0; _memcpy_i3_0 < 4; ++_memcpy_i3_0) {
// Parallel pragma comes from SgInitializedName:Cin_buf_0_0
            
#pragma ACCEL parallel
            Cin_buf_0_0[_memcpy_i3_2][_memcpy_i3_1][_memcpy_i3_0] = Cin[j0 * 204304 + (((0 * 226 + _memcpy_i3_2) * 226 + _memcpy_i3_1) * 4 + _memcpy_i3_0)];
          }
        }
      }
      for (int p = 0; p < 3; p++) {
        for (int q = 0; q < 3; q++) {
          for (int h = 0; h < 226; h++) {
            for (int w = 0; w < 226; w++) 
// Original: #pragma ACCEL pipeline flatten
{
              if (p == 0 && q == 0) {
                for (int ii = 0; ii < 4; ++ii) {
                  
#pragma ACCEL parallel
                  Cin_buf_0_0_rf2[h][w][ii] = Cin_buf_0_0[h][w][ii];
                }
              }
              if (h == 2 && w == 2) {
                for (int ii = 0; ii < 4; ii++) {
                  
#pragma ACCEL parallel
                  for (int jj = 0; jj < 4; jj++) {
                    
#pragma ACCEL parallel
                    weight_buf_0_0_rf1[i0][j0][p][q][jj][ii] = weight_buf_0_0[i0][j0][p][q][jj][ii];
                  }
                }
              }
              if (j0 == 0 && p == 0 && q == 0 && h == 2 && w == 2) {
                for (int ii = 0; ii < 4; ii++) {
                  
#pragma ACCEL parallel
                  bias_buf_0_rf0[i0][ii] = bias_buf_0[i0][ii];
                }
              }
              
#pragma ACCEL PIPELINE II=1
              if (p == 0 && q == 0) {
                for (int ii = 0; ii < 4; ++ii) {
                  
#pragma ACCEL PARALLEL COMPLETE
                  Cin_tmp_0_0[h][w][ii] = Cin_buf_0_0_rf2[h][w][ii];
                }
              }
              if (h >= 3 - 1 && w >= 3 - 1) {
                for (int ii = 0; ii < 4; ii++) {
                  
#pragma ACCEL PARALLEL COMPLETE
                  if (p == 0 && q == 0 && j0 == 0) {
                    Cout_buf_tmp_0_0[ii] = bias_buf_0_rf0[i0][ii];
                  }
                   else {
                    Cout_buf_tmp_0_0[ii] = Cout_buf_0_0[- 2 + h][- 2 + w][ii];
                  }
// Cout[(i0 * TILE + ii) * IMROW * IMROW + h * IMROW +
// w];
                }
                for (int jj = 0; jj < 4; jj++) {
                  
#pragma ACCEL PARALLEL COMPLETE
                  for (int ii = 0; ii < 4; ii++) {
                    
#pragma ACCEL PARALLEL COMPLETE
                    Cout_buf_tmp_0_0[ii] += weight_buf_0_0_rf1[i0][j0][p][q][jj][ii] * Cin_tmp_0_0[h + p - 3 + 1][w + q - 3 + 1][jj];
// weight[(i0 * TILE + ii) * NUM * KERNEL * KERNEL +
//       (j0 * TILE + jj) * KERNEL * KERNEL + p * KERNEL
//       +
//       q] *
// Cin[(j0 * TILE + jj) * INIMROW * INIMROW +
//    (h + p) * INIMROW + w + q];
                  }
                }
                for (int ii = 0; ii < 4; ii++) {
                  
#pragma ACCEL PARALLEL COMPLETE
// Cout[(i0 * TILE + ii) * IMROW * IMROW + h * IMROW + w] =
                  Cout_buf_0_0_tmp = Cout_buf_tmp_0_0[ii];
                  Cout_buf_0_0[- 2 + h][- 2 + w][ii] = Cout_buf_0_0_tmp;
                  Cout_buf_0_0_rn_rf3[- 2 + h][- 2 + w][ii] = Cout_buf_0_0_tmp;
                }
                if (j0 == 7 && p == 2 && q == 2) {
                  for (int ii = 0; ii < 4; ii++) {
                    
#pragma ACCEL parallel
                    Cout_buf_0_0_rn[- 2 + h][- 2 + w][ii] = Cout_buf_0_0_rn_rf3[- 2 + h][- 2 + w][ii];
                  }
                }
              }
            }
          }
        }
      }
    }
    for (_memcpy_i4_2 = 0; _memcpy_i4_2 < 224; ++_memcpy_i4_2) {
      for (_memcpy_i4_1 = 0; _memcpy_i4_1 < 224; ++_memcpy_i4_1) {
        for (_memcpy_i4_0 = 0; _memcpy_i4_0 < 4; ++_memcpy_i4_0) {
// Parallel pragma comes from SgInitializedName:Cout_buf_0_0_rn
          
#pragma ACCEL parallel
          Cout[i0 * 200704 + (((0 * 224 + _memcpy_i4_2) * 224 + _memcpy_i4_1) * 4 + _memcpy_i4_0)] = Cout_buf_0_0_rn[_memcpy_i4_2][_memcpy_i4_1][_memcpy_i4_0];
        }
      }
    }
  }
}
