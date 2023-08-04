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
#include "memcpy_512_1d.h"
#include <string.h> 
#include "cmost.h"
#include "merlin_type_define.h"
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_kernel_pragma();
// Original: #pragma ACCEL kernel

void workload(merlin_uint_512 *merlin_feature,merlin_uint_512 *membership,float *clusters)
{
  
#pragma HLS DATA_PACK VARIABLE=membership
  
#pragma HLS DATA_PACK VARIABLE=merlin_feature
  __merlin_access_range(clusters,0,159UL);
  __merlin_access_range(membership,0,819199UL);
  __merlin_access_range(merlin_feature,0,26214399UL);
  
#pragma ACCEL interface max_depth=26214400 depth=26214400 variable=merlin_feature
  
#pragma ACCEL interface variable=membership max_depth=819200 depth=819200
  
#pragma ACCEL interface variable=clusters max_depth=160 depth=160
  
#pragma ACCEL coalescing force=on variable=merlin_feature
  
#pragma ACCEL coalescing variable=membership force=on
  
#pragma ACCEL coalescing variable=clusters force=on
  int i;
  int j;
  int k;
  int index;
  
#pragma omp parallel for num_threads(16) private(i,j,k,index,local_clusters)
/*UPDATE_MEMBER:*/
// Original pramga:  ACCEL TILING FACTOR=2048 
  for (i = 0; i < 400; i++) 
// Original: #pragma ACCEL pipeline
// Original: #pragma ACCEL tiling factor=2048
// Original: #pragma ACCEL PIPELINE
// Original: #pragma ACCEL TILING FACTOR=2048
{
    
#pragma ACCEL COALESCING VARIABLE=membership_buf_0 FORCE=ON
    int membership_buf_0[2048];
    
#pragma ACCEL COALESCING VARIABLE=clusters_buf_0 FORCE=ON
    float clusters_buf_0[160];
    
#pragma ACCEL COALESCING VARIABLE=merlin_feature_buf_0 FORCE=ON
    float merlin_feature_buf_0[65536];
    
#pragma HLS array_partition variable=merlin_feature_buf_0 cyclic factor = 16 dim=1
    memcpy_wide_bus_read_float_512(&merlin_feature_buf_0[0],(merlin_uint_512 *)merlin_feature,(i * 65536 * 4),sizeof(float ) * ((unsigned long )65536),65536LL);
    memcpy((void *)(&clusters_buf_0[0]),(const void *)(&clusters[0]),sizeof(float ) * 160);
    
#pragma ACCEL PIPELINE
    for (int i_sub = 0; i_sub < 2048; ++i_sub) 
// Original: #pragma ACCEL pipeline
// Original: #pragma ACCEL tiling factor=2048
// Original: #pragma ACCEL PIPELINE
// Original: #pragma ACCEL TILING FACTOR=2048
{
      
#pragma ACCEL PIPELINE AUTO
      float min_dist = (float )3.40282347e+38;
/* find the cluster center id with min distance to pt */
/*MIN:*/
      for (j = 0; j < 5; j++) 
// Original: #pragma ACCEL parallel
// Original: #pragma ACCEL PARALLEL COMPLETE
{
        
#pragma ACCEL PARALLEL COMPLETE
        float dist = (float )0.0;
/*DIST:*/
        for (k = 0; k < 32; k++) 
// Original: #pragma ACCEL parallel
// Original: #pragma ACCEL PARALLEL COMPLETE
{
          
#pragma ACCEL PARALLEL COMPLETE
          float diff = merlin_feature_buf_0[i_sub * 32 + k] - clusters_buf_0[j * 32 + k];
          dist += diff * diff;
        }
        if (dist < min_dist) {
          min_dist = dist;
          index = j;
        }
      }
/* assign the membership to object i */
      membership_buf_0[i_sub] = index;
    }
    
#pragma HLS array_partition variable=membership_buf_0 cyclic factor = 16 dim=1
    memcpy_wide_bus_write_int_512((merlin_uint_512 *)membership,&membership_buf_0[0],(4 * (i * 2048)),sizeof(int ) * ((unsigned long )2048),2048LL);
  }
}
