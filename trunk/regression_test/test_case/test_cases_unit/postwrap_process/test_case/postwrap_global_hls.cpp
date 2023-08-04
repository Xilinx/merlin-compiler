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
#include <stdio.h>
#include <stdlib.h>

struct As
{
    int a;
};

struct As g_9;

void g9()
{
    g_9.a = 44;
    return ;
}


int g_8 = 99;
void g8()
{
    g_8 = 22;
    // int g_8 = 32;
    g9();
    return;
}

extern int g_7;
int g_7 = 99;

void g7()
{
    g_7 = 42;
    g8();
    return;
}

int g_6;
int g6()
{
    g_6 = 0;
    g7();
    return 0;
}

int * g_5;
int g5()
{
    // g_5 = NULL;
    g6();
    return 0;
}

#define SIZE 32
int g_4[SIZE];
int g4()
{
    for (int i = 0; i < SIZE; ++i)
    {
        g_4[i] = 0;
    }
    g5();
    return 0;
}

int * g_3[SIZE];
int g3()
{
    for (int i = 0; i < SIZE; ++i)
    {
        g_3[i] = NULL;
    }
    g4();
    return 0;
}

const int *g_2;
int g2()
{
    g_2 = NULL;
    g3();
    return 0;
}

const int **g_1;
int g1()
{
    g_1 = NULL;
    g2();
    return 0;
}

int __merlinwrapper_g0()
{
}

// int &const g_0 = g_6;
int g_0 = g_6;
#pragma ACCEL kernel
int g0()
{
    g_0 = 0;
    g1();
    return 0;
}

int main()
{
    g0();
    int a = g_0;
    void *b = g_1;

    int *c;
    for (int i = 0; i < SIZE; ++i)
    {
        c = g_3[i];
    }
    int d;
    for (int i = 0; i < SIZE; ++i)
    {
        d = g_4[i];
    }
    void *e = g_5;
    int f = g_6;
    return 0;
}
