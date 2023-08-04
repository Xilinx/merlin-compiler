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
#include <assert.h>
#include <stdio.h>
#define SIZE 10
int g = 10;
const int c_g = 11;

void f() { g++; }
void f2(int *m) { *m = 0; }
void f3(int &n) { n = 10; }

void f4(int k) { k++; }

void f5_sub(int k) { k--; }

void f5(int k) { f5_sub(k - 10); }

void f6(int k) { k++; }
void f7(int &a, int b) { a = b; }
void f8(int &a, int b) { a = b; }

void f9(int k) { k++; }

void f10(int &k) { k++; }

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
  int n1 = 100;
  int n2 = 100;
  int n3 = 100;
  int n4 = 100;
  int n5 = 100;
  int nn = sizeof(n);
  printf("%d", nn);
  int nn0 = 5, nn1 = 6;
  nn1 = m + n - i * j;
  int nn3 = m + n - i * j;
  int nn4 = 1, nn5 = m + n - i * j, nn6;
  int val, val1, val2;
  for (i = 0; i < 100; ++i) {
    n1--;
  }
  for (; i < 10; ++i) {
    n--;
  }
  n2--;
  n3 = n3 + 1;
  n4 += 1;
  n5++;
  int x, x1;
  int yy = 100;
  int y = (x = 3) + (x = 4);
  int y1 = (x1 = y = 3) + (x = 4);
  int y2 = (x1 = yy = 3) + (x = 4);
  int nn2 = m + n - i * j;
  nn6 = nn4;

  int h = k + 10;
  f4(h);
  f5(h + 100);
  f6(10);
  f7(val1, 10);
  f7(val2, val1);
  f8(val, nn);
  f8(val, SIZE);
  f8(val, nn + nn1);
  f8(val, nn + nn2);
  f8(val, nn3);
  f8(val, nn5);
  f9(n);
  f9(n1);
  f9(n2);
  f9(n3);
  f9(n4);
  f9(n5);
  f9(y);
  f9(x);
  f9(x1);
  f9(y2);
  f9(y1);
  f10(n);

  int *x0 = &nn4;

  int xx = x > 3 ? 5 : 7;
  if (3 < 4)
    xx = 8;
  assert(xx == 6);
  f9(xx);

  int xx1 = 100;
  if (0)
    xx1 = 8;
  f9(xx1);
}

int main() {
  int a = 1;
  int b = 1;
  int c = 1;

  top(a, b, c);
  return 0;
}
