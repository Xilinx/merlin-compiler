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
#pragma ACCEL kernel
void kmeans_kernel(int N,int K,int D,double points[1000 * 784],double centroids[10 * 784],double merlin_output[(784 + 4) * 10])
{
  double merlin_output_buf_0[10][787];
  double centroids_buf_0[10][10][49][16];
  int i_sub;

#pragma ACCEL interface max_depth=7880 depth=7880 variable=merlin_output

#pragma ACCEL interface variable=centroids max_depth=7840 depth=7840

#pragma ACCEL interface variable=points max_depth=784000 depth=784000
  // Create buffers for sums and counts
  double sums[10 * 784];
  int counts[10];
  double distances[10][10];
  double partials[10][784][10];
  int best_index[10];
  double best_distance[10];
  for (int chunk = 0; chunk < 10; chunk++) {
    for (int k = 0; k < 10; k++) 
    {

#pragma ACCEL parallel
      counts[k] = 0;
    }
    for (int i = 0; i < 980; i++) 
    {

#pragma ACCEL PIPELINE AUTO
      for (i_sub = 0; i_sub < 8; ++i_sub) 
      {

#pragma ACCEL PARALLEL
        sums[i * 8 + i_sub] = 0.0;
      }
    }
  }
  {
    for (int sub = 0; sub < 10; sub++) {

#pragma HLS UNROLL
      memcpy(centroids_buf_0[sub][0][0],((const void *)(&centroids[0])),sizeof(double ) * 7840);
    }
  }
  // Clear sums and counts
  for (int chunk = 0; chunk < 1000 / 10; chunk++) 
  {
    double points_buf_1[10][784];
    double points_buf_0[10][49][16];
    {
      memcpy(points_buf_0[0][0],((const void *)(&points[chunk * 7840])),sizeof(double ) * 7840);
    }

#pragma ACCEL PIPELINE
    for (int sub = 0; sub < 10; sub++) 
    {

#pragma ACCEL FALSE_DEPENDENCE VARIABLE=centroids_buf_0

#pragma ACCEL FALSE_DEPENDENCE VARIABLE=centroids

#pragma ACCEL FALSE_DEPENDENCE VARIABLE=points_buf_0

#pragma ACCEL FALSE_DEPENDENCE VARIABLE=points

#pragma ACCEL PARALLEL COMPLETE
      best_index[sub] = 0;
      best_distance[sub] = 9999.00;
      int n = chunk * 10 + sub;
      for (int k = 0; k < 10; k++) 
      {

#pragma ACCEL PARALLEL COMPLETE
        distances[sub][k] = 0.0;
      }
      for (int d = 0; d < 784 / 16; d++) 
      {

#pragma ACCEL PIPELINE
        for (int k = 0; k < 10; k++) 
        {

#pragma ACCEL PIPELINE AUTO

#pragma ACCEL TILE
          double partials_d_k = 0.0;
          for (int dd = 0; dd < 16; dd++) {

#pragma ACCEL PARALLEL COMPLETE
            double diff = centroids_buf_0[sub][k][d][dd] - points_buf_0[sub][d][dd];
            double diff2 = diff * diff;

#pragma ACCEL reduction variable=partials_d_k
            partials_d_k += diff2;
          }
          partials[sub][d][k] = partials_d_k;
        }
      }
      for (int d = 0; d < 784 / 16; d++) 
      {

#pragma ACCEL FALSE_DEPENDENCE VARIABLE=partials

#pragma ACCEL PIPELINE
        for (int k = 0; k < 10; k++) 
        {

#pragma ACCEL PARALLEL COMPLETE

#pragma ACCEL TILE

#pragma ACCEL reduction variable=distances
          distances[sub][k] += partials[sub][d][k];
        }
      }
      // find best k
      for (int k = 0; k < 10; k++) 
      {

#pragma ACCEL PIPELINE
        if (distances[sub][k] < best_distance[sub]) {
          best_distance[sub] = distances[sub][k];
          best_index[sub] = k;
        }
      }
      //// Update sums and counts
      //counts[best_index]++;
      //for (int d = 0; d < D_STATIC; d++) {
      //    sums[best_index*D_STATIC+d] += points[n*D_STATIC+d];
      //}
    }
    memcpy(points_buf_1[0],((const void *)(&points[chunk * 7840])),sizeof(double ) * 7840);
    for (int sub = 0; sub < 10; sub++) {
      int n = chunk * 10 + sub;
      int best = best_index[sub];
      counts[best]++;
      for (int d = 0; d < 784; d++) 
      {

#pragma ACCEL PIPELINE AUTO

#pragma ACCEL TILE

#pragma ACCEL FALSE_DEPENDENCE VARIABLE=sums
        sums[best * 784 + d] += points_buf_1[sub][d];
      }
    }
  }
  // For every point, assign a label (centroid index) which is the closest
  // pack output data
  for (int k = 0; k < 10; k++) {
    int offset = k * (2 + 784 + 1);
    merlin_output_buf_0[k][0] = ((double )0);
    merlin_output_buf_0[k][1] = ((double )k);
    for (int d = 0; d < 784; d++) {

#pragma ACCEL PIPELINE AUTO
      merlin_output_buf_0[k][2 + d] = sums[k * 784 + d];
    }
    merlin_output_buf_0[k][786] = ((double )counts[k]);
  }
  memcpy(((void *)(&merlin_output[0])),merlin_output_buf_0[0],sizeof(double ) * 7870);
}
