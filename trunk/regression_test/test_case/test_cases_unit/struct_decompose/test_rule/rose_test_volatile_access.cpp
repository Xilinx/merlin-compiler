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
#pragma ACCEL attribute variable=b depth=10
#pragma ACCEL attribute variable=c depth=10
const int *b;
volatile int *c;}st;

struct st2 
{
  
#pragma ACCEL attribute variable=a depth=10
  volatile st *a;
  const st c;
}
;
#pragma ACCEL kernel

void top(volatile int *const a_a,volatile const int **const a_b,volatile int **const a_c,volatile int *const b_a,const int **const b_b,volatile int **const b_c,volatile int *c_a_a,volatile const int **c_a_b,volatile int **c_a_c,const volatile int c_c_a,volatile int *d_a_a)
{
  
#pragma ACCEL INTERFACE DEPTH=100,10 VARIABLE=c_a_c max_depth=10,10
  
#pragma ACCEL INTERFACE DEPTH=100,10 VARIABLE=c_a_b max_depth=10,10
  
#pragma ACCEL INTERFACE DEPTH=100 VARIABLE=c_a_a max_depth=10
  
#pragma ACCEL INTERFACE DEPTH=10 VARIABLE=b_c max_depth=100,10
  
#pragma ACCEL INTERFACE DEPTH=10 VARIABLE=b_b max_depth=100,10
  
#pragma ACCEL INTERFACE DEPTH=100 VARIABLE=b_a  max_depth=100
  
#pragma ACCEL INTERFACE DEPTH=10 VARIABLE=a_c max_depth=100,10
  
#pragma ACCEL INTERFACE DEPTH=10 VARIABLE=a_b max_depth=100,10
  
#pragma ACCEL INTERFACE DEPTH=100 VARIABLE=a_a  max_depth=100
  for (int i = 0; i < 100; ++i) {
    b_a[i] = a_a[i];
    b_b[i] = ((const int *)a_b[i]);
    b_c[i] = ((volatile int *)a_c[i]);
  }
  volatile int *here_c;
  const int *here_b;
  volatile int here_a;
  here_a = ((volatile int *)c_a_a)[0];
  here_b = ((const int *)((const int **)c_a_b)[0]);
  here_c = ((volatile int *)((volatile int **)c_a_c)[0]);
  volatile int *here2_c;
  const int *here2_b;
  volatile int here2_a;
  here2_a = ((volatile int *)( *((volatile int **)(&c_a_a))))[0];
  here2_b = ((const int *)((const int **)( *((volatile const int ***)(&c_a_b))))[0]);
  here2_c = ((volatile int *)((volatile int **)( *((volatile int ***)(&c_a_c))))[0]);
  b_a[10] = here_a;
  b_b[10] = ((const int *)here_b);
  b_c[10] = ((volatile int *)here_c);
  b_a[11] = here2_a;
  b_b[11] = ((const int *)here2_b);
  b_c[11] = ((volatile int *)here2_c);
  volatile int **ptr_c;
  const int **ptr_b;
  volatile int *ptr_a;
  ptr_a = ((volatile int *)c_a_a);
  ptr_b = ((const int **)c_a_b);
  ptr_c = ((volatile int **)c_a_c);
  b_a[0] = b_b[1][0];
  b_a[0] = b_c[1][0];
  b_a[0] = c_a_a[0];
  b_a[0] = c_c_a;
  b_a[0] = d_a_a[0];
  b_a[12] =  *ptr_a;
  b_b[12] = ((const int *)( *ptr_b));
  b_c[12] = ((volatile int *)( *ptr_c));
  return ;
}
