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
#define VEC_DIM_SIZE 100

#pragma ACCEL kernel
void vec_add_kernel(int *a,int *b,int *c)
{
  int c_1_0_buf[100];
  int b_1_0_buf[100];
  int a_1_0_buf[100];
  int i;
  int j;
  memcpy(&c_1_0_buf[0],&c[0],sizeof(int ) * 100);
  for (i = 0; i < 100; i++) {
    
#pragma ACCEL pipeline
    memcpy(&a_1_0_buf[0],&a[100 * i],sizeof(int ) * 100);
    memcpy(&b_1_0_buf[0],&b[100 * i],sizeof(int ) * 100);
    for (j = 0; j < 100; j++) {
      
#pragma ACCEL parallel complete
      c_1_0_buf[j] += a_1_0_buf[j] + b_1_0_buf[j];
    }
  }
  memcpy(&c[0],&c_1_0_buf[0],sizeof(int ) * 100);
}
