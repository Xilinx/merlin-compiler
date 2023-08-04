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
#include<stdio.h>

#pragma ACCEL kernel
void func_loop() {
  int a[100]={0};
  int b[100];
  int c[100];
  for(int pp=0; pp<100; pp++) {
    b[pp]=1;
    c[pp]=2;
  }

  int i = 0;
  int j = 0;
  int N = 100;

  for (i = 0; i >= 0; i += -1) {
    /* block 2*/
    c[i] = a[i] + b[i];
    printf("index = %d\n", i);
  }

  for (i = 0; i < N; i += 2) {
    /* block 3*/
    c[i] = a[i] + b[i];
    printf("index = %d\n", i);
  }

  for (i = -4; i < 96; i += 8) {
    /* block 3.1*/
    c[i+4] -=2;
    printf("index = %d\n", i+4);
  }

  for (i = 0; i >= 0; i -= 1) {
    /* block 4*/
    c[i] = a[i] + b[i];
    printf("index = %d\n", i);
  }

  for (i = N-1; i >= 0; i--) {
    /* block 6*/
    c[i] = a[i] + b[i];
    printf("index = %d\n", i);
  }
//#if 0
  for (i = N-1, j = 3; i >= 0; --i) {
    /* block 7*/
    c[i] = a[i] + b[i];
    printf("index = %d\n", i);
  }

  for (i = 0, j = 3; i >= N; i -= 2, j--) {
    /* block 10*/
    c[i]++;
    printf("index = %d\n", i);
  }
//#endif
  for (i = 0; i < N; i += (1 + 4 * 5 / 2)) {
    /* block 14.1*/
    c[i]++;
    printf("index = %d\n", i);
  }
  for(int pp=0; pp<100; pp++)
    printf("result = %d\n", c[pp]);

}

int main() {

  func_loop();
  return 0;
}
