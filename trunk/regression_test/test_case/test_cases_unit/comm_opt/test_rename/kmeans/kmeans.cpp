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
//#include <string.h>
//#include <cmath>
#include <float.h>
//#include <algorithm>
//#include <sstream>
static void __merlin_dummy_my_axpy();
static void __merlin_dummy_my_dot();
static void __merlin_dummy_my_dist();
static void __merlin_dummy_fastSquareDistance();
#define MAX_NUM_CLUSTERS 10
#define MAX_NUM_RUNS 1
#define MAX_VECTOR_LENGTH 1024
#define MAX_NUM_SAMPLES 10000
static void __merlin_dummy_4();
static void __merlin_dummy_kernel_pragma();
// Original: #pragma ACCEL kernel name="kmeans"

#pragma ACCEL kernel
void kmeans(double *data_0,double *centers,double sums[784 * 10],int counts[10])
{
  int _memcpy_i2_1;
  int _memcpy_i2_0;
  int _memcpy_i1_1;
  int _memcpy_i1_0;
  int _memcpy_i0_2;
  int _memcpy_i0_1;
  int _memcpy_i0_0;
  double centers_buf_0[10][98][8];
  for (_memcpy_i0_1 = 0; _memcpy_i0_1 < 98UL; ++_memcpy_i0_1) {
    for (_memcpy_i0_2 = 0; _memcpy_i0_2 < 10UL; ++_memcpy_i0_2) {
      for (_memcpy_i0_0 = 0; _memcpy_i0_0 < 8UL; ++_memcpy_i0_0) {
        centers_buf_0[_memcpy_i0_2][_memcpy_i0_1][_memcpy_i0_0] = centers[0 + (((0 * 10UL + _memcpy_i0_2) * 98UL + _memcpy_i0_1) * 8UL + _memcpy_i0_0)];
      }
    }
  }
  double sums_buf_0[784][10];
  
#pragma ACCEL interface variable=counts depth=10 max_depth=10
  
#pragma ACCEL interface variable=sums depth=7840 max_depth=7840
  
#pragma ACCEL interface variable=centers depth=10250 max_depth=10250
  
#pragma ACCEL interface variable=data_0 depth=10240000 max_depth=10240000
  int k_sub_0;
  int k_sub;
  int num_samples;
  for (int k = 0; ((long long )k) < 784LL; k++) {
    for (k_sub = 0; k_sub < 10; ++k_sub) 
// Original: #pragma ACCEL parallel
{
      
#pragma ACCEL PARALLEL COMPLETE
      sums_buf_0[k][k_sub] = 0.0;
    }
  }
  for (_memcpy_i1_1 = 0; _memcpy_i1_1 < 784UL; ++_memcpy_i1_1) {
    for (_memcpy_i1_0 = 0; _memcpy_i1_0 < 10UL; ++_memcpy_i1_0) {
      sums[0 + ((0 * 784UL + _memcpy_i1_1) * 10UL + _memcpy_i1_0)] = sums_buf_0[_memcpy_i1_1][_memcpy_i1_0];
    }
  }
  for (int k = 0; k < 10; k++) 
// Original: #pragma ACCEL parallel
{
    
#pragma ACCEL PARALLEL COMPLETE
    counts[k] = 0;
  }
  for (int i = 0; i < 1024; i++) {
    double data_0_buf_0[98][8];
    for (_memcpy_i2_1 = 0; _memcpy_i2_1 < 98UL; ++_memcpy_i2_1) {
      for (_memcpy_i2_0 = 0; _memcpy_i2_0 < 8UL; ++_memcpy_i2_0) {
        data_0_buf_0[_memcpy_i2_1][_memcpy_i2_0] = data_0[(i * 784) + ((0 * 98UL + _memcpy_i2_1) * 8UL + _memcpy_i2_0)];
      }
    }
    int bestCenter_arr[1];
    int bestCenter;
    double bestDistance;
    double distance[10];
    for (int j0 = 0; j0 < 784 / 8; j0++) 
// Original: #pragma ACCEL parallel flatten exclusive
{
      for (int k = 0; k < 10; k++) {
        
#pragma ACCEL PARALLEL COMPLETE
        for (int jj = 0; jj < 8; jj++) {
          
#pragma ACCEL PARALLEL COMPLETE
          int j;
          j = j0 * 8 + jj;
          if (j == 0) {
            distance[k] = ((double )0);
          }
          double dist;
          dist = centers_buf_0[k][j0][jj] - data_0_buf_0[j0][jj];
          distance[k] += dist * dist;
        }
      }
    }
    for (int k = 0; k < 10; k++) 
// Original: #pragma ACCEL parallel
{
      
#pragma ACCEL PARALLEL COMPLETE
      if (k == 0) {
        bestDistance = ((double )1.79769313486231570815e+308L);
      }
// to fix the problem that range mismatch
      if (distance[k] < bestDistance) {
        bestDistance = distance[k];
        bestCenter_arr[0] = k;
      }
    }
    counts[bestCenter_arr[0]]++;
    for (int k = 0; ((long long )k) < 98LL; k++) {
      for (k_sub_0 = 0; k_sub_0 < 8; ++k_sub_0) 
// Original: #pragma ACCEL parallel
{
        
#pragma ACCEL PARALLEL COMPLETE
        sums[bestCenter_arr[0] * 784 + (k * 8 + k_sub_0)] += data_0_buf_0[k][k_sub_0];
      }
    }
//counts[bestCenter] ++;
  }
}
