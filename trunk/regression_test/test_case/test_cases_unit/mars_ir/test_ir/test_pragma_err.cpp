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

#define n 100

#pragma ACCEL kernel
void func_top_0(int N) {
  int a[100][100];
  int b[100][100];
  int sum=0;
  int k;
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    for (j = 0; j < n; j++) {
#pragma ACCEL pArallel 
      a[i][j] = b[i][j] + b[i + 1][j + 1];
    }
  }  for (i = 0; i < 100; i++) {
    for (j = 0; j < 100; j++) {
#pragma ACCEL factor=-1
      a[i][j] = b[i][j] + b[i + 1][j + 1];
    }
  }
  for (i = 0; i < 100; i++) 
#pragma ACCEL pipeline_parallel
    sum+=i;

}

#pragma ACCEL kernel
void func_top_1(int a[100], int b[100]) {
  int i;
  for (i = 0; i < 99; i++) {
#pragma ACCEL pipeline
      a[i] = b[i] + b[i + 1];
    }
  for (i = 1; i < 100; i++) {
#pragma ACCEL pipeline
      a[i] -= b[i] - b[i - 1];
  }

}

#pragma ACCEL kernel
void func_top_2(int a[100], int b[100]) {
  int i;
  for (i = 0; i < 99; i++) {
#pragma ACCEL parallel parallel_iterator=1,2
#pragma ACCEL parallel factor=1 factor=2
      a[i] = b[i] + b[i + 1];
    }
}
