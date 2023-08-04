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
void two_kernel(int *b)
{
  int b_buf_0_rn[100];
  int b_buf_0[100];
  int __memcpy_i_0;
  int a_0[100];
  int i_0;
  int j_0;
  int i1_0;
  int c_0;
  int __memcpy_i_1_0;
  for (__memcpy_i_0 = 0; __memcpy_i_0 < sizeof(int ) * ((unsigned long )100) / 4; ++__memcpy_i_0) {
    b_buf_0[__memcpy_i_0 + 0] = b[__memcpy_i_0 + 0];
  }
  for (j_0 = 0; j_0 < 10; j_0++) {
    for (i_0 = 0; i_0 < 100; i_0++) {
      a_0[i_0] = i_0;
    }
    for (i1_0 = 0; i1_0 < 100; i1_0++) {
//b[i1] = a[i1] ;
      b_buf_0[i1_0] = a_0[99 - i1_0];
    }
  }
  for (i_0 = 0; i_0 < 100; i_0++) {
    b_buf_0[i_0] += 3;
    b_buf_0_rn[i_0] = b_buf_0[i_0];
  }
  for (__memcpy_i_1_0 = 0; __memcpy_i_1_0 < sizeof(int ) * ((unsigned long )100) / 4; ++__memcpy_i_1_0) {
    b[__memcpy_i_1_0 + 0] = b_buf_0_rn[__memcpy_i_1_0 + 0];
  }
}
