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

int add_func_1(int a, int b, int *c) { return a + b + *c; }

int add_func_2(int a[100], int b, int i, int j, int &c) { return a[i] + b * c; }

void add_func_3_0(int a[100], int i, int c) { a[i] = c + 1; }

void add_func_3(int a[100], int i, int &c) { a[i] = c + 1; }

void add_func_4(int a[100], int i, int &c) { c = a[i]; }

void add_func_5(int a, int *&c) { c = &a; }

#pragma ACCEL kernel
void func_top_0() {
  int a[1];
  int b[100];
  int i;
  int j;
  int aa = 1;
  int bb = 0;
  a[0] = 50;
  for (i = 0; i < 99; i++) {
    for (j = 0; j < 90; j++) {
      a[i] = add_func_1(b[j], b[j + 1], &aa);
      a[i + 1] = add_func_1(b[j + 2], b[j + 3], &bb);
    }
  }

  for (i = 0; i < 99; i++) {
    for (j = 0; j < 99; j++) {
      b[i] = add_func_2(b, a[0], j, j + 1, aa);
    }
  }
}

#pragma ACCEL kernel
void func_top_1() {
  int a[100];
  int b[100];
  int i;
  int j;
  int aa = 1;
  for (i = 0; i < 99; i++) {
    for (j = 0; j < 90; j++) {
      a[i] = add_func_1(b[j], b[j + 1], &aa);
      a[i + 1] = add_func_1(b[j + 2], b[j + 3], &aa);
    }
  }
}

#pragma ACCEL kernel
void func_top_2() {
  int a[1];
  int b[100];
  int i;
  int j;
  int aa = 1;
  int bb = 0;
  a[0] = 50;

  for (i = 0; i < 99; i++) {
    for (j = 0; j < 99; j++) {
      b[i] = add_func_2(b, a[0], j, j + 1, aa);
      b[i + 1] = add_func_2(b, a[0], j, j + 1, bb);
    }
  }
}

#pragma ACCEL kernel
void func_top_3() {
  int a[1];
  int b[100];
  int i;
  int j;
  int aa = 1;
  int bb = 0;
  int cc = 2;
  a[0] = 50;

  for (j = 0; j < 99; j++) {
    add_func_3(b, j, aa);
    add_func_4(b, j, a[0]);
    add_func_5(a[0], &bb);
    add_func_3(b, j, bb);
    add_func_3_0(b, j, aa);
    add_func_3_0(b, j, cc);
  }
}
