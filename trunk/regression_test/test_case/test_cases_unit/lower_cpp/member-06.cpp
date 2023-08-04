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
// Don't lower C-like aggregates.

struct A {
    unsigned u;
};

typedef struct {
    bool b;
} B;

class C {
  public:
    char *c;
};

class D {
  public:
    char *c;
    static bool b;
};

B untouched(C c) {
    B ret;
    ret.b = c.c != 0;
    return ret;
}

struct S;

struct S {
    void f() {}
};

#pragma ACCEL kernel
void top() {
    A a;
    C c;
    B b = untouched(c);
    D d;
    S s;
}
