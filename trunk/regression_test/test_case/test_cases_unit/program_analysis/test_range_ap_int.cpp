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
void top(int *a, int *b, ap_int<10> *c, ap_uint<10> *d, int *e, int *f, int *g) {
  for (ap_int<4> i = 0; i < 10; ++i) {
    a[100 - 2 * i] = i;
  }
  ap_uint<5> j;
  for (j = 0; j < 10; j+=1) {
    b[j * 2 + 1] = j;
  }
  for (ap_int<5> k = 1; k < 10; k = k + 1) {
    c[k * 2 + 3] = k;
  }
  for (ap_int<5> m = 10; m >= 0; m = m - 1) {
    d[m * 4 + 2] = m;
  }
  ap_int<10> bound = 5;
  for (int i0 = 0; i0 < bound; ++i0) {
    e[i0] = i0;
  }
  bound = 10;
  for (ap_int<10> i1 = 0; i1 < bound + 1; ++i1) {
    f[i1] = i1;
  }
  for (ap_int<10> i = 0; i < 100; ++i) {
    for (ap_int<10> j = 0; j < i / 2; ++j) {
      g[j] = 0;
    }
  }
}
