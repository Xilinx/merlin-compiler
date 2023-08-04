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
//#include <stdlib.h>
#include <math.h>
//#include "../../common/support.h"
#include "support.h"

// Fixed parameters
#define input_dimension  13
#define possible_outputs  3
#define training_sets   163
#define nodes_per_layer  64
#define layers            2
#define learning_rate  0.01
#define epochs            1
#define test_sets        15
#define norm_param    0.005

#define max 1.0
#define offset 0.5

//Data Bounds
#define TYPE double
#define MAX 1000
#define MIN 1

/*
void backprop_kernel(
    TYPE weights1[input_dimension*nodes_per_layer],
    TYPE weights2[nodes_per_layer*nodes_per_layer],
    TYPE weights3[nodes_per_layer*possible_outputs],
    TYPE biases1[nodes_per_layer],
    TYPE biases2[nodes_per_layer],
    TYPE biases3[possible_outputs],
    TYPE training_data[training_sets*input_dimension],
    TYPE training_targets[training_sets*possible_outputs]);
	*/
////////////////////////////////////////////////////////////////////////////////
// Test harness interface code.

struct bench_args_t {
    TYPE weights1[input_dimension*nodes_per_layer];
    TYPE weights2[nodes_per_layer*nodes_per_layer];
    TYPE weights3[nodes_per_layer*possible_outputs];
    TYPE biases1[nodes_per_layer];
    TYPE biases2[nodes_per_layer];
    TYPE biases3[possible_outputs];
    TYPE training_data[training_sets*input_dimension];
    TYPE training_targets[training_sets*possible_outputs];
};
