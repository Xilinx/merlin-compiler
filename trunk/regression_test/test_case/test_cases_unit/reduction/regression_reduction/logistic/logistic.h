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

#include <math.h>
#include <stdio.h>
#include <assert.h>

#define LABEL_SIZE      10
//#define LABEL_SIZE      100
//#define LABEL_SIZE      50
#define FEATURE_SIZE    784
#define WEIGHTS_SIZE (LABEL_SIZE*(FEATURE_SIZE+1))  //LABEL_SIZE*(FEATURE_SIZE+1)
#define TOTAL_SIZE 60000
//#define TOTAL_SIZE 64 // org 60000
#define DATA_SIZE (TOTAL_SIZE*(LABEL_SIZE+FEATURE_SIZE)) //TOTAL_SIZE*(LABEL_SIZE+FEATURE_SIZE)
#define CHUNK_SIZE 32
#define DUP 10

void logistic_regression_kernel( int n_samples, float global_weight[WEIGHTS_SIZE], float global_data[DATA_SIZE], float global_gradient[WEIGHTS_SIZE] );
