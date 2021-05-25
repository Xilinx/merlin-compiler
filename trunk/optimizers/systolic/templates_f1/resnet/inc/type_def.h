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


#ifndef __TYPE_DEF_H_
#define __TYPE_DEF_H_

//#include <ap_int.h>
#include "systolic_params.h"

#define DATA_BITWIDTH 8
//#define DATA_BITWIDTH 16
//#define DATA_BITWIDTH 32

#if (DATA_BITWIDTH == 32)
typedef float data_bitwidth;
#elif (DATA_BITWIDTH == 16)
typedef short data_bitwidth;
#elif (DATA_BITWIDTH == 8)
typedef char data_bitwidth;
#else
// other bitwidths
//typedef half data_bitwidth;
#endif


struct vec_input_t
{
	data_bitwidth data[IN_PACK_SIZE];
};

struct vec_output_t
{
	data_bitwidth data[OUT_PACK_SIZE];
};


#endif
