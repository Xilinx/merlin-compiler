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
#include "kmeans.h"
#include <string.h>
#include <stdio.h>
#define NFEATURES 32
#define NPOINTS 819200
#define NCLUSTERS 5
static void __merlin_dummy_kernel_pragma();
/* [npoints][nfeatures] */

void workload(float *feature,int *membership,
/* [n_clusters][n_features] */
float *clusters)
{
  
#pragma ACCEL interface variable=feature depth=26214400
  
#pragma ACCEL interface variable=membership depth=819200
  
#pragma ACCEL interface variable=clusters depth=160
  
#pragma ACCEL coalescing variable=feature force=on
  
#pragma ACCEL coalescing variable=membership force=on
  
#pragma ACCEL coalescing variable=clusters force=on
  int i;
  int j;
  int k;
  int index;
  float local_clusters[5 * 32];
  
#pragma ACCEL tiling factor=1024
  
#pragma omp parallel for num_threads(16) private(i,j,k,index,local_clusters)
/*UPDATE_MEMBER:*/
  for (i = 0; i < 819200; i++) {
    float min_dist = 3.40282347e+38;
/* find the cluster center id with min distance to pt */
/*MIN:*/
    for (j = 0; j < 5; j++) {
      float dist = 0.0;
/*DIST:*/
      for (k = 0; k < 32; k++) {
        float diff = feature[32 * i + k] - clusters[32 * j + k];
        dist += diff * diff;
      }
      if (dist < min_dist) {
        min_dist = dist;
        index = j;
      }
    }
/* assign the membership to object i */
    membership[i] = index;
  }
}
