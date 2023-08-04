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
#include <string.h> 
#include "cmost.h"
#include "merlin_type_define.h"
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_sbox();

static void aes_subBytes(unsigned char *buf)
{
  unsigned char i = (unsigned char )16;
  sub:
{
  }
  while(i--){
    buf[i] = sbox[buf[i]];
  }
/* aes_subBytes */
}

static void aes_addRoundKey(unsigned char *buf,__global unsigned char *key)
{
  unsigned char i = (unsigned char )16;
  addkey:
{
  }
  while(i--){
    buf[i] ^= ((int )key[i]);
  }
/* aes_addRoundKey */
}

static void aes_addRoundKey_cpy(unsigned char *buf,__global unsigned char *key,__global unsigned char *cpk)
{
  unsigned char i = (unsigned char )16;
  cpkey:
{
  }
  while(i--){
    (buf[i] ^= ((int )(cpk[i] = key[i])) , cpk[16 + ((int )i)] = key[16 + ((int )i)]);
  }
/* aes_addRoundKey_cpy */
}

static void aes_shiftRows(unsigned char *buf)
{
/* to make it potentially parallelable :) */
  unsigned char i;
  unsigned char j;
  i = buf[1];
  buf[1] = buf[5];
  buf[5] = buf[9];
  buf[9] = buf[13];
  buf[13] = i;
  i = buf[10];
  buf[10] = buf[2];
  buf[2] = i;
  j = buf[3];
  buf[3] = buf[15];
  buf[15] = buf[11];
  buf[11] = buf[7];
  buf[7] = j;
  j = buf[14];
  buf[14] = buf[6];
  buf[6] = j;
/* aes_shiftRows */
}

static unsigned char rj_xtime(unsigned char x)
{
  int rose_temp;
  if (((int )x) & 0x80) {
    rose_temp = ((int )x) << 1 ^ 0x1b;
  }
   else {
    rose_temp = ((int )x) << 1;
  }
  return (unsigned char )rose_temp;
/* rj_xtime */
}

static void aes_mixColumns(unsigned char *buf)
{
  unsigned char a;
  unsigned char b;
  unsigned char c;
  unsigned char d;
  unsigned char e;
  mix:
{
  }
  int _s_i;
/* Canonicalized from: for(_s_i =((unsigned char )0);((int )_s_i) < 16;_s_i += 4) {...} */
/* Stantardize from: for(_s_i =((unsigned char )0);_s_i <= 15;_s_i += 4) {...} */
  
#pragma unroll
  for (_s_i = 0; _s_i <= 3; _s_i++) {
    int _in_s_i = 0 + ((int )4L) * _s_i;
    a = buf[_s_i * 4];
    b = buf[1 + _s_i * 4];
    c = buf[2 + _s_i * 4];
    d = buf[3 + _s_i * 4];
    e = ((unsigned char )(((int )a) ^ ((int )b) ^ ((int )c) ^ ((int )d)));
    buf[_s_i * 4] ^= ((int )e) ^ ((int )(rj_xtime((unsigned char )(((int )a) ^ ((int )b)))));
    buf[1 + _s_i * 4] ^= ((int )e) ^ ((int )(rj_xtime((unsigned char )(((int )b) ^ ((int )c)))));
    buf[2 + _s_i * 4] ^= ((int )e) ^ ((int )(rj_xtime((unsigned char )(((int )c) ^ ((int )d)))));
    buf[3 + _s_i * 4] ^= ((int )e) ^ ((int )(rj_xtime((unsigned char )(((int )d) ^ ((int )a)))));
  }
  _s_i = 12 + ((int )4L);
/* aes_mixColumns */
}
#pragma ACCEL string 1234

static void aes_expandEncKey(__global unsigned char *k,unsigned char *rc)
{
  k[0] ^= ((int )sbox[k[29]]) ^ ((int )( *rc));
  k[1] ^= ((int )sbox[k[30]]);
  k[2] ^= ((int )sbox[k[31]]);
  k[3] ^= ((int )sbox[k[28]]);
   *rc = ((unsigned char )(((int )( *rc)) << 1 ^ (((int )( *rc)) >> 7 & 1) * 0x1b));
  exp1:
{
  }
  int _s_i_0;
/* Canonicalized from: for(_s_i_0 =((unsigned char )4);((int )_s_i_0) < 16;_s_i_0 += 4) {...} */
/* Stantardize from: for(_s_i_0 =((unsigned char )4);_s_i_0 <= 15;_s_i_0 += 4) {...} */
  
#pragma unroll
  for (_s_i_0 = 0; _s_i_0 <= 2; _s_i_0++) {
    int _in_s_i_0 = 4 + ((int )4L) * _s_i_0;
    (((k[_in_s_i_0] ^= ((int )k[((int )_in_s_i_0) - 4]) , k[((int )_in_s_i_0) + 1] ^= ((int )k[((int )_in_s_i_0) - 3])) , k[((int )_in_s_i_0) + 2] ^= ((int )k[((int )_in_s_i_0) - 2])) , k[((int )_in_s_i_0) + 3] ^= ((int )k[((int )_in_s_i_0) - 1]));
  }
  _s_i_0 = 12 + ((int )4L);
  k[16] ^= ((int )sbox[k[12]]);
  k[17] ^= ((int )sbox[k[13]]);
  k[18] ^= ((int )sbox[k[14]]);
  k[19] ^= ((int )sbox[k[15]]);
  exp2:
{
  }
  int _s_i;
/* Canonicalized from: for(_s_i =((unsigned char )20);((int )_s_i) < 32;_s_i += 4) {...} */
/* Stantardize from: for(_s_i =((unsigned char )20);_s_i <= 31;_s_i += 4) {...} */
  
#pragma unroll
  for (_s_i = 0; _s_i <= 2; _s_i++) {
    int _in_s_i = 20 + ((int )4L) * _s_i;
    (((k[_in_s_i] ^= ((int )k[((int )_in_s_i) - 4]) , k[((int )_in_s_i) + 1] ^= ((int )k[((int )_in_s_i) - 3])) , k[((int )_in_s_i) + 2] ^= ((int )k[((int )_in_s_i) - 2])) , k[((int )_in_s_i) + 3] ^= ((int )k[((int )_in_s_i) - 1]));
  }
  _s_i = 28 + ((int )4L);
/* aes_expandEncKey */
}
static void __merlin_dummy_kernel_pragma();
/* Original: #pragma ACCEL kernel */

__kernel void aes256_encrypt_ecb_kernel(__global unsigned char * restrict ctx_key,__global unsigned char * restrict ctx_enckey,__global unsigned char * restrict ctx_deckey,__global unsigned char * restrict k,__global unsigned char * restrict buf)
{
  unsigned char k_buf_0[32];
  long _memcpy_i_0;
  
#pragma unroll 32
  for (_memcpy_i_0 = 0; _memcpy_i_0 < sizeof(unsigned char ) * ((unsigned long )32) / 1; ++_memcpy_i_0) {
    long total_offset1 = (0 * 32 + 0);
    long total_offset2 = (0 * 32 + 0);
    k_buf_0[total_offset1 + _memcpy_i_0] = k[total_offset2 + _memcpy_i_0];
  }
  unsigned char buf_buf_0[16];
  long _memcpy_i_1;
  
#pragma unroll 16
  for (_memcpy_i_1 = 0; _memcpy_i_1 < sizeof(unsigned char ) * ((unsigned long )16) / 1; ++_memcpy_i_1) {
    long total_offset1 = (0 * 16 + 0);
    long total_offset2 = (0 * 16 + 0);
    buf_buf_0[total_offset1 + _memcpy_i_1] = buf[total_offset2 + _memcpy_i_1];
  }
  
#pragma ACCEL INTERFACE BURST_OFF VARIABLE=ctx_deckey max_depth=1,32 DEPTH=1,32
  
#pragma ACCEL INTERFACE BURST_OFF VARIABLE=ctx_enckey max_depth=1,32 DEPTH=1,32
  
#pragma ACCEL INTERFACE BURST_OFF VARIABLE=ctx_key max_depth=1,32 DEPTH=1,32
//INIT
  unsigned char rcon = (unsigned char )1;
  
#pragma ACCEL interface variable=k max_depth=32 depth=32
  
#pragma ACCEL interface variable=buf max_depth=16 depth=16
  ecb1:
{
  }
  int _s_i_1;
/* Canonicalized from: for(_s_i_1 =((unsigned char )0);((unsigned long )_s_i_1) < sizeof(ctx -> key);_s_i_1++) {...} */
  for (_s_i_1 = ((int )((unsigned char )0)); _s_i_1 <= 31; ++_s_i_1) {
    ctx_enckey[_s_i_1] = ctx_deckey[_s_i_1] = k_buf_0[_s_i_1];
  }
  ecb2:
{
  }
  int _s_i_0;
/* Canonicalized from: for(_s_i_0 =((unsigned char )8);((int )_s_i_0) > 1;--_s_i_0) {...} */
/* Stantardize from: for(_s_i_0 =((unsigned char )8);_s_i_0 >= 2;--_s_i_0) {...} */
  for (_s_i_0 = 0; _s_i_0 <= 6; _s_i_0++) {
    aes_expandEncKey(ctx_deckey,&rcon);
  }
  _s_i_0 = 2 + ((int )(- 1));
//DEC
  aes_addRoundKey_cpy(buf_buf_0,ctx_enckey,ctx_key);
  ecb3:
{
  }
  int _s_i;
  rcon = ((unsigned char )1);
/* Canonicalized from: for((_s_i =((unsigned char )1) , rcon =((unsigned char )1));((int )_s_i) < 14;++_s_i) {...} */
  for (_s_i = ((int )((unsigned char )1)); _s_i <= 13; ++_s_i) {
    aes_subBytes(buf_buf_0);
    aes_shiftRows(buf_buf_0);
    aes_mixColumns(buf_buf_0);
    if (((int )_s_i) & 1) {
      aes_addRoundKey(buf_buf_0,&ctx_key[16]);
    }
     else {
      aes_expandEncKey(ctx_key,&rcon);
      aes_addRoundKey(buf_buf_0,ctx_key);
    }
  }
  aes_subBytes(buf_buf_0);
  aes_shiftRows(buf_buf_0);
  aes_expandEncKey(ctx_key,&rcon);
  aes_addRoundKey(buf_buf_0,ctx_key);
  long _memcpy_i_2;
  
#pragma unroll 16
  for (_memcpy_i_2 = 0; _memcpy_i_2 < sizeof(unsigned char ) * ((unsigned long )16) / 1; ++_memcpy_i_2) {
    long total_offset1 = (0 * 16 + 0);
    long total_offset2 = (0 * 16 + 0);
    buf[total_offset1 + _memcpy_i_2] = buf_buf_0[total_offset2 + _memcpy_i_2];
  }
/* aes256_encrypt */
}
