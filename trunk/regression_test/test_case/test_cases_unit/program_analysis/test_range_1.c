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


extern int e_a[10], bb;
int g_a = 0;

int e_a[10] = {0};
int e_b[10][10] = {0};

void func(int * a, int *b, int *c)
{
    int l_a= 10;
    *c = *a + *b + e_a[0] + c[*a];
    e_b[a[1]][2] = e_a[1] + *(*(e_b + 1) + 2);
    e_b[a[1]][2] = e_a[1] + *( (*(e_b - 1)) );
}



void func_loop()
{
    int a[10];
    int b[10];
    int c[10];

    int i, j, k;
    int x, y, z;
    int N = 100;
    for (i = 100; i > 0; i--)
    {
        c[i] = a[i] + b[i] ;    
    }

    {
        for (int i = 0, jj, j1=0, j2=0, j3=0, x0=0,y1,a1; i < N; i-=b[i])
        {
            c[i] = a[i] + b[i] ;    
        }
    }
    {
        for (int i = 0, jj, j1=0, j2=0, j3=0, x0=0,y1,a1; i < N; i-=b[i])
            c[i] = a[i] + b[i] ;    
    }

    {
        for (int i = 0, jjj; i>=0; i+=-1)
        {
            c[i] = a[i] + b[i] ;    
        }
    }


    for (i = 0, j=1; i < N, i>0; i+=2)
    {
        c[i] = a[i] + b[i] ;    
    }
    for (i = 0, j=1; i > N && i>0; i++)
    {
        c[i] = a[i] + b[i] ;    
    }


    for (i = 0, j*3; i <= N; i--)
    {
        c[i] = a[i] + b[i] ;    
    }

    for (i = j = 0; i <= N; i++)
    {
        c[i] = a[i] + b[i] ;    
    }

    for (i = j = k; i <= N; i++)
    {
        c[i] = a[i] + b[i] ;    
    }

    for (i = 0, j=3; i <= N && j>=0; i--, j--)
    {
        c[i] = a[i] + b[i] ;    
    }

    for (i = 0, j=3; i <= N && j>=0; )
    {
    }

    for (i = 0, j=3; i <= N ; i++)
    {
    }
    for (a[0], i = 0, j=3, x=1, y; i <= N ; i++)
    {
    }

    for (i = 0, j=3; i >= N ; i-=2, j--)
    {
        c[i] = a[i] + b[i] ;    
    }


    for (i = a[0]; i < a[1]; i++)
    {
        i+1;
    }


    j=0;
    for (i =0; i <100; i++)
    {
        j = j+1;
        j;
    }
}



