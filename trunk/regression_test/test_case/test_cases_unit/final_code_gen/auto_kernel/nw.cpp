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
#include "nw.h"
#define MATCH_SCORE 1
#define MISMATCH_SCORE -1
#define GAP_SCORE -1
#define ALIGN '\\'
#define SKIPA '^'
#define SKIPB '<'
//int M[(ALEN+1)*(BLEN+1)], char ptr[(ALEN+1)*(BLEN+1)]);
void needwun_core(char SEQA[128],char SEQB[128],char alignedA[128 + 128],char alignedB[128 + 128]);
#define MAX(A,B) ( ((A)>(B))?(A):(B) )
//void needwun_kernel(char SEQA[ALEN*BATCH], char SEQB[BLEN*BATCH],
//             char alignedA[ALEN*BATCH+BLEN*BATCH], char alignedB[ALEN*BATCH+BLEN*BATCH],
//             int M[(ALEN+1)*(BLEN+1)*BATCH], char ptr[(ALEN+1)*(BLEN+1)*BATCH])
static void __merlin_dummy_kernel_pragma();

void needwun_kernel(char *SEQA,char *SEQB,char *alignedA,char *alignedB)
//int *M, char *ptr)
{
  
#pragma ACCEL interface variable=SEQA bus_bitwidth=256 depth=131072 stream_prefetch=off
  
#pragma ACCEL interface variable=SEQB bus_bitwidth=256 depth=131072 stream_prefetch=off
  
#pragma ACCEL interface variable=alignedA bus_bitwidth=256 depth=262144 stream_prefetch=off
  
#pragma ACCEL interface variable=alignedB bus_bitwidth=256 depth=262144 stream_prefetch=off
  int i;
//#pragma ACCEL pipeline // turn off cgpip because of bram usage
//#pragma unroll 16
  
#pragma max_concurrency 1
  
#pragma ACCEL parallel factor=32
  for (i = 0; i < 1024; i++) {
    needwun_core(SEQA + 128 * i,SEQB + 128 * i,alignedA + (128 + 128) * i,alignedB + (128 + 128) * i);
//alignedB + (ALEN+BLEN) * i, M + ((ALEN+1)*(BLEN+1)) * i, ptr + ((ALEN+1)*(BLEN+1)) * i);
  }
}

void needwun_core(char SEQA[128],char SEQB[128],char alignedA[128 + 128],char alignedB[128 + 128])
//int M_[(ALEN+1)*(BLEN+1)], char ptr_[(ALEN+1)*(BLEN+1)]){
{
//int M[(ALEN+1)*(BLEN+1)];
  int M_shift_reg[128 + 1];
  int M_shift_idx = 0;
  char ptr[129 * 129];
  int score;
  int up_left;
  int up;
  int left;
  int max;
  int row;
  int row_up;
  int r;
  int a_idx;
  int b_idx;
  int a_str_idx;
  int b_str_idx;
  char SEQA_buf[128];
  char SEQB_buf[128];
  for (int i = 0; i < 128; ++i) {
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
  int Mmax_up = 0;
// Matrix filling loop
  fill_out:
  for (b_idx = 1; b_idx < 128 + 1; b_idx++) {
    row_up = (b_idx - 1) * (128 + 1);
    row = b_idx * (128 + 1);
//M[row];
    M_pre = b_idx * - 1;
//M[row_up];
    M_up_pre = b_idx * - 1;
    fill_in:
    for (a_idx = 1; a_idx < 128 + 1; a_idx++) {
      if (SEQA_buf[a_idx - 1] == SEQB_buf[b_idx - 1]) {
        score = 1;
      }
       else {
        score = - 1;
      }
//int M_up = M[row_up + (a_idx  )] ;
      int M_up = b_idx == 1?(a_idx + 1) * - 1 : M_shift_reg[1];
// M_up += GAP_SCORE;
      up_left = M_up_pre + score;
      up = M_up;
      left = M_pre + - 1;
      M_up_pre = M_up - - 1;
      max = (left > ((up_left > up?up_left : up))?left : ((up_left > up?up_left : up)));
//M[row + a_idx] = 
      M_pre = max;
      char tmp_ch;
      if (max == left) {
//ptr[row + a_idx] = SKIPB;
        tmp_ch = '<';
      }
       else if (max == up) {
//ptr[row + a_idx] = SKIPA;
        tmp_ch = '^';
      }
       else {
//ptr[row + a_idx] = ALIGN;
        tmp_ch = '\\';
      }
      ptr[row + a_idx] = tmp_ch;
      
#pragma ACCEL parallel
//#pragma unroll 
      for (int t = 0; t < 128 - 1; t++) 
        M_shift_reg[t] = M_shift_reg[t + 1];
      M_shift_reg[128 - 1] = M_shift_reg[128] + - 1;
      M_shift_reg[128] = M_pre;
      Mmax_up = (M_shift_reg[0] > M_shift_reg[1]?M_shift_reg[0] : M_shift_reg[1]);
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
  while(a_idx > 0 || b_idx > 0){
    
#pragma HLS loop_tripcount max=128
    r = b_idx * (128 + 1);
    if (ptr[r + a_idx] == '\\') {
      alignedA_t[a_str_idx++] = SEQA_buf[a_idx - 1];
      alignedB_t[b_str_idx++] = SEQB_buf[b_idx - 1];
      a_idx--;
      b_idx--;
    }
     else if (ptr[r + a_idx] == '<') {
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
// Pad the result
  int b1 = a_str_idx;
  pad_a:
  for (a_str_idx = 0; a_str_idx < 128 + 128; a_str_idx++) {
    
#pragma HLS loop_tripcount max=256
//alignedA[a_str_idx] = (a_str_idx < b1) ? alignedA_t[a_str_idx] : '_';
    char tmp = a_str_idx < b1?alignedA_t[a_str_idx] : '_';
    alignedA[a_str_idx] = tmp;
  }
  int b2 = b_str_idx;
  pad_b:
  for (b_str_idx = 0; b_str_idx < 128 + 128; b_str_idx++) {
    
#pragma HLS loop_tripcount max=256
//alignedB[b_str_idx] = (b_str_idx < b2) ? alignedB_t[b_str_idx] :'_';
    char tmp = b_str_idx < b2?alignedB_t[b_str_idx] : '_';
    alignedB[b_str_idx] = tmp;
  }
}
