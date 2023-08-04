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
volatile int *b;
const int *c;}st;

struct st2 
{
  
#pragma ACCEL attribute variable=a depth=10
  const st *a;
  st c;
}
;
typedef const struct st2 *const_p_st2;
#pragma ACCEL kernel

void top(const volatile int *const a_a,const volatile int **const a_b,const int **const a_c,volatile int *const b_a,volatile int **const b_b,const int **const b_c,const volatile int *c_a_a,const volatile int **c_a_b,const int **c_a_c,const volatile int c_c_a,const volatile int *d_a_a,const volatile int *&e_a_a,const volatile int **&e_a_b,const int **&e_a_c,const volatile int &e_c_a,const volatile int *&e_c_b,const int *&e_c_c,const volatile int **f_a_a,const volatile int ***f_a_b,const int ***f_a_c)
{
  
#pragma ACCEL INTERFACE DEPTH=10,10 VARIABLE=f_a_c max_depth=100,10,10
  
#pragma ACCEL INTERFACE DEPTH=10,10 VARIABLE=f_a_b max_depth=100,10,10
  
#pragma ACCEL INTERFACE DEPTH=10 VARIABLE=f_a_a max_depth=100,10
  
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
    b_b[i] = ((volatile int *)a_b[i]);
    b_c[i] = ((const int *)a_c[i]);
  }
  const int *here_c;
  volatile int *here_b;
  volatile int here_a;
  here_a = c_a_a[0];
  here_b = ((volatile int *)c_a_b[0]);
  here_c = ((const int *)c_a_c[0]);
  b_a[10] = here_a;
  b_b[10] = ((volatile int *)here_b);
  b_c[10] = ((const int *)here_c);
  b_a[0] = b_b[1][0];
  b_a[0] = b_c[1][0];
  b_a[0] = c_a_a[0];
  b_a[0] = c_c_a;
  b_a[0] = d_a_a[0];
  const int *one_c_c;
  volatile int *one_c_b;
  volatile int one_c_a;
  const int **one_a_c;
  const volatile int **one_a_b;
  const volatile int *one_a_a;
  one_a_a = e_a_a;
  one_a_b = e_a_b;
  one_a_c = e_a_c;
  one_c_a = e_c_a;
  one_c_b = ((volatile int *)e_c_b);
  one_c_c = ((const int *)e_c_c);
  b_a[1] = f_a_a[1][0];
  b_b[1] = ((volatile int *)f_a_b[1][0]);
  b_c[1] = ((const int *)f_a_c[1][0]);
  b_a[0] = one_a_a[0];
  b_b[0] = ((volatile int *)one_a_b[0]);
  b_c[0] = ((const int *)one_a_c[0]);
  return ;
}
