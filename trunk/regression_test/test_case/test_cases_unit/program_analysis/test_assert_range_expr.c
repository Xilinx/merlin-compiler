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
#include <assert.h>
#pragma ACCEL kernel
void foo(int count, unsigned long count2) {
  int a[10];
  assert(a[2] < 10);
  int b[100];
  for (int i = 0; i < a[2]; ++i) {
    i;
  }
  for (int ii = 0; ii < a[2] / 3; ++ii) {
    ii;
    4 * ii;
    4 * ii + 2;
  }
  assert(count < 20);
  for (int i0 = 0; i0 < count / 3; ++i0) {
    i0;
    4 * i0;
    4 * i0 + 2;
  }
  assert(count2 < 20);
  for (int i1 = 0; i1 < count / 3; ++i1) {
    i1;
    4 * i1;
    4 * i1 + 2;
  }
  assert(a[2] - a[3] < 10);
  for (int j = 0; j < a[2] - a[3]; ++j) {
    j;
  }
  assert(a[3] + a[2] < 10);
  for (int k = 0; k < a[2] + a[3]; ++k) {
    k;
  }
  assert(count * count > 10);
  for (int k0 = count * count; k0 < 100; ++k0) {
    k0;
  }
  for (int k1 = count * count - 2; k1 < a[2] + 10; ++k1) {
    k1;
  }
  return;
}
