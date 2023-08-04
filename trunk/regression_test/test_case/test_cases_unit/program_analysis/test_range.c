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



void func(int M)
{
    int N = 100;
    int a[100];
    int i, j;
    int b[100];
    int *p = b;
    int *p2 = &b[99];
    for (i = 0, j=23; i <= N; i++)
    for (j = -50; j < 1; j++)
    {
        a[i] = i;
        i;
        i+j;
        i*j;
        2*i - 2*j;
        -i+j;
        *p++;
        *(p2--) = 0;
    }
    0;
    0;
    for (i = 100; i <= 200; i++)
    for (j = 100; j <= 200; j++)
    {
        a[i] = i;
        i;
        i+j;
        i*j;
        2*i - 2*j;
        -i+j;
        j/3;
    }

    for (i = a[0]; i < a[1]; i++)
    {
        i+1;
    }
}

void f() {                                                                      
  int i, x = 0;                                                                 
  int a[1000];                                                                  
  for (i = 0; i < 100; ++i) {                                                   
A: x += a[i];                                                                   
  }                                                                             
  i = 999;
  goto A;                                                                       
}                                                                               

void f2() {
  int a[100];
  for (int i = 0; i < 50; ++i) {
	a[i] = a[i + 50];
  }
}

void f3() {
  int a[100];
  for (int i = 0; i < 100; ++i) {
    for (int j = 0; j < 100; ++j) {
      if (i + j < 100) {
        a[i + j] = i * j;
      }
    }
  }
}
