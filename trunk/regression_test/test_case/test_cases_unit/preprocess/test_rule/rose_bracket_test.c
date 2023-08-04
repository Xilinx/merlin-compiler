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

void func_top_0(int *a,int *b)
{
  int i;
  for (i = 0; i < 100; i++) {
    a[i] = b[i];
  }
  while(i < 100){
    i++;
  }
  do {
    a[i++] = 0;
  }while (i < 100);
}
#pragma ACCEL kernel

void func_top_1()
{
  int a[100][100];
  int b[100][100];
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 100; j++) {
//bracelet
      a[i][j] = b[i][j];
    }
  }
}
#pragma ACCEL kernel

void func_top_2()
{
  int a[100][100];
  int b[100][100];
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 100; j++) {
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
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 100; j++) {
      a[i][j] = b[i][j];
    }
    for (j = 0; j < 100; j++) {
      a[i + 1][j] = b[i][j + 1];
    }
  }
}
//ifelse
#pragma ACCEL kernel

void func_top_6()
{
  int a[100][100];
  int b[100][100];
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 100; j++) {
      if (a[0][0]) {
        a[i][j] = b[i][j];
      }
    }
  }
}
#pragma ACCEL kernel

void func_top_7()
{
  int a[100][100];
  int b[100][100];
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    if (a[0][0]) {
      for (j = 0; j < 100; j++) {
        a[i][j] = b[i][j];
      }
    }
  }
}
// sub function

void sub_0(int a[100][100],int b[100][100])
{
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    if (a[0][0]) {
      for (j = 0; j < 100; j++) {
        a[i][j] = b[i][j];
      }
    }
     else {
      a[i][0] = b[i][0];
    }
  }
}
