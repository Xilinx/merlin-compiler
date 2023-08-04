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
  int test1_1_a[16][17][18];

  int (*b)[18] = test1_1_a[0];

  b = test1_1_a[0] + 1;
  int c[8][16][32];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    test1_1_a[1][0][i]  = 0;
    test1_1_a[i];
    c[0][i];
  }

  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    b[1][i]=0;
    b[1];
  }
}

void sub1_1(int (*sub1_1a)[32], int i);
#pragma ACCEL kernel
void test_1_1()
{
  int test1_1_a[16];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    test1_1_a[i];
  }

  int test1_1_b[16][32];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    test1_1_b[i][0];
  }

  int test1_1_c[16][32];
  for (int i = 0; i < 16; i ++)
  for (int j = 0; j < 16; j ++)
  {
#pragma ACCEL parallel 
    test1_1_c[i][0];
  }

  int test1_1_d[16][32];
  for (int j = 0; j < 16; j ++)
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    test1_1_d[i][0];
  }

  int test1_1_e[16][32];
  for (int j = 0; j < 16; j ++)
  {
#pragma ACCEL parallel 
    for (int i = 0; i < 16; i ++)
    {
#pragma ACCEL parallel 
      test1_1_e[i][j];
    }
  }

  int test1_1_f[16][32][64];
  for (int j = 0; j < 16; j ++)
  {
#pragma ACCEL parallel 
    for (int i = 0; i < 16; i ++)
    {
#pragma ACCEL parallel 
      test1_1_f[i][0][j];
    }
  }


  int test1_1_g[16][32][64];
  for (int j = 0; j < 16; j ++)
  {
    for (int i = 0; i < 16; i ++)
    {
#pragma ACCEL parallel 
      test1_1_g[i][0][j];
    }
    for (int i = 0; i < 16; i ++)
    {
      test1_1_g[i][0][j];
    }
  }

  int test1_1_h[16][32];
  for (int j = 0; j < 16; j ++) 
#pragma ACCEL parallel 
  for (int i = 0; i < 16; i ++)
  {
    test1_1_h[j][i];
  }

  int test1_1_i[16][32];
  int test1_1_ia[16][32];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    test1_1_i[i][0];
    sub1_1(test1_1_ia,i);
  }
}

void sub1_1(int (*sub1_1a)[32], int i)
{
  sub1_1a[i][0];
  int sub1_1b[16][32];
  for (int t = 0; t < 16; t++)
  {
#pragma ACCEL parallel 
    sub1_1b[t][0];
  }
}

#pragma ACCEL kernel
void test_1_2()
{
  int test1_2_a[16][32];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel factor=4
    test1_2_a[i][0];
  }

  int test1_2_b[16][32];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL pipeline_parallel 
    test1_2_b[i][0];
  }

  int test1_2_c[16][32];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL pipeline parallel 
    test1_2_c[i][0];
  }
}



#pragma ACCEL kernel
void test_2_1()
{
  int test2_1_a[16];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel
    test2_1_a[i];
  }

  int test2_1_b[16][32];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel
    test2_1_b[i][0];
  }

  int test2_1_c[16][32][64];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    test2_1_c[i][1][2];
  }

  int (*test2_1_d1)[32][64];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    test2_1_d1[i][1][2];
  }

  int (*test2_1_d2[16])[64];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    test2_1_d2[i][1][2];
  }

  int *test2_1_d3[16][32];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    test2_1_d3[i][1][2];
  }

  int (**test2_1_e1)[16];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    test2_1_e1[i][1][2];
  }

  int *(*test2_1_e2)[32];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    test2_1_e2[i][1][2];
  }

  int **test2_1_e3[64];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    test2_1_e3[i][1][2];
  }

  int ***test2_1_f;
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    test2_1_f[i][1][2];
  }
}


#pragma ACCEL kernel
void test_2_2()
{

  int test2_2_a[16][32][64];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel
    test2_2_a[i][1][2];
  }

  int test2_2_b[16][32][64];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel
    test2_2_b[1][i][2];
  }

  int test2_2_c[16][32][64];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel
    test2_2_c[1][2][i];
  }

  int test2_2_d[16][32][64];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel
    test2_2_d[1][i][i];
  }

  int test2_2_e[16][32][64];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel
    test2_2_e[i][2][i];
  }

  int test2_2_f[16][32][64];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel
    test2_2_f[i][i][2];
  }

  int test2_2_g[16][32][64];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel
    test2_2_g[i][i][i];
  }

  int test2_2_h[16][32][64];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel
    test2_2_h[i][1][2];
    test2_2_h[0][i][2];
  }

  int test2_2_h1[16][32][64];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel
    test2_2_h1[i][1][2];
  }
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel
    test2_2_h1[0][i][2];
  }

  // the following is related to pointer dim
  
  int test2_2_i[16][32][64];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel
    test2_2_i[i][1][2];
  }
  test2_2_i[0];
  test2_2_i[0][1];
  test2_2_i[0][1][2];

  int test2_2_j[16][32][64];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel
    test2_2_j[0][i][2];
  }
  test2_2_j[0];
  test2_2_j[0][1];
  test2_2_j[0][1][2];

  int test2_2_k[16][32][64];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel
    test2_2_k[i][0][2];
  }
  test2_2_k[3];
  test2_2_k[3][1];
  test2_2_k[3][1][2];
}

void sub2_3_l(double (* sub2_3_la)[10], int j) {
  sub2_3_la[j][0];
}

int g_test2_3_a[16][32];
int g_test2_3_a1[16][32];
#pragma ACCEL kernel
void test_2_3(int test2_3_b[16][32])
{
  int (* test2_3_a1)[32] = g_test2_3_a1;
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    g_test2_3_a[i][2];
    test2_3_a1[i];
  }

  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    test2_3_b[i][2];
  }

  int test2_3_c[16][32][48];
  int (*test2_3_c1) = test2_3_c[0][1];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    test2_3_c[1][i][2];
  }

  int test2_3_d[16][32][48];
  int **test2_3_d1;
  test2_3_d1[0] = test2_3_d[0][1];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    test2_3_d[1][i][2];
  }

  int test2_3_e[16][32][48];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    1, test2_3_e[1][i];
  }

  int test2_3_e1[16][32][48];
  int (* test2_3_e1a)[48] = test2_3_e1[0];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    test2_3_e1[1][i];
  }
  &test2_3_e1a;

  int test2_3_f[16][32][48];
  int (**test2_3_f1)[32][48];
  test2_3_f1[0] = test2_3_f;
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    test2_3_f[1][i][2];
  }

  int **test2_3_g;
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    test2_3_g[i][0];
  }

  int test2_3_h[16][32][48];
  int (*test2_3_h1)[48] = test2_3_h[1];
  int *test2_3_h2 = test2_3_h1[1];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    test2_3_h[i][2][3];
  }

  int n; 
  int test2_3_i[16][32][n];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    test2_3_i[i][1][2];
  }

  int test2_3_j[16][32][10];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    test2_3_j[1][i][i];
  }

  int test2_3_k[16][32][10];
  int test2_3_k1[16][32][10];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    (double)test2_3_k[1][i][0];
    (double*)test2_3_k1[1][i];
  }

// Disable this case because data type difference is ignored in this pass
//  int test2_3_l[32][10];
//  for (int i = 0; i < 16; i ++)
//  {
//#pragma ACCEL parallel 
//    sub2_3_l(test2_3_l, i);
//  }

}

void sub2_3a_a1(int (* sub2_3a_a1)[16][32][64]) { sub2_3a_a1[1][2][3][4] = 0; }
void sub2_3a_a2(int (* sub2_3a_a2)    [32][64]) { sub2_3a_a2   [2][3][4] = 0; }
void sub2_3a_b1(int (* sub2_3a_b1)[16][32][64]) { sub2_3a_b1[1][2][3][4]++; }
void sub2_3a_b2(int (* sub2_3a_b2)    [32][64]) { sub2_3a_b2   [2][3][4]++; }

typedef int (*type2_3a_c1)[64];
void sub2_3a_c1(type2_3a_c1 * sub2_3a_c1      ) { }
typedef int (type2_3a_d1)[8][16][32][64];
void sub2_3a_d1(type2_3a_d1 * sub2_3a_d1      ) { sub2_3a_d1[0][0][0][0][0] = 0; }

#pragma ACCEL kernel
void test2_3a() {

  int test2_3a_a[8][16][32][64];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test2_3a_a[0][i][0][0];
  }
  sub2_3a_a1(test2_3a_a);
  sub2_3a_a2(test2_3a_a[1]);

  int test2_3a_b[8][16][32][64];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test2_3a_b[0][i][0][0];
  }

  sub2_3a_b1(test2_3a_b);
  sub2_3a_b2(test2_3a_b[1]);

  int test2_3a_c[8][16][32][64];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test2_3a_c[0][i][0][0];
  }
  int (* test2_3a_c1)[64]  = test2_3a_c [1][2];
  sub2_3a_c1(&test2_3a_c1);

  int test2_3a_d[8][16][32][64];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test2_3a_d[0][i][0][0];
  }
  sub2_3a_d1(&test2_3a_d);

  int test2_3a_e[8][16][32][64];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test2_3a_e[0][0][i][0];
  }
  int (** test2_3a_e1)[32][64]; 
  test2_3a_e1[0] = test2_3a_e[1]; 
  test2_3a_e1[1][0][0];

}

int sub2_4_e(int * a) ;

#pragma ACCEL kernel
void test_2_4()
{
  int test2_4_a[16][32][48];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    test2_4_a[0][i+1];
  }

  int test2_4_b[16][32][48];
  for (int i = 0; i < 16; i ++) {
#pragma ACCEL parallel 
    test2_4_b[0][i+1];
    test2_4_b[0][i];
  }

  int test2_4_c[16][32][48];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    test2_4_c[0][i++];
  }

  int test2_4_d[16][32][48];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    test2_4_d[0][i][i++];
  }

  int test2_4_e[16][32][48];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    test2_4_e[0][i][sub2_4_e(&i)];
  }

  int test2_4_f[16][32][48];
  for (int i = 0; i < 16; i ++)
  {
    int j;
#pragma ACCEL parallel 
    test2_4_f[0][i][1 + (j=0)];
  }
}

int sub2_4_e(int * a) { *a = 3; return 0; }

#pragma ACCEL kernel
void test_2_5()
{
  int test2_5_a[16][32];
  int test2_5_a1[16][32];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    test2_5_a[i];
  }
  test2_5_a1[test2_5_a[1][2]];

  int test2_5_b[16][32];
  int test2_5_b1[16][32];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    test2_5_b[i];
  }
  test2_5_b[test2_5_b1[1][2]];

  int test2_5_c[16][32];
  int test2_5_c1[16][32];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    test2_5_c[i];
    test2_5_c1[i];
  }
  test2_5_c[test2_5_c1[1][2]];

  int test2_5_d[16][32];
  int test2_5_d1[16][32];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    test2_5_d[i];
    test2_5_d1[i];
  }
  test2_5_d1[test2_5_d[1][2]];
  
  int test2_5_e[16][32];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    test2_5_e[i];
  }
  test2_5_e[test2_5_e[1][2]];

  int test2_5_f[16][32];
  int * test2_5_f1 = test2_5_f[0] + test2_5_f[1][2];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    test2_5_f[i];
  }

}

void sub3_1_a(int * sub3_1_a)
{
  sub3_1_a[0];
}

void sub3_1_b(int sub3_1_b[32], int j)
{
  sub3_1_b[j];
}
void sub3_1_c(int (*sub3_1_c)[32], int j)
{
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
  sub3_1_c[i][j];
  }
}
void sub3_1_ds(int (*sub3_1_ds)[32][48], int i, int j)
{
  for (int t = 0; t < 16; t ++)
  {
    sub3_1_ds[i][j][t];
  }
}
void sub3_1_d(int (*sub3_1_d)[32][48], int j)
{
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    sub3_1_ds(sub3_1_d, j, i);
  }
}

#pragma ACCEL kernel
void test3_1()
{

  int test3_1_a[16][32];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    sub3_1_a(test3_1_a[i]);
  }

  int test3_1_b[16][32];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    sub3_1_b(test3_1_b[0], i);
  }

  int test3_1_c[16][32];
  sub3_1_c(test3_1_c, 0);

  int test3_1_d[16][32][48];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    sub3_1_d(test3_1_d, i);
  }
}

void sub3_2_a(int sub3_2_a[16][32][48], int i)
{
  sub3_2_a[1][i][2];
}

void sub3_2_b(int sub3_2_b[32][48], int i)
{
  sub3_2_b[i][2];
}

void sub3_2_c(int sub3_2_c[48], int i)
{
  sub3_2_c[0];
}

void sub3_2_d(int sub3_2_d, int i)
{
  sub3_2_d;
}


#pragma ACCEL kernel
void test3_2()
{
  int test3_2_a[16][32][48];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    sub3_2_a(test3_2_a, i);
  }

  int test3_2_b[16][32][48];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    sub3_2_b(test3_2_b[0], i);
  }

  int test3_2_c[16][32][48];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    sub3_2_c(test3_2_c[0][i], i);
  }

  int test3_2_d[16][32][48];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    sub3_2_d(test3_2_d[0][i][1], i);
  }

}

void sub3_3_a(int sub3_3_a[16][32][48], int j)
{
  sub3_3_a[1][j][2];
}
void sub3_3_b(int sub3_3_b[16][32][48], int j)
{
  sub3_3_b[1][j][2];
}
void sub3_3_c(int sub3_3_c[16][32][48], int j)
{
  sub3_3_c[1][j][2];
}
void sub3_3_d(int sub3_3_d[16][32][48], int sub3_3_d1[16][32][48], int j)
{
  sub3_3_d[j][1][2];
  sub3_3_d1[1][j][2];
}
void sub3_3_e(int sub3_3_e[16][32][48], int j)
{
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    sub3_3_e[j][i][2];
  }
}
void sub3_3_f(int sub3_3_f[16][32][48], int k)
{
  sub3_3_f[1][k][2];
}
void sub3_3_f1(int sub3_3_f1[16][32][48], int j)
{
  sub3_3_f1[j][0][2];
}


#pragma ACCEL kernel
void test3_3()
{
  int test3_3_a[16][32][48];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    sub3_3_a(test3_3_a, i);
  }
  sub3_3_a(test3_3_a, 1);

  int test3_3_b[16][32][48];
  int test3_3_b1[16][32][48];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    sub3_3_b(test3_3_b, i);
  }
  sub3_3_b(test3_3_b1, 1);

  int test3_3_c[16][32][48];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    sub3_3_c(test3_3_c, i);
    sub3_3_c(test3_3_c, i);
  }

  int test3_3_d[16][32][48];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    sub3_3_d(test3_3_d, test3_3_d, i);
  }

  int test3_3_e[16][32][48];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    sub3_3_e(test3_3_e, i);
    sub3_3_e(test3_3_e, 1);
  }

  int test3_3_f[16][32][48];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    sub3_3_f(test3_3_f, i);
  }
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    sub3_3_f1(test3_3_f, i);
  }

}


void sub3_4_a2(int sub3_4_a2[16][32][48], int j)
{
  sub3_4_a2[j][0][2];
}

void sub3_4_a1(int sub3_4_a1[16][32][48], int j)
{
  sub3_4_a1[0][2];
  sub3_4_a2(sub3_4_a1, j);
}

void sub3_4_a(int sub3_4_a[16][32][48], int j)
{
  sub3_4_a1(sub3_4_a, j);
  sub3_4_a2(sub3_4_a, j);
}

void sub3_4_b2(int sub3_4_b2[16][32][48], int j)
{
  sub3_4_b2[j][0][2];
}

void sub3_4_b1(int sub3_4_b1[16][32][48], int j)
{
  sub3_4_b1[0][2];
  sub3_4_b2(sub3_4_b1, j);
}

void sub3_4_b(int sub3_4_b[16][32][48], int j)
{
  sub3_4_b2(sub3_4_b, j);
  sub3_4_b1(sub3_4_b, j);
}


#pragma ACCEL kernel
void test3_4()
{
  int test3_4_a[16][32][48];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    sub3_4_a(test3_4_a, i);
  }
  sub3_4_a(test3_4_a, 1);

  int test3_4_b[16][32][48];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    sub3_4_b(test3_4_b, i);
  }
  sub3_4_b(test3_4_b, 1);

}

void sub4_1_a(int, int) ;
void sub4_1_b(int* , int) ;


#pragma ACCEL kernel
void test4_1()
{
  int test4_1_a[16][32][48];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    sub4_1_a(test4_1_a[i][0][0], i);
  }

  int test4_1_b[16][32][48];
  for (int i = 0; i < 16; i ++)
  {
#pragma ACCEL parallel 
    sub4_1_b(test4_1_b[i][0], i);
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
void test4_5(int * in_a)
{
  int test4_5_a[16][32];
  for ( int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel
    test4_5_a[i][0];
  }
  memcpy(in_a, test4_5_a[0], 16*sizeof(int));
}

void sub4_6(int sub4_6[32])
{
  __merlin_access_range(sub4_6, 0, 31UL);
}

#pragma ACCEL kernel
void test4_6(int * in_a)
{
  int test4_6_a[16][32];
  for ( int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel
    test4_6_a[i][0];
  }
  sub4_6(test4_6_a[1]);
}

void sub4_6_b(int (*sub4_6_b)[32])
{
  __merlin_access_range(sub4_6_b, 0, 0, 0, 31UL);
}

#pragma ACCEL kernel
void test4_6_b(int * in_a)
{
  int test4_6_b[16][32];
  for ( int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel
    test4_6_b[i][0];
  }
  sub4_6_b(test4_6_b);
}



void sub5_0_b(int (*sub5_0_b)[64])
{
  sub5_0_b[1];
}

#pragma ACCEL kernel
void test_5_0()
{
  int test5_0_a[16][32][64];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_0_a[i];
  }

  int test5_0_b[16][32][64];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    sub5_0_b(test5_0_b[i]);
  }

  int test5_0_c[16][32][64];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    int (* test5_0_c1)[64] = test5_0_c[i];
    test5_0_c1[1][2];   // ...
  }

  int test5_0_d[16][32][64];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    int (* test5_0_d1)[64];
    test5_0_d1 = test5_0_d[i];
    test5_0_d1[1][2];   // ...
  }

  int test5_0_e[16][32][64][96];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_0_e[1][i];
  }
}

void sub5_1_b(int (*sub5_1_b)[64])
{
  sub5_1_b[1];
}
void sub5_1_h(int (*sub5_1_h)[64])
{
  sub5_1_h[1];
}

#pragma ACCEL kernel
void test5_1()  // insert offset variable - condition to add offset variable
{ 

  int test5_1_a[16][32][64];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_1_a[i];
  }

  int test5_1_b[16][32][64];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    sub5_1_b(test5_1_b[i]);
  }

  int test5_1_c[16][32][64];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    int (* test5_1_c1)[64] = test5_1_c[i];
  }

  int test5_1_d[16][32][64];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    int (* test5_1_d1)[64];
    test5_1_d1 = test5_1_d[i];
  }

  int test5_1_e[16][32][64];
  int (* test5_1_e1)[64];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_1_e1  = test5_1_e[i];
  }
  test5_1_e1 = test5_1_e[1];

  int test5_1_f[16][32][64];
  int (* test5_1_f1)[64];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_1_f1  = test5_1_f[i];
  }
  test5_1_f1 = test5_1_f1 + 1;

  int test5_1_fa[16][32][64];
  int (* test5_1_fa1)[64];
  int (* test5_1_fa2)[64];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_1_fa1  = test5_1_fa[i];
  }
  test5_1_fa2 = test5_1_fa1 + 1;
  test5_1_fa1 = test5_1_fa2 + 1;
  
  int test5_1_fb[16][32][64];
  int (* test5_1_fb1)[64];
  int (* test5_1_fb2)[64];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_1_fb1  = test5_1_fb[i];
  }
  test5_1_fb2 = test5_1_fb1 + 1;
  test5_1_fb2 = test5_1_fb1 + 2;

  int test5_1_g[16][32][64];
  int (* test5_1_g1)[64];
  int test5_1_g2[16][32][64];
  int (* test5_1_g3)[64];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_1_g1  = test5_1_g[i];
  }
  test5_1_g1 = test5_1_g2[1];
  test5_1_g3 = test5_1_g2[1];

  int test5_1_h[16][32][64];
  int (*test5_1_h1)[64];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_1_h1 = test5_1_h[i];
  }
  test5_1_h1 = test5_1_h[1];
  sub5_1_h(test5_1_h1);
  
}


void sub5_2_i(int (*sub5_2_i)[64]);

#pragma ACCEL kernel
void test5_2()  // insert offset variable - unsupport partial pntr (for adding offset)
{ 
  int test5_2_a[16][32];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    int * test5_2_a1 = test5_2_a[i];
    test5_2_a1;
  }

  int test5_2_b[16][32][64];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    int (* test5_2_b1)[64] = test5_2_b[0] + i;
    test5_2_b1[1];
    if (test5_2_b1[1]) ;
    while (test5_2_b1[1]);
    do {} while (test5_2_b1[1]);
    for (;test5_2_b1[1];) { } ;
  }

  int test5_2_c[16][32][64];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    int (* test5_2_c1)[64] = test5_2_c[i];
    (test5_2_c1[1]) ;
    if (test5_2_c1[1]) ;
    while (test5_2_c[1]);
    for (;test5_2_c[1];) { } ;
  }

  int test5_2_d[16][32][64];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    int (* test5_2_d1)[64] = test5_2_d[i];
    test5_2_d1[1], 1;
  }

  int test5_2_e[16][32][64];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    int (* test5_2_e1)[64] = test5_2_e[i];
    test5_2_e1[1] == 0;
  }

  int test5_2_f[16][32][64];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    int (* test5_2_f1)[64] = test5_2_f[i];
    !test5_2_f1[1];
  }

  int test5_2_g[16][32][64];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    int (* test5_2_g1)[64] = test5_2_g[i];
    test5_2_g1;
  }

  int test5_2_h[16][32][64];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    int (* test5_2_h1)[64] = test5_2_h[i];
    int (* test5_2_h2)[64];

    (test5_2_h2 = test5_2_h1)[1];
  }

  int test5_2_i[16][32][64];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    int (* test5_2_i1)[64] = test5_2_i[i];
    int (* test5_2_i2)[64];

    sub5_2_i(test5_2_i2 = test5_2_i1);
  }

  int test5_2_j[16][64];
  int (* test5_2_j1)[64];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_2_j[i];
  }
  {
    int tt = (test5_2_j1 = test5_2_j) ? 1 : 0;
  }

}

#pragma ACCEL kernel
void test5_3()  // insert offset variable - insert position
{ 
  int test5_3_a[16][64];
  int (* test5_3_a1)[64];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_3_a[i];

    if (1) test5_3_a1 = test5_3_a;
    while (1) test5_3_a1 = test5_3_a;
  }

  int test5_3_b[16][64];
  int (* test5_3_b1)[64];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_3_b[i];

    if ( test5_3_b1 = test5_3_b)  { test5_3_b1[1][2]; }
    while (test5_3_b1 = test5_3_b+1 ) { test5_3_b1[1][2]; }
  }

  int test5_3_ba[16][64];
  int (* test5_3_ba1)[64];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_3_ba[i];

    if ( test5_3_ba1 = test5_3_ba)  { test5_3_ba1[1][2]; }
    else { test5_3_ba1[3][4]; }
  }

  int test5_3_c[16][64];
  int (* test5_3_c1)[64];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_3_c[i];
  }
LABEL_test5_3_c: test5_3_c1 = test5_3_c;


  int test5_3_d[16][64];
  int (* test5_3_d1)[64];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_3_d[i];
  }
  test5_3_d1 = test5_3_d;
LABEL_test5_3_d: 
  ;

  int test5_3_e[16][64];
  for (int (* test5_3_e1)[64] = test5_3_e; ; ) {}
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_3_e[i];
  }

  int test5_3_f[16][64];
  int (* test5_3_f1)[64];
  for (; test5_3_f1 = test5_3_f; ) {}
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_3_f[i];
  }

  int test5_3_g[16][64];
  int (* test5_3_g1)[64];
  for (; ; test5_3_g1 = test5_3_g ) {}
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_3_g[i];
  }

  int test5_3_h[16][64];
  int (* test5_3_h1)[64];
  for (; test5_3_h1 = test5_3_h+1; test5_3_h1 = test5_3_h ) {}
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_3_h[i];
  }

  int test5_3_i[16][64];
  int (* test5_3_i1)[64];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_3_i[i];
  }
  int t;
  switch (t) {
    case 1:
      test5_3_i1 = test5_3_i;
    case 2:
      ;
      test5_3_i1 = test5_3_i+1;
      break;
    default:
      test5_3_i1 = test5_3_i+2;
  }
}


int __test5_4_c1_offset0;

void sub5_4_e(int (*sub5_4_e)[64], int __sub5_4_e_offset0)
{
  int __sub5_4_e_offset1;
}
#pragma ACCEL kernel
void test5_4()  // insert offset variable - name conflict
{ 

  int test5_4_a[16][64];
  int (* test5_4_a1)[64] = test5_4_a;
  int __test5_4_a1_offset0;
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_4_a[i];
  }

  int test5_4_b[16][64];
  int (* test5_4_b1)[64] = test5_4_b;
  for (int i = 0; i < 16; i++)
  {
    int __test5_4_b1_offset0;
    int __test5_4_b1_offset01;
#pragma ACCEL parallel 
    test5_4_b[i];
  }

  int test5_4_c[16][64];
  int (* test5_4_c1)[64] = test5_4_c;
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_4_c[i];
  }

  int __test5_4_d1_offset0;
  int __test5_4_d1_offset00;
  int __test5_4_d1_offset01;
  int __test5_4_d1_offset03;
  {
    int test5_4_d[16][64];
    int (* test5_4_d1)[64] = test5_4_d;
    for (int i = 0; i < 16; i++)
    {
#pragma ACCEL parallel 
      test5_4_d[i];
    }
  }

  int test5_4_e[16][64];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_4_e[i];
  }
  sub5_4_e(test5_4_e, 0);

}

#pragma ACCEL kernel
void test5_5()  // insert offset variable - Initializer
{ 
  int test5_5_a[16][64];
  int (* test5_5_a1)[64] = test5_5_a, a = 0;
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_5_a[i];
  }

  // test5_5_b // initializer, moved to cpp
  
  int test5_5_c[16][64];
  int (* test5_5_c1)[64] = 0;
  test5_5_c1 = test5_5_c;
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_5_c[i];
  }

  typedef int (*T)[64];

  int test5_5_d[16][64];
  int (* test5_5_d1)[64] = (T)1;
  test5_5_d1 = test5_5_d;
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_5_d[i];
  }

  int test5_5_e[16][64];
  int * test5_5_e1 = (int*)1;
  test5_5_e1 = test5_5_e[1];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_5_e[i];
  }

  int test5_5_f[16][64];
  int * test5_5_f1 = 0;
  test5_5_f1 = test5_5_f[1];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_5_f[i];
  }
}

#pragma ACCEL kernel
void test5_6()  //  pntr structure
{ 
  int test5_6_a[8][16][32][64];
  int (* test5_6_a1)[16][32][64] = test5_6_a;
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_6_a[i][0][0][0];
  }
  test5_6_a1[1][1][1][1];
  test5_6_a1[1][2][3] + 4;
  (test5_6_a1[1][2] + 3)[4];
  ((test5_6_a1[1][2] + 3)[4]+5)[6];
  int (* test5_6_aa1)[64] = (&(test5_6_a1[1][2][3])) + 5;
  int (* test5_6_aa2)[32][64] = (&(test5_6_a1[1][2])) + 5;


  int test5_6_b[8][16][32][64];
  int (* test5_6_b1)[16][32][64] = test5_6_b;
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_6_b[i][0][0][0];
  }
  (&(test5_6_b1[1][2][3][4])) + 5;

}

#pragma ACCEL kernel
void test5_6a()  //  pntr structure
{ 
  int test5_6a1_a0[8][16];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_6a1_a0[i][0];
  }
  int * test5_6a1_a01 = test5_6a1_a0[2];
  int n ;
  test5_6a1_a01[1] + n;

  int test5_6a1_a1[8][16];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_6a1_a1[i][0];
  }
  int * test5_6a1_a11 = test5_6a1_a1[2];
  test5_6a1_a11[1]++;

  int test5_6a_a[8][16];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_6a_a[i][0];
  }
  int * test5_6a_a1 = test5_6a_a[1];
  int * test5_6a_a2 = test5_6a_a[2];
  test5_6a_a1[1] - test5_6a_a2[1];

  int test5_6a_b[8][16];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_6a_b[i][0];
  }
  int * test5_6a_b2 = test5_6a_b[2];
  int * test5_6a_b1 = test5_6a_b[1];
  test5_6a_b1[1] - test5_6a_b2[1];

  int test5_6a_c[8][16];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_6a_c[i][0];
  }
  int * test5_6a_c2 = test5_6a_c[2];
  test5_6a_c2[1]++;

  int test5_6a_d[8][16];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_6a_d[i][0];
  }
  int * test5_6a_d1 = test5_6a_d[1];
  int * test5_6a_d2 = test5_6a_d[2];
  test5_6a_d1[1] - test5_6a_d2[1] - test5_6a_d1[1] ;
  - test5_6a_d2[1] - test5_6a_d1[1] - test5_6a_d2[1];

  int test5_6a_e[8][16];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_6a_e[i][0];
  }
  int * test5_6a_e1 = test5_6a_e[1];
  int * test5_6a_e2 = test5_6a_e[2];
  test5_6a_e1 - test5_6a_e2;

  int test5_6a_f[8][16];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_6a_f[i][0];
  }
  int * test5_6a_f2 = test5_6a_f[2];
  int * test5_6a_f1 = test5_6a_f[1];
  test5_6a_f1 - test5_6a_f2;

  int test5_6a_g[8][16];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_6a_g[i][0];
  }
  int (* test5_6a_g2) = test5_6a_g[2];
  (test5_6a_g2++)[1];

}

#pragma ACCEL kernel
void test5_7()  // pntr dim vs array dim vs max_para_dim
{ 
  int test5_7_a[8][16][32][64];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_7_a[0][i][0][0];
  }

  int (* test5_7_a1)[16][32][64] = test5_7_a;
  int (* test5_7_a2)[32][64] = test5_7_a[1];;
  int (* test5_7_a3)[64] = test5_7_a[1][2];;
  int (* test5_7_a4) = test5_7_a[1][2][3];;


  int test5_7_b[8][16][32][64];
  int test5_7_ba[8][16][32][64];
  int test5_7_bb[8][16][32][64];
  int test5_7_bc[8][16][32][64];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_7_b [i][0][0][0];
    test5_7_ba[0][i][0][0];
    test5_7_bb[0][0][i][0];
    test5_7_bc[0][0][0][i];
  }
  int (* test5_7_b1)[64]  = test5_7_b [1][2];;
  int (* test5_7_b1a)[64] = test5_7_ba[1][2];;
  int (* test5_7_b1b)[64] = test5_7_bb[1][2];;
  int (* test5_7_b1c)[64] = test5_7_bc[1][2];;

  int test5_7_c[8][16][32][64];
  int test5_7_ca[16][32][64];
  int test5_7_cb[32][64];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_7_c [0][0][i][0];
    test5_7_ca   [0][i][0];
    test5_7_cb      [i][0];
  }
  int (* test5_7_c1)[64];
  test5_7_c1 = test5_7_c [1][2];
  test5_7_c1 = test5_7_ca[1];
  test5_7_c1 = test5_7_cb + 1;

}


void sub5_8_a1(int (* sub5_8_a1)[16][32][64]) { sub5_8_a1[1][2][3][4]; }
void sub5_8_a2(int (* sub5_8_a2)    [32][64]) { sub5_8_a2   [2][3][4]; }
void sub5_8_a3(int (* sub5_8_a3)        [64]) { sub5_8_a3      [3][4]; }
void sub5_8_a4(int (* sub5_8_a4)            ) { sub5_8_a4         [4]; }

void sub5_8_b1(int (* sub5_8_b1)[16][32][64]) { sub5_8_b1[1][2][3][4]; }
void sub5_8_b2(int (* sub5_8_b2)    [32][64]) { sub5_8_b2   [2][3][4]; }
void sub5_8_b3(int (* sub5_8_b3)        [64]) { sub5_8_b3      [3][4]; }
void sub5_8_b4(int (* sub5_8_b4)            ) { sub5_8_b4         [4]; }

void sub5_8_b5 (int (* sub5_8_b5 )        [64]) { sub5_8_b5       [3][4]; }
void sub5_8_b5a(int (* sub5_8_b5a)        [64]) { sub5_8_b5a      [3][4]; }
void sub5_8_b5b(int (* sub5_8_b5b)        [64]) { sub5_8_b5b      [3][4]; }
void sub5_8_b5c(int (* sub5_8_b5c)        [64]) { sub5_8_b5c      [3][4]; }

void sub5_8_c1 (int (* sub5_8_c1 )        [64]) { sub5_8_c1       [3][4]; }

#pragma ACCEL kernel
void test5_8()  // pntr dim vs array dim vs max_para_dim in function call
{ 
  int test5_8_a[8][16][32][64];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_8_a[0][i][0][0];
  }
  sub5_8_a1(test5_8_a);
  sub5_8_a2(test5_8_a[1]);
  sub5_8_a3(test5_8_a[1][2]);
  sub5_8_a4(test5_8_a[1][2][3]);

  int test5_8_b[8][16][32][64];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_8_b[0][i][0][0];
  }
  sub5_8_b1(test5_8_b);
  sub5_8_b2(test5_8_b[1]);
  sub5_8_b3(test5_8_b[1][2]);
  sub5_8_b4(test5_8_b[1][2][3]);

  int test5_8_b5[8][16][32][64];
  int test5_8_b5a[8][16][32][64];
  int test5_8_b5b[8][16][32][64];
  int test5_8_b5c[8][16][32][64];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_8_b5 [i][0][0][0];
    test5_8_b5a[0][i][0][0];
    test5_8_b5b[0][0][i][0];
    test5_8_b5c[0][0][0][i];
  }
  sub5_8_b5 (test5_8_b5 [1][2]);
  sub5_8_b5a(test5_8_b5a[1][2]);
  sub5_8_b5b(test5_8_b5b[1][2]);
  sub5_8_b5c(test5_8_b5c[1][2]);


  int test5_8_c[8][16][32][64];
  int test5_8_ca[16][32][64];
  int test5_8_cb[32][64];
  for (int i = 0; i < 16; i++)
  {
#pragma ACCEL parallel 
    test5_8_c [0][0][i][0];
    test5_8_ca   [0][i][0];
    test5_8_cb      [i][0];
  }
  sub5_8_c1(test5_8_c [1][2]);
  sub5_8_c1(test5_8_ca[1]);
  sub5_8_c1(test5_8_cb + 1);

}

#else  // FOCUS


#endif  // FOCUS


