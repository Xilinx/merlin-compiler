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
  int j;
  int i;
  mylabel_outer:
  for (j = 0; j < 2; j++) {
    
#pragma ACCEL PARALLEL 
    
#pragma ACCEL PARTIAL_PARALLEL_TAG
    for (int j_sub = 0; j_sub < 4; ++j_sub) {
      mylabel_inner:
      for (i = 0; i < 1; i++) {
        
#pragma ACCEL PARALLEL 
        
#pragma ACCEL PARTIAL_PARALLEL_TAG
        for (int i_sub = 0; i_sub < 4; ++i_sub) {
          c[j * 4 + j_sub] = a[i * 4 + i_sub] + b[j * 4 + j_sub] + inc;
        }
      }
{
        for (i = 4L; i < 6; i++) {
          c[j * 4 + j_sub] = a[i] + b[j * 4 + j_sub] + inc;
        }
      }
    }
  }
{
    for (j = 8L; j < 10; j++) {
      for (i = 0; i < 1; i++) {
        
#pragma ACCEL PARALLEL 
        
#pragma ACCEL PARTIAL_PARALLEL_TAG
        for (int i_sub = 0; i_sub < 4; ++i_sub) {
          c[j] = a[i * 4 + i_sub] + b[j] + inc;
        }
      }
{
        for (i = 4L; i < 6; i++) {
          c[j] = a[i] + b[j] + inc;
        }
      }
    }
  }
}
