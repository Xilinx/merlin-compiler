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


//; 
//; my $cfg_root = param_tree_define("dummy", "systolic_cfg.xml", "item_for_array");
//; 
//; my $pe_dsize = $cfg_root->{PE}->{PE_DSIZE};
//; my $pe_rows = $cfg_root->{PE}->{PE_ROWS};
//; my $pe_cols = $cfg_root->{PE}->{PE_COLS};
//;
//; my $block_dim_h = $cfg_root->{Loader_H}->{block_dim};
//; my $block_dim_v = $cfg_root->{Loader_V}->{block_dim};
//; 
//; my $block_size_h = $cfg_root->{Loader_H}->{block_size};
//; my $block_size_v = $cfg_root->{Loader_V}->{block_size};
//;
//; my $feeder_iteration_domain= $cfg_root->{Feeder_H}->{iteration_domain};
//;
//; my $buffer_dim_h= $cfg_root->{Feeder_H}->{buffer_dim};
//; my $buffer_dim_v= $cfg_root->{Feeder_V}->{buffer_dim};
//;
//;#  my $debug_print = "on";
//;  my $debug_print = "off";
//;
//;#  my $debug_pe_idx = "on";
//;  my $debug_pe_idx = "off";
//; 

#ifndef _PE_SYSTOLIC_ARRAY_H_
#define _PE_SYSTOLIC_ARRAY_H_

//#define C 5
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

#define NUM_CONV 4

//#define PE_DSIZE	`$pe_dsize`
//#define PE_ROWS	`$pe_rows`
//#define PE_COLS	`$pe_cols`

#define PE_DSIZE	I
#define PE_ROWS	R
#define PE_COLS	O

//; my $bn_k = 1;
//; my @v_block_dim_h = split(',', $block_dim_h);
//; for (my $i = 1; $i < @v_block_dim_h ; $i++){
//;    $bn_k *= $v_block_dim_h[$i];
//; }
//;
//; $bn_i = $v_block_dim_h[0];
//;
//; my $bn_j = 1;
//; my @v_block_dim_v = split(',', $block_dim_v);
//; for (my $i = 0; $i < @v_block_dim_v - 1 ; $i++){
//;    $bn_j *= $v_block_dim_v[$i];
//; }

//#define BNUM_I	`$bn_i`
//#define BNUM_J	`$bn_j`
//#define	BNUM_K	`$bn_k`


//;
//; my @v_buffer_dim_h = split(',', $buffer_dim_h);
//; my $bs_k = 1;
//; for (my $i = @v_buffer_dim_h - 1; $i >= 1; $i--) {
//;    $bs_k *= $v_buffer_dim_h[$i];
//; }
//;
//; my $bs_i = $v_buffer_dim_h[0];
//;
//; my @v_buffer_dim_v = split(',', $buffer_dim_v);
//; my $bs_j = 1;
//; for (my $i = @v_buffer_dim_v - 2, $j = @v_buffer_dim_h - 2; $i >= 0; $i--, $j--) {
//;    $bs_j *= ($v_buffer_dim_v[$i] - $v_buffer_dim_h[$j] + 1);
//; }

/*
#define BSIZE_I `$bs_i`
#define	BSIZE_J	`$bs_j`
#define BSIZE_K	`$bs_k`
*/

#define DOT_PROD_VECTOR_SIZE      PE_DSIZE
#define SYS_ARRAY_NUM_ROWS        PE_ROWS
#define SYS_ARRAY_NUM_COLS        PE_COLS

//; my $c_out_shift_reg_size = 1;
//; my @v_feeder_iteration_domain = split(',', $feeder_iteration_domain);
//; for (my $i = 3; $i < @v_feeder_iteration_domain; $i++){
//;    $c_out_shift_reg_size *= $v_feeder_iteration_domain[$i];
//; }

//#define C_OUT_SHIFT_REG_SIZE      `$c_out_shift_reg_size`
#define C_OUT_SHIFT_REG_SIZE      BS_R * BS_C * BS_O
#define ACCUM_SHIFT_REG_SIZE      C_OUT_SHIFT_REG_SIZE

//; my $h_block_width = 1;
//; my @v_block_size_h = split(',', $block_size_h);
//; for (my $i = 1; $i < @v_block_size_h ; $i++){
//;    $h_block_width *= $v_block_size_h[$i];
//; }
//;
//; my $h_block_height = $v_block_size_h[0];

//; my $v_block_width = 1;
//; my @v_block_size_v = split(',', $block_size_v);
//; for (my $i = 0; $i < @v_block_size_v - 1 ; $i++){
//;    $v_block_width *= $v_block_size_v[$i];
//; }
//;
//; my $v_block_height = $v_block_size_v[@v_block_size_v - 1];

/*
#define MAT_A_BLOCK_WIDTH         `$h_block_width`
#define MAT_A_BLOCK_HEIGHT        `$h_block_height`
#define MAT_A_BLOCK_SIZE          (MAT_A_BLOCK_HEIGHT * MAT_A_BLOCK_WIDTH)
#define MAT_A_BLOCK_NUM_VECTORS   (MAT_A_BLOCK_SIZE / DOT_PROD_VECTOR_SIZE)
*/
#define IN_BLOCK_NUM_VECTORS	(R * BS_R + K - 1) * (BS_C + K - 1) * BS_I

/*
#define MAT_B_BLOCK_HEIGHT        `$v_block_height`
#define MAT_B_BLOCK_WIDTH         `$v_block_width`
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


//; if ($debug_print eq "on") {
#define KERNEL_TIMEOUT 120
//#define DEBUG_KERNEL_IDX=1 // to enable grep_last
//; }

//; if ($debug_pe_idx eq "on") {
#define DEBUG_PE 1
#define DEBUG_PE_I 3
#define DEBUG_PE_J 5
#define DEBUG_PE_IDX 76
#define CPU_PE_DEBUG_I 3
#define CPU_PE_DEBUG_J 5
#define CPU_PE_DEBUG_IDX 76
//; }


#endif // _PE_SYSTOLIC_ARRAY_H_

