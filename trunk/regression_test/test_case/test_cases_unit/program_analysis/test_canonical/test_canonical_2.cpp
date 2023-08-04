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
// Testing multiple loop initial/testing/increment

#pragma ACCEL kernel
void func_loop() {
  int a[10];
  int b[10];
  int c[10];

  int i = 0;
  int j = 0;
  int jjj;
  int x, y, z;
  int N = 100;
  {
    for (int i = 0, jj, j1 = 0, j2 = 0, j3 = 0, x0 = 0, y1, a1; i < N;
         i -= b[i]) {
      /* block 1*/
      c[i] = a[i] + b[i];
    }
  }
  {
    /* block 2*/
    for (int i = 0, jj, j1 = 0, j2 = 0, j3 = 0, x0 = 0, y1, a1; i < N;
         i -= b[i])
      c[i] = a[i] + b[i];
  }

  {
    for (int i = 0, jjj; i >= 0; i += -1) {
      /* block 2.1*/
      c[i] = a[i] + b[i];
    }
  }
  {
    for (int i = 0, jjj = 0; i >= 0; i += -1) {
      /* block 2.2*/
      c[i] = a[i] + b[i + jjj];
    }
  }

  for (i = 0, jjj; i >= 0; i++) {
    /* block 2.3*/
    c[i] = a[i] + b[i];
  }

  for (i = 0, jjj = 0; i >= 0; i++) {
    /* block 2.4*/
    c[i] = a[i] + b[i + jjj];
  }

  for (i = 0, j = 1; i<N, i> 0; i += 2) {
    /* block 4*/
    c[i] = a[i] + b[i];
  }
  for (i = 0, j = 1; i > N && i > 0; i++) {
    /* block 5*/
    c[i] = a[i] + b[i];
  }

  for (i = 0, j * 3; i <= N; i--) {
    /* block 6*/
    c[i] = a[i] + b[i];
  }

  for (i = 0, j = 3; i <= N && j >= 0; i--, j--) {
    /* block 7*/
    c[i] = a[i] + b[i];
  }

  for (i = 0, j = 3; i <= N && j >= 0;) {
    /* block 8*/
  }

  for (i = 0, j = 3; i <= N; i++) {
    /* block 9*/
  }
  for (a[0], i = 0, j = 3, x = 1, y; i <= N; i++) {
    /* block 10*/
  }

  for (i = 0, j = 3; i >= N; i -= 2, j--) {
    /* block 11*/
    c[i] = a[i] + b[i];
  }

  for (i = 0, j = 3; i >= N && j >= N; i *= 2, j *= 2) {
    /* block 12*/
    c[i] = a[i] + b[i];
  }

  for (i = a[0]; i < a[1]; i++) {
    /* block 13*/
    i + 1;
  }

  j = 0;
  for (i = 0; i < 100; i++) {
    /* block 14*/
    j = j + 1;
    j;
  }
  for (i = 0, j = 23; i <= N; i++)
    for (j = -50; j < 1; j++) {
      /* block 15*/
      if (j % 2)
        a[i] = i;
    }
}
