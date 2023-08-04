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
struct st_child {
int a;
char b;}g_c;

struct st_parent 
{
  int a;
  char b;
  struct st_child l_c;
}
;
#pragma ACCEL kernel

void top(int *a_a,char *a_b,int *a_l_c_a,char *a_l_c_b)
{
  
#pragma ACCEL INTERFACE DEPTH=100 VARIABLE=a_l_c_b max_depth=100
  
#pragma ACCEL INTERFACE DEPTH=100 VARIABLE=a_l_c_a max_depth=100
  
#pragma ACCEL INTERFACE DEPTH=100 VARIABLE=a_b max_depth=100
  
#pragma ACCEL INTERFACE DEPTH=100 VARIABLE=a_a max_depth=100
  int i;
  for (i = 0; i < 100; ++i) {
    a_a[i] = a_a[i] + a_l_c_a[i];
    a_b[i] = ((char )(((int )a_b[i]) + ((int )a_l_c_b[i])));
  }
  return ;
}

int main()
{
  char *a_l_c_b;
  int *a_l_c_a;
  char *a_b;
  int *a_a;
  
#pragma ACCEL wrapper VARIABLE=a->a port=a_a data_type=int
  
#pragma ACCEL wrapper VARIABLE=a->b port=a_b data_type=char
  
#pragma ACCEL wrapper VARIABLE=a->l_c->a port=a_l_c_a data_type=int
  
#pragma ACCEL wrapper VARIABLE=a->l_c->b port=a_l_c_b data_type=char
  
#pragma ACCEL wrapper VARIABLE=a port=a data_type="struct st_parent"
  int i;
  struct st_parent *a;
  a = ((struct st_parent *)(malloc(sizeof(struct st_parent ) * 100)));
  for (i = 0; i < 100; ++i) {
    a[i] . a = i;
    a[i] . b = (i + '0');
    a[i] . l_c . a = i + 1;
    a[i] . l_c . b = (i + '1');
  }
  top(a_a,a_b,a_l_c_a,a_l_c_b);
  return 0;
}
