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
#include<assert.h>
void func_2(float * a);
void func_0(float * a);

void func_1(float * a)
{
    int i , j;
    for (i = 0; i < 100; i++)
    for (j = 0; j < 100; j++)
    {
        a[i];
        a[i-1];
        a[i+1];
    }
}

void func_3(float * f, int n)
{
	assert(n < 10);
    int i , j;
    for (i = 0; i < 100; i++)
    for (j = 0; j < 100; j++)
    {
        f[i];
        f[i-n];
        f[i+n];
    }
}



void main()
{
    int n;
    int a[1024];
    func_2(a);
	func_3(a, n);
}

void func_2(float * a)
{
    int k;
    int i, j;
    for (k = 0; k < 100; k++)
        func_0(a);

    int b[10][10];
    int c[10][10];
    int d[10];
    int e[10];
    for (i = 0; i < 100/10; i ++)
    {
        b[i][0];
        for (j = 0; j < 10; j ++)
        {
            b[i-1][j*100];
            a[i*10+j];
            a[i*10+10+j];
            a[i*10+10+j-1];
            c[i][j];
            c[i][j-1];
            c[i-1][j];
            c[i-1][j-1];

            d[i-1];
            d[i-1];

            e[d[i]];
            e[d[i-1]];
        }
    }
}


void func_0(float * a)
{
    int ii;
    for (ii = 0; ii < 100; ii++)
    {
        a[256]; // not been supported, ZP 20150812
        func_1(a);
//        func_1(a+1);
    }
}

