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
// Ensure that calls to default constructors (implicit or otherwise) are
// lowered.

struct S {
    unsigned u;
    unsigned f() {}
};

struct T {
    unsigned u;
    unsigned f() {}
};

struct U {
    T t;
    U() : t() {}
};

void f(unsigned) {}

// We use tfn insts to create SgConstructorInitializer calls to builtin types.
template <typename T> T invoke_implicit_ctor() { return T(); }

// We use tfn insts to create SgConstructorInitializer calls to builtin types.
template <typename T> T invoke_copy_ctor(T t) { return T(t); }

struct Unlowered {};

#pragma ACCEL kernel
void kernel1() {
    // Implicit call to implicit default ctor. Nothing to lower.
    S r;

    // Explicit call to implict default ctor. This needs to be lowered despite
    // no explicit decl/def of default ctor.
    f(S().f());

    invoke_implicit_ctor<Unlowered>();
    invoke_implicit_ctor<unsigned>();

    invoke_copy_ctor<Unlowered>(Unlowered());
    invoke_copy_ctor<unsigned>(3);
}
