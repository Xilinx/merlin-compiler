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
void sub(int *a, int *b, int i){
    for (int j = 0; j < 100; j++) {
      a[i * 100 + j] = b[i * 100 + j] + 100;
	}
}
#pragma ACCEL kernel
void func_top_1(int a[10000],int b[10000])
{
  int i;
  int j;
  for (i = 0; i < 100; i++) {
#pragma ACCEL cache
	  sub(a, b, i);
  }
}
void sub_1(int *a, int *b, int i){
#pragma ACCEL cache
    for (int j = 0; j < 100; j++) {
      a[i * 100 + j] = b[i * 100 + j] + 100;
	}
}
#pragma ACCEL kernel
void func_top_2(int a[10000],int b[10000])
{
  int i;
  int j;
  for (i = 0; i < 100; i++) {
	  sub_1(a, b, i);
  }
}
void sub_2(int *a, int *b, int i){
    for (int j = 0; j < 100; j++) {
      a[i * 100 + j] = b[i * 100 + j] + 100;
	}
}
#pragma ACCEL kernel
void func_top_3(int a[10000],int b[10000])
{
  int i;
  int j;
#pragma ACCEL cache
  for (i = 0; i < 100; i++) {
	  sub_2(a, b, i);
  }
}

void sub_3(int *a, int *b, int i){
    for (int j = 0; j < 100; j++) {
      a[i * 100 + j] = b[i * 100 + j] + 100;
	}
}
#pragma ACCEL kernel
void func_top_4(int a[10000],int b[10000])
{
  int i;
  int j;
#pragma ACCEL cache variable=a,b
  for (i = 0; i < 100; i++) {
	  sub_3(a, b, i);
  }
}
void sub_4(int *a, int *b, int i){
#pragma ACCEL cache variable=a
    for (int j = 0; j < 100; j++) {
      a[i * 100 + j] = b[i * 100 + j] + 100;
	}
}
#pragma ACCEL kernel
void func_top_5(int a[10000],int b[10000])
{
  int i;
  int j;
  for (i = 0; i < 100; i++) {
	  sub_4(a, b, i);
  }
}
void sub_5(int *a, int *b, int i){
    for (int j = 0; j < 100; j++) {
      a[i * 100 + j] = b[i * 100 + j] + 100;
	}
}
#pragma ACCEL kernel
void func_top_6(int a[10000],int b[10000])
{
  int i;
  int j;
#pragma ACCEL cache variable=a,b
  for (i = 0; i < 100; i++) {
	  sub_5(a, b, i);
  }
}

void sub_6(int *a, int *b, int i){
#pragma ACCEL cache
    for (int j = 0; j < 100; j++) {
      a[i * 100 + j] = b[i * 100 + j] + 100;
	}
}

void sub_6_2(int *a, int *b, int i){
    for (int j = 0; j < 100; j++) {
      a[i * 100 + j] = b[i * 100 + j] + 100;
	}
}
#pragma ACCEL kernel
void func_top_7(int a[10000],int b[10000])
{
  int i;
  int j;
  for (i = 0; i < 100; i++) {
	  sub_6(a, b, i);
	  sub_6_2(a, b, i);
  }
}
