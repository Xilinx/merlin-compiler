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

int g6(int & a)
{
    int * p = &a;
    return 0;
}

int g5(int & a)
{
    int * p = &a;
    g6(*p);
    return 0;
}

int g4(int & a)
{
    int * p = &a;
    g5(*p);
    return 0;
}

int g3(int & a)
{
    int * p = &a;
    g4(*p);
    return 0;
}

int g2(int & a)
{
    int * p = &a;
    g3(*p);
    return 0;
}

int g1(int & a)
{
    int * p = &a;
    g2(*p);
    return 0;
}

#pragma ACCEL kernel
int g0(int & a)
{
    int * p = &a;
    g1(*p);
    return 0;
}

#pragma ACCEL kernel
void fun(int & a,
        int & b,
        int & c,
        int & d,
        int & e,
        int & f,
        int & g)
{
    int aa = a;
    int bb = b;
    int cc = c;
    int dd = d;
    int ee = e;
    int ff = f;
    int gg = g;
}

#pragma ACCEL kernel
void hun(float &a, const double &b, int (&c)[10][10], const char * const &d)
{
    const float aa = a;
    const double bb = b;
    int (*cc)[10][10] = &c;
    const char *dd = d;
}

int main()
{
    int a;
    g0(a);

    int b;
    fun(b, b, b, b, b, b, b);

    float a3;
    double b3;
    int c3[10][10];
    char d3_;
    const char *d3 = &d3_;
    hun(a3, b3, c3, d3);
    return 0;
}

