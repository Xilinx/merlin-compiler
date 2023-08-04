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
#include <stdio.h>
// Testing different loop test
#pragma ACCEL kernel
void func_loop() {
  int a[10];
  int b[10];
  int c[10];

  int i = 0;
  int j = 0;
  int x, y, z;
  int N = 100;
  for (i = 0; i < N; i += 1) {
    /* block 1*/
    c[i] = a[i] + b[i];
  }

  for (i = 0; i <= N; i += 1) {
    /* block 2*/
    c[i] = a[i] + b[i];
  }

  for (i = 0, j = 3; i != N, j != N; i += 2) {
    /* block 3*/
    c[i] = a[i] + b[i];
  }

  for (i = 0; i != N; i += 2) {
    /* block 4*/
    c[i] = a[i] + b[i];
  }

  for (i = N, j = 1; j > i && i > 0; i--) {
    /* block 5*/
    c[i] = a[i] + b[i];
    j--;
  }

  for (i = 0, j = 3; 10 > i; i++) {
    /* block 6*/
    c[i] = a[i] + b[i];
  }

  for (i = N, j = 3; 0 < i; --i) {
    /* block 7*/
    c[i] = a[i] + b[i];
  }

  // Loop range is loop invariant
  for (i = N, j = 3; i > b[i]; --i) {
    /* block 8*/
    c[i] = a[i] + b[i];
  }
  for (i = N; i > b[0]; --i) {
    /* block 9*/
    c[i] = a[i] + b[i];
  }

  for (i = N; i >= b[0]; i -= 0.99) {
    /* block 10*/
    c[i] = a[i] + b[i];
  }
  // TODO
  for (i = N; i <= 101; i += 1.28) {
    /* block 11*/
    printf("1) %d\n", i);
  }

  // TODO
  for (i = N; i <= 120; i += 1.51) {
    /* block 12*/
    printf("2) %d\n", i);
  }

  // TODO
  for (i = N; i > 95; i -= 1.51) {
    /* block 13*/
    printf("3) %d\n", i);
  }

  // TODO
  for (i = 0; i > -5; i -= 1.51) {
    /* block 14*/
    printf("4) %d\n", i);
  }

  // TODO
  for (i = -5; i <= 0; i += 1.51) {
    /* block 15*/
    printf("5) %d\n", i);
  }
  // TODO
  for (i = -10; i <= 10; i += 1.51) {
    /* block 16*/
    printf("6) %d\n", i);
  }
  for (i = N, j = 3; i > b[i] * 2; --i) {
    /* block 17*/
    c[i] = a[i] + b[i];
  }

  for (i = 0, j = 3; (2 + 3 * 4 / 6) > i; i++) {
    /* block 18*/
    c[i] = a[i] + b[i];
  }
}

int main() {

  func_loop();

  return 0;
}
