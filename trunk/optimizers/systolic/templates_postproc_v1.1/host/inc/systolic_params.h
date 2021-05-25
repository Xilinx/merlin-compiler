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



#ifndef _PE_SYSTOLIC_ARRAY_H_
#define _PE_SYSTOLIC_ARRAY_H_

#define R 5
#define O 32
#define I 8

#define BS_R 4
#define BS_O 1
#define BS_C 14
#define BS_I 4
#define K 3

//#define BN_O 2
//#define BN_C 1
//#define BN_I 2

#define NUM_CONV 3

//#define PE_DSIZE	8
//#define PE_ROWS	10
//#define PE_COLS	16

#define PE_DSIZE	I
#define PE_ROWS	R
#define PE_COLS	O


//#define BNUM_I	1
//#define BNUM_J	1
//#define	BNUM_K	2



/*
#define BSIZE_I 8
#define	BSIZE_J	16
#define BSIZE_K	72
*/

#define DOT_PROD_VECTOR_SIZE      PE_DSIZE
#define SYS_ARRAY_NUM_ROWS        PE_ROWS
#define SYS_ARRAY_NUM_COLS        PE_COLS


//#define C_OUT_SHIFT_REG_SIZE      128
#define C_OUT_SHIFT_REG_SIZE      BS_R * BS_C * BS_O
#define ACCUM_SHIFT_REG_SIZE      C_OUT_SHIFT_REG_SIZE



/*
#define MAT_A_BLOCK_WIDTH         576
#define MAT_A_BLOCK_HEIGHT        80
#define MAT_A_BLOCK_SIZE          (MAT_A_BLOCK_HEIGHT * MAT_A_BLOCK_WIDTH)
#define MAT_A_BLOCK_NUM_VECTORS   (MAT_A_BLOCK_SIZE / DOT_PROD_VECTOR_SIZE)
*/
#define IN_BLOCK_NUM_VECTORS	(R * BS_R + K - 1) * (BS_C + K - 1) * BS_I

/*
#define MAT_B_BLOCK_HEIGHT        64
#define MAT_B_BLOCK_WIDTH         396
#define MAT_B_BLOCK_SIZE          (MAT_B_BLOCK_HEIGHT  * MAT_B_BLOCK_WIDTH)
#define MAT_B_BLOCK_NUM_VECTORS   (MAT_B_BLOCK_SIZE    / DOT_PROD_VECTOR_SIZE)


#define MAT_C_BLOCK_HEIGHT        MAT_A_BLOCK_HEIGHT
#define MAT_C_BLOCK_WIDTH         MAT_B_BLOCK_WIDTH

#define HA    (BNUM_I * MAT_A_BLOCK_HEIGHT)
#define WA    (BNUM_K * MAT_A_BLOCK_WIDTH) 

#define HB    (BNUM_K * MAT_B_BLOCK_HEIGHT)                                     
#define WB    (BNUM_J* MAT_B_BLOCK_WIDTH)     

#define HC HA                                           
#define WC WB                                     
*/





#endif // _PE_SYSTOLIC_ARRAY_H_

