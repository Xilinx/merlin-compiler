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
#define PRINT 0

#if PRINT
#include <iostream>
using namespace std;
#endif

struct S1 {
  int x;
  int y;
};

struct S {
  int x;
  int y;
  S() : x(168), y(0) {}
  S(S1 s1) : x(s1.x), y(s1.y) {}
  operator int() {
    return x;
  }
};

void g1(S1 s1, int y) {
  S s(s1);
  s + y;
#if PRINT
  cout << "g1: " << s.x << ", " << y << endl;
  cout << "g1: " << s + y << endl;
#endif
}

int operator+(S x, int y) {
  return x.x + y + 10000;
}

S operator+(S x, S y) {
  S s;
  s.x = x.x + y.x;
  s.y = x.y + y.y;
  return s;
}

#if PRINT
  std::ostream& operator<<(std::ostream& O, S x) {
    O << x.x << ", " << x.y << endl;
    return O;
  }
#endif

void g2(S1 s1, int y) {
  S s(s1);
  s + y;
#if PRINT
  cout << "g2: " << s.x << ", " << y << endl;
  cout << "g2: " << s + y << endl;
#endif
}

#pragma ACCEL kernel
void f() {
  S s1, s2;
  int x, y = 8;
  s1 + y;
  s1 + s2;
  S1 ss;
  ss.x = 168;
  ss.y = 0;
  g1(ss, y);
  g2(ss, y);
#if PRINT
  cout << s1 + y << endl;
  cout << s1 + s2 << endl;
#endif
}

int main() {
  f();
}
