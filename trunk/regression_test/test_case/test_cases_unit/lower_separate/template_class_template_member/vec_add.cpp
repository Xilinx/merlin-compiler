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
#define SIZE 10000
#include "ap_int.h"

template <int SIZE2>
class test_class {
public:
void test3(ap_uint<32> a[SIZE2], int b[SIZE2]);
template<int SIZE3>
void test4(ap_uint<32> a[SIZE2], int b[SIZE2]);
};

template<int SIZE2>
void test_class<SIZE2>::test3(ap_uint<32> a[100], int b[100]) {
    for(int i=0; i<100; i++) {
        b[i] = a[i];
    }
}

template<int SIZE2>
template<int SIZE3>
void test_class<SIZE2>::test4(ap_uint<32> a[100], int b[100]) {
    for(int i=0; i<100; i++) {
        b[i] = a[i];
    }
}

template <int SIZE2>
void test2(ap_uint<32> a[SIZE2], int b[SIZE2]) {
    for(int i=0; i<SIZE2; i++) {
        b[i] = a[i];
    }
}
template <int SIZE2>
void test1(ap_uint<32> a[SIZE2], int b[SIZE2]) {
    test2<100>(a , b);
}

#pragma ACCEL kernel
void test(ap_uint<32> a[SIZE], int b[SIZE]) {
#pragma HLS INTERFACE m_axi port=a offset=slave bundle=gmem1
#pragma HLS INTERFACE m_axi port=b offset=slave bundle=gmem2
#pragma HLS INTERFACE s_axilite port=a  bundle=control
#pragma HLS INTERFACE s_axilite port=b  bundle=control
#pragma HLS INTERFACE s_axilite port=return   bundle=control
    test1<100>(a , b);
    test_class<100> new_class;
    new_class.test3(a, b);
    new_class.test4<100>(a, b);
}
