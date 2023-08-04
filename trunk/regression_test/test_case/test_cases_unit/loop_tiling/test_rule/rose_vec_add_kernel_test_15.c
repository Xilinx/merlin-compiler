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
#define VEC_SIZE 10000
#pragma ACCEL kernel

void vec_add_kernel(int *a,int *b,int *c,int inc)
{
  int j;
  int k;
  int n;
  
#pragma ACCEL pipeline
  for (j = 0; j < n / 16L; j++) {
    
#pragma ACCEL PARALLEL 
    
#pragma ACCEL PARTIAL_PARALLEL_TAG
    for (int j_sub = 0; j_sub < 16; ++j_sub) {
      for (k = 0; k < n; k++) {
        
#pragma ACCEL PARALLEL 
        
#pragma ACCEL PARTIAL_PARALLEL_TAG
        for (int k_sub = 0; k_sub < 10; ++k_sub) {
          c[55 * (j * 16 + j_sub) + (k * 10 + k_sub)] = a[55 * (j * 16 + j_sub) + (k * 10 + k_sub)] + b[55 * (j * 16 + j_sub) + (k * 10 + k_sub)] + inc;
        }
      }
    }
  }
{
    for (j = n / 16 * 16; j < n; j++) {
      
#pragma HLS LOOP_TRIPCOUNT max=15
      for (k = 0; k < n; k++) {
        
#pragma ACCEL PARALLEL 
        
#pragma ACCEL PARTIAL_PARALLEL_TAG
        for (int k_sub = 0; k_sub < 10; ++k_sub) {
          c[55 * j + (k * 10 + k_sub)] = a[55 * j + (k * 10 + k_sub)] + b[55 * j + (k * 10 + k_sub)] + inc;
        }
      }
    }
  }
}
