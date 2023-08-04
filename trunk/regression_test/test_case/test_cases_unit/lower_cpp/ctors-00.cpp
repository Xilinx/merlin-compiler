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
// Constructor tests.

// Inline ctor decls.
struct A {
    A() {}
};

// Initializer lists.
struct B {
    unsigned u;
    B() : u(21) {}
    // Overloaded.
    B(unsigned u) : u(u) {}
    B(unsigned u_, bool) { u = u_ + 42; }
};

// Field initializers. Summary of semantics:
// http://www.stroustrup.com/C++11FAQ.html#member-init
struct C {
    unsigned u;
    char bob; // This becomes part of each ctor init list
    C(unsigned u) : u(u) { bob += 1; }
    C() : u(42) { bob += 1; }
    // Overrides the field initializer.
    C(bool b) : u(168), bob(b) { bob += 1; }
};

// Out-of-line decls.
struct D {
    unsigned u;
    D();
    D(unsigned u);
    void f() {}
};

D::D() { u = 12; }
D::D(unsigned u) : u(u) { f(); }

// Recursive ctor calls.
struct E {
    D d;
    E(unsigned u) : d(u) { d.f(); }
};

extern "C" int puts(const char *);

// Const member init.
struct F {
    unsigned u;
    const char m_c;
    const char *p;
    const char * const c_p;
    E e;
    // Note that our lowering strategy prevents RVO from happening.
    F(char c) : m_c(c), p(&m_c), c_p(&m_c), e(c) { puts("F constructed.\n"); }
    void f(const char *c) {}
    void call() {
      f(p);
    }
    void ff(const char *const c_p) {}
    void call_c() {
      ff(c_p);
    }
    // TODO: Support placement new. See below.
    // static void *operator new(unsigned long, void *ptr) { return ptr; }
};

struct G {};

F make_f() {
    G g;
    D d;
    E e(46);
    return F(94);
}

// TODO: Support placement new expressions.
// void placement_new() {
//    static char space[256];
//    F *f = new (space) F(29);
// }

// Anonymous decls.
unsigned anonymous_decl() { return E(23).d.u; }

#pragma ACCEL kernel
int main() {
    A a;
    B b;
    C c;
    make_f();
    anonymous_decl();
}
