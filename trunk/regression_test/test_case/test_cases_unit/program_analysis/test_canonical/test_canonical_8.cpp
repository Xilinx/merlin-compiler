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
// Testing labels

#pragma ACCEL kernel
void func_loop() {
  int a[101];
  int b[101];
  int c[101];

  int i = 0;
  int j = 0;
  int k = 0;
  int x, y, z;
  int N = 100;

l1:
  for (char ic = N; ic >= 0; ic--) {
  l2:
    for (char jc = ic; jc < 100; jc += 2) {
      /* block 1*/
      c[ic] = a[ic] + b[jc];
    }
  }

l3:
  for (char ic = N; ic >= 0; ic--) {
    int cc = 10;
  l4:
    for (char jc = (char)cc - 1; jc < 100; jc += 2) {
      /* block 2*/
      c[ic] = a[ic] + b[jc];
    }
  }
l5:
  for (char ic = N; ic >= 0; ic--) {
    int cc = c[0];
  l6:
    for (char jc = cc; jc < 100; jc += 2) {
      /* block 3*/
      c[ic] = a[ic] + b[jc];
    }
  }
  {

  l7:
    for (char ic = N; ic >= 0; ic--) {
    l8:
      for (char jc = ic; jc < 100; jc += 2) {
        /* block 4*/
        c[ic] = a[ic] + b[jc];
      }
    }
  }
}

int main() {
  int aa[100][100];
  int bb[100][100];
  int cc[100][100];
  int *a, *b, *c;
  int a_s, b_s, c_s, d_s;
  float aa_f[100][100];
  float bb_f[100][100];
  float cc_f[100][100];
  float *a_f, *b_f, *c_f;
  float a_s_f, b_s_f, c_s_f, d_s_f;

  func_loop();

  return 0;
}
