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
//counter example

#pragma ACCEL kernel
void test_1(char *a)
{
#pragma ACCEL interface variable=a depth=1
    a = "hello";
}

#pragma ACCEL kernel
void test_2(char **a)
{
#pragma ACCEL interface variable=a depth=1,2
    a = (char **)"hello";
}

#pragma ACCEL kernel
void test_3(char *a)
{
#pragma ACCEL interface variable=a depth=1
    char b = 'b';
    a = &b;
}

void test_4_sub(char **b)
{
    b = (char **)"hello";
}

#pragma ACCEL kernel
void test_4(char **a)
{
#pragma ACCEL interface variable=a depth=1,2
    test_4_sub(a);
}

void test_5_sub_sub(char **b)
{
    b = (char **)"hello";
}

void test_5_sub(char **b)
{
    test_5_sub_sub(b);
}

#pragma ACCEL kernel
void test_5(char **a)
{
#pragma ACCEL interface variable=a depth=1,2
    test_5_sub(a);
}

void test_6_sub(char **b)
{
    b = (char **)"hello";
}

#pragma ACCEL kernel
void test_6(char ***a)
{
#pragma ACCEL interface variable=a depth=1,2,3
    test_6_sub(*a);
}

#pragma ACCEL kernel
void test_7(char ****a)
{
#pragma ACCEL interface variable=a depth=1,2,3,4
    (**a) = (char **)"hello";
}

#pragma ACCEL kernel
void test_8(char ****a)
{
#pragma ACCEL interface variable=a depth=1,2,3,4
    *&(*&(**a)) = (char **)"hello";
}

#pragma ACCEL kernel
void test_9(char ****a)
{
#pragma ACCEL interface variable=a depth=1,2,3,4
    *&(a[0][1]) = (char **)"hello";
}

#pragma ACCEL kernel
void test_10(char ****a)
{
#pragma ACCEL interface variable=a depth=1,2,3,4
    *&(*(*(a + 1) - 2)) = (char **)"hello";
}

#pragma ACCEL kernel
void test_11(char ****a)
{
#pragma ACCEL interface variable=a depth=1,2,3,4
    ((*(a + 1)[2])) = (char **)"hello";
}

void test_12_sub(char **b)
{
    *b = (char *)"hello";
}

#pragma ACCEL kernel
void test_12(char **a)
{
#pragma ACCEL interface variable=a depth=1,2
    test_12_sub(a);
}

void test_13_sub_sub(char **b)
{
    *b = (char *)"hello";
}

void test_13_sub(char **b)
{
    test_13_sub_sub(b);
}

#pragma ACCEL kernel
void test_13(char **a)
{
#pragma ACCEL interface variable=a depth=1,2
    test_13_sub(a);
}

void test_14_sub(char **b)
{
    *b = (char *)"hello";
}

#pragma ACCEL kernel
void test_14(char ***a)
{
#pragma ACCEL interface variable=a depth=1,2,3
    test_14_sub(*a);
}
