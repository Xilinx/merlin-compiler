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
#include <string.h>
#define SIZE 10
int g = 10;
const int c_g = 11;

void f() {
  g++;
}
void f2(int *m) {
  *m = 0;
}
void f3(int &n) {
  n = 10;
}

void f4(int k) {
  k++;
}

void f5_sub(int k) {
  k--;
}

void f5(int k) {
  f5_sub(k - 10);
}

void f6(int k) {
  k++;
}
void f7(int k) {
  k++;
}
void f8(int &a, int b) { a = b; }
#pragma ACCEL kernel
void top(int a, int b, int c) {
  a = 10;
  if (c)
    b = 5;
  int i = 10;
  int ii = 10;
  int j = 0;
  int k;
  int m = 0;
  int n = 100;
  int nn = sizeof(n);
  int nn1 = m + n - i * j;
  int val;
  char str[20] = "0123456789";
  int a1 = strlen(str);
  int b1 = sizeof(str);
  float b2 = sizeof(str);

  if (c) {
    int aa[2][2] = {{1, 2}, {1, 2}};
    ii;
    a;
    b;
  }
  for (i = 0; i < 100; ++i) {}
  for (; i < 10; ++i) {}
  j++;
  *(&j) = 1;
  k= 0;
  k = 100;
  f();
  c + c_g;

  f2(&m);
  f3(n);
  int h = k + 10;
  f4(h);
  f5(h + 100);
  f6(10);
  f6(10);
  f7(10);
  f7(11);
  f8(val, nn);
  f8(val, SIZE);
  f8(val, nn + nn1);
  f8(val, a1);
  f8(val, b1);
  f8(val, b2);
}
