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

int dummy() 
{
    return 1;
}
#if 1
void test0(int *a)
{
    double b[1000];
    int M = dummy();
    int N = 100;

    for (int j = 1; j < 10; j++)
        b[M], a[N];
}
#endif

#if 1
void test1(int *a)
{
    double b[1000];
    //memcpy(b, a, 100*4);
    int N = 10;
    int M = 100;

    for (int j = 1; j < 10; j++)
        memcpy(b + M, a, 10*8);
    
        //*(b+M) = 0;
}
#endif

#if 1
void test(int *a)
{
    double b[1000];
    double c[1000];
    //memcpy(b, a, 100*4);
    int N = 10;
    int M = 100;

    size_t j;
    for (j = 1; j < N; j++)
    {
        memcpy(b + M, a, j*8);
    }
}
#endif

#if 1
void test3_sub(double * b, int *a1, size_t c)
{
        memcpy(b , a1, c*8);
}
void test3(int *a)
{
    double b[1000];
    double c[1000];
    //memcpy(b, a, 100*4);
    int N = 10;
    int M = 100;

    size_t j;
    for (j = 1; j < N; j++)
    {
        test3_sub(b, a, j);
    }
}
#endif

#if 1
void test4_sub(int *a1)
{
    a1[0] = 1;
    a1[1] = 1;
}
void test4(int *a)
{
    double b[1000];
    double c[1000];
    //memcpy(b, a, 100*4);
    int N = 10;
    int M = 100;

    int j;
    for (j = 1; j < N; j++)
    {
        int pe;
        for (pe = 1; pe < 4; pe++)
        {
            //int * b = &a[j+pe];
            //b[1] = 1;
            test4_sub(&(a[j+pe]));
        }
    }
}
#endif

