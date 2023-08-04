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
// From bug2158. As a result of both the operator cast to const ref and the copy
// constructor call, ROSE generates a renegade SgConstructorInitializer node:
//
// SgPragmaDeclaration:#pragma ACCEL kernel name = "g"
//         ptr 0x7fe1b0755010
// SgPragma:#pragma ACCEL kernel name = "g"
//         ptr 0xf9cf60
// SgFunctionDeclaration:void g(struct ::Foo foo) {...}
//         ptr 0x7fe1b0ba4c08
// SgFunctionParameterList:struct ::Foo foo
//         ptr 0x7fe1b0cdebe0
// SgInitializedName:foo
//         ptr 0x7fe1b0c8b060
// SgFunctionDefinition:void g(struct Foo foo){f(Bar());}
//         ptr 0x7fe1b06c5010
// SgBasicBlock:{f(Bar());}
//         ptr 0x7fe1b0710010
// SgExprStatement:f(Bar());
//         ptr 0x10625e0
// SgFunctionCallExp:f(Bar())
//         ptr 0x1046f00
// SgFunctionRefExp:f
//         ptr 0xfba440
// SgExprListExp:(Bar())
//         ptr 0xfd5b08
// SgConstructorInitializer:(Bar())       <=== problematic.
//         ptr 0x100c570
// SgExprListExp:Bar()
//         ptr 0xfd59d0
// SgFunctionCallExp:Bar()
//         ptr 0x1046e90
// SgDotExp:Bar() . operator const Foo &
//         ptr 0x102b900
// SgConstructorInitializer:Bar()
//         ptr 0x100c4f0
// SgExprListExp:
//         ptr 0xfd5a38
// SgMemberFunctionRefExp:operator const Foo &
//         ptr 0xfef020
// SgExprListExp:
//         ptr 0xfd5aa0
//
// So we attempt to prevent this situation by rejecting operator casts to const
// refs.

struct Foo {
    Foo(const struct Foo &);
};

struct Bar {
    operator const Foo &() const;
};
void f(struct Foo);
#pragma ACCEL kernel name = "g"
void g(struct Foo foo) { f(Bar()); }
