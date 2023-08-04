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
// Original: #pragma ACCEL array_partition type=channel variable=ch_profbuf_vec_data_3_0 dim=2 factor=16
// Original: #pragma ACCEL array_partition type=channel variable=ch_profbuf_vec_data_3_0 dim=1 factor=4
// Original: #pragma ACCEL array_partition type=channel variable=ch_rf_vec_2 dim=1 factor=4
// Original: #pragma ACCEL array_partition type=channel variable=ch_i_1 dim=1 factor=4
// Original: #pragma ACCEL array_partition type=channel variable=ch_i_sub_0 dim=1 factor=4
// Original: #pragma ACCEL array_partition type=channel variable=ch_results_4 dim=1 factor=4
// Original: #pragma ACCEL array_partition type=channel variable=ch_profbuf_vec_data_3 dim=2 factor=16
// Original: #pragma ACCEL array_partition type=channel variable=ch_profbuf_vec_data_3 dim=1 factor=4
// Original: #pragma ACCEL array_partition type=channel variable=ch_profbuf_size_vec_2 dim=1 factor=4
// Original: #pragma ACCEL array_partition type=channel variable=ch_rf_len_vec_1 dim=1 factor=4
// Original: #pragma ACCEL array_partition type=channel variable=ch_rf_vec_0 dim=1 factor=4
#include "merlin_stream.h" 
#include "merlin_stream.h" 
#include "merlin_stream.h" 
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy__pragma_ACCEL_STREAM_CHANNEL_C_();
#ifndef __defined_ch_profbuf_vec_data_3_0_p
#define __defined_ch_profbuf_vec_data_3_0_p
#pragma channel unsigned char ch_profbuf_vec_data_3_0_p[4][16] __attribute__((depth(128)));
typedef int __merlin_channel_typedef;
#include "channel_def.h"
__merlin_channel_typedef ch_profbuf_vec_data_3_0_p[4][16];
#endif
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy__pragma_ACCEL_STREAM_CHANNEL_C_();
#ifndef __defined_ch_rf_vec_2_p
#define __defined_ch_rf_vec_2_p
#pragma channel char ch_rf_vec_2_p[4] __attribute__((depth(1035)));
typedef int __merlin_channel_typedef;
#include "channel_def.h"
::__merlin_channel_typedef ch_rf_vec_2_p[4];
#endif
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy__pragma_ACCEL_STREAM_CHANNEL_C_();
#ifndef __defined_ch_i_1_p
#define __defined_ch_i_1_p
#pragma channel int ch_i_1_p[4] __attribute__((depth(32)));
typedef int __merlin_channel_typedef;
#include "channel_def.h"
::__merlin_channel_typedef ch_i_1_p[4];
#endif
static void __merlin_dummy__pragma_ACCEL_STREAM_CHANNEL_C_();
#ifndef __defined_ch_i_sub_0_p
#define __defined_ch_i_sub_0_p
#pragma channel int ch_i_sub_0_p[4] __attribute__((depth(32)));
typedef int __merlin_channel_typedef;
#include "channel_def.h"
::__merlin_channel_typedef ch_i_sub_0_p[4];
#endif
#include "merlin_stream.h" 
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy__pragma_ACCEL_STREAM_CHANNEL_C_();
#ifndef __defined_ch_results_4_p
#define __defined_ch_results_4_p
#pragma channel long ch_results_4_p[4] __attribute__((depth(32)));
typedef int __merlin_channel_typedef;
#include "channel_def.h"
::__merlin_channel_typedef ch_results_4_p[4];
#endif
static void __merlin_dummy__pragma_ACCEL_STREAM_CHANNEL_C_();
#ifndef __defined_ch_profbuf_vec_data_3_p
#define __defined_ch_profbuf_vec_data_3_p
#pragma channel unsigned char ch_profbuf_vec_data_3_p[4][16] __attribute__((depth(32)));
typedef int __merlin_channel_typedef;
#include "channel_def.h"
::__merlin_channel_typedef ch_profbuf_vec_data_3_p[4][16];
#endif
static void __merlin_dummy__pragma_ACCEL_STREAM_CHANNEL_C_();
#ifndef __defined_ch_profbuf_size_vec_2_p
#define __defined_ch_profbuf_size_vec_2_p
#pragma channel int ch_profbuf_size_vec_2_p[4] __attribute__((depth(32)));
typedef int __merlin_channel_typedef;
#include "channel_def.h"
::__merlin_channel_typedef ch_profbuf_size_vec_2_p[4];
#endif
static void __merlin_dummy__pragma_ACCEL_STREAM_CHANNEL_C_();
#ifndef __defined_ch_rf_len_vec_1_p
#define __defined_ch_rf_len_vec_1_p
#pragma channel int ch_rf_len_vec_1_p[4] __attribute__((depth(32)));
typedef int __merlin_channel_typedef;
#include "channel_def.h"
::__merlin_channel_typedef ch_rf_len_vec_1_p[4];
#endif
static void __merlin_dummy__pragma_ACCEL_STREAM_CHANNEL_C_();
#ifndef __defined_ch_rf_vec_0_p
#define __defined_ch_rf_vec_0_p
#pragma channel char ch_rf_vec_0_p[4] __attribute__((depth(32)));
typedef int __merlin_channel_typedef;
#include "channel_def.h"
::__merlin_channel_typedef ch_rf_vec_0_p[4];
#endif
#include "cmost.h"
#include <assert.h>
#include "merlin_type_define.h"
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_NWORDS_PER_REG();
static void __merlin_dummy_NBYTES_PER_REG();
static void __merlin_dummy_NBYTES_PER_WORD();

void merlin_assign_m128i_67(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

static void mm_setzero_si128(unsigned char (*ret_var_data)[16])
{
// #pragma ACCEL attribute register variable=x_data
  unsigned char __attribute__((register)) x_data[16];
  
#pragma unroll
  for (int i = 0; i < 16; i++) 
// Original: #pragma ACCEL PARALLEL COMPLETE
// Original: #pragma ACCEL PARALLEL COMPLETE
// Original: #pragma ACCEL PARALLEL COMPLETE 
{
    x_data[i] = ((unsigned char )0);
  }
  merlin_assign_m128i_67(&( *ret_var_data),x_data);
  return ;
}

void merlin_assign_m128i_68(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

static void mm_insert_epi16(unsigned char (*ret_var_data)[16],unsigned char a_data[16],const int i,const int imm8)
{
// #pragma ACCEL attribute register variable=dst_data
  unsigned char __attribute__((register)) dst_data[16];
  
#pragma unroll
  for (int j = 0; j < 16; j++) {
    dst_data[j] = a_data[j];
  }
  dst_data[imm8 * 2] = ((unsigned char )(i & 0xff));
  dst_data[1 + imm8 * 2] = ((unsigned char )(i >> 8 & 0xff));
  merlin_assign_m128i_68(&( *ret_var_data),dst_data);
  return ;
}

void merlin_assign_m128i_69(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

static void mm_shufflelo_epi16(unsigned char (*ret_var_data)[16],unsigned char a_data[16])
{
// #pragma ACCEL attribute register variable=dst_data
  unsigned char __attribute__((register)) dst_data[16];
// The logic for this function is actually a little bit annoying to implement.
// In bowtie2 alignNucleotides, it is only called with loc = 0.
// So, only the code for loc=0 is specified here.
  unsigned int x0 = (unsigned int )a_data[0];
  unsigned int x1 = (unsigned int )a_data[1];
// Standardize from: for(int i = 0;i < 8;i += 2) {...}
  
#pragma unroll
  for (int i = 0; i <= 3; i++) {
    dst_data[i * 2] = ((unsigned char )x0);
    dst_data[1 + i * 2] = ((unsigned char )x1);
  }
  
#pragma unroll
  for (int i = 8; i < 16; i++) {
    dst_data[i] = a_data[i];
  }
  merlin_assign_m128i_69(&( *ret_var_data),dst_data);
  return ;
}

void merlin_assign_m128i_70(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

static void mm_shuffle_epi32(unsigned char (*ret_var_data)[16],unsigned char a_data[16])
{
// #pragma ACCEL attribute register variable=dst_data
  unsigned char __attribute__((register)) dst_data[16];
// This is also a cut-down intrinsic similar to shufflelo_epi16
  unsigned char x0 = a_data[0];
  unsigned char x1 = a_data[1];
  unsigned char x2 = a_data[2];
  unsigned char x3 = a_data[3];
// Standardize from: for(int i = 0;i < 16;i += 4) {...}
  
#pragma unroll
  for (int i = 0; i <= 3; i++) {
    dst_data[i * 4] = x0;
    dst_data[1 + i * 4] = x1;
    dst_data[2 + i * 4] = x2;
    dst_data[3 + i * 4] = x3;
  }
  merlin_assign_m128i_70(&( *ret_var_data),dst_data);
  return ;
}

void merlin_assign_m128i_71(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

static void mm_cmpeq_epi16(unsigned char (*ret_var_data)[16],unsigned char a_data[16],unsigned char b_data[16])
{
// #pragma ACCEL attribute register variable=dst_data
  unsigned char __attribute__((register)) dst_data[16];
// Standardize from: for(int i = 0;i < 16;i += 2) {...}
  
#pragma unroll
  for (int i = 0; i <= 7; i++) {
    int _in_i = 0 + ((int )2L) * i;
    unsigned char a_lo = a_data[_in_i];
    unsigned char a_hi = a_data[_in_i + 1];
    unsigned char b_lo = b_data[_in_i];
    unsigned char b_hi = b_data[_in_i + 1];
    unsigned short a_i = (unsigned short )(((int )a_lo) | ((int )a_hi) << 8);
    unsigned short b_i = (unsigned short )(((int )b_lo) | ((int )b_hi) << 8);
    if (((int )a_i) == ((int )b_i)) {
      dst_data[i * 2] = ((unsigned char )0xff);
      dst_data[1 + i * 2] = ((unsigned char )0xff);
    }
     else {
      dst_data[i * 2] = ((unsigned char )0x00);
      dst_data[1 + i * 2] = ((unsigned char )0x00);
    }
  }
  merlin_assign_m128i_71(&( *ret_var_data),dst_data);
  return ;
}

void merlin_assign_m128i_74(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

static void mm_cmpeq_epi8(unsigned char (*ret_var_data)[16],unsigned char a_data[16],unsigned char b_data[16])
{
// #pragma ACCEL attribute register variable=dst_data
  unsigned char __attribute__((register)) dst_data[16];
  
#pragma unroll
  for (int i = 0; i < 16; i++) 
// Original: #pragma ACCEL PARALLEL COMPLETE
{
    if (((int )a_data[i]) == ((int )b_data[i])) {
      dst_data[i] = ((unsigned char )0xff);
    }
     else {
      dst_data[i] = ((unsigned char )0x00);
    }
  }
  merlin_assign_m128i_74(&( *ret_var_data),dst_data);
  return ;
}

void merlin_assign_m128i_72(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

static void mm_xor_si128(unsigned char (*ret_var_data)[16],unsigned char a_data[16],unsigned char b_data[16])
{
// #pragma ACCEL attribute register variable=dst_data
  unsigned char __attribute__((register)) dst_data[16];
  
#pragma unroll
  for (int i = 0; i < 16; i++) {
    dst_data[i] = ((unsigned char )(((int )a_data[i]) ^ ((int )b_data[i])));
  }
  merlin_assign_m128i_72(&( *ret_var_data),dst_data);
  return ;
}

void merlin_assign_m128i_73(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

static void mm_srli_si128(unsigned char (*ret_var_data)[16],unsigned char a_data[16])
{
// #pragma ACCEL attribute register variable=dst_data
  unsigned char __attribute__((register)) dst_data[16];
  int tmp = 15 & 0xff;
  if (tmp > 15) {
    tmp = 16;
  }
// fill in the bottom bits
  for (int i = 0; i < 16 - tmp; i++) {
    
#pragma HLS LOOP_TRIPCOUNT max=16
    dst_data[i] = a_data[i + tmp];
  }
// fill in the top bits
  for (int i = 16 - tmp; i < 16; i++) {
    
#pragma HLS LOOP_TRIPCOUNT max=16
    dst_data[i] = ((unsigned char )0);
  }
  merlin_assign_m128i_73(&( *ret_var_data),dst_data);
  return ;
}

static void mm_store_si128(unsigned char (*dst_data)[16],unsigned char a_data[16])
{
  
#pragma unroll
  for (int i = 0; i < 16; i++) {
    ( *dst_data)[i] = a_data[i];
  }
}

void merlin_assign_m128i_75(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

static void mm_load_si128(unsigned char (*ret_var_data)[16],unsigned char (*src_data)[16])
{
// #pragma ACCEL attribute register variable=dst_data
  unsigned char __attribute__((register)) dst_data[16];
  
#pragma unroll
  for (int i = 0; i < 16; i++) 
// Original: #pragma ACCEL PARALLEL COMPLETE
// Original: #pragma ACCEL PARALLEL COMPLETE 
// Original: #pragma ACCEL PARALLEL COMPLETE 
// Original: #pragma ACCEL PARALLEL COMPLETE 
{
    dst_data[i] = ( *src_data)[i];
  }
  merlin_assign_m128i_75(&( *ret_var_data),dst_data);
  return ;
}

void merlin_assign_m128i_76(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

static void mm_slli_si128(unsigned char (*ret_var_data)[16],unsigned char a_data[16])
{
// #pragma ACCEL attribute register variable=dst_data
  unsigned char __attribute__((register)) dst_data[16];
  int tmp = 1 & 0xff;
  if (tmp > 15) {
    tmp = 16;
  }
// fill in the top bits
  for (int i = 0; i < 16 - tmp; i++) 
// Original: #pragma ACCEL PARALLEL COMPLETE
{
    
#pragma HLS LOOP_TRIPCOUNT max=16
    dst_data[i + tmp] = a_data[i];
  }
// fill in the bottom bits
  for (int i = 0; i < tmp; i++) 
// Original: #pragma ACCEL PARALLEL COMPLETE
{
    
#pragma HLS LOOP_TRIPCOUNT max=16
    dst_data[i] = ((unsigned char )0);
  }
  merlin_assign_m128i_76(&( *ret_var_data),dst_data);
  return ;
}

void merlin_assign_m128i_77(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

static void mm_or_si128(unsigned char (*ret_var_data)[16],unsigned char a_data[16],unsigned char b_data[16])
{
// #pragma ACCEL attribute register variable=dst_data
  unsigned char __attribute__((register)) dst_data[16];
  
#pragma unroll
  for (int i = 0; i < 16; i++) 
// Original: #pragma ACCEL PARALLEL COMPLETE
{
    dst_data[i] = ((unsigned char )(((int )a_data[i]) | ((int )b_data[i])));
  }
  merlin_assign_m128i_77(&( *ret_var_data),dst_data);
  return ;
}

void merlin_assign_m128i_78(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

static void mm_subs_epu8(unsigned char (*ret_var_data)[16],unsigned char a_data[16],unsigned char b_data[16])
{
// #pragma ACCEL attribute register variable=dst_data
  unsigned char __attribute__((register)) dst_data[16];
  
#pragma unroll
  for (int i = 0; i < 16; i++) 
// Original: #pragma ACCEL PARALLEL COMPLETE
// Original: #pragma ACCEL PARALLEL COMPLETE 
{
    unsigned char a_i = a_data[i];
    unsigned char b_i = b_data[i];
    unsigned char result = (unsigned char )(((int )a_i) - ((int )b_i));
    if (((int )result) > ((int )a_i)) {
      result = ((unsigned char )0x00);
    }
    dst_data[i] = result;
  }
  merlin_assign_m128i_78(&( *ret_var_data),dst_data);
  return ;
}

void merlin_assign_m128i_79(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

static void mm_max_epu8(unsigned char (*ret_var_data)[16],unsigned char a_data[16],unsigned char b_data[16])
{
// #pragma ACCEL attribute register variable=dst_data
  unsigned char __attribute__((register)) dst_data[16];
  
#pragma unroll
  for (int i = 0; i < 16; i++) 
// Original: #pragma ACCEL PARALLEL COMPLETE
// Original: #pragma ACCEL PARALLEL COMPLETE 
{
    unsigned char a_i = a_data[i];
    unsigned char b_i = b_data[i];
    if (((int )a_i) > ((int )b_i)) {
      dst_data[i] = a_i;
    }
     else {
      dst_data[i] = b_i;
    }
  }
  merlin_assign_m128i_79(&( *ret_var_data),dst_data);
  return ;
}

static unsigned int mm_movemask_epi8(unsigned char a_data[16])
{
// #pragma ACCEL attribute register variable=mask
  unsigned char __attribute__((register)) mask[16];
  
#pragma unroll
  for (int i = 0; i < 16; i++) 
// Original: #pragma ACCEL PARALLEL COMPLETE
{
    mask[i] = ((unsigned char )(((int )a_data[i]) >> 7 & 0x1));
  }
  unsigned int mask_new = (unsigned int )0x0000;
  
#pragma unroll
  for (int i = 0; i < 16; i++) 
// Original: #pragma ACCEL PARALLEL COMPLETE
{
    mask_new |= ((unsigned int )(((int )mask[i]) << i));
  }
  return mask_new;
}

void merlin_assign_m128i_0(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_1(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_2(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_3(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_4(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_5(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_6(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_7(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_8(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_9(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_10(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_11(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_12(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_13(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_14(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_15(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_16(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_17(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_18(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_19(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_20(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_21(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_22(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_23(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_24(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_25(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_26(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_27(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_28(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_29(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_30(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_31(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    lhs_data[0][i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_32(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    lhs_data[0][i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_33(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_34(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_35(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_36(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_37(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_38(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_39(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_40(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_41(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_42(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_43(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_44(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_45(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_46(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_47(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_48(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_49(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_50(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_51(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_52(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_53(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_54(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_55(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_56(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_57(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_58(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_59(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_60(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_61(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_62(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_63(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_64(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_65(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

void merlin_assign_m128i_66(unsigned char (*lhs_data)[16],unsigned char rhs_data[16])
{
  int i_0;
  
#pragma unroll
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    ( *lhs_data)[i_0] = rhs_data[i_0];
  }
}

static long align_kernel(unsigned char profbuf_data[128][16],int i_sub)
{
// #pragma ACCEL attribute register variable=ret_var_24_data
  unsigned char __attribute__((register)) ret_var_24_data[16];
// #pragma ACCEL attribute register variable=ret_var_23_data
  unsigned char __attribute__((register)) ret_var_23_data[16];
// #pragma ACCEL attribute register variable=ret_var_22_data
  unsigned char __attribute__((register)) ret_var_22_data[16];
  unsigned char ret_var_21_data[16];
  unsigned char ret_var_20_data[16];
  unsigned char ret_var_19_data[16];
  unsigned char ret_var_18_data[16];
  unsigned char ret_var_17_data[16];
  unsigned char ret_var_16_data[16];
  unsigned char ret_var_15_data[16];
  unsigned char ret_var_14_data[16];
  unsigned char ret_var_13_data[16];
  unsigned char ret_var_12_data[16];
  unsigned char ret_var_11_data[16];
  unsigned char ret_var_10_data[16];
  unsigned char ret_var_9_data[16];
  unsigned char ret_var_7_data[16];
  unsigned char ret_var_6_data[16];
  unsigned char ret_var_5_data[16];
  unsigned char ret_var_4_data[16];
  unsigned char ret_var_3_data[16];
  unsigned char ret_var_2_data[16];
  unsigned char ret_var_1_data[16];
  unsigned char ret_var_0_data[16];
  unsigned char ret_var_data[16];
// Skip this step; the profile buffer is loaded as a kernel argument
//buildQueryProfileEnd2EndSseU8(fw_);
// Constants
  const int dpRows = 36;
  const int refGapOpen = 8;
  const int refGapExtend = 3;
  const int readGapOpen = 8;
  const int readGapExtend = 3;
  const int rfi_ = 0;
  const int rff_ = 1035;
  const int lastWord_ = 11;
  const int lastIter_ = 2;
  int firsts5[32] = {(- 1), (0), (1), (0), (2), (0), (1), (0), (3), (0), (1), (0), (2), (0), (1), (0), (4), (0), (1), (0), (2), (0), (1), (0), (3), (0), (1), (0), (2), (0), (1), (0)};
  const unsigned long iter = (((unsigned long )dpRows) + (NWORDS_PER_REG - ((unsigned long )1))) / NWORDS_PER_REG;
// iter = segLen
  int dup;
// Many thanks to Michael Farrar for releasing his striped Smith-Waterman
// implementation:
//
//  http://sites.google.com/site/farrarmichael/smith-waterman
//
// Much of the implmentation below is adapted from Michael's code.
// Set all elts to reference gap open penalty
  mm_setzero_si128(&ret_var_data);
  unsigned char rfgapo_data[16];
  merlin_assign_m128i_0(&rfgapo_data,ret_var_data);
  mm_setzero_si128(&ret_var_0_data);
  unsigned char rfgape_data[16];
  merlin_assign_m128i_1(&rfgape_data,ret_var_0_data);
  mm_setzero_si128(&ret_var_1_data);
  unsigned char rdgapo_data[16];
  merlin_assign_m128i_2(&rdgapo_data,ret_var_1_data);
  mm_setzero_si128(&ret_var_2_data);
  unsigned char rdgape_data[16];
  merlin_assign_m128i_3(&rdgape_data,ret_var_2_data);
  mm_setzero_si128(&ret_var_3_data);
  unsigned char vlo_data[16];
  merlin_assign_m128i_4(&vlo_data,ret_var_3_data);
  mm_setzero_si128(&ret_var_4_data);
  unsigned char vhi_data[16];
  merlin_assign_m128i_5(&vhi_data,ret_var_4_data);
//	m128i ve       = mm_setzero_si128();
//	m128i vf       = mm_setzero_si128();
//	m128i vh       = mm_setzero_si128();
  mm_setzero_si128(&ret_var_5_data);
  unsigned char vtmp_data[16];
  merlin_assign_m128i_6(&vtmp_data,ret_var_5_data);
  mm_setzero_si128(&ret_var_6_data);
  unsigned char vzero_data[16];
  merlin_assign_m128i_7(&vzero_data,ret_var_6_data);
  mm_setzero_si128(&ret_var_7_data);
// #pragma ACCEL attribute register variable=vhilsw_data
  unsigned char __attribute__((register)) vhilsw_data[16];
  merlin_assign_m128i_8(&vhilsw_data,ret_var_7_data);
  dup = refGapOpen << 8 | refGapOpen & 0x00ff;
  mm_insert_epi16(&ret_var_9_data,rfgapo_data,dup,0);
  merlin_assign_m128i_9(&rfgapo_data,ret_var_9_data);
  mm_shufflelo_epi16(&ret_var_13_data,rfgapo_data);
  merlin_assign_m128i_10(&rfgapo_data,ret_var_13_data);
  mm_shuffle_epi32(&ret_var_17_data,rfgapo_data);
  merlin_assign_m128i_11(&rfgapo_data,ret_var_17_data);
// Set all elts to reference gap extension penalty
  dup = refGapExtend << 8 | refGapExtend & 0x00ff;
  mm_insert_epi16(&ret_var_10_data,rfgape_data,dup,0);
  merlin_assign_m128i_12(&rfgape_data,ret_var_10_data);
  mm_shufflelo_epi16(&ret_var_14_data,rfgape_data);
  merlin_assign_m128i_13(&rfgape_data,ret_var_14_data);
  mm_shuffle_epi32(&ret_var_18_data,rfgape_data);
  merlin_assign_m128i_14(&rfgape_data,ret_var_18_data);
// Set all elts to read gap open penalty
  dup = readGapOpen << 8 | readGapOpen & 0x00ff;
  mm_insert_epi16(&ret_var_11_data,rdgapo_data,dup,0);
  merlin_assign_m128i_15(&rdgapo_data,ret_var_11_data);
  mm_shufflelo_epi16(&ret_var_15_data,rdgapo_data);
  merlin_assign_m128i_16(&rdgapo_data,ret_var_15_data);
  mm_shuffle_epi32(&ret_var_19_data,rdgapo_data);
  merlin_assign_m128i_17(&rdgapo_data,ret_var_19_data);
// Set all elts to read gap extension penalty
  dup = readGapExtend << 8 | readGapExtend & 0x00ff;
  mm_insert_epi16(&ret_var_12_data,rdgape_data,dup,0);
  merlin_assign_m128i_18(&rdgape_data,ret_var_12_data);
  mm_shufflelo_epi16(&ret_var_16_data,rdgape_data);
  merlin_assign_m128i_19(&rdgape_data,ret_var_16_data);
  mm_shuffle_epi32(&ret_var_20_data,rdgape_data);
  merlin_assign_m128i_20(&rdgape_data,ret_var_20_data);
// all elts = 0xffff
  mm_cmpeq_epi16(&ret_var_22_data,vhi_data,vhi_data);
  merlin_assign_m128i_21(&vhi_data,ret_var_22_data);
// all elts = 0
  mm_xor_si128(&ret_var_23_data,vlo_data,vlo_data);
  merlin_assign_m128i_22(&vlo_data,ret_var_23_data);
// vhilsw: topmost (least sig) word set to 0x7fff, all other words=0
  mm_shuffle_epi32(&ret_var_21_data,vhi_data);
  merlin_assign_m128i_23(&vhilsw_data,ret_var_21_data);
  mm_srli_si128(&ret_var_24_data,vhilsw_data);
  merlin_assign_m128i_24(&vhilsw_data,ret_var_24_data);
//m128i vScore[12432];
  
#pragma ACCEL register variable=vScore_data
  unsigned char vScore_data[32][16];
//assert((long) &vScore[0] & 15 == 0);
//m128i * vScore = NULL;
//vScore = (m128i *) alignedMalloc(12432 * sizeof(m128i));
//d.mat_.init(dpRows, rff_ - rfi_, NWORDS_PER_REG);
//const size_t colstride = d.mat_.colstride();
  const unsigned long colstride = (unsigned long )12;
//const size_t rowstride = d.mat_.rowstride();
// Initialize the H and E vectors in the first matrix column
//m128i *pvHTmp = d.mat_.tmpvec(0, 0);
//m128i *pvETmp = d.mat_.evec(0, 0);
  int pvHTmp = (int )(((unsigned long )0) + (((unsigned long )(0 * 4)) + ((unsigned long )0) * colstride + ((unsigned long )3)));
  int pvETmp = (int )(((unsigned long )0) + (((unsigned long )(0 * 4)) + ((unsigned long )0) * colstride + ((unsigned long )0)));
// Maximum score in final row
  unsigned char found = (unsigned char )false;
//TCScore lrmax = MIN_U8;
  unsigned char lrmax = (unsigned char )0;
// 3 constant
  for (unsigned long i = (unsigned long )0; i < iter; i++) {
    mm_store_si128(&vScore_data[pvETmp],vlo_data);
// start high in end-to-end mode
    mm_store_si128(&vScore_data[pvHTmp],vlo_data);
    pvETmp += 4;
    pvHTmp += 4;
  }
  int pvHLoad = (int )(((unsigned long )0) + (((unsigned long )(0 * 4)) + ((unsigned long )0) * colstride + ((unsigned long )3)));
  unsigned long nfixup = (unsigned long )0;
//  printf("%d\n", pvHLoad);
  main_inner_loop:
{
// 1035
    for (unsigned long ii = (unsigned long )rfi_; ii < ((unsigned long )rff_); ii++) 
// Original: #pragma ACCEL pipeline flatten
// Original: #pragma ACCEL PIPELINE
{
      char rf_vec_sn_tmp_0;
      unsigned char ret_var_60_data[16];
      unsigned char ret_var_50_data[16];
      unsigned char ret_var_49_data[16];
// #pragma ACCEL attribute register variable=ret_var_41_data
      unsigned char __attribute__((register)) ret_var_41_data[16];
      unsigned char ret_var_40_data[16];
      unsigned char ret_var_39_data[16];
      unsigned char ret_var_35_data[16];
      unsigned char ret_var_34_data[16];
      unsigned char ret_var_33_data[16];
      unsigned char ret_var_27_data[16];
      unsigned char ret_var_25_data[16];
      unsigned char ret_var_8_data[16];
//pvHStore_init + colstride * ii;
      int pvHStore = (int )(((unsigned long )2) + ((unsigned long )12) * ii);
//pvFStore_init + colstride * ii;
// new pvHLoad = pvHStore
      if (ii > ((unsigned long )0)) {
        pvHLoad = pvHStore - 12;
      }
       else {
        pvHLoad = 3;
      }
//   printf("%d %d\n", pvHStore, pvFStore);
// Fetch the appropriate query profile.  Note that elements of rf_ must
// be numbers, not masks.
      rf_vec_sn_tmp_0 = ((char )(read_channel_altera_ch_rf_vec_0_p__merlinalterafuncend(ch_rf_vec_0_p[i_sub])));
      const int refc = (int )rf_vec_sn_tmp_0;
      unsigned long off = ((unsigned long )firsts5[refc]) * iter * ((unsigned long )2);
//assert(off >= 0 && off < profbuf_size);
//if (off < 0) off = 0;
//if (off >= profbuf_size) off = profbuf_size - 2;
// Set all cells to low value
//mm_xor_si128(vf, vf);
      mm_setzero_si128(&ret_var_8_data);
      unsigned char vf_data[16];
      merlin_assign_m128i_25(&vf_data,ret_var_8_data);
// Load H vector from the final row of the previous column
//m128i vh = mm_load_si128(&vScore[pvHLoad + colstride - ROWSTRIDE]);
      mm_load_si128(&ret_var_27_data,&vScore_data[(((unsigned long )pvHLoad) + colstride - ((unsigned long )4)) % ((unsigned long )32)]);
      unsigned char vh_data[16];
      merlin_assign_m128i_26(&vh_data,ret_var_27_data);
// Shift 2 bytes down so that topmost (least sig) cell gets 0
      mm_slli_si128(&ret_var_39_data,vh_data);
      merlin_assign_m128i_27(&vh_data,ret_var_39_data);
// Fill topmost (least sig) cell with high value
      mm_or_si128(&ret_var_41_data,vh_data,vhilsw_data);
      merlin_assign_m128i_28(&vh_data,ret_var_41_data);
// For each character in the reference text:
      unsigned long j;
      unsigned char ve_t_data[3][16];
      
#pragma unroll
      for (j = ((unsigned long )0); j < ((unsigned long )3); j++) 
// Original: #pragma ACCEL PARALLEL COMPLETE
{
        merlin_assign_m128i_29(&ve_t_data[j],vScore_data[((unsigned long )4) * j]);
      }
      unsigned char vhl_t_data[3][16];
      
#pragma unroll
      for (j = ((unsigned long )0); j < ((unsigned long )3); j++) 
// Original: #pragma ACCEL PARALLEL COMPLETE
{
        unsigned char ret_var_28_data[16];
        mm_load_si128(&ret_var_28_data,&vScore_data[(((unsigned long )pvHLoad) + j * ((unsigned long )4)) % ((unsigned long )32)]);
        merlin_assign_m128i_30(&vhl_t_data[j],ret_var_28_data);
      }
      unsigned char vh_n_data[3][16];
      unsigned char vf_n_data[3][16];
      unsigned char ve_n_data[3][16];
      unsigned char pr_t_data[6][16];
      
#pragma unroll
      for (j = ((unsigned long )0); j < ((unsigned long )3); j++) 
// Original: #pragma ACCEL PARALLEL COMPLETE
{
        unsigned char ret_var_30_data[16];
        unsigned char ret_var_29_data[16];
        mm_load_si128(&ret_var_29_data,&profbuf_data[j * ((unsigned long )2) + off + ((unsigned long )0)]);
        merlin_assign_m128i_31(&pr_t_data[j * ((unsigned long )2)],ret_var_29_data);
        mm_load_si128(&ret_var_30_data,&profbuf_data[j * ((unsigned long )2) + off + ((unsigned long )1)]);
        merlin_assign_m128i_32(&pr_t_data[((unsigned long )1) + j * ((unsigned long )2)],ret_var_30_data);
      }
// printf("pvHLoad %d, pvHStore=%d \n", pvHLoad, pvHStore);
// 3 // scan rows  // constant
      
#pragma unroll
      for (j = ((unsigned long )0); j < iter; j++) 
// Original: #pragma ACCEL PARALLEL COMPLETE
{
        unsigned char ret_var_59_data[16];
        unsigned char ret_var_58_data[16];
        unsigned char ret_var_57_data[16];
        unsigned char ret_var_56_data[16];
        unsigned char ret_var_48_data[16];
        unsigned char ret_var_47_data[16];
        unsigned char ret_var_46_data[16];
        unsigned char ret_var_45_data[16];
        unsigned char ret_var_44_data[16];
        unsigned char ret_var_43_data[16];
        unsigned char ret_var_42_data[16];
        unsigned char ret_var_32_data[16];
        unsigned char ret_var_31_data[16];
// Load cells from E, calculated previously
//m128i ve = mm_load_si128(&vScore[pvELoad + ROWSTRIDE * j]);
        mm_load_si128(&ret_var_31_data,&ve_t_data[j]);
        unsigned char ve_data[16];
        merlin_assign_m128i_33(&ve_data,ret_var_31_data);
//      printf("%d\n", pvELoad);
//      assert(pvELoad == 0 );
//pvELoad += ROWSTRIDE;
// Store cells in F, calculated previously
//vf = mm_subs_epu8(vf, profbuf[pvProfbuf+1]); // veto some ref gap extensions
// veto some ref gap extensions
        mm_subs_epu8(&ret_var_42_data,vf_data,pr_t_data[((unsigned long )1) + j * ((unsigned long )2)]);
        merlin_assign_m128i_34(&vf_data,ret_var_42_data);
//mm_store_si128(&vScore[pvFStore + j * ROWSTRIDE ], vf);
        mm_store_si128(&vf_n_data[j],vf_data);
// Factor in query profile (matches and mismatches)
//vh = mm_subs_epu8(vh, profbuf[pvProfbuf+0 + 2*j]);
        mm_subs_epu8(&ret_var_43_data,vh_data,pr_t_data[j * ((unsigned long )2)]);
        merlin_assign_m128i_35(&vh_data,ret_var_43_data);
// Update H, factoring in E and F
        mm_max_epu8(&ret_var_56_data,vh_data,ve_data);
        merlin_assign_m128i_36(&vh_data,ret_var_56_data);
        mm_max_epu8(&ret_var_57_data,vh_data,vf_data);
        merlin_assign_m128i_37(&vh_data,ret_var_57_data);
// Save the new vH values
//mm_store_si128(&vScore[pvHStore + j * ROWSTRIDE], vh);
        mm_store_si128(&vh_n_data[j],vh_data);
//pvHStore += ROWSTRIDE;
// Update vE value
        merlin_assign_m128i_38(&vtmp_data,vh_data);
        mm_subs_epu8(&ret_var_44_data,vh_data,rdgapo_data);
        merlin_assign_m128i_39(&vh_data,ret_var_44_data);
//vh = mm_subs_epu8(vh, profbuf[pvProfbuf+1 + 2*j]); // veto some read gap opens
// veto some read gap opens
        mm_subs_epu8(&ret_var_45_data,vh_data,pr_t_data[((unsigned long )1) + j * ((unsigned long )2)]);
        merlin_assign_m128i_40(&vh_data,ret_var_45_data);
        mm_subs_epu8(&ret_var_46_data,ve_data,rdgape_data);
        merlin_assign_m128i_41(&ve_data,ret_var_46_data);
        mm_max_epu8(&ret_var_58_data,ve_data,vh_data);
        merlin_assign_m128i_42(&ve_data,ret_var_58_data);
// Load the next h value
        mm_load_si128(&ret_var_32_data,&vhl_t_data[j]);
        merlin_assign_m128i_43(&vh_data,ret_var_32_data);
//pvHLoad += ROWSTRIDE;
// Save E values
//mm_store_si128(&vScore[pvEStore], ve);
        mm_store_si128(&ve_n_data[j],ve_data);
// Update vf value
        mm_subs_epu8(&ret_var_47_data,vtmp_data,rfgapo_data);
        merlin_assign_m128i_44(&vtmp_data,ret_var_47_data);
        mm_subs_epu8(&ret_var_48_data,vf_data,rfgape_data);
        merlin_assign_m128i_45(&vf_data,ret_var_48_data);
        mm_max_epu8(&ret_var_59_data,vf_data,vtmp_data);
        merlin_assign_m128i_46(&vf_data,ret_var_59_data);
//pvProfbuf += 2; // move on to next query profile / gap veto
      }
//		pvFStore += ROWSTRIDE * 3;
//	pvFStore -= colstride; // reset to start of column
//vtmp = mm_load_si128(&vScore[pvFStore]);
//vh = mm_load_si128(&vScore[pvHStore]);
//vtmp = mm_load_si128(&vScore[1 + 12 * ii]);
//vh = mm_load_si128(&vScore[2 + 12*ii]);
      mm_load_si128(&ret_var_33_data,&vf_n_data[0]);
      merlin_assign_m128i_47(&vtmp_data,ret_var_33_data);
      mm_load_si128(&ret_var_34_data,&vh_n_data[0]);
      merlin_assign_m128i_48(&vh_data,ret_var_34_data);
//pvEStore -= colstride; // reset to start of column
//m128i ve = mm_load_si128(&vScore[pvEStore]);
      mm_load_si128(&ret_var_35_data,&ve_n_data[0]);
      unsigned char ve_data_0[16];
      merlin_assign_m128i_49(&ve_data_0,ret_var_35_data);
//       if (off+1 >= profbuf_size) off = profbuf_size - 2;
//printf("%d, %d\n", off, profbuf_size);
// vf from last row gets shifted down by one to overlay the first row
// rfgape has already been subtracted from it.
      mm_slli_si128(&ret_var_40_data,vf_data);
      merlin_assign_m128i_50(&vf_data,ret_var_40_data);
//vf = mm_subs_epu8(vf, profbuf[pvProfbuf]); // veto some ref gap extensions
// veto some ref gap extensions
      mm_subs_epu8(&ret_var_49_data,vf_data,pr_t_data[1]);
      merlin_assign_m128i_51(&vf_data,ret_var_49_data);
      mm_max_epu8(&ret_var_60_data,vtmp_data,vf_data);
      merlin_assign_m128i_52(&vf_data,ret_var_60_data);
      mm_subs_epu8(&ret_var_50_data,vf_data,vtmp_data);
      merlin_assign_m128i_53(&vtmp_data,ret_var_50_data);
      mm_cmpeq_epi8(&ret_var_25_data,vtmp_data,vzero_data);
      merlin_assign_m128i_54(&vtmp_data,ret_var_25_data);
      int cmp = (int )(mm_movemask_epi8(vtmp_data));
// If any element of vtmp is greater than H - gap-open...
      j = ((unsigned long )0);
      int tt = 0;
//      static int max_tt = 0;
//while(cmp != 0xffff) 
      if (cmp != 0xffff) 
//for (int tt = 0; tt < 20; tt++) 
{
        unsigned char ret_var_63_data[16];
        unsigned char ret_var_62_data[16];
        unsigned char ret_var_61_data[16];
        unsigned char ret_var_55_data[16];
        unsigned char ret_var_54_data[16];
        unsigned char ret_var_53_data[16];
        unsigned char ret_var_52_data[16];
        unsigned char ret_var_51_data[16];
        unsigned char ret_var_26_data[16];
//  if (cmp == 0xffff) break;
        
#pragma ACCEL loop_tripcount max=20
// Store this vf
//mm_store_si128(&vScore[pvFStore1], vf); // 1 + 12 * ii
// 1 + 12 * ii
        mm_store_si128(&vf_n_data[j],vf_data);
//			pvFStore += ROWSTRIDE;
//pvFStore1 += ROWSTRIDE;
// Update vh w/r/t new vf
        mm_max_epu8(&ret_var_61_data,vh_data,vf_data);
        merlin_assign_m128i_55(&vh_data,ret_var_61_data);
// Save vH values
//mm_store_si128(&vScore[pvHStore1], vh);
        mm_store_si128(&vh_n_data[j],vh_data);
// Update E in case it can be improved using our new vh
        mm_subs_epu8(&ret_var_51_data,vh_data,rdgapo_data);
        merlin_assign_m128i_56(&vh_data,ret_var_51_data);
//vh = mm_subs_epu8(vh, profbuf[pvProfbuf]); // veto some read gap opens
// veto some read gap opens
        mm_subs_epu8(&ret_var_52_data,vh_data,pr_t_data[1]);
        merlin_assign_m128i_57(&vh_data,ret_var_52_data);
        mm_max_epu8(&ret_var_62_data,ve_data_0,vh_data);
        merlin_assign_m128i_58(&ve_data_0,ret_var_62_data);
//mm_store_si128(&vScore[pvEStore1], ve);
        mm_store_si128(&ve_n_data[j],ve_data_0);
//pvProfbuf += 2;
//if(++j == iter) {
//		//		pvFStore -= colstride;
//  //pvFStore1 += ROWSTRIDE;
//	//pvFStore1 -= 8;
//	vtmp = mm_load_si128(&vScore[pvFStore1-8]);   // load next vf ASAP
//	//pvHStore1 -= 8;
//	vh = mm_load_si128(&vScore[pvHStore1-8]);     // load next vh ASAP
//	//pvEStore1 -= 8;
//	ve = mm_load_si128(&vScore[pvEStore1-8]);     // load next ve ASAP
//          assert(off+1 <= profbuf_size);
//	//pvProfbuf = off + 1;
//	j = 0;
//	vf = mm_slli_si128(vf, NBYTES_PER_WORD);
//  pvFStore1 -= 12;;
//  pvHStore1 -= 12;
//  pvEStore1 -= 12;
//} else 
{
          unsigned char ret_var_38_data[16];
          unsigned char ret_var_37_data[16];
          unsigned char ret_var_36_data[16];
//vtmp = mm_load_si128(&vScore[pvFStore1]);     // load next vf ASAP
//vh   = mm_load_si128(&vScore[pvHStore1]);     // load next vh ASAP
//ve   = mm_load_si128(&vScore[pvEStore1]);     // load next vh ASAP
// load next vf ASAP
          mm_load_si128(&ret_var_36_data,&vf_n_data[j]);
          merlin_assign_m128i_59(&vtmp_data,ret_var_36_data);
// load next vh ASAP
          mm_load_si128(&ret_var_37_data,&vh_n_data[j]);
          merlin_assign_m128i_60(&vh_data,ret_var_37_data);
// load next vh ASAP
          mm_load_si128(&ret_var_38_data,&ve_n_data[j]);
          merlin_assign_m128i_61(&ve_data_0,ret_var_38_data);
        }
        ++j;
        j = j % ((unsigned long )3);
//pvFStore1 += 4;;
//pvHStore1 += 4;
//pvEStore1 += 4;
// Update F with another gap extension
        mm_subs_epu8(&ret_var_53_data,vf_data,rfgape_data);
        merlin_assign_m128i_62(&vf_data,ret_var_53_data);
//vf = mm_subs_epu8(vf, profbuf[pvProfbuf]); // veto some ref gap extensions
// veto some ref gap extensions
        mm_subs_epu8(&ret_var_54_data,vf_data,pr_t_data[1]);
        merlin_assign_m128i_63(&vf_data,ret_var_54_data);
        mm_max_epu8(&ret_var_63_data,vtmp_data,vf_data);
        merlin_assign_m128i_64(&vf_data,ret_var_63_data);
        mm_subs_epu8(&ret_var_55_data,vf_data,vtmp_data);
        merlin_assign_m128i_65(&vtmp_data,ret_var_55_data);
        mm_cmpeq_epi8(&ret_var_26_data,vtmp_data,vzero_data);
        merlin_assign_m128i_66(&vtmp_data,ret_var_26_data);
// exit if the [7] bit are all one
        cmp = ((int )(mm_movemask_epi8(vtmp_data)));
        nfixup++;
        tt++;
//      if (tt > max_tt) max_tt = tt;
      }
      
#pragma unroll
      for (int j = 0; j < 3; j++) 
// Original: #pragma ACCEL PARALLEL COMPLETE
{
        mm_store_si128(&vScore_data[(((unsigned long )1) + ((unsigned long )12) * ii + ((unsigned long )(4 * j))) % ((unsigned long )32)],vf_n_data[j]);
        mm_store_si128(&vScore_data[(((unsigned long )2) + ((unsigned long )12) * ii + ((unsigned long )(4 * j))) % ((unsigned long )32)],vh_n_data[j]);
        mm_store_si128(&vScore_data[(24 + 4 * j) % 32],ve_n_data[j]);
      }
//   printf("idx=%d ii=%d, max_tt=%d\n", idx, ii, max_tt);
      int vtmp = (int )(((unsigned long )0) + (((unsigned long )(lastIter_ * 4)) + (ii - ((unsigned long )rfi_)) * colstride + ((unsigned long )2)));
// Note: we may not want to extract from the final row
//TCScore lr = ((TCScore*)(vtmp))[lastWord_];
//TCScore lr = vtmp->data[lastWord_];
//TODO hack!!!
      unsigned char lr = vScore_data[vtmp % 32][lastWord_];
      found = ((unsigned char )true);
      if (((int )lr) > ((int )lrmax)) {
        lrmax = lr;
      }
// pvELoad and pvHLoad are already where they need to be
// Adjust the load and store vectors here.  
//pvHStore = pvHStore + colstride;
//pvEStore = pvELoad + colstride + 3 * ROWSTRIDE;
//pvFStore = pvFStore + ROWSTRIDE * 3;
    }
  }
//// Update metrics
//if(!debug) {
//size_t ninner = (rff_ - rfi_) * iter;
//met.col   += (rff_ - rfi_);             // DP columns
//met.cell  += (ninner * NWORDS_PER_REG); // DP cells
//met.inner += ninner;                    // DP inner loop iters
//met.fixup += nfixup;                    // DP fixup loop iters
//}
//flag = 0;
// Did we find a solution?
  long score = (long )0x8000000000000000;
  if (!((bool )found)) {
//flag = -1; // no
//if(!debug) met.dpfail++;
    score = ((long )0x8000000000000000);
  }
   else {
    score = ((long )(((int )lrmax) - 0xff));
  }
// Could we have saturated?
  if (((int )lrmax) == 0) {
//flag = -2; // yes
//if(!debug) met.dpsat++;
    score = ((long )0x8000000000000000);
  }
// Return largest score
//if(!debug) met.dpsucc++;
//free(vScore);
  return score;
}
static void __merlin_dummy_kernel_pragma();
// Original: #pragma ACCEL kernel
void __merlin_dummy_msm_node_1();
 __attribute__ (( autorun))
 __attribute__ (( num_compute_units(4)))
 __attribute__ (( max_global_work_dim(0)))

__kernel void msm_node_1()
{
  int id_0 = get_compute_id(0);
  while(1){
    for (unsigned long ii = (unsigned long )0; ii < ((unsigned long )1035); ii++) 
// Original: #pragma ACCEL pipeline flatten
// Original: #pragma ACCEL PIPELINE
{
      char rf_vec_sn_tmp_0;
      char rf_vec_sp_tmp_0;
      rf_vec_sn_tmp_0 = ((char )(read_channel_altera_ch_rf_vec_2_p__merlinalterafuncend(ch_rf_vec_2_p[id_0])));
      rf_vec_sp_tmp_0 = rf_vec_sn_tmp_0;
      write_channel_altera_ch_rf_vec_0_p__merlinalterafuncend(ch_rf_vec_0_p[id_0],rf_vec_sp_tmp_0);
// pvELoad and pvHLoad are already where they need to be
// Adjust the load and store vectors here.  
//pvHStore = pvHStore + colstride;
//pvEStore = pvELoad + colstride + 3 * ROWSTRIDE;
//pvFStore = pvFStore + ROWSTRIDE * 3;
    }
  }
}

void msm_port_rf_vec_msm_node_1_0(__global char rf_vec[20480000],int _i_sub,int i)
{
  int __i_sub;
  int _i;
  __i_sub = _i_sub;
  _i = i;
  for (unsigned long ii = (unsigned long )0; ii < ((unsigned long )1035); ii++) 
// Original: #pragma ACCEL pipeline flatten
// Original: #pragma ACCEL PIPELINE
{
    int _i_sub_0;
{
      
#pragma unroll
      for (_i_sub_0 = ((int )0); _i_sub_0 <= ((int )3); ++_i_sub_0) {
        char rf_vec_sp_tmp_0;
        rf_vec_sp_tmp_0 = rf_vec[((unsigned long )((_i * 4 + _i_sub_0) * 2048)) + ii];
        write_channel_altera_ch_rf_vec_2_p__merlinalterafuncend(ch_rf_vec_2_p[_i_sub_0],rf_vec_sp_tmp_0);
      }
    }
// pvELoad and pvHLoad are already where they need to be
// Adjust the load and store vectors here.  
//pvHStore = pvHStore + colstride;
//pvEStore = pvELoad + colstride + 3 * ROWSTRIDE;
//pvFStore = pvFStore + ROWSTRIDE * 3;
  }
}

void msm_port_i_sub_msm_node_1_0(int _i_sub)
{
  int __i_sub;
  __i_sub = _i_sub;
  __i_sub;
}

void msm_port_i_msm_node_1_0(int i)
{
  int _i;
  _i = i;
  _i;
}

__kernel void msm_port_rf_vec_align_kernel_fpga_0(__global char * restrict rf_vec)
{
  int _i_sub;
//   memcpy(profbuf_vec_buf, profbuf_vec, sizeof(m128i) * MAX_PROFBUF_LEN*MAX_NUM_SAMPLES);
//for (int i = 0; i < N; i++) {  // 10000
// 10000
// Original pramga:  ACCEL PARALLEL FACTOR=4 
  
#pragma unroll 1
  for (int i = 0; i < 2500; i++) 
// Original: #pragma ACCEL parallel factor=4
// Original: #pragma ACCEL PARALLEL FACTOR=4
{
    msm_port_rf_vec_msm_node_1_0(rf_vec,0,i);
    msm_port_i_sub_msm_node_1_0(0);
    msm_port_i_msm_node_1_0(i);
    
#pragma ACCEL mem_fence autokernel=msm_node_1 parent=msm_port_rf_vec_align_kernel_fpga_0 depth=4
// 1035
  }
// 1035
// Original pragma: ACCEL STREAM_PORT ACCESS_TYPE=read DIM_NUM=1 NODE=msm_node_1 VARIABLE=rf_vec
// Original: #pragma ACCEL parallel factor=4
// Original: #pragma ACCEL PARALLEL FACTOR=4
// Original pragma: ACCEL STREAM_NODE 
// Original pragma: ACCEL STREAM_NODE PARALLEL_FACTOR=4
}

__kernel void msm_port_rf_len_vec_align_kernel_fpga_0(__global int * restrict rf_len_vec)
{
  int _i_sub;
//   memcpy(profbuf_vec_buf, profbuf_vec, sizeof(m128i) * MAX_PROFBUF_LEN*MAX_NUM_SAMPLES);
//for (int i = 0; i < N; i++) {  // 10000
// 10000
// Original pramga:  ACCEL PARALLEL FACTOR=4 
  
#pragma unroll 1
  for (int i = 0; i < 2500; i++) 
// Original: #pragma ACCEL parallel factor=4
// Original: #pragma ACCEL PARALLEL FACTOR=4
{
    
#pragma unroll
    for (_i_sub = 0; _i_sub < 4; ++_i_sub) 
// Original: #pragma ACCEL parallel factor=4
// Original: #pragma ACCEL PARALLEL FACTOR=4
{
      int rf_len_vec_sp_tmp_0;
      rf_len_vec_sp_tmp_0 = rf_len_vec[i * 4 + _i_sub];
      write_channel_altera_ch_rf_len_vec_1_p__merlinalterafuncend(ch_rf_len_vec_1_p[_i_sub],rf_len_vec_sp_tmp_0);
    }
  }
}

__kernel void msm_port_profbuf_size_vec_align_kernel_fpga_0(__global int * restrict profbuf_size_vec)
{
  int _i_sub;
//   memcpy(profbuf_vec_buf, profbuf_vec, sizeof(m128i) * MAX_PROFBUF_LEN*MAX_NUM_SAMPLES);
//for (int i = 0; i < N; i++) {  // 10000
// 10000
// Original pramga:  ACCEL PARALLEL FACTOR=4 
  
#pragma unroll 1
  for (int i = 0; i < 2500; i++) 
// Original: #pragma ACCEL parallel factor=4
// Original: #pragma ACCEL PARALLEL FACTOR=4
{
    
#pragma unroll
    for (_i_sub = 0; _i_sub < 4; ++_i_sub) 
// Original: #pragma ACCEL parallel factor=4
// Original: #pragma ACCEL PARALLEL FACTOR=4
{
      int profbuf_size_vec_sp_tmp_0;
      profbuf_size_vec_sp_tmp_0 = profbuf_size_vec[i * 4 + _i_sub];
      write_channel_altera_ch_profbuf_size_vec_2_p__merlinalterafuncend(ch_profbuf_size_vec_2_p[_i_sub],profbuf_size_vec_sp_tmp_0);
    }
  }
}
void __merlin_dummy_msm_node_2();
 __attribute__ (( autorun))
 __attribute__ (( num_compute_units(4)))
 __attribute__ (( max_global_work_dim(0)))

__kernel void msm_node_2()
{
  int id_0 = get_compute_id(0);
  while(1){
    for (int j = 0; j < 128; j++) {
      
#pragma unroll
      for (int k = 0; k < 16; ++k) {
        unsigned char profbuf_vec_data_sn_tmp_0;
        unsigned char profbuf_vec_data_sp_tmp_0;
        profbuf_vec_data_sn_tmp_0 = ((unsigned char )(read_channel_altera_ch_profbuf_vec_data_3_0_p__merlinalterafuncend(ch_profbuf_vec_data_3_0_p[id_0][k])));
        profbuf_vec_data_sp_tmp_0 = profbuf_vec_data_sn_tmp_0;
        write_channel_altera_ch_profbuf_vec_data_3_p__merlinalterafuncend(ch_profbuf_vec_data_3_p[id_0][k],profbuf_vec_data_sp_tmp_0);
      }
    }
  }
}

void msm_port_profbuf_vec_data_msm_node_2_0(__global unsigned char *profbuf_vec_data,int _i_sub,int i)
{
  int __i_sub;
  int _i;
  __i_sub = _i_sub;
  _i = i;
  for (int j = 0; j < 128; j++) {
    
#pragma unroll
    for (int k = 0; k < 16; ++k) {
      int _i_sub_0;
{
        
#pragma unroll
        for (_i_sub_0 = ((int )0); _i_sub_0 <= ((int )3); ++_i_sub_0) {
          unsigned char profbuf_vec_data_sp_tmp_0;
          profbuf_vec_data_sp_tmp_0 = profbuf_vec_data[(_i * 4 + _i_sub_0) * 2048 + j * 16 + k];
          write_channel_altera_ch_profbuf_vec_data_3_0_p__merlinalterafuncend(ch_profbuf_vec_data_3_0_p[_i_sub_0][k],profbuf_vec_data_sp_tmp_0);
        }
      }
    }
  }
}

void msm_port_i_sub_msm_node_2_0(int _i_sub)
{
  int __i_sub;
  __i_sub = _i_sub;
  __i_sub;
}

void msm_port_i_msm_node_2_0(int i)
{
  int _i;
  _i = i;
  _i;
}

__kernel void msm_port_profbuf_vec_data_align_kernel_fpga_0(__global unsigned char * restrict profbuf_vec_data)
{
  int _i_sub;
//   memcpy(profbuf_vec_buf, profbuf_vec, sizeof(m128i) * MAX_PROFBUF_LEN*MAX_NUM_SAMPLES);
//for (int i = 0; i < N; i++) {  // 10000
// 10000
// Original pramga:  ACCEL PARALLEL FACTOR=4 
  
#pragma unroll 1
  for (int i = 0; i < 2500; i++) 
// Original: #pragma ACCEL parallel factor=4
// Original: #pragma ACCEL PARALLEL FACTOR=4
{
    msm_port_profbuf_vec_data_msm_node_2_0(profbuf_vec_data,0,i);
    msm_port_i_sub_msm_node_2_0(0);
    msm_port_i_msm_node_2_0(i);
    
#pragma ACCEL mem_fence autokernel=msm_node_2 parent=msm_port_profbuf_vec_data_align_kernel_fpga_0 depth=4
  }
// Original pragma: ACCEL STREAM_PORT ACCESS_TYPE=read DIM_NUM=1 NODE=msm_node_2 VARIABLE=profbuf_vec_data
// Original: #pragma ACCEL parallel factor=4
// Original: #pragma ACCEL PARALLEL FACTOR=4
// Original pragma: ACCEL STREAM_NODE 
// Original pragma: ACCEL STREAM_NODE PARALLEL_FACTOR=4
}

__kernel void msm_port_results_align_kernel_fpga_0(__global long * restrict results)
{
  int _i_sub;
//   memcpy(profbuf_vec_buf, profbuf_vec, sizeof(m128i) * MAX_PROFBUF_LEN*MAX_NUM_SAMPLES);
//for (int i = 0; i < N; i++) {  // 10000
// 10000
// Original pramga:  ACCEL PARALLEL FACTOR=4 
  
#pragma unroll 1
  for (int i = 0; i < 2500; i++) 
// Original: #pragma ACCEL parallel factor=4
// Original: #pragma ACCEL PARALLEL FACTOR=4
{
    
#pragma unroll
    for (_i_sub = 0; _i_sub < 4; ++_i_sub) 
// Original: #pragma ACCEL parallel factor=4
// Original: #pragma ACCEL PARALLEL FACTOR=4
{
      long results_sp_tmp_0;
      results_sp_tmp_0 = ((long )(read_channel_altera_ch_results_4_p__merlinalterafuncend(ch_results_4_p[_i_sub])));
      results[i * 4 + _i_sub] = results_sp_tmp_0;
    }
  }
}
void __merlin_dummy_msm_node_0();
 __attribute__ (( autorun))
 __attribute__ (( num_compute_units(4)))
 __attribute__ (( max_global_work_dim(0)))

__kernel void msm_node_0()
{
  int id_0 = get_compute_id(0);
  while(1){
    int i_sn_tmp_0;
    int _i;
    i_sn_tmp_0 = ((int )(read_channel_altera_ch_i_1_p__merlinalterafuncend(ch_i_1_p[id_0])));
    _i = i_sn_tmp_0;
    int i_sub_sn_tmp_0;
    int _i_sub;
    i_sub_sn_tmp_0 = ((int )(read_channel_altera_ch_i_sub_0_p__merlinalterafuncend(ch_i_sub_0_p[id_0])));
    _i_sub = i_sub_sn_tmp_0;
    long results_sn_tmp_0;
    int profbuf_size_vec_sn_tmp_0;
    int rf_len_vec_sn_tmp_0;
    unsigned char profbuf_data[128][16];
    for (int j = 0; j < 128; j++) {
      
#pragma unroll
      for (int k = 0; k < 16; ++k) {
        unsigned char profbuf_vec_data_sn_tmp_0;
        profbuf_vec_data_sn_tmp_0 = ((unsigned char )(read_channel_altera_ch_profbuf_vec_data_3_p__merlinalterafuncend(ch_profbuf_vec_data_3_p[id_0][k])));
        profbuf_data[j][k] = profbuf_vec_data_sn_tmp_0;
      }
    }
// rf_vec: 2048
//results[i] = align_kernel(&rf_vec[i*MAX_RF_LEN], rf_len_vec[i],
//                          profbuf_size_vec[i], &profbuf_vec[i*MAX_PROFBUF_LEN], i);
    rf_len_vec_sn_tmp_0 = ((int )(read_channel_altera_ch_rf_len_vec_1_p__merlinalterafuncend(ch_rf_len_vec_1_p[id_0])));
    profbuf_size_vec_sn_tmp_0 = ((int )(read_channel_altera_ch_profbuf_size_vec_2_p__merlinalterafuncend(ch_profbuf_size_vec_2_p[id_0])));
    results_sn_tmp_0 = align_kernel(profbuf_data,id_0);
    write_channel_altera_ch_results_4_p__merlinalterafuncend(ch_results_4_p[id_0],results_sn_tmp_0);
  }
}

void msm_port_rf_vec_msm_node_0_0()
{
}

void msm_port_i_sub_msm_node_0_0(int i_sub)
{
  int i_sub_0;
{
    
#pragma unroll
    for (i_sub_0 = ((int )0); i_sub_0 <= ((int )3); ++i_sub_0) {
      int i_sub_sp_tmp_0;
      int _i_sub;
      i_sub_sp_tmp_0 = i_sub_0;
      write_channel_altera_ch_i_sub_0_p__merlinalterafuncend(ch_i_sub_0_p[i_sub_0],i_sub_sp_tmp_0);
      _i_sub = i_sub_0;
      _i_sub;
    }
  }
}

void msm_port_i_msm_node_0_0(int i,int i_sub)
{
  int i_sub_0;
{
    
#pragma unroll
    for (i_sub_0 = ((int )0); i_sub_0 <= ((int )3); ++i_sub_0) {
      int i_sp_tmp_0;
      int _i;
      i_sp_tmp_0 = i;
      write_channel_altera_ch_i_1_p__merlinalterafuncend(ch_i_1_p[i_sub_0],i_sp_tmp_0);
      _i = i;
      _i;
    }
  }
}

__kernel void align_kernel_fpga(__global char * restrict rf_vec,__global int * restrict rf_len_vec,__global int * restrict profbuf_size_vec,__global unsigned char * restrict profbuf_vec_data,__global long * restrict results,int N)
{
  int _i_sub;
  int _i_sub_1;
  
#pragma unroll
  for (_i_sub_1 = 0; _i_sub_1 <= 3; ++_i_sub_1) {
    int k;
  }
  
#pragma ACCEL STREAM_PORT ACCESS_TYPE=read DIM_NUM=1 NODE=align_kernel_fpga VARIABLE=profbuf_vec_data
  int _i_sub_3;
  
#pragma ACCEL STREAM_PORT ACCESS_TYPE=read DIM_NUM=1 NODE=align_kernel_fpga VARIABLE=profbuf_size_vec
  int _i_sub_5;
  
#pragma ACCEL STREAM_PORT ACCESS_TYPE=read DIM_NUM=1 NODE=align_kernel_fpga VARIABLE=rf_len_vec
  int i_sub_0;
  
#pragma ACCEL STREAM_PORT ACCESS_TYPE=read DIM_NUM=1 NODE=align_kernel_fpga VARIABLE=rf_vec
  int i_sub;
  
#pragma ACCEL INTERFACE STREAM_PREFETCH=on VARIABLE=profbuf_vec_data max_depth=1280000,16 DEPTH=1280000,16 memory_burst=off
  
#pragma ACCEL interface variable=rf_vec stream_prefetch=on max_depth=20480000 depth=20480000 memory_burst=off
  
#pragma ACCEL interface variable=rf_len_vec stream_prefetch=on max_depth=10000 depth=10000 memory_burst=off
  
#pragma ACCEL interface variable=profbuf_size_vec stream_prefetch=on max_depth=10000 depth=10000 memory_burst=off
  
#pragma ACCEL interface variable=results stream_prefetch=on max_depth=10000 depth=10000 memory_burst=off
//   memcpy(profbuf_vec_buf, profbuf_vec, sizeof(m128i) * MAX_PROFBUF_LEN*MAX_NUM_SAMPLES);
//for (int i = 0; i < N; i++) {  // 10000
// 10000
// Original pramga:  ACCEL PARALLEL FACTOR=4 
  
#pragma unroll 1
  for (int i = 0; i < 2500; i++) 
// Original: #pragma ACCEL parallel factor=4
// Original: #pragma ACCEL PARALLEL FACTOR=4
{
    msm_port_rf_vec_msm_node_0_0();
    msm_port_i_sub_msm_node_0_0(0);
    msm_port_i_msm_node_0_0(i,0);
    
#pragma ACCEL mem_fence autokernel=msm_node_0 parent=align_kernel_fpga depth=4
  }
// Original pragma: ACCEL STREAM_PORT ACCESS_TYPE=read DIM_NUM=0 NODE=msm_node_0 VARIABLE=i_sub
// Original pragma: ACCEL STREAM_PORT ACCESS_TYPE=read DIM_NUM=0 NODE=msm_node_0 VARIABLE=i
// Original: #pragma ACCEL parallel factor=4
// Original: #pragma ACCEL PARALLEL FACTOR=4
// Original pragma: ACCEL STREAM_NODE 
// Original pragma: ACCEL STREAM_NODE PARALLEL_FACTOR=4
  int i_sub_1;
  int _i_sub_6;
  int _i_sub_4;
  int _i_sub_2;
  
#pragma unroll
  for (_i_sub_2 = 0; _i_sub_2 <= 3; ++_i_sub_2) {
    int k_0;
  }
  
#pragma ACCEL STREAM_PORT ACCESS_TYPE=write DIM_NUM=1 NODE=align_kernel_fpga VARIABLE=results
  int _i_sub_0;
}
