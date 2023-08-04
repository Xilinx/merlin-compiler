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
#pragma ACCEL kernel

void func_top_0()
{
  int a[100][100];
  int b[101][101];
  int k;
  int i;
  int j;
  for (i = 0; i < 10; i++) 
  {

#pragma ACCEL FALSE_DEPENDENCE VARIABLE=a 
    for (k = 0; k < 10; k++) 
    {

#pragma ACCEL PARALLEL COMPLETE 
      for (j = 0; j < 100; j++) 
      {
#pragma ACCEL PARALLEL COMPLETE 
        a[i][j] = b[i * k][j] + b[i * k + 1][j + 1];
      }
    }
  }
}
#pragma ACCEL kernel

void func_top_1()
{
  int a[100][100];
  int b[101][101];
  int k;
  int i;
  int j;
  for (i = 0; i < 10; i++) 
    // Original: #pragma ACCEL pipeline flatten
  {

#pragma ACCEL FALSE_DEPENDENCE VARIABLE=a 
    for (k = 0; k < 10; k++) {

#pragma ACCEL PARALLEL COMPLETE
      for (j = 0; j < 100; j++) {

#pragma ACCEL PARALLEL COMPLETE
        a[k][j] += b[i * k][j] + b[i * k + 1][j + 1];
      }
    }
  }
}
#pragma ACCEL kernel

void func_top_2()
{
  int a[100][100];
  int b[101][101];
  int k;
  int i;
  int j;
  for (i = 0; i < 10; i++) 
    // Original: #pragma ACCEL parallel complete
    // Original: #pragma ACCEL pipeline_parallel flatten
  {

#pragma ACCEL FALSE_DEPENDENCE 

    for (k = 0; k < 10; k++) {

#pragma ACCEL PARALLEL COMPLETE
      for (j = 0; j < 100; j++) {

#pragma ACCEL PARALLEL COMPLETE
        a[i][j] = b[i * k][j] + b[i * k + 1][j + 1];
      }
    }
  }
}
#pragma ACCEL kernel

void func_top_3()
{
  int a[100][100];
  int b[101][101];
  int k;
  int i;
  int j;
  for (i = 0; i < 10; i++) 
    // Original: #pragma ACCEL pipeline
  {

#pragma ACCEL FALSE_DEPENDENCE 
    for (k = 0; k < 10; k++) 
      // Original: #pragma ACCEL pipeline flatten
    {

#pragma ACCEL PIPELINE 
      for (j = 0; j < 100; j++) {

#pragma ACCEL PARALLEL COMPLETE
        a[k][j] += b[i * k][j] + b[i * k + 1][j + 1];
      }
    }
  }
}
#pragma ACCEL kernel

void func_top_4()
{
  int a[100][100];
  int b[100][100];
  int k;
  int i;
  int j;
  for (i = 0; i < 100; i++) 
    // Original: #pragma ACCEL parallel complete
    // Original: #pragma ACCEL pipeline
  {

#pragma ACCEL FALSE_DEPENDENCE 
    for (k = 0; k < 100; k++) {
      for (j = 0; j < 100; j++) 
        // Original: #pragma ACCEL pipeline flatten
      {

#pragma ACCEL PIPELINE 
        a[k][j] += b[i * k][j] + b[i * k + 1][j + 1];
        b[k][j] += a[k][j];
      }
    }
  }
}
