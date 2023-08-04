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
#define M 100

struct st {
#pragma ACCEL attribute variable=a depth=M
  int *a;
  int len;
};

#pragma ACCEL kernel
void func_top_0(int *a, int *b) 
{
// marco 'M' cannot support
#pragma ACCEL interface variable=a depth=M
    for (int i = 0; i < 100; i++)
    {
        a[i] = b[i];
    }
}


#pragma ACCEL kernel
void func_top_1(struct st *a, int a_len)
{
//'a_le' is a typo
#pragma ACCEL interface variable=a depth=a_le
    for (int i = 0; i < 100; i++)
    for (int j = 0; j < 100; j++)   //bracelet
    {
        a[i].a[j] = 0;
    }
}

#pragma ACCEL kernel
void func_top_2(struct st *a)
{
#pragma ACCEL interface variable=a depth=a->len
    for (int i = 0; i < 100; i++)
    for (int j = 0; j < 100; j++)   //bracelet
    {
        a[i].a[j] = 0;
    }
}

#pragma ACCEL kernel
void func_top_3(int *a, int *b) 
{
//multiple interface pragmas 
#pragma ACCEL interface variable=a depth=100
#pragma ACCEL interface variable=a max_depth=100
#pragma ACCEL interface variable=b depth=100
    for (int i = 0; i < 100; i++)
    {
        a[i] = b[i];
    }
}


