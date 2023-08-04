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
// if not included, the type analysis will fails
#include "stdlib.h"
#include "assert.h"

void sub1(int b[11])
{
    b;
}
void sub2(int * c)
{
    c+1;
}
void sub3(int * c)
{
    &(c[1]);
}
void sub4(int * c)
{
    c[0];
}
void sub5(int * c_multi_calls)
{
    c_multi_calls;
}
void sub6(int * a_malloc)
{
    a_malloc;
}
void sub7(double * a_calloc)
{
    a_calloc;
}
void sub8(int * a_new)
{
    a_new;
}
void sub91(int * a_two_level, int * b_two_level)
{
    a_two_level;
    b_two_level;
}
void sub9(int * a, int b[30][31])
{
    sub91(a, b[0]);
}
void func()
{
    int a[10][20];
    int * b;

    a;
    a[0];
    a[0][1];

    sub1(b);

    int c[12];
    sub2(c);

    sub5(c);
    sub5(c);

    int * d;
    d = (int*)malloc(1024);
    sub6(d);

    double* e = (double*)calloc(8,  1024);
    sub7(e);

    int *f;
    f = new int [2323];
    sub8(f);

    int g[30][31];
    sub9(f, g);

    int n = 101;
    int * new_1 = new int [n];
    new_1;

    int n1;
    n1 = a[0][1];
    assert(n1 >= 0 && n1 <= 10);
    int * new_2 = new int [n1]; 
    new_2;
    int * new_3 = (int*) malloc(sizeof(int)*n1);
    new_3;
    int * new_4;
    cmost_malloc_1d((void**)&new_4, "input", sizeof(int), n1);
    new_4;
    int * new_5;
    cmost_malloc((void **)&new_5, sizeof(int) * n1);
    new_5;
    int * new_6;
    posix_memalign((void **) &new_6, 16, sizeof(int) * n1);
    new_6;
    int * new_7 = (int *)calloc(sizeof(int), n1);
    new_7;
    int * new_4_c;
    cmost_malloc_1d((void**)&new_4_c, "input", sizeof(int), 100);
    new_4_c;
    int * new_5_c;
    cmost_malloc((void **)&new_5_c, sizeof(int) * 100);
    new_5_c;
    int * new_6_c;
    posix_memalign((void **) &new_6_c, 16, sizeof(int) * 100);
    new_6_c;
}

