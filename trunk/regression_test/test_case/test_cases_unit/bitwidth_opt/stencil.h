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
#include <stdio.h>
#include <stdlib.h>
#include "support.h"

//Define input sizes
#define col_size 640
#define row_size 1280
#define f_size 9

//Data Bounds
#define TYPE int32_t
#define MAX 1000
#define MIN 1

//Set number of iterations to execute
#define MAX_ITERATION 1

/*
void stencil_kernel( TYPE orig[row_size * col_size],
        TYPE sol[row_size * col_size],
        TYPE filter[f_size] );
		*/

////////////////////////////////////////////////////////////////////////////////
// Test harness interface code.

struct bench_args_t {
    TYPE orig[row_size*col_size];
    TYPE sol[row_size*col_size];
    TYPE filter[f_size];
};

#define BATCH 1024


