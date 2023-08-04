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
#ifndef _PE_SYSTOLIC_ARRAY_H_
#define _PE_SYSTOLIC_ARRAY_H_

// design space exploration of three vector sizes: float4, float8 and float16
//#define DOT_PROD_VECTOR_SIZE 16
//#define DOT_PROD_VECTOR_LEVELS 4

#define DOT_PROD_VECTOR_SIZE      8
#define DOT_PROD_VECTOR_LEVELS    3

#define ACCUM_SHIFT_REG_SIZE      512
#define C_OUT_SHIFT_REG_SIZE      ACCUM_SHIFT_REG_SIZE

#define SYS_ARRAY_NUM_ROWS        4
#define SYS_ARRAY_NUM_COLS        4

#define ROWS_INTERLEAVED          32
#define COLUMNS_INTERLEAVED       32

#define MAT_A_BLOCK_WIDTH         128
#define MAT_A_BLOCK_HEIGHT        (ROWS_INTERLEAVED   * SYS_ARRAY_NUM_ROWS)
#define MAT_A_BLOCK_SIZE          (MAT_A_BLOCK_HEIGHT * MAT_A_BLOCK_WIDTH)
#define MAT_A_BLOCK_NUM_VECTORS   (MAT_A_BLOCK_SIZE   / DOT_PROD_VECTOR_SIZE)

#define MAT_B_BLOCK_HEIGHT        MAT_A_BLOCK_WIDTH
#define MAT_B_BLOCK_WIDTH         (COLUMNS_INTERLEAVED * SYS_ARRAY_NUM_COLS)
#define MAT_B_BLOCK_SIZE          (MAT_B_BLOCK_HEIGHT  * MAT_B_BLOCK_WIDTH)
#define MAT_B_BLOCK_NUM_VECTORS   (MAT_B_BLOCK_SIZE    / DOT_PROD_VECTOR_SIZE)

#define MAT_C_BLOCK_HEIGHT        MAT_A_BLOCK_HEIGHT
#define MAT_C_BLOCK_WIDTH         MAT_B_BLOCK_WIDTH



#include "cnn.h"

#include "PE_systolic_array_generic_params.h"


#endif // _PE_SYSTOLIC_ARRAY_H_

