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
//#pragma ACCEL kernel
//void test_typical()
//{
//
//  {
//    int a1[1024];
//    for (int i = 0; i < 1024; i++)
//    {
//#pragma ACCEL pipeline
//      a1[i] = a1[i-1];
//    }
//  }
//
//  {
//    int a8;
//    {
//      for (int j = 0; j < 8; j++)
//      {
//#pragma ACCEL pipeline
//        a8;
//      }
//    }
//  }
//  {
//    int a9[1024][1024];
//    for (int i = 0; i < 1024; i++)
//    {
//#pragma ACCEL parallel
//      for (int j = 0; j < 8; j++)
//      {
//#pragma ACCEL pipeline
//        a9[i][j];
//      }
//    }
//  }
//}
#pragma ACCEL kernel

void test1_1()
{
  int test1_1_a[1024];
  
#pragma ivdep array(test1_1_a)
  for (int i = 0; i < 1024; i++) {
    
#pragma ACCEL pipeline
    test1_1_a[i]++;
  }
{
    int test1_1_a2[1024];
    int i;
    for (i = 0; i < 1024; i++) {
      
#pragma ACCEL pipeline
    }
    test1_1_a2[i]++;
  }
  int test1_1_a3[1024];
  for (int i = 0; i < 1024; (i++ , test1_1_a3[i]++)) {
    
#pragma ACCEL pipeline
  }
  int test1_1_a4[1024];
  
#pragma ACCEL pipeline
  
#pragma ivdep array(test1_1_a4)
  for (int i = 0; i < 1024; i++) {
    test1_1_a4[i]++;
  }
  int test1_1_b[1024];
  for (int i = 0; i < 1024; i++) {
    
#pragma ivdep array(test1_1_b)
    for (int j = 0; j < 1024; j++) {
      
#pragma ACCEL pipeline
      test1_1_b[j]++;
    }
  }
  int test1_1_c[1024];
  for (int i = 0; i < 1024; i++) {
    
#pragma ACCEL pipeline
    
#pragma ivdep array(test1_1_c)
    for (int j = 0; j < 1024; j++) {
      test1_1_c[j]++;
    }
  }
  int test1_1_d[1024];
  for (int i = 0; i < 1024; i++) {
    for (int j = 0; j < 1024; j++) {
      
#pragma ACCEL pipeline
      test1_1_d[i]++;
    }
  }
  int test1_1_e[1024];
  for (int i = 0; i < 1024; i++) {
    
#pragma ACCEL pipeline
    for (int j = 0; j < 1024; j++) {
      test1_1_e[i]++;
    }
  }
  int test1_1_f[1024];
  
#pragma ivdep array(test1_1_f)
  for (int i = 0; i < 1024; i++) {
    
#pragma ACCEL pipeline
    for (int j = 0; j < 1024; j++) {
      
#pragma ACCEL parallel
      test1_1_f[i]++;
    }
  }
  int test1_1_g[1024];
  for (int i = 0; i < 1024; i++) {
    for (int j = 0; j < 1024; j++) {
      
#pragma ACCEL pipeline
      test1_1_g[i]++;
    }
    for (int j = 0; j < 1024; j++) {
      
#pragma ACCEL parallel
      test1_1_g[i]++;
    }
  }
  int test1_1_h[1024];
  for (int i = 0; i < 1024; i++) {
    
#pragma ACCEL pipeline
    for (int j = 0; j < 1024; j++) {
      
#pragma ACCEL pipeline
      test1_1_h[i]++;
    }
  }
  int test1_1_h1[1024];
  for (int i = 0; i < 1024; i++) {
    
#pragma ACCEL parallel
    for (int j = 0; j < 1024; j++) {
      test1_1_h1[i]++;
    }
  }
  int test1_1_i[1024][1024];
  
#pragma ivdep array(test1_1_i)
  for (int i = 0; i < 1024; i++) {
    
#pragma ACCEL parallel
    
#pragma ivdep array(test1_1_i)
    for (int j = 0; j < 1024; j++) {
      
#pragma ACCEL parallel
      test1_1_i[i][j]++;
    }
    
#pragma ivdep array(test1_1_i)
    for (int j = 0; j < 1024; j++) {
      
#pragma ACCEL parallel
      test1_1_i[i][j]++;
    }
  }
}
#pragma ACCEL kernel

void test1_2()
{
  int test1_2_a[1024];
  
#pragma ivdep array(test1_2_a)
  for (int i = 0; i < 1024; i++) {
    
#pragma ACCEL parallel factor=2
    test1_2_a[i]++;
  }
  int test1_2_b[1024];
  
#pragma ivdep array(test1_2_b)
  for (int i = 0; i < 1024; i++) {
    
#pragma ACCEL tile factor=2
    test1_2_b[i]++;
  }
  int test1_2_c[1024];
  
#pragma ivdep array(test1_2_c)
  for (int i = 0; i < 1024; i++) {
    
#pragma ACCEL pipeline priority=logic
    test1_2_c[i]++;
  }
}

void sub1_3_a(int *sub1_3_a,int i)
{
  sub1_3_a[i]++;
}

void sub1_3_b(int *sub1_3_b,int i)
{
  sub1_3_b[0]++;
}

void sub1_3_c(int (*sub1_3_c)[1024],int i)
{
  
#pragma ivdep array(sub1_3_c)
  for (int j = 0; j < 1024; j++) {
    
#pragma ACCEL parallel
    sub1_3_c[i][j];
  }
}
#pragma ACCEL kernel

void test1_3()
{
  int test1_3_a[1024];
  
#pragma ivdep array(test1_3_a)
  for (int i = 0; i < 1024; i++) {
    
#pragma ACCEL pipeline
    sub1_3_a(test1_3_a,i);
  }
  int test1_3_b[1024];
  for (int i = 0; i < 1024; i++) {
    
#pragma ACCEL pipeline
    sub1_3_b(test1_3_b,i);
  }
  int test1_3_c[1024][1024];
  
#pragma ivdep array(test1_3_c)
  for (int i = 0; i < 1024; i++) {
    
#pragma ACCEL pipeline
    sub1_3_c(test1_3_c,i);
  }
}
#pragma ACCEL kernel

void test2_1()
{
  int test2_1_a;
  for (int i = 0; i < 1024; i++) {
    
#pragma ACCEL pipeline
    test2_1_a + i;
  }
  float test2_1_b[1024];
  
#pragma ivdep array(test2_1_b)
  for (int i = 0; i < 1024; i++) {
    
#pragma ACCEL pipeline
    test2_1_b[i];
  }
  float test2_1_c[1024];
  float *test2_1_c1 = test2_1_c;
  ;
  
#pragma ivdep array(test2_1_c1)
  for (int i = 0; i < 1024; i++) {
    
#pragma ACCEL pipeline
    test2_1_c1[i];
  }
  float test2_1_d[1024][1024];
  
#pragma ivdep array(test2_1_d)
  for (int i = 0; i < 1024; i++) {
    
#pragma ACCEL pipeline
    test2_1_d[i][0];
  }
}
#pragma ACCEL kernel

void test2_2()
{
  float test2_2_a[1024];
  
#pragma ivdep array(test2_2_a)
  for (int i = 0; i < 1024; i++) {
    
#pragma ACCEL pipeline
    test2_2_a[i] = test2_2_a[i] + 1;
  }
  float test2_2_b[1024];
  for (int i = 0; i < 1024; i++) {
    
#pragma ACCEL pipeline
    test2_2_b[i] = test2_2_b[i - 1] + 1;
  }
  float test2_2_c[1024];
  for (int i = 0; i < 1024; i++) {
    
#pragma ACCEL pipeline
    test2_2_c[i] = test2_2_c[i + 1] + 1;
  }
  float test2_2_d[1024];
  
#pragma ivdep array(test2_2_d)
  for (int i = 0; i < 1024; i++) {
    
#pragma ACCEL pipeline
    test2_2_d[i] = 2;
    test2_2_d[i] + 1;
  }
  float test2_2_e[1024];
  
#pragma ivdep array(test2_2_e)
  for (int i = 0; i < 1024; i++) {
    
#pragma ACCEL pipeline
    test2_2_e[i] = 2;
    test2_2_e[i]++;
  }
  float test2_2_f[1024];
  
#pragma ivdep array(test2_2_f)
  for (int i = 0; i < 1024; i++) {
    
#pragma ACCEL pipeline
    test2_2_f[i] + 1;
    test2_2_f[i] = 2;
  }
}
#pragma ACCEL kernel

void test2_3()
{
  float test2_3_a1[1024];
  float test2_3_a2[1024];
  float *test2_3_a3;
  test2_3_a3 = test2_3_a1;
  test2_3_a3 = test2_3_a2;
  
#pragma ivdep array(test2_3_a3)
  for (int i = 0; i < 1024; i++) {
    
#pragma ACCEL pipeline
    test2_3_a3[i]++;
  }
  float test2_3_b1[1024];
  float test2_3_b2[1024];
  float *test2_3_b3;
  test2_3_b3 = test2_3_b1;
  test2_3_b3 = test2_3_b2;
  
#pragma ivdep array(test2_3_b2)
  for (int i = 0; i < 1024; i++) {
    
#pragma ACCEL pipeline
    test2_3_b2[i]++;
  }
  float test2_3_c1[1024];
  float *test2_3_c3;
  for (int i = 0; i < 1024; i++) {
    
#pragma ACCEL pipeline
    test2_3_c3 = test2_3_c1 + i;
    test2_3_c3[0]++;
  }
}

void sub2_4_a(float *sub2_4_a)
{
  sub2_4_a[0]++;
}

void sub2_4_b1(float *sub2_4_b1)
{
  sub2_4_b1[0]++;
}

void sub2_4_b(float *sub2_4_b)
{
  sub2_4_b1(sub2_4_b + 1);
}

void sub2_4_c(float *sub2_4_c)
{
  sub2_4_c[0]++;
}

void sub2_4_d(float *sub2_4_d)
{
  sub2_4_d[0]++;
}

void sub2_4_d1(float *sub2_4_d)
{
  sub2_4_d[1]++;
}

void sub2_4_e1(float *sub2_4_e1)
{
  sub2_4_e1[1]++;
}

void sub2_4_e(float *sub2_4_e)
{
  sub2_4_e1(sub2_4_e);
}
#pragma ACCEL kernel

void test2_4()
{
  float test2_4_a[1024];
  
#pragma ivdep array(test2_4_a)
  for (int i = 0; i < 1024; i++) {
    
#pragma ACCEL pipeline
    sub2_4_a(test2_4_a + i);
  }
  float test2_4_b[1024];
  
#pragma ivdep array(test2_4_b)
  for (int i = 0; i < 1024; i++) {
    
#pragma ACCEL pipeline
    sub2_4_b(test2_4_b + i);
  }
  float test2_4_c[1024];
  
#pragma ivdep array(test2_4_c)
  for (int i = 0; i < 1024; i++) {
    
#pragma ACCEL pipeline
    sub2_4_c(test2_4_c + i + 1);
    sub2_4_c(test2_4_c + i + 1);
  }
  float test2_4_d[1024];
  for (int i = 0; i < 1024; i++) {
    
#pragma ACCEL pipeline
    sub2_4_d(test2_4_d + i + 1);
    sub2_4_d1(test2_4_d + i + 1);
  }
  float test2_4_e[1024];
  
#pragma ivdep array(test2_4_e)
  for (int i = 0; i < 1024; i++) {
    
#pragma ACCEL pipeline
    sub2_4_e(test2_4_e + i + 1);
    sub2_4_e1(test2_4_e + i + 1);
  }
}

int sub2_5_e(int i)
{
  return i;
}
#pragma ACCEL kernel

void test2_5()
{
  int test2_5_a[1024];
  
#pragma ivdep array(test2_5_a)
  for (int i = 0; i < 1024; i++) {
    
#pragma ACCEL pipeline
    test2_5_a[2 * i]++;
  }
  int test2_5_b[1024];
  
#pragma ivdep array(test2_5_b)
  for (int i = 0; i < 1024; i++) {
    
#pragma ACCEL pipeline
    test2_5_b[2 * i]++;
  }
  int test2_5_c[1024];
  for (int i = 0; i < 1024; i++) {
    
#pragma ACCEL pipeline
    test2_5_c[1023 - i]++;
  }
  int test2_5_d[1024];
  for (int i = 0; i < 1024; i++) {
    
#pragma ACCEL pipeline
    test2_5_d[i * i]++;
  }
  int test2_5_e[1024];
  for (int i = 0; i < 1024; i++) {
    
#pragma ACCEL pipeline
    test2_5_e[sub2_5_e(i)]++;
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
