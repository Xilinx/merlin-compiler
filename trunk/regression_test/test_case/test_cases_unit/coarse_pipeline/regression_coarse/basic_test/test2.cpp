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
//
//Break stmt is not supported
#pragma ACCEL kernel
void func_top_0(int * a, int * b, int c)
{
  int i;
  int j;
  int a_buf[100][100];
  for (j = 0; j < 100; ++j) {
#pragma ACCEL pipeline
    for (int i = 0; i < 100; i++)
    {
      a_buf[j][i] = b[i];
    }
    {
      c++;
      for (int i = 99; i >= 0; --i)
        a[i] = a_buf[j][i]+c;

    }    
  }
}
//Direct continue is not supported
#pragma ACCEL kernel
void func_top_0_1(int * a, int * b, int c)
{
    int i;
    int j;
    int a_buf[100];
    for (j = 0; j < 100; ++j) {
#pragma ACCEL pipeline
        //FIXME:
        //int a_buf[100];
        for (i = 0; i < 100; i++)
        {
            a_buf[i] = b[i];
        }

        if (j == c)
            continue;
    }
}

#pragma ACCEL kernel
void func_top_0_2(int * a, int * b, int c11)
{
    int i;
    int j;
    int a_buf[100];
    for (j = 0; j < 100; ++j) {
#pragma ACCEL pipeline
        for (i = 1; i < 100; i++) {
            a[i] = b[i];
        }
        a[0] = a_buf[0];
        for (i = 98; i >= 0; --i) {
          //  a[i] = a_buf[i];
            a[i] = c11;
        }
    }
}

#pragma ACCEL kernel
void func_top_0_2_1(int * a, int * b, int c)
{
    int i;
    int j;
    int a_buf[101];
    for (j = 0; j < 100; ++j) {
#pragma ACCEL pipeline
        //FIXME:
        //int a_buf[100];
        for (i = 0; i < 100; i++)
            a_buf[i] = b[i];
        
				for (i = 100; i >= 0; --i)
            a[i] = a_buf[i];
    }
}

#pragma ACCEL kernel
void func_top_0_2_2(int * a, int * b, int c)
{
    int i;
    int j;
    int a_buf[100];
    for (j = 0; j < 100; ++j) {
#pragma ACCEL pipeline
        //FIXME:
        //int a_buf[100];
        for (i = 0; i < 100; i++)
            a_buf[i] = b[i];
        
				for (i = 100-1; i >= 0; --i)
            a[i] = a_buf[i];
    }
}

#pragma ACCEL kernel
void func_top_1(int a[100][100], int b[100],  float c[50])
{
    int i, j;
    int a_buf[100][100];
    float c_buf[50];
    for (i = 0; i < 100; i++) {
#pragma ACCEL pipeline
        for (j = i; j < 100; j++)   //bracelet
        {
            a_buf[0][j] = b[j];
        }
        for (j= i ; j >= 0; j--) {
            if (j == 2)
                break;
            c_buf[j] = c[i];
        }
        for (j = i + 1; j < 100; ++j) {
            if (j == 10) continue;
            a[i][j] = c_buf[j / 2] + a_buf[i][j];
        }
    }
}

#pragma ACCEL kernel
void func_top_1_0(int a[100][100], int b[100],  float c[50])
{
    int i, j;
    int a_buf[100][100];
    float c_buf[50];
    for (i = 0; i < 100; i++) {
#pragma ACCEL pipeline
        for (j = i; j < 100; j++)   //bracelet
        {
            a_buf[0][j] = b[j];
        }
        for (j= i ; j >= 0; j--) {
            if (j == 2)
                break;
            c_buf[j] = c[i];
        }
        for (j = i + 1; j < 100; ++j) {
            if (j == 10) continue;
            a[i][j] = c_buf[j / 2] + a_buf[0][j];
        }
    }
}

#pragma ACCEL kernel
void func_top_2(int a[100], int b[100], int c)
{

    int i, j;
    int a_buf[100];
    for (i = 0; i < 100; i++) {
#pragma ACCEL pipeline
        for (j = 0; j < 100; j++)
        {
            a_buf[j] = b[j];
        }
        if (i == c)
            continue;
        j = 100;
        while(j < 100) {
            a[j] = a_buf[j];
            --j;
        }
    }
}

//Loop structure 
#pragma ACCEL kernel
void func_top_3(int a[100][100], int b[100][100], 
        int c[50], int c1, int c2)
{

    int i, j;
    int a_buf[100][100];
    int c_buf[50];
    for (j = 0; j < 100; j++) {
#pragma ACCEL pipeline
        if (c1) 
            for (i = 0; i < 100; i++) {
                a_buf[i][j] = b[i][j];
                c_buf[i/ 2] = b[i][j];
            }
        else 
            a_buf[i][50] = b[i][50];
        if (c2) {
            for (i = 0; i < 100; i++)
                a[i][j] = a_buf[i][j];
        }
        for (i = 0; i < 50; i++) {
            c[i] = c_buf[i];
        }
    }
}

#pragma ACCEL kernel
void func_top_3_0(int a[100][100],int b[100][100],int c[50],int c1,int c2)
{
  int i;
  int j;
  int a_buf[100][100];
  int c_buf[50];
  for (j = 0; j < 100; j++) 
{
#pragma ACCEL pipeline
      for (i = 0; i < 100; i++) {
        a_buf[i][j] = b[i][j];
        c_buf[i / 2] = b[i][j];
      }
    if (c2) {
      for (i = 0; i < 100; i++) {
        a[i][j] = a_buf[i][j];
      }
    }
    for (i = 0; i < 50; i++) {
      c[i] = c_buf[i];
    }
  }
}

#pragma ACCEL kernel
void func_top_4(float a[100], float b[100], float c[100])
{

    int i, j;
    float a_buf[100];
    float b_buf[100];
    float c_buf[100];
    for (i = 0; i < 100; i++)
    {
#pragma ACCEL pipeline
        for (j = 0; j < 100; j++)
        {
            a_buf[j] = b[j] * b[j];
        }
        j = 0;
        while (j < 100) {
            c_buf[j] = c[j] * 2;
						j++;
        }
        for (j = 0; j < 100; j+=2) {
            b_buf[j] = a_buf[j] + c_buf[j];
        }
        for (j = 0; j < 100; j++)
        {
            a[j] = b_buf[j] + j;
        }
    }
}

#pragma ACCEL kernel
void func_top_5(int a[100],int b[100], int c[100])
{

    int i=0, j=0;

    int a_buf[100];
    int c_buf[100];
    while (i < 100)
    {
#pragma ACCEL pipeline
        for (j = 0; j < 100; j++)
        {
            a_buf[j] = b[j];
            c_buf[j] = b[j];
        }
        for (j = 0; j < 100; j++)
        {
            a[j] = a_buf[j];
        }
        for (j = 0; j < 100; j++)
        {
            c[j] = c_buf[j+1] + a_buf[j];
        }
        ++i;
    }
    i = 0;
    do {
#pragma ACCEL pipeline
        for (j = 0; j < 100; j++)
        {
            a_buf[j] = b[j+1];
        }

        for (j = 0; j < 100; j++)
        {
            a[j] = a_buf[j+1];
        }
        ++i;
    } while (i < 100);

}

//ifelse



#pragma ACCEL kernel
void func_top_6(int a[100][100], int b[100][100])
{

    int i, j;
    int a_buf[100][100];
    for (i = 0; i < 100; i++) {
#pragma ACCEL pipeline
        if (b[0][0]) {
            j = 10;
            do {
                a_buf[i][j] = b[i][j];
                ++j;
            } while (j < 100);
        }
        if (a_buf[1][1]) {
            j = 100;
            while (j >= 0) {
                a[i][j] = a_buf[i][j];
                --j;
            }
        }
    }
}

// sub function
void sub_0_0(int * a, int c) {
    int i;
    for (i = 0; i < 100; i+=2) 
        a[i] = i + c;
}

void sub_0(int a[100], int b[100])
{

    int i, j;
    for (i = 0; i < 100; i++) {
        if (b[i]) {
            sub_0_0(a, b[i]);
        }
    }
}




#pragma ACCEL kernel
void func_top_7(int a[100],
        int b[100],
        int c[100]) {

    int i;
    int a_buf[100];
    int c_buf[100];
    for (i = 0; i < 2; ++i) {
#pragma ACCEL pipeline
        sub_0(a_buf, b);
        sub_0(a, a_buf);
        sub_0(c_buf, a_buf);
        sub_0(c, c_buf);
    }
}

void sub_1(float a[100], float b[100])
{

    int i, j;
    for (i = 0; i < 100; i++) {
        if (b[i] > 1.0) {
            a[i] = b[i];
        } else
            a[i] = 1.0;
    }
}

float sub_2(float a[100], float b[100], int c)
{
    int i, j;
    for (i = 0; i < 100; i++) {
        a[i] = b[i] + c;
    }
    return c;
}

#pragma ACCEL kernel
void func_top_8(float a[100],
        float b[100],
        float c[100]) {



    float sum = 0;
    int i = 0, j;
    float a_buf[100];
    for (i = 0; i < 100; ++i) {
#pragma ACCEL pipeline
        sub_1(a_buf, b);
        sum = sub_2(c, a_buf, i);
        for (j = 0; j < 50; ++j)
            a[j] = sum + a_buf[j];
    }
}

void sub_2_1(float a[100], float b[100], float &c)
{
    int i, j;
    for (i = 0; i < 100; i++) {
        a[i] = b[i] + c;
    }
		c=a[0];
}

#pragma ACCEL kernel
void func_top_8_1(float a[100],
        float b[100],
        float c[100]) 
{

    float sum = 0;
    int i = 0, j;
    float a_buf[100];
    for (i = 0; i < 100; ++i) {
#pragma ACCEL pipeline
        sub_1(a_buf, b);
        sub_2_1(c, a_buf, sum);
        for (j = 0; j < 50; ++j)
            a[j] = sum + a_buf[j];
    }
}

#pragma ACCEL kernel
void func_top_8_2(float a[100],
        float b[100],
        float c[100],
        float *result) 
{

    float sum = 0;
    int i = 0, j;
    float a_buf[100];
    for (i = 0; i < 100; ++i) {
#pragma ACCEL pipeline
        sub_1(a_buf, b);
        sub_2_1(c, a_buf, sum);
        for (j = 0; j < 50; ++j)
            a[j] = sum + a_buf[j];
    }
    *result = sum;
}
