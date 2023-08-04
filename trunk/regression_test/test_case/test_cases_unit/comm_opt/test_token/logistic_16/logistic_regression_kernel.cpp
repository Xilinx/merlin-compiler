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
#include<string.h>
#include "logistic.h"
//1. split input global_data to global_data and global_data2
//2. input global_weight, global gradient array index sequence changed
//3. directly transfer weight and gradient instead of shift-register
//void recv_data( volatile float *global_data, unsigned int offset, float data[32][784] )
//{
//    memcpy( *data, (void*)(global_data + offset), 784*32*4 );
//}
//
//void recv_data2( volatile float *global_data2, unsigned int offset, float data2[32][16] )
//{
//    memcpy( *data2, (void*)(global_data2 + offset), 16*32*4 );
//}
//void compute1( float data[32][784], float weights[784+1][16], float result1[32][16] )

void compute1(float data[32][784],float *global_weights,float result1[32][16])
{
  int i;
  int j;
  int k;
//for( j = 0; j < 32; j++ )
//{
//    for( k = 0; k < 16; k++ )
//    {
//        result1[j][k] = weights[0][k];
//    }
//}
  float result1_t[32][16];
  for (i = 0; i < 784; i++) {
    for (j = 0; j < 32; j++) {
      for (k = 0; k < 16; k++) {
        if (i == 0) {
          result1_t[j][k] = global_weights[k * 785];
        }
//result1[j][k] += weights[i+1][k] * data[j][i];
        result1_t[j][k] += global_weights[k * 785 + (i + 1)] * data[j][i];
        if (i == 783) {
          result1[j][k] = result1_t[j][k];
        }
      }
    }
  }
}

void compute2(float result1[32][16],float data2[32][16],float result2[32][16])
{
  int j;
  int k;
  float result1_t_0;
  float temp_0;
  for (k = 0; k < 16; k++) {
    for (j = 0; j < 32; j++) {
      result1_t_0 = result1[j][k];
      temp_0 = 1 + result1_t_0 * result1_t_0;
      result2[j][k] = result1_t_0 / temp_0 - (2.f * data2[j][k] - 1.f) / sqrtf(temp_0);
    }
  }
}
//void compute3(float result2[32][16], float data[32][784], float gradient[784+1][16])

void compute3(float result2[32][16],float data[32][784],float gradient[784 + 1][16])
{
  int i;
  int j;
  int k;
  float result2_t[32][16];
  for (j = 0; j < 32; j++) {
    for (i = 0; i < 784 + 1; i++) {
      for (k = 0; k < 16; k++) {
        if (i == 0) {
          result2_t[j][k] = result2[j][k];
        }
        gradient[i][k] += result2_t[j][k] * ((i == 0?1.f : data[j][i - 1]));
      }
    }
  }
}
//void pipeline( int n_samples, float *global_data, float *global_data2, float weights[784+1][16], float gradient[784+1][16] )
//void pipeline( int n_samples, float *global_data, float *global_data2, float * global_weights, float * global_gradient )
//{
//    int i;
//    int n_stage;
//    //n_stage = n_samples / 32;
//    //for(i = 0; i < n_stage; i++ )
//    for(i = 0; i < n_samples / 32; i++ )
//    {
//        float data[32][784];
//        float data2[32][16];
//        float result1[32][16];
//        float result2[32][16];
//
//        //recv_data( global_data, i * 32 * 784, data );
//        //recv_data2( global_data2, i * 32 * 16, data2 );
//        memcpy( *data, (void*)(global_data + i * 32 * 784), 784*32*4 );
//        memcpy( *data2, (void*)(global_data2 + i * 32 * 16), 16*32*4 );
//        //int __i_fcs_2;
//        //for (__i_fcs_2 = 0; __i_fcs_2 < ((size_t )(784 * 32 * 4)) / 4 - 1; ++__i_fcs_2) {
//        //  data[(__i_fcs_2 + 0) / 784][(__i_fcs_2 + 0) % 784] = global_data[__i_fcs_2 + i * 32 * 784];
//        //}
//        //int __i_fcs_3;
//        //for (__i_fcs_3 = 0; __i_fcs_3 < ((size_t )(16 * 32 * 4)) / 4 - 1; ++__i_fcs_3) {
//        //  data2[(__i_fcs_3 + 0) / 16][(__i_fcs_3 + 0) % 16] = global_data2[__i_fcs_3 + i * 32 * 16];
//        //}
//        compute1( data, global_weights, result1 );
//        compute2( result1, data2, result2 );
//        compute3( result2, data, global_gradient );
//    }
//}
#pragma ACCEL kernel
void logistic_regression_kernel(int n_samples,float *global_weights,float *global_data,float *global_data2,float *global_gradient)
{
  float gradient_rn_rf3[784 + 1][16];
  float result2_0_rf2[32][16];
  float result1_0_rf1[32][16];
  float gradient_rf0[784 + 1][16];
  float gradient_rn[784 + 1][16];
  int _memcpy_i3_1;
  int _memcpy_i3_0;
  int _memcpy_i2_1;
  int _memcpy_i2_0;
  int _memcpy_i1_1;
  int _memcpy_i1_0;
  int _memcpy_i0_1;
  int _memcpy_i0_0;
  float global_gradient_buf_0[16][785];
  float weights[784 + 1][16];
  float gradient[784 + 1][16];
  int i;
  int j;
  int k;
  float regs[16];
  float buffer[784 + 1];
  int j_0;
  float data_0[32][784];
  float data2_0[32][16];
  float result1_0[32][16];
  float result2_0[32][16];
  int j_1;
  float global_weights_buf_0_0[16][785];
  int i__compute1_0;
  int j__compute1_0;
  int k__compute1_0;
  float result1_t__compute1_0[32][16];
  int j__compute2_0;
  int k__compute2_0;
  float result1_t_0__compute2_0;
  float temp_0__compute2_0;
  int i__compute3_0;
  int j__compute3_0;
  int k__compute3_0;
  float result2_t__compute3_0[32][16];
  for (i = 0; i < 785; i++) {
    for (j_0 = 0; j_0 < 16; j_0++) {
      gradient[i][j_0] = 0.f;
    }
  }
//for(i=0; i < 16; i++)
//{
//    int j;
//    for(j = 0; j < 784+1; j++) {
//        weights[j][i] = global_weights[i*784+j];
//    }
//}
//pipeline( n_samples, global_data, global_data2, weights, gradient);
//pipeline( n_samples, global_data, global_data2, global_weights, global_gradient);
  for (i = 0; i < n_samples / 32; i++) {
    for (_memcpy_i0_1 = 0; _memcpy_i0_1 < 16; ++_memcpy_i0_1) {
      for (_memcpy_i0_0 = 0; _memcpy_i0_0 < 785; ++_memcpy_i0_0) {
        global_weights_buf_0_0[_memcpy_i0_1][_memcpy_i0_0] = global_weights[0 + ((0 * 16 + _memcpy_i0_1) * 785 + _memcpy_i0_0)];
      }
    }
    for (_memcpy_i1_1 = 0; _memcpy_i1_1 < 32; ++_memcpy_i1_1) {
      for (_memcpy_i1_0 = 0; _memcpy_i1_0 < 784; ++_memcpy_i1_0) {
        data_0[_memcpy_i1_1][_memcpy_i1_0] = global_data[i * 32 * 784 + ((0 * 32 + _memcpy_i1_1) * 784 + _memcpy_i1_0)];
      }
    }
    for (_memcpy_i2_1 = 0; _memcpy_i2_1 < 32; ++_memcpy_i2_1) {
      for (_memcpy_i2_0 = 0; _memcpy_i2_0 < 16; ++_memcpy_i2_0) {
        data2_0[_memcpy_i2_1][_memcpy_i2_0] = global_data2[i * 32 * 16 + ((0 * 32 + _memcpy_i2_1) * 16 + _memcpy_i2_0)];
      }
    }
{
      for (i__compute1_0 = 0; i__compute1_0 < 784; i__compute1_0++) {
        for (j__compute1_0 = 0; j__compute1_0 < 32; j__compute1_0++) {
          for (k__compute1_0 = 0; k__compute1_0 < 16; k__compute1_0++) {
            if (i__compute1_0 == 0) {
              result1_t__compute1_0[j__compute1_0][k__compute1_0] = global_weights_buf_0_0[k__compute1_0][0];
            }
//result1[j][k] += weights[i+1][k] * data[j][i];
            result1_t__compute1_0[j__compute1_0][k__compute1_0] += global_weights_buf_0_0[k__compute1_0][1 + i__compute1_0] * data_0[j__compute1_0][i__compute1_0];
            if (i__compute1_0 == 783) {
              result1_0[j__compute1_0][k__compute1_0] = result1_t__compute1_0[j__compute1_0][k__compute1_0];
            }
          }
        }
      }
    }
{
      for (k__compute2_0 = 0; k__compute2_0 < 16; k__compute2_0++) {
        for (j__compute2_0 = 0; j__compute2_0 < 32; j__compute2_0++) {
          result1_t_0__compute2_0 = result1_0[j__compute2_0][k__compute2_0];
          temp_0__compute2_0 = ((float )1) + result1_t_0__compute2_0 * result1_t_0__compute2_0;
          result2_0[j__compute2_0][k__compute2_0] = result1_t_0__compute2_0 / temp_0__compute2_0 - (2.f * data2_0[j__compute2_0][k__compute2_0] - 1.f) / sqrtf(temp_0__compute2_0);
        }
      }
    }
{
      for (j__compute3_0 = 0; j__compute3_0 < 32; j__compute3_0++) {
        for (i__compute3_0 = 0; i__compute3_0 < 784 + 1; i__compute3_0++) {
          for (k__compute3_0 = 0; k__compute3_0 < 16; k__compute3_0++) {
            if (i == 0 && j__compute3_0 == 0) {
              gradient_rf0[i__compute3_0][k__compute3_0] = gradient[i__compute3_0][k__compute3_0];
            }
            if (i__compute3_0 == 0) {
              result2_t__compute3_0[j__compute3_0][k__compute3_0] = result2_0[j__compute3_0][k__compute3_0];
            }
            gradient_rf0[i__compute3_0][k__compute3_0] += result2_t__compute3_0[j__compute3_0][k__compute3_0] * ((i__compute3_0 == 0?1.f : data_0[j__compute3_0][i__compute3_0 - 1]));
            gradient_rn_rf3[i__compute3_0][k__compute3_0] = gradient_rf0[i__compute3_0][k__compute3_0];
            if (i == - 1 + n_samples / 32 && j__compute3_0 == 31) {
              gradient_rn[i__compute3_0][k__compute3_0] = gradient_rn_rf3[i__compute3_0][k__compute3_0];
            }
          }
        }
      }
    }
  }
  for (i = 0; i < 16; i++) {
    for (j_1 = 0; j_1 < 784 + 1; j_1++) {
      global_gradient_buf_0[i][j_1] = gradient_rn[j_1][i];
    }
  }
  for (_memcpy_i3_1 = 0; _memcpy_i3_1 < 16; ++_memcpy_i3_1) {
    for (_memcpy_i3_0 = 0; _memcpy_i3_0 < 785; ++_memcpy_i3_0) {
      global_gradient[0 + ((0 * 16 + _memcpy_i3_1) * 785 + _memcpy_i3_0)] = global_gradient_buf_0[_memcpy_i3_1][_memcpy_i3_0];
    }
  }
}
