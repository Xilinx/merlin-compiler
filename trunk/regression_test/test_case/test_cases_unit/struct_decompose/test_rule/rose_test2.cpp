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
  int a[100];
  char b[100];
}
;
#pragma ACCEL kernel

void top(int (*a_a)[100],char (*a_b)[100])
{
  
#pragma ACCEL INTERFACE DEPTH=100,100 VARIABLE=a_b max_depth=100,100
  
#pragma ACCEL INTERFACE DEPTH=100,100 VARIABLE=a_a max_depth=100,100
  for (int i = 0; i < 100; ++i) 
    ( *a_b)[i] = ((char )(( *a_a)[i] + 10));
  return ;
}

int main()
{
  char (*a_b)[100];
  int (*a_a)[100];
  
#pragma ACCEL wrapper VARIABLE=a->a port=a_a data_type=int
  
#pragma ACCEL wrapper VARIABLE=a->b port=a_b data_type=char
  
#pragma ACCEL wrapper VARIABLE=a port=a data_type="struct st"
  struct st *a;
  a = ((struct st *)(malloc(sizeof(struct st ))));
  for (int i = 0; i < 100; ++i) 
    a -> a[i] = i;
  top(a_a,a_b);
  return 0;
}
