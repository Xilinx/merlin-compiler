// (C) Copyright 2016-2021 Xilinx, Inc.
// All Rights Reserved.
//
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.


#ifndef TRUNK_SOURCE_OPT_TOOLS_INCLUDE_HEADER_H_
#define TRUNK_SOURCE_OPT_TOOLS_INCLUDE_HEADER_H_

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "math.h"
#define SQR(x) ((x) * (x))
#define CUB(x) ((x) * (x) * (x))

#define data_type float
#define cmost_data_type_FLOAT 1

//  #define IMG_P 8
//  #define IMG_N 8
//  #define IMG_M 8
//  #define TS_P 4
//  #define TS_N 4
//  #define TS_M 4
//
//  #define PITCH_P (IMG_P+2)
//  #define PITCH_N (IMG_N+2)
//  #define PITCH_M (IMG_M+2)
//
//  #define IMG_SIZE (PITCH_M*PITCH_N*PITCH_P)

const int IMG_P = 8;
const int IMG_N = 8;
const int IMG_M = 8;
const int TS_P = 4;
const int TS_N = 4;
const int TS_M = 4;

#define PITCH_P = (IMG_P + 2)
#define PITCH_N = (IMG_N + 2)
#define PITCH_M = (IMG_M + 2)

#define IMG_SIZE = (PITCH_M * PITCH_N * PITCH_P)

union cmost_int_float_type {
  int i;
  float f;
};

#endif  // TRUNK_SOURCE_OPT_TOOLS_INCLUDE_HEADER_H_
