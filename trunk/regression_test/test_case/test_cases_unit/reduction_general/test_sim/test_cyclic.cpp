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
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>

#pragma ACCEL kernel
void func_top(int a[1000], int sum[100])
{
	int j, j_sub;
	for (j = 0; j < 100; j++) 
  {
#pragma ACCEL PIPELINE
#pragma ACCEL REDUCTION VARIABLE=sum
    for (j_sub = 0; j_sub < 10; ++j_sub) 
    {
#pragma ACCEL PARALLEL
      for (int j_sub1 = 0; j_sub1 < 1; ++j_sub1) {
#pragma ACCEL PARALLEL
        sum[j] += a[j * 10 + j_sub];
      }
    }
  }
}

void func_top_sw(int a[1000], int sum[100])
{
	int j, j_sub;
  for (j = 0; j < 100; j++) 
  {
    for (j_sub = 0; j_sub < 10; ++j_sub) 
    {
      for (int j_sub1 = 0; j_sub1 < 1; ++j_sub1) {
        sum[j] += a[j * 10 + j_sub];
      }
    }
  }
}

int main() {

  int c[1000];
  for (int i = 0; i < 1000; i++)
  {
    c[i]=i;
  }
  int sum[100]={0};
  int sum_sw[100]={0};
  func_top(c, sum);
  func_top_sw(c, sum_sw);
  for (int j = 0; j < 100; j++) {
    int larger = sum[j]-sum_sw[j]; 
    if (larger != 0)
    {   
      printf("[ERROR] out=%d exp=%d\n", sum[j], sum_sw[j]);
      printf("Error.\n");
      return 1 ;
    }   
  }
  printf("Success.\n");
  return 0;
}
