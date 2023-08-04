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
// 1. loop structure
// 2. function structure
// 3. access pattern
// 4. pragma modes
#pragma ACCEL kernel

void func_top_1()
{
  int a[100][100];
  int b[100][100];
  int i;
  int j;
  for (i = 0; i < 100; i++) 
/* Original: #pragma ACCEL pipeline flatten */
{
    
#pragma ACCEL PIPELINE 
    for (j = 0; j < 100; j++) 
//bracelet
{
      
#pragma ACCEL PARALLEL COMPLETE
      a[i][j] = b[i][j];
    }
  }
}
#pragma ACCEL kernel

void func_top_4()
{
  int a[100][100];
  int b[100][100];
  int i;
  int j;
  for (i = 0; i < 100; i++) 
/* Original: #pragma ACCEL pipeline flatten */
{
    
#pragma ACCEL PIPELINE 
    for (j = 0; j < 100; j++) {
      
#pragma ACCEL PARALLEL COMPLETE
      a[i][j] = b[i][j];
    }
    for (j = 0; j < 100; j++) {
      
#pragma ACCEL PARALLEL COMPLETE
      a[i + 1][j] = b[i][j + 1];
    }
  }
}
#pragma ACCEL kernel

void func_top_4_0()
{
  int a[100][100];
  int b[100][100];
  int i;
  int j;
  for (j = 0; j < 100; j++) 
/* Original: #pragma ACCEL pipeline flatten */
{
    
#pragma ACCEL PIPELINE 
    for (i = 0; i < 100; i++) {
      
#pragma ACCEL PARALLEL COMPLETE
      a[i][j] = b[i][j];
    }
    
#pragma ACCEL pipeline_parallel factor = 10
    for (i = 0; i < 100; i++) {
      
#pragma ACCEL PARALLEL COMPLETE
      a[i + 1][j] = b[i][j + 1];
    }
  }
}
#pragma ACCEL kernel

void func_top_5()
{
  int a[100][100];
  int b[100][100];
  int i = 0;
  int j = 0;
  for (j = 0; j < 100; j++) {
    a[i + 1][j] = b[i][j + 1];
  }
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 100; j++) 
/* Original: #pragma ACCEL pipeline */
{
      
#pragma ACCEL PIPELINE 
      a[i][j] = b[i][j];
    }
    for (j = 0; j < 100; j++) {
      a[i + 2][j] = b[i + 2][j];
    }
    for (j = 0; j < 100; j++) 
/* Original: #pragma ACCEL pipeline */
{
      
#pragma ACCEL PIPELINE 
      a[i + 1][j] = b[i][j + 1];
    }
  }
  for (j = 0; j < 100; j++) 
/* Original: #pragma ACCEL parallel factor = 10 */
{
    
#pragma ACCEL PARALLEL FACTOR=10 
    a[i + 1][j] = b[i][j + 1];
  }
}
