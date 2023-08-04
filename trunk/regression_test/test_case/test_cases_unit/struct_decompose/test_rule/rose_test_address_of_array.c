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
struct st g[100];
char g_b[100];
int g_a[100];
#pragma ACCEL kernel

void top(int *a_a)
{
  
#pragma ACCEL INTERFACE DEPTH=100 VARIABLE=a_a max_depth=100
  int i;
  int *p_a[2] = {&a_a[0], &a_a[0]};
  for (i = 0; i < 100; ++i) {
    p_a[0][i] = p_a[1][i] + ((int )g_b[i]);
  }
  return ;
}

int main()
{
  char *a_b;
  int *a_a;
  
#pragma ACCEL wrapper VARIABLE=a->a port=a_a data_type=int
  
#pragma ACCEL wrapper VARIABLE=a->b port=a_b data_type=char
  
#pragma ACCEL wrapper VARIABLE=a port=a data_type="struct st"
  int i;
  struct st *a;
  a = ((struct st *)(malloc(sizeof(struct st ) * 100)));
  for (i = 0; i < 100; ++i) {
    a[i] . a = i;
    g[i] . b = '0';
  }
  top(a_a);
  return 0;
}