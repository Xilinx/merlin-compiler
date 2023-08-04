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
// Test static storage variable lowering. Static storage duration are variables
// that are allocated at program start and de-allocated at program end.
// Informally, they are a generalization of global variables:
//
// - (Namespace-)global variables
// - Static class member vars

namespace {
unsigned u = 23;
}

namespace nested {
float f;
}

struct S {
    static bool b;
    const static int y = 10;
};

template <int N>
class A {
  public:
  static bool a;
};

bool S::b = false;

#pragma ACCEL kernel
void kernel() {
    nested::f = 23.0f;
    u <<= 9 + S::b + S::y + A<1>::a;
}
