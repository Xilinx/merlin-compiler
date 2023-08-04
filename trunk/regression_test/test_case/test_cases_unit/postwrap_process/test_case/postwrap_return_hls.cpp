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
int handle(int b);
#pragma ACCEL kernel
int handle(int b)
{
    int a = 89;
    if (!a) {
        return ++a;
    }

    switch (b) {
        case 0: 
            return b--;
            break;
        case 1: {
            return a + b;
            break;}
        default:{
            return (a, b);
            break;}
    }

    for (int i = 0; i <= 10; ++i) {
        do {

            while (i > 5) {
                return (i * i * i);
            }
        } while (0);
    }
    return a * b;
}
#pragma ACCEL kernel
int fun()
{
    int a = 0;
    int b = 1;
    return (a > b) ? (a) : (b);
}

#pragma ACCEL kernel
static int fun_1()
{
    int a = 0;
    int b = 1;
    return (a > b) ? (a) : (b);
}


int g1(int & a)
{
    int * p = &a;
    return *p;
}

#pragma ACCEL kernel
int g0(int & a)
{
    int * p = &a;
    return g1(*p);
}
int main() {
    int a = 0;
    int b = g0(a);
    if (const int c = fun()) 
    {
        printf("hello\n");
    }
    int d = 0;
#pragma ACCEL kernel
    d = handle(b);
    if (a = fun_1()) 
    {
      g0(d);
    }
    return 0;
}

