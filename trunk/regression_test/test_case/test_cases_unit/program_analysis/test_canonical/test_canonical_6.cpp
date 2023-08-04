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
// Testing different loop initial

#pragma ACCEL kernel
void func_loop() {
  int a[10];
  int b[10];
  int c[10];

  int i = 0;
  int j = 0;
  int k = 0;
  int x, y, z;
  int N = 100;

  for (i = a[i], j = 3; i < N; i++) {
    /* block 1*/
    c[i] = a[i] + b[j];
  }
  int TC=N/10;
  for (i = a[0]; i >= 0; i--) {
#pragma HLS loop_tripcount  max=TC
    /* block 2*/
    c[i] = a[i] + b[i];
  }
  for (i = a[1]; i >= 0; i--) {
#pragma HLS loop_tripcount  max=11
    /* block 2.1*/
    c[i] = a[i] + b[i];
  }

  for (i = N * 3, j = 3; i >= 0; i--) {
    /* block 3*/
    c[i] = a[i] + b[i];
  }

  for (i = N * x, j = 3; i >= 0; i++) {
    /* block 4*/
    c[i] = a[i] + b[i];
  }

  for (i = N; i >= 0; i--)
    for (j = i, k = 3; j < 10; j++) {
      /* block 5*/
      c[i] = a[k] + b[j];
    }

  for (i = N; i >= 0; i--) {
    for (j = i; j < -1; j += 1) {
      /* block 6*/
      c[i] = a[k] + b[j];
    }
  }

  for (i = N; i >= 0; i--) {
    for (j = i; j < -1; j += 1) {
      /* block 6.1*/
      c[i] = a[k] + b[j];
    }
    i--;
  }

  for (i = N; i >= 0; i--) {
    for (j = i; j < -1; j += 1) {
      /* block 6.2*/
      c[i] = a[k] + b[j];
    }
    i = 2 * i;
  }

  for (i != 0; i < N; i++) {
    /* block 7*/
    c[i] = a[i] + b[i];
  }

  for (i = i / 2; i < N; i++) {
    /* block 8*/
    c[i] = a[i] + b[i];
  }

  for (i << 2; i < N; i--) {
    /* block 9*/
    c[i] = a[i] + b[i];
  }

  for (i = -32767; i > N; i--) {
    c[i] = a[i] + b[i];
  }
}

int main() {
  int aa[100][100];
  int bb[100][100];
  int cc[100][100];
  int *a, *b, *c;
  int a_s, b_s, c_s, d_s;
  float aa_f[100][100];
  float bb_f[100][100];
  float cc_f[100][100];
  float *a_f, *b_f, *c_f;
  float a_s_f, b_s_f, c_s_f, d_s_f;

  func_loop();

  return 0;
}
