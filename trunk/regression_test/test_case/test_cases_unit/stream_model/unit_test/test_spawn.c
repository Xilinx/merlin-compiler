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
#include<string.h>


#pragma ACCEL kernel
void kernel_top(int * a, int * b)
{

  for (int j = 0; j < 100; j++)
  {
#pragma ACCEL spawn
    for (int i = 0; i < 100; i++)
    {
      a[j* 100 + i] = b[j* 100 + i];
    }
  }

}


#pragma ACCEL kernel
void kernel_top1(int * a, int * b)
{

  for (int j = 0; j < 100; j++)
  {
#pragma ACCEL spawn
    for (int i = 0; i < 100; i++)
    {
      a[i] = b[i]+1;
    }
  }

}

#pragma ACCEL kernel
void kernel_top2(int * a, int * b)
{

  int a_buf[10000];
  int b_buf[10000];
  memcpy(b_buf, b, sizeof(int) * 10000);
  for (int j = 0; j < 100; j++)
  {
#pragma ACCEL spawn
    for (int i = 0; i < 100; i++)
    {
      a_buf[j* 100 + i] = b_buf[j* 100 + i];
    }
  }
  memcpy(a, a_buf, sizeof(int) * 10000);

}

void func3(int a_buf[10000], int j)  {
    for (int i = 0; i < 100; i++)
    {
      a_buf[j* 100 + i] = i;
    }
}

#pragma ACCEL kernel
void kernel_top3(int * a, int * b)
{

  int a_buf[10000];
  for (int j = 0; j < 100; j++)
  {
#pragma ACCEL spawn
    func3(a_buf, j);
  }
  memcpy(a, a_buf, sizeof(int) * 10000);
}

void func4(int a[100])  {
    for (int i = 0; i < 100; i++)
    {
      a[i] = i;
    }
}

#pragma ACCEL kernel
void kernel_top4(int * a, int * b)
{

  int a_buf[100][100];
  for (int j = 0; j < 100; j++)
  {
#pragma ACCEL spawn
    func4(a_buf[j]);
  }
  memcpy(a, a_buf, sizeof(int) * 10000);
}
