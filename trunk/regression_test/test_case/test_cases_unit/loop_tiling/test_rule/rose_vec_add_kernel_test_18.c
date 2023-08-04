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
  int i;
  int j;
  int k;
  int n;
  for (i = 0; i < 10; i++) {
    
#pragma ACCEL PARALLEL 
    
#pragma ACCEL PARTIAL_PARALLEL_TAG
    for (int i_sub = 0; i_sub < 30; ++i_sub) {
      
#pragma ACCEL tile factor = 20
      for (j = 2 * n; j < 200; j++) {
        for (k = 0; k < 10; k++) {
          
#pragma ACCEL PARALLEL 
          
#pragma ACCEL PARTIAL_PARALLEL_TAG
          for (int k_sub = 0; k_sub < 10; ++k_sub) {
            c[3 * (i * 30 + i_sub) + 2 * j + (k * 10 + k_sub)] = a[3 * (i * 30 + i_sub) + 2 * j + (k * 10 + k_sub)] + b[3 * (i * 30 + i_sub) + 2 * j + (k * 10 + k_sub)] + inc;
          }
        }
{
          for (k = 100L; k < 101; k++) {
            c[3 * (i * 30 + i_sub) + 2 * j + k] = a[3 * (i * 30 + i_sub) + 2 * j + k] + b[3 * (i * 30 + i_sub) + 2 * j + k] + inc;
          }
        }
      }
    }
  }
}
