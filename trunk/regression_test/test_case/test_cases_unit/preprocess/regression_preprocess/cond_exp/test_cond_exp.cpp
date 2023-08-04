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
#define MIN(a, b) (a > b) ? (b) : (a)
#define MAX(a, b) (a > b) ? (a) : (b)
int sub(int i) {
  return i > 0 ?  1 : 0;
}
typedef ap_int<10> int10_t;
#pragma ACCEL kernel
void test(double b[10]) {
  int a[100];
  int aa[100][50];
  int i;

  for (i = MIN(b[0], b[1]); i < MAX(b[0], b[1]); ++i) {
    a[i] = b[i] > 0 ? b[i] : -b[i];
  }

  int &a_r = i > 0 ? a[0] : a[1];
  const int a_c = i > 0 ? a[0] : a[1];
  const int *a_ptr = i > 0 ? a : aa[0];
  (i > 0 ? a[0] : a[1]) = 1;
  int ret = sub(i);
  while (i > 0 ? a[0] : a[1]) {}
  do {
  } while (i < 10 ? a[0] : (i > 5 ? a[1] : a[2]));
  if (i < 9 ? a[0] : a[1]) {}
  switch (i > 5 ? i : i + 10) {
	case 1: break;
	default: break;
  }

  int10_t a0[10];
  a0[1] = i > 0 ? a0[0] : ap_int<10>("100", 10);
  static int s_a = i > 0 ? 0 : 1;
  const int c_i = 10;
  static const int s_c_a = 1 >> (c_i > 10 ? 1 : 2);
  static const int s_c_a_a[2] = { 1 >> (c_i > 5 ? 1 : 2), 2 << (c_i > 2 ? 2 : 3)};
}

