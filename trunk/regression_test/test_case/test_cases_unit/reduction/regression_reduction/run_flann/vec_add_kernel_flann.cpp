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
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <ap_int.h>
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
#define TS_Q 32
#define TS_T 16
#define TS_L 8
//#define TS_N 8
//#define TS_M 8
typedef class ap_fixed< 32 , 16 , AP_TRN , AP_SAT , 0 > in_t;
//typedef double in_t;
// NOTE : if using ap_fixed classes, please use -O3 in gcc, otherwise it is very slow
// int_t_diff
//<14,2>: four errors (534, 1666, 6692, 7999)
//<16,2>: two errors (135, 1391)
//<18,2>: zero error
//<18,0>: > 10 errors
typedef class ap_fixed< 18 , 2 , AP_TRN , AP_SAT , 0 > in_t_diff;
//typedef float in_t_diff;
//
typedef class ap_ufixed< 32 , 4 , AP_TRN , AP_WRAP , 0 > mult_t;
//typedef double mult_t;
typedef class ap_ufixed< 34 , 6 , AP_TRN , AP_WRAP , 0 > sum_t;
//typedef double sum_t;
typedef class ap_ufixed< 34 , 6 , AP_TRN , AP_WRAP , 0 > diff_t;
//typedef double diff_t;

float my_diff(float a,float b)
{
  return a - b;
}

diff_t nn_linear_compute_elem_new(int x1,int y1,int x2,int y2,int x3,int y3)
{
  in_t target_x_fixed;
  in_t target_y_fixed;
  in_t target_z_fixed;
  in_t query_x_fixed;
  in_t query_y_fixed;
  in_t query_z_fixed;
  target_x_fixed . range(31,0) = y1;
  target_y_fixed . range(31,0) = y2;
  target_z_fixed . range(31,0) = y3;
  query_x_fixed . range(31,0) = x1;
  query_y_fixed . range(31,0) = x2;
  query_z_fixed . range(31,0) = x3;
  in_t_diff dist_x(target_x_fixed - query_x_fixed);
  in_t_diff dist_y(target_y_fixed - query_y_fixed);
  in_t_diff dist_z(target_z_fixed - query_z_fixed);
//	#pragma HLS RESOURCE variable=dist_x core=AddSub_DSP
//	#pragma HLS RESOURCE variable=dist_y core=AddSub_DSP
//	#pragma HLS RESOURCE variable=dist_z core=AddSub_DSP
  mult_t pair_dist_x(dist_x * dist_x);
  mult_t pair_dist_y(dist_y * dist_y);
  mult_t pair_dist_z(dist_z * dist_z);
  sum_t pair_dist((pair_dist_x + pair_dist_y) + pair_dist_z);
//	#pragma HLS RESOURCE variable=pair_dist core=AddSub_DSP
  diff_t dist = pair_dist;
  return dist;
}

diff_t nn_linear_compute_elem(float x1,float y1,float x2,float y2,float x3,float y3)
{
  return (pow((x1 - y1),2) + pow((x2 - y2),2) + pow((x3 - y3),2));
}

void reduce_func(diff_t &dist_reduce,int &index_reduce,diff_t one_diff,int one_index)
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
ap_uint<512> *target_x,ap_uint<512> *target_y,ap_uint<512> *target_z,ap_uint<512> *query_x,ap_uint<512> *query_y,ap_uint<512> *query_z,
// outputs
ap_uint<512> *index)
{
  static int target_z_5_0_buf[142][8][16];
  static int target_y_5_0_buf[142][8][16];
  static int target_x_5_0_buf[142][8][16];
  static int query_z_5_0_buf[300][32];
  static int query_y_5_0_buf[300][32];
  static int query_x_5_0_buf[300][32];
  static int index_5_0_buf[300][32];
  int i;
  int j;
  int i0;
  int ii;
  int j0;
  int jj;
  int j00;
  int jj0;
  diff_t local_dist[32];
  int local_index[32];
{
    
#pragma  HLS array_partition  variable=index_5_0_buf cyclic factor = 16 dim=2
    memcpy_wide_bus_read_int_2d(index_5_0_buf,0, 0, ((ap_uint<512> *)index),0 * sizeof(int ),sizeof(int ) * ((unsigned long )9600));
  }
{
    
#pragma  HLS array_partition  variable=query_x_5_0_buf cyclic factor = 16 dim=2
    memcpy_wide_bus_read_int_2d(query_x_5_0_buf,0, 0, ((ap_uint<512> *)query_x),0 * sizeof(int ),sizeof(int ) * ((unsigned long )9600));
  }
{
    
#pragma  HLS array_partition  variable=query_y_5_0_buf cyclic factor = 16 dim=2
    memcpy_wide_bus_read_int_2d(query_y_5_0_buf,0, 0, ((ap_uint<512> *)query_y),0 * sizeof(int ),sizeof(int ) * ((unsigned long )9600));
  }
{
    
#pragma  HLS array_partition  variable=query_z_5_0_buf cyclic factor = 16 dim=2
    memcpy_wide_bus_read_int_2d(query_z_5_0_buf, 0, 0, ((ap_uint<512> *)query_z),0 * sizeof(int ),sizeof(int ) * ((unsigned long )9600));
  }
{
    
#pragma  HLS array_partition  variable=target_x_5_0_buf cyclic factor = 16 dim=3
    memcpy_wide_bus_read_int_3d(target_x_5_0_buf,0, 0, 0, ((ap_uint<512> *)target_x),0 * sizeof(int ),sizeof(int ) * ((unsigned long )18176));
  }
{
    
#pragma  HLS array_partition  variable=target_y_5_0_buf cyclic factor = 16 dim=3
    memcpy_wide_bus_read_int_3d(target_y_5_0_buf,0, 0, 0, ((ap_uint<512> *)target_y),0 * sizeof(int ),sizeof(int ) * ((unsigned long )18176));
  }
{
    
#pragma  HLS array_partition  variable=target_z_5_0_buf cyclic factor = 16 dim=3
    memcpy_wide_bus_read_int_3d(target_z_5_0_buf,0, 0, 0, ((ap_uint<512> *)target_z),0 * sizeof(int ),sizeof(int ) * ((unsigned long )18176));
  }
#define LOOP_BOUND_1 (TOTAL_Q / TS_Q)
#define LOOP_BOUND_2 (TOTAL_T / TS_T)
  for (i0 = 0; i0 < 9600 / 32; i0++) {
    for (ii = 0; ii < 32; ii++) {
// a very large number
      local_dist[ii] = 999999;
      local_index[ii] = - 1;
    }
//for (j0 = 0; j0 < target_row_num/TS_N; j0++) {
    for (j00 = 0; j00 < 18176 / 16 / 8; j00++) {
      for (jj0 = 0; jj0 < 8; jj0++) 
// Original: #pragma ACCEL pipeline flatten
{
        
#pragma ACCEL PIPELINE II=1
        j0 = j00 * 8 + jj0;
        for (jj = 0; jj < 16; jj++) {
          
#pragma ACCEL PARALLEL COMPLETE
          for (ii = 0; ii < 32; ii++) {
            
#pragma ACCEL PARALLEL COMPLETE
            i = i0 * 32 + ii;
            j = j0 * 16 + jj;
            diff_t pair_dist = nn_linear_compute_elem_new(query_x_5_0_buf[i0][ii],target_x_5_0_buf[j00][jj0][jj],query_y_5_0_buf[i0][ii],target_y_5_0_buf[j00][jj0][jj],query_z_5_0_buf[i0][ii],target_z_5_0_buf[j00][jj0][jj]);
            pair_dist < 999999?((void )0) : __assert_fail("pair_dist < 999999","vec_add_kernel.cpp",167,__PRETTY_FUNCTION__);
            
#pragma ACCEL reduction
            reduce_func(local_dist[ii],index_5_0_buf[i0][ii],pair_dist,j);
          }
        }
      }
    }
  }
{
    
#pragma  HLS array_partition  variable=index_5_0_buf cyclic factor = 16 dim=2
    memcpy_wide_bus_write_int_2d(((ap_uint<512> *)index),index_5_0_buf, 0, 0, sizeof(int ) * 0,sizeof(int ) * ((unsigned long )9600));
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
