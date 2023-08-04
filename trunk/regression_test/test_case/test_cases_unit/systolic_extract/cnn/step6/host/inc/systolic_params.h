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

#define PE_DSIZE	1
#define PE_ROWS	4
#define PE_COLS	4
#define BNUM_I	4
#define BNUM_J	2
#define	BNUM_K	1
#define	K	4
#define	BSIZE_J	16
#define BSIZE_K	32


#define BSIZE_I (K * K)

//#define ROWS_INTERLEAVED          BSIZE_I // 
#define ROWS_INTERLEAVED			(K * K) // 16
#define COLUMNS_INTERLEAVED       BSIZE_J // 16

// design space exploration of three vector sizes: float4, float8 and float16
//#define DOT_PROD_VECTOR_SIZE 16
//#define DOT_PROD_VECTOR_LEVELS 4

#define DOT_PROD_VECTOR_SIZE      PE_DSIZE
#define SYS_ARRAY_NUM_ROWS        PE_ROWS
#define SYS_ARRAY_NUM_COLS        PE_COLS
//#define DOT_PROD_VECTOR_LEVELS    3

//#define ACCUM_SHIFT_REG_SIZE      1024
//#define C_OUT_SHIFT_REG_SIZE      ACCUM_SHIFT_REG_SIZE
#define C_OUT_SHIFT_REG_SIZE      (ROWS_INTERLEAVED * COLUMNS_INTERLEAVED)
#define ACCUM_SHIFT_REG_SIZE      C_OUT_SHIFT_REG_SIZE



//#define MAT_A_BLOCK_WIDTH         128
#define MAT_A_BLOCK_WIDTH         BSIZE_K * DOT_PROD_VECTOR_SIZE
#define MAT_A_BLOCK_HEIGHT        (ROWS_INTERLEAVED   * SYS_ARRAY_NUM_ROWS)
#define MAT_A_BLOCK_SIZE          (MAT_A_BLOCK_HEIGHT * MAT_A_BLOCK_WIDTH)
#define MAT_A_BLOCK_NUM_VECTORS   (MAT_A_BLOCK_SIZE   / DOT_PROD_VECTOR_SIZE)

#define MAT_B_BLOCK_HEIGHT        MAT_A_BLOCK_WIDTH
//#define MAT_B_BLOCK_WIDTH         (COLUMNS_INTERLEAVED * SYS_ARRAY_NUM_COLS)         // MatrixMult
#define MAT_B_BLOCK_WIDTH         (COLUMNS_INTERLEAVED * SYS_ARRAY_NUM_COLS + K-1) * K // CNN
#define MAT_B_BLOCK_SIZE          (MAT_B_BLOCK_HEIGHT  * MAT_B_BLOCK_WIDTH)
#define MAT_B_BLOCK_NUM_VECTORS   (MAT_B_BLOCK_SIZE    / DOT_PROD_VECTOR_SIZE)

#define MAT_C_BLOCK_HEIGHT        MAT_A_BLOCK_HEIGHT
#define MAT_C_BLOCK_WIDTH         MAT_B_BLOCK_WIDTH

#define HA    (BNUM_I * MAT_A_BLOCK_HEIGHT)       // Matrix A height
#define WA    (BNUM_K * MAT_A_BLOCK_WIDTH)    // Matrix A width

#define HB    WA                                          // Matrix B height
#define WB    (BNUM_J* MAT_B_BLOCK_WIDTH)       // Matrix B width

#define HC HA                                             // Matrix C height
#define WC WB                                             // Matrix C width 


// for debugging
//#define PE_DEBUG 1
//#define KERNEL_TIMEOUT=20
//#define DEBUG_KERNEL_IDX=1 // to enable grep_last
#define DEBUG_PE_I 0
#define DEBUG_PE_J 0
#define DEBUG_PE_IDX 16
#define CPU_PE_DEBUG_I 0
#define CPU_PE_DEBUG_J 0
#define CPU_PE_DEBUG_IDX 16


#endif // _PE_SYSTOLIC_ARRAY_H_

