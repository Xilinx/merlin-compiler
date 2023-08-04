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
class A;

struct {
  int a;
  short b;
}g;

struct B {
  struct {
	int c;
    char d;
  }d;
};
struct C {
  static int a;
};

struct D {
  int :2;
  int a;
};

union E {
  int a;
  float b;
};

struct F {
#pragma ACCEL attribute variable=a depth=1
  void *a;
};

struct H {
  union E e;
};

void sub(A *a) {}

#pragma ACCEL kernel
void top (A *a, B * b, C *c, D *d, union E *e, F * f, H *h) { 
#pragma ACCEL interface variable=a depth=1
#pragma ACCEL interface variable=b depth=1
#pragma ACCEL interface variable=c depth=1
#pragma ACCEL interface variable=d depth=1
#pragma ACCEL interface variable=e depth=1
#pragma ACCEL interface variable=f depth=1
#pragma ACCEL interface variable=h depth=1
  sub(a); g.a; b[0].d.c++; C::a++; d[0].a++; e[0].b = 0.0; *(int *)(f->a) = 0;
  h->e.b = 1.0;}
