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


#ifndef __CNN_H_
#define __CNN_H_

//; 
//; my $cfg_root = param_tree_define("dummy", "systolic_cfg.xml", "item_for_array");
//; 
//; my $img_in_num = $cfg_root->{Conv_Params}->{IMG_IN_NUM};
//; my $img_out_num = $cfg_root->{Conv_Params}->{IMG_OUT_NUM};
//; my $img_row = $cfg_root->{Conv_Params}->{IMG_ROW};
//; my $img_col = $cfg_root->{Conv_Params}->{IMG_COL};
//; my $kernel = $cfg_root->{Conv_Params}->{KERNEL};


//#define IMG_IN_NUM  `$img_in_num` // I * BS_I * BN_I
//#define IMG_OUT_NUM `$img_out_num` //   O * BS_O * B_O
//#define IMG_ROW `$img_row`  //       BSIZE_J * PE_COLS
//#define IMG_COL `$img_col` //         BNUM_J
//
//#define KERNEL  `$kernel`   //        BSIZE_I=K*K
//#define IMG_IN_ROW (IMG_ROW + KERNEL-1)
//#define IMG_IN_COL (IMG_COL + KERNEL-1)


//#define O 10
//#define R 16
//#define I 8
//
//#define BS_O 8
//#define BS_R 4
//#define BS_I 10
//#define BS_C 4
////#define K 3
//
//#define BN_O 2
//#define BN_C 1
//#define BN_I 2


//#define NUM_CONV 1

//const unsigned int cfg_in_num[] = {16, 64};
//const unsigned int cfg_out_num[] = {64, 64};
//const unsigned int cfg_img_row[] = {224, 224};
//const unsigned int cfg_img_col[] = {224, 224};
//const unsigned int cfg_kernel[] = {3, 3};
//
//const unsigned int cfg_BN_R[] = {12, 12};
//const unsigned int cfg_BN_C[] = {16, 16};
//const unsigned int cfg_BN_O[] = {2, 2};
//const unsigned int cfg_BN_I[] = {1, 2};

//const unsigned int cfg_pooling_size[] = {1, 2};

void block_wise_reformat_weight(float *weight, float *weight_block_wise);
void block_wise_reformat_input(float *input, float *input_block_wise);
void block_wise_reformat_output(float *output_block_wise, float *output);

void cnn_gold(
       float *weight,
       float *input_fm,
       float *output_fm
		);

#endif // __CNN_H_
