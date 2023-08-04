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
#define __constant
#define __kernel
#define __global
#include <string.h> 

#include "merlin_type_define.h"



static void needwun_core(char SEQA[128],char SEQB[128],char alignedA[256],char alignedB[256])
{
//int M[(ALEN+1)*(BLEN+1)];
  int M_shift_reg[2][128];
  
#pragma HLS array_partition variable=M_shift_reg complete dim=2
  
#pragma HLS array_partition variable=M_shift_reg complete dim=1
  char ptr[129 * 129];
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
  for (int i = 0; i < 128; ++i) 
// Original: #pragma ACCEL PIPELINE AUTO
{
    
#pragma HLS pipeline
    SEQA_buf[i] = SEQA[i];
    SEQB_buf[i] = SEQB[i];
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
    for (a_idx = 1; a_idx < 128 + 1; a_idx++) 
// Original: #pragma ACCEL PIPELINE AUTO
{
      
#pragma HLS pipeline
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
        char rose__temp = true;
        int rose_temp_0;
        if (up_left > up) {
          rose_temp_0 = up_left;
        }
         else {
          rose_temp_0 = up;
        }
        rose__temp = ((bool )(left > rose_temp_0));
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
      for (int t = 0; t < 128 - 1; t++) 
// Original: #pragma ACCEL parallel
// Original: #pragma ACCEL PARALLEL COMPLETE
// Original: #pragma ACCEL PARALLEL COMPLETE
{
        
#pragma HLS unroll
        M_shift_reg[0][t] = M_shift_reg[0][1 + t];
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
  char alignedA_t[128 + 128];
  char alignedB_t[128 + 128];
  trace:
  while(a_idx > 0 || b_idx > 0)
// Original: #pragma ACCEL PIPELINE AUTO
{
    
#pragma HLS pipeline
    
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
  for (a_str_idx = 0; a_str_idx < 128 + 128; a_str_idx++) 
// Original: #pragma ACCEL PIPELINE AUTO
{
    
#pragma HLS pipeline
    
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
    alignedA[a_str_idx] = tmp;
  }
  int b2 = b_str_idx;
  pad_b:
  for (b_str_idx = 0; b_str_idx < 128 + 128; b_str_idx++) 
// Original: #pragma ACCEL PIPELINE AUTO
{
    
#pragma HLS pipeline
    
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
    alignedB[b_str_idx] = tmp;
  }
}

// Original: #pragma ACCEL kernel

void needwun_kernel_L_0_2_para_sub(int i_sub,char alignedB_buf_0[256],char alignedA_buf_0[256],char SEQB_buf_0[128],char SEQA_buf_0[128])
// Original: #pragma ACCEL parallel factor=64
// Original: #pragma ACCEL PARALLEL FACTOR=64
// Original: #pragma ACCEL PARALLEL
{
  
#pragma HLS function_instantiate variable=i_sub
  
#pragma HLS inline off
  needwun_core(SEQA_buf_0,SEQB_buf_0,alignedA_buf_0,alignedB_buf_0);
//alignedB + (ALEN+BLEN) * i, M + ((ALEN+1)*(BLEN+1)) * i, ptr + ((ALEN+1)*(BLEN+1)) * i);
}

void needwun_kernel_L_0_2_para(char alignedB_buf_0[64][256],char alignedA_buf_0[64][256],char SEQB_buf_0[64][128],char SEQA_buf_0[64][128])
{
  
#pragma HLS inline off
  int i_sub;
  for (i_sub = 0; i_sub < 64; ++i_sub) {
    
#pragma HLS unroll complete
    needwun_kernel_L_0_2_para_sub(i_sub,alignedB_buf_0[i_sub],alignedA_buf_0[i_sub],SEQB_buf_0[i_sub],SEQA_buf_0[i_sub]);
  }
}
static int needwun_kernel_dummy_pos;

static void merlin_memcpy_0(char dst[128],int dst_idx_1,char *src,int src_idx_0,unsigned int len,unsigned int max_len)
{
  
#pragma HLS inline off
  
#pragma HLS function_instantiate variable=dst_idx_1,src_idx_0
  long long i;
  long long total_offset1 = 0 * 128 + dst_idx_1;
  long long total_offset2 = 0 * 0 + src_idx_0;
  for (i = 0; i < len / 1; ++i) {
    
#pragma HLS PIPELINE II=1
    
#pragma HLS LOOP_TRIPCOUNT max=128
    dst[total_offset1 + i] = src[total_offset2 + i];
  }
}

static void merlin_memcpy_1(char dst[128],int dst_idx_1,char *src,int src_idx_0,unsigned int len,unsigned int max_len)
{
  
#pragma HLS inline off
  
#pragma HLS function_instantiate variable=dst_idx_1,src_idx_0
  long long i;
  long long total_offset1 = 0 * 128 + dst_idx_1;
  long long total_offset2 = 0 * 0 + src_idx_0;
  for (i = 0; i < len / 1; ++i) {
    
#pragma HLS PIPELINE II=1
    
#pragma HLS LOOP_TRIPCOUNT max=128
    dst[total_offset1 + i] = src[total_offset2 + i];
  }
}

static void merlin_memcpy_2(char *dst,int dst_idx_0,char src[256],int src_idx_1,unsigned int len,unsigned int max_len)
{
  
#pragma HLS inline off
  
#pragma HLS function_instantiate variable=dst_idx_0,src_idx_1
  long long i;
  long long total_offset1 = 0 * 0 + dst_idx_0;
  long long total_offset2 = 0 * 256 + src_idx_1;
  for (i = 0; i < len / 1; ++i) {
    
#pragma HLS PIPELINE II=1
    
#pragma HLS LOOP_TRIPCOUNT max=256
    dst[total_offset1 + i] = src[total_offset2 + i];
  }
}

static void merlin_memcpy_3(char *dst,int dst_idx_0,char src[256],int src_idx_1,unsigned int len,unsigned int max_len)
{
  
#pragma HLS inline off
  
#pragma HLS function_instantiate variable=dst_idx_0,src_idx_1
  long long i;
  long long total_offset1 = 0 * 0 + dst_idx_0;
  long long total_offset2 = 0 * 256 + src_idx_1;
  for (i = 0; i < len / 1; ++i) {
    
#pragma HLS PIPELINE II=1
    
#pragma HLS LOOP_TRIPCOUNT max=256
    dst[total_offset1 + i] = src[total_offset2 + i];
  }
}
extern "C" { 

__kernel void needwun_kernel(char *SEQA,char *SEQB,char *alignedA,char *alignedB)
{
  
#pragma HLS INTERFACE m_axi port=SEQA offset=slave depth=131072
  
#pragma HLS INTERFACE m_axi port=SEQB offset=slave depth=131072
  
#pragma HLS INTERFACE m_axi port=alignedA offset=slave depth=262144
  
#pragma HLS INTERFACE m_axi port=alignedB offset=slave depth=262144
  
#pragma HLS INTERFACE s_axilite port=SEQA bundle=control
  
#pragma HLS INTERFACE s_axilite port=SEQB bundle=control
  
#pragma HLS INTERFACE s_axilite port=alignedA bundle=control
  
#pragma HLS INTERFACE s_axilite port=alignedB bundle=control
  
#pragma HLS INTERFACE s_axilite port=return bundle=control
  int i_sub;
  
#pragma ACCEL interface variable=SEQA max_depth=131072 depth=131072
  
#pragma ACCEL interface variable=SEQB max_depth=131072 depth=131072
  
#pragma ACCEL interface variable=alignedA max_depth=262144 depth=262144
  
#pragma ACCEL interface variable=alignedB max_depth=262144 depth=262144
  int i;
//#pragma ACCEL pipeline // turn off cgpip because of bram usage
//#pragma unroll 16
// Original pramga:  ACCEL PARALLEL FACTOR=64 
  for (i = 0; i < 16; i++) 
// Original: #pragma ACCEL parallel factor=64
// Original: #pragma ACCEL PARALLEL FACTOR=64
{
    char alignedB_buf_0[64][256];
// Existing HLS partition: #pragma HLS array_partition variable=alignedB_buf_0 dim=1 complete
    
#pragma HLS array_partition variable=alignedB_buf_0 complete dim=1
    char alignedA_buf_0[64][256];
// Existing HLS partition: #pragma HLS array_partition variable=alignedA_buf_0 dim=1 complete
    
#pragma HLS array_partition variable=alignedA_buf_0 complete dim=1
    char SEQB_buf_0[64][128];
// Existing HLS partition: #pragma HLS array_partition variable=SEQB_buf_0 dim=1 complete
    
#pragma HLS array_partition variable=SEQB_buf_0 complete dim=1
    char SEQA_buf_0[64][128];
// Existing HLS partition: #pragma HLS array_partition variable=SEQA_buf_0 dim=1 complete
    
#pragma HLS array_partition variable=SEQA_buf_0 complete dim=1
{
      int i_sub;
      for (i_sub = 0; i_sub < 64; ++i_sub) {
        
#pragma HLS UNROLL
        merlin_memcpy_0(SEQA_buf_0[i_sub],0,SEQA,i * 8192 + i_sub * 128,sizeof(char ) * ((unsigned long )128),128UL);
      }
    }
{
      int i_sub;
      for (i_sub = 0; i_sub < 64; ++i_sub) {
        
#pragma HLS UNROLL
        merlin_memcpy_1(SEQB_buf_0[i_sub],0,SEQB,i * 8192 + i_sub * 128,sizeof(char ) * ((unsigned long )128),128UL);
      }
    }
    needwun_kernel_L_0_2_para(alignedB_buf_0,alignedA_buf_0,SEQB_buf_0,SEQA_buf_0);
    for (i_sub = 0; i_sub < 64; ++i_sub) 
// Original: #pragma ACCEL parallel factor=64
// Original: #pragma ACCEL PARALLEL FACTOR=64
// Original: #pragma ACCEL PARALLEL
{
//alignedB + (ALEN+BLEN) * i, M + ((ALEN+1)*(BLEN+1)) * i, ptr + ((ALEN+1)*(BLEN+1)) * i);
    }{
      int i_sub;
      for (i_sub = 0; i_sub < 64; ++i_sub) {
        
#pragma HLS UNROLL
        merlin_memcpy_2(alignedB,i * 16384 + i_sub * 256,alignedB_buf_0[i_sub],0,sizeof(char ) * ((unsigned long )256),256UL);
      }
    }
{
      int i_sub;
      for (i_sub = 0; i_sub < 64; ++i_sub) {
        
#pragma HLS UNROLL
        merlin_memcpy_3(alignedA,i * 16384 + i_sub * 256,alignedA_buf_0[i_sub],0,sizeof(char ) * ((unsigned long )256),256UL);
      }
    }
//alignedB + (ALEN+BLEN) * i, M + ((ALEN+1)*(BLEN+1)) * i, ptr + ((ALEN+1)*(BLEN+1)) * i);
  }
}
}
