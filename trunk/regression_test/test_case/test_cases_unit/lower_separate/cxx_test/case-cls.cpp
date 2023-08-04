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

struct Foo {
  int x;
  volatile int x1;
  int y = 168;
  static int z;

  Foo() {
    x = 168;
  }

  Foo(int val) : x(val) {}
};

int Foo::z = 168;

struct Bar {
  Foo f() { return Foo(); }
  Foo f(int val) { return Foo(val); }
  Foo f1() { return Foo(168); }

  Common c() { return Common(); }
  Common c(int val) { return Common(val); }
  Common c1() { return Common(168); }

  Bar() {
    this->foo = Foo();
    this->common = Common();
  }

  Bar(int val) : foo(val) {
    this->common = Common(val);
  }

  Bar(Foo f, Common c) : foo(f) {
    common = c;
  }

  Foo foo;
  Common common;
  const int x = 168;

  static int fstatic();

  static int fstatic1() {
    return 168;
  }
};

struct INT {
  int x;

  INT() : x(168) {}

  friend struct Bar;
  operator int() {
    return x;
  }
};

void fff(Bar &bar1, Bar *bar2) {}

#pragma ACCEL kernel
void g() {
  Foo foo1 = Foo();
  Foo foo2 = Foo(128);
  Foo foo3 = Bar().f();
  Foo foo4 = Bar().f(128);
  Foo foo5 = Bar().f1();

  Common common1 = Common();
  Common common2 = Common(128);
  Common common3 = Bar().c();
  Common common4 = Bar().c(128);
  Common common5 = Bar().c1();

  Bar bar1 = Bar();
  Bar bar2 = Bar(168);
  Bar bar3 = Bar(foo5, common5);

  Bar::fstatic();
  bar1.fstatic();
  (&bar2)->fstatic();

  Bar::fstatic1();
  bar3.fstatic1();
  (&bar3)->fstatic1();

  int zz = foo1.z;
  zz = common1.z;

  fff(bar1, &bar2);
  bool b;
  b = 168;
  b += 168;
  zz = b;

  int x_from_INT = INT();
}

int Bar::fstatic() { return 168; }

namespace xxx {
struct S {
  int x;
  S() : x(168) {}
};

int x;
void fy() {}
}

namespace yyy {
  int y;
  void fy() {}
}

using namespace yyy;

#pragma ACCEL kernel
void ng() {
//  xxx::S s;
  xxx::x = 0;
  y = 168;
  fy();
  xxx::fy();
  yyy::fy();
  int x;
  (const int &)x;
}
