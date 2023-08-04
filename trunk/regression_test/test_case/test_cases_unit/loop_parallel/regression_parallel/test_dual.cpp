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
// 1. loop structure
// 2. function structure
// 3. access pattern
// 4. pragma modes

#pragma ACCEL kernel
void func_top_0() {
  int a[100];
  int b[100];
  int i;
  for (i = 0; i < 100; i++)
  // Original: #pragma ACCEL pipeline_parallel factor = 10
  {

#pragma ACCEL PARALLEL FACTOR = 10

    a[i] = b[i];
  }
}

#pragma ACCEL kernel
void func_top_1() {
  int a[100][100];
  int b[100][100];
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 100; j++)
    // Original: #pragma ACCEL pipeline_parallel factor = 10
    {

#pragma ACCEL PARALLEL FACTOR = 10

      a[i][j] = b[i][j];
    }
  }
}

#pragma ACCEL kernel
void func_top_3(int *a, int *b) {
  int i;
  int j;
  for (j = 0; j < 100; j++) {
#pragma ACCEL PARALLEL FACTOR = 10

    a[j] = b[j];
  }
}

// Loop structure

#pragma ACCEL kernel
void func_top_4() {
  int a[101][101];
  int b[101][101];
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 100; j++) {

#pragma ACCEL PARALLEL FACTOR = 10

      a[i][j] = b[i][j];
    }
    for (j = 0; j < 100; j++) {

#pragma ACCEL PARALLEL FACTOR = 10

      a[i + 1][j] = b[i][j + 1];
    }
  }
}

#pragma ACCEL kernel
void func_top_5() {
  int a[102][102];
  int b[102][102];
  int i = 0;
  int j = 0;
  for (j = 0; j < 100; j++)
  // Original: #pragma ACCEL pipeline_parallel factor = 10
  {

#pragma ACCEL PARALLEL FACTOR = 10

    a[i + 1][j] = b[i][j + 1];
  }
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 100; j++)
    // Original: #pragma ACCEL pipeline_parallel factor = 10
    {

#pragma ACCEL PARALLEL FACTOR = 10

      a[i][j] = b[i][j];
    }
    for (j = 0; j < 100; j++) {
      a[i + 2][j] = b[i + 2][j];
    }
    for (j = 0; j < 100; j++) {

#pragma ACCEL PARALLEL FACTOR = 10

      a[i + 1][j] = b[i][j + 1];
    }
  }
  for (j = 0; j < 100; j++) {

#pragma ACCEL PARALLEL FACTOR = 10

    a[i + 1][j] = b[i][j + 1];
  }
}
int sub_0(int a[100], int b[100][100], int c) {
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 100; j++)
    // Original: #pragma ACCEL pipeline_parallel factor = 10
    {

#pragma ACCEL PARALLEL FACTOR = 10

      a[i] = b[i][j] + c;
    }
  }
  return 1024;
}

#pragma ACCEL kernel
void func_top_10() {
  int a[100][100];
  int b[100][100];
  int c = 1;
  int sum = sub_0(a[0], b, c);
}

void sub_1(int a[100][100], int b[100][100]) {
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    if (a[0][0]) {
      for (j = 0; j < 100; j++)
      // Original: #pragma ACCEL pipeline_parallel factor = 10
      {

#pragma ACCEL PARALLEL FACTOR = 10

        a[i][j] = b[i][j];
      }
    }
  }
}

#pragma ACCEL kernel
void func_top_11() {
  int a[100][100];
  int b[100][100];
  sub_1(a, b);
  sub_1(a, b);
}

int sub_2(int b[100][100], int i, int j) { return b[i][j] + b[i][j + 1]; }

int sub_00(int a[100], int b[100][100], int c) {
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 99; j++)
    // Original: #pragma ACCEL pipeline_parallel factor = 10
    {

#pragma ACCEL PARALLEL FACTOR = 10

      a[i] = sub_2(b, i, j);
    }
  }
  return 1024;
}

#pragma ACCEL kernel
void func_top_12() {
  int a[100][100];
  int b[100][100];
  int c = 1;
  int sum = sub_00(a[0], b, c);
}
// Acccess pattern

#pragma ACCEL kernel
void func_top_13() {
  int a[100][100];
  int b[100][100];
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 100; j++)
    // Original: #pragma ACCEL pipeline_parallel factor = 10
    {

#pragma ACCEL PARALLEL FACTOR = 10

      a[i][j] = b[i][j];
    }
  }
}

#pragma ACCEL kernel
void func_top_14() {
  int a[100][100];
  int b[101][10];
  int i;
  int j;
  for (i = 1; i < 100; i++) {
    for (j = 0; j < 10; j++)
    // Original: #pragma ACCEL pipeline_parallel factor = 10
    {

#pragma ACCEL PARALLEL FACTOR = 10

      a[i][j] = b[i][j] + b[i + 1][j] + b[i - 1][j];
    }
  }
}

#pragma ACCEL kernel
void func_top_15() {
  int a[101][100];
  int b[101][100];
  int i;
  int j;
  for (i = 1; i < 50; i++) {
    for (j = 0; j < 100; j++)
    // Original: #pragma ACCEL pipeline_parallel factor = 10
    {

#pragma ACCEL PARALLEL FACTOR = 10

      a[i][j] = b[2 * i][j] + b[2 * i + 1][j] + b[2 * i - 1][j];
    }
  }
}

#pragma ACCEL kernel
void func_top_16() {
  int a[10][100];
  int b[200][100];
  int i;
  int j;
  for (i = 1; i < 10; i++) {
    for (j = 0; j < 100; j++)
    // Original: #pragma ACCEL pipeline_parallel factor = 10
    {

#pragma ACCEL PARALLEL FACTOR = 10

      a[i][j] = b[16 * i][j] + b[16 * i + 1][j] + b[16 * i - 1][j];
    }
  }
}

#pragma ACCEL kernel
void func_top_17() {
  int a[100][100];
  int b[100][100];
  int i;
  int j;
  for (i = 0; i < 1; i++) {
    for (j = 0; j < 100; j++)
    // Original: #pragma ACCEL pipeline_parallel factor = 10
    {

#pragma ACCEL PARALLEL FACTOR = 10

      a[i][j] =
          b[10587946 * i][j] + b[10587946 * i + 1][j] + b[10587946 * i + 2][j];
    }
  }
}

#pragma ACCEL kernel
void func_top_18() {
  int a[100][100];
  int b[100][100];
  int i;
  int j;
  for (i = 0; i < 1; i++) {
    for (j = 0; j < 100; j++)
    // Original: #pragma ACCEL pipeline_parallel factor = 10
    {

#pragma ACCEL PARALLEL FACTOR = 10

      a[i][j] =
          b[10587946 * i][j] + b[35935221 * i + 1][j] + b[43453045 * i][j];
    }
  }
}

#pragma ACCEL kernel
void func_top_20() {
  int a[100][100];
  int b[100][100];
  int i;
  int j;
  for (i = 0; i < 50; i++) {
    for (j = 0; j < 100; j++)
    // Original: #pragma ACCEL pipeline_parallel factor = 10
    {

#pragma ACCEL PARALLEL FACTOR = 10

      a[i][j] = b[i][j] + b[i + 1][j] + b[i + 2][j] + b[i + 3][j] +
                b[i + 4][j] + b[i + 5][j] + b[i + 6][j] + b[i + 7][j] +
                b[i + 8][j] + b[i + 9][j] + b[i + 10][j] + b[i + 11][j] +
                b[i + 12][j] + b[i + 13][j] + b[i + 14][j] + b[i + 15][j] +
                b[i + 16][j] + b[i + 17][j] + b[i + 18][j] + b[i + 19][j] +
                b[i + 20][j] + b[i + 21][j] + b[i + 22][j] + b[i + 23][j] +
                b[i + 24][j] + b[i + 25][j] + b[i + 26][j] + b[i + 27][j] +
                b[i + 28][j] + b[i + 29][j] + b[i + 30][j];
    }
  }
}

#pragma ACCEL kernel
void func_top_21() {
  int a[100][100];
  int b[100][100];
  int i;
  int j;
  for (i = 0; i < 1; i++) {
    for (j = 0; j < 100; j++)
    // Original: #pragma ACCEL pipeline_parallel factor = 10
    {

#pragma ACCEL PARALLEL FACTOR = 10

      a[i][j] =
          b[10587946 * i][j] + b[-10587946 * i][j] + b[43453045 * i + 1][j];
    }
  }
}

#pragma ACCEL kernel
void func_top_22() {
  int a[100][100];
  int b[100][100];
  int i;
  int j;
  for (i = 1; i < 10; i++) {
    for (j = 0; j < 100; j++)
    // Original: #pragma ACCEL pipeline_parallel factor = 10
    {

#pragma ACCEL PARALLEL FACTOR = 10

      a[i][j] = b[3 * i][j] + b[2 * i][j] + b[1 * i - 1][j];
    }
  }
}

#pragma ACCEL kernel
void func_top_23() {
  int a[100][100];
  int b[100][100];
  int i;
  int j;
  for (i = 1; i < 10; i++) {
    for (j = 0; j < 100; j++)
    // Original: #pragma ACCEL pipeline_parallel factor = 10
    {

#pragma ACCEL PARALLEL FACTOR = 10

      a[i][j] = b[3 * i][j] + b[2 * i][j] + b[1 * i - 1][j];
    }
  }
  for (i = 1; i < 10; i++) {
    for (j = 0; j < 100; j++)
    // Original: #pragma ACCEL pipeline_parallel factor = 10
    {

#pragma ACCEL PARALLEL FACTOR = 10

      a[i][j] = b[3 * i][j] + b[2 * i][j] + b[1 * i - 1][j];
    }
  }
}

#pragma ACCEL kernel
void func_top_24() {
  int a[101][101];
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 100; j++)
    // Original: #pragma ACCEL pipeline_parallel factor = 10
    {

#pragma ACCEL PARALLEL FACTOR = 10

      a[i][j] += a[i][j] * a[i+1][j+1];
    }
  }
}

#pragma ACCEL kernel
void func_top_25() {
  int a[100][100];
  int b[101][101];
  int i;
  int j;
  for (i = 1; i < 100; i++) {
#pragma ACCEL PIPELINE
    for (j = 1; j < 100; j++)
    // Original: #pragma ACCEL pipeline_parallel complete
    {

#pragma ACCEL PARALLEL COMPLETE

      a[i][j] = b[i + 1][j + 1] * b[i - 1][j - 1];
    }
  }
}
// pragma modes

#pragma ACCEL kernel
void func_top_26() {
  int a[101][100];
  int i;
  int j;
  for (i = 1; i < 100; i++) {
    for (j = 0; j < 100; j++)
    // Original: #pragma ACCEL pipeline
    {

#pragma ACCEL PIPELINE II = 1
      a[i][j] += a[i + 1][j] * a[i - 1][j];
    }
  }
}
// pragma modes

#pragma ACCEL kernel
void func_top_27() {
  int a[100][100];
  int i;
  int j;
  for (i = 1; i < 99; i++) {
    for (j = 0; j < 100; j++)
    // Original: #pragma ACCEL parallel factor = 10
    {

#pragma ACCEL PARALLEL FACTOR = 10
      a[i][j] += a[i + 1][j] * a[i - 1][j];
    }
  }
}
// pragma modes

#pragma ACCEL kernel
void func_top_28() {
  int a[100][100];
  int b[100][100];
  int i;
  int j;
  for (i = 1; i < 99; i++) {
    for (j = 0; j < 100; j++)
    // Original: #pragma ACCEL parallel factor=complete
    {

      a[i][j] += a[i + 1][j] * a[i - 1][j];
    }
  }
}
// pragma modes

#pragma ACCEL kernel
void func_top_29() {
  int a[100][100];
  int b[100][100];
  int i;
  int j;
  for (i = 1; i < 99; i++) {
    for (j = 0; j < 100; j++)
    // Original: #pragma ACCEL parallel complete
    {

#pragma ACCEL PARALLEL COMPLETE
      a[i][j] += a[i + 1][j] * a[i - 1][j];
    }
  }
}
// pragma modes

#pragma ACCEL kernel
void func_top_30() {
  int a[100][100];
  int b[100][100];
  int i;
  int j;
  for (i = 1; i < 99; i++) {
    for (j = 0; j < 100; j++) {
#pragma ACCEL PIPELINE II = 1 PRIORITY = logic
      a[i][j] = b[i - 1][j] + b[i + 1][j] + b[i][j];
    }
  }
}
// test priority

#pragma ACCEL kernel
void func_top_31() {
  int a[100][100];
  int b[100][100];
  int i;
  int j;
  for (i = 1; i < 99; i++) {
    for (j = 0; j < 100; j++) {

#pragma ACCEL PARALLEL FACTOR = 4 PRIORITY = logic

      a[i][j] = b[i - 1][j] + b[i + 1][j] + b[i][j];
    }
  }
}

#pragma ACCEL kernel
void func_top_36() {
  int a[100];
  int b[100];
  int i;
  for (i = 0; i < 100; i++)
  // Original: #pragma ACCEL pipeline_parallel factor = 1000
  {

#pragma ACCEL PARALLEL FACTOR = 1000

    a[i] = b[i];
  }
}

#pragma ACCEL kernel
void func_top_37() {
  int a[100];
  int b[100];
  int i;
  for (i = 0; i < 100; i++)
  // Original: #pragma ACCEL pipeline_parallel factor = 100
  {

#pragma ACCEL PARALLEL FACTOR = 100

    a[i] = b[i];
  }
}

#pragma ACCEL kernel
void func_top_38() {
  int a[100][100];
  int b[100][100];
  int i;
  int j;
  for (i = 0; i < 100; i++)
  // Original: #pragma ACCEL pipeline
  {

#pragma ACCEL PIPELINE
    for (j = 0; j < 100; j++)
    // Original: #pragma ACCEL parallel complete
    {

#pragma ACCEL PARALLEL COMPLETE
      a[i][j] = b[i][j];
    }
  }
}

#pragma ACCEL kernel
void func_top_39() {
  int a[100][100];
  int b[100][100];
  int i;
  int j;
  for (i = 0; i < 100; i++)
  // Original: #pragma ACCEL pipeline
  {

#pragma ACCEL PIPELINE II = 1
    for (j = 0; j < 100; j++)
    // Original: #pragma ACCEL parallel factor=10
    {

#pragma ACCEL PARALLEL FACTOR = 10
      a[i][j] = b[i][j];
    }
  }
}

#pragma ACCEL kernel
void func_top_41() {
  int a[100][100];
  int b[100][100];
  int i;
  int j;
  int k;
  for (k = 0; k < 100; k++)
  // Original: #pragma ACCEL pipeline
  {

#pragma ACCEL PIPELINE II = 1
    for (i = 0; i < 100; i++) {
      for (j = 0; j < 100; j++) {
        a[i][j] = b[i][j];
      }
    }
  }
}

#pragma ACCEL kernel
void func_top_42() {
  int a[100][100];
  int b[100][100];
  int i;
  int j;
  int k;
  for (k = 0; k < 100; k++)
  // Original: #pragma ACCEL parallel factor = 10
  {

#pragma ACCEL PARALLEL FACTOR = 10
    for (i = 0; i < 100; i++) {
      for (j = 0; j < 100; j++) {
        a[i][j] = b[i][j];
      }
    }
  }
}

#pragma ACCEL kernel
void func_top_43() {
  int a[100];
  int b[100];
  int i;
  for (i = 0; i < 100; i++) {
    b[i] = i;
  }
  for (i = 0; i < 100; i++)
  // Original: #pragma ACCEL pipeline_parallel factor = 10
  {

#pragma ACCEL PARALLEL FACTOR = 10
    a[i] = b[i];
  }
}

#pragma ACCEL kernel
void func_top_44() {
  int a[100][100];
  int b[100][100];
  int i;
  int j;
  for (i = 1; i < 10; i++) {
    for (j = 0; j < 100; j++) {

#pragma ACCEL PARALLEL FACTOR = 10

      a[i][j] = b[3 * i][j] + b[2 * i][j] + b[1 * i - 1][j];
    }
  }
  for (i = 1; i < 10; i++) {
    int k;
    for (j = 0; j < 10; j++) {
#pragma ACCEL PARALLEL FACTOR = 10

      a[i][j] = b[3 * i][i + j] + b[2 * i][i + j] + b[1 * i - 1][i + j + 1];
    }
  }
}

#pragma ACCEL kernel
void func_top_45(int A_out[5], int k) {
  int A[5][5][5][5][5];
  int i0;
  int i1;
  int i2;
  int i3;
  int i4;
  int j;
  //	for( i0 = 0 ; i0 <  n-1; i0++){
  for (i1 = 0; i1 < 5 - 1; i1++) {
    for (i2 = 0; i2 < 5 - 1; i2++) {
      for (i3 = 0; i3 < 5 - 1; i3++) {
        for (i4 = 0; i4 < 5 - 1; i4++) {
          for (j = 0; j < 5 - 1; j++) {
            A[i1][i2][i3][i4][j] = j;
          }
        }
      }
    }
  }
  /*Array Access*/
  //    for( i0 = 0 ; i0 <  n-1; i0++){
  for (i1 = 0; i1 < 5 - 1; i1++) {
    for (i2 = 0; i2 < 5 - 1; i2++) {
      for (i3 = 0; i3 < 5 - 1; i3++) {
        for (i4 = 0; i4 < 5 - 1; i4++) {
          for (j = 0; j < 5 - 1; j++)
          // Original: #pragma ACCEL parallel factor=4
          {

#pragma ACCEL PARALLEL FACTOR = 4
            int r0 = A[i1][i2][i3][i4][j];
            A_out[j] = r0 / 8;
          }
        }
      }
    }
  }
}

void sub_46(int a[100][100]) {
  int b[100][100];
  int i;
  int j;
  int k;
  for (i = 0; i < 10; i++) {
    for (j = 0; j < 10; j++) {
      int var1 = i + 1;
      int var0 = i;
      for (k = 0; k < 10; k++)
      // Original: #pragma ACCEL pipeline_parallel factor=2
      {

#pragma ACCEL PARALLEL FACTOR = 2

#pragma ACCEL PIPELINE II = 1
        a[var1][k] += b[var1][j + k];
        a[var0][k] += b[var0][j + k];
      }
    }
  }
}

#pragma ACCEL kernel
void func_top_46() {
  int aa[100][100];
  sub_46(aa);
}
