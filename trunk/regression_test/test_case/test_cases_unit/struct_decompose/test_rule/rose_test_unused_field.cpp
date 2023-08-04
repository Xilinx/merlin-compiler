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
typedef struct st2 {
int e;
int *f;
int g[1];}st2;

struct st 
{
  int a;
  int b;
  int *c;
  ::st2 *d;
  ::st2 h;
}
;
static void __merlin_dummy_g_a();
int g_a_h_g[1] = {(4)};
int g_a_h_e = 3;
int g_a_b = 2;
int g_a_a = 1;
#pragma ACCEL kernel

void top(int *a_a,int *a_b,int *a_h_e,int (*a_h_g)[1])
{
  
#pragma ACCEL INTERFACE DEPTH=2,1 VARIABLE=a_h_g max_depth=2,1
  
#pragma ACCEL INTERFACE DEPTH=2 VARIABLE=a_h_e max_depth=2
  
#pragma ACCEL INTERFACE DEPTH=2 VARIABLE=a_b max_depth=2
  
#pragma ACCEL INTERFACE DEPTH=2 VARIABLE=a_a max_depth=2
  int i_3_0_2;
  int i_3_0_1;
  int i_3_0_0;
  int i_3_0;
  int tmp_h_g[1];
  int tmp_h_e;
  int tmp_b;
  int tmp_a;
  tmp_a = a_a[0];
  tmp_b = a_b[0];
  tmp_h_e = a_h_e[0];
  for (i_3_0 = 0; i_3_0 < 1UL; ++i_3_0) {
    tmp_h_g[i_3_0] = a_h_g[0][i_3_0];
  }
  int *ptr_h_e = a_h_e;
  int *ptr_a = a_a;
  a_a[0] = tmp_a;
  a_b[0] = tmp_b;
  a_h_e[0] = tmp_h_e;
  for (i_3_0_0 = 0; i_3_0_0 < 1UL; ++i_3_0_0) {
    a_h_g[0][i_3_0_0] = tmp_h_g[i_3_0_0];
  }
  int tmp2_h_g[1] = {(4)};
  int tmp2_h_e = 3;
  int tmp2_b = 2;
  int tmp2_a = 1;
  a_a[1] = tmp2_a;
  a_b[1] = tmp2_b;
  a_h_e[1] = tmp2_h_e;
  for (i_3_0_1 = 0; i_3_0_1 < 1UL; ++i_3_0_1) {
    a_h_g[1][i_3_0_1] = tmp2_h_g[i_3_0_1];
  }
  a_a[0] = g_a_a;
  a_b[0] = g_a_b;
  a_h_e[0] = g_a_h_e;
  for (i_3_0_2 = 0; i_3_0_2 < 1UL; ++i_3_0_2) {
    a_h_g[0][i_3_0_2] = g_a_h_g[i_3_0_2];
  }
  ( *ptr_a)++;
  ( *ptr_h_e)++;
}
