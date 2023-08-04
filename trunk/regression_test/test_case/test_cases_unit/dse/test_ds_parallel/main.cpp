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
/****************************************************************************/
// main.cpp
// Copyright (C) 2017 Xilinx, Inc. - All rights reserved.
/****************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#define VEC_SIZE 10000

extern void vec_add_kernel(int a[VEC_SIZE], int b[VEC_SIZE], int c[VEC_SIZE], int inc);


int main(int argc, char* argv[])
{   
    int i=0;
    int inc = 1;
    int * a = (int *)malloc(sizeof(int)*VEC_SIZE); 
    int * b = (int *)malloc(sizeof(int)*VEC_SIZE); 
    int * c_acc = (int *)malloc(sizeof(int)*VEC_SIZE); 

    // init
    for(i=0; i<VEC_SIZE; i++) {
        a[i] = i;
        b[i] = 2*i;
        c_acc[i] = 1;
    }    

    vec_add_kernel(a, b, c_acc, inc);

    free(a);
    free(b);
    free(c_acc);

    return 0;
}


