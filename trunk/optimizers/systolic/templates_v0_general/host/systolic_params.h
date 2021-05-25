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
//; my $reuse_num_o= $cfg_root->{Drain_O}->{reuse_num};

//;
//;#  my $debug_print = "on";
//;  my $debug_print = "off";
//;
//;#  my $debug_pe_idx = "on";
//;  my $debug_pe_idx = "off";
//; 

#ifndef _PE_SYSTOLIC_ARRAY_H_
#define _PE_SYSTOLIC_ARRAY_H_

#define PE_ROWS `$pe_rows`
#define PE_COLS `$pe_cols`
#define PE_DSIZE `$pe_dsize`


#define DOT_PROD_VECTOR_SIZE      PE_DSIZE
#define SYS_ARRAY_NUM_ROWS        PE_ROWS
#define SYS_ARRAY_NUM_COLS        PE_COLS

//; my $c_out_shift_reg_size = 1;
//; my @v_reuse_num_o = split(',', $reuse_num_o);
//; for (my $i = 0; $i < @v_reuse_num_o; $i++){
//;    $c_out_shift_reg_size *= $v_reuse_num_o[$i];
//; }

#define C_OUT_SHIFT_REG_SIZE      `$c_out_shift_reg_size`
//#define C_OUT_SHIFT_REG_SIZE      BS_R * BS_C * BS_O
#define ACCUM_SHIFT_REG_SIZE      C_OUT_SHIFT_REG_SIZE

//; my $h_block_num_vectors = 1;
//; my @v_block_size_h = split(',', $block_size_h);
//; for (my $i = 0; $i < @v_block_size_h; $i++){
//;    $h_block_num_vectors *= $v_block_size_h[$i];
//; }
//; $h_block_num_vectors /= $pe_dsize;

//#define IN_BLOCK_NUM_VECTORS	(R * BS_R + K - 1) * (BS_C + K - 1) * BS_I
#define IN_BLOCK_NUM_VECTORS	`$h_block_num_vectors`


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

