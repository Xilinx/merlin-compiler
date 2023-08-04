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
void sub_func1(const char **p)
{
    *p += 10;
}

#pragma ACCEL kernel
void func1(const char **b)
{
#pragma ACCEL interface variable=b depth=1,2
    sub_func1(b);
}

void sub_func2(char **p, int count)
{
    *p += count;
}

#pragma ACCEL kernel
void func2(char **b)
{
#pragma ACCEL interface variable=b depth=1,2
    sub_func2(b, 20);
}

void sub_sub_func3(char **p, int count)
{
    *p += count;
}

void sub_func3(char **p, int count)
{
    sub_sub_func3(p, count);
}

#pragma ACCEL kernel
void func3(char **b)
{
#pragma ACCEL interface variable=b depth=1,2
    sub_func3(b, 20);
}

void sub_func4(char **b)
{
    ++*b;
}

#pragma ACCEL kernel
void func4(char **b) // not pass, no fail
{
#pragma ACCEL interface variable=b depth=1,2
    sub_func4(b);
}

void sub_sub_func5(char **b)
{
    (*b)--;
}

void sub_func5(char **b)
{
    sub_sub_func5(b);
}

#pragma ACCEL kernel
void func5(char **b) // not pass, no fail
{
#pragma ACCEL interface variable=b depth=1,2
    sub_func5(b);
}

#pragma ACCEL kernel
void func6(char *b)
{
#pragma ACCEL interface variable=b depth=1
    b += 10;
}

void sub_func7(char **c)
{
    (*c)++;
}

#pragma ACCEL kernel
void func7(char *b) // false negative
{
#pragma ACCEL interface variable=b depth=1
    char **c = &b;
    sub_func7(c);
}

#pragma ACCEL kernel
void func8(char ***b)
{
#pragma ACCEL interface variable=b depth=1,2,3
    *b += 1;
}

char **g9 = (char **)0;

void sub_func9(char ***b)
{
    *b = g9;
}

#pragma ACCEL kernel
void func9(char ***b)
{
#pragma ACCEL interface variable=b depth=1,2,3
    sub_func9(b);
}

void sub_func10(const char **p)
{
    p += 10;
}

#pragma ACCEL kernel
void func10(const char **b)
{
#pragma ACCEL interface variable=b depth=1,2
    sub_func10(b);
}

void sub_sub_func11(char **p, int count)
{
    p += count;
}

void sub_func11(char **p, int count)
{
    sub_sub_func11(p, count);
}

#pragma ACCEL kernel
void func12(char ***b)
{
#pragma ACCEL interface variable=b depth=1,2,3
    b += 1;
}

void sub_func13(char **p)
{
    **p += 10;
}

#pragma ACCEL kernel
void func13(char *b)
{
#pragma ACCEL interface variable=b depth=1
    char **c = &b;
    sub_func13(c);
}

void sub_func14(char **b)
{
    ++b;
}

#pragma ACCEL kernel
void func14(char **b)
{
#pragma ACCEL interface variable=b depth=1,2
    sub_func4(b);
}

