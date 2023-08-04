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


#define VEC_SIZE 1000000

class class_a{

public:
    int m_a [VEC_SIZE];
    void add( int * in, int * out, int remain)
    {
        int j;
        for (j = 0 ;j < VEC_SIZE; j++) 
        {
#pragma ACCEL pipeline_parallel factor = 16
            out[j] = in[j] + m_a[j] + remain;
        }
    }
};


void vec_add_kernel(int *a, int *b, int*c, int inc)
{
    int j;
    class class_a obj;
    for (j = 0 ;j < VEC_SIZE; j++) 
    {
#pragma ACCEL pipeline_parallel factor = 16
        obj.m_a[j] = a[j];
    }

    obj.add(b, c, inc);
}



