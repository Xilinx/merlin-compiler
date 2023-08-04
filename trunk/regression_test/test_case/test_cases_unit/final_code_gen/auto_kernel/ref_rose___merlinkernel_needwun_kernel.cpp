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
#define get_compute_id(x) x
extern int __merlin_include__GB__0;
#pragma OPENCL EXTENSION cl_altera_channels : enable
// Original: #pragma ACCEL array_partition type=channel variable=ch__i_sub_l5_4 dim=1 factor=32
// Original: #pragma ACCEL array_partition type=channel variable=ch_alignedB_buf_0_3 dim=1 factor=32
// Original: #pragma ACCEL array_partition type=channel variable=ch_alignedA_buf_0_2 dim=1 factor=32
// Original: #pragma ACCEL array_partition type=channel variable=ch_SEQB_buf_0_1 dim=1 factor=32
// Original: #pragma ACCEL array_partition type=channel variable=ch_SEQA_buf_0_0 dim=1 factor=32
#include "merlin_stream.h" 
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy__pragma_ACCEL_STREAM_CHANNEL_C_();
#ifndef __defined_ch__i_sub_l5_4_p
#define __defined_ch__i_sub_l5_4_p
#pragma channel int ch__i_sub_l5_4_p[32] __attribute__((depth(32)));
typedef int __merlin_channel_typedef;
#include "channel_def.h"
__merlin_channel_typedef ch__i_sub_l5_4_p[32];
#endif
static void __merlin_dummy__pragma_ACCEL_STREAM_CHANNEL_C_();
#ifndef __defined_ch_alignedB_buf_0_3_p
#define __defined_ch_alignedB_buf_0_3_p
#pragma channel char ch_alignedB_buf_0_3_p[32] __attribute__((depth(256)));
typedef int __merlin_channel_typedef;
#include "channel_def.h"
::__merlin_channel_typedef ch_alignedB_buf_0_3_p[32];
#endif
static void __merlin_dummy__pragma_ACCEL_STREAM_CHANNEL_C_();
#ifndef __defined_ch_alignedA_buf_0_2_p
#define __defined_ch_alignedA_buf_0_2_p
#pragma channel char ch_alignedA_buf_0_2_p[32] __attribute__((depth(256)));
typedef int __merlin_channel_typedef;
#include "channel_def.h"
::__merlin_channel_typedef ch_alignedA_buf_0_2_p[32];
#endif
static void __merlin_dummy__pragma_ACCEL_STREAM_CHANNEL_C_();
#ifndef __defined_ch_SEQB_buf_0_1_p
#define __defined_ch_SEQB_buf_0_1_p
#pragma channel char ch_SEQB_buf_0_1_p[32] __attribute__((depth(128)));
typedef int __merlin_channel_typedef;
#include "channel_def.h"
::__merlin_channel_typedef ch_SEQB_buf_0_1_p[32];
#endif
static void __merlin_dummy__pragma_ACCEL_STREAM_CHANNEL_C_();
#ifndef __defined_ch_SEQA_buf_0_0_p
#define __defined_ch_SEQA_buf_0_0_p
#pragma channel char ch_SEQA_buf_0_0_p[32] __attribute__((depth(128)));
typedef int __merlin_channel_typedef;
#include "channel_def.h"
::__merlin_channel_typedef ch_SEQA_buf_0_0_p[32];
#endif
#include <string.h> 
#include "cmost.h"
#include "merlin_type_define.h"
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();

static void needwun_core(int _i_sub_l5)
{
//int M[(ALEN+1)*(BLEN+1)];
  int M_shift_reg[2][128];
  char ptr[16641];
  int score;
  int up_left;
  int up;
  int left;
  int max;
  int row;
  int r;
  int a_idx;
  int b_idx;
  int a_str_idx;
  int b_str_idx;
  char SEQA_buf[128];
  char SEQB_buf[128];
  for (int _i_l1 = 0; _i_l1 < 128; ++_i_l1) {
    char SEQB_buf_0_sn_tmp_0;
    char SEQA_buf_0_sn_tmp_0;
    SEQA_buf_0_sn_tmp_0 = ((char )(read_channel_altera_ch_SEQA_buf_0_0_p__merlinalterafuncend(ch_SEQA_buf_0_0_p[_i_sub_l5])));
    SEQA_buf[_i_l1] = SEQA_buf_0_sn_tmp_0;
    SEQB_buf_0_sn_tmp_0 = ((char )(read_channel_altera_ch_SEQB_buf_0_1_p__merlinalterafuncend(ch_SEQB_buf_0_1_p[_i_sub_l5])));
    SEQB_buf[_i_l1] = SEQB_buf_0_sn_tmp_0;
  }
/*
    #pragma ACCEL interface variable=SEQA depth=128
    #pragma ACCEL interface variable=SEQB depth=128
    #pragma ACCEL interface variable=alignedA depth=256
    #pragma ACCEL interface variable=alignedB depth=256
    #pragma ACCEL interface variable=alignedA bus_bitwidth=256 
    #pragma ACCEL interface variable=alignedB bus_bitwidth=256 
    #pragma ACCEL interface variable=M depth=16641
    #pragma ACCEL interface variable=ptr depth=16641
    */
//#pragma ACCEL parallel
//    init_row: for(a_idx=0; a_idx<(ALEN+1); a_idx++){
//M[a_idx] = a_idx * GAP_SCORE;
//M_shift_reg[a_idx] = a_idx * GAP_SCORE;
//  }
//init_col: for(b_idx=0; b_idx<(BLEN+1); b_idx++){
//    M[b_idx*(ALEN+1)] = b_idx * GAP_SCORE;
//}
  int M_pre = 0;
  int M_up_pre = 0;
// Matrix filling loop
  fill_out:
  for (b_idx = 1; b_idx < 128 + 1; b_idx++) {
    row = b_idx * (128 + 1);
//M[row];
    M_pre = b_idx * - 1;
//M[row_up];
    M_up_pre = b_idx * - 1;
    fill_in:
    for (a_idx = 1; a_idx < 128 + 1; a_idx++) {
      if (((int )SEQA_buf[a_idx - 1]) == ((int )SEQB_buf[b_idx - 1])) {
        score = 1;
      }
       else {
        score = - 1;
      }
//int M_up = M[row_up + (a_idx  )] ;
      int rose_temp;
      if (b_idx == 1) {
        rose_temp = (a_idx + 1) * - 1;
      }
       else {
        rose_temp = M_shift_reg[0][1];
      }
      int M_up = rose_temp;
// M_up += GAP_SCORE;
      up_left = M_up_pre + score;
      up = M_up;
      left = M_pre + - 1;
      M_up_pre = M_up - - 1;
{
        char rose__temp = (char )true;
        int rose_temp_0;
        if (up_left > up) {
          rose_temp_0 = up_left;
        }
         else {
          rose_temp_0 = up;
        }
        rose__temp = ((char )((bool )(left > rose_temp_0)));
        if ((bool )rose__temp) {
          max = left;
        }
         else {
          if (up_left > up) {
            max = up_left;
          }
           else {
            max = up;
          }
        }
      }
//M[row + a_idx] = 
      M_pre = max;
      char tmp_ch;
      if (max == left) {
//ptr[row + a_idx] = SKIPB;
        tmp_ch = '<';
      }
       else {
        if (max == up) {
//ptr[row + a_idx] = SKIPA;
          tmp_ch = '^';
        }
         else {
//ptr[row + a_idx] = ALIGN;
          tmp_ch = '\\';
        }
      }
      ptr[row + a_idx] = tmp_ch;
//#pragma unroll 
      
#pragma unroll
      for (int _t_l2 = 0; _t_l2 < 128 - 1; _t_l2++) 
// Original: #pragma ACCEL parallel
// Original: #pragma ACCEL PARALLEL COMPLETE
{
        M_shift_reg[0][_t_l2] = M_shift_reg[0][1 + _t_l2];
      }
      M_shift_reg[0][127] = M_shift_reg[1][0] + - 1;
      M_shift_reg[1][0] = M_pre;
    }
  }
// TraceBack (n.b. aligned sequences are backwards to avoid string appending)
  a_idx = 128;
  b_idx = 128;
  a_str_idx = 0;
  b_str_idx = 0;
  char alignedA_t[256];
  char alignedB_t[256];
  trace:
  while(a_idx > 0 || b_idx > 0){
    
#pragma HLS loop_tripcount max=128
    r = b_idx * (128 + 1);
    if (((int )ptr[r + a_idx]) == ((int )'\\')) {
      alignedA_t[a_str_idx++] = SEQA_buf[a_idx - 1];
      alignedB_t[b_str_idx++] = SEQB_buf[b_idx - 1];
      a_idx--;
      b_idx--;
    }
     else {
      if (((int )ptr[r + a_idx]) == ((int )'<')) {
        alignedA_t[a_str_idx++] = SEQA_buf[a_idx - 1];
        alignedB_t[b_str_idx++] = '-';
        a_idx--;
      }
       else 
// SKIPA
{
        alignedA_t[a_str_idx++] = '-';
        alignedB_t[b_str_idx++] = SEQB_buf[b_idx - 1];
        b_idx--;
      }
    }
  }
// Pad the result
  int b1 = a_str_idx;
  pad_a:
  for (a_str_idx = 0; a_str_idx < 128 + 128; a_str_idx++) {
    char alignedA_buf_0_sn_tmp_0;
    
#pragma HLS loop_tripcount max=256
//alignedA[a_str_idx] = (a_str_idx < b1) ? alignedA_t[a_str_idx] : '_';
    char rose_temp_1;
    if (a_str_idx < b1) {
      rose_temp_1 = alignedA_t[a_str_idx];
    }
     else {
      rose_temp_1 = '_';
    }
    char tmp = rose_temp_1;
    alignedA_buf_0_sn_tmp_0 = tmp;
    write_channel_altera_ch_alignedA_buf_0_2_p__merlinalterafuncend(ch_alignedA_buf_0_2_p[_i_sub_l5],alignedA_buf_0_sn_tmp_0);
  }
  int b2 = b_str_idx;
  pad_b:
  for (b_str_idx = 0; b_str_idx < 128 + 128; b_str_idx++) {
    char alignedB_buf_0_sn_tmp_0;
    
#pragma HLS loop_tripcount max=256
//alignedB[b_str_idx] = (b_str_idx < b2) ? alignedB_t[b_str_idx] :'_';
    char rose_temp_2;
    if (b_str_idx < b2) {
      rose_temp_2 = alignedB_t[b_str_idx];
    }
     else {
      rose_temp_2 = '_';
    }
    char tmp = rose_temp_2;
    alignedB_buf_0_sn_tmp_0 = tmp;
    write_channel_altera_ch_alignedB_buf_0_3_p__merlinalterafuncend(ch_alignedB_buf_0_3_p[_i_sub_l5],alignedB_buf_0_sn_tmp_0);
  }
}
static void __merlin_dummy_kernel_pragma();
// Original: #pragma ACCEL kernel
void __merlin_dummy_msm_node_0();
 __attribute__ (( autorun))
 __attribute__ (( num_compute_units(32)))
 __attribute__ (( max_global_work_dim(0)))

__kernel void msm_node_0()
{
  int id_0 = get_compute_id(0);
  while(1){
    int _i_sub_l5_sn_tmp_0;
    int __i_sub_l5;
    _i_sub_l5_sn_tmp_0 = ((int )(read_channel_altera_ch__i_sub_l5_4_p__merlinalterafuncend(ch__i_sub_l5_4_p[id_0])));
    __i_sub_l5 = _i_sub_l5_sn_tmp_0;
    needwun_core(id_0);
//alignedB + (ALEN+BLEN) * i, M + ((ALEN+1)*(BLEN+1)) * i, ptr + ((ALEN+1)*(BLEN+1)) * i);
  }
}

void msm_port_SEQA_buf_0_msm_node_0_0(char SEQA_buf_0[32][128],int _i_sub_l5)
{
  int __i_sub_l5;
  __i_sub_l5 = _i_sub_l5;
  for (int _i_l1 = 0; _i_l1 < 128; ++_i_l1) {
    int _i_sub_l5_0;
{
      
#pragma unroll
      for (_i_sub_l5_0 = ((int )0); _i_sub_l5_0 <= ((int )31); ++_i_sub_l5_0) {
        char SEQA_buf_0_sp_tmp_0;
        SEQA_buf_0_sp_tmp_0 = SEQA_buf_0[((long )_i_sub_l5_0) - 0L][_i_l1];
        write_channel_altera_ch_SEQA_buf_0_0_p__merlinalterafuncend(ch_SEQA_buf_0_0_p[_i_sub_l5_0],SEQA_buf_0_sp_tmp_0);
      }
    }
  }
}

void msm_port_SEQB_buf_0_msm_node_0_0(char SEQB_buf_0[32][128],int _i_sub_l5)
{
  int __i_sub_l5;
  __i_sub_l5 = _i_sub_l5;
  for (int _i_l1 = 0; _i_l1 < 128; ++_i_l1) {
    int _i_sub_l5_0;
{
      
#pragma unroll
      for (_i_sub_l5_0 = ((int )0); _i_sub_l5_0 <= ((int )31); ++_i_sub_l5_0) {
        char SEQB_buf_0_sp_tmp_0;
        SEQB_buf_0_sp_tmp_0 = SEQB_buf_0[((long )_i_sub_l5_0) - 0L][_i_l1];
        write_channel_altera_ch_SEQB_buf_0_1_p__merlinalterafuncend(ch_SEQB_buf_0_1_p[_i_sub_l5_0],SEQB_buf_0_sp_tmp_0);
      }
    }
  }
}

void msm_port_alignedA_buf_0_msm_node_0_0(char alignedA_buf_0[32][256],int _i_sub_l5)
{
  int __i_sub_l5;
  __i_sub_l5 = _i_sub_l5;
  int _a_str_idx_1;
  for (_a_str_idx_1 = 0; _a_str_idx_1 < 128 + 128; _a_str_idx_1++) {
    int _i_sub_l5_0;
{
      
#pragma unroll
      for (_i_sub_l5_0 = ((int )0); _i_sub_l5_0 <= ((int )31); ++_i_sub_l5_0) {
        char alignedA_buf_0_sp_tmp_0;
        alignedA_buf_0_sp_tmp_0 = ((char )(read_channel_altera_ch_alignedA_buf_0_2_p__merlinalterafuncend(ch_alignedA_buf_0_2_p[_i_sub_l5_0])));
        alignedA_buf_0[((long )_i_sub_l5_0) - 0L][_a_str_idx_1] = alignedA_buf_0_sp_tmp_0;
      }
    }
  }
}

void msm_port_alignedB_buf_0_msm_node_0_0(char alignedB_buf_0[32][256],int _i_sub_l5)
{
  int __i_sub_l5;
  __i_sub_l5 = _i_sub_l5;
  int _b_str_idx_2;
  for (_b_str_idx_2 = 0; _b_str_idx_2 < 128 + 128; _b_str_idx_2++) {
    int _i_sub_l5_0;
{
      
#pragma unroll
      for (_i_sub_l5_0 = ((int )0); _i_sub_l5_0 <= ((int )31); ++_i_sub_l5_0) {
        char alignedB_buf_0_sp_tmp_0;
        alignedB_buf_0_sp_tmp_0 = ((char )(read_channel_altera_ch_alignedB_buf_0_3_p__merlinalterafuncend(ch_alignedB_buf_0_3_p[_i_sub_l5_0])));
        alignedB_buf_0[((long )_i_sub_l5_0) - 0L][_b_str_idx_2] = alignedB_buf_0_sp_tmp_0;
      }
    }
  }
}

void msm_port_i_sub_l5_msm_node_0_0(int _i_sub_l5)
{
  int _i_sub_l5_0;
{
    
#pragma unroll
    for (_i_sub_l5_0 = ((int )0); _i_sub_l5_0 <= ((int )31); ++_i_sub_l5_0) {
      int _i_sub_l5_sp_tmp_0;
      int __i_sub_l5;
      _i_sub_l5_sp_tmp_0 = _i_sub_l5_0;
      write_channel_altera_ch__i_sub_l5_4_p__merlinalterafuncend(ch__i_sub_l5_4_p[_i_sub_l5_0],_i_sub_l5_sp_tmp_0);
      __i_sub_l5 = _i_sub_l5_0;
      __i_sub_l5;
    }
  }
}

__kernel void needwun_kernel(__global char * restrict SEQA,__global char * restrict SEQB,__global char * restrict alignedA,__global char * restrict alignedB)
{
  int _memcpy_i3_0;
  int _memcpy_i2_0;
  int _memcpy_i1_0;
  int _memcpy_i0_0;
  
#pragma ACCEL interface variable=SEQA bus_bitwidth=256 stream_prefetch=off max_depth=131072 depth=131072
  
#pragma ACCEL interface variable=SEQB bus_bitwidth=256 stream_prefetch=off max_depth=131072 depth=131072
  
#pragma ACCEL interface variable=alignedA bus_bitwidth=256 stream_prefetch=off max_depth=262144 depth=262144
  
#pragma ACCEL interface variable=alignedB bus_bitwidth=256 stream_prefetch=off max_depth=262144 depth=262144
  int i;
//#pragma ACCEL pipeline // turn off cgpip because of bram usage
//#pragma unroll 16
  
#pragma max_concurrency 1
// Original pramga:  ACCEL PARALLEL FACTOR=32 
  
#pragma unroll 1
  for (i = 0; i < 32; i++) 
// Original: #pragma ACCEL parallel factor=32
// Original: #pragma ACCEL PARALLEL FACTOR=32
{
    char alignedB_buf_0[32][256];
    char alignedA_buf_0[32][256];
    char SEQB_buf_0[32][128];
    char SEQA_buf_0[32][128];
{
      
#pragma unroll
      for (int _i_sub_l3 = 0; _i_sub_l3 < 32; ++_i_sub_l3) {
        
#pragma unroll 32
        for (_memcpy_i0_0 = 0; _memcpy_i0_0 < 128; ++_memcpy_i0_0) {
          
#pragma HLS COALESCING_WIDTH=256
          SEQA_buf_0[((long )_i_sub_l3) - 0L][_memcpy_i0_0] = SEQA[i * 4096 + _i_sub_l3 * 128 + (0 * 128 + _memcpy_i0_0)];
        }
      }
    }
{
      
#pragma unroll
      for (int _i_sub_l4 = 0; _i_sub_l4 < 32; ++_i_sub_l4) {
        
#pragma unroll 32
        for (_memcpy_i1_0 = 0; _memcpy_i1_0 < 128; ++_memcpy_i1_0) {
          
#pragma HLS COALESCING_WIDTH=256
          SEQB_buf_0[((long )_i_sub_l4) - 0L][_memcpy_i1_0] = SEQB[i * 4096 + _i_sub_l4 * 128 + (0 * 128 + _memcpy_i1_0)];
        }
      }
    }
    msm_port_SEQA_buf_0_msm_node_0_0(SEQA_buf_0,0);
    msm_port_SEQB_buf_0_msm_node_0_0(SEQB_buf_0,0);
    msm_port_i_sub_l5_msm_node_0_0(0);
    
#pragma ACCEL mem_fence autokernel=msm_node_0 parent=needwun_kernel depth=32
    msm_port_alignedA_buf_0_msm_node_0_0(alignedA_buf_0,0);
    msm_port_alignedB_buf_0_msm_node_0_0(alignedB_buf_0,0);
{
      
#pragma unroll
      for (int _i_sub_l6 = 0; _i_sub_l6 < 32; ++_i_sub_l6) {
        
#pragma unroll 32
        for (_memcpy_i3_0 = 0; _memcpy_i3_0 < 256; ++_memcpy_i3_0) {
          
#pragma HLS COALESCING_WIDTH=256
          alignedB[i * 8192 + _i_sub_l6 * 256 + (0 * 256 + _memcpy_i3_0)] = alignedB_buf_0[((long )_i_sub_l6) - 0L][_memcpy_i3_0];
        }
      }
    }
{
      
#pragma unroll
      for (int _i_sub_l7 = 0; _i_sub_l7 < 32; ++_i_sub_l7) {
        
#pragma unroll 32
        for (_memcpy_i2_0 = 0; _memcpy_i2_0 < 256; ++_memcpy_i2_0) {
          
#pragma HLS COALESCING_WIDTH=256
          alignedA[i * 8192 + _i_sub_l7 * 256 + (0 * 256 + _memcpy_i2_0)] = alignedA_buf_0[((long )_i_sub_l7) - 0L][_memcpy_i2_0];
        }
      }
    }
//alignedB + (ALEN+BLEN) * i, M + ((ALEN+1)*(BLEN+1)) * i, ptr + ((ALEN+1)*(BLEN+1)) * i);
  }
}
