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
#include "include.h"

void assert(bool v);

#pragma ACCEL kernel
void top(int *a, int len) {
    assert(len < 100 && len > 50);
    for (int i = 0; i < len; ++i) {
        a[i] = i * i;
    }
    sub(a, len);
}


/* test kernel overload */
void top(int *a, int len, int b) {

}

/* test unknow kernel param dim size */
#pragma ACCEL kernel
void top2(int a[], int len) {

    for (int i = 0; i < len; ++i) {
        a[i] = i * i;
    }
}

struct SS
{
    int i;
};

/* test kernel param with unsupport type */
#pragma ACCEL kernel
void top3(int a[], int len, int &b, int (*c)(int, int), struct SS &d) {

    for (int i = 0; i < len; ++i) {
        a[i] = i * i;
    }
}

#pragma ACCEL kernel
void top4(int a[100]) {

#pragma ACCEL pipeline flatten
#pragma ACCEL parallel flatten
    for (int i = 0; i < 100; ++i) {
        for (int j = 0; j < i; ++j) {
            a[i] = i + j;
        }
    }
}

void top5(int i)
{
}

#pragma ACCEL kernel
void top5(int a[100]) 
{
    int i = 100;
    top5(i);
}

