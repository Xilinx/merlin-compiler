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
// Testing labels
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

l1:
  for (char ic = N; ic >= 0; ic--) {
  l2:
    for (char jc = ic; jc < 100; jc += 2) {
      /* block 1*/
      c[ic] = a[ic] + b[jc];
      printf("index = %d\n", ic);
    }
  }

l3:
  for (char ic = N; ic >= 0; ic--) {
    int cc = 10;
  l4:
    for (char jc = (char)cc - 1; jc < 100; jc += 2) {
      /* block 2*/
      c[ic] = a[ic] + b[jc];
      printf("index = %d\n", ic);
    }
  }
l5:
  for (char ic = N; ic >= 0; ic--) {
    int cc = c[0];
  l6:
    for (char jc = cc; jc < 100; jc += 2) {
      /* block 3*/
      c[ic] = a[ic] + b[jc];
      printf("index = %d\n", ic);
    }
  }
  {

  l7:
    for (char ic = N; ic >= 0; ic--) {
    l8:
      for (char jc = ic; jc < 100; jc += 2) {
        /* block 4*/
        c[ic] = a[ic] + b[jc];
        printf("index = %d\n", ic);
      }
    }
  }

  for(int pp=0; pp<100; pp++)
    printf("result = %d\n", c[pp]);

}

int main() {

  func_loop();
  return 0;
}
