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
#include<hls_stream.h>
#pragma ACCEL kernel
void func_top_0(int *b, hls::stream<int> &c)
{
  hls::stream<int> a;
  for (int i = 0; i < 8; ++i) {
    a.write(i);
  }
  for (int i = 0; i < 8; ++i) {
    b[i] = c.read();
  }
  for (int i = 0; i < 8; ++i) {
    a << i;
  }
  for (int i = 0; i < 8; ++i) {
    c >> b[i];
  }
  hls::stream<int> arr[8];
  for (int i = 0; i < 8; ++i) {
    arr[i].write(i);
  }
  // trip count = 1
  hls::stream<int> arr2[1];
  for (int k = 0; k < 1; ++k) {
    arr2[k].write(k);
  }
}

void sub(hls::stream<int> &a, hls::stream<int> &b) {
  a << b.read();
}
#pragma ACCEL kernel
void func_top_1(hls::stream<int> a[8], hls::stream<int> b[8]) {
  for (int i = 0; i < 8; ++i) {
    sub(a[i], b[i]);
  }
}

void sub_1(hls::stream<int> a[8], hls::stream<int> b[8]) {
  for (int i = 0; i < 8; ++i) {
    a[i].write(b[i].read());
  }
}
#pragma ACCEL kernel
void func_top_2(hls::stream<int> a[4][8], hls::stream<int> b[4][8]) {
  for (int i = 0; i < 4; ++i) {
    sub_1(a[i], b[i]);
  }
}
