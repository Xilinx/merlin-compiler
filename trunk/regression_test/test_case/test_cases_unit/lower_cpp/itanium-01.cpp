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
// Operator function mangling

namespace n {
struct T {};
} // namespace n

struct S {
    void operator+() {}
    void operator-() {}
    void operator&() {}
    void operator*() {}
    void operator~() {}
    void operator+(int) {}
    void operator-(int) {}
    void operator*(int) {}
    void operator/(int) {}
    void operator%(int) {}
    void operator&(int) {}
    void operator|(int) {}
    void operator^(int) {}
    void operator=(int) {}
    void operator+=(int) {}
    void operator-=(int) {}
    void operator*=(int) {}
    void operator/=(int) {}
    void operator%=(int) {}
    void operator&=(int) {}
    void operator|=(int) {}
    void operator^=(int) {}
    void operator<<(int) {}
    void operator>>(int) {}
    void operator<<=(int) {}
    void operator>>=(int) {}
    void operator==(int) {}
    void operator!=(int) {}
    void operator<(int) {}
    void operator>(int) {}
    void operator<=(int) {}
    void operator>=(int) {}
    void operator!() {}
    void operator&&(int) {}
    void operator||(int) {}
    void operator++(int) {}
    void operator++() {}
    void operator--(int) {}
    void operator--() {}
    void operator,(int) {}
    void operator->*(int) {}
    void operator->() {}
    void operator()(int) {}
    void operator()() {}
    void operator[](int) {}

    // operator cast
    operator n::T();
    operator double();

    void operatornotreally() {}
};

struct R {};
void operator<<(R, R) {}

void operator~(R) {}

void operator++(R, int) {}

#pragma ACCEL kernel
void kernel() {
    S s;
    s(2);
    double d = s;
    n::T t = s;
    s.operatornotreally();
    R() << R();
    ~R();
    R()++;
}

// Copied from the Itanium mangling spec.
// ::= nw    # new
// ::= na    # new[]
// ::= dl    # delete
// ::= da    # delete[]
//
// ::= ps    # + (unary)
// ::= ng    # - (unary)
// ::= ad    # & (unary)
// ::= de    # * (unary)
// ::= co    # ~
// ::= pl    # +
// ::= mi    # -
// ::= ml    # *
// ::= dv    # /
// ::= rm    # %
// ::= an    # &
// ::= or    # |
// ::= eo    # ^
// ::= aS    # =
// ::= pL    # +=
// ::= mI    # -=
// ::= mL    # *=
// ::= dV    # /=
// ::= rM    # %=
// ::= aN    # &=
// ::= oR    # |=
// ::= eO    # ^=
// ::= ls    # <<
// ::= rs    # >>
// ::= lS    # <<=
// ::= rS    # >>=
// ::= eq    # ==
// ::= ne    # !=
// ::= lt    # <
// ::= gt    # >
// ::= le    # <=
// ::= ge    # >=
// ::= nt    # !
// ::= aa    # &&
// ::= oo    # ||
// ::= pp    # ++ (postfix in <expression> context)
// ::= mm    # -- (postfix in <expression> context)
// ::= cm    # ,
// ::= pm    # ->*
// ::= pt    # ->
// ::= cl    # ()
// ::= ix    # []
// ::= qu    # ?
//             ^ what is this?!
// ::= cv <type>    # (cast)
// ::= li <source-name>          # operator ""
// ::= v <digit> <source-name>    # vendor extended operator
