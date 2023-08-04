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

void vec_add_kernel(int *a,int *b,int *c,int inc,int n)
{
  int k;
  int j;
  for (j = 0; j < 33; j++) {
    
#pragma ACCEL cache
    
#pragma ACCEL cache variable=c
    
#pragma ACCEL TILED_TAG
    for (int j_sub = 0; j_sub < 3; ++j_sub) {
      
#pragma ACCEL pipeline
      for (k = 0; k <= 100; k++) {
        c[100 * (j * 3 + j_sub) + k] = a[100 * (j * 3 + j_sub) + k] + b[100 * (j * 3 + j_sub) + k] + inc;
      }
    }
  }
{
    for (j = 99L; j < 100; j++) {
      
#pragma ACCEL pipeline
      
#pragma ACCEL cache variable=c
      
#pragma ACCEL cache
      for (k = 0; k <= 100; k++) {
        c[100 * j + k] = a[100 * j + k] + b[100 * j + k] + inc;
      }
    }
  }
}
#pragma ACCEL kernel

void vec_add_kernel_2(int *a,int *b,int *c,int inc,int n)
{
  int k;
  int j;
  for (j = 0; j < 10; j++) {
    
#pragma ACCEL cache
    
#pragma ACCEL cache variable=c
    
#pragma ACCEL TILED_TAG
    for (int j_sub = 0; j_sub < 5; ++j_sub) {
      
#pragma ACCEL PARALLEL 
      
#pragma ACCEL PARTIAL_PARALLEL_TAG
      for (int j_sub_sub = 0; j_sub_sub < 2; ++j_sub_sub) {
        
#pragma ACCEL pipeline
        for (k = 0; k <= 100; k++) {
          c[100 * (j * 10 + (j_sub * 2 + j_sub_sub)) + k] = a[100 * (j * 10 + (j_sub * 2 + j_sub_sub)) + k] + b[100 * (j * 10 + (j_sub * 2 + j_sub_sub)) + k] + inc;
        }
      }
    }
  }
}
