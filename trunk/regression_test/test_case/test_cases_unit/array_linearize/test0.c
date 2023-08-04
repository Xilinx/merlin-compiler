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

#include <assert.h>

#if 0

#pragma ACCEL kernel
void func6(int b[4][8])
{
    b;
}


#else

#pragma ACCEL kernel
void func5(int *b)
{
    b;
}

void main5()
{
    int *a = (int*)malloc(14 * sizeof(int));
    func5(a);
}


#pragma ACCEL kernel
void func4(int *b)
{
    b;
}

void main4()
{
    int (* a)[1024];// = (int*)malloc(14 * sizeof(int));
    func4(a[0]);
}

#pragma ACCEL kernel
void func1(int *(* a)[1024])
{
#pragma ACCEL interface variable=a depth=10,,30
    // assert( a != 0);
    int **f0 =a[0] + 1;
    f0[0][0];
}


struct my_struct {
#pragma ACCEL attribute depth=50
    int * buf;
};

void func_sub(int ** e)
{
    float * g = e[0];
    g[0];
}

#pragma ACCEL kernel
void func(int ** a[1024], int **b, int cc[3][5], int n)
{
#pragma ACCEL interface variable=a depth=,n,128 max_depth=,100
#pragma ACCEL interface variable=b depth=10,10
    assert(n <=100);

	int i;
	for (i = 0; i < 100; i++)
	{
        int * f = b[i];
        int ** f0;
		cc[i][0] = a[i][2][0] + b[i][3] + f[3] + f0[0][0];
        cc[i][0] = f0[2][2];
        func_sub(a[1]);
        func_sub(a [2]);
	}
}

#if 0
int main()
{
    int ** a[1024];
    int ** b;
    int (* c)[5];
    int n;

    func(a, b, c, n);

    return 0;
}
#endif

#endif 


