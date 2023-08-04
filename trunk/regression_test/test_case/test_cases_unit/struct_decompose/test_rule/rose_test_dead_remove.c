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
#include<stdlib.h>
#define N 100

struct st 
{
  int a;
  char b;
}
;

int f(int a)
{
  return a;
}

void g(int *b)
{
  int t = 1;
  ((int (*)[t])b)[0][0] = 0;
}

void fake_dead_f(int a,int b)
{
}
#pragma ACCEL kernel

void top(int *a_a,char *a_b,int n,int *b)
{
  
#pragma ACCEL INTERFACE DEPTH=100 VARIABLE=a_b max_depth=100
  
#pragma ACCEL INTERFACE DEPTH=100 VARIABLE=a_a max_depth=100
  
#pragma ACCEL interface variable=b depth=10
  char *a_p_b = a_b;
  int temp = (int )(a_b[0]++);
  int i;
  for (i = 0; i < 100; ++i) {
    if (i >= n) 
      break; 
    a_p_b[i] = ((char )10);
    if (i % 2 == 1) {
      continue; 
    }
  }
//label:
//goto label;
  if (n == 10) 
    return ;
  switch(n){
    case 1:
    break; 
    case 2:
    break; 
    default:
    break; 
  }
  for (int i = (int )(({
    float b = (float )0;
    b;
  })); ((double )i) < ((
{
    double c = (double )1;
    c;
  })); i += ((int )((
{
    char d = (char )1;
    d;
  })))) 
    ;
  b[0] = f((f(2)));
  g(b);
  fake_dead_f((f(2)),i++);
  char *a_p_1_b;
  int *a_p_1_a;
  a_p_1_a = a_a;
  a_p_1_b = a_b;
  a_p_1_a[0] = 0;
  return ;
}

int main()
{
  char *a_b;
  int *a_a;
  
#pragma ACCEL wrapper VARIABLE=a->a port=a_a data_type=int
  
#pragma ACCEL wrapper VARIABLE=a->b port=a_b data_type=char
  
#pragma ACCEL wrapper VARIABLE=a port=a data_type="struct st"
  
#pragma ACCEL wrapper VARIABLE=n port=n data_type=int
  
#pragma ACCEL wrapper VARIABLE=b port=b data_type=int
  struct st *a;
  a = ((struct st *)(malloc(sizeof(struct st ) * 100)));
  int b[10];
  int i;
  for (i = 0; i < 100; ++i) 
    a[i] . a = i;
  top(a_a,a_b,49,b);
  return 0;
}
