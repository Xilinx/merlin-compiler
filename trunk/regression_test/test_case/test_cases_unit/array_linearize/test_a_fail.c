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

#pragma ACCEL kernel
void func2(int a[10][1024])
{
    a;
}

#pragma ACCEL kernel
void func3(int *a[1024])
{
#pragma ACCEL interface variable=a depth=,10
    a;
}

#pragma ACCEL kernel
void func0(int a[10][1024])
{
    // too large array can be detect by rose
    // int b[1LL << 31][1LL << 31][ 1LL << 31];
    int c[0];

    int d[10];
    /* test out of bound */
    for (int i = -1; i < 10; ++i) {
        d[i] = 0;
    }
}

#pragma ACCEL kernel
void func4(int ** a[1024], int **b, int c[3][5], int n)
{
#pragma ACCEL interface variable=a depth=,n,128 max_depth=,100
#pragma ACCEL interface variable=b depth=10,10
    assert(n <=100);

	int i;
	for (i = 0; i < 100; i++)
	{
		c[i][0] = a[i][2][0];
        ++c[i][0];
        c[i][0] = a[i][2][0];
        c[i][0]--;

        c[i][0] += a[i][2][0];
        c[i][0] -= a[i][2][0];
	}
}

#pragma ACCEL kernel
void func5(int ** a[1024 + 1024])
{
#pragma ACCEL interface variable=a depth=10,10,2048 max_depth=,100
}

#pragma ACCEL kernel
void func6(int ** a[1024])
{
#pragma ACCEL interface variable=a depth=10,10,1024 max_depth=,100
    sub6(*a);
}
