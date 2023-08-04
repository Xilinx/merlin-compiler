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
// sub function
void sub_0(int a[100][100], int b[100][100])
{

    int i, j;
    for (i = 0; i < 100; i++) {
        if (a[0][0]) {for (j = 0; j < 100; j++) 
            a[i][j] += b[i][j];

        }
    }
}

#pragma ACCEL kernel
void func_top_7(int a[100][100],
        int b[100][100],
        int c[100][100]) {

    int i;
    int a_buf[100][100];
    int c_buf[100][100];
    for (i = 0; i < 2; ++i) {
#pragma ACCEL pipeline
        sub_0(a_buf, b);
        sub_0(a, a_buf);
        sub_0(c_buf, a_buf);
        sub_0(c, c_buf);
    }
}

#pragma ACCEL kernel
void func_top_7_1(int a[100][100], int b[100][100], int c[100][100]) {

  int i;
  int a_buf[100][100];
  int c_buf[100][100];
  for (i = 0; i < 2; ++i) {
#pragma ACCEL pipeline
    if (i > 0)
      sub_0(c_buf, a_buf);
    sub_0(a_buf, b);
    sub_0(a, a_buf);
    sub_0(c, c_buf);
  }
}

int sub_0_0(int a[100][100], int b[100][100], int c)
{
    int i, j;
    for (i = 0; i < 100; i++) {
        a[c][i] += b[i][j] + c;
    }
    return 1024;
}

#pragma ACCEL kernel
void func_top_8(int a[100][100],
        int b[100][100],
        int c[100][100]) {



    int sum = 0;
    int i = 0, j;
    int a_buf[100][100];
    for (i = 0; i < 100; ++i) {
#pragma ACCEL pipeline
        sub_0(a_buf, b);
        sum = sub_0_0(c, a_buf, i);
        for (j = 0; j < 50; ++j)
            a[j][i] = sum + a_buf[i][j];
    }
}


//Loop structure 
void func_top_4(float a[100][100], float b[100][100])
{

    int i, j;
    float a_buf[100][100];
    for (i = 0; i < 100; i++)
    {
#pragma ACCEL pipeline
        for (j = 0; j < 100; j++)
        {
            a_buf[i][j] = b[i][j] * b[i][j];
        }
        for (j = 0; j < 99; j++)
        {
            a[i+1][j] += a_buf[i][j+1];
        }
    }
}

void func_top_4_0(float a[100][100], float b[100][100])
{

    int i, j;
    float a_buf[100][100];
    for (i = 0; i < 100; i++)
    {
#pragma ACCEL pipeline
        for (j = 0; j < 100; j++)
        {
            a_buf[i][j] = b[i][j] * b[i][j];
        }
        for (j = 0; j < 100; j++)
        {
            a[i][j] += a_buf[i][j];
        }
    }
}


#pragma ACCEL kernel
void func_top_11(float a[100][100], float b[100][100])
{

    int i, j;
    float a_buf[100][100];
    for (i = 0; i < 100; i++)
    {
#pragma ACCEL pipeline
        for (j = 0; j < 100; j++)
        {
#pragma ACCEL pipeline
            func_top_4(a_buf, a);
            func_top_4(b, a_buf);
        }
        for (j = 0; j < 100; j++)
        {
            a[i+1][j] = a_buf[i][j+1];
        }
    }
}

#pragma ACCEL kernel
void func_top_11_0(float a[100][100], float b[100][100])
{

    int i, j;
    float a_buf[100][100];
    for (i = 0; i < 100; i++)
    {
#pragma ACCEL pipeline
        for (j = 0; j < 100; j++)
        {
#pragma ACCEL pipeline
#pragma HLS array_partition variable=a_buf cyclic factor=2 dim=1
#pragma HLS array_partition variable=a_buf cyclic factor=2 dim=2
            func_top_4_0(a_buf, a);
            func_top_4_0(b, a_buf);
        }
        for (j = 0; j < 100; j++)
        {
            a[i+1][j] = a_buf[i][j+1];
        }
    }
}

void func_top_4_1(float a[100][100], float b[100][100]) {

  int i, j;
  float a_buf[100][100];
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 100; j++) {
      a_buf[i][j] = b[i][j] * b[i][j];
    }
    for (j = 0; j < 100; j++) {
      a[i][j] = a_buf[i][j];
    }
  }
}

#pragma ACCEL kernel
void func_top_11_1(float a[100][100], float b[100][100]) {

  int i, j;
  float a_buf[100][100];
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 100; j++) {
#pragma ACCEL pipeline
      func_top_4_1(a_buf, a);
      func_top_4_1(b, a_buf);
    }
  }
}

#pragma ACCEL kernel
void func_top_11_2(float a[100][100], float b[100][100])
{

    int i, j;
    float a_buf[100][100];
#pragma HLS array_partition variable=a_buf complete dim=1

    for (i = 0; i < 100; i++)
    {
#pragma ACCEL pipeline
    float b_buf[100];
#pragma HLS array_partition variable=b_buf cyclic factor=10 dim=1
#pragma HLS array_partition variable=a_buf cyclic factor=16 dim=2
        memcpy(&a_buf[i][0], a[i], sizeof(int) * 100);
        for (j = 0; j < 100; j++)
        {
            b_buf[j] = a_buf[i][j];
        }
        memcpy(b, b_buf, sizeof(int) * 100);
    }
}

void func_top_4_01(float a[100][100], float b[100][100], int val)
{

    for (int i = 0; i < 100; i++)
    for (int j = 0; j < 100; j++)
    {
#pragma ACCEL pipeline
        a[i][j] = b[i][j] * val;
    }
}


#pragma ACCEL kernel
void func_top_11_3(float a[100][100], float b[100][100])
{

    int i, j;
    float a_buf[100][100];
    for (i = 0; i < 100; i++)
    {
        for (j = 0; j < 100; j++)
        {
#pragma ACCEL pipeline
            func_top_4_01(a_buf, a, i);
            func_top_4_0(b, a_buf);
        }
    }
}
