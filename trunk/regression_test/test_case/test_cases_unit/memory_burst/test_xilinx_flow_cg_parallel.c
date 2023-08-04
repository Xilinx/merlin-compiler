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
#include "cmost.h"
#include <assert.h>
#pragma ACCEL kernel
void top(int a[100], int b[10]) {
  int i;
  for (i = 0; i < 10; ++i) {
#pragma ACCEL parallel
    for (int j = 0; j < 10; ++j) {
      a[i * 10 + j ] = b[j];
    }
  }
  return;
}

#pragma ACCEL kernel
void top2(int a[1000000], int b[10]) {
  int i, j;
  for (i = 0; i < 100000; ++i) {
#pragma ACCEL parallel
    for (j = 0; j < 10; ++j) {
      a[i * 10 + j] = b[j];
    }
  }
  return;
}

#pragma ACCEL kernel
void top3(int a[1000000], int b[10]) {
  int i, j, k;
  for (k = 0; k < 10; ++k) {
    for (i = 0; i < 100; ++i) {
#pragma ACCEL parallel
      for (j = 0; j < 10; ++j) {
        a[i * 10 + j] = b[j] + 1;
      }
    }
    for (i = 0; i < 10; ++i) {
      a[i + 1000] = b[i] + 2;
    }
  }
  return;
}

#pragma ACCEL kernel
void top4(int a[100], int b[10], int size) {
  int i;
  assert(size < 10);
  for (i = 0; i < 10; ++i) {
#pragma ACCEL parallel
    for (int j = 0; j < size; ++j) {
      a[i * 10 + j ] = b[j];
    }
  }
  return;
}


