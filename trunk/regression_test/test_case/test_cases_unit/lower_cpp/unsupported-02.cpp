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
// Check that we say no to member pointer access operators. See below.

struct A {
    int a();
};
#pragma ACCEL kernel name = "a"
void a() {
    struct A x;
    x.a();
    (x.*&A::a)();
    //^^ LowerCpp completely does not expect this. We're not sure how to lower
    // it either.
}

// Check that non-C-like enums are rejected.
namespace nested {
enum E { A, B, C };
}

enum struct E2 { A, B, C };

#pragma ACCEL kernel
void kernb() {
    nested::E e;
    E2 e2;
}
