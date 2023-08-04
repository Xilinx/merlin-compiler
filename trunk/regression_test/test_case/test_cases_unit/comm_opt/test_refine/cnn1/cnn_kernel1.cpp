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
#include "config.h"
#include <string.h>

void cnn_kernel_sub_core(int w0,int h0,int i0,float *Cout,float *Cin,float *weight,float *bias,int j0,float Cout_buf[28][28][16])
{
  float Cin_tmp[30][30][16];
  float Cin_buf[30][30][16];
  float Cout_tmp[28][28][16];
  float Weight_buf[16][16];
  float Weight_reg[16][16];
//float bias_buf[TILE];
//        #pragma ACCEL parallel 
//        for (int ii = 0; ii < TILE; ++ii)
//            bias_buf[ii] = bias[i0 * TILE  + ii];
//       for (int j0 = 0; j0 < NUM / TILE; j0++) 
  int h_0;
  int w_0;
  float new_value_0;
  int hh_0;
  int h_1;
  int w_1;
  float bias_t_0[16];
  float tmp_0[16];
{
    for (int hh = 0; hh < 28 + 2; hh++) {
      for (int ww = 0; ww < 28 + 2; ww++) 
// Original: #pragma ACCEL pipeline flatten
{
        
#pragma ACCEL PIPELINE II=1
        h_0 = h0 * 28 + hh;
        w_0 = w0 * 28 + ww;
        for (int ii = 0; ii < 16; ++ii) {
          
#pragma ACCEL PARALLEL COMPLETE
          Cin_buf[hh][ww][ii] = Cin[j0 * 16 * 240 * 240 + h0 * 30 * 30 * 8 * 16 + w0 * 30 * 30 * 16 + hh * 30 * 16 + ww * 16 + ii];
        }
      }
    }
    for (int p = 0; p < 3; p++) {
      for (int q = 0; q < 3; q++) {
        for (int jj = 0; jj < 16; jj++) {
          for (int ii = 0; ii < 16; ii++) 
// Original: #pragma ACCEL parallel
{
            
#pragma ACCEL PARALLEL COMPLETE
            Weight_buf[jj][ii] = weight[i0 * 16 * 32 * 3 * 3 + j0 * 16 * 3 * 3 * 16 + p * 3 * 16 * 16 + q * 16 * 16 + jj * 16 + ii];
          }
        }
        for (int hh_t = 0; hh_t < 1 + 28 + 2; hh_t++) {
          for (int ww = 0; ww < 28 + 2; ww++) 
// Original: #pragma ACCEL pipeline flatten
{
            
#pragma ACCEL PIPELINE II=1
            if (hh_t == 0) {
              if (ww < 16) {
                for (int ii = 0; ii < 16; ii++) {
                  
#pragma ACCEL PARALLEL COMPLETE
                  new_value_0 = Weight_buf[ww][ii];
// shift register
// jj+1 -> jj 
// new -> 15
                  for (int jj = 0; jj < 15; jj++) {
                    
#pragma ACCEL PARALLEL COMPLETE
                    Weight_reg[jj][ii] = Weight_reg[jj + 1][ii];
                  }
                  Weight_reg[15][ii] = new_value_0;
                }
              }
            }
//else {
            if (hh_t > 0) {
              hh_0 = hh_t - 1;
              h_1 = h0 * 28 + hh_0;
              w_1 = w0 * 28 + ww;
              if (p == 0 && q == 0) {
                for (int ii = 0; ii < 16; ++ii) {
                  
#pragma ACCEL PARALLEL COMPLETE
                  Cin_tmp[hh_0][ww][ii] = Cin_buf[hh_0][ww][ii];
                }
              }
              for (int ii = 0; ii < 16; ++ii) {
                
#pragma ACCEL PARALLEL COMPLETE
                bias_t_0[ii] = bias[ii];
              }
              if (ww >= 2 && hh_0 >= 2) {
                if (j0 == 0 && p == 0 && q == 0) {
                  for (int ii = 0; ii < 16; ++ii) {
                    
#pragma ACCEL PARALLEL COMPLETE
//bias_t[ii]; //bias[i0 * TILE  + ii];
                    Cout_tmp[hh_0 - 2][ww - 2][ii] = 0;
                  }
                }
                 else {
                  if (j0 == 1 && p == 0 && q == 0) {
                    for (int ii = 0; ii < 16; ++ii) {
                      
#pragma ACCEL PARALLEL COMPLETE
                      Cout_tmp[hh_0 - 2][ww - 2][ii] = 0;
                    }
                  }
                }
              }
              if (ww >= 2 && hh_0 >= 2) {
                for (int ii = 0; ii < 16; ii++) {
                  
#pragma ACCEL PARALLEL COMPLETE
                  tmp_0[ii] = 0.0;
                }
                for (int jj = 0; jj < 16; jj++) {
                  
#pragma ACCEL PARALLEL COMPLETE
                  for (int ii = 0; ii < 16; ii++) {
                    
#pragma ACCEL PARALLEL COMPLETE
                    tmp_0[ii] += Weight_reg[jj][ii] * Cin_tmp[hh_0 + p - 2][ww + q - 2][jj];
//weight[i0 * TILE * NUM * KERNEL * KERNEL +
//j0 * TILE * KERNEL * KERNEL * TILE +
//p * KERNEL * TILE * TILE + q * TILE * TILE +
//jj * TILE + ii] 
                  }
                }
                for (int ii = 0; ii < 16; ++ii) {
                  
#pragma ACCEL PARALLEL COMPLETE
                  Cout_tmp[hh_0 - 2][ww - 2][ii] += tmp_0[ii];
                }
                for (int ii = 0; ii < 16; ++ii) {
                  
#pragma ACCEL PARALLEL COMPLETE
                  Cout_buf[hh_0 - 2][ww - 2][ii] = Cout_tmp[hh_0 - 2][ww - 2][ii];
                }
              }
            }
// ww
          }
// hh
        }
// q
      }
// p
    }
// j0
  }
}

void cnn_kernel_sub(float *Cout,float *Cin,float *weight,float *bias,int i0)
{
//    float Cin_tmp1[INIMROW][30][TILE];
//    float Cout_buf1[IMROW][28][TILE];
  float Cout_buf0[28][28][16];
  float Cout_buf1[28][28][16];
  for (int w0 = 0; w0 < 8; w0++) {
    for (int h0 = 0; h0 < 8; h0++) {
      cnn_kernel_sub_core(w0,h0,i0,Cout,Cin,weight,bias,0,Cout_buf0);
      cnn_kernel_sub_core(w0,h0,i0,Cout,Cin,weight,bias,1,Cout_buf1);
      for (int hh = 0; hh < 28; hh++) {
        for (int ww = 0; ww < 28; ww++) 
// Original: #pragma ACCEL pipeline flatten
{
          
#pragma ACCEL PIPELINE II=1
          for (int ii = 0; ii < 16; ++ii) {
            
#pragma ACCEL PARALLEL COMPLETE
            Cout[i0 * 16 * 224 * 224 + h0 * 28 * 28 * 8 * 16 + w0 * 28 * 28 * 16 + hh * 28 * 16 + ww * 16 + ii] = Cout_buf0[hh][ww][ii] + Cout_buf1[hh][ww][ii];
          }
          ;
        }
      }
// h0
    }
  }
}
#pragma ACCEL kernel
void cnn_kernel(float *Cout,float *Cin,float *weight,float *bias,float *Cout1,float *Cin1,float *weight1,float *bias1)
{
  int i0__cnn_kernel_sub_1;
  float Cout_buf0__cnn_kernel_sub_1[28][28][16];
  float Cout_buf1__cnn_kernel_sub_1[28][28][16];
  int j0__cnn_kernel_sub_core_3;
  int i0__cnn_kernel_sub_core_3;
  int h0__cnn_kernel_sub_core_3;
  int w0__cnn_kernel_sub_core_3;
  float Cin_tmp__cnn_kernel_sub_core_3[30][30][16];
  float Cin_buf__cnn_kernel_sub_core_3[30][30][16];
  float Cout_tmp__cnn_kernel_sub_core_3[28][28][16];
  float Weight_buf__cnn_kernel_sub_core_3[16][16];
  float Weight_reg__cnn_kernel_sub_core_3[16][16];
  int h_0__cnn_kernel_sub_core_3;
  int w_0__cnn_kernel_sub_core_3;
  float new_value_0__cnn_kernel_sub_core_3;
  int hh_0__cnn_kernel_sub_core_3;
  int h_1__cnn_kernel_sub_core_3;
  int w_1__cnn_kernel_sub_core_3;
  float bias_t_0__cnn_kernel_sub_core_3[16];
  float tmp_0__cnn_kernel_sub_core_3[16];
  int j0__cnn_kernel_sub_core_0_0;
  int i0__cnn_kernel_sub_core_0_0;
  int h0__cnn_kernel_sub_core_0_0;
  int w0__cnn_kernel_sub_core_0_0;
  float Cin_tmp__cnn_kernel_sub_core_0_0[30][30][16];
  float Cin_buf__cnn_kernel_sub_core_0_0[30][30][16];
  float Cout_tmp__cnn_kernel_sub_core_0_0[28][28][16];
  float Weight_buf__cnn_kernel_sub_core_0_0[16][16];
  float Weight_reg__cnn_kernel_sub_core_0_0[16][16];
  int h_0__cnn_kernel_sub_core_0_0;
  int w_0__cnn_kernel_sub_core_0_0;
  float new_value_0__cnn_kernel_sub_core_0_0;
  int hh_0__cnn_kernel_sub_core_0_0;
  int h_1__cnn_kernel_sub_core_0_0;
  int w_1__cnn_kernel_sub_core_0_0;
  float bias_t_0__cnn_kernel_sub_core_0_0[16];
  float tmp_0__cnn_kernel_sub_core_0_0[16];
  int i0__cnn_kernel_sub_0_0;
  float Cout_buf0__cnn_kernel_sub_0_0[28][28][16];
  float Cout_buf1__cnn_kernel_sub_0_0[28][28][16];
  int j0__cnn_kernel_sub_core_1_0;
  int i0__cnn_kernel_sub_core_1_0;
  int h0__cnn_kernel_sub_core_1_0;
  int w0__cnn_kernel_sub_core_1_0;
  float Cin_tmp__cnn_kernel_sub_core_1_0[30][30][16];
  float Cin_buf__cnn_kernel_sub_core_1_0[30][30][16];
  float Cout_tmp__cnn_kernel_sub_core_1_0[28][28][16];
  float Weight_buf__cnn_kernel_sub_core_1_0[16][16];
  float Weight_reg__cnn_kernel_sub_core_1_0[16][16];
  int h_0__cnn_kernel_sub_core_1_0;
  int w_0__cnn_kernel_sub_core_1_0;
  float new_value_0__cnn_kernel_sub_core_1_0;
  int hh_0__cnn_kernel_sub_core_1_0;
  int h_1__cnn_kernel_sub_core_1_0;
  int w_1__cnn_kernel_sub_core_1_0;
  float bias_t_0__cnn_kernel_sub_core_1_0[16];
  float tmp_0__cnn_kernel_sub_core_1_0[16];
  int j0__cnn_kernel_sub_core_2_0;
  int i0__cnn_kernel_sub_core_2_0;
  int h0__cnn_kernel_sub_core_2_0;
  int w0__cnn_kernel_sub_core_2_0;
  float Cin_tmp__cnn_kernel_sub_core_2_0[30][30][16];
  float Cin_buf__cnn_kernel_sub_core_2_0[30][30][16];
  float Cout_tmp__cnn_kernel_sub_core_2_0[28][28][16];
  float Weight_buf__cnn_kernel_sub_core_2_0[16][16];
  float Weight_reg__cnn_kernel_sub_core_2_0[16][16];
  int h_0__cnn_kernel_sub_core_2_0;
  int w_0__cnn_kernel_sub_core_2_0;
  float new_value_0__cnn_kernel_sub_core_2_0;
  int hh_0__cnn_kernel_sub_core_2_0;
  int h_1__cnn_kernel_sub_core_2_0;
  int w_1__cnn_kernel_sub_core_2_0;
  float bias_t_0__cnn_kernel_sub_core_2_0[16];
  float tmp_0__cnn_kernel_sub_core_2_0[16];
{
    for (int w0__cnn_kernel_sub = 0; w0__cnn_kernel_sub < 8; w0__cnn_kernel_sub++) {
      for (int h0__cnn_kernel_sub = 0; h0__cnn_kernel_sub < 8; h0__cnn_kernel_sub++) {{{
            for (int hh__cnn_kernel_sub_core = 0; hh__cnn_kernel_sub_core < 28 + 2; hh__cnn_kernel_sub_core++) {
              for (int ww__cnn_kernel_sub_core = 0; ww__cnn_kernel_sub_core < 28 + 2; ww__cnn_kernel_sub_core++) 
// Original: #pragma ACCEL pipeline flatten
{
                
#pragma ACCEL PIPELINE II=1
                h_0__cnn_kernel_sub_core_3 = h0__cnn_kernel_sub * 28 + hh__cnn_kernel_sub_core;
                w_0__cnn_kernel_sub_core_3 = w0__cnn_kernel_sub * 28 + ww__cnn_kernel_sub_core;
                for (int ii__cnn_kernel_sub_core = 0; ii__cnn_kernel_sub_core < 16; ++ii__cnn_kernel_sub_core) {
                  
#pragma ACCEL PARALLEL COMPLETE
                  Cin_buf__cnn_kernel_sub_core_3[hh__cnn_kernel_sub_core][ww__cnn_kernel_sub_core][ii__cnn_kernel_sub_core] = Cin[0 * 16 * 240 * 240 + h0__cnn_kernel_sub * 30 * 30 * 8 * 16 + w0__cnn_kernel_sub * 30 * 30 * 16 + hh__cnn_kernel_sub_core * 30 * 16 + ww__cnn_kernel_sub_core * 16 + ii__cnn_kernel_sub_core];
                }
              }
            }
            for (int p__cnn_kernel_sub_core = 0; p__cnn_kernel_sub_core < 3; p__cnn_kernel_sub_core++) {
              for (int q__cnn_kernel_sub_core = 0; q__cnn_kernel_sub_core < 3; q__cnn_kernel_sub_core++) {
                for (int jj__cnn_kernel_sub_core = 0; jj__cnn_kernel_sub_core < 16; jj__cnn_kernel_sub_core++) {
                  for (int ii__cnn_kernel_sub_core = 0; ii__cnn_kernel_sub_core < 16; ii__cnn_kernel_sub_core++) 
// Original: #pragma ACCEL parallel
{
                    
#pragma ACCEL PARALLEL COMPLETE
                    Weight_buf__cnn_kernel_sub_core_3[jj__cnn_kernel_sub_core][ii__cnn_kernel_sub_core] = weight[0 * 16 * 32 * 3 * 3 + 0 * 16 * 3 * 3 * 16 + p__cnn_kernel_sub_core * 3 * 16 * 16 + q__cnn_kernel_sub_core * 16 * 16 + jj__cnn_kernel_sub_core * 16 + ii__cnn_kernel_sub_core];
                  }
                }
                for (int hh_t__cnn_kernel_sub_core = 0; hh_t__cnn_kernel_sub_core < 1 + 28 + 2; hh_t__cnn_kernel_sub_core++) {
                  for (int ww__cnn_kernel_sub_core = 0; ww__cnn_kernel_sub_core < 28 + 2; ww__cnn_kernel_sub_core++) 
// Original: #pragma ACCEL pipeline flatten
{
                    
#pragma ACCEL PIPELINE II=1
                    if (hh_t__cnn_kernel_sub_core == 0) {
                      if (ww__cnn_kernel_sub_core < 16) {
                        for (int ii__cnn_kernel_sub_core = 0; ii__cnn_kernel_sub_core < 16; ii__cnn_kernel_sub_core++) {
                          
#pragma ACCEL PARALLEL COMPLETE
                          new_value_0__cnn_kernel_sub_core_3 = Weight_buf__cnn_kernel_sub_core_3[ww__cnn_kernel_sub_core][ii__cnn_kernel_sub_core];
// shift register
// jj+1 -> jj 
// new -> 15
                          for (int jj__cnn_kernel_sub_core = 0; jj__cnn_kernel_sub_core < 15; jj__cnn_kernel_sub_core++) {
                            
#pragma ACCEL PARALLEL COMPLETE
                            Weight_reg__cnn_kernel_sub_core_3[jj__cnn_kernel_sub_core][ii__cnn_kernel_sub_core] = Weight_reg__cnn_kernel_sub_core_3[jj__cnn_kernel_sub_core + 1][ii__cnn_kernel_sub_core];
                          }
                          Weight_reg__cnn_kernel_sub_core_3[15][ii__cnn_kernel_sub_core] = new_value_0__cnn_kernel_sub_core_3;
                        }
                      }
                    }
//else {
                    if (hh_t__cnn_kernel_sub_core > 0) {
                      hh_0__cnn_kernel_sub_core_3 = hh_t__cnn_kernel_sub_core - 1;
                      h_1__cnn_kernel_sub_core_3 = h0__cnn_kernel_sub * 28 + hh_0__cnn_kernel_sub_core_3;
                      w_1__cnn_kernel_sub_core_3 = w0__cnn_kernel_sub * 28 + ww__cnn_kernel_sub_core;
                      if (p__cnn_kernel_sub_core == 0 && q__cnn_kernel_sub_core == 0) {
                        for (int ii__cnn_kernel_sub_core = 0; ii__cnn_kernel_sub_core < 16; ++ii__cnn_kernel_sub_core) {
                          
#pragma ACCEL PARALLEL COMPLETE
                          Cin_tmp__cnn_kernel_sub_core_3[hh_0__cnn_kernel_sub_core_3][ww__cnn_kernel_sub_core][ii__cnn_kernel_sub_core] = Cin_buf__cnn_kernel_sub_core_3[hh_0__cnn_kernel_sub_core_3][ww__cnn_kernel_sub_core][ii__cnn_kernel_sub_core];
                        }
                      }
                      for (int ii__cnn_kernel_sub_core = 0; ii__cnn_kernel_sub_core < 16; ++ii__cnn_kernel_sub_core) {
                        
#pragma ACCEL PARALLEL COMPLETE
                        bias_t_0__cnn_kernel_sub_core_3[ii__cnn_kernel_sub_core] = bias[ii__cnn_kernel_sub_core];
                      }
                      if (ww__cnn_kernel_sub_core >= 2 && hh_0__cnn_kernel_sub_core_3 >= 2) {
                        if (p__cnn_kernel_sub_core == 0 && q__cnn_kernel_sub_core == 0) {
                          for (int ii__cnn_kernel_sub_core = 0; ii__cnn_kernel_sub_core < 16; ++ii__cnn_kernel_sub_core) {
                            
#pragma ACCEL PARALLEL COMPLETE
//bias_t[ii]; //bias[i0 * TILE  + ii];
                            Cout_tmp__cnn_kernel_sub_core_3[hh_0__cnn_kernel_sub_core_3 - 2][ww__cnn_kernel_sub_core - 2][ii__cnn_kernel_sub_core] = ((float )0);
                          }
                        }
                         else {
                        }
                      }
                      if (ww__cnn_kernel_sub_core >= 2 && hh_0__cnn_kernel_sub_core_3 >= 2) {
                        for (int ii__cnn_kernel_sub_core = 0; ii__cnn_kernel_sub_core < 16; ii__cnn_kernel_sub_core++) {
                          
#pragma ACCEL PARALLEL COMPLETE
                          tmp_0__cnn_kernel_sub_core_3[ii__cnn_kernel_sub_core] = ((float )0.0);
                        }
                        for (int jj__cnn_kernel_sub_core = 0; jj__cnn_kernel_sub_core < 16; jj__cnn_kernel_sub_core++) {
                          
#pragma ACCEL PARALLEL COMPLETE
                          for (int ii__cnn_kernel_sub_core = 0; ii__cnn_kernel_sub_core < 16; ii__cnn_kernel_sub_core++) {
                            
#pragma ACCEL PARALLEL COMPLETE
                            tmp_0__cnn_kernel_sub_core_3[ii__cnn_kernel_sub_core] += Weight_reg__cnn_kernel_sub_core_3[jj__cnn_kernel_sub_core][ii__cnn_kernel_sub_core] * Cin_tmp__cnn_kernel_sub_core_3[hh_0__cnn_kernel_sub_core_3 + p__cnn_kernel_sub_core - 2][ww__cnn_kernel_sub_core + q__cnn_kernel_sub_core - 2][jj__cnn_kernel_sub_core];
//weight[i0 * TILE * NUM * KERNEL * KERNEL +
//j0 * TILE * KERNEL * KERNEL * TILE +
//p * KERNEL * TILE * TILE + q * TILE * TILE +
//jj * TILE + ii] 
                          }
                        }
                        for (int ii__cnn_kernel_sub_core = 0; ii__cnn_kernel_sub_core < 16; ++ii__cnn_kernel_sub_core) {
                          
#pragma ACCEL PARALLEL COMPLETE
                          Cout_tmp__cnn_kernel_sub_core_3[hh_0__cnn_kernel_sub_core_3 - 2][ww__cnn_kernel_sub_core - 2][ii__cnn_kernel_sub_core] += tmp_0__cnn_kernel_sub_core_3[ii__cnn_kernel_sub_core];
                        }
                        for (int ii__cnn_kernel_sub_core = 0; ii__cnn_kernel_sub_core < 16; ++ii__cnn_kernel_sub_core) {
                          
#pragma ACCEL PARALLEL COMPLETE
                          Cout_buf0__cnn_kernel_sub_1[hh_0__cnn_kernel_sub_core_3 - 2][ww__cnn_kernel_sub_core - 2][ii__cnn_kernel_sub_core] = Cout_tmp__cnn_kernel_sub_core_3[hh_0__cnn_kernel_sub_core_3 - 2][ww__cnn_kernel_sub_core - 2][ii__cnn_kernel_sub_core];
                        }
                      }
                    }
// ww
                  }
// hh
                }
// q
              }
// p
            }
// j0
          }
        }
{
{
            for (int hh__cnn_kernel_sub_core_0 = 0; hh__cnn_kernel_sub_core_0 < 28 + 2; hh__cnn_kernel_sub_core_0++) {
              for (int ww__cnn_kernel_sub_core_0 = 0; ww__cnn_kernel_sub_core_0 < 28 + 2; ww__cnn_kernel_sub_core_0++) 
// Original: #pragma ACCEL pipeline flatten
{
                
#pragma ACCEL PIPELINE II=1
                h_0__cnn_kernel_sub_core_0_0 = h0__cnn_kernel_sub * 28 + hh__cnn_kernel_sub_core_0;
                w_0__cnn_kernel_sub_core_0_0 = w0__cnn_kernel_sub * 28 + ww__cnn_kernel_sub_core_0;
                for (int ii__cnn_kernel_sub_core_0 = 0; ii__cnn_kernel_sub_core_0 < 16; ++ii__cnn_kernel_sub_core_0) {
                  
#pragma ACCEL PARALLEL COMPLETE
                  Cin_buf__cnn_kernel_sub_core_0_0[hh__cnn_kernel_sub_core_0][ww__cnn_kernel_sub_core_0][ii__cnn_kernel_sub_core_0] = Cin[1 * 16 * 240 * 240 + h0__cnn_kernel_sub * 30 * 30 * 8 * 16 + w0__cnn_kernel_sub * 30 * 30 * 16 + hh__cnn_kernel_sub_core_0 * 30 * 16 + ww__cnn_kernel_sub_core_0 * 16 + ii__cnn_kernel_sub_core_0];
                }
              }
            }
            for (int p__cnn_kernel_sub_core_0 = 0; p__cnn_kernel_sub_core_0 < 3; p__cnn_kernel_sub_core_0++) {
              for (int q__cnn_kernel_sub_core_0 = 0; q__cnn_kernel_sub_core_0 < 3; q__cnn_kernel_sub_core_0++) {
                for (int jj__cnn_kernel_sub_core_0 = 0; jj__cnn_kernel_sub_core_0 < 16; jj__cnn_kernel_sub_core_0++) {
                  for (int ii__cnn_kernel_sub_core_0 = 0; ii__cnn_kernel_sub_core_0 < 16; ii__cnn_kernel_sub_core_0++) 
// Original: #pragma ACCEL parallel
{
                    
#pragma ACCEL PARALLEL COMPLETE
                    Weight_buf__cnn_kernel_sub_core_0_0[jj__cnn_kernel_sub_core_0][ii__cnn_kernel_sub_core_0] = weight[0 * 16 * 32 * 3 * 3 + 1 * 16 * 3 * 3 * 16 + p__cnn_kernel_sub_core_0 * 3 * 16 * 16 + q__cnn_kernel_sub_core_0 * 16 * 16 + jj__cnn_kernel_sub_core_0 * 16 + ii__cnn_kernel_sub_core_0];
                  }
                }
                for (int hh_t__cnn_kernel_sub_core_0 = 0; hh_t__cnn_kernel_sub_core_0 < 1 + 28 + 2; hh_t__cnn_kernel_sub_core_0++) {
                  for (int ww__cnn_kernel_sub_core_0 = 0; ww__cnn_kernel_sub_core_0 < 28 + 2; ww__cnn_kernel_sub_core_0++) 
// Original: #pragma ACCEL pipeline flatten
{
                    
#pragma ACCEL PIPELINE II=1
                    if (hh_t__cnn_kernel_sub_core_0 == 0) {
                      if (ww__cnn_kernel_sub_core_0 < 16) {
                        for (int ii__cnn_kernel_sub_core_0 = 0; ii__cnn_kernel_sub_core_0 < 16; ii__cnn_kernel_sub_core_0++) {
                          
#pragma ACCEL PARALLEL COMPLETE
                          new_value_0__cnn_kernel_sub_core_0_0 = Weight_buf__cnn_kernel_sub_core_0_0[ww__cnn_kernel_sub_core_0][ii__cnn_kernel_sub_core_0];
// shift register
// jj+1 -> jj 
// new -> 15
                          for (int jj__cnn_kernel_sub_core_0 = 0; jj__cnn_kernel_sub_core_0 < 15; jj__cnn_kernel_sub_core_0++) {
                            
#pragma ACCEL PARALLEL COMPLETE
                            Weight_reg__cnn_kernel_sub_core_0_0[jj__cnn_kernel_sub_core_0][ii__cnn_kernel_sub_core_0] = Weight_reg__cnn_kernel_sub_core_0_0[jj__cnn_kernel_sub_core_0 + 1][ii__cnn_kernel_sub_core_0];
                          }
                          Weight_reg__cnn_kernel_sub_core_0_0[15][ii__cnn_kernel_sub_core_0] = new_value_0__cnn_kernel_sub_core_0_0;
                        }
                      }
                    }
//else {
                    if (hh_t__cnn_kernel_sub_core_0 > 0) {
                      hh_0__cnn_kernel_sub_core_0_0 = hh_t__cnn_kernel_sub_core_0 - 1;
                      h_1__cnn_kernel_sub_core_0_0 = h0__cnn_kernel_sub * 28 + hh_0__cnn_kernel_sub_core_0_0;
                      w_1__cnn_kernel_sub_core_0_0 = w0__cnn_kernel_sub * 28 + ww__cnn_kernel_sub_core_0;
                      if (p__cnn_kernel_sub_core_0 == 0 && q__cnn_kernel_sub_core_0 == 0) {
                        for (int ii__cnn_kernel_sub_core_0 = 0; ii__cnn_kernel_sub_core_0 < 16; ++ii__cnn_kernel_sub_core_0) {
                          
#pragma ACCEL PARALLEL COMPLETE
                          Cin_tmp__cnn_kernel_sub_core_0_0[hh_0__cnn_kernel_sub_core_0_0][ww__cnn_kernel_sub_core_0][ii__cnn_kernel_sub_core_0] = Cin_buf__cnn_kernel_sub_core_0_0[hh_0__cnn_kernel_sub_core_0_0][ww__cnn_kernel_sub_core_0][ii__cnn_kernel_sub_core_0];
                        }
                      }
                      for (int ii__cnn_kernel_sub_core_0 = 0; ii__cnn_kernel_sub_core_0 < 16; ++ii__cnn_kernel_sub_core_0) {
                        
#pragma ACCEL PARALLEL COMPLETE
                        bias_t_0__cnn_kernel_sub_core_0_0[ii__cnn_kernel_sub_core_0] = bias[ii__cnn_kernel_sub_core_0];
                      }
                      if (ww__cnn_kernel_sub_core_0 >= 2 && hh_0__cnn_kernel_sub_core_0_0 >= 2) {{
                          if (p__cnn_kernel_sub_core_0 == 0 && q__cnn_kernel_sub_core_0 == 0) {
                            for (int ii__cnn_kernel_sub_core_0 = 0; ii__cnn_kernel_sub_core_0 < 16; ++ii__cnn_kernel_sub_core_0) {
                              
#pragma ACCEL PARALLEL COMPLETE
                              Cout_tmp__cnn_kernel_sub_core_0_0[hh_0__cnn_kernel_sub_core_0_0 - 2][ww__cnn_kernel_sub_core_0 - 2][ii__cnn_kernel_sub_core_0] = ((float )0);
                            }
                          }
                        }
                      }
                      if (ww__cnn_kernel_sub_core_0 >= 2 && hh_0__cnn_kernel_sub_core_0_0 >= 2) {
                        for (int ii__cnn_kernel_sub_core_0 = 0; ii__cnn_kernel_sub_core_0 < 16; ii__cnn_kernel_sub_core_0++) {
                          
#pragma ACCEL PARALLEL COMPLETE
                          tmp_0__cnn_kernel_sub_core_0_0[ii__cnn_kernel_sub_core_0] = ((float )0.0);
                        }
                        for (int jj__cnn_kernel_sub_core_0 = 0; jj__cnn_kernel_sub_core_0 < 16; jj__cnn_kernel_sub_core_0++) {
                          
#pragma ACCEL PARALLEL COMPLETE
                          for (int ii__cnn_kernel_sub_core_0 = 0; ii__cnn_kernel_sub_core_0 < 16; ii__cnn_kernel_sub_core_0++) {
                            
#pragma ACCEL PARALLEL COMPLETE
                            tmp_0__cnn_kernel_sub_core_0_0[ii__cnn_kernel_sub_core_0] += Weight_reg__cnn_kernel_sub_core_0_0[jj__cnn_kernel_sub_core_0][ii__cnn_kernel_sub_core_0] * Cin_tmp__cnn_kernel_sub_core_0_0[hh_0__cnn_kernel_sub_core_0_0 + p__cnn_kernel_sub_core_0 - 2][ww__cnn_kernel_sub_core_0 + q__cnn_kernel_sub_core_0 - 2][jj__cnn_kernel_sub_core_0];
//weight[i0 * TILE * NUM * KERNEL * KERNEL +
//j0 * TILE * KERNEL * KERNEL * TILE +
//p * KERNEL * TILE * TILE + q * TILE * TILE +
//jj * TILE + ii] 
                          }
                        }
                        for (int ii__cnn_kernel_sub_core_0 = 0; ii__cnn_kernel_sub_core_0 < 16; ++ii__cnn_kernel_sub_core_0) {
                          
#pragma ACCEL PARALLEL COMPLETE
                          Cout_tmp__cnn_kernel_sub_core_0_0[hh_0__cnn_kernel_sub_core_0_0 - 2][ww__cnn_kernel_sub_core_0 - 2][ii__cnn_kernel_sub_core_0] += tmp_0__cnn_kernel_sub_core_0_0[ii__cnn_kernel_sub_core_0];
                        }
                        for (int ii__cnn_kernel_sub_core_0 = 0; ii__cnn_kernel_sub_core_0 < 16; ++ii__cnn_kernel_sub_core_0) {
                          
#pragma ACCEL PARALLEL COMPLETE
                          Cout_buf1__cnn_kernel_sub_1[hh_0__cnn_kernel_sub_core_0_0 - 2][ww__cnn_kernel_sub_core_0 - 2][ii__cnn_kernel_sub_core_0] = Cout_tmp__cnn_kernel_sub_core_0_0[hh_0__cnn_kernel_sub_core_0_0 - 2][ww__cnn_kernel_sub_core_0 - 2][ii__cnn_kernel_sub_core_0];
                        }
                      }
                    }
// ww
                  }
// hh
                }
// q
              }
// p
            }
// j0
          }
        }
        for (int hh__cnn_kernel_sub = 0; hh__cnn_kernel_sub < 28; hh__cnn_kernel_sub++) {
          for (int ww__cnn_kernel_sub = 0; ww__cnn_kernel_sub < 28; ww__cnn_kernel_sub++) 
// Original: #pragma ACCEL pipeline flatten
{
            
#pragma ACCEL PIPELINE II=1
            for (int ii__cnn_kernel_sub = 0; ii__cnn_kernel_sub < 16; ++ii__cnn_kernel_sub) {
              
#pragma ACCEL PARALLEL COMPLETE
              Cout[0 * 16 * 224 * 224 + h0__cnn_kernel_sub * 28 * 28 * 8 * 16 + w0__cnn_kernel_sub * 28 * 28 * 16 + hh__cnn_kernel_sub * 28 * 16 + ww__cnn_kernel_sub * 16 + ii__cnn_kernel_sub] = Cout_buf0__cnn_kernel_sub_1[hh__cnn_kernel_sub][ww__cnn_kernel_sub][ii__cnn_kernel_sub] + Cout_buf1__cnn_kernel_sub_1[hh__cnn_kernel_sub][ww__cnn_kernel_sub][ii__cnn_kernel_sub];
            }
            ;
          }
        }
// h0
      }
    }
  }
{
    for (int w0__cnn_kernel_sub_0 = 0; w0__cnn_kernel_sub_0 < 8; w0__cnn_kernel_sub_0++) {
      for (int h0__cnn_kernel_sub_0 = 0; h0__cnn_kernel_sub_0 < 8; h0__cnn_kernel_sub_0++) {{{
            for (int hh__cnn_kernel_sub_core_1 = 0; hh__cnn_kernel_sub_core_1 < 28 + 2; hh__cnn_kernel_sub_core_1++) {
              for (int ww__cnn_kernel_sub_core_1 = 0; ww__cnn_kernel_sub_core_1 < 28 + 2; ww__cnn_kernel_sub_core_1++) 
// Original: #pragma ACCEL pipeline flatten
{
                
#pragma ACCEL PIPELINE II=1
                h_0__cnn_kernel_sub_core_1_0 = h0__cnn_kernel_sub_0 * 28 + hh__cnn_kernel_sub_core_1;
                w_0__cnn_kernel_sub_core_1_0 = w0__cnn_kernel_sub_0 * 28 + ww__cnn_kernel_sub_core_1;
                for (int ii__cnn_kernel_sub_core_1 = 0; ii__cnn_kernel_sub_core_1 < 16; ++ii__cnn_kernel_sub_core_1) {
                  
#pragma ACCEL PARALLEL COMPLETE
                  Cin_buf__cnn_kernel_sub_core_1_0[hh__cnn_kernel_sub_core_1][ww__cnn_kernel_sub_core_1][ii__cnn_kernel_sub_core_1] = Cin1[0 * 16 * 240 * 240 + h0__cnn_kernel_sub_0 * 30 * 30 * 8 * 16 + w0__cnn_kernel_sub_0 * 30 * 30 * 16 + hh__cnn_kernel_sub_core_1 * 30 * 16 + ww__cnn_kernel_sub_core_1 * 16 + ii__cnn_kernel_sub_core_1];
                }
              }
            }
            for (int p__cnn_kernel_sub_core_1 = 0; p__cnn_kernel_sub_core_1 < 3; p__cnn_kernel_sub_core_1++) {
              for (int q__cnn_kernel_sub_core_1 = 0; q__cnn_kernel_sub_core_1 < 3; q__cnn_kernel_sub_core_1++) {
                for (int jj__cnn_kernel_sub_core_1 = 0; jj__cnn_kernel_sub_core_1 < 16; jj__cnn_kernel_sub_core_1++) {
                  for (int ii__cnn_kernel_sub_core_1 = 0; ii__cnn_kernel_sub_core_1 < 16; ii__cnn_kernel_sub_core_1++) 
// Original: #pragma ACCEL parallel
{
                    
#pragma ACCEL PARALLEL COMPLETE
                    Weight_buf__cnn_kernel_sub_core_1_0[jj__cnn_kernel_sub_core_1][ii__cnn_kernel_sub_core_1] = weight1[1 * 16 * 32 * 3 * 3 + 0 * 16 * 3 * 3 * 16 + p__cnn_kernel_sub_core_1 * 3 * 16 * 16 + q__cnn_kernel_sub_core_1 * 16 * 16 + jj__cnn_kernel_sub_core_1 * 16 + ii__cnn_kernel_sub_core_1];
                  }
                }
                for (int hh_t__cnn_kernel_sub_core_1 = 0; hh_t__cnn_kernel_sub_core_1 < 1 + 28 + 2; hh_t__cnn_kernel_sub_core_1++) {
                  for (int ww__cnn_kernel_sub_core_1 = 0; ww__cnn_kernel_sub_core_1 < 28 + 2; ww__cnn_kernel_sub_core_1++) 
// Original: #pragma ACCEL pipeline flatten
{
                    
#pragma ACCEL PIPELINE II=1
                    if (hh_t__cnn_kernel_sub_core_1 == 0) {
                      if (ww__cnn_kernel_sub_core_1 < 16) {
                        for (int ii__cnn_kernel_sub_core_1 = 0; ii__cnn_kernel_sub_core_1 < 16; ii__cnn_kernel_sub_core_1++) {
                          
#pragma ACCEL PARALLEL COMPLETE
                          new_value_0__cnn_kernel_sub_core_1_0 = Weight_buf__cnn_kernel_sub_core_1_0[ww__cnn_kernel_sub_core_1][ii__cnn_kernel_sub_core_1];
// shift register
// jj+1 -> jj 
// new -> 15
                          for (int jj__cnn_kernel_sub_core_1 = 0; jj__cnn_kernel_sub_core_1 < 15; jj__cnn_kernel_sub_core_1++) {
                            
#pragma ACCEL PARALLEL COMPLETE
                            Weight_reg__cnn_kernel_sub_core_1_0[jj__cnn_kernel_sub_core_1][ii__cnn_kernel_sub_core_1] = Weight_reg__cnn_kernel_sub_core_1_0[jj__cnn_kernel_sub_core_1 + 1][ii__cnn_kernel_sub_core_1];
                          }
                          Weight_reg__cnn_kernel_sub_core_1_0[15][ii__cnn_kernel_sub_core_1] = new_value_0__cnn_kernel_sub_core_1_0;
                        }
                      }
                    }
//else {
                    if (hh_t__cnn_kernel_sub_core_1 > 0) {
                      hh_0__cnn_kernel_sub_core_1_0 = hh_t__cnn_kernel_sub_core_1 - 1;
                      h_1__cnn_kernel_sub_core_1_0 = h0__cnn_kernel_sub_0 * 28 + hh_0__cnn_kernel_sub_core_1_0;
                      w_1__cnn_kernel_sub_core_1_0 = w0__cnn_kernel_sub_0 * 28 + ww__cnn_kernel_sub_core_1;
                      if (p__cnn_kernel_sub_core_1 == 0 && q__cnn_kernel_sub_core_1 == 0) {
                        for (int ii__cnn_kernel_sub_core_1 = 0; ii__cnn_kernel_sub_core_1 < 16; ++ii__cnn_kernel_sub_core_1) {
                          
#pragma ACCEL PARALLEL COMPLETE
                          Cin_tmp__cnn_kernel_sub_core_1_0[hh_0__cnn_kernel_sub_core_1_0][ww__cnn_kernel_sub_core_1][ii__cnn_kernel_sub_core_1] = Cin_buf__cnn_kernel_sub_core_1_0[hh_0__cnn_kernel_sub_core_1_0][ww__cnn_kernel_sub_core_1][ii__cnn_kernel_sub_core_1];
                        }
                      }
                      for (int ii__cnn_kernel_sub_core_1 = 0; ii__cnn_kernel_sub_core_1 < 16; ++ii__cnn_kernel_sub_core_1) {
                        
#pragma ACCEL PARALLEL COMPLETE
                        bias_t_0__cnn_kernel_sub_core_1_0[ii__cnn_kernel_sub_core_1] = bias1[ii__cnn_kernel_sub_core_1];
                      }
                      if (ww__cnn_kernel_sub_core_1 >= 2 && hh_0__cnn_kernel_sub_core_1_0 >= 2) {
                        if (p__cnn_kernel_sub_core_1 == 0 && q__cnn_kernel_sub_core_1 == 0) {
                          for (int ii__cnn_kernel_sub_core_1 = 0; ii__cnn_kernel_sub_core_1 < 16; ++ii__cnn_kernel_sub_core_1) {
                            
#pragma ACCEL PARALLEL COMPLETE
//bias_t[ii]; //bias[i0 * TILE  + ii];
                            Cout_tmp__cnn_kernel_sub_core_1_0[hh_0__cnn_kernel_sub_core_1_0 - 2][ww__cnn_kernel_sub_core_1 - 2][ii__cnn_kernel_sub_core_1] = ((float )0);
                          }
                        }
                         else {
                        }
                      }
                      if (ww__cnn_kernel_sub_core_1 >= 2 && hh_0__cnn_kernel_sub_core_1_0 >= 2) {
                        for (int ii__cnn_kernel_sub_core_1 = 0; ii__cnn_kernel_sub_core_1 < 16; ii__cnn_kernel_sub_core_1++) {
                          
#pragma ACCEL PARALLEL COMPLETE
                          tmp_0__cnn_kernel_sub_core_1_0[ii__cnn_kernel_sub_core_1] = ((float )0.0);
                        }
                        for (int jj__cnn_kernel_sub_core_1 = 0; jj__cnn_kernel_sub_core_1 < 16; jj__cnn_kernel_sub_core_1++) {
                          
#pragma ACCEL PARALLEL COMPLETE
                          for (int ii__cnn_kernel_sub_core_1 = 0; ii__cnn_kernel_sub_core_1 < 16; ii__cnn_kernel_sub_core_1++) {
                            
#pragma ACCEL PARALLEL COMPLETE
                            tmp_0__cnn_kernel_sub_core_1_0[ii__cnn_kernel_sub_core_1] += Weight_reg__cnn_kernel_sub_core_1_0[jj__cnn_kernel_sub_core_1][ii__cnn_kernel_sub_core_1] * Cin_tmp__cnn_kernel_sub_core_1_0[hh_0__cnn_kernel_sub_core_1_0 + p__cnn_kernel_sub_core_1 - 2][ww__cnn_kernel_sub_core_1 + q__cnn_kernel_sub_core_1 - 2][jj__cnn_kernel_sub_core_1];
//weight[i0 * TILE * NUM * KERNEL * KERNEL +
//j0 * TILE * KERNEL * KERNEL * TILE +
//p * KERNEL * TILE * TILE + q * TILE * TILE +
//jj * TILE + ii] 
                          }
                        }
                        for (int ii__cnn_kernel_sub_core_1 = 0; ii__cnn_kernel_sub_core_1 < 16; ++ii__cnn_kernel_sub_core_1) {
                          
#pragma ACCEL PARALLEL COMPLETE
                          Cout_tmp__cnn_kernel_sub_core_1_0[hh_0__cnn_kernel_sub_core_1_0 - 2][ww__cnn_kernel_sub_core_1 - 2][ii__cnn_kernel_sub_core_1] += tmp_0__cnn_kernel_sub_core_1_0[ii__cnn_kernel_sub_core_1];
                        }
                        for (int ii__cnn_kernel_sub_core_1 = 0; ii__cnn_kernel_sub_core_1 < 16; ++ii__cnn_kernel_sub_core_1) {
                          
#pragma ACCEL PARALLEL COMPLETE
                          Cout_buf0__cnn_kernel_sub_0_0[hh_0__cnn_kernel_sub_core_1_0 - 2][ww__cnn_kernel_sub_core_1 - 2][ii__cnn_kernel_sub_core_1] = Cout_tmp__cnn_kernel_sub_core_1_0[hh_0__cnn_kernel_sub_core_1_0 - 2][ww__cnn_kernel_sub_core_1 - 2][ii__cnn_kernel_sub_core_1];
                        }
                      }
                    }
// ww
                  }
// hh
                }
// q
              }
// p
            }
// j0
          }
        }
{
{
            for (int hh__cnn_kernel_sub_core_2 = 0; hh__cnn_kernel_sub_core_2 < 28 + 2; hh__cnn_kernel_sub_core_2++) {
              for (int ww__cnn_kernel_sub_core_2 = 0; ww__cnn_kernel_sub_core_2 < 28 + 2; ww__cnn_kernel_sub_core_2++) 
// Original: #pragma ACCEL pipeline flatten
{
                
#pragma ACCEL PIPELINE II=1
                h_0__cnn_kernel_sub_core_2_0 = h0__cnn_kernel_sub_0 * 28 + hh__cnn_kernel_sub_core_2;
                w_0__cnn_kernel_sub_core_2_0 = w0__cnn_kernel_sub_0 * 28 + ww__cnn_kernel_sub_core_2;
                for (int ii__cnn_kernel_sub_core_2 = 0; ii__cnn_kernel_sub_core_2 < 16; ++ii__cnn_kernel_sub_core_2) {
                  
#pragma ACCEL PARALLEL COMPLETE
                  Cin_buf__cnn_kernel_sub_core_2_0[hh__cnn_kernel_sub_core_2][ww__cnn_kernel_sub_core_2][ii__cnn_kernel_sub_core_2] = Cin1[1 * 16 * 240 * 240 + h0__cnn_kernel_sub_0 * 30 * 30 * 8 * 16 + w0__cnn_kernel_sub_0 * 30 * 30 * 16 + hh__cnn_kernel_sub_core_2 * 30 * 16 + ww__cnn_kernel_sub_core_2 * 16 + ii__cnn_kernel_sub_core_2];
                }
              }
            }
            for (int p__cnn_kernel_sub_core_2 = 0; p__cnn_kernel_sub_core_2 < 3; p__cnn_kernel_sub_core_2++) {
              for (int q__cnn_kernel_sub_core_2 = 0; q__cnn_kernel_sub_core_2 < 3; q__cnn_kernel_sub_core_2++) {
                for (int jj__cnn_kernel_sub_core_2 = 0; jj__cnn_kernel_sub_core_2 < 16; jj__cnn_kernel_sub_core_2++) {
                  for (int ii__cnn_kernel_sub_core_2 = 0; ii__cnn_kernel_sub_core_2 < 16; ii__cnn_kernel_sub_core_2++) 
// Original: #pragma ACCEL parallel
{
                    
#pragma ACCEL PARALLEL COMPLETE
                    Weight_buf__cnn_kernel_sub_core_2_0[jj__cnn_kernel_sub_core_2][ii__cnn_kernel_sub_core_2] = weight1[1 * 16 * 32 * 3 * 3 + 1 * 16 * 3 * 3 * 16 + p__cnn_kernel_sub_core_2 * 3 * 16 * 16 + q__cnn_kernel_sub_core_2 * 16 * 16 + jj__cnn_kernel_sub_core_2 * 16 + ii__cnn_kernel_sub_core_2];
                  }
                }
                for (int hh_t__cnn_kernel_sub_core_2 = 0; hh_t__cnn_kernel_sub_core_2 < 1 + 28 + 2; hh_t__cnn_kernel_sub_core_2++) {
                  for (int ww__cnn_kernel_sub_core_2 = 0; ww__cnn_kernel_sub_core_2 < 28 + 2; ww__cnn_kernel_sub_core_2++) 
// Original: #pragma ACCEL pipeline flatten
{
                    
#pragma ACCEL PIPELINE II=1
                    if (hh_t__cnn_kernel_sub_core_2 == 0) {
                      if (ww__cnn_kernel_sub_core_2 < 16) {
                        for (int ii__cnn_kernel_sub_core_2 = 0; ii__cnn_kernel_sub_core_2 < 16; ii__cnn_kernel_sub_core_2++) {
                          
#pragma ACCEL PARALLEL COMPLETE
                          new_value_0__cnn_kernel_sub_core_2_0 = Weight_buf__cnn_kernel_sub_core_2_0[ww__cnn_kernel_sub_core_2][ii__cnn_kernel_sub_core_2];
// shift register
// jj+1 -> jj 
// new -> 15
                          for (int jj__cnn_kernel_sub_core_2 = 0; jj__cnn_kernel_sub_core_2 < 15; jj__cnn_kernel_sub_core_2++) {
                            
#pragma ACCEL PARALLEL COMPLETE
                            Weight_reg__cnn_kernel_sub_core_2_0[jj__cnn_kernel_sub_core_2][ii__cnn_kernel_sub_core_2] = Weight_reg__cnn_kernel_sub_core_2_0[jj__cnn_kernel_sub_core_2 + 1][ii__cnn_kernel_sub_core_2];
                          }
                          Weight_reg__cnn_kernel_sub_core_2_0[15][ii__cnn_kernel_sub_core_2] = new_value_0__cnn_kernel_sub_core_2_0;
                        }
                      }
                    }
//else {
                    if (hh_t__cnn_kernel_sub_core_2 > 0) {
                      hh_0__cnn_kernel_sub_core_2_0 = hh_t__cnn_kernel_sub_core_2 - 1;
                      h_1__cnn_kernel_sub_core_2_0 = h0__cnn_kernel_sub_0 * 28 + hh_0__cnn_kernel_sub_core_2_0;
                      w_1__cnn_kernel_sub_core_2_0 = w0__cnn_kernel_sub_0 * 28 + ww__cnn_kernel_sub_core_2;
                      if (p__cnn_kernel_sub_core_2 == 0 && q__cnn_kernel_sub_core_2 == 0) {
                        for (int ii__cnn_kernel_sub_core_2 = 0; ii__cnn_kernel_sub_core_2 < 16; ++ii__cnn_kernel_sub_core_2) {
                          
#pragma ACCEL PARALLEL COMPLETE
                          Cin_tmp__cnn_kernel_sub_core_2_0[hh_0__cnn_kernel_sub_core_2_0][ww__cnn_kernel_sub_core_2][ii__cnn_kernel_sub_core_2] = Cin_buf__cnn_kernel_sub_core_2_0[hh_0__cnn_kernel_sub_core_2_0][ww__cnn_kernel_sub_core_2][ii__cnn_kernel_sub_core_2];
                        }
                      }
                      for (int ii__cnn_kernel_sub_core_2 = 0; ii__cnn_kernel_sub_core_2 < 16; ++ii__cnn_kernel_sub_core_2) {
                        
#pragma ACCEL PARALLEL COMPLETE
                        bias_t_0__cnn_kernel_sub_core_2_0[ii__cnn_kernel_sub_core_2] = bias1[ii__cnn_kernel_sub_core_2];
                      }
                      if (ww__cnn_kernel_sub_core_2 >= 2 && hh_0__cnn_kernel_sub_core_2_0 >= 2) {{
                          if (p__cnn_kernel_sub_core_2 == 0 && q__cnn_kernel_sub_core_2 == 0) {
                            for (int ii__cnn_kernel_sub_core_2 = 0; ii__cnn_kernel_sub_core_2 < 16; ++ii__cnn_kernel_sub_core_2) {
                              
#pragma ACCEL PARALLEL COMPLETE
                              Cout_tmp__cnn_kernel_sub_core_2_0[hh_0__cnn_kernel_sub_core_2_0 - 2][ww__cnn_kernel_sub_core_2 - 2][ii__cnn_kernel_sub_core_2] = ((float )0);
                            }
                          }
                        }
                      }
                      if (ww__cnn_kernel_sub_core_2 >= 2 && hh_0__cnn_kernel_sub_core_2_0 >= 2) {
                        for (int ii__cnn_kernel_sub_core_2 = 0; ii__cnn_kernel_sub_core_2 < 16; ii__cnn_kernel_sub_core_2++) {
                          
#pragma ACCEL PARALLEL COMPLETE
                          tmp_0__cnn_kernel_sub_core_2_0[ii__cnn_kernel_sub_core_2] = ((float )0.0);
                        }
                        for (int jj__cnn_kernel_sub_core_2 = 0; jj__cnn_kernel_sub_core_2 < 16; jj__cnn_kernel_sub_core_2++) {
                          
#pragma ACCEL PARALLEL COMPLETE
                          for (int ii__cnn_kernel_sub_core_2 = 0; ii__cnn_kernel_sub_core_2 < 16; ii__cnn_kernel_sub_core_2++) {
                            
#pragma ACCEL PARALLEL COMPLETE
                            tmp_0__cnn_kernel_sub_core_2_0[ii__cnn_kernel_sub_core_2] += Weight_reg__cnn_kernel_sub_core_2_0[jj__cnn_kernel_sub_core_2][ii__cnn_kernel_sub_core_2] * Cin_tmp__cnn_kernel_sub_core_2_0[hh_0__cnn_kernel_sub_core_2_0 + p__cnn_kernel_sub_core_2 - 2][ww__cnn_kernel_sub_core_2 + q__cnn_kernel_sub_core_2 - 2][jj__cnn_kernel_sub_core_2];
//weight[i0 * TILE * NUM * KERNEL * KERNEL +
//j0 * TILE * KERNEL * KERNEL * TILE +
//p * KERNEL * TILE * TILE + q * TILE * TILE +
//jj * TILE + ii] 
                          }
                        }
                        for (int ii__cnn_kernel_sub_core_2 = 0; ii__cnn_kernel_sub_core_2 < 16; ++ii__cnn_kernel_sub_core_2) {
                          
#pragma ACCEL PARALLEL COMPLETE
                          Cout_tmp__cnn_kernel_sub_core_2_0[hh_0__cnn_kernel_sub_core_2_0 - 2][ww__cnn_kernel_sub_core_2 - 2][ii__cnn_kernel_sub_core_2] += tmp_0__cnn_kernel_sub_core_2_0[ii__cnn_kernel_sub_core_2];
                        }
                        for (int ii__cnn_kernel_sub_core_2 = 0; ii__cnn_kernel_sub_core_2 < 16; ++ii__cnn_kernel_sub_core_2) {
                          
#pragma ACCEL PARALLEL COMPLETE
                          Cout_buf1__cnn_kernel_sub_0_0[hh_0__cnn_kernel_sub_core_2_0 - 2][ww__cnn_kernel_sub_core_2 - 2][ii__cnn_kernel_sub_core_2] = Cout_tmp__cnn_kernel_sub_core_2_0[hh_0__cnn_kernel_sub_core_2_0 - 2][ww__cnn_kernel_sub_core_2 - 2][ii__cnn_kernel_sub_core_2];
                        }
                      }
                    }
// ww
                  }
// hh
                }
// q
              }
// p
            }
// j0
          }
        }
        for (int hh__cnn_kernel_sub_0 = 0; hh__cnn_kernel_sub_0 < 28; hh__cnn_kernel_sub_0++) {
          for (int ww__cnn_kernel_sub_0 = 0; ww__cnn_kernel_sub_0 < 28; ww__cnn_kernel_sub_0++) 
// Original: #pragma ACCEL pipeline flatten
{
            
#pragma ACCEL PIPELINE II=1
            for (int ii__cnn_kernel_sub_0 = 0; ii__cnn_kernel_sub_0 < 16; ++ii__cnn_kernel_sub_0) {
              
#pragma ACCEL PARALLEL COMPLETE
              Cout1[1 * 16 * 224 * 224 + h0__cnn_kernel_sub_0 * 28 * 28 * 8 * 16 + w0__cnn_kernel_sub_0 * 28 * 28 * 16 + hh__cnn_kernel_sub_0 * 28 * 16 + ww__cnn_kernel_sub_0 * 16 + ii__cnn_kernel_sub_0] = Cout_buf0__cnn_kernel_sub_0_0[hh__cnn_kernel_sub_0][ww__cnn_kernel_sub_0][ii__cnn_kernel_sub_0] + Cout_buf1__cnn_kernel_sub_0_0[hh__cnn_kernel_sub_0][ww__cnn_kernel_sub_0][ii__cnn_kernel_sub_0];
            }
            ;
          }
        }
// h0
      }
    }
  }
}
