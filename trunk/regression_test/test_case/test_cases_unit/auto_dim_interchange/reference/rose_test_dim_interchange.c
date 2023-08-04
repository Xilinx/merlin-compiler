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
#pragma ACCEL kernel

void test_typical()
{
  int test1_1_a[17][16][18];
  int __b_offset0 = 0;
  int __b_offset1 = 0;
  int __b_offset2 = 0;
  int (*b)[16][18] = test1_1_a;
  __b_offset2 = 0;
  __b_offset1 = 0;
  __b_offset0 = 0;
  b = test1_1_a;
  __b_offset2 = 0;
  __b_offset1 = 1;
  __b_offset0 = 0;
  int c[8][32][16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test1_1_a[0][1][i] = 0;
    test1_1_a[0] + i;
    c[0][0] + i;
  }
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    b[2][0][i] = 0;
    b[2][0];
  }
}
void sub1_1(int (*sub1_1a)[16],int i,int __sub1_1a_offset1,int __sub1_1a_offset0);
#pragma ACCEL kernel

void test_1_1()
{
  int test1_1_a[16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test1_1_a[i];
  }
  int test1_1_b[32][16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test1_1_b[0][i];
  }
  int test1_1_c[16][32];
  for (int i = 0; i < 16; i++) 
    for (int j = 0; j < 16; j++) {
      
#pragma ACCEL parallel
      test1_1_c[i][0];
    }
  int test1_1_d[32][16];
  for (int j = 0; j < 16; j++) 
    for (int i = 0; i < 16; i++) {
      
#pragma ACCEL parallel
      test1_1_d[0][i];
    }
  int test1_1_e[16][32];
  for (int j = 0; j < 16; j++) {
    
#pragma ACCEL parallel
    for (int i = 0; i < 16; i++) {
      
#pragma ACCEL parallel
      test1_1_e[i][j];
    }
  }
  int test1_1_f[32][16][64];
  for (int j = 0; j < 16; j++) {
    
#pragma ACCEL parallel
    for (int i = 0; i < 16; i++) {
      
#pragma ACCEL parallel
      test1_1_f[0][i][j];
    }
  }
  int test1_1_g[32][64][16];
  for (int j = 0; j < 16; j++) {
    for (int i = 0; i < 16; i++) {
      
#pragma ACCEL parallel
      test1_1_g[0][j][i];
    }
    for (int i = 0; i < 16; i++) {
      test1_1_g[0][j][i];
    }
  }
  int test1_1_h[32][16];
  for (int j = 0; j < 16; j++) {
    
#pragma ACCEL parallel
    for (int i = 0; i < 16; i++) {
      test1_1_h[i][j];
    }
  }
  int test1_1_i[32][16];
  int test1_1_ia[32][16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test1_1_i[0][i];
    sub1_1(test1_1_ia,i,0,0);
  }
}

void sub1_1(int (*sub1_1a)[16],int i,int __sub1_1a_offset1,int __sub1_1a_offset0)
{
  sub1_1a[0][i];
  int sub1_1b[32][16];
  for (int t = 0; t < 16; t++) {
    
#pragma ACCEL parallel
    sub1_1b[0][t];
  }
}
#pragma ACCEL kernel

void test_1_2()
{
  int test1_2_a[16][32];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel factor=4
    test1_2_a[i][0];
  }
  int test1_2_b[16][32];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL pipeline_parallel
    test1_2_b[i][0];
  }
  int test1_2_c[16][32];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL pipeline parallel
    test1_2_c[i][0];
  }
}
#pragma ACCEL kernel

void test_2_1()
{
  int test2_1_a[16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_1_a[i];
  }
  int test2_1_b[32][16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_1_b[0][i];
  }
  int test2_1_c[32][64][16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_1_c[1][2][i];
  }
  int *(*test2_1_d1)[64];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_1_d1[1][2][i];
  }
  int (*test2_1_d2[16])[64];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_1_d2[i][1][2];
  }
  int *test2_1_d3[16][32];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_1_d3[i][1][2];
  }
  int (**test2_1_e1)[16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_1_e1[i][1][2];
  }
  int *(*test2_1_e2)[32];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_1_e2[i][1][2];
  }
  int **test2_1_e3[64];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_1_e3[i][1][2];
  }
  int ***test2_1_f;
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_1_f[i][1][2];
  }
}
#pragma ACCEL kernel

void test_2_2()
{
  int test2_2_a[32][64][16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_2_a[1][2][i];
  }
  int test2_2_b[16][64][32];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_2_b[1][2][i];
  }
  int test2_2_c[16][32][64];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_2_c[1][2][i];
  }
  int test2_2_d[16][32][64];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_2_d[1][i][i];
  }
  int test2_2_e[32][16][64];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_2_e[2][i][i];
  }
  int test2_2_f[64][16][32];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_2_f[2][i][i];
  }
  int test2_2_g[16][32][64];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_2_g[i][i][i];
  }
  int test2_2_h[64][16][32];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_2_h[2][i][1];
    test2_2_h[2][0][i];
  }
  int test2_2_h1[64][16][32];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_2_h1[2][i][1];
  }
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_2_h1[2][0][i];
  }
// the following is related to pointer dim
  int test2_2_i[32][64][16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_2_i[1][2][i];
  }
  test2_2_i[0];
  test2_2_i[1][0];
  test2_2_i[1][2][0];
  int test2_2_j[16][64][32];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_2_j[0][2][i];
  }
  test2_2_j[0];
  test2_2_j[0][0] + 1;
  test2_2_j[0][2][1];
  int test2_2_k[16][32][64];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_2_k[i][0][2];
  }
  test2_2_k[3];
  test2_2_k[3][1];
  test2_2_k[3][1][2];
}

void sub2_3_l(double (*sub2_3_la)[10],int j)
{
  sub2_3_la[j][0];
}
int g_test2_3_a[16][32];
int g_test2_3_a1[16][32];
#pragma ACCEL kernel

void test_2_3(int test2_3_b[16][32])
{
  int (*test2_3_a1)[32] = g_test2_3_a1;
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    g_test2_3_a[i][2];
    test2_3_a1[i];
  }
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_3_b[i][2];
  }
  int test2_3_c[16][48][32];
  int __test2_3_c1_offset0 = 0;
  int __test2_3_c1_offset1 = 0;
  int (*test2_3_c1)[32] = test2_3_c[0];
  __test2_3_c1_offset1 = 1;
  __test2_3_c1_offset0 = 0;
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_3_c[1][2][i];
  }
  int test2_3_d[16][32][48];
  int **test2_3_d1;
  test2_3_d1[0] = test2_3_d[0][1];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_3_d[1][i][2];
  }
  int test2_3_e[16][32][48];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    (1 , test2_3_e[1][i]);
  }
  int test2_3_e1[16][32][48];
  int (*test2_3_e1a)[48] = test2_3_e1[0];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_3_e1[1][i];
  }
  &test2_3_e1a;
  int test2_3_f[16][32][48];
  int (**test2_3_f1)[32][48];
  test2_3_f1[0] = test2_3_f;
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_3_f[1][i][2];
  }
  int **test2_3_g;
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_3_g[i][0];
  }
  int test2_3_h[32][48][16];
  int __test2_3_h1_offset0 = 0;
  int __test2_3_h1_offset1 = 0;
  int __test2_3_h1_offset2 = 0;
  int (*test2_3_h1)[48][16] = test2_3_h;
  __test2_3_h1_offset2 = 1;
  __test2_3_h1_offset1 = 0;
  __test2_3_h1_offset0 = 0;
  int __test2_3_h2_offset0 = 0;
  int __test2_3_h2_offset1 = 0;
  int __test2_3_h2_offset2 = 0;
  int (*test2_3_h2)[48][16] = test2_3_h1;
  __test2_3_h2_offset2 = __test2_3_h1_offset2;
  __test2_3_h2_offset1 = 1 + __test2_3_h1_offset1;
  __test2_3_h2_offset0 = __test2_3_h1_offset0;
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_3_h[2][3][i];
  }
  int n;
  int test2_3_i[16][32][n];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_3_i[i][1][2];
  }
  int test2_3_j[16][32][10];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_3_j[1][i][i];
  }
  int test2_3_k[16][10][32];
  int test2_3_k1[16][32][10];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    (double )test2_3_k[1][0][i];
    (double *)test2_3_k1[1][i];
  }
// Disable this case because data type difference is ignored in this pass
//  int test2_3_l[32][10];
//  for (int i = 0; i < 16; i ++)
//  {
//#pragma ACCEL parallel 
//    sub2_3_l(test2_3_l, i);
//  }
}

void sub2_3a_a1(int (*sub2_3a_a1)[32][64][16],int __sub2_3a_a1_offset2,int __sub2_3a_a1_offset1,int __sub2_3a_a1_offset0)
{
  sub2_3a_a1[1][3][4][2] = 0;
}

void sub2_3a_a2(int (*sub2_3a_a2)[64][16],int __sub2_3a_a2_offset2,int __sub2_3a_a2_offset1,int __sub2_3a_a2_offset0)
{
  sub2_3a_a2[3][4][2] = 0;
}

void sub2_3a_b1(int (*sub2_3a_b1)[32][64][16],int __sub2_3a_b1_offset2,int __sub2_3a_b1_offset1,int __sub2_3a_b1_offset0)
{
  sub2_3a_b1[1][3][4][2]++;
}

void sub2_3a_b2(int (*sub2_3a_b2)[64][16],int __sub2_3a_b2_offset2,int __sub2_3a_b2_offset1,int __sub2_3a_b2_offset0)
{
  sub2_3a_b2[3][4][2]++;
}
typedef int (*type2_3a_c1)[64];

void sub2_3a_c1(type2_3a_c1 *sub2_3a_c1)
{
}
typedef int type2_3a_d1[8][16][32][64];

void sub2_3a_d1(type2_3a_d1 *sub2_3a_d1)
{
  sub2_3a_d1[0][0][0][0][0] = 0;
}
#pragma ACCEL kernel

void test2_3a()
{
  int test2_3a_a[8][32][64][16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_3a_a[0][0][0][i];
  }
  sub2_3a_a1(test2_3a_a,0,0,0);
  sub2_3a_a2(test2_3a_a[1],0,0,0);
  int test2_3a_b[8][32][64][16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_3a_b[0][0][0][i];
  }
  sub2_3a_b1(test2_3a_b,0,0,0);
  sub2_3a_b2(test2_3a_b[1],0,0,0);
  int test2_3a_c[8][16][32][64];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_3a_c[0][i][0][0];
  }
  int (*test2_3a_c1)[64] = test2_3a_c[1][2];
  sub2_3a_c1(&test2_3a_c1);
  int test2_3a_d[8][16][32][64];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_3a_d[0][i][0][0];
  }
  sub2_3a_d1(&test2_3a_d);
  int test2_3a_e[8][16][32][64];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_3a_e[0][0][i][0];
  }
  int (**test2_3a_e1)[32][64];
  test2_3a_e1[0] = test2_3a_e[1];
  test2_3a_e1[1][0][0];
}
int sub2_4_e(int *a);
#pragma ACCEL kernel

void test_2_4()
{
  int test2_4_a[16][32][48];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_4_a[0][i + 1];
  }
  int test2_4_b[16][48][32];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_4_b[0][0] + (1 + i);
    test2_4_b[0][0] + i;
  }
  int test2_4_c[16][32][48];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_4_c[0][i++];
  }
  int test2_4_d[16][32][48];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_4_d[0][i][i++];
  }
  int test2_4_e[16][32][48];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_4_e[0][i][sub2_4_e(&i)];
  }
  int test2_4_f[16][32][48];
  for (int i = 0; i < 16; i++) {
    int j;
    
#pragma ACCEL parallel
    test2_4_f[0][i][1 + (j = 0)];
  }
}

int sub2_4_e(int *a)
{
   *a = 3;
  return 0;
}
#pragma ACCEL kernel

void test_2_5()
{
  int test2_5_a[32][16];
  int test2_5_a1[16][32];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_5_a[0] + i;
  }
  test2_5_a1[test2_5_a[2][1]];
  int test2_5_b[32][16];
  int test2_5_b1[16][32];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_5_b[0] + i;
  }
  test2_5_b[0] + test2_5_b1[1][2];
  int test2_5_c[32][16];
  int test2_5_c1[32][16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_5_c[0] + i;
    test2_5_c1[0] + i;
  }
  test2_5_c[0] + test2_5_c1[2][1];
  int test2_5_d[32][16];
  int test2_5_d1[32][16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_5_d[0] + i;
    test2_5_d1[0] + i;
  }
  test2_5_d1[0] + test2_5_d[2][1];
  int test2_5_e[16][32];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_5_e[i];
  }
  test2_5_e[test2_5_e[1][2]];
  int test2_5_f[32][16];
  int __test2_5_f1_offset0 = 0;
  int __test2_5_f1_offset1 = 0;
  int (*test2_5_f1)[16] = test2_5_f;
  __test2_5_f1_offset1 = 0;
  __test2_5_f1_offset0 = test2_5_f[2][1];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test2_5_f[0] + i;
  }
}

void sub3_1_a(int (*sub3_1_a)[16],int __sub3_1_a_offset1,int __sub3_1_a_offset0)
{
  sub3_1_a[0][__sub3_1_a_offset1];
}

void sub3_1_b(int sub3_1_b[32],int j)
{
  sub3_1_b[j];
}

void sub3_1_c(int (*sub3_1_c)[16],int j,int __sub3_1_c_offset1,int __sub3_1_c_offset0)
{
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    sub3_1_c[0][i];
  }
}

void sub3_1_ds(int (*sub3_1_ds)[16][32],int i,int j,int __sub3_1_ds_offset2,int __sub3_1_ds_offset1,int __sub3_1_ds_offset0)
{
  for (int t = 0; t < 16; t++) {
    sub3_1_ds[t][i][j];
  }
}

void sub3_1_d(int (*sub3_1_d)[16][32],int j,int __sub3_1_d_offset2,int __sub3_1_d_offset1,int __sub3_1_d_offset0)
{
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    sub3_1_ds(sub3_1_d,j,i,__sub3_1_d_offset2,__sub3_1_d_offset1,__sub3_1_d_offset0);
  }
}
#pragma ACCEL kernel

void test3_1()
{
  int test3_1_a[32][16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    sub3_1_a(test3_1_a,i,0);
  }
  int test3_1_b[16][32];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    sub3_1_b(test3_1_b[0],i);
  }
  int test3_1_c[32][16];
  sub3_1_c(test3_1_c,0,0,0);
  int test3_1_d[48][16][32];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    sub3_1_d(test3_1_d,i,0,0,0);
  }
}

void sub3_2_a(int sub3_2_a[16][48][32],int i,int __sub3_2_a_offset1,int __sub3_2_a_offset0)
{
  sub3_2_a[1][2][i];
}

void sub3_2_b(int sub3_2_b[48][32],int i,int __sub3_2_b_offset1,int __sub3_2_b_offset0)
{
  sub3_2_b[2][i];
}

void sub3_2_c(int sub3_2_c[48][32],int i,int __sub3_2_c_offset1,int __sub3_2_c_offset0)
{
  sub3_2_c[0][__sub3_2_c_offset1];
}

void sub3_2_d(int sub3_2_d,int i)
{
  sub3_2_d;
}
#pragma ACCEL kernel

void test3_2()
{
  int test3_2_a[16][48][32];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    sub3_2_a(test3_2_a,i,0,0);
  }
  int test3_2_b[16][48][32];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    sub3_2_b(test3_2_b[0],i,0,0);
  }
  int test3_2_c[16][48][32];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    sub3_2_c(test3_2_c[0],i,i,0);
  }
  int test3_2_d[16][48][32];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    sub3_2_d(test3_2_d[0][1][i],i);
  }
}

void sub3_3_a(int sub3_3_a[16][48][32],int j,int __sub3_3_a_offset1,int __sub3_3_a_offset0)
{
  sub3_3_a[1][2 + __sub3_3_a_offset0][__sub3_3_a_offset1 + j];
}

void sub3_3_b(int sub3_3_b[16][48][32],int j,int __sub3_3_b_offset1,int __sub3_3_b_offset0)
{
  sub3_3_b[1][2 + __sub3_3_b_offset0][__sub3_3_b_offset1 + j];
}

void sub3_3_c(int sub3_3_c[16][48][32],int j,int __sub3_3_c_offset1,int __sub3_3_c_offset0)
{
  sub3_3_c[1][2 + __sub3_3_c_offset0][__sub3_3_c_offset1 + j];
}

void sub3_3_d(int sub3_3_d[48][16][32],int sub3_3_d1[48][16][32],int j,int __sub3_3_d_offset2,int __sub3_3_d_offset1,int __sub3_3_d_offset0,int __sub3_3_d1_offset2,int __sub3_3_d1_offset1,int __sub3_3_d1_offset0)
{
  sub3_3_d[2][j][1];
  sub3_3_d1[2][1][j];
}

void sub3_3_e(int sub3_3_e[48][16][32],int j,int __sub3_3_e_offset2,int __sub3_3_e_offset1,int __sub3_3_e_offset0)
{
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    sub3_3_e[2 + __sub3_3_e_offset0][__sub3_3_e_offset2 + j][__sub3_3_e_offset1 + i];
  }
}

void sub3_3_f(int sub3_3_f[48][16][32],int k,int __sub3_3_f_offset2,int __sub3_3_f_offset1,int __sub3_3_f_offset0)
{
  sub3_3_f[2][1][k];
}

void sub3_3_f1(int sub3_3_f1[48][16][32],int j,int __sub3_3_f1_offset2,int __sub3_3_f1_offset1,int __sub3_3_f1_offset0)
{
  sub3_3_f1[2][j][0];
}
#pragma ACCEL kernel

void test3_3()
{
  int test3_3_a[16][48][32];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    sub3_3_a(test3_3_a,i,0,0);
  }
  sub3_3_a(test3_3_a,1,0,0);
  int test3_3_b[16][48][32];
  int test3_3_b1[16][48][32];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    sub3_3_b(test3_3_b,i,0,0);
  }
  sub3_3_b(test3_3_b1,1,0,0);
  int test3_3_c[16][48][32];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    sub3_3_c(test3_3_c,i,0,0);
    sub3_3_c(test3_3_c,i,0,0);
  }
  int test3_3_d[48][16][32];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    sub3_3_d(test3_3_d,test3_3_d,i,0,0,0,0,0,0);
  }
  int test3_3_e[48][16][32];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    sub3_3_e(test3_3_e,i,0,0,0);
    sub3_3_e(test3_3_e,1,0,0,0);
  }
  int test3_3_f[48][16][32];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    sub3_3_f(test3_3_f,i,0,0,0);
  }
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    sub3_3_f1(test3_3_f,i,0,0,0);
  }
}

void sub3_4_a2(int sub3_4_a2[32][48][16],int j,int __sub3_4_a2_offset2,int __sub3_4_a2_offset1,int __sub3_4_a2_offset0)
{
  sub3_4_a2[__sub3_4_a2_offset1][2 + __sub3_4_a2_offset0][__sub3_4_a2_offset2 + j];
}

void sub3_4_a1(int sub3_4_a1[32][48][16],int j,int __sub3_4_a1_offset2,int __sub3_4_a1_offset1,int __sub3_4_a1_offset0)
{
  sub3_4_a1[2 + __sub3_4_a1_offset1][__sub3_4_a1_offset0] + __sub3_4_a1_offset2;
  sub3_4_a2(sub3_4_a1,j,__sub3_4_a1_offset2,__sub3_4_a1_offset1,__sub3_4_a1_offset0);
}

void sub3_4_a(int sub3_4_a[32][48][16],int j,int __sub3_4_a_offset2,int __sub3_4_a_offset1,int __sub3_4_a_offset0)
{
  sub3_4_a1(sub3_4_a,j,__sub3_4_a_offset2,__sub3_4_a_offset1,__sub3_4_a_offset0);
  sub3_4_a2(sub3_4_a,j,__sub3_4_a_offset2,__sub3_4_a_offset1,__sub3_4_a_offset0);
}

void sub3_4_b2(int sub3_4_b2[32][48][16],int j,int __sub3_4_b2_offset2,int __sub3_4_b2_offset1,int __sub3_4_b2_offset0)
{
  sub3_4_b2[__sub3_4_b2_offset1][2 + __sub3_4_b2_offset0][__sub3_4_b2_offset2 + j];
}

void sub3_4_b1(int sub3_4_b1[32][48][16],int j,int __sub3_4_b1_offset2,int __sub3_4_b1_offset1,int __sub3_4_b1_offset0)
{
  sub3_4_b1[2 + __sub3_4_b1_offset1][__sub3_4_b1_offset0] + __sub3_4_b1_offset2;
  sub3_4_b2(sub3_4_b1,j,__sub3_4_b1_offset2,__sub3_4_b1_offset1,__sub3_4_b1_offset0);
}

void sub3_4_b(int sub3_4_b[32][48][16],int j,int __sub3_4_b_offset2,int __sub3_4_b_offset1,int __sub3_4_b_offset0)
{
  sub3_4_b2(sub3_4_b,j,__sub3_4_b_offset2,__sub3_4_b_offset1,__sub3_4_b_offset0);
  sub3_4_b1(sub3_4_b,j,__sub3_4_b_offset2,__sub3_4_b_offset1,__sub3_4_b_offset0);
}
#pragma ACCEL kernel

void test3_4()
{
  int test3_4_a[32][48][16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    sub3_4_a(test3_4_a,i,0,0,0);
  }
  sub3_4_a(test3_4_a,1,0,0,0);
  int test3_4_b[32][48][16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    sub3_4_b(test3_4_b,i,0,0,0);
  }
  sub3_4_b(test3_4_b,1,0,0,0);
}
void sub4_1_a(int ,int );
void sub4_1_b(int *,int );
#pragma ACCEL kernel

void test4_1()
{
  int test4_1_a[32][48][16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    sub4_1_a(test4_1_a[0][0][i],i);
  }
  int test4_1_b[16][32][48];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    sub4_1_b(test4_1_b[i][0],i);
  }
}
// Recursive function is detected, but not add to regression test becuase of mars_ir exception
//void test4_2();
//void sub4_2(int * sub4_2_a) { test4_2(); }
//
//#pragma ACCEL kernel
//void test4_2()
//{
//  int test4_2_a[16];
//
//  for (int i = 0; i < 16; i++)
//  {
//#pragma ACCEL parallel 
//    sub4_2(test4_2_a+1); 
//  }
//}
//struct s4_3 { 
//  int s4_3_a[48][96]; 
//};
//void sub4_3(struct s4_3 (* sub4_3_a)[32], int i) { 
//  sub4_3_a[i][0];
//}
//void sub4_3s(int (* sub4_3s_a)[96], int i) {
//  sub4_3s_a[i][0];
//}
//
//
//#pragma ACCEL kernel
//void test4_3()
//{
//  struct s4_3 test4_3_a[16][32];
//  struct s4_3 test4_3s_a[16][32];
//  for (int i = 0; i < 16; i++)
//  {
//#pragma ACCEL parallel
//
//    sub4_3(test4_3_a, i); 
//    sub4_3s(test4_3s_a[i][0].s4_3_a, i); 
//  }
//}
#pragma ACCEL kernel

void test4_5(int *in_a)
{
  int test4_5_a[16][32];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test4_5_a[i][0];
  }
  memcpy(in_a,test4_5_a[0],16 * sizeof(int ));
}

void sub4_6(int sub4_6[32][16],int __sub4_6_offset1,int __sub4_6_offset0)
{
  __merlin_access_range(sub4_6,0,31UL,0,15UL);
}
#pragma ACCEL kernel

void test4_6(int *in_a)
{
  int test4_6_a[32][16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test4_6_a[0][i];
  }
  sub4_6(test4_6_a,1,0);
}

void sub4_6_b(int (*sub4_6_b)[16],int __sub4_6_b_offset1,int __sub4_6_b_offset0)
{
  __merlin_access_range(sub4_6_b,0,31UL,0,15UL);
}
#pragma ACCEL kernel

void test4_6_b(int *in_a)
{
  int test4_6_b[32][16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test4_6_b[0][i];
  }
  sub4_6_b(test4_6_b,0,0);
}

void sub5_0_b(int (*sub5_0_b)[64][16],int __sub5_0_b_offset2,int __sub5_0_b_offset1,int __sub5_0_b_offset0)
{
  sub5_0_b[1][0] + __sub5_0_b_offset2;
}
#pragma ACCEL kernel

void test_5_0()
{
  int test5_0_a[16][32][64];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_0_a[i];
  }
  int test5_0_b[32][64][16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    sub5_0_b(test5_0_b,i,0,0);
  }
  int test5_0_c[32][64][16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    int __test5_0_c1_offset0 = 0;
    int __test5_0_c1_offset1 = 0;
    int __test5_0_c1_offset2 = 0;
    int (*test5_0_c1)[64][16] = test5_0_c;
    __test5_0_c1_offset2 = i;
    __test5_0_c1_offset1 = 0;
    __test5_0_c1_offset0 = 0;
    test5_0_c1[1][2][i];
// ...
  }
  int test5_0_d[32][64][16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    int __test5_0_d1_offset0 = 0;
    int __test5_0_d1_offset1 = 0;
    int __test5_0_d1_offset2 = 0;
    int (*test5_0_d1)[64][16];
    test5_0_d1 = test5_0_d;
    __test5_0_d1_offset2 = i;
    __test5_0_d1_offset1 = 0;
    __test5_0_d1_offset0 = 0;
    test5_0_d1[1][2][i];
// ...
  }
  int test5_0_e[16][32][64][96];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_0_e[1][i];
  }
}

void sub5_1_b(int (*sub5_1_b)[64][16],int __sub5_1_b_offset2,int __sub5_1_b_offset1,int __sub5_1_b_offset0)
{
  sub5_1_b[1][0] + __sub5_1_b_offset2;
}

void sub5_1_h(int (*sub5_1_h)[64][16],int __sub5_1_h_offset2,int __sub5_1_h_offset1,int __sub5_1_h_offset0)
{
  sub5_1_h[1 + __sub5_1_h_offset1][__sub5_1_h_offset0] + __sub5_1_h_offset2;
}
#pragma ACCEL kernel

void test5_1()
// insert offset variable - condition to add offset variable
{
  int test5_1_a[16][32][64];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_1_a[i];
  }
  int test5_1_b[32][64][16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    sub5_1_b(test5_1_b,i,0,0);
  }
  int test5_1_c[32][64][16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    int __test5_1_c1_offset0 = 0;
    int __test5_1_c1_offset1 = 0;
    int __test5_1_c1_offset2 = 0;
    int (*test5_1_c1)[64][16] = test5_1_c;
    __test5_1_c1_offset2 = i;
    __test5_1_c1_offset1 = 0;
    __test5_1_c1_offset0 = 0;
  }
  int test5_1_d[32][64][16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    int __test5_1_d1_offset0 = 0;
    int __test5_1_d1_offset1 = 0;
    int __test5_1_d1_offset2 = 0;
    int (*test5_1_d1)[64][16];
    test5_1_d1 = test5_1_d;
    __test5_1_d1_offset2 = i;
    __test5_1_d1_offset1 = 0;
    __test5_1_d1_offset0 = 0;
  }
  int test5_1_e[32][64][16];
  int __test5_1_e1_offset0 = 0;
  int __test5_1_e1_offset1 = 0;
  int __test5_1_e1_offset2 = 0;
  int (*test5_1_e1)[64][16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_1_e1 = test5_1_e;
    __test5_1_e1_offset2 = i;
    __test5_1_e1_offset1 = 0;
    __test5_1_e1_offset0 = 0;
  }
  test5_1_e1 = test5_1_e;
  __test5_1_e1_offset2 = 1;
  __test5_1_e1_offset1 = 0;
  __test5_1_e1_offset0 = 0;
  int test5_1_f[16][32][64];
  int (*test5_1_f1)[64];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_1_f1 = test5_1_f[i];
  }
  test5_1_f1 = test5_1_f1 + 1;
  int test5_1_fa[16][32][64];
  int (*test5_1_fa1)[64];
  int (*test5_1_fa2)[64];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_1_fa1 = test5_1_fa[i];
  }
  test5_1_fa2 = test5_1_fa1 + 1;
  test5_1_fa1 = test5_1_fa2 + 1;
  int test5_1_fb[32][64][16];
  int __test5_1_fb1_offset0 = 0;
  int __test5_1_fb1_offset1 = 0;
  int __test5_1_fb1_offset2 = 0;
  int (*test5_1_fb1)[64][16];
  int __test5_1_fb2_offset0 = 0;
  int __test5_1_fb2_offset1 = 0;
  int __test5_1_fb2_offset2 = 0;
  int (*test5_1_fb2)[64][16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_1_fb1 = test5_1_fb;
    __test5_1_fb1_offset2 = i;
    __test5_1_fb1_offset1 = 0;
    __test5_1_fb1_offset0 = 0;
  }
  test5_1_fb2 = test5_1_fb1;
  __test5_1_fb2_offset2 = __test5_1_fb1_offset2;
  __test5_1_fb2_offset1 = 1 + __test5_1_fb1_offset1;
  __test5_1_fb2_offset0 = __test5_1_fb1_offset0;
  test5_1_fb2 = test5_1_fb1;
  __test5_1_fb2_offset2 = __test5_1_fb1_offset2;
  __test5_1_fb2_offset1 = 2 + __test5_1_fb1_offset1;
  __test5_1_fb2_offset0 = __test5_1_fb1_offset0;
  int test5_1_g[32][64][16];
  int __test5_1_g1_offset0 = 0;
  int __test5_1_g1_offset1 = 0;
  int __test5_1_g1_offset2 = 0;
  int (*test5_1_g1)[64][16];
  int test5_1_g2[32][64][16];
  int __test5_1_g3_offset0 = 0;
  int __test5_1_g3_offset1 = 0;
  int __test5_1_g3_offset2 = 0;
  int (*test5_1_g3)[64][16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_1_g1 = test5_1_g;
    __test5_1_g1_offset2 = i;
    __test5_1_g1_offset1 = 0;
    __test5_1_g1_offset0 = 0;
  }
  test5_1_g1 = test5_1_g2;
  __test5_1_g1_offset2 = 1;
  __test5_1_g1_offset1 = 0;
  __test5_1_g1_offset0 = 0;
  test5_1_g3 = test5_1_g2;
  __test5_1_g3_offset2 = 1;
  __test5_1_g3_offset1 = 0;
  __test5_1_g3_offset0 = 0;
  int test5_1_h[32][64][16];
  int __test5_1_h1_offset0 = 0;
  int __test5_1_h1_offset1 = 0;
  int __test5_1_h1_offset2 = 0;
  int (*test5_1_h1)[64][16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_1_h1 = test5_1_h;
    __test5_1_h1_offset2 = i;
    __test5_1_h1_offset1 = 0;
    __test5_1_h1_offset0 = 0;
  }
  test5_1_h1 = test5_1_h;
  __test5_1_h1_offset2 = 1;
  __test5_1_h1_offset1 = 0;
  __test5_1_h1_offset0 = 0;
  sub5_1_h(test5_1_h1,__test5_1_h1_offset2,__test5_1_h1_offset1,__test5_1_h1_offset0);
}
void sub5_2_i(int (*sub5_2_i)[64]);
#pragma ACCEL kernel

void test5_2()
// insert offset variable - unsupport partial pntr (for adding offset)
{
  int test5_2_a[16][32];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    int *test5_2_a1 = test5_2_a[i];
    test5_2_a1;
  }
  int test5_2_b[16][64][32];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    int __test5_2_b1_offset0 = 0;
    int __test5_2_b1_offset1 = 0;
    int (*test5_2_b1)[32] = test5_2_b[0];
    __test5_2_b1_offset1 = i;
    __test5_2_b1_offset0 = 0;
    test5_2_b1[0] + (1 + i);
    if (test5_2_b1[0] + (1 + i)) 
      ;
    while(test5_2_b1[0] + (1 + i))
      ;
    do {
    }while (test5_2_b1[0] + (1 + i));
    for (; test5_2_b1[0] + (1 + i); ) {
    }
    ;
  }
  int test5_2_c[16][32][64];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    int __test5_2_c1_offset0 = 0;
    int __test5_2_c1_offset1 = 0;
    int __test5_2_c1_offset2 = 0;
    int (*test5_2_c1)[32][64] = test5_2_c;
    __test5_2_c1_offset2 = i;
    __test5_2_c1_offset1 = 0;
    __test5_2_c1_offset0 = 0;
    test5_2_c1[__test5_2_c1_offset2][1 + __test5_2_c1_offset1];
    if (test5_2_c1[__test5_2_c1_offset2][1 + __test5_2_c1_offset1]) 
      ;
    while(test5_2_c[1])
      ;
    for (; test5_2_c[1]; ) {
    }
    ;
  }
  int test5_2_d[16][32][64];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    int (*test5_2_d1)[64] = test5_2_d[i];
    (test5_2_d1[1] , 1);
  }
  int test5_2_e[16][32][64];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    int (*test5_2_e1)[64] = test5_2_e[i];
    test5_2_e1[1] == 0;
  }
  int test5_2_f[16][32][64];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    int (*test5_2_f1)[64] = test5_2_f[i];
    !test5_2_f1[1];
  }
  int test5_2_g[16][32][64];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    int (*test5_2_g1)[64] = test5_2_g[i];
    test5_2_g1;
  }
  int test5_2_h[16][32][64];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    int (*test5_2_h1)[64] = test5_2_h[i];
    int (*test5_2_h2)[64];
    (test5_2_h2 = test5_2_h1)[1];
  }
  int test5_2_i[16][32][64];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    int (*test5_2_i1)[64] = test5_2_i[i];
    int (*test5_2_i2)[64];
    sub5_2_i(test5_2_i2 = test5_2_i1);
  }
  int test5_2_j[16][64];
  int (*test5_2_j1)[64];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_2_j[i];
  }
{
    int tt = (test5_2_j1 = test5_2_j)?1 : 0;
  }
}
#pragma ACCEL kernel

void test5_3()
// insert offset variable - insert position
{
  int test5_3_a[16][64];
  int __test5_3_a1_offset0 = 0;
  int __test5_3_a1_offset1 = 0;
  int (*test5_3_a1)[64];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_3_a[i];
    if (1) 
      test5_3_a1 = test5_3_a;
    while(1)
      test5_3_a1 = test5_3_a;
  }
  int test5_3_b[16][64];
  int __test5_3_b1_offset0 = 0;
  int __test5_3_b1_offset1 = 0;
  int (*test5_3_b1)[64];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_3_b[i];
    if (test5_3_b1 = test5_3_b) {
      test5_3_b1[1 + __test5_3_b1_offset1][2 + __test5_3_b1_offset0];
    }
    while(test5_3_b1 = test5_3_b){
      test5_3_b1[1 + __test5_3_b1_offset1][2 + __test5_3_b1_offset0];
    }
  }
  int test5_3_ba[16][64];
  int __test5_3_ba1_offset0 = 0;
  int __test5_3_ba1_offset1 = 0;
  int (*test5_3_ba1)[64];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_3_ba[i];
    if (test5_3_ba1 = test5_3_ba) {
      test5_3_ba1[1 + __test5_3_ba1_offset1][2 + __test5_3_ba1_offset0];
    }
     else {
      test5_3_ba1[3 + __test5_3_ba1_offset1][4 + __test5_3_ba1_offset0];
    }
  }
  int test5_3_c[64][16];
  int __test5_3_c1_offset0 = 0;
  int __test5_3_c1_offset1 = 0;
  int (*test5_3_c1)[16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_3_c[0] + i;
  }
  LABEL_test5_3_c:
  test5_3_c1 = test5_3_c;
  __test5_3_c1_offset1 = 0;
  __test5_3_c1_offset0 = 0;
  int test5_3_d[64][16];
  int __test5_3_d1_offset0 = 0;
  int __test5_3_d1_offset1 = 0;
  int (*test5_3_d1)[16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_3_d[0] + i;
  }
  test5_3_d1 = test5_3_d;
  __test5_3_d1_offset1 = 0;
  __test5_3_d1_offset0 = 0;
  LABEL_test5_3_d:
  ;
  int test5_3_e[16][64];
  for (int (*test5_3_e1)[64] = test5_3_e; ; ) {
  }
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_3_e[i];
  }
  int test5_3_f[16][64];
  int __test5_3_f1_offset0 = 0;
  int __test5_3_f1_offset1 = 0;
  int (*test5_3_f1)[64];
  for (; test5_3_f1 = test5_3_f; ) {
  }
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_3_f[i];
  }
  int test5_3_g[16][64];
  int (*test5_3_g1)[64];
  for (; ; test5_3_g1 = test5_3_g) {
  }
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_3_g[i];
  }
  int test5_3_h[16][64];
  int (*test5_3_h1)[64];
  for (; test5_3_h1 = test5_3_h + 1; test5_3_h1 = test5_3_h) {
  }
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_3_h[i];
  }
  int test5_3_i[64][16];
  int __test5_3_i1_offset0 = 0;
  int __test5_3_i1_offset1 = 0;
  int (*test5_3_i1)[16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_3_i[0] + i;
  }
  int t;
  switch(t){
    case 1:
    test5_3_i1 = test5_3_i;
    __test5_3_i1_offset1 = 0;
    __test5_3_i1_offset0 = 0;
    case 2:
    ;
    test5_3_i1 = test5_3_i;
    __test5_3_i1_offset1 = 1;
    __test5_3_i1_offset0 = 0;
    break; 
    default:
    test5_3_i1 = test5_3_i;
    __test5_3_i1_offset1 = 2;
    __test5_3_i1_offset0 = 0;
  }
}
int __test5_4_c1_offset0;

void sub5_4_e(int (*sub5_4_e)[16],int __sub5_4_e_offset0,int __sub5_4_e_offset11,int __sub5_4_e_offset01)
{
  int __sub5_4_e_offset1;
}
#pragma ACCEL kernel

void test5_4()
// insert offset variable - name conflict
{
  int test5_4_a[64][16];
  int __test5_4_a1_offset01 = 0;
  int __test5_4_a1_offset1 = 0;
  int (*test5_4_a1)[16] = test5_4_a;
  __test5_4_a1_offset1 = 0;
  __test5_4_a1_offset01 = 0;
  int __test5_4_a1_offset0;
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_4_a[0] + i;
  }
  int test5_4_b[64][16];
  int __test5_4_b1_offset02 = 0;
  int __test5_4_b1_offset1 = 0;
  int (*test5_4_b1)[16] = test5_4_b;
  __test5_4_b1_offset1 = 0;
  __test5_4_b1_offset02 = 0;
  for (int i = 0; i < 16; i++) {
    int __test5_4_b1_offset0;
    int __test5_4_b1_offset01;
    
#pragma ACCEL parallel
    test5_4_b[0] + i;
  }
  int test5_4_c[64][16];
  int __test5_4_c1_offset0 = 0;
  int __test5_4_c1_offset1 = 0;
  int (*test5_4_c1)[16] = test5_4_c;
  __test5_4_c1_offset1 = 0;
  __test5_4_c1_offset0 = 0;
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_4_c[0] + i;
  }
  int __test5_4_d1_offset0;
  int __test5_4_d1_offset00;
  int __test5_4_d1_offset01;
  int __test5_4_d1_offset03;
{
    int test5_4_d[64][16];
    int __test5_4_d1_offset0 = 0;
    int __test5_4_d1_offset1 = 0;
    int (*test5_4_d1)[16] = test5_4_d;
    __test5_4_d1_offset1 = 0;
    __test5_4_d1_offset0 = 0;
    for (int i = 0; i < 16; i++) {
      
#pragma ACCEL parallel
      test5_4_d[0] + i;
    }
  }
  int test5_4_e[64][16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_4_e[0] + i;
  }
  sub5_4_e(test5_4_e,0,0,0);
}
#pragma ACCEL kernel

void test5_5()
// insert offset variable - Initializer
{
  int test5_5_a[64][16];
  int __test5_5_a1_offset0 = 0;
  int __test5_5_a1_offset1 = 0;
  int (*test5_5_a1)[16] = test5_5_a;
  __test5_5_a1_offset1 = 0;
  __test5_5_a1_offset0 = 0;
  int a = 0;
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_5_a[0] + i;
  }
// test5_5_b // initializer, moved to cpp
  int test5_5_c[64][16];
  int __test5_5_c1_offset0 = 0;
  int __test5_5_c1_offset1 = 0;
  int (*test5_5_c1)[16] = 0;
  test5_5_c1 = test5_5_c;
  __test5_5_c1_offset1 = 0;
  __test5_5_c1_offset0 = 0;
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_5_c[0] + i;
  }
  typedef int (*T)[64];
  int test5_5_d[16][64];
  int __test5_5_d1_offset0 = 0;
  int __test5_5_d1_offset1 = 0;
  int (*test5_5_d1)[64] = (T )1;
  test5_5_d1 = test5_5_d;
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_5_d[i];
  }
  int test5_5_e[16][64];
  int __test5_5_e1_offset0 = 0;
  int __test5_5_e1_offset1 = 0;
  int *test5_5_e1 = (int *)1;
  test5_5_e1 = test5_5_e[1];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_5_e[i];
  }
  int test5_5_f[16][64];
  int __test5_5_f1_offset0 = 0;
  int __test5_5_f1_offset1 = 0;
  int *test5_5_f1 = 0;
  test5_5_f1 = test5_5_f[0];
  __test5_5_f1_offset1 = 1;
  __test5_5_f1_offset0 = 0;
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_5_f[i];
  }
}
#pragma ACCEL kernel

void test5_6()
//  pntr structure
{
  int test5_6_a[16][32][64][8];
  int __test5_6_a1_offset0 = 0;
  int __test5_6_a1_offset1 = 0;
  int __test5_6_a1_offset2 = 0;
  int __test5_6_a1_offset3 = 0;
  int (*test5_6_a1)[32][64][8] = test5_6_a;
  __test5_6_a1_offset3 = 0;
  __test5_6_a1_offset2 = 0;
  __test5_6_a1_offset1 = 0;
  __test5_6_a1_offset0 = 0;
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_6_a[0][0][0][i];
  }
  test5_6_a1[1][1][1][1];
  test5_6_a1[2][3][4] + 1;
  test5_6_a1[2][7][0] + 1;
  test5_6_a1[2][7][11][1];
  int __test5_6_aa1_offset0 = 0;
  int __test5_6_aa1_offset1 = 0;
  int __test5_6_aa1_offset2 = 0;
  int __test5_6_aa1_offset3 = 0;
  int (*test5_6_aa1)[32][64][8] = test5_6_a1;
  __test5_6_aa1_offset3 = 1 + __test5_6_a1_offset3;
  __test5_6_aa1_offset2 = 2 + __test5_6_a1_offset2;
  __test5_6_aa1_offset1 = 3 + 5 + __test5_6_a1_offset1;
  __test5_6_aa1_offset0 = __test5_6_a1_offset0;
  int __test5_6_aa2_offset0 = 0;
  int __test5_6_aa2_offset1 = 0;
  int __test5_6_aa2_offset2 = 0;
  int __test5_6_aa2_offset3 = 0;
  int (*test5_6_aa2)[32][64][8] = test5_6_a1;
  __test5_6_aa2_offset3 = 1 + __test5_6_a1_offset3;
  __test5_6_aa2_offset2 = 2 + 5 + __test5_6_a1_offset2;
  __test5_6_aa2_offset1 = __test5_6_a1_offset1;
  __test5_6_aa2_offset0 = __test5_6_a1_offset0;
  int test5_6_b[16][32][64][8];
  int __test5_6_b1_offset0 = 0;
  int __test5_6_b1_offset1 = 0;
  int __test5_6_b1_offset2 = 0;
  int __test5_6_b1_offset3 = 0;
  int (*test5_6_b1)[32][64][8] = test5_6_b;
  __test5_6_b1_offset3 = 0;
  __test5_6_b1_offset2 = 0;
  __test5_6_b1_offset1 = 0;
  __test5_6_b1_offset0 = 0;
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_6_b[0][0][0][i];
  }
  test5_6_b1[2][3][9] + 1;
}
#pragma ACCEL kernel

void test5_6a()
//  pntr structure
{
  int test5_6a1_a0[16][8];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_6a1_a0[0][i];
  }
  int __test5_6a1_a01_offset0 = 0;
  int __test5_6a1_a01_offset1 = 0;
  int (*test5_6a1_a01)[8] = test5_6a1_a0;
  __test5_6a1_a01_offset1 = 2;
  __test5_6a1_a01_offset0 = 0;
  int n;
  test5_6a1_a01[1][2] + n;
  int test5_6a1_a1[16][8];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_6a1_a1[0][i];
  }
  int __test5_6a1_a11_offset0 = 0;
  int __test5_6a1_a11_offset1 = 0;
  int (*test5_6a1_a11)[8] = test5_6a1_a1;
  __test5_6a1_a11_offset1 = 2;
  __test5_6a1_a11_offset0 = 0;
  test5_6a1_a11[1][2]++;
  int test5_6a_a[16][8];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_6a_a[0][i];
  }
  int __test5_6a_a1_offset0 = 0;
  int __test5_6a_a1_offset1 = 0;
  int (*test5_6a_a1)[8] = test5_6a_a;
  __test5_6a_a1_offset1 = 1;
  __test5_6a_a1_offset0 = 0;
  int __test5_6a_a2_offset0 = 0;
  int __test5_6a_a2_offset1 = 0;
  int (*test5_6a_a2)[8] = test5_6a_a;
  __test5_6a_a2_offset1 = 2;
  __test5_6a_a2_offset0 = 0;
  test5_6a_a1[1][1] - test5_6a_a2[1][2];
  int test5_6a_b[16][8];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_6a_b[0][i];
  }
  int __test5_6a_b2_offset0 = 0;
  int __test5_6a_b2_offset1 = 0;
  int (*test5_6a_b2)[8] = test5_6a_b;
  __test5_6a_b2_offset1 = 2;
  __test5_6a_b2_offset0 = 0;
  int __test5_6a_b1_offset0 = 0;
  int __test5_6a_b1_offset1 = 0;
  int (*test5_6a_b1)[8] = test5_6a_b;
  __test5_6a_b1_offset1 = 1;
  __test5_6a_b1_offset0 = 0;
  test5_6a_b1[1][1] - test5_6a_b2[1][2];
  int test5_6a_c[16][8];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_6a_c[0][i];
  }
  int __test5_6a_c2_offset0 = 0;
  int __test5_6a_c2_offset1 = 0;
  int (*test5_6a_c2)[8] = test5_6a_c;
  __test5_6a_c2_offset1 = 2;
  __test5_6a_c2_offset0 = 0;
  test5_6a_c2[1][2]++;
  int test5_6a_d[16][8];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_6a_d[0][i];
  }
  int __test5_6a_d1_offset0 = 0;
  int __test5_6a_d1_offset1 = 0;
  int (*test5_6a_d1)[8] = test5_6a_d;
  __test5_6a_d1_offset1 = 1;
  __test5_6a_d1_offset0 = 0;
  int __test5_6a_d2_offset0 = 0;
  int __test5_6a_d2_offset1 = 0;
  int (*test5_6a_d2)[8] = test5_6a_d;
  __test5_6a_d2_offset1 = 2;
  __test5_6a_d2_offset0 = 0;
  test5_6a_d1[1][1] - test5_6a_d2[1][2] - test5_6a_d1[1][1];
  -test5_6a_d2[1][2] - test5_6a_d1[1][1] - test5_6a_d2[1][2];
  int test5_6a_e[8][16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_6a_e[i][0];
  }
  int *test5_6a_e1 = test5_6a_e[1];
  int *test5_6a_e2 = test5_6a_e[2];
  test5_6a_e1 - test5_6a_e2;
  int test5_6a_f[8][16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_6a_f[i][0];
  }
  int *test5_6a_f2 = test5_6a_f[2];
  int *test5_6a_f1 = test5_6a_f[1];
  test5_6a_f1 - test5_6a_f2;
  int test5_6a_g[8][16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_6a_g[i][0];
  }
  int *test5_6a_g2 = test5_6a_g[2];
  (test5_6a_g2++)[1];
}
#pragma ACCEL kernel

void test5_7()
// pntr dim vs array dim vs max_para_dim
{
  int test5_7_a[8][32][64][16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_7_a[0][0][0][i];
  }
  int __test5_7_a1_offset0 = 0;
  int __test5_7_a1_offset1 = 0;
  int __test5_7_a1_offset2 = 0;
  int (*test5_7_a1)[32][64][16] = test5_7_a;
  __test5_7_a1_offset2 = 0;
  __test5_7_a1_offset1 = 0;
  __test5_7_a1_offset0 = 0;
  int __test5_7_a2_offset0 = 0;
  int __test5_7_a2_offset1 = 0;
  int __test5_7_a2_offset2 = 0;
  int (*test5_7_a2)[64][16] = test5_7_a[1];
  __test5_7_a2_offset2 = 0;
  __test5_7_a2_offset1 = 0;
  __test5_7_a2_offset0 = 0;
  ;
  int __test5_7_a3_offset0 = 0;
  int __test5_7_a3_offset1 = 0;
  int __test5_7_a3_offset2 = 0;
  int (*test5_7_a3)[64][16] = test5_7_a[1];
  __test5_7_a3_offset2 = 2;
  __test5_7_a3_offset1 = 0;
  __test5_7_a3_offset0 = 0;
  ;
  int __test5_7_a4_offset0 = 0;
  int __test5_7_a4_offset1 = 0;
  int __test5_7_a4_offset2 = 0;
  int (*test5_7_a4)[64][16] = test5_7_a[1];
  __test5_7_a4_offset2 = 2;
  __test5_7_a4_offset1 = 3;
  __test5_7_a4_offset0 = 0;
  ;
  int test5_7_b[16][32][64][8];
  int test5_7_ba[8][32][64][16];
  int test5_7_bb[8][16][64][32];
  int test5_7_bc[8][16][32][64];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_7_b[0][0][0][i];
    test5_7_ba[0][0][0][i];
    test5_7_bb[0][0][0][i];
    test5_7_bc[0][0][0][i];
  }
  int __test5_7_b1_offset0 = 0;
  int __test5_7_b1_offset1 = 0;
  int __test5_7_b1_offset2 = 0;
  int __test5_7_b1_offset3 = 0;
  int (*test5_7_b1)[32][64][8] = test5_7_b;
  __test5_7_b1_offset3 = 1;
  __test5_7_b1_offset2 = 2;
  __test5_7_b1_offset1 = 0;
  __test5_7_b1_offset0 = 0;
  ;
  int __test5_7_b1a_offset0 = 0;
  int __test5_7_b1a_offset1 = 0;
  int __test5_7_b1a_offset2 = 0;
  int (*test5_7_b1a)[64][16] = test5_7_ba[1];
  __test5_7_b1a_offset2 = 2;
  __test5_7_b1a_offset1 = 0;
  __test5_7_b1a_offset0 = 0;
  ;
  int __test5_7_b1b_offset0 = 0;
  int __test5_7_b1b_offset1 = 0;
  int (*test5_7_b1b)[32] = test5_7_bb[1][2];
  __test5_7_b1b_offset1 = 0;
  __test5_7_b1b_offset0 = 0;
  ;
  int (*test5_7_b1c)[64] = test5_7_bc[1][2];
  ;
  int test5_7_c[8][16][64][32];
  int test5_7_ca[16][64][32];
  int test5_7_cb[64][32];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_7_c[0][0][0][i];
    test5_7_ca[0][0][i];
    test5_7_cb[0][i];
  }
  int __test5_7_c1_offset0 = 0;
  int __test5_7_c1_offset1 = 0;
  int (*test5_7_c1)[32];
  test5_7_c1 = test5_7_c[1][2];
  __test5_7_c1_offset1 = 0;
  __test5_7_c1_offset0 = 0;
  test5_7_c1 = test5_7_ca[1];
  __test5_7_c1_offset1 = 0;
  __test5_7_c1_offset0 = 0;
  test5_7_c1 = test5_7_cb;
  __test5_7_c1_offset1 = 1;
  __test5_7_c1_offset0 = 0;
}

void sub5_8_a1(int (*sub5_8_a1)[32][64][16],int __sub5_8_a1_offset2,int __sub5_8_a1_offset1,int __sub5_8_a1_offset0)
{
  sub5_8_a1[1][3][4][2];
}

void sub5_8_a2(int (*sub5_8_a2)[64][16],int __sub5_8_a2_offset2,int __sub5_8_a2_offset1,int __sub5_8_a2_offset0)
{
  sub5_8_a2[3][4][2];
}

void sub5_8_a3(int (*sub5_8_a3)[64][16],int __sub5_8_a3_offset2,int __sub5_8_a3_offset1,int __sub5_8_a3_offset0)
{
  sub5_8_a3[3][4][2];
}

void sub5_8_a4(int (*sub5_8_a4)[64][16],int __sub5_8_a4_offset2,int __sub5_8_a4_offset1,int __sub5_8_a4_offset0)
{
  sub5_8_a4[3][4][2];
}

void sub5_8_b1(int (*sub5_8_b1)[32][64][16],int __sub5_8_b1_offset2,int __sub5_8_b1_offset1,int __sub5_8_b1_offset0)
{
  sub5_8_b1[1][3][4][2];
}

void sub5_8_b2(int (*sub5_8_b2)[64][16],int __sub5_8_b2_offset2,int __sub5_8_b2_offset1,int __sub5_8_b2_offset0)
{
  sub5_8_b2[3][4][2];
}

void sub5_8_b3(int (*sub5_8_b3)[64][16],int __sub5_8_b3_offset2,int __sub5_8_b3_offset1,int __sub5_8_b3_offset0)
{
  sub5_8_b3[3][4][2];
}

void sub5_8_b4(int (*sub5_8_b4)[64][16],int __sub5_8_b4_offset2,int __sub5_8_b4_offset1,int __sub5_8_b4_offset0)
{
  sub5_8_b4[3][4][2];
}

void sub5_8_b5(int (*sub5_8_b5)[32][64][8],int __sub5_8_b5_offset3,int __sub5_8_b5_offset2,int __sub5_8_b5_offset1,int __sub5_8_b5_offset0)
{
  sub5_8_b5[2][3][4][1];
}

void sub5_8_b5a(int (*sub5_8_b5a)[64][16],int __sub5_8_b5a_offset2,int __sub5_8_b5a_offset1,int __sub5_8_b5a_offset0)
{
  sub5_8_b5a[3][4][2];
}

void sub5_8_b5b(int (*sub5_8_b5b)[32],int __sub5_8_b5b_offset1,int __sub5_8_b5b_offset0)
{
  sub5_8_b5b[4][3];
}

void sub5_8_b5c(int (*sub5_8_b5c)[64])
{
  sub5_8_b5c[3][4];
}

void sub5_8_c1(int (*sub5_8_c1)[32],int __sub5_8_c1_offset1,int __sub5_8_c1_offset0)
{
  sub5_8_c1[4 + __sub5_8_c1_offset0][3 + __sub5_8_c1_offset1];
}
#pragma ACCEL kernel

void test5_8()
// pntr dim vs array dim vs max_para_dim in function call
{
  int test5_8_a[8][32][64][16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_8_a[0][0][0][i];
  }
  sub5_8_a1(test5_8_a,0,0,0);
  sub5_8_a2(test5_8_a[1],0,0,0);
  sub5_8_a3(test5_8_a[1],2,0,0);
  sub5_8_a4(test5_8_a[1],2,3,0);
  int test5_8_b[8][32][64][16];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_8_b[0][0][0][i];
  }
  sub5_8_b1(test5_8_b,0,0,0);
  sub5_8_b2(test5_8_b[1],0,0,0);
  sub5_8_b3(test5_8_b[1],2,0,0);
  sub5_8_b4(test5_8_b[1],2,3,0);
  int test5_8_b5[16][32][64][8];
  int test5_8_b5a[8][32][64][16];
  int test5_8_b5b[8][16][64][32];
  int test5_8_b5c[8][16][32][64];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_8_b5[0][0][0][i];
    test5_8_b5a[0][0][0][i];
    test5_8_b5b[0][0][0][i];
    test5_8_b5c[0][0][0][i];
  }
  sub5_8_b5(test5_8_b5,1,2,0,0);
  sub5_8_b5a(test5_8_b5a[1],2,0,0);
  sub5_8_b5b(test5_8_b5b[1][2],0,0);
  sub5_8_b5c(test5_8_b5c[1][2]);
  int test5_8_c[8][16][64][32];
  int test5_8_ca[16][64][32];
  int test5_8_cb[64][32];
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_8_c[0][0][0][i];
    test5_8_ca[0][0][i];
    test5_8_cb[0][i];
  }
  sub5_8_c1(test5_8_c[1][2],0,0);
  sub5_8_c1(test5_8_ca[1],0,0);
  sub5_8_c1(test5_8_cb,1,0);
}
#else  // FOCUS
#endif  // FOCUS
//#define FOCUS 0
#if !FOCUS
#pragma ACCEL kernel

void test5_5_cpp()
// insert offset variable - Initializer
{
  int test5_5_b[64][16];
  int __test5_5_b1_offset0 = 0;
  int __test5_5_b1_offset1 = 0;
  int (*test5_5_b1)[16] = test5_5_b;
  __test5_5_b1_offset1 = 0;
  __test5_5_b1_offset0 = 0;
  for (int i = 0; i < 16; i++) {
    
#pragma ACCEL parallel
    test5_5_b[0] + i;
  }
}
#else
#endif
