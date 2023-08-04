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
typedef struct {
volatile int a;
int a1;
int a2;
int a3;
int a4;
const int *b;
volatile int *c;}st;

struct st2 
{
  
#pragma ACCEL attribute variable=a depth=10
  int a5;
  int a6;
  int a7;
  volatile st *a;
  const st c;
}
;
#pragma ACCEL kernel

void top(volatile int **c_a_a,volatile int **c_a_a1,volatile int **c_a_a2,volatile int **c_a_a3,volatile int **c_a_a4,volatile int *b_a,int *b_a1,int *b_a2,int *b_a3,int *b_a4)
{
  
#pragma ACCEL INTERFACE DEPTH=20 VARIABLE=b_a4 max_depth=20
  
#pragma ACCEL INTERFACE DEPTH=20 VARIABLE=b_a3 max_depth=20
  
#pragma ACCEL INTERFACE DEPTH=20 VARIABLE=b_a2 max_depth=20
  
#pragma ACCEL INTERFACE DEPTH=20 VARIABLE=b_a1 max_depth=20
  
#pragma ACCEL INTERFACE DEPTH=20 VARIABLE=b_a max_depth=20
  
#pragma ACCEL INTERFACE DEPTH=10,100 VARIABLE=c_a_a4 max_depth=10,100
  
#pragma ACCEL INTERFACE DEPTH=10,100 VARIABLE=c_a_a3 max_depth=10,100
  
#pragma ACCEL INTERFACE DEPTH=10,100 VARIABLE=c_a_a2 max_depth=10,100
  
#pragma ACCEL INTERFACE DEPTH=10,100 VARIABLE=c_a_a1 max_depth=10,100
  
#pragma ACCEL INTERFACE DEPTH=10,100 VARIABLE=c_a_a max_depth=10,100
  int here_a4;
  int here_a3;
  int here_a2;
  int here_a1;
  volatile int here_a;
  int here2_a4;
  int here2_a3;
  int here2_a2;
  int here2_a1;
  volatile int here2_a;
  here_a = ((volatile int *)( *c_a_a))[0];
  here_a1 = ((int *)( *c_a_a1))[0];
  here_a2 = ((int *)( *c_a_a2))[0];
  here_a3 = ((int *)( *c_a_a3))[0];
  here_a4 = ((int *)( *c_a_a4))[0];
  here2_a = ((volatile int *)( *((volatile int **)c_a_a)))[0];
  here2_a1 = ((int *)( *((volatile int **)c_a_a1)))[0];
  here2_a2 = ((int *)( *((volatile int **)c_a_a2)))[0];
  here2_a3 = ((int *)( *((volatile int **)c_a_a3)))[0];
  here2_a4 = ((int *)( *((volatile int **)c_a_a4)))[0];
  b_a[10] = here_a;
  b_a1[10] = here_a1;
  b_a2[10] = here_a2;
  b_a3[10] = here_a3;
  b_a4[10] = here_a4;
  b_a[11] = here2_a;
  b_a1[11] = here2_a1;
  b_a2[11] = here2_a2;
  b_a3[11] = here2_a3;
  b_a4[11] = here2_a4;
  int *ptr_a4;
  int *ptr_a3;
  int *ptr_a2;
  int *ptr_a1;
  volatile int *ptr_a;
  ptr_a = ((volatile int *)( *c_a_a));
  ptr_a1 = ((int *)( *c_a_a1));
  ptr_a2 = ((int *)( *c_a_a2));
  ptr_a3 = ((int *)( *c_a_a3));
  ptr_a4 = ((int *)( *c_a_a4));
  b_a[12] =  *ptr_a;
  b_a1[12] =  *ptr_a1;
  b_a2[12] =  *ptr_a2;
  b_a3[12] =  *ptr_a3;
  b_a4[12] =  *ptr_a4;
  return ;
}
