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
// ifelse

#pragma ACCEL kernel
void func_top_6() {
  int a[100][100];
  int b[100][100];
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 100; j++) {
#pragma ACCEL PARALLEL FACTOR = 10

      if (a[0][0]) {
        a[i][j] = b[i][j];
      }
    }
  }
}

#pragma ACCEL kernel
void func_top_6_1(int cond) {
  int a[100][100];
  int b[100][100];
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 100; j++) {
#pragma ACCEL PARALLEL FACTOR = 10
      if (cond) {
        a[i][j] = b[i][j];
      }
    }
  }
}

#pragma ACCEL kernel
void func_top_7() {
  int a[100][100];
  int b[100][100];
  int i;
  int j;
  for (i = 0; i < 100; i++) {
#pragma ACCEL PIPELINE II = 1
    if (a[0][0]) {
      for (j = 0; j < 100; j++) {

#pragma ACCEL PARALLEL complete

        a[i][j] = b[i][j];
      }
    }
  }
}

#pragma ACCEL kernel
void func_top_8(int cond) {
  int a[100][100];
  int b[100][100];
  int i;
  int j;
  for (i = 0; i < 100; i++) {
#pragma ACCEL PIPELINE II = 1
    if (cond) {
      for (j = 0; j < 100; j++) {
#pragma ACCEL PARALLEL complete

        a[i][j] = b[i][j];
      }
    }
  }
}
// sub function

void sub_0(int a[100][100], int b[100][100]) {
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    if (a[0][0]) {
      for (j = 0; j < 100; j++) {
#pragma ACCEL PARALLEL FACTOR = 10
        a[i][j] = b[i][j];
      }
    }
  }
}

#pragma ACCEL kernel
void func_top_9() {
  int a[100][100];
  int b[100][100];
  sub_0(a, b);
}

#pragma ACCEL kernel
void func_top_10(int *min) {
  int dst[1024];
  int n_sub_0;
  for (n_sub_0 = 0; n_sub_0 < 64; ++n_sub_0) {
#pragma ACCEL PARALLEL
    *min = dst[n_sub_0];
    if (n_sub_0 > 0) {
      if (*min <= dst[-1 + n_sub_0]) {
        *min = *min;
      } else {
        *min = dst[-1 + n_sub_0];
      }
    }
    if (n_sub_0 < 63) {
      if (*min <= dst[1 + n_sub_0]) {
        *min = *min;
      } else {
        *min = dst[1 + n_sub_0];
      }
    }
  }
}

#pragma ACCEL kernel
void func_top_11(int qlen) {
  int eh_e[32][2];
  int eh_h[64];
  int j, j_sub;

  for (j = 0; j < 18; j++) {
#pragma ACCEL PIPELINE AUTO
    for (j_sub = 0; j_sub < 2; ++j_sub) {     
#pragma ACCEL PARALLEL
      if (j * 2 + j_sub < qlen) {
        int h;
        int M = eh_h[j * 2 + j_sub];
        int e = eh_e[j][j_sub];
        if (M > e) {
          h = M;
        }
        else {
          h = e;
        }
        eh_e[j][j_sub] = h;
      }
    }
  }

}

#pragma ACCEL kernel
void func_top_12(int var) {
  int a[100][64];
  int b[100][64];
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 8; j++) {
#pragma ACCEL PARALLEL 
      if (j < var) {
        a[i][8*j] = b[i][8*j];
      }
    }
  }
}
