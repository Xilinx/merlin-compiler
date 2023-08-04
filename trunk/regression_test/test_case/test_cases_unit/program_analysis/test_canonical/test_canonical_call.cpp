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
// Testing function calls
#include <stdio.h>

void test_1(int idx, int a[10]) {
  a[idx] = 0;
}

void test_2(int &idx, int a[10]) {
  a[idx] = 0;
}

void test_3(int *idx, int a[10]) {
  a[*idx] = 0;
}

void test_4(int idx, int a[10]) {
  printf("value: %d", idx);
}

void test_5(int idx, int a[10]) {
  printf("value: %d, %d", a[0], idx);
}

void test_6(int idx, int a[10]) {
  printf("value: %d, %d", a[idx], ++idx);
}

void test_7(int &idx, int a[10]) {
  printf("value: %d, %d", a[idx], ++idx);
}

#pragma ACCEL kernel
void func_loop() {
  int a[10];
  int b[10];
  int c[10];

  for (int i1 = 0; i1 < 10; i1++) {
    test_1 (i1, a);
  }
  for (int i2 = 0; i2 < 10; i2++) {
    test_2 (i2, a);
  }
  for (int i3 = 0; i3 < 10; i3++) {
    test_3 (&i3, a);
  }

  for (int i4 = 0; i4 < 10; i4++) {
    test_4 (i4, a);
  }
  for (int i5 = 0; i5 < 10; i5++) {
    test_5 (i5, a);
  }
  for (int i6 = 0; i6 < 10; i6++) {
    test_6 (i6, a);
  }
  for (int i7 = 0; i7 < 10; i7++) {
    test_7 (i7, a);
  }

}

