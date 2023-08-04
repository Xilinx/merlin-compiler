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
#define TILE_SIZE 10
#pragma ACCEL kernel

void vec_add_kernel(int a[10000],int b[10000],int c[10000],int inc)
{
  int i;
  int j;
  int k;
  int p;
  for (i = 0; i < 10000 / 10; i++) {
    
#pragma ACCEL TILE FACTOR=auto{options:T0=[x for x in [1,2,4,8,16,32,64,128,256,512,1000] ]; default:1}
// Test if ignored due to innermost loop
    for (j = 0; j < 10; j++) {
      int idx = i * 10 + j;
      c[idx] = a[idx] + b[idx] + inc;
    }
  }
// Test imperfect loop
// Test shadowed by pipeline flatten
  for (i = 0; i < 10000 / 10; i++) {
    
#pragma ACCEL PIPELINE auto{options: I1=["off","","flatten"]}
    
#pragma ACCEL TILE FACTOR=auto{options:T1=[x for x in [1,2,4,8,16,32,64,128,256,512,1000] if x==1 or T1_0==1 and T1_1==1]; default:1}
    for (j = 0; j < 10; j++) {
      
#pragma ACCEL TILE FACTOR=auto{options:T1_0=[x for x in [1,2,4,8,10] if x==1 or T1==1 and T1_1==1 and I1!="flatten"]; default:1}
      for (k = 0; k < 100; ++k) {
        int idx = i * 10 + j;
        c[idx] += a[idx] + b[idx] + inc;
      }
    }
    for (p = 0; p < 10; ++p) {
      
#pragma ACCEL TILE FACTOR=auto{options:T1_1=[x for x in [1,2,4,8,10] if x==1 or T1==1 and T1_0==1 and I1!="flatten"]; default:1}
      for (k = 0; k > 100; ++k) {
        c[p] += a[p + k];
      }
    }
  }
// Test if ignored due to conflict
  for (i = 0; i < 10000 / 10; i++) {
    
#pragma ACCEL tiling factor=4
    for (j = 0; j < 10; j++) {
      int idx = i * 10 + j;
      c[idx] = a[idx] + b[idx] + inc;
    }
  }
// Test if ignored due to unbound loop
  for (j = 0; j < inc; j++) {
    c[j] += j;
  }
// Test if fully unrolled when tripcount < 10
  for (j = 0; j < 3; j++) {
    c[j] += a[j];
  }
}
