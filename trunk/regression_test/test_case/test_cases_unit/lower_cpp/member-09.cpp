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
// Static member function tests.

struct S {
    static unsigned f(unsigned) { return 23; }
    static void g(unsigned, S *this_) {}
    void h(unsigned) {}

    // Out-of-line definition.
    static void j();
    static int i;
    void inc() {
      this->i++;
      this->f(12);
    }
};

void S::j() {}

#pragma ACCEL kernel
int main() {
    S s;
    S::j();
    s.h(32);
    s.g(32, &s);
    S::g(32, &s);
    S *sptr;
    sptr->f(32);
    sptr->h(32);
    return S::f(32);
}
