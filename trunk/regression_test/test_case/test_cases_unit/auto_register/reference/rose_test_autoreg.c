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
#include <string.h>
//#define FOCUS 0
#if !FOCUS

void sub1_1_c(int *sub1_1_c,int i)
{
  sub1_1_c[i];
}
#pragma ACCEL kernel

void test1_1()
{
/* #pragma ACCEL attribute register variable=test1_1_a */
  int __attribute__((register)) test1_1_a[16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test1_1_a[i];
  }
  int test1_1_a1[16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test1_1_a1[0];
  }
/* #pragma ACCEL attribute register variable=test1_1_b */
  int __attribute__((register)) test1_1_b[16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    for (int j = 0; j < 16; j++) 
      test1_1_b[i];
  }
/* #pragma ACCEL attribute register variable=test1_1_c */
  int __attribute__((register)) test1_1_c[16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    sub1_1_c(test1_1_c,i);
  }
/* #pragma ACCEL attribute register variable=test1_1_d */
  int __attribute__((register)) test1_1_d[16];
  int *test1_1_d1 = test1_1_d + 1;
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test1_1_d1[i];
  }
/* #pragma ACCEL attribute register variable=test1_1_e */
  int __attribute__((register)) test1_1_e[16];
  int *test1_1_e1 = test1_1_e + 1;
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test1_1_e1[i];
  }
  test1_1_e[0];
  int test1_1_f[16][2];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    int *test1_1_f1 = test1_1_f[i];
    test1_1_f1[0];
  }
  int test1_1_g[16][2];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    int *test1_1_g1 = test1_1_g[2];
    test1_1_g1[i];
  }
}
#pragma ACCEL kernel

void test1_2()
{
  char test1_2_a[512];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test1_2_a[i];
  }
  char test1_2_a1[513];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test1_2_a1[i];
  }
  char test1_2_a2[512];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    (&test1_2_a2[i * 2])[1];
  }
  char *test1_2_b;
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test1_2_b[i];
  }
  short test1_2_c[256];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test1_2_c[i];
  }
  short test1_2_c1[257];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test1_2_c1[i];
  }
  long long test1_2_d[64];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test1_2_d[i];
  }
  long long test1_2_d1[65];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test1_2_d1[i];
  }
  char test1_2_e[256][2];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test1_2_e[i][0];
  }
  char test1_2_e1[257][2];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test1_2_e1[i][0];
  }
  char test1_2_f[256][2];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test1_2_f[i];
  }
}
#pragma ACCEL kernel

void test1_3()
{
  char test1_3_a[256][2];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test1_3_a[0] + i;
  }
  char test1_3_b[256][2];
  char *test1_3_b1;
// do not support separate decl and assign
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test1_3_b1 = test1_3_b[0] + i;
    test1_3_b1[0];
  }
  char test1_3_c[256][2];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    char *test1_3_c1 = test1_3_c[i];
    test1_3_c1[0];
  }
  char test1_3_d[256][2];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    char *test1_3_d1 = test1_3_d[i];
    test1_3_d[i][0];
    test1_3_d1++;
  }
  char test1_3_e[256][2];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    char (*test1_3_e1)[2] = &test1_3_e[i];
    test1_3_e[i][0];
    test1_3_e1++;
  }
  char test1_3_f[256][2];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    char *test1_3_f1 = test1_3_f[0];
    test1_3_f1[i];
  }
  char test1_3_g[256][2];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    char *test1_3_g1 = test1_3_g[0];
    test1_3_g1[2];
  }
  char test1_3_h[256][2];
  char test1_3_h1[256][2];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    char *test1_3_h2 = i < 10?test1_3_h[0] : test1_3_h1[0];
    test1_3_h2[i];
  }
  char test1_3_i[256][2];
  char *test1_3_i1 = test1_3_i[0];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    char *test1_3_i2 = test1_3_i1 + i;
    test1_3_i2[0];
  }
}

void sub1_4_a(char *sub1_4_a)
{
  sub1_4_a[0];
}

void sub1_4_b(char *sub1_4_b)
{
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    sub1_4_b[i];
  }
}

void sub1_4_c(char *sub1_4_c)
{
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    sub1_4_c[i];
  }
}

void sub1_4_c1(char *sub1_4_c1)
{
  for (int i = 0; i < 16; i++) {
    sub1_4_c1[i];
  }
}

void sub1_4_d(char *sub1_4_d)
{
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    sub1_4_d[i];
  }
}

void sub1_4_e()
{
  char sub1_4_e[256][2];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    sub1_4_e[i];
  }
}
#pragma ACCEL kernel

void test1_4()
{
  char test1_4_a[256][2];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    sub1_4_a(test1_4_a[i]);
  }
  char test1_4_b[256][2];
  sub1_4_b(test1_4_b[0]);
  char test1_4_c[256][2];
  sub1_4_c(test1_4_c[0]);
  sub1_4_c1(test1_4_c[0]);
  char test1_4_d[256][2];
  char test1_4_d1[256][2];
  sub1_4_d(test1_4_d[0]);
  sub1_4_d(test1_4_d1[0]);
  sub1_4_e();
}
#pragma ACCEL kernel

void test2_1()
{
/* #pragma ACCEL attribute register variable=test2_1_a */
  char __attribute__((register)) test2_1_a[256][2];
  for (int i = 0; i < 16; i++) {
    test2_1_a[i][0];
  }
/* #pragma ACCEL attribute register variable=test2_1_b */
  char __attribute__((register)) test2_1_b[256][2];
  for (int i = 0; i < 16; i++) {
    test2_1_b[i][0];
  }
/* #pragma ACCEL attribute register variable=test2_1_c */
  char __attribute__((register)) test2_1_c[256][2];
  for (int i = 0; i < 16; i++) {
    test2_1_c[i][0];
  }
  char test2_1_d[256][2];
  
#pragma ACCEL register variable=test2_1_d
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_1_d[i][0];
  }
  char test2_1_e[256][2];
  
#pragma ACCEL register variable=test2_1_e off
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_1_e[i][0];
  }
}
#pragma ACCEL kernel

void test2_2()
{
  char test2_2_a[256][2];
  for (int i = 0; i < 16; i++) {
//#pragma ACCEL parallel
    test2_2_a[i][0];
{
      char test2_2_a[256][2];
      for (int i = 0; i < 16; i++) {
        
#pragma ACCEL parallel
        test2_2_a[i][0];
      }
    }
  }
  char test2_2_b[256][2];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_2_b[i][0];
{
      char test2_2_b[256][2];
      for (int i = 0; i < 16; i++) {
        
#pragma ACCEL parallel
        test2_2_b[0][0];
      }
    }
  }
}
#else  // FOCUS
#endif  // FOCUS
//#define FOCUS 0
#if !FOCUS
#pragma ACCEL kernel

void test5_5_cpp()
// insert offset variable - Initializer
{
  int test5_5_b[16][64];
  int (*test5_5_b1)[64] = test5_5_b;
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_5_b[i];
  }
}
#else
#endif
