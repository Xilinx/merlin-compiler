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
void func_top_1(int a[10000],int b[10000])
{
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 100; j++) {
      
#pragma ACCEL cache
      a[i * 100 + j] = b[i * 100 + j] + 100;
    }
  }
}
#pragma ACCEL kernel
void func_top_2(int a[10000],int b[10000])
{
  int i;
  int j;
  for (i = 0; i < 100; i++) {
#pragma ACCEL cache
    for (j = 0; j < 100; j++) {
      
      a[i * 100 + j] = b[i * 100 + j] + 100;
    }
  }
}
#pragma ACCEL kernel
void func_top_3(int a[10000],int b[10000])
{
  int i;
  int j;
#pragma ACCEL cache
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 100; j++) {
      
      a[i * 100 + j] = b[i * 100 + j] + 100;
    }
  }
}


#pragma ACCEL kernel
void func_top_4(int a[10000],int b[10000])
{
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 100; j++) {
      
#pragma ACCEL cache variable=a
      a[i * 100 + j] = b[i * 100 + j] + 100;
    }
  }
}
#pragma ACCEL kernel
void func_top_5(int a[10000],int b[10000])
{
  int i;
  int j;
  for (i = 0; i < 100; i++) {
#pragma ACCEL cache variable=a
    for (j = 0; j < 100; j++) {
      
      a[i * 100 + j] = b[i * 100 + j] + 100;
    }
  }
}
#pragma ACCEL kernel
void func_top_6(int a[10000],int b[10000])
{
  int i;
  int j;
#pragma ACCEL cache variable=a
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 100; j++) {
      
      a[i * 100 + j] = b[i * 100 + j] + 100;
    }
  }
}
#pragma ACCEL kernel
void func_top_7(int a[10000],int b[10000])
{
  int i;
  int j;
  for (i = 0; i < 100; i++) {
#pragma ACCEL cache variable=a
    for (j = 0; j < 100; j++) {
      
#pragma ACCEL cache variable=b
      a[i * 100 + j] = b[i * 100 + j] + 100;
    }
  }
}
#pragma ACCEL kernel
void func_top_8(int a[10000],int b[10000])
{
  int i;
  int j;
#pragma ACCEL cache
  for (i = 0; i < 100; i++) {
#pragma ACCEL cache variable=a
    for (j = 0; j < 100; j++) {
      
      a[i * 100 + j] = b[i * 100 + j] + 100;
    }
  }
}
#pragma ACCEL kernel
void func_top_9(int a[10000],int b[10000])
{
  int i;
  int j;
#pragma ACCEL cache
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 100; j++) {
#pragma ACCEL cache variable=b
      
      a[i * 100 + j] = b[i * 100 + j] + 100;
    }
  }
}
#pragma ACCEL kernel
void func_top_10(int a[10000],int b[10000])
{
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 100; j++) {
      
#pragma ACCEL cache variable=a,b
      a[i * 100 + j] = b[i * 100 + j] + 100;
    }
  }
}

#pragma ACCEL kernel
void func_top_11(int a[10000000],int b[10000000])
{
  int i;
  int j;
  for (i = 0; i < 100; i++) {
#pragma ACCEL cache variable=a,b
    for (j = 0; j < 100; j++) {
      
      a[i * 100 + j] = b[i * 1000000 + j] + b[i * 1000000 + j + 1000000];
    }
  }
}
#pragma ACCEL kernel
void func_top_12(int a[10000000],int b[10000000])
{
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 100; j++) {
      
#pragma ACCEL cache variable=a,b
      a[i * 100 + j] = b[i * 1000000 + j] + b[i * 1000000 + j + 1000000];
    }
  }
}
