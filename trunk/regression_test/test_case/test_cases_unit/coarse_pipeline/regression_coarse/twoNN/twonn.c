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
#include <string.h>
#include <math.h>
//#include "cmost.h"

#define DUMP 0
#define learning_rate 0.01f
#define LABEL_SIZE		10
#define HIDDEN_SIZE     50
#define FEATURE_SIZE	784
#define CHUNK_SIZE 20
#define lambda 0.0001f
#define two_lambda 0.0002f
#define two_lambda_learning_rate 0.000002f
#define learning_rate_CHUNK_SIZE 0.0005f
#define WEIGHTS_SIZE 238510 //(FEATURE_SIZE+1)*HIDDEN_SIZE+(HIDDEN_SIZE+1)*LABEL_SIZE
#define TOTAL_SIZE 60000
#define DATA_SIZE 47640000

void twonn_kernel( int n_samples, volatile float* global_weights, volatile float* global_data );

int main()
{   
    int inc1 = 0, inc2 = 0;
    int n_samples = TOTAL_SIZE;
    float * global_weights; cmost_malloc_1d( &global_weights, "0" , 4, WEIGHTS_SIZE);
    float * global_data; cmost_malloc_1d( &global_data, "global.dat", 4, DATA_SIZE);
	if (!(global_weights && global_data)) while (1);

    twonn_kernel( n_samples, global_weights, global_data );

    cmost_dump_1d(global_weights, "weights_out.dat");
    cmost_free_1d(global_data);
    cmost_free_1d(global_weights);
    return 0;
}

