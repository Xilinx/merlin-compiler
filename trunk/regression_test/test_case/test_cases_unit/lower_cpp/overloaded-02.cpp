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
// From string-plus-int-f.cpp from LLVM's test-suite. Here, we have a pair of
// operator functions that are also overloads of each other and we want to
// ensure that they are not double-lowered by operator and overload lowering.

void consume(const char *c) {}
enum OperatorOverloadEnum { kMyOperatorOverloadedEnum = 5 };
const char *operator+(const char *c, enum OperatorOverloadEnum e) {
    return "yo";
}
const char *operator+(enum OperatorOverloadEnum e, const char *c) {
    return "yo";
}
#pragma ACCEL kernel name = "f"
void f(int index) {
    consume("foo" + kMyOperatorOverloadedEnum);
    consume(kMyOperatorOverloadedEnum + "pfoo");
}
