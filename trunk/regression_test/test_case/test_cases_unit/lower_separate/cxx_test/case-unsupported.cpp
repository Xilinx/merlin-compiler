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
#include <stddef.h>
#include <stdio.h>
struct Foo {
 class Inner {
   Inner() {}
   Foo f() { return Foo(); }
 };
 enum Color { red, green, blue };
 int x : 3;
};

struct X {
  ~X() {printf("hello");}
};

struct Child : X {
  int x = 168;
};

struct A {
  int &x;
  int y;
  A() : x(y) {}
  void *operator new(size_t i) {return (void *)0;}
  void operator delete(void *p) {}
};

struct V {
  virtual void f() {};
  template <typename T> T id(T t) {return t;}
};

#pragma ACCEL kernel
void g(struct Foo foo) {
 class InFunc {
   InFunc() {}
   Foo f() { return Foo(); }
 };
 X x;
 Child child;
 struct {int x; int f() {return x+1;}} s; 
 A a;
 V v;
}

struct SSS {
  int x = 168;
  int y;
  SSS() {}
  SSS(int yy) : SSS() { y = yy; }
};

namespace {
#pragma ACCEL kernel
void ng() {
  SSS s(8);
}
}
