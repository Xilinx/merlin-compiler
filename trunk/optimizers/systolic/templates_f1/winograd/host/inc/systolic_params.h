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


#ifndef __SYSTOLIC_H_
#define __SYSTOLIC_H_

//#include "hls_half.h"

#define WINOGRAD 1

#define BIG_DESIGN 0


//#define DATA_BITWIDTH 32
#define DATA_BITWIDTH 8

#if (BIG_DESIGN == 0)

#define BN_R 1
#define BN_O 1
#define BN_I 1

#define BS_R 1
#define BS_C 1
#define BS_O 1
#define BS_I 1

#define R 1
#define C 4
#define O 1
#define I 2

#elif (BIG_DESIGN == 1)

#define BN_R 1
#define BN_O 1
#define BN_I 1

#define BS_R 32
#define BS_O 16
#define BS_I 16

#define R 4
#define O 32
#define I 16

#else // ResNet

#define BN_R 4
#define BN_O 4
#define BN_I 8

#define BS_R 32
#define BS_O 16
#define BS_I 16

#define R 8
#define O 16
#define I 16

#endif

#define K1 3
#define K2 3

#define SYS_ARRAY_NUM_ROWS R
#define SYS_ARRAY_NUM_COLS O
#define SYS_ARRAY_PE_DSIZE I

#define IN_PACK_SIZE I
#define OUT_PACK_SIZE O

#define ACCUM_SHIFT_REG_SIZE (BS_R * BS_C * BS_O)
#define O_OUT_SHIFT_REG_SIZE (BS_R * BS_C * BS_O)

#define NUM_BLOCKS (BN_R * BN_O * BN_I)
#define TOTAL_NUM_BLOCKS (BN_R * BN_O + 1) * BN_I

#define NUM_SHIFT_REG_PARTITION 1

#if (DATA_BITWIDTH == 32)
typedef float data_bitwidth;
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

#endif // __SYSTOLIC_H_
