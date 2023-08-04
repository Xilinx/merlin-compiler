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
#include<assert.h>
#define VEC_SIZE 10000
#pragma ACCEL kernel

void vec_add_kernel(int *a,int *b,int *c,int inc,int n,int m)
{
  int j;
  n == 10?((void )0) : __assert_fail("n == 10","vec_add_kernel_test_divisible.c",7,__PRETTY_FUNCTION__);
  for (j = 0; j < n / 2L; j++) {
    
#pragma ACCEL TILED_TAG
    for (int j_sub_1 = 0; j_sub_1 < 2; ++j_sub_1) {
      c[j * 2 + j_sub_1] = a[j * 2 + j_sub_1] + b[j * 2 + j_sub_1] + inc;
    }
  }
  for (j = 0; j < m; j++) {
    
#pragma ACCEL TILED_TAG
    for (int j_sub_0 = 0; j_sub_0 < 3; ++j_sub_0) {
      c[j * 3 + j_sub_0] = a[j * 3 + j_sub_0] + b[j * 3 + j_sub_0] + inc;
    }
  }
  for (j = 0; j < 2 + n; j++) {
    
#pragma ACCEL TILED_TAG
    for (int j_sub = 0; j_sub < 6; ++j_sub) {
      c[j * 6 + j_sub] = a[j * 6 + j_sub] + b[j * 6 + j_sub] + inc;
    }
  }
}
