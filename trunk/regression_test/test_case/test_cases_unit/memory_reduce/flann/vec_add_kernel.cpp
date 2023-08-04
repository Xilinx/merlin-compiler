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
#pragma channel float __ch_query_z_buf_0[4] __attribute__((depth(32)));
typedef int __merlin_channel_typedef;
#include <string.h> 
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
//#include <ap_int.h>
#include <assert.h>
#include "config.h"
#define query_row_num  9665
#define query_col_num  3
#define idx_row_num  9665
#define idx_col_num  1
#define dist_row_num  9665
#define dist_col_num  1
//#define target_row_num  18213
#define target_row_num  18200
#define target_col_num  3
//#define TS_N 20
//#define TS_M 20
#define TS_Q 4
#define TS_T 4
#define TS_L 8
//#define TS_N 8
//#define TS_M 8
//typedef ap_fixed<32,16,AP_TRN,AP_SAT> in_t;
//typedef double in_t;
#define in_t double
// NOTE : if using ap_fixed classes, please use -O3 in gcc, otherwise it is very slow
// int_t_diff
//<14,2>: four errors (534, 1666, 6692, 7999)
//<16,2>: two errors (135, 1391)
//<18,2>: zero error
//<18,0>: > 10 errors
//typedef ap_fixed<18,2,AP_TRN,AP_SAT> in_t_diff;
//typedef float in_t_diff;
#define in_t_diff float
//
//typedef ap_ufixed<32,4,AP_TRN> mult_t;
//typedef double mult_t;
#define mult_t double
//typedef ap_ufixed<34,6,AP_TRN> sum_t;
//typedef double sum_t;
#define sum_t double
//typedef ap_ufixed<34,6> diff_t;
//typedef double diff_t;
#define diff_t double
__merlin_channel_typedef __ch_query_z_buf_0[4];
#include "channel_def.h"
#pragma channel float __ch_query_y_buf_0_0[4] __attribute__((depth(32)));
typedef int __merlin_channel_typedef;
__merlin_channel_typedef __ch_query_y_buf_0_0[4];
#include "channel_def.h"
#pragma channel float __ch_query_x_buf_0_0[4] __attribute__((depth(32)));
typedef int __merlin_channel_typedef;
__merlin_channel_typedef __ch_query_x_buf_0_0[4];
#include "channel_def.h"
#pragma channel float __ch_target_z_buf_0_0[4] __attribute__((depth(32)));
typedef int __merlin_channel_typedef;
__merlin_channel_typedef __ch_target_z_buf_0_0[4];
#include "channel_def.h"
#pragma channel float __ch_target_y_buf_0_0[4] __attribute__((depth(32)));
typedef int __merlin_channel_typedef;
__merlin_channel_typedef __ch_target_y_buf_0_0[4];
#include "channel_def.h"
#pragma channel float __ch_target_x_buf_0_0[4] __attribute__((depth(32)));
typedef int __merlin_channel_typedef;
__merlin_channel_typedef __ch_target_x_buf_0_0[4];
#include "channel_def.h"
#pragma channel double __ch_local_dist_0[4] __attribute__((depth(32)));
typedef int __merlin_channel_typedef;
__merlin_channel_typedef __ch_local_dist_0[4];
#include "channel_def.h"
#pragma channel int __ch_index_buf_0[4] __attribute__((depth(32)));
typedef int __merlin_channel_typedef;
__merlin_channel_typedef __ch_index_buf_0[4];

float my_diff(float a,float b)
{
  return a - b;
}
#if 0
//	#pragma HLS RESOURCE variable=dist_x core=AddSub_DSP
//	#pragma HLS RESOURCE variable=dist_y core=AddSub_DSP
//	#pragma HLS RESOURCE variable=dist_z core=AddSub_DSP
//	#pragma HLS RESOURCE variable=pair_dist core=AddSub_DSP
#endif
#define pow(x, y) ((x) * (x))

double nn_linear_compute_elem(float x1,float y1,float x2,float y2,float x3,float y3)
{
  return ((x1 - y1) * (x1 - y1) + (x2 - y2) * (x2 - y2) + (x3 - y3) * (x3 - y3));
}

void reduce_func(double &dist_reduce,int &index_reduce,double one_diff,int one_index)
{
//#pragma HLS inline
  if (one_diff < dist_reduce) {
    dist_reduce = one_diff;
    index_reduce = one_index;
  }
}
#pragma ACCEL kernel
void vec_add_kernel(
// inputs
float *target_x,float *target_y,float *target_z,float *query_x,float *query_y,float *query_z,
// outputs
int *index)
{
  int index_buf_0_rf7[2400][4];
  double local_dist_0_rf6[4];
  float target_x_buf_0_0_rf5[568][8][4];
  float target_y_buf_0_0_rf4[568][8][4];
  float target_z_buf_0_0_rf3[568][8][4];
  float query_x_buf_0_0_rf2[2400][4];
  float query_y_buf_0_0_rf1[2400][4];
  float query_z_buf_0_rf0[2400][4];
  int _memcpy_i6_1;
  int _memcpy_i6_0;
  int _memcpy_i5_2;
  int _memcpy_i5_1;
  int _memcpy_i5_0;
  int _memcpy_i4_2;
  int _memcpy_i4_1;
  int _memcpy_i4_0;
  int _memcpy_i3_2;
  int _memcpy_i3_1;
  int _memcpy_i3_0;
  int _memcpy_i2_1;
  int _memcpy_i2_0;
  int _memcpy_i1_1;
  int _memcpy_i1_0;
  int _memcpy_i0_1;
  int _memcpy_i0_0;
  int index_buf_0[2400][4];
  float query_z_buf_0[2400][4];
#define LOOP_BOUND_1 (TOTAL_Q / TS_Q)
#define LOOP_BOUND_2 (TOTAL_T / TS_T)
  float query_y_buf_0_0[2400][4];
  float query_x_buf_0_0[2400][4];
  float target_z_buf_0_0[568][8][4];
  float target_y_buf_0_0[568][8][4];
  float target_x_buf_0_0[568][8][4];
  int i_0;
  int j_0;
  int i0_0;
  int ii_0;
  int j0_0;
  int jj_0;
  int j00_0;
  int jj0_0;
  double local_dist_0[4];
  int local_index_0[4];
  int index_buf_1[9600 / 4][4];
  double pair_dist_0_0;
  double return_0_nn_linear_compute_elem_0;
  float y3__nn_linear_compute_elem_0;
  float x3__nn_linear_compute_elem_0;
  float y2__nn_linear_compute_elem_0;
  float x2__nn_linear_compute_elem_0;
  float y1__nn_linear_compute_elem_0;
  float x1__nn_linear_compute_elem_0;
  int one_index__reduce_func_0;
  double one_diff__reduce_func_0;
  for (_memcpy_i0_1 = 0; _memcpy_i0_1 < 2400; ++_memcpy_i0_1) {
    for (_memcpy_i0_0 = 0; _memcpy_i0_0 < 4; ++_memcpy_i0_0) {
// Parallel pragma comes from SgInitializedName:query_z_buf_0
      
#pragma ACCEL parallel
      write_channel_altera___ch_query_z_buf_0__merlinalterafuncend(__ch_query_z_buf_0[_memcpy_i0_0],query_z[0 + ((0 * 2400 + _memcpy_i0_1) * 4 + _memcpy_i0_0)]);
    }
  }
  for (_memcpy_i1_1 = 0; _memcpy_i1_1 < 2400; ++_memcpy_i1_1) {
    for (_memcpy_i1_0 = 0; _memcpy_i1_0 < 4; ++_memcpy_i1_0) {
// Parallel pragma comes from SgInitializedName:query_y_buf_0_0
      
#pragma ACCEL parallel
      write_channel_altera___ch_query_y_buf_0_0__merlinalterafuncend(__ch_query_y_buf_0_0[_memcpy_i1_0],query_y[0 + ((0 * 2400 + _memcpy_i1_1) * 4 + _memcpy_i1_0)]);
    }
  }
  for (_memcpy_i2_1 = 0; _memcpy_i2_1 < 2400; ++_memcpy_i2_1) {
    for (_memcpy_i2_0 = 0; _memcpy_i2_0 < 4; ++_memcpy_i2_0) {
// Parallel pragma comes from SgInitializedName:query_x_buf_0_0
      
#pragma ACCEL parallel
      write_channel_altera___ch_query_x_buf_0_0__merlinalterafuncend(__ch_query_x_buf_0_0[_memcpy_i2_0],query_x[0 + ((0 * 2400 + _memcpy_i2_1) * 4 + _memcpy_i2_0)]);
    }
  }
  for (_memcpy_i3_2 = 0; _memcpy_i3_2 < 568; ++_memcpy_i3_2) {
    for (_memcpy_i3_1 = 0; _memcpy_i3_1 < 8; ++_memcpy_i3_1) {
      for (_memcpy_i3_0 = 0; _memcpy_i3_0 < 4; ++_memcpy_i3_0) {
// Parallel pragma comes from SgInitializedName:target_z_buf_0_0
        
#pragma ACCEL parallel
        write_channel_altera___ch_target_z_buf_0_0__merlinalterafuncend(__ch_target_z_buf_0_0[_memcpy_i3_0],target_z[0 + (((0 * 568 + _memcpy_i3_2) * 8 + _memcpy_i3_1) * 4 + _memcpy_i3_0)]);
      }
    }
  }
  for (_memcpy_i4_2 = 0; _memcpy_i4_2 < 568; ++_memcpy_i4_2) {
    for (_memcpy_i4_1 = 0; _memcpy_i4_1 < 8; ++_memcpy_i4_1) {
      for (_memcpy_i4_0 = 0; _memcpy_i4_0 < 4; ++_memcpy_i4_0) {
// Parallel pragma comes from SgInitializedName:target_y_buf_0_0
        
#pragma ACCEL parallel
        write_channel_altera___ch_target_y_buf_0_0__merlinalterafuncend(__ch_target_y_buf_0_0[_memcpy_i4_0],target_y[0 + (((0 * 568 + _memcpy_i4_2) * 8 + _memcpy_i4_1) * 4 + _memcpy_i4_0)]);
      }
    }
  }
  for (_memcpy_i5_2 = 0; _memcpy_i5_2 < 568; ++_memcpy_i5_2) {
    for (_memcpy_i5_1 = 0; _memcpy_i5_1 < 8; ++_memcpy_i5_1) {
      for (_memcpy_i5_0 = 0; _memcpy_i5_0 < 4; ++_memcpy_i5_0) {
// Parallel pragma comes from SgInitializedName:target_x_buf_0_0
        
#pragma ACCEL parallel
        write_channel_altera___ch_target_x_buf_0_0__merlinalterafuncend(__ch_target_x_buf_0_0[_memcpy_i5_0],target_x[0 + (((0 * 568 + _memcpy_i5_2) * 8 + _memcpy_i5_1) * 4 + _memcpy_i5_0)]);
      }
    }
  }
  for (i0_0 = 0; i0_0 < 9600 / 4; i0_0++) {
    for (ii_0 = 0; ii_0 < 4; ii_0++) {
// Parallel pragma comes from SgInitializedName:local_dist_0
      
#pragma ACCEL parallel
// a very large number
      write_channel_altera___ch_local_dist_0__merlinalterafuncend(__ch_local_dist_0[ii_0],((double )999999));
      local_index_0[ii_0] = - 1;
    }
//for (j0 = 0; j0 < target_row_num/TS_N; j0++) {
    for (j00_0 = 0; j00_0 < 18176 / 4 / 8; j00_0++) {
      for (jj0_0 = 0; jj0_0 < 8; jj0_0++) 
// Original: #pragma ACCEL pipeline flatten
{
        if (j00_0 == 0 && jj0_0 == 0) {
          for (ii_0 = 0; ii_0 < 4; ii_0++) {
            
#pragma ACCEL parallel
            local_dist_0_rf6[ii_0] = read_channel_altera___ch_local_dist_0__merlinalterafuncend(__ch_local_dist_0[ii_0]);
          }
        }
        if (i0_0 == 0) {
          for (jj_0 = 0; jj_0 < 4; jj_0++) {
            
#pragma ACCEL parallel
            target_x_buf_0_0_rf5[j00_0][jj0_0][jj_0] = read_channel_altera___ch_target_x_buf_0_0__merlinalterafuncend(__ch_target_x_buf_0_0[jj_0]);
          }
        }
        if (i0_0 == 0) {
          for (jj_0 = 0; jj_0 < 4; jj_0++) {
            
#pragma ACCEL parallel
            target_y_buf_0_0_rf4[j00_0][jj0_0][jj_0] = read_channel_altera___ch_target_y_buf_0_0__merlinalterafuncend(__ch_target_y_buf_0_0[jj_0]);
          }
        }
        if (i0_0 == 0) {
          for (jj_0 = 0; jj_0 < 4; jj_0++) {
            
#pragma ACCEL parallel
            target_z_buf_0_0_rf3[j00_0][jj0_0][jj_0] = read_channel_altera___ch_target_z_buf_0_0__merlinalterafuncend(__ch_target_z_buf_0_0[jj_0]);
          }
        }
        if (j00_0 == 0 && jj0_0 == 0) {
          for (ii_0 = 0; ii_0 < 4; ii_0++) {
            
#pragma ACCEL parallel
            query_x_buf_0_0_rf2[i0_0][ii_0] = read_channel_altera___ch_query_x_buf_0_0__merlinalterafuncend(__ch_query_x_buf_0_0[ii_0]);
          }
        }
        if (j00_0 == 0 && jj0_0 == 0) {
          for (ii_0 = 0; ii_0 < 4; ii_0++) {
            
#pragma ACCEL parallel
            query_y_buf_0_0_rf1[i0_0][ii_0] = read_channel_altera___ch_query_y_buf_0_0__merlinalterafuncend(__ch_query_y_buf_0_0[ii_0]);
          }
        }
        if (j00_0 == 0 && jj0_0 == 0) {
          for (ii_0 = 0; ii_0 < 4; ii_0++) {
            
#pragma ACCEL parallel
            query_z_buf_0_rf0[i0_0][ii_0] = read_channel_altera___ch_query_z_buf_0__merlinalterafuncend(__ch_query_z_buf_0[ii_0]);
          }
        }
        
#pragma ACCEL PIPELINE II=1
        j0_0 = j00_0 * 8 + jj0_0;
        for (jj_0 = 0; jj_0 < 4; jj_0++) {
          
#pragma ACCEL PARALLEL COMPLETE
          for (ii_0 = 0; ii_0 < 4; ii_0++) {
            
#pragma ACCEL PARALLEL COMPLETE
            i_0 = i0_0 * 4 + ii_0;
            j_0 = j0_0 * 4 + jj_0;
{
              y3__nn_linear_compute_elem_0 = target_z_buf_0_0_rf3[j00_0][jj0_0][jj_0];
              x3__nn_linear_compute_elem_0 = query_z_buf_0_rf0[i0_0][ii_0];
              y2__nn_linear_compute_elem_0 = target_y_buf_0_0_rf4[j00_0][jj0_0][jj_0];
              x2__nn_linear_compute_elem_0 = query_y_buf_0_0_rf1[i0_0][ii_0];
              y1__nn_linear_compute_elem_0 = target_x_buf_0_0_rf5[j00_0][jj0_0][jj_0];
              x1__nn_linear_compute_elem_0 = query_x_buf_0_0_rf2[i0_0][ii_0];
              return_0_nn_linear_compute_elem_0 = ((double )((x1__nn_linear_compute_elem_0 - y1__nn_linear_compute_elem_0) * (x1__nn_linear_compute_elem_0 - y1__nn_linear_compute_elem_0) + (x2__nn_linear_compute_elem_0 - y2__nn_linear_compute_elem_0) * (x2__nn_linear_compute_elem_0 - y2__nn_linear_compute_elem_0) + (x3__nn_linear_compute_elem_0 - y3__nn_linear_compute_elem_0) * (x3__nn_linear_compute_elem_0 - y3__nn_linear_compute_elem_0)));
            }
            pair_dist_0_0 = return_0_nn_linear_compute_elem_0;
//assert(pair_dist < 999999);
            
#pragma ACCEL reduction
{
              one_index__reduce_func_0 = j_0;
              one_diff__reduce_func_0 = pair_dist_0_0;
//#pragma HLS inline
              if (one_diff__reduce_func_0 < local_dist_0_rf6[ii_0]) {
                local_dist_0_rf6[ii_0] = one_diff__reduce_func_0;
                index_buf_1[i0_0][ii_0] = one_index__reduce_func_0;
              }
            }
            index_buf_0_rf7[i0_0][ii_0] = index_buf_1[i0_0][ii_0];
          }
        }
        if (j00_0 == 567 && jj0_0 == 7) {
          for (ii_0 = 0; ii_0 < 4; ii_0++) {
            
#pragma ACCEL parallel
            write_channel_altera___ch_index_buf_0__merlinalterafuncend(__ch_index_buf_0[ii_0],index_buf_0_rf7[i0_0][ii_0]);
          }
        }
      }
    }
  }
  for (_memcpy_i6_1 = 0; _memcpy_i6_1 < 2400; ++_memcpy_i6_1) {
    for (_memcpy_i6_0 = 0; _memcpy_i6_0 < 4; ++_memcpy_i6_0) {
// Parallel pragma comes from SgInitializedName:index_buf_0
      
#pragma ACCEL parallel
      index[0 + ((0 * 2400 + _memcpy_i6_1) * 4 + _memcpy_i6_0)] = read_channel_altera___ch_index_buf_0__merlinalterafuncend(__ch_index_buf_0[_memcpy_i6_0]);
    }
  }
  return ;
}
#if 0
// inputs
// outputs
#define LOOP_BOUND_1 2   // 40 / 20
#define LOOP_BOUND_2 910
// a very large number
//for (j0 = 0; j0 < target_row_num/TS_N; j0++) {
//reduce_func(local_dist[ii], index[i], pair_dist, j);
#endif
