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

int f(int a, int *b) {
  return a;
}

void g(int *b) {
  int t = 1;
  ((int(*)[t])b)[0][0] = 0;
}

void fake_dead_f(int a, int b) {}
#pragma ACCEL kernel
void top(struct st *a,int n, int *b)
{
  
#pragma ACCEL interface  variable=a depth=100
#pragma ACCEL interface  variable=b depth=10
  
  struct st *a_p = a;
  int temp = (int )(a[0].b++);
  int i;
  int e[4];
#pragma HLS ARRAY_PARTITION variable=e dim=1 complete
 
  for (i = 0; i < 100; ++i) {
    if (i >= n) 
      break; 
    a_p[i].b = ((char )10);
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
  for (int i = (({
    float b = 0;
    b;
  })); i < ((
{
    double c = 1;
    c;
  })); i += ((
{
    char d = 1;
    d;
  }))) 
    ;

  b[0] = f(f(2, 0), 0);

  g(b);
  fake_dead_f(f(2, 0), i++);

  struct st *const a_p_1 = a;
  a_p_1[0].a = 0;
  return ;
}

int main()
{
  struct st *a;
  a = ((struct st *)(malloc(sizeof(struct st ) * 100)));
  int b[10];
  int i;
  for (i = 0; i < 100; ++i) 
    a[i] . a = i;
  
  top(a,49,b );
  return 0;
}
