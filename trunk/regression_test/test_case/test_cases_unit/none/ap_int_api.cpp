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
#include<ap_int.h>
#pragma ACCEL kernel
void top(ap_int<10> *a, ap_uint<20> *b, ap_fixed<10,20, AP_TRN, AP_WRAP, 10> *c, ap_ufixed<10, 5, AP_TRN, AP_WRAP, 5> * d) {
    a[0] = 0;
    b[0] = 0;
    c[0] = 0;
    d[0] = 0;
    a[1](0, 1) = 2;
    a[2].range(0, 1) = 3;
    b[1] = b[2].range();
    b[2] = c[0].range();
    for (ap_int<4> i = 0; i < 4; ++i) {}
    for (ap_int<4> i = 0; i < 4; i--) {}
    for (ap_uint<4> i = 0; i < 4; i += 1) {}
    ap_int<4> index = 0;
    a[index++] = 0;
    a[index--] = 0;
    a[++index] = 0;
    a[--index] = 0;
    c[0] = c[1] + c[2];
    c[0] = a[0] + b[0];
    c[1] = a[1] + c[1];
    c[2] = c[2] + d[2];
    c[0] >>= 2;
    a[0] >>= 1;
    b[0] >>= 2;
    d[0] >>= 3;
    d[0] <<= 1;
    a[0] <<= 1;
    a[0] = b[0] > d[1];
    c[1][1] = b[0].range(0,0);
    a[0][2] = c[1].range(1,1);
    a[0] = 1.0f;
    a[1] = 2.0;
    b[0] = 1.0f;
    b[1] = 2.0;
    c[0] = 1.0f;
    c[1] = 2.0;
    d[0] = 1.0f;
    d[1] = 2.0;
    switch(a[0]) {
      case 1: break;
      default: break;
    }

    switch (b[2]) {
      case 2: break;
      default: break;
    }
    const ap_int<10> c_a[4] = {0, 1, 2, 3};
    const ap_ufixed<10, 1> c_c[4] = {0, 1, 2, 3.2};
    unsigned int s_i;
    s_i * c_a[0];
    a[0] * s_i;
    c[1] | c[2];
    c[1] ^ c[2];
    a[0] = 2 ? a[0].range() : 124;
    a[0] |= a[1];
    a[0] %= a[1];
    a[0] ^= a[1];
    a[0] &= a[1];

    ~a[0][0];
    a[0] = a[0][0] + 1;
    a[0] += b[0][0];
    b[1][0] = (~b[0][0] || false) ? b[0][0] : true;
    d[1][0] = (~d[0][0] || false) ? d[0][0] : true;
    a[0] = c[0][0];
    b[0] = d[0][1];
    a[0] = b[0][1];
    b[0] = a[0][2];
    a[0] = c[0];
    b[0] = d[0];
    c[0] = a[0];
    d[0] = b[0];
}
