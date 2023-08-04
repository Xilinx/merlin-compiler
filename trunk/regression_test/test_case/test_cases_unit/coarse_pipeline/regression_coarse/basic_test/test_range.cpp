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
void func_top_1(float a[100][100], float b[100][100]) {

  int i, j;
  float c_buf[100][100];
  float a_buf[100];

  for (i = 0; i < 100; i++) {
#pragma ACCEL pipeline
    int val2 = 1;
    for (j = 0; j < 100; j++) {
      c_buf[i][j] = a[i][j] + val2;
    }
    for (j = 0; j < 100; j++) {
      b[i][j] = c_buf[i][j];
    }
  }
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 100; j++) {
#pragma ACCEL pipeline
      a[i][j] = c_buf[i][j];
    }
  }
}

#pragma ACCEL kernel
void func_top_2(float a[100][100], float b[100][100]) {

  int i, j;
  float a_buf[100];
  float b_buf[100];

  for (i = 0; i < 100; i++) {
#pragma ACCEL pipeline
    if(i%2) {
      for (j = 0; j < 50; j++) {
        a_buf[j] = a[i][j];
      }
    } else {
      for (j = 50; j < 100; j++) {
        a_buf[j] = a[i][j];
      }
    }
    for (j = 0; j < 100; j++) {
      b[i][j] = a[i][j];
    }
  }

  for (i = 0; i < 100; i++) {
#pragma ACCEL pipeline
    if(i%2) {
      for (j = 0; j < 50; j++) {
        b_buf[j] = a[i][j];
      }
    } else {
      for (j = 50; j < 100; j++) {
        b_buf[j] = a[i][j];
      }
    }
    for (j = 0; j < 100; j++) {
      b[i][j] = b_buf[j];
    }
  }
    for (j = 0; j < 100; j++) {
#pragma ACCEL pipeline
      a[0][j] = b_buf[j] + a_buf[j];
    }
}

#pragma ACCEL kernel
void func_top_3(float a[100][100], float b[100][100]) {

  int i, j;
  float a_buf[100];
  float b_buf[100];

  for (i = 0; i < 100; i++) {
#pragma ACCEL pipeline
    for (j = 0; j < 40; j++) {
      b_buf[j] = a[i][j];
    }
    for (j = 0; j < 60; j++) {
      b_buf[j] = a[i][j];
    }
  }
    for (j = 0; j < 100; j++) {
#pragma ACCEL pipeline
      a[0][j] = b_buf[j];
    }
}
