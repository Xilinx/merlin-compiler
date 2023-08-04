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
// This file is meant to test LowerCpp's implementation of Itanium C++ mangling.

// Even though we don't lower ap_uint (or any of its friends), it could still
// show up as the type of a function parameter and thus needs to be handled in
// the mangler. See `templated_fn` and the `falcon` overload below.
#include "ap_int.h"
#include <stdarg.h>

bool falcon(unsigned (*)(char, bool[10]), long (*)[10]) { return true; }
bool falcon(long (*)[10 + 24]) { return true; }
bool falcon() { return true; }
bool falcon(ap_uint<512>) { return true; }

void zero_params() {}
void zero_params(bool) {}

void falcon(unsigned (*zero_params)()) {}

// Ensure completeness of the mangler's leaf type handling.
void all_the_types(signed short, signed int, signed long, signed long long) {}
void all_the_types() {}

template <typename T> void templated_fn(T) {}

// Test _Complex mangling.
void falcon(float _Complex, double _Complex) {}

// Test va_list mangling.
void falcon(unsigned, va_list, char) {}

#pragma ACCEL kernel
void top() {
    falcon(0, 0);
    falcon(static_cast<long(*)[34]>(0));
    falcon();
    zero_params();
    zero_params(false);
    falcon(static_cast<unsigned (*)()>(0));
    all_the_types();
    all_the_types(0, 0, 0, 0);
    templated_fn(ap_uint<512>());
    falcon(ap_uint<512>());
    templated_fn(ap_ufixed<32, 16, AP_TRN>());

    float _Complex fcomp = 23;
    falcon(fcomp, 0);

    falcon(0, 0, 'z');
}
// TODO: namespaced and member fns.
