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
#include <math.h>
#include <stdlib.h>


void func1( int * a0)
{
    a0[0] = 1;
}
void func2( int * a0)
{
    int b = a0[0];
}
void func3( int * a0)
{
    a0[0] ++;
}
void func4( int & a0)
{
    a0 ++;
}

int main(int * argv, int n)
{
    int a[1024];
    int b;
    float c;

    int i,j,k,l,m;
    m = argv[1];

    {
    }

    b = a[10];
    c = a[10];
    a[10] = b;
    a[10] = c;


    for (i = 0;i  < 100; i++)
    {
        a[i] = i;
        c = a[i] + 3;
    }

    a[0]++;
    b+=1;
    --b;

    a[0] += 1; 
    b += a[0];
    a[2] = a[1];

    a;

    int *d = &(a[0]);

    func1(a);
    func2(a);
    func3(a);
    func4(a[0]);
    memset(a, 0, 1024);
    abs(b);
    log(a[0]);

    return 1;
}


