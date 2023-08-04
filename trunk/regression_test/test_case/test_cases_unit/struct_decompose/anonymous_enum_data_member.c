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
#include<stdlib.h>
#include "t.h"
#define N 100
const st g[10];
st g_2[10];
st *g_3;
#pragma ACCEL kernel
void top(st *a) {
#pragma ACCEL interface variable=a depth=100
  for (int i = 0; i < N; ++i)
    a[i].b = a[i].a + a[i].c;
  a[0] = g[0];
  a[1] = g_2[1];
  a[2] = g_3[2];
  return;
}

int main() {
  st *a;
  a = (st*) malloc(sizeof(st) * N);
  for (int i = 0; i < N; ++i) {
    a[i].a = i;
    a[i].c = INT;
  }
  top(a);
  return 0;

}
