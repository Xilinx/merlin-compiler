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
  char c[12];
}
;
static void __merlin_dummy_g();
char g_c[12];
char g_b;
int g_a;

void sub(char (*a_c)[12])
{
  
#pragma HLS array_partition variable=a_c dim=2
}
#pragma ACCEL kernel

void top(int a_a[10],char a_b[10],char a_c[10][12],char *b_b,int *c)
{
  
#pragma ACCEL INTERFACE DEPTH=100 VARIABLE=b_b max_depth=100
  
#pragma ACCEL INTERFACE DEPTH=100,12 VARIABLE=a_c max_depth=10,12
  
#pragma ACCEL INTERFACE DEPTH=100 VARIABLE=a_b max_depth=10
  
#pragma ACCEL INTERFACE DEPTH=100 VARIABLE=a_a max_depth=10
  char tmp_c[12];
  sub(&tmp_c);
  for (int i = 0; i < 12; ++i) {
    tmp_c[i] = ((char )c[i]);
  }
  sub(a_c);
  for (int i = 0; i < 100; ++i) {
    
#pragma ACCEL FALSE_DEPENDENCE variable=a_a
    
#pragma ACCEL FALSE_DEPENDENCE variable=a_b
    
#pragma ACCEL FALSE_DEPENDENCE variable=a_c
    a_b[i] = ((char )(a_a[i] + ((int )b_b[i]) + ((int )tmp_c[i % 12]) + ((int )g_c[i % 12])));
  }
  return ;
}

int main()
{
  char (*b_c)[12];
  char *b_b;
  int *b_a;
  char a_c[10][12];
  char a_b[10];
  int a_a[10];
  
#pragma ACCEL wrapper VARIABLE=a->a port=a_a data_type=int
  
#pragma ACCEL wrapper VARIABLE=a->b port=a_b data_type=char
  
#pragma ACCEL wrapper VARIABLE=a->c port=a_c data_type=char
  
#pragma ACCEL wrapper VARIABLE=a port=a data_type="struct st"
  
#pragma ACCEL wrapper VARIABLE=b->a port=b_a data_type=int
  
#pragma ACCEL wrapper VARIABLE=b->b port=b_b data_type=char
  
#pragma ACCEL wrapper VARIABLE=b->c port=b_c data_type=char
  
#pragma ACCEL wrapper VARIABLE=b port=b data_type="struct st"
  
#pragma ACCEL wrapper VARIABLE=c port=c data_type=int
  struct st *a;
  a = ((struct st *)(malloc(sizeof(struct st ) * 100)));
  struct st *b;
  b = ((struct st *)(malloc(sizeof(struct st ) * 100)));
  int *c = (int *)(malloc(sizeof(int ) * 12));
  for (int i = 0; i < 100; ++i) 
    a[i] . a = i;
  top(a_a,a_b,a_c,b_b,c);
  free(a);
  free(b);
  free(c);
  return 0;
}
