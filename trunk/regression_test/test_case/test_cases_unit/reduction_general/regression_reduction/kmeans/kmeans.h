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
#define CHUNK_SIZE 8 
#define MAX_NUM_RUNS 1
#define MAX_NUM_CLUSTERS 10
#define MAX_NUM_SAMPLES 10000
#define DATA_STRIDE 1024
#define CENTERS_STRIDE 1024

#ifndef HLS_SIM
//#define SUPPORT_WIDE_BUS
#endif
#ifdef  SUPPORT_WIDE_BUS
#include "ap_int.h"
typedef ap_uint<512> BUS_TYPE;
#else
typedef double BUS_TYPE;
#endif

typedef BUS_TYPE BTYPE;
typedef double DTYPE;
// configurations
//const int L = 10;     // max no. of centers
//const int D = 1024;   // max dimension of a point
//const int F_UR = 4;   // unroll factor for dimensionypedef double DTYPE;
#define L 10     // max no. of centers
#define D 1024   // max dimension of a point
#define F_UR 4   // unroll factor for dimensionypedef double DTYPE;

#ifdef __cplusplus
extern "C" {
#endif
void kmeans(
    int num_samples,
    int num_runs,
    int num_clusters,
    int vector_length,
    BUS_TYPE* data,
    double* centers,
    double* output, 
    int data_size, 
    int center_size, 
    int output_size
    );
#ifdef __cplusplus
}
#endif

