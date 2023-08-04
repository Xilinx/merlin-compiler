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
// Testing different increments

#pragma ACCEL kernel
void func_loop() {
  int a[10];
  int b[10];
  int c[10];

  int i = 0;
  int j = 0;
  int x, y, z;
  int N = 100;

  for (i = 0; i < N; i += b[0]) {
    /* block 1*/
    c[i] = a[i] + b[i];
  }

  for (i = 0; i >= 0; i += -1) {
    /* block 2*/
    c[i] = a[i] + b[i];
  }

  // range: [-INF,+INF]
  for (i = 0; i >= 0; i -= -1) {
    /* block 2.1*/
    c[i] = a[i] + b[i];
  }

  for (i = 1; i >= 0; i -= -1) {
    /* block 2.2*/
    c[i] = a[i] + b[i];
  }

  for (i = 0; i < N; i += 2) {
    /* block 3*/
    c[i] = a[i] + b[i];
  }

  for (i = 0; i >= 0; i -= 1) {
    /* block 4*/
    c[i] = a[i] + b[i];
  }

  for (i = 0; i < N; i -= 2) {
    /* block 5*/
    c[i] = a[i] + b[i];
  }
  for (i = N; i >= 0; i--) {
    /* block 6*/
    c[i] = a[i] + b[i];
  }

  // range: [-INF,+INF]
  for (i = 0; i >= N; i--) {
    /* block 6.1*/
    c[i] = a[i] + b[i];
  }

  for (i = N, j = 3; i >= 0; --i) {
    /* block 7*/
    c[i] = a[i] + b[i];
  }

  for (i = N, j = 3; i >= 0; i++) {
    /* block 8*/
    c[i] = a[i] + b[i];
  }

  for (i = N, j = 3; i >= 0; ++i) {
    /* block 9*/
    c[i] = a[i] + b[i];
  }
  for (i = 0, j = 3; i >= N; i -= 2, j--) {
    /* block 10*/
    c[i] = a[i] + b[i];
  }

  for (i = 0; i >= N; i *= 2, j *= 2) {
    /* block 11*/
    c[i] = a[i] + b[i];
  }

  for (i = N, j = 3; i >= 0; i *= 2) {
    /* block 12*/
    c[i] = a[i] + b[i];
  }

  for (i = 0; i >= N; i = i + 4 * 5 / 2) {
    /* block 13*/
    c[i] = a[i] + b[i];
  }

  // range: [-INF,+INF], step: 11
  for (i = 0; i >= N; i += (1 + 4 * 5 / 2)) {
    /* block 14*/
    c[i] = a[i] + b[i];
  }

  for (i = 0; i <= N; i += (1 + 4 * 5 / 2)) {
    /* block 14.1*/
    c[i] = a[i] + b[i];
  }

  for (i = 0; i >= N; i = i + 2) {
    /* block 15*/
    c[i] = a[i] + b[i];
  }

  for (i = 0; i >= N; i >> 1) {
    /* block 16*/
    c[i] = a[i] + b[i];
  }
}

int main() {

  func_loop();

  return 0;
}
