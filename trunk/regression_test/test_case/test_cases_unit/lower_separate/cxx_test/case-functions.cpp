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
#include "common.h"

int var;

void f() { var++; }

void f(int x) { var += x; }

void f(int x, int y, int z = 100) { var += x + y + z; }

void og() { var++; }

void og(Common x, int y = 100) { var += x.x + y; }

void og(Common x, Common y, Common z = 100) { var += x.x + y.x + z.x; }

void rec(int i) {
  if (i <= 0) {
    return;
  } else {
    var += i;
    rec(i - 1);
  }
}

void rec_dead() {
  rec_dead();
}

void rec1(int i);
void rec2(int i);
void rec3(int i);

void rec1(int i) {
  if (i <= 0) {
    return;
  } else {
    var += i;
    rec2(i - 1);
  }
}

void rec2(int i) {
  var += i;
  rec3(i - 1);
}

void rec3(int i) {
  var += i;
  rec1(i - 1);
}

void fc() {
  var += 1;
}

void mid1() {
  fc();
}

void mid2() {
  fc();
}

void start() {
  mid1();
  mid2();
}

#pragma ACCEL kernel
void g() {
  f();
  f(1);
  f(2, 3);
  f(4, 5, 6);

  og();
  og(1);
  og(1, 2);
  og(2, Common(3));
  og(4, 5, 6);

  fc();
  start();
}
