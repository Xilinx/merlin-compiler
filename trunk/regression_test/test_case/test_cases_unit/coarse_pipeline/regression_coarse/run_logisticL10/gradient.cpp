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
#include "memcpy_512.h"
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
void gradient(int n_samples,int label_size,int feature_size,class ap_uint< 512 > *weights,BUS_TYPE *data,class ap_uint< 512 > *output,class ap_uint< 512 > *feature_samples,class ap_uint< 512 > *label_samples)
//double   global_weights[9*784], 
//BUS_TYPE global_data[32*785], 
//double   global_output[9*784+1])
{
  static double output_24_0_buf[9][784];
  static double weights_13_0_buf[9][784];
  static double feature_samples_9_0_buf[16][784];
  static double label_samples_6_0_buf[16];
//FIXME:
// const int L = 10; //label_size;
// const int D = 784; //feature_size;
// const int weight_size = (L-1)*D;
  int num_samples = n_samples;
// ZP: 2015-11-15
//memset(output, 0, sizeof(double)*(weight_size+1));
//double* margins = new double[L-1];
  double margins[16][10 - 1];
  double label_buf[16];
  double feature_buf[16][784];
  double marginY_buf[16];
//-std::numeric_limits<double>::infinity();
  double maxMargin_buf[16];
  int maxMarginIndex_buf[16];
  int upper_bound = (n_samples + 16 - 1) / 16;
//for(int k = 0; k < num_samples; k++ ) {
  n_samples % 16 == 0?((void )0) : __assert_fail("(n_samples % 16) == 0","gradient.cpp",50,__PRETTY_FUNCTION__);
  int j0;
  int kk;
  int i;
  double output_loss_buffer = memcpy_wide_bus_single_read_double(((class ap_uint< 512 > *)output),(((10 - 1) * 784) * sizeof(double )));
  
#pragma HLS array_partition variable=weights_13_0_buf cyclic factor = 8 dim=2
  ::memcpy_wide_bus_read_double_2d(weights_13_0_buf, 0, 0, ((class ap_uint< 512 > *)weights),(0 * sizeof(double )),(sizeof(double ) * ((unsigned long )7056)));
  
#pragma HLS array_partition variable=output_24_0_buf cyclic factor = 8 dim=2
  ::memcpy_wide_bus_read_double_2d(output_24_0_buf,0 , 0, ((class ap_uint< 512 > *)output),(0 * sizeof(double )),(sizeof(double ) * ((unsigned long )7056)));
  for (int k0 = 0; k0 < upper_bound; k0++) 
// Original: #pragma ACCEL pipeline
{
    
#pragma ACCEL PIPELINE II=1
    
#pragma HLS loop_tripcount max=1872
    double sum_buf[16];
    double margin_buf[16][10];
    double multiplier_buf[16][10];
// combine these two nodes because they both access data
{
//memset(sum_buf, 0, sizeof(double) * CHUNK_SIZE);
//memset(margin_buf, 0, sizeof(double) * CHUNK_SIZE*L);
//memset(multiplier_buf, 0, sizeof(double) * CHUNK_SIZE*L);
      int t_size = 16;
      int t;
      for (t = 0; t < t_size; t++) {
        sum_buf[t] = 0;
      }
      for (t = 0; t < t_size; t++) {
        for (int t1 = 0; t1 < 10; t1++) {
          margin_buf[t][t1] = 0;
        }
      }
      for (t = 0; t < t_size; t++) {
        for (int t1 = 0; t1 < 10; t1++) {
          multiplier_buf[t][t1] = 0;
        }
      }
/*INIT: */
      
#pragma HLS array_partition variable=label_samples_6_0_buf cyclic factor = 8 dim=1
      memcpy_wide_bus_read_double(&label_samples_6_0_buf[0],((class ap_uint< 512 > *)label_samples),((0 + k0 * 16) * sizeof(double )),(sizeof(double ) * ((unsigned long )16)));
      for (int kk = 0; kk < 16; kk++) 
// Original: #pragma ACCEL pipeline
{
        
#pragma ACCEL PIPELINE II=1
        int k = k0 * 16 + kk;
//              if (k >= num_samples) break;
        marginY_buf[kk] = 0.0;
//-std::numeric_limits<double>::infinity();
        maxMargin_buf[kk] = (-__builtin_inff());
        maxMarginIndex_buf[kk] = 0;
//label_buf[kk] = data[k*(D+1)];
        label_buf[kk] = label_samples_6_0_buf[kk];
      }
//      }
//
//      {
      
#pragma HLS array_partition variable=feature_samples_9_0_buf cyclic factor = 8 dim=2
      ::memcpy_wide_bus_read_double_2d(feature_samples_9_0_buf, 0, 0, ((class ap_uint< 512 > *)feature_samples),((0 + k0 * 12544) * sizeof(double )),(sizeof(double ) * ((unsigned long )12544)));
      for (int j0 = 0; j0 < 784 / 16; j0 += 1) {
        for (int kk = 0; kk < 16; kk++) 
// Original: #pragma ACCEL pipeline flatten
{
          
#pragma ACCEL PIPELINE II=1
          for (int jj = 0; jj < 16; jj += 1) {
            
#pragma ACCEL PARALLEL COMPLETE
            int k = k0 * 16 + kk;
            int j = j0 * 16 + jj;
            feature_buf[kk][j] = feature_samples_9_0_buf[kk][j0 * 16 + jj];
          }
        }
      }
    }
{
      double margin_buf_reduc[16][4][9];
/*STAGE_1:*/
//for(j0 = 0; j0 < D; j0+=F_UR ) {
      for (kk = 0; kk < 16; kk++) {
        
#pragma ACCEL PIPELINE II=1
        for (int jj = 0; jj < 4; jj += 1) {
          
#pragma ACCEL PARALLEL COMPLETE
          for (int i = 0; i < 10 - 1; i++) {
            
#pragma ACCEL PARALLEL COMPLETE
            margin_buf_reduc[kk][jj][i] = 0;
          }
        }
      }
      for (j0 = 0; j0 < 784 / 4; j0 += 1) {
        for (kk = 0; kk < 16; kk++) 
// Original: #pragma ACCEL pipeline flatten
{
          
#pragma ACCEL PIPELINE II=1
          for (int jj = 0; jj < 4; jj += 1) {
            
#pragma ACCEL PARALLEL COMPLETE
            for (int i = 0; i < 10 - 1; i++) {
              
#pragma ACCEL PARALLEL COMPLETE
              int j = j0 * 4 + jj;
              margin_buf_reduc[kk][jj][i] += weights_13_0_buf[i][j0 * 4 + jj] * feature_buf[kk][j];
            }
          }
        }
      }
      for (kk = 0; kk < 16; kk++) {
        
#pragma ACCEL PIPELINE II=1
        for (int jj = 0; jj < 4; jj += 1) {
          
#pragma ACCEL PARALLEL COMPLETE
          for (int i = 0; i < 10 - 1; i++) {
            
#pragma ACCEL PARALLEL COMPLETE
            margin_buf[0 + 1 * kk][0 + 1 * i] += margin_buf_reduc[kk][jj][i];
          }
        }
      }
    }
// STAGE_2:      
{
     for (int kk = 0; kk < 16; kk++) 
// Original: #pragma ACCEL pipeline
{
        
#pragma ACCEL PIPELINE II=1
       // double label = label_buf[kk];
        int label = (int)label_buf[kk];
				assert(label <= 9);
        marginY_buf[kk] = margin_buf[kk][((int )label) - 1];
      }
      for (i = 0; i < 10 - 1; i++) {
        for (kk = 0; kk < 16; kk++) {
//  #pragma ACCEL pipeline II=2
          if (margin_buf[kk][i] > maxMargin_buf[kk]) {
            maxMargin_buf[kk] = margin_buf[kk][i];
            maxMarginIndex_buf[kk] = i;
          }
          margins[kk][i] = margin_buf[kk][i];
        }
      }
/*STAGE_2: */
//double sum = 0.0;
      for (i = 0; i < 10 - 1; i++) {
        for (kk = 0; kk < 16; kk++) 
// Original: #pragma ACCEL pipeline
{
          
#pragma ACCEL PIPELINE II=1
          if (maxMargin_buf[kk] > 0) {
            margins[kk][i] -= maxMargin_buf[kk];
            if (i == maxMarginIndex_buf[kk]) {
              sum_buf[kk] += exp(-maxMargin_buf[kk]);
            }
             else {
              sum_buf[kk] += exp(margins[kk][i]);
            }
          }
           else {
            sum_buf[kk] += exp(margins[kk][i]);
          }
        }
      }
      for (int kk = 0; kk < 16; kk++) {
        double label = label_buf[kk];
// update gradient
        for (int i = 0; i < 10 - 1; i++) 
// Original: #pragma ACCEL pipeline
{
//FIXME:  
          
#pragma ACCEL PIPELINE II=1
          multiplier_buf[kk][i] = exp(margins[kk][i]) / (sum_buf[kk] + 1.0);
          if (label != 0.0 && label == (i + 1)) {
            multiplier_buf[kk][i] -= 1.0;
          }
        }
      }
    }
//STAGE_3: 
    for (kk = 0; kk < 16; kk++) {
      for (j0 = 0; j0 < 784; j0 += 4) 
// Original: #pragma ACCEL pipeline flatten
{
        
#pragma ACCEL PIPELINE II=1
        for (int jj = 0; jj < 4; jj++) 
// Original: #pragma ACCEL parallel copmlete
{
          
#pragma ACCEL PARALLEL COMPLETE
          for (int i = 0; i < 10 - 1; i++) 
// Original: #pragma ACCEL parallel copmlete
{
            
#pragma ACCEL PARALLEL COMPLETE
            int j = j0 + jj;
            output_24_0_buf[i][j0 + jj] += multiplier_buf[kk][i] * feature_buf[kk][j];
//FIXME:      //output[i*D+j] += multiplier_buf[kk][i]*feature_buf[kk][j];
          }
        }
      }
    }
//STAGE_4: 
    for (kk = 0; kk < 16; kk++) 
// Original: #pragma ACCEL pipeline
{
      
#pragma ACCEL PIPELINE II=1
      double label = label_buf[kk];
// compute loss
// FIXME:
// math.logip(sum)
      double loss = log(sum_buf[kk] + 1);
      if (label > 0.0) {
        loss -= marginY_buf[kk];
      }
      if (maxMargin_buf[kk] > 0) {
        loss += maxMargin_buf[kk];
      }
//output[weight_size] += loss;
      output_loss_buffer += loss;
    }
  }
  
#pragma HLS array_partition variable=output_24_0_buf cyclic factor = 8 dim=2
  ::memcpy_wide_bus_write_double_2d(((class ap_uint< 512 > *)output),output_24_0_buf, 0, 0, (sizeof(double ) * 0),(sizeof(double ) * ((unsigned long )7056)));
  memcpy_wide_bus_single_write_double(((class ap_uint< 512 > *)output),output_loss_buffer,(sizeof(double ) * ((10 - 1) * 784)));
}
