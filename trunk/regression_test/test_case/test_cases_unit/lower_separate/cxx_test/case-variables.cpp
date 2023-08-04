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
struct A {
  int x;
  A() : x(168) {}
};

struct S {
  int x;
  A y;
  S() : x(168) {}
};

struct C {
  int x;
  A y;
};

void f0(int x, A a, S s, C c) {
}

void f1(int x, A a, C c) {
}

void f2(int x, C c) {
}

void f3(int x) {
}

#pragma ACCEL kernel
void f(int x, C c) {
  int i;
  int xx;
  S s;
  A yy;
  for (i = 0; i < 100; ++i) {
    int x;
    A y;
  }

  if (i > 20) {
    int x;
    A y;
  }
//  f1(x, yy, c);
  f2(x, c);
  f3(x);
}
