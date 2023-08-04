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
#include <string.h>
#define N 100

struct st 
{
  int alpha[100];
  int beta[100];
}
;
#pragma ACCEL kernel

void top(int (*a_alpha)[100],int (*a_beta)[100],int *res)
{
  
#pragma ACCEL INTERFACE DEPTH=100,100 VARIABLE=a_beta max_depth=100,100
  
#pragma ACCEL INTERFACE DEPTH=100,100 VARIABLE=a_alpha max_depth=100,100
  int i;
  memcpy((void *)res,(const void *)( *a_alpha),(size_t )(4 * 100));
  for (i = 0; i < 100; ++i) {
    res[i] = (res[i] + ( *a_beta)[i]) * 2;
  }
  return ;
}

int main()
{
  int (*a_beta)[100];
  int (*a_alpha)[100];
  
#pragma ACCEL wrapper VARIABLE=a->alpha port=a_alpha data_type=int
  
#pragma ACCEL wrapper VARIABLE=a->beta port=a_beta data_type=int
  
#pragma ACCEL wrapper VARIABLE=a port=a data_type="struct st"
  
#pragma ACCEL wrapper VARIABLE=res port=res data_type=int
  int i;
  struct st a;
  int arr[100];
  for (i = 0; i < 100; ++i) {
    a . alpha[i] = i;
    a . beta[i] = 100 - i;
  }
  top(a_alpha,a_beta,arr);
  return 0;
}
