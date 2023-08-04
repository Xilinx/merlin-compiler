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
#define SIZE_1 8
#undef SIZE_1
#include <string.h> 
#include "cmost.h"
#include <assert.h>
#include <string.h>

#pragma ACCEL kernel
void kmeans(int num_samples,int num_runs_,int num_clusters_,int vector_length_,double centers[1024 * 10 * 1],double merlin_output[(1024 + 3) * 10 * 1],int data_size,int center_size,int output_size)
{
  double centers_buf_0[1][10][128][8];
  memcpy(centers_buf_0[0][0][0],((const void *)(&centers[0])),sizeof(double ) * 10000);
  int j_sub;
  int k_sub;
#define num_runs 1
#define num_clusters 10
#define vector_length 784
#define data_length (vector_length+1)
  double sums[10240];
  int counts[1 * 10];
  for (int i = 0; i < 1 * 10 * 1024; i++) {
    
#pragma ACCEL PIPELINE
    sums[i] = ((double )0);
  }
  for (int i = 0; i < 1 * 10; i++) {
    
#pragma ACCEL PARALLEL COMPLETE
    counts[i] = 0;
  }
  num_samples > 0 && num_samples <= 10000?((void )0) : __assert_fail("num_samples>0 && num_samples<=10000","kmeans_kernel.cpp",((unsigned int )108),__PRETTY_FUNCTION__);
  1 > 0 && 1 < 2?((void )0) : __assert_fail("1>0 && 1<2","kmeans_kernel.cpp",((unsigned int )109),__PRETTY_FUNCTION__);
  10 > 0 && 10 <= 10?((void )0) : __assert_fail("10>0 && 10<=10","kmeans_kernel.cpp",((unsigned int )110),__PRETTY_FUNCTION__);
  784 > 0 && 784 <= 1024?((void )0) : __assert_fail("784>0 && 784<=1024","kmeans_kernel.cpp",((unsigned int )111),__PRETTY_FUNCTION__);
//vector_length = 784;
// compute sum of centers and counts
  for (int i = 0; i < num_samples; i++) {
    double data_buf_0[98][8];
    
    for (int r = 0; r < 1; r++) {
      int bestCenter;
      bestCenter = 0;
      double bestDistance;
      bestDistance = ((double )(__builtin_inff()));
      double distance[10];
      for (int k = 0; k < 10; k++) {
        
#pragma ACCEL PARALLEL COMPLETE
        distance[k] = ((double )0);
      }
// Original pramga:  ACCEL PARALLEL FACTOR=8 
      for (int j = 0; j < 98; j++) 
// Original: #pragma ACCEL parallel factor=8
// Original: #pragma ACCEL PARALLEL FACTOR=8
{
        
#pragma ACCEL PIPELINE
#pragma ACCEL REDUCTION VARIABLE=distance
        for (j_sub = 0; j_sub < 8; ++j_sub) 
// Original: #pragma ACCEL parallel factor=8
// Original: #pragma ACCEL PARALLEL FACTOR=8
{
          
#pragma ACCEL PARALLEL
          for (int k = 0; k < 10; k++) {
            
#pragma ACCEL PARALLEL COMPLETE
            int offset;
            offset = r * 10 * 1024 + k * 1024;
            double diff;
            diff = centers_buf_0[r][k][j][j_sub] - data_buf_0[j][j_sub];
            distance[k] += diff * diff;
          }
        }
      }
      for (int k = 0; k < 10; k++) {
        
#pragma ACCEL PARALLEL COMPLETE
        if (distance[k] < bestDistance) {
          bestDistance = distance[k];
          bestCenter = k;
        }
      }
// update sums(r)(bestCenter)
// Original pramga:  ACCEL PARALLEL FACTOR=8 
      for (int k = 0; k < 98; k++) 
// Original: #pragma ACCEL parallel factor=8
// Original: #pragma ACCEL PARALLEL FACTOR=8
{
        
#pragma ACCEL PIPELINE
        for (k_sub = 0; k_sub < 8; ++k_sub) 
// Original: #pragma ACCEL parallel factor=8
// Original: #pragma ACCEL PARALLEL FACTOR=8
{
          
#pragma ACCEL PARALLEL
          sums[k * 8 + k_sub] += data_buf_0[k][k_sub];
        }
      }
// update counts(r)(bestCenter)
      counts[r * 10 + bestCenter]++;
    }
  }
}
