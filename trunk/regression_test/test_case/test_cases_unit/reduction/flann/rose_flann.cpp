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
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
//#include <ap_int.h>
#include <assert.h>
#include "config.h"
#define diff_t float

float my_diff(float a,float b)
{
  return a - b;
}
#define pow(x, y) ((x) * (x))

float nn_linear_compute_elem(float x1,float y1,float x2,float y2,float x3,float y3)
{
  return (x1 - y1) * (x1 - y1) + (x2 - y2) * (x2 - y2) + (x3 - y3) * (x3 - y3);
}
#define TS_T 8
#define TS_Q 8

void vec_add_kernel(
// inputs
float *target_x,float *target_y,float *target_z,float *query_x,float *query_y,float *query_z,
// outputs
int index[9600])
{
  int i;
  int j;
  int j0;
  int jj;
  float local_dist[9600];
  for (int i0 = 0; i0 < 9600 / 4; i0++) {
    
#pragma ACCEL parallel
    for (int ii = 0; ii < 4; ii++) {
      int i = i0 * 4 + ii;
// a very large number
      local_dist[i] = 999999;
    }
  }
  for (int i0 = 0; i0 < 9600 / 4; i0++) {
    for (int j0 = 0; j0 < 18176 / 4; j0++) {
      int index_rdc[9600];
      float local_dist_rdc[9600];
      
#pragma ACCEL PARALLEL
      for (int ii = 0; ii < 4; ii++) {
        local_dist_rdc[i0 * 4 + ii] = 999999;
      }
      
#pragma ACCEL parallel
      for (int ii = 0; ii < 4; ii++) {
        
#pragma ACCEL parallel
        for (int jj = 0; jj < 4; jj++) {
          int i = i0 * 4 + ii;
          int j = j0 * 4 + jj;
          float pair_dist = nn_linear_compute_elem(query_x[i],target_x[j],query_y[i],target_y[j],query_z[i],target_z[j]);
          
#pragma ACCEL reduction
          if (pair_dist < local_dist_rdc[i]) {
            local_dist_rdc[i0 * 4 + ii] = pair_dist;
            index_rdc[i0 * 4 + ii] = j;
          }
        }
      }
      
#pragma ACCEL PARALLEL
      for (int ii = 0; ii < 4; ii++) {
        if (local_dist_rdc[i0 * 4 + ii] < local_dist[i]) {
          local_dist[i0 * 4 + ii] = local_dist_rdc[i0 * 4 + ii];
          index[i0 * 4 + ii] = index_rdc[i0 * 4 + ii];
        }
      }
    }
  }
  return ;
}
