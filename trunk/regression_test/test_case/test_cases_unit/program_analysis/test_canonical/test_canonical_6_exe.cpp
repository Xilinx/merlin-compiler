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
  int k = 0;
  int N = 100;

  for (i = a[i], j = 3; i < N; i++) {
    /* block 1*/
    c[i] = a[i] + b[j];
    printf("index = %d\n", i);
  }
  for (i = N*3-1, j = 3; i >= 0; i--) {
    /* block 3*/
    c[i/3] = a[i/3] + b[i/3];
    printf("index = %d\n", i/3);
  }
  for (i = N; i >= 0; i--)
    for (j = i, k = 3; j < 10; j++) {
      /* block 5*/
      c[i] = a[k] + b[j];
      printf("index = %d\n", i);
    }

  for (i = N; i >= 0; i--) {
    for (j = i; j < -1; j += 1) {
      /* block 6*/
      c[i] = a[k] + b[j];
      printf("index = %d\n", i);
    }
  }
  for(int pp=0; pp<100; pp++)
    printf("result = %d\n", c[pp]);

}

int main() {

  func_loop();
  return 0;
}
