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



//; my $cfg_root = param_tree_define("dummy", "systolic_cfg.xml", "item_for_array");

//; my $outer_loop_bounds = $cfg_root->{Host_Params}->{outer_loop_bounds};
//; my $feeder_loop_bounds = $cfg_root->{Host_Params}->{feeder_loop_bounds};

#ifndef __BLOCK_WISE_REFORMAT_H_
#define __BLOCK_WISE_REFORMAT_H_

#include "__merlinhead_kernel_top.h"
#include "systolic_params.h"

#define TEST_CNN 0
#define TEST_GOLDEN_BW 0

/*
 * for conv
 * 0: r
 * 1: c
 * */
#define CNN_PE_ROW_SWITCH 0

#if (CNN_PE_ROW_SWITCH == 0)
#define R PE_ROWS
#else
#define C PE_ROWS
#endif // CNN_PE_ROW_SWITCH
#define O PE_COLS
#define I PE_DSIZE

#if (TEST_CNN == 1)

/********CNN parameters*********/

/*
 * manual backend parameters for test1
 * */
//#define BN_R 2
//#define BN_C 1
//#define BN_O 3
//#define BN_I 2
//
//#define BS_R 2
//#define BS_C 4
//#define BS_O 4
//#define BS_I 2
//
//#define K 3

/*
 * manual flow parameters for test10
 * */
#define BN_R 2
#define BN_C 1
#define BN_O 4
#define BN_I 4

#define BS_R 4
#define BS_C 8
#define BS_O 1
#define BS_I 1

#define K 3


//#define IMG_IN_NUM (BN_I * BS_I * I)
//#define IMG_OUT_NUM (BN_O * BS_O * O)
//#define IMG_ROW (BN_R * BS_R * R)
//#define IMG_COL (BN_C * BS_C)

#define IMG_IN_NUM 32
#define IMG_OUT_NUM 137
#define IMG_ROW 16
#define IMG_COL 8

/*
 * manual backend parameters for test3
 * */
/*
#define BN_R 2
#define BN_C 1
#define BN_O 2
#define BN_I 1

#define BS_R 8
#define BS_C 1
#define BS_O 7
#define BS_I 4
#define K 3

#define IMG_IN_NUM 32
#define IMG_OUT_NUM 64
#define IMG_ROW 16
#define IMG_COL 32
*/

/********End of CNN parameters*********/
#else

/********MATMUL parameters*********/

// TODO: fill bounds from systolic_cfg.xml
//const int bounds[] = {1, 2, 2, 2, 2, 32};
const int bounds[] = {
//; my @v_outer_loop_bounds = split(',', $outer_loop_bounds);
//; for (my $i = 0; $i < @v_outer_loop_bounds ; $i++){
	`$v_outer_loop_bounds[$i]`,
//; }

//; my @v_feeder_loop_bounds = split(',', $feeder_loop_bounds);
//; for (my $i = 0; $i < @v_feeder_loop_bounds ; $i++){
	`$v_feeder_loop_bounds[$i]`
//; if ($i != @v_feeder_loop_bounds - 1)
//; {
	,
//; }
//; }
};

// FIXME: this pattern must be fixed for now
#define BN_L bounds[0] // K
#define BN_M bounds[1] // I
#define BN_N bounds[2] // J

#define BS_L bounds[3]
#define BS_M bounds[4]
#define BS_N bounds[5]

#if 0
#define BN_I 1
#define BN_J 2
#define BN_K 2

#define BS_I 2
#define BS_J 2
#define BS_K 32
#endif

//#define M (BN_M * BS_M * R)
//#define N (BN_N * BS_N * O)
//#define L (BN_L * BS_L * I)

#define M 16
#define N 16
#define L 32

/********End of MATMUL parameters*********/
#endif

void block_wise_reformat_weight(data_precision *weight, data_precision *weight_block_wise);
void block_wise_reformat_input(data_precision *input, data_precision *input_block_wise);
void block_wise_reformat_output(data_precision *output_block_wise, data_precision *output);
void data_reorganization(data_precision *output_fm_drain, data_precision *output_fm_opencl_block_wise);

#endif // __BLOCK_WISE_REFORMAT_H_
