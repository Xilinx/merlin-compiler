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
  union tag {
  int ival;
  float fval;}u;
}
;
#pragma ACCEL kernel

void top(int *a_a,union tag *a_u)
{
  
#pragma ACCEL INTERFACE DEPTH=100 VARIABLE=a_u max_depth=100
  
#pragma ACCEL INTERFACE DEPTH=100 VARIABLE=a_a max_depth=100
  int i;
  for (i = 0; i < 100; ++i) {
    a_a[i] = a_a[i] + a_u[i] . ival;
  }
  return ;
}

int main()
{
  union tag *a_u;
  int *a_a;
  
#pragma ACCEL wrapper VARIABLE=a->a port=a_a data_type=int
  
#pragma ACCEL wrapper VARIABLE=a->u port=a_u data_type="union tag"
  
#pragma ACCEL wrapper VARIABLE=a port=a data_type="struct st"
  int i;
  struct st *a;
  a = ((struct st *)(malloc(sizeof(struct st ) * 100)));
  for (i = 0; i < 100; ++i) {
    a[i] . a = i;
    a[i] . u . ival = 10;
  }
  top(a_a,a_u);
  return 0;
}
