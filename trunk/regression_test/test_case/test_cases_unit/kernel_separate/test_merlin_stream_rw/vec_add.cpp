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
//#include <stdio.h>
#include "merlin_stream.h"

#pragma ACCEL STREAM_CHANNEL CHANNEL_DEPTH=32 //NODE=vec_add_kernel PORT=prefetch
merlin_stream ch_a_0;
#pragma ACCEL STREAM_CHANNEL
merlin_stream ch_b_0;


#define VEC_SIZE 10000

void vec_add_kernel_s0(int *a);
void vec_add_kernel_s1(int *b, int*c, int inc);

#pragma ACCEL task parallel
void vec_add_kernel(int * a, int * b, int *c, int inc)
{
    vec_add_kernel_s0(a);
    vec_add_kernel_s1(b, c, inc);
}

#pragma ACCEL kernel
void vec_add_kernel_s0(int a[VEC_SIZE])
{
    for (int j = 0; j < VEC_SIZE; j++)
    {
        int tmp = a[j];
        merlin_stream_write(&ch_a_0,(&tmp));
    }
}


#pragma ACCEL kernel
void vec_add_kernel_s1(int b[VEC_SIZE], int c[VEC_SIZE], int inc)
{

    for (int j = 0; j < VEC_SIZE; j++)
    {
        int tmp;
        merlin_stream_read(&ch_b_0,(&tmp));
        c[j] = tmp+inc;
    }
}
