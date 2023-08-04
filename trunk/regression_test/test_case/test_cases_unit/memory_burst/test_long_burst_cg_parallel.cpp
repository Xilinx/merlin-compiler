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
#include "cmost.h"
static void CPF_1(char pattern[4],int kmpNext[4])
{
  __merlin_access_range(kmpNext,0,3UL);
  __merlin_access_range(pattern,0,3UL);
  int k;
  int q;
  k = 0;
  kmpNext[0] = 0;
  c1:
  for (q = 1; q < 4; q++) {
    c2:
    while(k > 0 && ((int )pattern[k]) != ((int )pattern[q])){
      
#pragma ACCEL PIPELINE AUTO
      
#pragma HLS loop_tripcount max=6
      k = kmpNext[q];
    }
    if (((int )pattern[k]) == ((int )pattern[q])) {
      k++;
    }
    kmpNext[q] = k;
  }
}

static int kmp_core_1(char pattern[4],char input[32768],int n_matches[1])
{
  __merlin_access_range(n_matches,0,0UL);
  __merlin_access_range(input,0,32767UL);
  __merlin_access_range(pattern,0,3UL);
  int i;
  int q;
  int kmpNext[4];
  n_matches[0] = 0;
  CPF_1(pattern,kmpNext);
  q = 0;
  k1:
  for (i = 0; i < 32768; i++) {
    char i_t = input[i];
    k2:
    while(q > 0 && ((int )pattern[q]) != ((int )i_t)){
      
#pragma ACCEL PIPELINE AUTO
      
#pragma HLS loop_tripcount max=6
      q = kmpNext[q];
    }
    if (((int )pattern[q]) == ((int )i_t)) {
      q++;
    }
    if (q >= 4) {
      n_matches[0]++;
      q = kmpNext[q - 1];
    }
  }
  return 0;
}
#pragma ACCEL kernel

void kmp_kernel(char pattern[16384],char merlin_input[134217728],int n_matches[4096])
{
  
#pragma ACCEL interface variable=n_matches depth=4096 max_depth=4096
  
#pragma ACCEL interface variable=merlin_input depth=134217728 max_depth=134217728
  
#pragma ACCEL interface variable=pattern depth=16384 max_depth=16384
  __merlin_access_range(n_matches,0,4095UL);
  __merlin_access_range(merlin_input,0,134217727UL);
  __merlin_access_range(pattern,0,16383UL);
  int i;
  
#pragma omp parallel for num_threads(16)
  for (i = 0; i < 256; i++) 
{
    for (int i_sub = 0; i_sub < 16; ++i_sub) 
{
      
#pragma ACCEL PARALLEL
      kmp_core_1(pattern + (i * 16 + i_sub) * 4,merlin_input + (i * 16 + i_sub) * 32768,n_matches + (i * 16 + i_sub));
    }
  }
}
