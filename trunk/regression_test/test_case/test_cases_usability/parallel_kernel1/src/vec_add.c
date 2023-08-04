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

#define VEC_SIZE 10000

void vec_add_kernel(int *a, int *b, int*c, int inc);


int main()
{   
    int i;

    int inc = 0;
    int * a; cmost_malloc_1d( &a, "+" , 4, VEC_SIZE);
    int * b; cmost_malloc_1d( &b, "0", 4, VEC_SIZE);
    int * c; cmost_malloc_1d( &c, "0"  , 4, VEC_SIZE);
    if (!(a && b && c)) while(1);

    for (i = 0; i < VEC_SIZE; i++) b[i] = i*i;

#pragma ACCEL task name="task1" async
    vec_add_kernel(a, b, c, inc);


#pragma ACCEL taskwait task1 

    cmost_dump_1d(c, "c_out.dat"); // added
    cmost_free_1d(a);
    cmost_free_1d(b);
    cmost_free_1d(c);




    return 0;
}

