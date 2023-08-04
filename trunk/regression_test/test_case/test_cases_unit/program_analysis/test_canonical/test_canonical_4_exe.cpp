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
// Testing different loop test
#include <stdio.h>

#pragma ACCEL kernel
void func_loop() {
  int a[100]={0};
  int b[100];
  int c[100];
  for(int pp=0; pp<100; pp++) {
    a[pp]=1;
    b[pp]=5;
    c[pp]=2;
  }
  int i = 0;
  int j = 0;
  int N = 100;

  for (i = 0; i < N; i += 1) {
    /* block 1*/
    c[i] = a[i] * b[i];
    printf("index = %d\n", i);
  }

  for (i = 0; i <= N-1; i += 1) {
    /* block 2*/
    c[i] += a[i] + b[i];
    printf("index = %d\n", i);
  }
  for (i = 0, j = 3; 10 > i; i++) {
    /* block 6*/
    c[i] += j;
    printf("index = %d\n", i);
  }

  for (i = N-1, j = 3; 0 < i; --i) {
    /* block 7*/
    c[i]++;
    printf("index = %d\n", i);
  }

  // Loop range is loop invariant
  for (i = N-2, j = 3; i > b[i]; --i) {
    /* block 8*/
    c[i]--;
    printf("index = %d\n", i);
  }

  for (i = N-3, j = 3; i > b[i] * 2; --i) {
    /* block 17*/
    c[i] += a[i];
    printf("index = %d\n", i);
  }

  for (i = 0, j = 3; (2 + 3 * 4 / 6) > i; i++) {
    /* block 18*/
    c[i] += a[i] + b[i];
    printf("index = %d\n", i);
  }

  for(int pp=0; pp<100; pp++)
    printf("result = %d\n", c[pp]);

}

int main() {

  func_loop();
  return 0;
}
