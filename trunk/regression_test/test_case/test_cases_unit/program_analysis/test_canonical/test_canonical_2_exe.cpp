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
#include<stdio.h>

#pragma ACCEL kernel
void func_loop() {
  int a[100]={0};
  int b[100]={1};
  int c[100]={2};
  int i = 0;
  int j = 0;

  int jjj;
  int x, y, z;
  int N = 100;
  {
    for (int i = 0, jjj; i >= 0; i += -1) {
      /* block 2.1*/
      c[-i] = a[-i] + b[-i];
      printf("index = %d\n", -i);
    }
  }
  {
    for (int i = 0, jjj = 0; i >= 0; i += -1) {
      /* block 2.2*/
      c[-i] = a[-i] + b[-i + jjj];
      printf("index = %d\n", -i);
    }
  }

  for (i = 0, jjj; i < N; i++) {
    /* block 2.3*/
    c[i] = a[i] + b[i];
    printf("index = %d\n", i);
  }
  for (i = 0, j = 3; i <= N-1; i++) {
    /* block 9*/
    c[i]--;
    printf("index = %d\n", i);
  }
  for (a[0], i = 1, j = 3, x = 1, y; i <= N; i++) {
    /* block 10*/
    c[i-1]+=2;
    printf("index = %d\n", i-1);
  }

  for (i = 0, j = 3; i >= N; i -= 2, j--) {
    /* block 11*/
    c[i] = a[i] + b[i];
    printf("index = %d\n", i);
  }
  j = 0;
  for (i = 0, j = 23; i <= N; i++)
    for (j = -50; j < 1; j++) {
      /* block 15*/
      if (j % 2) {
        c[i] += i;
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
