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
#include "test_delinear.h"
#include <assert.h>
#include <string.h>

void test6_2_sub(int **p)
{
    return ;
}

#pragma ACCEL kernel
void test6_2() 
{
    int a[100][100];
    for (int i = 0; i < 25; ++i) {
        for (int j = 0; j < 4; ++j) {
            for (int k = 0; k < 25; ++k) {
                for (int l = 0; l < 4; ++l) {
                    a[i * 4 + j][k * 4 + l] = i * i + j * j + k * k + l * l;
                }
            }
        }
    }

    test6_2_sub((int **)a);
}

#pragma ACCEL kernel
void test6_1(int *b) {
  int a0[1024];
  int a0_1[1024];
  int a1[1024];
  int a2[1024];
  int a3[1024];
  int a4[1024];
  int a5[1024];
  int a6[1024];
  int a7[1024][1024];
  int a8[1024][1024];
  int a9[1024];
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 4; j += 2)
      for (int k = 0; k < 8; k++)
        for (int l = 0; l < 4; l += 3) {
          memcpy(a0, b, 500);
          memset(a0_1, 0, 500);
          memcpy(b, a1, 500);

          memcpy(a1, b, sizeof(int) * (4 * i + j));
          memcpy(a2 + 32 * i, b, sizeof(int) * j);
          memcpy(a3 + 32 * i, b, sizeof(int) * 8);
          memcpy(a4 + 8 * i, b, sizeof(int) * (j + 8));
          memcpy(a5 + 8 * i, b, sizeof(int) * (j + 7));
          memcpy(a6 + 8 * i, b, sizeof(int) * (j + 9));

          memcpy(a7[4 * i + j] + 8 * i, b, sizeof(int) * (j + 4));
          memcpy(a8[4 * i + j], b, sizeof(int) * (j + 4));

          memcpy(a9 + 8 * i, b, sizeof(int) * 8);
          a9[16 * i + 8 * j + 4];
        }
}

#pragma ACCEL kernel
void test6_1_1(int * b)
{
  int a[1024 - 20];
  int a1[16][1024 - 20];
  int a2[16][1024 - 20][20];
  int a3[1024 - 20][16];
  int a4[1024 - 20][16];

  for (int i = 0; i < 8; i++)
  for (int j = 0; j < 4; j+=2)
  for (int k = 0; k < 8; k++)
  for (int l = 0; l < 4; l+=3)
  {
    a[ 64 * i + k]; // support
    memcpy(a, b, 500);

    a1[0][ 64 * i + k];
    memcpy(a1[0], b, 5000); // not supported

    a2[0][ 64 * i + k][0];
    memcpy(b, a2[0][0], 5000); // not supported

    a3[ 64 * i + k][0]; // support
    memcpy(a3[0], b, 5000);

    a4[ 81 * i + 9*j + k][0]; // support
    memcpy(a4[0], b, 5000);
  }
}

#pragma ACCEL kernel
void test6_2(int * b)
{
    int a0[1024];
    int a1[1024];
    for (int i = 0; i < 8; i++)
    for (int j = 0; j < 4; j+=2)
    for (int k = 0; k < 8; k++)
    for (int l = 0; l < 4; l+=3)
    {
        memcpy(a0, b, 500);
        a0[32*i + j];
        memset(a1, 0, 500);
        a1[32*i + j];
    }
}

void test6_3_sub(int *a);

void test6_3_sub(int *a) {
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 4; j++)
      a[j + i * 16];
}

void test6_3_sub_1(int *a) {
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 4; j++)
      a[j + i * 16];
}

void test6_3_sub_2(int *a) {
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 4; j++)
      a[j + i * 16];
}

#pragma ACCEL kernel
void test6_3(int t) {
  t >= 0 && t < 4 ? ((void)0)
                  : __assert_fail("t>= 0 && t< 4", "test_delinear.cpp", 344,
                                  __PRETTY_FUNCTION__);
  int a[1024];
  test6_3_sub(a);

  int a1[1024];
  test6_3_sub_1(a1);

  int a2[1024][1024];
  test6_3_sub_2(a2[32 * t]);
}

void test6_3_sub(int *a);

#pragma ACCEL kernel
void test6_4(int *a) {
  int b[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 2; j++)
      for (int k = 0; k < 2; k++)
        for (int l = 0; l < 2; l++) {
          b[i * 8 + j * 4 + k * 2 + l];
        }
}

#pragma ACCEL kernel
void test6_5() {
  int a1[1024];
  int t = a1[8];
  t >= 0 && t < 8 ? ((void)0)
                  : __assert_fail("t>= 0 && t< 8", "test_delinear.cpp", 344,
                                  __PRETTY_FUNCTION__);
  for (int i = 0; i <= t; i++) {
    a1[64 + i];
    int p = a1[64];
    p >= 0 && p < 8 ? ((void)0)
                    : __assert_fail("p>= 0 && p< 8", "test_delinear.cpp", 344,
                                    __PRETTY_FUNCTION__);
    for (int j = 0; j <= p; j++) {
      a1[64 * j + i];
    }
  }
}

#pragma ACCEL kernel
void test6_5_1() {
  int a1[1024];
  int t = a1[8];
  t >= 0 && t < 8 ? ((void)0)
                  : __assert_fail("t>= 0 && t< 8", "test_delinear.cpp", 344,
                                  __PRETTY_FUNCTION__);
  for (int i = 0; i <= t; i++) {
    a1[64 + t];
    int p = a1[64];
    p >= 0 && p < 8 ? ((void)0)
                    : __assert_fail("p>= 0 && p< 8", "test_delinear.cpp", 344,
                                    __PRETTY_FUNCTION__);
    for (int j = 0; j <= p; j++) {
      a1[64 * p + t];
    }
  }
}

#pragma ACCEL kernel
void test6_5_2() {
  int a1[1024];
  int a2[1024];
  int t = a1[8];
  t >= 0 && t < 8 ? ((void)0)
                  : __assert_fail("t>= 0 && t< 8", "test_delinear.cpp", 344,
                                  __PRETTY_FUNCTION__);
  for (int i = 0; i <= t; i++) {
    a1[64 + t];
    int p = a1[64];
    p >= 0 && p < 8 ? ((void)0)
                    : __assert_fail("p>= 0 && p< 8", "test_delinear.cpp", 344,
                                    __PRETTY_FUNCTION__);
    for (int j = 0; j <= p; j++) {
      a2[64 * p + t]; // the range here is not correct
    }
  }
}

#pragma ACCEL kernel
void test6_5_3() {
  int i, j;
  int a[1024];
  int b[1024];
  int c[1024];

  for (i = 0; i < 10; i++) {
    for (j = b[i]; j < 10; j++) {
      a[j];
      c[j];
    }
  }
  return;
}

void test5_0_sub(int *a, int *b) {
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 0; k < 8; k++)
        for (int l = 0; l < 4; l++) {
          a[(k + l * 8) * 32 + j + i * 4];
          b[(k + l * 8) * 32 + j + i * 4];
        }
}
void test5_0_sub1(int a[1024], int *b) {
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 0; k < 8; k++)
        for (int l = 0; l < 4; l++) {
          a[(k + l * 8) * 32 + j + i * 4];
          b[(k + l * 8) * 32 + j + i * 4];
        }
}

#pragma ACCEL kernel
void test5_0() {
  int a[1024];
  test5_0_sub(a, NULL);
  test5_0_sub1(a, NULL);
}

void test5_1_sub(int *a, int *b) {
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 0; k < 8; k++)
        for (int l = 0; l < 4; l++) {
          a[(k + l * 8) * 32 + j + i * 4];
          b[(k + l * 8) * 32 + j + i * 4];
        }
}

#pragma ACCEL kernel
void test5_1() {
  int a[1024];
  test5_1_sub(a, NULL);
  test5_1_sub(a, NULL);
}
#pragma ACCEL kernel
void test5_1_1() {
  int a[1024];
  test5_1_sub(a, NULL);
  test5_1_sub(NULL, a);
}

void test5_1_2_sub(int *a, int *b, int n) {
  __merlin_access_range(a, 0, 31);
  a[n];
  b[n];
}

#pragma ACCEL kernel
void test5_1_2(int n) {
  int a[1024]; // delinearized
  int b[1024]; // not deliearized
  for (int i = 0; i < 100; i++)
    test5_1_2_sub(a + 32 * i, b + 32 * i, n);
}

void test5_1_3_sub(int *a, int *b, int n) {
  __merlin_access_range(a, 0, 31);
  a[n];
  b[n];
}

#pragma ACCEL kernel
void test5_1_3(int n) {
  int a[1024]; // delinearized
  int b[1024]; // not deliearized
  for (int i = 0; i < 100; i++) {
    test5_1_3_sub(a + 32 * i, b + 32 * i, n);
    test5_1_3_sub(a + 32 * i, b + 32 * i, n);
  }
}

void test5_1_4_sub(int *a, int *b, int n) {
  __merlin_access_range(a, 0, 31);
  a[n];
  b[n];
}

#pragma ACCEL kernel
void test5_1_4(int n) {
  int a[1024]; // delinearized
  int b[1024]; // not deliearized
  for (int i = 0; i < 100; i++) {
    test5_1_4_sub(a + 32 * i, b + 32 * i, n);
    test5_1_4_sub(a + 32 * i + 32, b + 32 * i, n);
  }
}

void test5_2_sub(int *a, int *b) {
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 0; k < 8; k++)
        for (int l = 0; l < 4; l++) {
          a[(k + l * 8) * 32 + j + i * 4];
          b[(k + l * 8) * 32 + j + i * 4];
        }
}

#pragma ACCEL kernel
void test5_2() {
  int a[1024];
  test5_2_sub(a, a);
}

void test5_3_sub(int *a, int *b) {
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 0; k < 8; k++)
        for (int l = 0; l < 4; l++) {
          a[(k + l * 8) * 32 + j + i * 4];
          b[(k + l * 8) * 35 + j + i * 4];
        }
}

#pragma ACCEL kernel
void test5_3() {
  int a[1024];
  int b[1024];
  test5_3_sub(a, b);
}

void test5_4_sub(int *a, int *b) {
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 0; k < 8; k++)
        for (int l = 0; l < 4; l++) {
          a[(k + l * 8) * 32 + j + i * 4];
          b[(k + l * 8) * 35 + j + i * 4];
        }
}

#pragma ACCEL kernel
void test5_4() {
  int a[1024];
  int b[1024];
  test5_4_sub(a, NULL);
  test5_4_sub(b, NULL);
}

void test5_5_sub_sub(int *a, int *b) {
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 0; k < 8; k++)
        for (int l = 0; l < 4; l++) {
          a[(k + l * 8) * 32 + j + i * 4];
          b[(k + l * 8) * 35 + j + i * 4];
        }
}

void test5_5_sub(int *a, int *b_) {
  int b[1024];
  test5_5_sub_sub(a, b);
}

#pragma ACCEL kernel
void test5_5() {
  int a[1024];
  int b[1024];
  test5_5_sub(a, NULL);
}

void test5_6_sub_sub(int *a, int *b) {
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 0; k < 8; k++)
        for (int l = 0; l < 4; l++) {
          a[(k + l * 8) * 32 + j + i * 4];
          b[(k + l * 8) * 35 + j + i * 4];
        }
}

void test5_6_sub(int *a, int *b_) {
  int b[1024];
  test5_6_sub_sub(a, b);
}

#pragma ACCEL kernel
void test5_6() {
  int a[1024];
  int b[1024];
  test5_6_sub(a, NULL);
  test5_6_sub_sub(a, b);
}

void test4_0_sub(int *a) {
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 0; k < 8; k++)
        for (int l = 0; l < 4; l++)
          a[(k + l * 8) * 32 + j + i * 4];
}

#pragma ACCEL kernel
void test4_0() {
  int a[1024];
  test4_0_sub(a);
}

void test4_1_sub(int *a) {
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 0; k < 8; k++)
        for (int l = 0; l < 4; l++)
          a[j + i * 4];
}

#pragma ACCEL kernel
void test4_1() {
  int a[1024];
  test4_1_sub(a + 32);
}

void test4_1_1_sub(int a[16]) {
  __merlin_access_range(a, 0, 15);

  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 0; k < 8; k++)
        for (int l = 0; l < 4; l++)
          a[k % 16]; // range not determined statically
}

#pragma ACCEL kernel
void test4_1_1() {
  int a[1024];
  for (int i = 0; i < 8; i++)
    test4_1_1_sub(a + 16 * i);
}

void test4_1_2_sub1(int a[16], int k) {
  __merlin_access_range(a, 0, 15);
  a[k % 16]++; // range not determined statically
}
void test4_1_2_sub(int a[16]) {
  __merlin_access_range(a, 0, 15);

  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 0; k < 8; k++)
        for (int l = 0; l < 4; l++) {
          test4_1_2_sub1(a, k);
          test4_1_2_sub1(a, k);
        }
}

#pragma ACCEL kernel
void test4_1_2() {
  int a[1024];
  for (int i = 0; i < 8; i++)
    test4_1_2_sub(a + 16 * i);
}

void test4_1_3_sub(int a[16]) {
  __merlin_access_range(a, 0, 15);

  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 0; k < 8; k++)
        for (int l = 0; l < 4; l++) {
          a[k % 16]++;
        }
}

#pragma ACCEL kernel
void test4_1_3() {
  int a[1024];
  int b[1024];
  for (int i = 0; i < 8; i++) {
    test4_1_3_sub(a + 16 * i);
    test4_1_3_sub(a + 16 * i);
    test4_1_3_sub(b);
    test4_1_3_sub(b + 16 * i);
  }
}

void test4_1_4_sub(int a[16]) {
  __merlin_access_range(a, 0, 15);
  for (int k = 0; k < 8; k++) {
    a[k % 16]++;
  }
}

#pragma ACCEL kernel
void test4_1_4() {
  int a[1024];
  for (int i = 0; i < 8; i++) {
    test4_1_4_sub(a);
    test4_1_4_sub(a);

    a[0];
    a[1];
    a[3]; // old error: range in _sub was missing, so it split into [256][4]
  }
}

#pragma ACCEL kernel
void test4_2() {
  int test4_2_a1[1024];
  int test4_2_a2[1024];
  int test4_2_a3[1024];
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 0; k < 8; k++)
        for (int l = 0; l < 4; l++) {
          &test4_2_a1[(k + l * 8) * 32 + j + i * 4];
          test4_2_a2;
          test4_2_a2[(k + l * 8) * 32 + j + i * 4];
          &test4_2_a3;
          test4_2_a3[(k + l * 8) * 32 + j + i * 4];
        }
}

#pragma ACCEL kernel
void test4_3() {
  int test4_3_a1[8][1024][8];
  int test4_3_a2[8][1024][8];
  int test4_3_a3[8][1024][8];
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 0; k < 8; k++)
        for (int l = 0; l < 4; l++) {
          test4_3_a1[8][(k + l * 8) * 32 + j + i * 4];
          test4_3_a2[8][(k + l * 8) * 32 + j + i * 4][8];
          test4_3_a3[8][(k + l * 8) * 32 + j + i * 4];
          test4_3_a3[8][(k + l * 8) * 32 + j + i * 4][8];
        }
}

#pragma ACCEL kernel
void test4_4() {
  int a1[1024];
  int a2[1024];
  int a3[1024];

  int *a1_ = a1;
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 0; k < 8; k++)
        for (int l = 0; l < 4; l++) {
          a1_[(k + l * 8) * 32 + j + i * 4];
        }
}

#pragma ACCEL kernel
void test4_4_1_0() {
  int a2[1024];

  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 0; k < 8; k++)
        for (int l = 0; l < 4; l++) {

          // steps: 1, 4, 32
          // a2[32][8][4];
          int *a2_ = a2 + 32 * k; // int (* a2_)[4] = a2[k];

          a2_[j + i * 4];         // a2_[i][j]
          a2[32 * k + j + i * 4]; // a2[k][i][j]
        }
}

#pragma ACCEL kernel
void test4_4_1() {
  //#split_dim pos
  // This case tests the position of splitted dimensions between alias and
  // reference Considering the offsets which determines the level to split
  //
  // The split on the alias side and original array side are both supposed to be
  // applied
  int a1[1024];
  int a2[1024];
  int a3[1024];
  int a4[1024];
  int a5[1024];

  int *a1_ = a1;
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 0; k < 8; k++)
        for (int l = 0; l < 4; l++) {

          int *a2_ = a2 + 32 * k;
          int *a3_ = a3 + 32 * k + 4 * i;
          int *a4_ = a4 + 32 * k + j; // not apply, cross alias
          int *a5_ = a5 + 32 * k;     // empty alias

          a1_[(k + l * 8) * 32 + j + i * 4];
          a1[(k + l * 8) * 32 + i + j * 8];

          a2_[j + i * 4];
          a2[32 * k + j + i * 4];

          a3_[j];
          a4_[4 * i];

          a5[4 * i];
        }
}

#pragma ACCEL kernel
void test4_4_2_ssss(int t, int p) {
  int a2[1024];
  t >= 0 && t < 4 ? ((void)0)
                  : __assert_fail("t>= 0 && t< 4", "test_delinear.cpp", 344,
                                  __PRETTY_FUNCTION__);
  p >= 0 && p < 4 ? ((void)0)
                  : __assert_fail("p>= 0 && p< 4", "test_delinear.cpp", 344,
                                  __PRETTY_FUNCTION__);

  int(*a2_) = a2 + 32 * t;
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 0; k < 8; k++)
        for (int l = 0; l < 4; l++) {
          a2_[i];
        }
}

#pragma ACCEL kernel
void test4_4_2_sss(int t, int p) {
  int a2[1024][1024];
  t >= 0 && t < 4 ? ((void)0)
                  : __assert_fail("t>= 0 && t< 4", "test_delinear.cpp", 344,
                                  __PRETTY_FUNCTION__);
  p >= 0 && p < 4 ? ((void)0)
                  : __assert_fail("p>= 0 && p< 4", "test_delinear.cpp", 344,
                                  __PRETTY_FUNCTION__);

  int(*a2_)[1024] = a2 + 32 * t;
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 0; k < 8; k++)
        for (int l = 0; l < 4; l++) {
          a2_[i][j];
        }
}

#pragma ACCEL kernel
void test4_4_2_ss(int t, int p) {
  int a2[1024][1024];
  t >= 0 && t < 4 ? ((void)0)
                  : __assert_fail("t>= 0 && t< 4", "test_delinear.cpp", 344,
                                  __PRETTY_FUNCTION__);
  p >= 0 && p < 4 ? ((void)0)
                  : __assert_fail("p>= 0 && p< 4", "test_delinear.cpp", 344,
                                  __PRETTY_FUNCTION__);

  int(*a2_)[1024] = a2 + 32 * t;
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 0; k < 8; k++)
        for (int l = 0; l < 4; l++) {
          a2_[i][j + i * 4];
        }
}

#pragma ACCEL kernel
void test4_4_2(int t, int p) {
  //#alias dim #
  // This case tests the support of multi-d array with alias
  //     the alias can have different dims (less than the original array dim)
  // Only the specific dimension is delinearized

  int a1[1024][1024][1024];
  int a2[1024][1024][1024];
  int a3[1024][1024][1024];
  //    assert(t>=0&&t<4); // can also support assert, but the file line# will
  //    not change as below for regression test assert(p>=0&&p<4);
  t >= 0 && t < 4 ? ((void)0)
                  : __assert_fail("t>= 0 && t< 4", "test_delinear.cpp", 344,
                                  __PRETTY_FUNCTION__);
  p >= 0 && p < 4 ? ((void)0)
                  : __assert_fail("p>= 0 && p< 4", "test_delinear.cpp", 344,
                                  __PRETTY_FUNCTION__);

  int(*a1_)[1024][1024] = a1 + 32 * t;
  int(*a2_)[1024] = a2[4] + 32 * t;
  int *a3_ = a3[16 * t + 2][4 * t + p] + 4 * t;
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 0; k < 8; k++)
        for (int l = 0; l < 4; l++) {
          a1_[1][2][k * 32 + 4 * i + j];
          a2_[i][j + i * 4];
          a3_[j + 32 * k];
        }
}

#pragma ACCEL kernel
void test4_4_3(int t, int p) {
  //#alias type
  // Only the full array are being considered to be splitted
  // Only the m-D pointer to the array type can be a feasible alias of the array
  // to be split
  //      e.g. the original array has 5 dims
  //           an alias is a 2-D pointer to a 3-d array
  // Processing:
  //      When a dimension is split, if it is a pointer dim in alias,
  //           that dim of the alias expends into multiple pointer dims
  //      And if the split dim is an array dim in alias,
  //           that dim of the alias expends into mulitple array dims

  int a1[1024][1024][1024];
  int a2[1024][1024][1024];
  int a3[1024][1024][1024];
  int a4[1024][1024][1024];

  typedef int(**t_2dp)[1024];
  t >= 0 && t < 4 ? ((void)0)
                  : __assert_fail("t>= 0 && t< 4", "test_delinear.cpp", 344,
                                  __PRETTY_FUNCTION__);
  p >= 0 && p < 4 ? ((void)0)
                  : __assert_fail("p>= 0 && p< 4", "test_delinear.cpp", 344,
                                  __PRETTY_FUNCTION__);

  int(*a1_)[1024][1024] = a1;                 // a 1-d pointer to 2-d array
  int(**a2_)[1024] = (t_2dp)a2 + (4 * t + p); // a 2-d pointer to 1-d array
  int ***a3_ = (int ***)a3 + (4 * t + p);     // a 3-d pointer to int
  int **a4_ = (int **)a4[0];                  // a 2-d pointer to int

  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 0; k < 8; k++)
        for (int l = 0; l < 4; l++) {
          a1_[0][0][j + i * 4];
          a2_[0][0][j + i * 4];
          a3_[0][0][j + i * 4];
          a4_[0][j + i * 4];
        }
}

#pragma ACCEL kernel
void test4_4_4(int t, int p) {
  //#alias of alias

  int a1[1024][1024][1024];
  int a2[1024][1024][1024];
  int a3[1024][1024][1024];
  int a4[1024][1024][1024];
  t >= 0 && t < 4 ? ((void)0)
                  : __assert_fail("t>= 0 && t< 4", "test_delinear.cpp", 344,
                                  __PRETTY_FUNCTION__);
  p >= 0 && p < 4 ? ((void)0)
                  : __assert_fail("p>= 0 && p< 4", "test_delinear.cpp", 344,
                                  __PRETTY_FUNCTION__);

  // direct assignments on alias
  int(*a1_)[1024][1024] = a1;   // a 1-d pointer to 2-d array
  int(*a1__)[1024][1024] = a1_; // a 1-d pointer to 2-d array

  // type coverted from array to pointer (not apply)
  typedef int(**t_2dp)[1024];
  int(*a2_)[1024][1024] = a2;     // a 1-d pointer to 2-d array
  int(**a2__)[1024] = (t_2dp)a2_; // a 3-d pointer to 1-d array

  // dimension reduce
  int **a3_ = (int **)a3[16 * p];
  int *a3__ = a3_[4 * t];

  // split one dim
  int **a4_ = (int **)(a4[1] + 32 * 4 * t);
  int **a4__ = (int **)(a4[3] + 32 * 4 * t);
  int **a4___ = a4_ + 32 * p;

  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 0; k < 8; k++)
        for (int l = 0; l < 4; l++) {
          a1_[0][0][32 * k + 8 * j];
          a1__[0][0][32 * k + i * 4 + j];

          a2_[0][0][32 * k + 8 * j];
          a2__[0][0][32 * k + i * 4 + j];

          a3_[0][32 * k + 8 * j];
          a3__[32 * k + i * 4 + j];

          a4_[i * 4 + j][i * 4];
          a4__[i * 4 + j][i * 4 + j];
          a4___[i * 4 + j][i * 4 + j];
        }
}

#pragma ACCEL kernel
void test4_4_5(int t, int p) {
  // small coeff offset, which makes the separation between alias_exp and
  // alias_ref disabled

  t >= 0 && t < 4 ? ((void)0)
                  : __assert_fail("t>= 0 && t< 4", "test_delinear.cpp", 344,
                                  __PRETTY_FUNCTION__);
  p >= 0 && p < 4 ? ((void)0)
                  : __assert_fail("p>= 0 && p< 4", "test_delinear.cpp", 344,
                                  __PRETTY_FUNCTION__);

  int a1[1024];
  int a2[1024][1024];
  int a3[1024][1024];
  int a4[1024];
  int a5[1024];

  int *a1_ = a1 + 32 * p + 1;
  a1_[4 * t];

  int(*a2_)[1024] = a2 + 32 * p + 1;
  a2_[4 * t][0];

  int(*a3_) = a3[0] + 32 * p + 1;
  a3_[4 * t];

  int *a4_ = a4 + p;
  a4_[4 * t];

  int *a5_ = a5 + t;
  a5_[4 * t];
}

#pragma ACCEL kernel
void test4_4_6(int t, int p) {
  // offset in sub-dim, also disable

  t >= 0 && t < 4 ? ((void)0)
                  : __assert_fail("t>= 0 && t< 4", "test_delinear.cpp", 344,
                                  __PRETTY_FUNCTION__);
  p >= 0 && p < 4 ? ((void)0)
                  : __assert_fail("p>= 0 && p< 4", "test_delinear.cpp", 344,
                                  __PRETTY_FUNCTION__);

  int a1[1024][1024];
  int a2[1024][1024];

  int *a1_ = &(a1[0][1]);
  int **a1__ = &a1_;
  a1__[4 * t][4 * p];

  int *a2_ = &(a2[0][0]);
  int **a2__ = &a2_;
  a2__[4 * t][4 * p];
}

#pragma ACCEL kernel
void test4_4_7(int t, int p) {
  // offset in sub-dim, also disable

  t >= 0 && t < 4 ? ((void)0)
                  : __assert_fail("t>= 0 && t< 4", "test_delinear.cpp", 344,
                                  __PRETTY_FUNCTION__);
  p >= 0 && p < 4 ? ((void)0)
                  : __assert_fail("p>= 0 && p< 4", "test_delinear.cpp", 344,
                                  __PRETTY_FUNCTION__);

  int a1[1024][1024];
  int a2[1024][1024];
  int a3[1024][1024][1024];

  int(*a1_)[1024] = a1 + 4 * t;
  a1_[4][4 * t];
}

#pragma ACCEL kernel
void test4_4_8(int t, int p) {
  // offset in sub-dim, also disable

  t >= 0 && t < 4 ? ((void)0)
                  : __assert_fail("t>= 0 && t< 4", "test_delinear.cpp", 344,
                                  __PRETTY_FUNCTION__);
  p >= 0 && p < 4 ? ((void)0)
                  : __assert_fail("p>= 0 && p< 4", "test_delinear.cpp", 344,
                                  __PRETTY_FUNCTION__);

  int a1[1024][1024];
  int a2[1024];

  int(*a1_)[1024] = a1 + 4 * t;
  a1_[0][4 + t];

  int(*a2_) = a2 + 4 * t;
  a2_[0];

  int *a3_ = a2_;
  a3_[0];
}

void add_one(int *&a) { a++; }

//#pragma ACCEL kernel
void test4_5() {
  // This case is to test the detection of single-assignment alias
  int a1[1024];
  int a2[1024];
  int a2_1[1024];
  int a3[1024];
  int a4[1024];

  int *a1_ = a1;
  // a_ = a2; // can detected as non-single-assign
  a1_++; // can not detected as non-single-assign

  int *a2_ = a2;
  a2_ = a2_1; // re-assign to another variable

  int *a3_ = a3;
  a3_ = a3; // re-assign to the same variable

  int *a4_ = a4;
  add_one(a4_); // function call

  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 0; k < 8; k++)
        for (int l = 0; l < 4; l++) {
          a1_[(k + l * 8) * 32 + j + i * 4];
          a2_[(k + l * 8) * 32 + j + i * 4];
          a3_[(k + l * 8) * 32 + j + i * 4];
          a4_[(k + l * 8) * 32 + j + i * 4];
        }
}
#pragma ACCEL kernel
void test4_6() {
  int a4_6[1024];
  int a4_6_1[1024];

  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 4; j++)
      for (int m = 0; m < 4; m++)
        for (int k = 0; k < 8; k++)
          for (int l = 0; l < 4; l++) {
            a4_6[(k + l * 8) * 32 + j + i * 4];
            a4_6[(k + l * 8) * 32 + m + i * 4];
            a4_6_1[(k + l * 8) * 32 + j + i * 4]; // not match
            a4_6_1[(k + l * 8) * 32 + m + 1 + i * 4];
          }
}

#pragma ACCEL kernel
void test4_7() {
  int a[1024];
  int a1[1024];
  int a2[1024];
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 0; k < 8; k++)
        for (int l = 0; l < 4; l++)
          for (int m = 0; m < 2; m++) {
            // merge ranges in one dim
            a[i * 8 + j];
            a[i * 8 + 2 * l];
            a[i * 8 + 5];
            a[i * 8 + 11];

            // higher dim
            a1[k * 64 + i * 8 + j];
            a1[k * 64 + (2 * l + 1) * 8 + j];
            a1[k * 64 + (4 * m) * 8 + j];
            a1[k * 64 + i * 8 + j];

            // merge in multiple pos
            a2[k * 64 + i * 8 + l * 2];
            a2[k * 64 + (2 * l + 1) * 8 + j];
            a2[k * 64 + (11) * 8 + 4];
          }
}

#pragma ACCEL kernel
void test3_0() {
  int a[1024];
  int b[1024];
  int c[1024];
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 0; k < 8; k++)
        for (int l = 0; l < 4; l++) {
          a[(k + l * 8) * 32 + j +
            i * 4]; // 4 section, fully split     // correct
          b[(k + l * 2) * 18 + j +
            i * 2]; // 4 section, merge 1-2, 3-4  // correct
          c[(k + l * 2) * 16 + j +
            i * 4]; // 4 section, merge 2-4       // correct
        }
}

int test3_1_a_s;
#pragma ACCEL kernel
void test3_1_s(int b) {
  int a_l = test3_1_a_s;
  a_l >= 0 && a_l < 8 ? ((void)0)
                      : __assert_fail("a_l>=0 && a_l< 8", "test_delinear.cpp",
                                      344, __PRETTY_FUNCTION__);
  int a[1024];
  int a2[1024];
  for (int j = 0; j < 4; j++) {
    a[a_l * 4 + j]; // Here is a bug in defuse analysis: after transformation of
                    // this line, the def of the a_l in the following statement
                    // is none (even after init_defuse)
    a2[4 * a_l]; //     then it makes the range analysis fails for the reference
                 //     "a_l" in the second statement
                 // This bug is to be revisited later.
  }
}

int test3_1_a;
#pragma ACCEL kernel
void test3_1(int b) {
  int a_l = test3_1_a;
  // assert(a_l>=0 && a_l< 8);
  a_l >= 0 && a_l < 8 ? ((void)0)
                      : __assert_fail("a_l>=0 && a_l< 8", "test_delinear.cpp",
                                      344, __PRETTY_FUNCTION__);
  // assert(test3_1_a>=0 && test3_1_a< 8);
  test3_1_a >= 0 && test3_1_a < 8
      ? ((void)0)
      : __assert_fail("test3_1_a>=0 && test3_1_a< 8", "test_delinear.cpp", 345,
                      __PRETTY_FUNCTION__);
  // assert(b>=0 && b< 8);
  b >= 0 && b < 8 ? ((void)0)
                  : __assert_fail("b>=0 && b< 8", "test_delinear.cpp", 329,
                                  __PRETTY_FUNCTION__);
  int a[1024];
  int a1[1024];
  int a2[1024];
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 4; j += 2)
      for (int k = 0; k < 8; k++)
        for (int l = 0; l < 4; l += 3) {
          a[a_l * 4 + j];
          a1[test3_1_a * 4 +
             j]; // not split due to range analysis of global var
          a2[(b + l * 8) * 32 + j +
             a_l * 4]; // ZP: 20170316 ERROR: should be "[l][b][test3_1_a][j];"
                       //        FIXME: using the error output temporarily, to
                       //        be fixed finally later
        }
}

#pragma ACCEL kernel
void test3_2() {
  int a[1024];
  int a1[1024];
  int a2[1024];
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 0; k < 8; k++)
        for (int l = 0; l < 4; l++) {
          a[i * 4 + 8 * k];
          a1[i * 4 + 3 - j];
          a2[(7 - i) * 4 + j];
        }
}

#pragma ACCEL kernel
void test3_2_1() {
  int a[1024];
  int a1[1024];
  int a2[1024];
  int a3[1024];
  for (int i = 0; i < 8; i++)
    for (int j = 4; j < 8; j++)
      for (int k = 0; k < 8; k++)
        for (int l = -7; l < -3; l++)
          for (int l1 = -2; l1 < 0; l1++) {
            a[i * 4 + j];
            a1[(i * 4 + j) * 4];
            a2[k * 16 * 8 + 4 * l + j]; // two variable: applied
            a3[k * 16 * 8 + 4 * l1 +
               j]; // l1 cannot split because of the shift 1 from j
          }
}

#pragma ACCEL kernel
void test3_3() {
  int a[1024];
  int a1[1024];
  int a2[1024];
  int a3[1024];
  int a4[1024];
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 0; k < 8; k++)
        for (int l = 0; l < 4; l++) {
          a[i * 33 + 2 * k];
          a1[j * 1000 + i * 33 + 2 * k];
          a2[j * 9999 + i * 33 + 2 * k];
          a3[j * 9999 + i * 32 + 2 * k];
          a4[10000 * l + j * 9999 + i * 32 + 2 * k];
        }
}

#pragma ACCEL kernel
void test3_4() {
  int a[1024];
  int a1[1024];
  int a2[1024];
  int a3[1024];
  int b;
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 0; k < 8; k++)
        for (int l = 0; l < 4; l++) {
          a[b * 55 + i * 4 + j];
          a1[i * 4 + b];
          a2[4 * i * b + j];
          a3[i & 1];
        }
}

#pragma ACCEL kernel
void test3_5() {
  int a3_5[1024];
  int a3_5_1[1024];
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 0; k < 8; k++)
        for (int l = 0; l < 4; l++) {
          a3_5[(k + l * 8) * 32 + j + i * 4 +
               1]; // ERROR: j+1 may be out of bound
          a3_5_1[(k + l * 8) * 32 + j + i * 4 + 4]; // Correct: not applied
        }
}

#pragma ACCEL kernel
void test2() {
  int a[1024][1024];
  for (int i = 0; i < 32; i++)
    for (int j = 0; j < 32; j++)
      a[i * 32 + j][8];
}

#pragma ACCEL kernel
void test2_1() {
  int a[1024][1024];
  for (int i = 0; i < 32; i++)
    for (int j = 0; j < 32; j++)
      a[8][i * 32 + j];
}
#pragma ACCEL kernel
void test2_2() {
  int a[1024][1024];
  for (int i = 0; i < 32; i++)
    for (int j = 0; j < 32; j++)
      a[j * 32 + i][i * 32 + j];
}
#pragma ACCEL kernel
void test2_3() {
  int a[32][1024][1024];
  int b[1024][32][1024];
  int c[1024][1024][32];
  for (int i = 0; i < 32; i++)
    for (int j = 0; j < 32; j++) {
      a[i][j * 32 + i][i * 32 + j];
      b[j * 32 + i][j][i * 32 + j];
      c[j * 32 + i][i * 32 + j][j];
    }
}

// array filtering
#pragma ACCEL kernel
void test1() {
  int a[1024];
  for (int i = 0; i < 32; i++)
    for (int j = 0; j < 32; j++)
      a[i * 32 + j];
}

// host function does not change
void test1_1() {
  int a[1024];
  for (int i = 0; i < 32; i++)
    for (int j = 0; j < 32; j++)
      a[i * 32 + j];
}

#pragma ACCEL kernel
void test1_2(int a[1024]) {
  for (int i = 0; i < 32; i++)
    for (int j = 0; j < 32; j++)
      a[i * 32 + j];
}

int a[1024];
#pragma ACCEL kernel
void test1_3() {
  for (int i = 0; i < 32; i++)
    for (int j = 0; j < 32; j++)
      a[i * 32 + j];
}

/////////////////////////////////////////////////////

void sub_func(int a[1024], int *read_port) {
  int i, j, k;
  for (i = 0; i < 4; i += 1)
    for (j = 0; j < 16; j += 4)
      for (k = 0; k < 16; k++) {
        a[i * 256 + j * 16 + k];
      }
  memcpy(a, read_port, 0);
}

/////////////////////////////////////////////////
//
//

void sub_func1(int a[1024], int *read_port) {
  int i, j, k;
  for (i = 0; i < 4; i += 1)
    for (j = 0; j < 16; j += 4)
      for (k = 0; k < 16; k++) {
        a[i * 256 + j * 16 + k];
      }
  memcpy(&(a[0]), read_port, 0);
}

#pragma ACCEL kernel
int main() {
  int a0[1024];
  int a1[1024];
  int a2[1024];
  int a3[1024];
  int a4[1024];
  int a5[1024];
  int a6[800];
  int a7[800];
  int i, j, k, l, m, n;
  int *read_port;

  //    {
  //        a0;
  //        a1;
  //        a2;
  //        a3;
  //        a4;
  //        a5;
  //        a6;
  //    }

  for (i = 0; i < 8; i += 1)
    for (j = 0; j < 128; j++) {
      a0[i * 256 + j + 2];
      a0[i * 256 + j + 3];
    }

  for (i = 0; i < 8; i += 1)
    for (j = 0; j < 256; j += 4)
      for (k = 0; k < 4; k++) {
        a1[i * 256 + j + k];
      }

  for (i = 0; i < 8; i += 1)
    for (j = 0; j < 128; j++) {
      a2[i * 256 + j + 2];
      a2[i * 256 + j + 3];
    }
  for (i = 0; i < 8; i += 1)
    for (j = 0; j < 256; j += 4)
      for (k = 0; k < 4; k++) {
        a2[i * 256 + j + k];
      }

  for (i = 0; i < 4; i += 1)
    for (j = 0; j < 16; j += 4)
      for (k = 0; k < 16; k++) {
        a3[i * 256 + j * 16 + k];
      }
  sub_func(a3, read_port);

  memcpy(a3, read_port, 0);

  // ZP: do not support multiple calls of the same function
  // both a3 and a4 is not delinearized
  // sub_func(a4, read_port);

  sub_func1(a5, read_port);

  for (int i = 0; i < 100; ++i) {
    for (int j = 0; j < 40; ++j) {
      a6[i * 40 + 80 + j];
      a7[i * 40 + 81 + j];
    }
  }
  return 1;
}

void g1(int x);
void g2(long x);
void g3(float x);
void g4(double x);
void g5(const int x);
void g6(volatile int x);

void h1(double &x);
void h2(double *x);
void h3(const int &x);

void fa1() {
  int a[100];
  for (int i = 0; i < 10; ++i) {
    for (int j = 0; j < 10; ++j) {
      g1(a[10 * i + j]);
    }
  }
}

void fa2() {
  long a[100];
  for (int i = 0; i < 10; ++i) {
    for (int j = 0; j < 10; ++j) {
      g2(a[10 * i + j]);
    }
  }
}

void fa3() {
  float a[100];
  for (int i = 0; i < 10; ++i) {
    for (int j = 0; j < 10; ++j) {
      g3(a[10 * i + j]);
    }
  }
}

void fa4() {
  double a[100];
  for (int i = 0; i < 10; ++i) {
    for (int j = 0; j < 10; ++j) {
      g4(a[10 * i + j]);
    }
  }
}

void fa5() {
  int a[100];
  for (int i = 0; i < 10; ++i) {
    for (int j = 0; j < 10; ++j) {
      g5(a[10 * i + j]);
    }
  }
}

void fa6() {
  volatile int a[100];
  for (int i = 0; i < 10; ++i) {
    for (int j = 0; j < 10; ++j) {
      g6(a[10 * i + j]);
    }
  }
}

void fb1() {
  double a[100];
  for (int i = 0; i < 10; ++i) {
    for (int j = 0; j < 10; ++j) {
      h1(a[10 * i + j]);
    }
  }
}

void fb2() {
  double a[100];
  for (int i = 0; i < 10; ++i) {
    for (int j = 0; j < 10; ++j) {
      h2(&a[10 * i + j]);
    }
  }
}

void fb3() {
  int a[100];
  for (int i = 0; i < 10; ++i) {
    for (int j = 0; j < 10; ++j) {
      h3(a[10 * i + j]);
    }
  }
}

#pragma ACCEL kernel
void f() {
  fa1();
  fa2();
  fa3();
  fa4();
  fa5();
  fa6();

  fb1();
  fb2();
  fb3();
}
