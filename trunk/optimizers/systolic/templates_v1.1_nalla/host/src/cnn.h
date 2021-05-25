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


typedef float data_precision;

//#define IMG_IN_NUM  128 // I * BS_I * BN_I
//#define IMG_OUT_NUM 80 //   O * BS_O * B_O
//#define IMG_ROW 64  //       BSIZE_J * PE_COLS
//#define IMG_COL 4 //         BNUM_J
//
//#define KERNEL  3   //        BSIZE_I=K*K
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

#define TEST_LAYER 1
#define DEBUG_CONV 0
#define DEBUG_POOLING 0
#define DEBUG_ZERO_PADDING 0

const unsigned int cfg_in_num[] = {16, 64, 64, 128, 128, 256, 256, 256, 512, 512, 512, 512, 512};
const unsigned int cfg_out_num[] = {64, 64, 128, 128, 256, 256, 256, 512, 512, 512, 512, 512, 512};
const unsigned int cfg_img_row[] = {224, 224, 112, 112, 56, 56, 56, 28, 28, 28, 14, 14, 14};
const unsigned int cfg_img_col[] = {224, 224, 112, 112, 56, 56, 56, 28, 28, 28, 14, 14, 14};
const unsigned int cfg_kernel[] = {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3};

const unsigned int cfg_BN_R[] = {12, 12, 6, 6, 3, 3, 3, 2, 2, 2, 1, 1, 1};
const unsigned int cfg_BN_C[] = {16, 16, 8, 8, 4, 4, 4, 2, 2, 2, 1, 1, 1};
const unsigned int cfg_BN_O[] = {2, 2, 4, 4, 8, 8, 8, 16, 16, 16, 16, 16, 16};
const unsigned int cfg_BN_I[] = {1, 2, 2, 4, 4, 8, 8, 8, 16, 16, 16, 16, 16};

const unsigned int cfg_pooling_size[] = {1, 2, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 1};

void block_wise_reformat_weights(int layer, data_precision *weights, data_precision *weights_block_wise);
void block_wise_reformat_input(int layer, data_precision *input, data_precision *input_block_wise);
void block_wise_reformat_output(int layer, data_precision *output_block_wise, data_precision *output);

void zero_padding_block_wise(int layer, data_precision *out, data_precision *out_padded);
void zero_padding(int layer, data_precision *out, data_precision *out_padded);

void cnn_gold(
       data_precision *weights,
       data_precision *input_fm,
       data_precision *output_fm
		);

#endif // __CNN_H_
