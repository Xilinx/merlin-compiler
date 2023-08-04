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
const unsigned char sbox[256] = {(0x63), (0x7c), (0x77), (0x7b), (0xf2), (0x6b), (0x6f), (0xc5), (0x30), (0x01), (0x67), (0x2b), (0xfe), (0xd7), (0xab), (0x76), (0xca), (0x82), (0xc9), (0x7d), (0xfa), (0x59), (0x47), (0xf0), (0xad), (0xd4), (0xa2), (0xaf), (0x9c), (0xa4), (0x72), (0xc0), (0xb7), (0xfd), (0x93), (0x26), (0x36), (0x3f), (0xf7), (0xcc), (0x34), (0xa5), (0xe5), (0xf1), (0x71), (0xd8), (0x31), (0x15), (0x04), (0xc7), (0x23), (0xc3), (0x18), (0x96), (0x05), (0x9a), (0x07), (0x12), (0x80), (0xe2), (0xeb), (0x27), (0xb2), (0x75), (0x09), (0x83), (0x2c), (0x1a), (0x1b), (0x6e), (0x5a), (0xa0), (0x52), (0x3b), (0xd6), (0xb3), (0x29), (0xe3), (0x2f), (0x84), (0x53), (0xd1), (0x00), (0xed), (0x20), (0xfc), (0xb1), (0x5b), (0x6a), (0xcb), (0xbe), (0x39), (0x4a), (0x4c), (0x58), (0xcf), (0xd0), (0xef), (0xaa), (0xfb), (0x43), (0x4d), (0x33), (0x85), (0x45), (0xf9), (0x02), (0x7f), (0x50), (0x3c), (0x9f), (0xa8), (0x51), (0xa3), (0x40), (0x8f), (0x92), (0x9d), (0x38), (0xf5), (0xbc), (0xb6), (0xda), (0x21), (0x10), (0xff), (0xf3), (0xd2), (0xcd), (0x0c), (0x13), (0xec), (0x5f), (0x97), (0x44), (0x17), (0xc4), (0xa7), (0x7e), (0x3d), (0x64), (0x5d), (0x19), (0x73), (0x60), (0x81), (0x4f), (0xdc), (0x22), (0x2a), (0x90), (0x88), (0x46), (0xee), (0xb8), (0x14), (0xde), (0x5e), (0x0b), (0xdb), (0xe0), (0x32), (0x3a), (0x0a), (0x49), (0x06), (0x24), (0x5c), (0xc2), (0xd3), (0xac), (0x62), (0x91), (0x95), (0xe4), (0x79), (0xe7), (0xc8), (0x37), (0x6d), (0x8d), (0xd5), (0x4e), (0xa9), (0x6c), (0x56), (0xf4), (0xea), (0x65), (0x7a), (0xae), (0x08), (0xba), (0x78), (0x25), (0x2e), (0x1c), (0xa6), (0xb4), (0xc6), (0xe8), (0xdd), (0x74), (0x1f), (0x4b), (0xbd), (0x8b), (0x8a), (0x70), (0x3e), (0xb5), (0x66), (0x48), (0x03), (0xf6), (0x0e), (0x61), (0x35), (0x57), (0xb9), (0x86), (0xc1), (0x1d), (0x9e), (0xe1), (0xf8), (0x98), (0x11), (0x69), (0xd9), (0x8e), (0x94), (0x9b), (0x1e), (0x87), (0xe9), (0xce), (0x55), (0x28), (0xdf), (0x8c), (0xa1), (0x89), (0x0d), (0xbf), (0xe6), (0x42), (0x68), (0x41), (0x99), (0x2d), (0x0f), (0xb0), (0x54), (0xbb), (0x16)};

static void aes_subBytes(unsigned char *buf)
{
  __merlin_access_range(buf,0,15UL);
  register unsigned char i = (unsigned char )16;
  sub:
{
  }
  while(i--){
    buf[i] = sbox[buf[i]];
  }
/* aes_subBytes */
}

static void aes_addRoundKey(unsigned char *buf,unsigned char *key)
{
  __merlin_access_range(buf,0,15UL);
  register unsigned char i = (unsigned char )16;
  addkey:
{
  }
  while(i--){
    buf[i] ^= ((int )key[i]);
  }
/* aes_addRoundKey */
}

static void aes_addRoundKey_cpy(unsigned char *buf,unsigned char *key,unsigned char *cpk)
{
  __merlin_access_range(cpk,0,31UL);
  __merlin_access_range(key,0,31UL);
  __merlin_access_range(buf,0,15UL);
  register unsigned char i = (unsigned char )16;
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
  __merlin_access_range(buf,0,15UL);
/* to make it potentially parallelable :) */
  register unsigned char i;
  register unsigned char j;
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
  __merlin_access_range(buf,0,15UL);
  register unsigned char a;
  register unsigned char b;
  register unsigned char c;
  register unsigned char d;
  register unsigned char e;
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
    buf[_s_i * 4] ^= ((int )e) ^ ((int )(rj_xtime(((unsigned char )(((int )a) ^ ((int )b))))));
    buf[1 + _s_i * 4] ^= ((int )e) ^ ((int )(rj_xtime(((unsigned char )(((int )b) ^ ((int )c))))));
    buf[2 + _s_i * 4] ^= ((int )e) ^ ((int )(rj_xtime(((unsigned char )(((int )c) ^ ((int )d))))));
    buf[3 + _s_i * 4] ^= ((int )e) ^ ((int )(rj_xtime(((unsigned char )(((int )d) ^ ((int )a))))));
  }
  _s_i = 12 + ((int )4L);
/* aes_mixColumns */
}

#pragma ACCEL string 1234
static void aes_expandEncKey(unsigned char *k,unsigned char *rc)
{
  __merlin_access_range(k,0,31UL);
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

void aes256_encrypt_ecb_kernel(unsigned char *ctx_key,unsigned char *ctx_enckey,unsigned char *ctx_deckey,unsigned char k[32],unsigned char buf[16])
{
  unsigned char k_buf_0[32];
  long _memcpy_i_0;
  
#pragma unroll 32
  for (_memcpy_i_0 = 0; _memcpy_i_0 < (sizeof(unsigned char ) * ((unsigned long )32) / 1); ++_memcpy_i_0) {
    long total_offset1 = (0 * 32 + 0);
    long total_offset2 = (0 * 32 + 0);
    k_buf_0[total_offset1 + _memcpy_i_0] = k[total_offset2 + _memcpy_i_0];
  }
  unsigned char buf_buf_0[16];
  long _memcpy_i_1;
  
#pragma unroll 16
  for (_memcpy_i_1 = 0; _memcpy_i_1 < (sizeof(unsigned char ) * ((unsigned long )16) / 1); ++_memcpy_i_1) {
    long total_offset1 = (0 * 16 + 0);
    long total_offset2 = (0 * 16 + 0);
    buf_buf_0[total_offset1 + _memcpy_i_1] = buf[total_offset2 + _memcpy_i_1];
  }
  __merlin_access_range(buf,0,15UL);
  __merlin_access_range(k,0,31UL);
  __merlin_access_range(ctx_deckey,0,31UL);
  __merlin_access_range(ctx_enckey,0,31UL);
  __merlin_access_range(ctx_key,0,31UL);
  
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
  for (_memcpy_i_2 = 0; _memcpy_i_2 < (sizeof(unsigned char ) * ((unsigned long )16) / 1); ++_memcpy_i_2) {
    long total_offset1 = (0 * 16 + 0);
    long total_offset2 = (0 * 16 + 0);
    buf[total_offset1 + _memcpy_i_2] = buf_buf_0[total_offset2 + _memcpy_i_2];
  }
/* aes256_encrypt */
}
