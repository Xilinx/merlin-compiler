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
template<int N>
void f() {
  for (int i = 0; i < N; ++i) {}
}
#pragma ACCEL kernel 
void vec_add_kernel() { 
  f<sizeof(ap_int<3>)>();
  f<sizeof(ap_int<8>)>();
  f<sizeof(ap_int<9>)>();
  f<sizeof(ap_int<15>)>();
  f<sizeof(ap_int<16>)>();
  f<sizeof(ap_int<18>)>();
  f<sizeof(ap_int<32>)>();
  f<sizeof(ap_int<33>)>();
  f<sizeof(ap_int<64>)>();
  f<sizeof(ap_int<65>)>();
  f<sizeof(ap_int<128>)>();
  f<sizeof(ap_int<127>)>();
  f<sizeof(ap_int<255>)>();
  f<sizeof(ap_int<256>)>();
  f<sizeof(ap_int<511>)>();
  f<sizeof(ap_int<512>)>();
  f<sizeof(ap_int<513>)>();
  f<sizeof(ap_uint<3>)>();
  f<sizeof(ap_uint<8>)>();
  f<sizeof(ap_uint<9>)>();
  f<sizeof(ap_uint<15>)>();
  f<sizeof(ap_uint<16>)>();
  f<sizeof(ap_uint<18>)>();
  f<sizeof(ap_uint<32>)>();
  f<sizeof(ap_uint<33>)>();
  f<sizeof(ap_uint<64>)>();
  f<sizeof(ap_uint<65>)>();
  f<sizeof(ap_uint<128>)>();
  f<sizeof(ap_uint<127>)>();
  f<sizeof(ap_uint<255>)>();
  f<sizeof(ap_uint<256>)>();
  f<sizeof(ap_uint<511>)>();
  f<sizeof(ap_uint<512>)>();
  f<sizeof(ap_uint<513>)>();
  f<sizeof(ap_fixed<3, 5>)>();
  f<sizeof(ap_fixed<8, 5>)>();
  f<sizeof(ap_fixed<9, 5>)>();
  f<sizeof(ap_fixed<15, 30>)>();
  f<sizeof(ap_fixed<16, 30>)>();
  f<sizeof(ap_fixed<18, 30>)>();
  f<sizeof(ap_fixed<32, 30>)>();
  f<sizeof(ap_fixed<33, 30>)>();
  f<sizeof(ap_fixed<64, 30>)>();
  f<sizeof(ap_fixed<65, 30>)>();
  f<sizeof(ap_fixed<128, 12>)>();
  f<sizeof(ap_fixed<127, 12>)>();
  f<sizeof(ap_fixed<255, 12>)>();
  f<sizeof(ap_fixed<256, 12>)>();
  f<sizeof(ap_fixed<511, 12>)>();
  f<sizeof(ap_fixed<512, 11>)>();
  f<sizeof(ap_fixed<513, 10>)>();
  f<sizeof(ap_ufixed<3, 5>)>();
  f<sizeof(ap_ufixed<8, 5>)>();
  f<sizeof(ap_ufixed<9, 5>)>();
  f<sizeof(ap_ufixed<15, 30>)>();
  f<sizeof(ap_ufixed<16, 30>)>();
  f<sizeof(ap_ufixed<18, 30>)>();
  f<sizeof(ap_ufixed<32, 30>)>();
  f<sizeof(ap_ufixed<33, 30>)>();
  f<sizeof(ap_ufixed<64, 30>)>();
  f<sizeof(ap_ufixed<65, 30>)>();
  f<sizeof(ap_ufixed<128, 12>)>();
  f<sizeof(ap_ufixed<127, 12>)>();
  f<sizeof(ap_ufixed<255, 12>)>();
  f<sizeof(ap_ufixed<256, 12>)>();
  f<sizeof(ap_ufixed<511, 12>)>();
  f<sizeof(ap_ufixed<512, 11>)>();
  f<sizeof(ap_ufixed<513, 10>)>();
}
