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
#define N 2

struct st 
{
  int a[2];
  char b;
}
;
static void __merlin_dummy_g();
char g_b[2] = {((char )'a'), ((char )'b')};
int g_a[2][2] = {{(1), (2)}, {(3), (4)}};
#pragma ACCEL kernel

void top(int (*a_a)[2])
{
  
#pragma ACCEL INTERFACE DEPTH=100,2 VARIABLE=a_a max_depth=100,2
  int i;
  int j;
  char f_b[2] = {((char )'c'), ((char )'d')};
  int f_a[2][2] = {{(5), (6)}, {(7), (8)}};
  for (i = 0; i < 2; ++i) {
    for (j = 0; j < 2; ++j) 
      a_a[i][j] = a_a[i][j] + g_a[i][j] + ((int )g_b[i]) + f_a[i][j] + ((int )f_b[i]);
  }
  return ;
}

int main()
{
  char *a_b;
  int (*a_a)[2];
  
#pragma ACCEL wrapper VARIABLE=a->a port=a_a data_type=int
  
#pragma ACCEL wrapper VARIABLE=a->b port=a_b data_type=char
  
#pragma ACCEL wrapper VARIABLE=a port=a data_type="struct st"
  int i;
  struct st *a;
  a = ((struct st *)(malloc(sizeof(struct st ) * 2)));
  for (i = 0; i < 2; ++i) {
    a[i] . a[0] = a[i] . a[1] = i;
  }
  top(a_a);
  return 0;
}
