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
// We test LowerCpp's error reporting by intentionally providing it with C++
// constructs that are not supported, and rigging the test runner to record the
// error messages in this file's .expected counterpart. Note that this file is
// not exhaustive -- as new constructs are encountered (typically in bug
// reports) and classified as unsupported, they should be included here.

namespace stuff {
void f() {}
} // namespace stuff

struct S {
    static int d_c;
    S() {}
    ~S() {
      d_c++;
    }
    template <typename G> void f(G) {}

    static void g(unsigned) {}

    unsigned h() { return 0; }
};
int S::d_c = 0;
void g() {
    try {
    } catch (...) {
    }
}

// Don't lower reserved classes.
template <unsigned N> struct ap_uint { unsigned vec[N]; };

template <unsigned N> struct TemplateClass { unsigned vec[N]; };

// Bit-fields are currently unsupported.
struct BitFielded {
    unsigned : 127;
};

struct VirtualFns {
    unsigned u;
    virtual void f() {}
};

// No inheritance permitted.
struct Derived : public VirtualFns {};

// Delegated ctors are a C++11 feature. Officially, LowerCpp does C++03.
struct Delegated {
    Delegated() {}
    Delegated(unsigned) : Delegated() {}
};

// Nested aggs that require lowering are unsupported for now.
void f() {
    struct Nested {
        void f() {}
    };
    Nested n;
}

struct BadOperators {
    static void *operator new(unsigned long);
    static void *operator new[](unsigned long);
    static void operator delete(void *, unsigned long);
    static void operator delete[](void *, unsigned long);
};

struct RefMem {
    unsigned &u;
};

S manyesses[] = {S(), S(), S()};

#pragma ACCEL kernel
void top() {
    stuff::f();
    S s;
    g();
    ap_uint<12> a;
    TemplateClass<12> tc;
    new int[42];
    BitFielded bf;
    VirtualFns vfn;

    Derived der;

    Delegated del;

    f();

    struct {
        unsigned u;
    } inlinestructdef;

    struct {
        void f() {}
    } inlinecppdef;

    BadOperators *t = new BadOperators;
    delete t;
    BadOperators *v = new BadOperators[32];
    delete[] v;

    // Cause RefMem to be lowered.
    RefMem *r = 0;

    // Cause the global to be inspected by the checker.
    manyesses[0];
    // Switch statement conditionals are SgStatements, not SgExpressions. As a
    // result, if we do a parent-ward walk from the SgExpression that we need to
    // lower, we will not know for sure whether the SgSwitchStatement or its
    // enclosing scope is the correct node to insert before.
    switch (S().h()) {}
    switch (s.h()) {}
}
