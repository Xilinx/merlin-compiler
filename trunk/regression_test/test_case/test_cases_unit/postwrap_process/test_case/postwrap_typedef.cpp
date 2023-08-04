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

typedef int INT_IIIII;
typedef INT_IIIII INT_IIII;
typedef INT_IIII INT_III;
typedef INT_III INT_II;
typedef INT_II INT_I;
typedef INT_I int32;

typedef int32* int32_ptr;
typedef int32& int32_ref;
typedef const int32 const_int32;

typedef const int32* int32_ptr_const;
typedef const int32* const int32_const_ptr_const;
struct st_a{int a;};
typedef st_a st_b;
typedef struct 
{
    int32_ptr a;
    const int32_ptr b[1];
    int32_ref c;
}st;

int32 get_zero()
{
    return 0;
}

void g0();
INT_IIIII sum(const INT_IIIII ar[6],int n);
int func_1(INT_IIIII a);
int32 g6(int a);
int32 g5();
static INT_I g4();
extern INT_I g8();
void g7(int32_const_ptr_const a) {}

#pragma ACCEL kernel
void fun()
{
    const_int32 a0 = 99;

    int32 o = 88;
    int32_ptr p = &o;
    int32_ptr &p_ref = p;

    const_int32 b0 = 77;
    const int32_ptr_const  b1 = &b0;
    int32_const_ptr_const  b2 = &b0;
    g7(b2);
    double num = 0.92;

    union 
    {
        int32 a;
        int32_ptr b;
    } u;

    int32 a = 0;
    for (int32_ref b = a; b < 1024; ++b)
    {
        printf("%d\n", (int32)num);
        while (int32 c = get_zero())
        {
            printf("%p\n", (int32_ptr)(int32)num);
            if (int32 d = get_zero())
            {
                printf("%p\n", (int32_ptr)(int32)num);
            }
        }

        int32 e;
        do
        {
            printf("%p\n", (int32_ptr)(int32)num);
        }
        while (e = get_zero());
    }
    printf("%p\n", (int32_ptr)(int32)num);

    g0();
}

int32 g6(int a)
{
    int32_ptr p = NULL;
    return 0;
}

int g5()
{
    int32_ptr p = NULL;
    int a;
    g6(a);
    return 0;
}

static int32 g4()
{
    int32_ptr p = NULL;
    g5();
    return 0;
}

int32 g3()
{
    int32_ptr p = NULL;
    g4();
    return 0;
}

int32 g2()
{
    int32_ptr p = NULL;
    g3();
    return 0;
}

int32 g1()
{
    int32_ptr p = NULL;
    g2();
    return 0;
}

void g0()
{
    g1();
}

int main()
{
    fun();
    return 0;
}

INT_IIIII sum(const INT_IIIII ar[6])
{
  int total=0;
  int val = sizeof(INT_IIIII);
  for(int i=0;i<6;i++)
    total+=ar[i]+val;
  return total;
}

#pragma ACCEL kernel
int func_1(int a) {
  int tot;
  (INT_IIIII [2]){10,20};
  int arr[6] = {1,2,3,4,5,6};
  tot=sum(arr);
}

st func_2(st_b aa){
  aa.a=0;
}
