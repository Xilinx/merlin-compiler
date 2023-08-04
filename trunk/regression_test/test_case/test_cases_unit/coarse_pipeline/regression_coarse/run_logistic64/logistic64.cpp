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
#include <assert.h>
#include "logistic.h"

#pragma ACCEL kernel
void logistic_regression_kernel(int n_samples,float *global_weights,float *global_data,float *global_gradient)
{
  int p;
  int n_stage = n_samples / 32;
  n_stage < 1875?((void )0) : __assert_fail("n_stage < 1875","logistic_regression_kernel.cpp",10,__PRETTY_FUNCTION__);
  float global_weights_buf_0[64][785];
  memcpy((&global_weights_buf_0[0][0]),(&global_weights[0]),sizeof(float ) * 50240);
  float global_gradient_buf_0[64][785];
  memcpy((&global_gradient_buf_0[0][0]),(&global_gradient[0]),sizeof(float ) * 50240);
  for (p = 0; p < n_stage; p++) 
// Original: #pragma ACCEL pipeline
{
#pragma ACCEL PIPELINE II=1
    float global_data_buf_0[32][848];
    memcpy((&global_data_buf_0[0][0]),(&global_data[p * 27136]),sizeof(float ) * 27136);
     
		int i;
    int j;
    int k;
//#pragma HLS loop_tripcount max=1875
    float data[32][784 + 64];
    float result1[32][64];
    float result2[32][64];
    int data_chunk_size = 32 * (784 + 64);
    int sample_size = 784 + 64;
    

		for (i = 0; i < 784; i++) {
      for (j = 0; j < 32; j++) 
// Original: #pragma ACCEL pipeline flatten
{
        
#pragma ACCEL PIPELINE II=1
        for (k = 0; k < 64; k++) {
          
#pragma ACCEL PARALLEL COMPLETE
          if (i == 0) {
            result1[j][k] = global_weights_buf_0[k][0];
          }
          result1[j][k] += global_weights_buf_0[k][1 + i] * global_data_buf_0[j][64 + i];
        }
      }
    }
    for (k = 0; k < 64; k++) {
      for (j = 0; j < 32; j++) 
// Original: #pragma ACCEL pipeline
{
        
#pragma ACCEL PIPELINE II=1
        float temp = 1 + result1[j][k] * result1[j][k];
        result2[j][k] = result1[j][k] / temp - (2.f * global_data_buf_0[j][k] - 1.f) / sqrtf(temp);
      }
    }
    for (j = 0; j < 32; j++) {
      for (i = 0; i < 784 + 1; i++) 
// Original: #pragma ACCEL pipeline flatten
{
        
#pragma ACCEL PIPELINE II=1
        for (k = 0; k < 64; k++) {
          
#pragma ACCEL PARALLEL COMPLETE
          global_gradient_buf_0[k][i] += result2[j][k] * ((i == 0?1.f : global_data_buf_0[j][63 + i]));
        }
      }
    }
  }
  memcpy((&global_gradient[0]),(&global_gradient_buf_0[0][0]),sizeof(float ) * 50240);
}
