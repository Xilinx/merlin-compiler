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
#define M 100
int val = 2;
#pragma ACCEL kernel
void func_top_0(int *b) 
{
#pragma ACCEL interface variable=b depth=M
#pragma ACCEL interface variable=hist_cache stream_prefetch=on

    int a[100][100];
 #pragma HLS array_partition variable=a dim=val factor=val1

    int i, j, k;
    for (i = 0; i < 100; i++)
    {
#pragma ACCEL parallel factor=complete
        for (j = 0; j < 100; j++)
        {
            a[i][j] = b[j];
        }
    }
}

struct st {
#pragma ACCEL attribute variable=a depth=M
  int *a;
  int len;
};


#pragma ACCEL kernel
void func_top_1(struct st *a, int a_len)
{
//'a_le' is a typo
#pragma ACCEL interface variable=a depth=a_le
    for (int i = 0; i < 100; i++)
    for (int j = 0; j < 100; j++)   //bracelet
    {
        a[i].a[j] = 0;
    }
}


