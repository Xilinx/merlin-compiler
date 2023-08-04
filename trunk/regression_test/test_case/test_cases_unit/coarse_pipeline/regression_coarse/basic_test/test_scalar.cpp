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
void func_top_5(float a[100][100], float b[100][100]) {

  int i, j;
  float c_buf[100][100];

  for (i = 0; i < 100; i++) {
#pragma ACCEL pipeline
    int val2 = 1;
    for (j = 0; j < 100; j++) {
      c_buf[i][j] = a[i][j] + val2;
    }
    const int val = 5;
    int val1 = 0;
    for (j = 0; j < 100; j++) {
      b[i][j] = c_buf[i][j] + val - val1 + val2;
    }
  }
}

#pragma ACCEL kernel
void func_top_5_0(float a[100][100], float b[100][100]) {

  int i, j;
  float c_buf[100][100];

  for (i = 0; i < 100; i++) {
#pragma ACCEL pipeline
    int val2 = 1;
    for (j = 0; j < 100; j++) {
      c_buf[i][j] = a[i][j] + val2;
    }
    const int val = 5;
    int val1 = 0;
    val2 = i;
    for (j = 0; j < 100; j++) {
      b[i][j] = c_buf[i][j] + val - val1 + val2;
    }
  }
}

#pragma ACCEL kernel
void func_top_5_1(float a[100][100], float b[100][100]) {

  int i, j;
  float c_buf[100][100];
  int val2 = 1;

  for (i = 0; i < 100; i++) {
#pragma ACCEL pipeline
    for (j = 0; j < 100; j++) {
      c_buf[i][j] = a[i][j] + val2;
    }
    const int val = 5;
    int val1 = 0;
    val2 = i;
    for (j = 0; j < 100; j++) {
      b[i][j] = c_buf[i][j] + val - val1 + val2;
    }
  }
}

#pragma ACCEL kernel
void func_top_6(float a[100][100], float b[100][100]) {

  int i, j;
  float c_buf[100][100];

  for (i = 0; i < 100; i++) {
#pragma ACCEL pipeline
    for (j = 0; j < 100; j++) {
      c_buf[i][j] = a[i][j];
    }
    const int val = c_buf[i][0];
    for (j = 0; j < 100; j++) {
      b[i][j] = c_buf[i][j] + val;
    }
  }
}



