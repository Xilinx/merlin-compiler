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
typedef struct {int a;} s_a;
typedef struct s_b {int b;};
void f() {
    for (int i = 0; i < 10; ++i) {
    }
}
void f(bool a) {
    int b;
    f();
}
void f(bool a, unsigned b) {
    f(a);
    f();
}

void f(s_a a) {
}

void f(s_b b) {
}

// Notice that since only one of the overloads are used by the kernel, mangling
// is not required.
#pragma ACCEL kernel
int main() {
    bool a;
    unsigned b;
    s_a sa;
    s_b sb;
    f(sa);
    f(sb);
    f(a, b);
}
