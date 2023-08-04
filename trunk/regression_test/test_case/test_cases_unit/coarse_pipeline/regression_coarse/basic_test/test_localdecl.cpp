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
// sub function
void sub_0(int a[100][100], int b[100][100]) {

  int i, j;
  for (i = 0; i < 100; i++) {
    if (a[0][0]) {
      for (j = 0; j < 100; j++)
        a[i][j] += b[i][j];
    }
  }
}

#pragma ACCEL kernel
void func_top_7(int a[100][100], int b[100][100], int c[100][100]) {

  int i;
  for (i = 0; i < 2; ++i) {
#pragma ACCEL pipeline
    int a_buf[100][100];
    int c_buf[100][100];
    sub_0(a_buf, b);
    sub_0(a, a_buf);
    sub_0(c_buf, a_buf);
    sub_0(c, c_buf);
  }
}

#pragma ACCEL kernel
void func_top_7_1(int a[100][100], int b[100][100], int c[100][100]) {

  int i;
  int c_buf[100][100];
  for (i = 0; i < 2; ++i) {
#pragma ACCEL pipeline
    int a_buf[100][100];
    if (i > 0)
      sub_0(c_buf, a_buf);
    sub_0(a_buf, b);
    sub_0(a, a_buf);
    sub_0(c, c_buf);
  }
}

#pragma ACCEL kernel
void func_top_7_2(int a[100][100], int b[100][100], int c[100][100]) {

  int i;
  for (i = 0; i < 2; ++i) {
#pragma ACCEL pipeline
    int a_buf[100][100];
    int c_buf[100][100];
    if (i > 0)
      sub_0(c_buf, a_buf);
    sub_0(a_buf, b);
    sub_0(a, a_buf);
    sub_0(c, c_buf);
  }
}

#pragma ACCEL kernel
void func_top_7_3(int a[100][100], int b[100][100], int c[100][100]) {

  int i;
  for (i = 0; i < 2; ++i) {
#pragma ACCEL pipeline
    int a_buf[100][100], c_buf[100][100];
    int inc1=0, inc2=1;
    int aa, bb, cc;
    sub_0(a_buf, b);
    sub_0(a, a_buf);
    sub_0(c_buf, a_buf);
    sub_0(c, c_buf);
  }
}
// MER-171
#pragma ACCEL kernel
void func_top_4(float a[100][100], float b[100][100]) {

  int i, j;
  for (i = 0; i < 100; i++) {
#pragma ACCEL pipeline
    float a_buf[100][100];
    float c_buf[100][100];
    for (j = 0; j < 100; j++) {
      c_buf[i][j] = a[i][j];
    }
    for (j = 0; j < 100; j++) {
      if (i == j)
        a_buf[i][j] = b[i][j];
    }
    for (j = 0; j < 100; j++) {
      c_buf[i][j] += a_buf[i][j];
    }
    for (j = 0; j < 100; j++) {
      a[i][j] = c_buf[i][j];
      b[i][j] = a_buf[i][j];
    }
  }
}

#pragma ACCEL kernel
void func_top_4_0(float a[100][100], float b[100][100]) {

  int i, j;
  float a_buf[100][100];
  float c_buf[100][100];

  for (i = 0; i < 100; i++) {
#pragma ACCEL pipeline
    for (j = 0; j < 100; j++) {
      c_buf[i][j] = a[i][j];
    }
    for (j = 0; j < 100; j++) {
      if (i == j)
        a_buf[i][j] = b[i][j];
    }
    for (j = 0; j < 100; j++) {
      c_buf[i][j] += a_buf[i][j];
    }
    for (j = 0; j < 100; j++) {
      a[i][j] = c_buf[i][j];
      b[i][j] = a_buf[i][j];
    }
  }
}

