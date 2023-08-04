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
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#define CHUNK_SIZE 16
#define L 10 //label_size;
#define D 784 //feature_size;
#define weight_size (L-1)*D
typedef double BUS_TYPE;
// to divide feature_size (D)
#define F_UR 4

#pragma ACCEL kernel
void gradient(int n_samples,int label_size,int feature_size,double *weights,BUS_TYPE *data,double *output,double *output_last_element,double *feature_samples,double *label_samples)
//double   global_weights[9*784], 
//BUS_TYPE global_data[32*785], 
//double   global_output[9*784+1])
{
  double margins_0_rn[16][10 - 1];
  double maxMargin_buf_0_tmp_0;
  double maxMargin_buf_0_rn_0[16];
  double maxMargin_buf_0_tmp;
  double maxMargin_buf_0_rn[16];
  double multiplier_buf_0_tmp;
  double multiplier_buf_0_rn[16][10];
  double margin_buf_0_rn[16][10];
  double sum_buf_0_rn_0[16];
  double sum_buf_0_rn[16];
  double output_loss_buffer_0_rn;
  int _memcpy_i1_2;
  int _memcpy_i1_1;
  int _memcpy_i1_0;
  int _memcpy_i0_2;
  int _memcpy_i0_1;
  int _memcpy_i0_0;
//FIXME:
// const int L = 10; //label_size;
// const int D = 784; //feature_size;
// const int weight_size = (L-1)*D;
  int num_samples;
// ZP: 2015-11-15
//memset(output, 0, sizeof(double)*(weight_size+1));
//double* margins = new double[L-1];
//-std::numeric_limits<double>::infinity();
  double margins_0[16][10 - 1];
  double label_buf_0[16];
  double feature_buf_0[16][784];
  double marginY_buf_0[16];
  double maxMargin_buf_0[16];
  int maxMarginIndex_buf_0[16];
  int upper_bound_0;
  int j0_0;
  int kk_0;
  int i_0;
  double output_loss_buffer_0;
  double sum_buf_0[16];
  double margin_buf_0[16][10];
  double multiplier_buf_0[16][10];
  int t_size_0;
  int t_0;
  int k_0;
  int k_1;
  int j_0;
  int j_1;
  double label_0;
  double label_1;
  int j_2;
  double label_2;
  double loss_0;
  double weights_buf_0_0[9][196][4];
  double feature_samples_buf_0_0[16][49][16];
  double label_samples_buf_0_0[16];
  int __memcpy_i_2_0;
  num_samples = n_samples;
  upper_bound_0 = (n_samples + 16 - 1) / 16;
//for(int k = 0; k < num_samples; k++ ) {
  n_samples % 16 == 0?((void )0) : __assert_fail("(n_samples % 16) == 0","gradient.cpp",51,__PRETTY_FUNCTION__);
//double output_loss_buffer = output[weight_size];
  output_loss_buffer_0 =  *output_last_element;
  for (int k0 = 0; k0 < (n_samples + 16 - 1) / 16; k0++) 
// Original: #pragma ACCEL pipeline
{
    for (_memcpy_i0_2 = 0; _memcpy_i0_2 < 9; ++_memcpy_i0_2) {
      for (_memcpy_i0_1 = 0; _memcpy_i0_1 < 196; ++_memcpy_i0_1) {
        for (_memcpy_i0_0 = 0; _memcpy_i0_0 < 4; ++_memcpy_i0_0) {
          weights_buf_0_0[_memcpy_i0_2][_memcpy_i0_1][_memcpy_i0_0] = weights[0 + (((0 * 9 + _memcpy_i0_2) * 196 + _memcpy_i0_1) * 4 + _memcpy_i0_0)];
        }
      }
    }
    
#pragma ACCEL PIPELINE II=1
//#pragma HLS loop_tripcount max=3750
    
#pragma HLS loop_tripcount max=256
// combine these two nodes because they both access data
{
      for (_memcpy_i1_2 = 0; _memcpy_i1_2 < 16; ++_memcpy_i1_2) {
        for (_memcpy_i1_1 = 0; _memcpy_i1_1 < 49; ++_memcpy_i1_1) {
          for (_memcpy_i1_0 = 0; _memcpy_i1_0 < 16; ++_memcpy_i1_0) {
            feature_samples_buf_0_0[_memcpy_i1_2][_memcpy_i1_1][_memcpy_i1_0] = feature_samples[k0 * 12544 + (((0 * 16 + _memcpy_i1_2) * 49 + _memcpy_i1_1) * 16 + _memcpy_i1_0)];
          }
        }
      }
      for (__memcpy_i_2_0 = 0; __memcpy_i_2_0 < sizeof(double ) * ((unsigned long )16) / 8; ++__memcpy_i_2_0) {
        label_samples_buf_0_0[__memcpy_i_2_0 + 0] = label_samples[__memcpy_i_2_0 + k0 * 16];
      }
      t_size_0 = 16;
      for (t_0 = 0; t_0 < 16; t_0++) {
        sum_buf_0[t_0] = 0;
      }
      for (t_0 = 0; t_0 < 16; t_0++) {
        for (int t1 = 0; t1 < 10; t1++) {
          margin_buf_0[t_0][t1] = 0;
        }
      }
      for (t_0 = 0; t_0 < 16; t_0++) {
        for (int t1 = 0; t1 < 10; t1++) {
          multiplier_buf_0[t_0][t1] = 0;
        }
      }
//#pragma ACCEL pipeline 
/*INIT: */
      for (int kk = 0; kk < 16; kk++) {
        k_0 = k0 * 16 + kk;
//              if (k >= num_samples) break;
        marginY_buf_0[kk] = 0.0;
//-std::numeric_limits<double>::infinity();
        maxMargin_buf_0[kk] = (-__builtin_inff());
        maxMarginIndex_buf_0[kk] = 0;
//label_buf[kk] = data[k*(D+1)];
        label_buf_0[kk] = label_samples_buf_0_0[kk];
      }
//      }
//
//      {
      for (int j0 = 0; j0 < 784 / 16; j0 += 1) {
        for (int kk = 0; kk < 16; kk++) 
// Original: #pragma ACCEL pipeline flatten
{
          
#pragma ACCEL PIPELINE II=1
          for (int jj = 0; jj < 16; jj += 1) {
            
#pragma ACCEL PARALLEL COMPLETE
            k_1 = k0 * 16 + kk;
            j_0 = j0 * 16 + jj;
            feature_buf_0[kk][j0 * 16 + jj] = feature_samples_buf_0_0[kk][j0][jj];
          }
        }
      }
    }
/*STAGE_1:*/
//for(j0 = 0; j0 < D; j0+=F_UR ) {
    for (j0_0 = 0; j0_0 < 784 / 4; j0_0 += 1) {
      for (kk_0 = 0; kk_0 < 16; kk_0++) 
// Original: #pragma ACCEL pipeline flatten
{
        
#pragma ACCEL PIPELINE II=1
        for (int jj = 0; jj < 4; jj += 1) {
          
#pragma ACCEL PARALLEL COMPLETE
          for (int i = 0; i < 10 - 1; i++) {
            
#pragma ACCEL PARALLEL COMPLETE
            j_1 = j0_0 * 4 + jj;
            margin_buf_0[kk_0][i] += weights_buf_0_0[i][j0_0][jj] * feature_buf_0[kk_0][j0_0 * 4 + jj];
            margin_buf_0_rn[kk_0][i] = margin_buf_0[kk_0][i];
          }
        }
      }
    }
// STAGE_2:      
{
//#pragma ACCEL pipeline
      for (int kk = 0; kk < 16; kk++) {
        label_0 = label_buf_0[kk];
        marginY_buf_0[kk] = margin_buf_0[kk][((int )label_buf_0[kk]) - 1];
      }
      for (i_0 = 0; i_0 < 10 - 1; i_0++) {
        for (kk_0 = 0; kk_0 < 16; kk_0++) {
//  #pragma ACCEL pipeline II=2
          if (margin_buf_0_rn[kk_0][i_0] > maxMargin_buf_0[kk_0]) {
            maxMargin_buf_0_tmp = margin_buf_0_rn[kk_0][i_0];
            maxMargin_buf_0_tmp_0 = maxMargin_buf_0_tmp;
            maxMargin_buf_0[kk_0] = maxMargin_buf_0_tmp_0;
            maxMargin_buf_0_rn_0[kk_0] = maxMargin_buf_0_tmp_0;
            maxMargin_buf_0_rn[kk_0] = maxMargin_buf_0_tmp;
            maxMarginIndex_buf_0[kk_0] = i_0;
          }
          margins_0[kk_0][i_0] = margin_buf_0_rn[kk_0][i_0];
        }
      }
/*STAGE_2: */
//double sum = 0.0;
      for (i_0 = 0; i_0 < 10 - 1; i_0++) {
//#pragma ACCEL pipeline
        for (kk_0 = 0; kk_0 < 16; kk_0++) {
          if (maxMargin_buf_0_rn[kk_0] > 0) {
            margins_0[kk_0][i_0] -= maxMargin_buf_0_rn[kk_0];
            margins_0_rn[kk_0][i_0] = margins_0[kk_0][i_0];
            if (i_0 == maxMarginIndex_buf_0[kk_0]) {
              sum_buf_0[kk_0] += exp(-maxMargin_buf_0_rn[kk_0]);
              sum_buf_0_rn_0[kk_0] = sum_buf_0[kk_0];
              sum_buf_0_rn[kk_0] = sum_buf_0[kk_0];
            }
             else {
              sum_buf_0[kk_0] += exp(margins_0[kk_0][i_0]);
              sum_buf_0_rn_0[kk_0] = sum_buf_0[kk_0];
              sum_buf_0_rn[kk_0] = sum_buf_0[kk_0];
            }
          }
           else {
            sum_buf_0[kk_0] += exp(margins_0[kk_0][i_0]);
            sum_buf_0_rn_0[kk_0] = sum_buf_0[kk_0];
            sum_buf_0_rn[kk_0] = sum_buf_0[kk_0];
          }
        }
      }
      for (int kk = 0; kk < 16; kk++) {
        label_1 = label_buf_0[kk];
// update gradient
//#pragma ACCEL pipeline 
        for (int i = 0; i < 10 - 1; i++) {
//FIXME:  
          multiplier_buf_0_tmp = exp(margins_0_rn[kk][i]) / (sum_buf_0_rn[kk] + 1.0);
          multiplier_buf_0[kk][i] = multiplier_buf_0_tmp;
          multiplier_buf_0_rn[kk][i] = multiplier_buf_0_tmp;
          if (label_buf_0[kk] != 0.0 && label_buf_0[kk] == (i + 1)) {
            multiplier_buf_0[kk][i] -= 1.0;
            multiplier_buf_0_rn[kk][i] = multiplier_buf_0[kk][i];
          }
        }
      }
    }
//STAGE_3: 
    for (kk_0 = 0; kk_0 < 16; kk_0++) {
      for (j0_0 = 0; j0_0 < 784; j0_0 += 4) 
// Original: #pragma ACCEL pipeline flatten
{
        
#pragma ACCEL PIPELINE II=1
        for (int jj = 0; jj < 4; jj++) {
          
#pragma ACCEL PARALLEL COMPLETE
          for (int i = 0; i < 10 - 1; i++) {
            
#pragma ACCEL PARALLEL COMPLETE
            j_2 = j0_0 + jj;
            output[i * 784 + (j0_0 + jj)] += multiplier_buf_0_rn[kk_0][i] * feature_buf_0[kk_0][j0_0 + jj];
          }
        }
      }
    }
//STAGE_4: 
//#pragma ACCEL pipeline 
    for (kk_0 = 0; kk_0 < 16; kk_0++) {
      label_2 = label_buf_0[kk_0];
      loss_0 = log(sum_buf_0_rn_0[kk_0] + ((double )1));
      if (label_buf_0[kk_0] > 0.0) {
        loss_0 -= marginY_buf_0[kk_0];
      }
      if (maxMargin_buf_0_rn_0[kk_0] > 0) {
        loss_0 += maxMargin_buf_0_rn_0[kk_0];
      }
//output[weight_size] += loss;
      output_loss_buffer_0 += loss_0;
      output_loss_buffer_0_rn = output_loss_buffer_0;
    }
// compute loss
// FIXME:
// math.logip(sum)
  }
//memset(sum_buf, 0, sizeof(double) * CHUNK_SIZE);
//memset(margin_buf, 0, sizeof(double) * CHUNK_SIZE*L);
//memset(multiplier_buf, 0, sizeof(double) * CHUNK_SIZE*L);
   *output_last_element = output_loss_buffer_0_rn;
//output[weight_size] = output_loss_buffer;
}
