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
#include <assert.h>


#if 0
#else 

void test44(int * buf[1024])
{
  int i = 16;
  while (i--) buf[i];
}

void test43(int addr, int * a[1024])
{
    int * a_ = a[3];
    a[0][1];
    a_[2] = 0;
}


void test42(int addr, int * a)
{
    int b;
    assert(b < 100);
    for (int i = 0; i < b/3; i++)
    {
        a[i] = 0;
    }
}

void test41_1(int addr, int * a)
{
    for (int i = 0; i < 10; i++)
    {
        addr++;
        a[addr * addr + i] = 0;
    }
}
void test41(int addr, int * a)
{
    for (int i = 0; i < 10; i++)
    {
        a[addr * addr + i] = 0;
    }
}


void test40(int addr, int * a)
{

    int * buf;
    for (int i = 0; i < 10; i++)
    {
        if (addr)
            buf = a;

        int d = *(buf+addr);
    }
}

void test39(int addr, int * a)
{
    int * buf = a;
    buf++;
    a = buf;
       
    a[addr] = 0;
}

void test39_1(int addr, int * a)
{
    int * buf = a;
    a = buf;
       
    a[addr] = 0;
}
    int * g_buf_a;
void test39_1_1(int addr, int * a)
{
    g_buf_a = a;
    a = g_buf_a;
       
    a[addr] = 0;
}
void test39_2(int addr, int * a)
{
    int * buf;
    a = buf;
    buf = a;
    a[addr] = 0;
}
void test39_3(int addr, int * a, int * b)
{
    a = b;
    b = a;
    a[addr] = 0;
}


void test38(int addr, int * a)
{
    a = a+1;
    a[addr] = 0;// not support because not single assigned alias
}
void test38_1(int addr, int * a)
{
    int * b = a+1;
    b = a+1;  // not support because not single assigned alias
    b[addr] = 0;
}
void test38_2(int addr, int * a)
{
    a++;
    a[addr] = 0;// not support because not single assigned alias
}
void test38_3_sub(int * a) { a++; }
void test38_3(int addr, int * a)
{
    test38_3_sub(a);
    a[addr] = 0; // not support because not single assigned alias 
}

void test37(int addr, int * a)
{
    int addr1;
    addr1 = addr;
    addr = addr1;

    a[addr] = 0;
}
void test37_1(int addr, int * a)
{
    int addr1;
    addr1 = addr+1;
    addr = addr1;

    a[addr] = 0;
}
void test37_2(int addr, int * a)
{
    int addr1;
    if(a[0])addr1 = addr+1;
    addr = addr1;

    a[addr] = 0;
}
void test37_3(int addr, int * a)
{
    int addr1;
    addr1 = addr+1;
    addr = addr1;

    a[addr] = 0;
}
void test37_31(int addr, int * a)
{
    addr = 1;
    test37(addr, a);
}


void test36(int M, int * a)
{

    for (int j = 0;j < 10; j++)
        for (int i = 0;i < 10; i++)
        {
            if (i) break;
            a[i];
        }
}

int test35_sub();
void test35(int M, int * a)
{
    int N = test35_sub();

    assert(N >= 0);
    assert(N < 100);

    a[N];
}

void test34(int M, int * a)
{
    int N1 = M;

    assert(N1 >= 0);
    assert(N1 < 100);

    a[N1];
}

void test34_1(int N, int * a)
{

    assert(N >= 0);
    assert(N < 100);

    a[N];
}

void test33_sub(int *a, int N1)
{
    for (int i = 0;i < N1; i++)
    a[i];
}
void test33(int *a, int N)
{
    assert(N < 100);
    test33_sub(a, N);
}
void test33_1(int *a, int N)
{
    test33(a,N);
}

void test32(int *a, int N)
{
    assert(N < 100);
    for (int i = 0;i < N; i++)
    a[i];
}
void test32_1(int *a, int N)
{
    assert(N < 100);
    assert(N > 0);
    for (int i = 0;i < N; i++)
    a[i];
}
void test32_2(int *a, int N)
{
    test32(a,N);
}

void test31(int *a, int *b, int *c, int inc)
{
    int i, j;
    for (i = 0; i < 100; ++i)
    for (j = 0 ;j < 100; j++) {
      c[j*100+i]; 
    }
    return;
}


void test30_sub(int *a,int *b,int *c,int low,int high)
{
    for (int j = low; j < high; j++) 
    {
        c[j] += a[j] + b[j];
    }
}

void test30(int *a,int *b,int *c,int inc,int inc_comp)
{
    int var = inc * 4;
    for (int j = 0; j < 1024; j += 256) 
    {
        int l1 = j;
        int l2 = j + 128;
        int h1 = j + 127;
        int h2 = j + 255;
        test30_sub(a,b,c,l1,h1);
        if (var > 100) {
            test30_sub(a,b,c,l2,h2);
        }
    }
    for (int j = 0; j < 1024; j += 256) 
    {
        int l1 = j;
        int l2 = j + 128;
        int h1 = j + 127;
        int h2 = j + 255;
        test30_sub(a,b,c,l1,h1);
    }
}

// NOTE: when scope is test29_sub, the range is [0..99 exact]. This is because the loop bound of i is initialized to be 0..99 without considering scope
void test29_sub(int low, int high, int * a)
{
    for (int i = low; i < high; i++)
    {
        a[i] = 0;
    }
}

void test29(int * a)
{
    test29_sub(0, 100, a);
}



void test28(double *data)
{
    int k0=0;
    int idx = k0 % 7;

    if (idx == 0) 
        data[0] = 0;
    else if (idx == 1) 
        data[1] = 0;
    else if (idx == 2) 
        data[2] = 0;
    else if (idx == 3) 
        data[3] = 0;
    else if (idx == 4) 
        data[4] = 0;
    else 
        data[5] = 0;
}



void test27_sub(int i, int * a)
{
    a[i] = 1;
}

void test27()
{
    int a[100];
    int offset = 0;
    int j;
    test27_sub(offset, a); 
    if (j == 0) 
        test27_sub(offset+1, a); 
    test27_sub(offset+1, a); 
}

void test26_sub(int i, int * a)
{
    a[i] = 1;
}

void test26()
{
    int a[100];
    int offset = 0;
    int j;
    test26_sub(offset, a); 
    if (j == 0) test26_sub(offset, a); 
    else if (j == 1) test26_sub(offset, a); 
//    else if (j == 2) test26_sub(offset, a); 
//    else             test26_sub(offset, a); 
}


void test25()
{
    int arr[10][10];
    for (int i = 0; i < 10; i++)
    for (int j = 0; j < 9; j++)
    {
        arr[i][j] = 0;
        if (i!=0)
        {
            arr[i][j] -= 1;
        }
    }
}

void test23(int offset, int * a)
{
    for (int k = 0; k < 10; k++)
    {
        a[10*offset+k];
    }
}

void test24()
{
    int arr[1000];
    for (int i = 0; i < 10; i++)
    {
        test23(i, arr);
    }
    for (int j = 0; j < 10; j++)
    {
        test23(j, arr+100);
    }
}


void test22(int *a)
{
    int accr[3][1000];
    int i;
    for (i = 0; i < 100; i++)
    {
        if (i == 0)
        {
            accr[0][2*i]  = 0;
            accr[1][2*i]  = 0;
            accr[2][2*i]  = 0;
        }
        else
        {
            accr[0][2*i]  += 1;
            accr[1][2*i]  += 1;
            accr[2][2*i]  += 1;
        }
        if (i == 0)
        {
            accr[0][2*i + 1]  = 0;
            accr[1][2*i + 1]  = 0;
            accr[2][2*i + 1]  = 0;
        }
        else
        {
            accr[0][2*i + 1]  += 1;
            accr[1][2*i + 1]  += 1;
            accr[2][2*i + 1]  += 1;
        }
    }
}



void test21(int *a)
{
    int i;
    for (i = 0; i < 100; i++)
    {
        a[2*i] = 0;
        a[2*i-1] = 1;
    }
}


void test20(int *a)
{
    int i;
    for (i = 0; i < 100; i++)
    {
        if (i %2) {
            a[3*i];
            a[3*i+1];
            a[3*i+2];
        }
        else {
            a[3*i];
            a[3*i+1];
            a[3*i+2];
        }
    }
}

void test19(int *a)
{
    // ZP: 20160223 Does not support this case currently.
    int i;
    for (i = 0; i < 100; i++)
    {
        a[6*i];
        a[6*i+2];
        a[6*i+1];
    }
    for (i = 0; i < 100; i++)
    {
        a[6*i+3];
        a[6*i+4];
        a[6*i+5];
    }
}


void test18(int *a)
{
    int i;
    for (i = 0; i < 100; i++)
    {
        a[3*i];
        a[3*i+1];
        a[3*i+2];
    }
}


void test17(int *a)
{
    int i;
    for (i = 0; i < 100; i++)
    {
        // inexact range covered by exact range
        if (i == 0) a[i] = 0;
        a[i] += 1;
    }


    float result1[32][64];
    int  j, k;
    for (i = 0; i < 784; i++) {
        for (j = 0; j < 32; j++) 
        {
            for (k = 0; k < 64; k++) {
    
                if (i == 0) {
                    result1[j][k] = 0;
                }
                result1[j][k] += 1;
            }
        }
    }

}


void test16(int *a)
{
    int i;
    for (i = 0; i < 100; i++)
    {
        a[i/2];
    }
}


void test14(int *a)
{
    int i;
    if (i == 0) a[0];
    else if (i == 1) a[0];
    else if (i == 2) a[0];
    else a[0];
}

void test15(int *a)
{
    int i;
    switch (i)
    {
        case 0: a[0];
    }
}



void test12(int *a)
{
    int j = 0;
    int * b;
    while (j > 10) {
        a[j];
    }
    do  {
        b[j+1];
    } while (j > 10);
}

void test13(int *a)
{
    int i, j = 0;
    int * b, * c, * a1, * a2;
    for (j = 0; j < 100; j++) {
        if (j>50) continue;
        b[j];
    }
    for (j = 0; j < 100; j++) {
        for (i = 0; i < 100; i++) if (j>50) continue;
        c[j];
    }

LABEL:
    for (j = 0; j < 100; j++) {
        if (j>50) break;
        a1[j+1];
    }

    for (j = 0; j < 100; j++) {
        if (j>50) goto LABEL;
        a2[j+2];
    }
}

void test11(int *a)
{
    if (a[0])
    {
        if (a[1]) a[2];
        else a[3];
    }
    else
    {
        if (a[4]) a[5];
        else a[6];
    }
}

void test10(int *a)
{
    a[0];
    a[2];
    a[1];
}


void test(int *a)
{
    double b[1000];
    //memcpy(b, a, 100*4);

    for (int j = 1; j < 10; j++)
       if (j % 2) 
        memcpy(b + 100, a, j*8);
}

void func2(int * bb);
void func3(int * bb);
void func4();
void func5(int i, int * a);

void func1(int * a)
{
    int b[1000];
    int b1[1000];
    int b4[1000];
    int b5[1000];
    int b6[1000];
    int b8[1000];
    int b9[1000];
    int j;

    int * c = b;
    int * c1 = b1 + 1;
    if (a[0])
      c1[-1] = c1[0];
    c[-1] = c[0];

    for (j = 1; j < 10; j += 3)
        memcpy(b4 + 100, b5, j*4);
    for (j = 0; j < 10; j+= 2)
        memcpy(b8 + 4*j, b9, 4*4);


    for (int i = 0; i < 100; i++)
    {
        int *d = b6;
        int a = d[i];
        if (i % 3)
           a  = d[i+1];
        func2(b6);
    }
}


void func2(int * bb)
{
    bb[1] = 1;
    int * cc;
    cc = bb;
    //int * cc = bb;

    for (int i = 10; i < bb[3]; i++)
    {
        cc[2] = 0;
        func3(cc);
    }
}

void func3(int * bb)
{
    int * cc;
    cc = bb;

    cc[101];
}


void func5(int i, int * a)
{
  if (i % 4) 
    for (int j = 0; j < 10; j++)
    {
        a[10*i+j];
    }
}

void func6(int offset, int * a)
{
    int b[100];
    memcpy(a+offset, b, 10*4);
}

void func4()
{
    int arr[1000];
    int arr1[1000];
    for (int i = 0; i < 100; i++)
    {
        func5(i, arr);
        func6(i, arr1);
    }
}

void func5_1(int offset, int * a)
{
    for (int j = 0; j < 10; j++)
    {
        a[10*offset+j];
    }
}

void func6_1(int offset, int * a)
{
    int b[100];
    memcpy(a+10*offset, b, 10*4);
}

void func4_1()
{
    int arr[1000];
    int arr1[1000];
    for (int i = 0; i < 100; i++)
    {
        func5_1(i, arr);
        func6_1(i, arr1);
    }
}

#endif


