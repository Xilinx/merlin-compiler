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
#include "merlin_type_define.h"
static void __merlin_dummy_extern_int_merlin_include_G_();

static unsigned char rj_sbox(int t)
{
  const unsigned char sbox[256] = {((unsigned char )0x63), ((unsigned char )0x7c), ((unsigned char )0x77), ((unsigned char )0x7b), ((unsigned char )0xf2), ((unsigned char )0x6b), ((unsigned char )0x6f), ((unsigned char )0xc5), ((unsigned char )0x30), ((unsigned char )0x01), ((unsigned char )0x67), ((unsigned char )0x2b), ((unsigned char )0xfe), ((unsigned char )0xd7), ((unsigned char )0xab), ((unsigned char )0x76), ((unsigned char )0xca), ((unsigned char )0x82), ((unsigned char )0xc9), ((unsigned char )0x7d), ((unsigned char )0xfa), ((unsigned char )0x59), ((unsigned char )0x47), ((unsigned char )0xf0), ((unsigned char )0xad), ((unsigned char )0xd4), ((unsigned char )0xa2), ((unsigned char )0xaf), ((unsigned char )0x9c), ((unsigned char )0xa4), ((unsigned char )0x72), ((unsigned char )0xc0), ((unsigned char )0xb7), ((unsigned char )0xfd), ((unsigned char )0x93), ((unsigned char )0x26), ((unsigned char )0x36), ((unsigned char )0x3f), ((unsigned char )0xf7), ((unsigned char )0xcc), ((unsigned char )0x34), ((unsigned char )0xa5), ((unsigned char )0xe5), ((unsigned char )0xf1), ((unsigned char )0x71), ((unsigned char )0xd8), ((unsigned char )0x31), ((unsigned char )0x15), ((unsigned char )0x04), ((unsigned char )0xc7), ((unsigned char )0x23), ((unsigned char )0xc3), ((unsigned char )0x18), ((unsigned char )0x96), ((unsigned char )0x05), ((unsigned char )0x9a), ((unsigned char )0x07), ((unsigned char )0x12), ((unsigned char )0x80), ((unsigned char )0xe2), ((unsigned char )0xeb), ((unsigned char )0x27), ((unsigned char )0xb2), ((unsigned char )0x75), ((unsigned char )0x09), ((unsigned char )0x83), ((unsigned char )0x2c), ((unsigned char )0x1a), ((unsigned char )0x1b), ((unsigned char )0x6e), ((unsigned char )0x5a), ((unsigned char )0xa0), ((unsigned char )0x52), ((unsigned char )0x3b), ((unsigned char )0xd6), ((unsigned char )0xb3), ((unsigned char )0x29), ((unsigned char )0xe3), ((unsigned char )0x2f), ((unsigned char )0x84), ((unsigned char )0x53), ((unsigned char )0xd1), ((unsigned char )0x00), ((unsigned char )0xed), ((unsigned char )0x20), ((unsigned char )0xfc), ((unsigned char )0xb1), ((unsigned char )0x5b), ((unsigned char )0x6a), ((unsigned char )0xcb), ((unsigned char )0xbe), ((unsigned char )0x39), ((unsigned char )0x4a), ((unsigned char )0x4c), ((unsigned char )0x58), ((unsigned char )0xcf), ((unsigned char )0xd0), ((unsigned char )0xef), ((unsigned char )0xaa), ((unsigned char )0xfb), ((unsigned char )0x43), ((unsigned char )0x4d), ((unsigned char )0x33), ((unsigned char )0x85), ((unsigned char )0x45), ((unsigned char )0xf9), ((unsigned char )0x02), ((unsigned char )0x7f), ((unsigned char )0x50), ((unsigned char )0x3c), ((unsigned char )0x9f), ((unsigned char )0xa8), ((unsigned char )0x51), ((unsigned char )0xa3), ((unsigned char )0x40), ((unsigned char )0x8f), ((unsigned char )0x92), ((unsigned char )0x9d), ((unsigned char )0x38), ((unsigned char )0xf5), ((unsigned char )0xbc), ((unsigned char )0xb6), ((unsigned char )0xda), ((unsigned char )0x21), ((unsigned char )0x10), ((unsigned char )0xff), ((unsigned char )0xf3), ((unsigned char )0xd2), ((unsigned char )0xcd), ((unsigned char )0x0c), ((unsigned char )0x13), ((unsigned char )0xec), ((unsigned char )0x5f), ((unsigned char )0x97), ((unsigned char )0x44), ((unsigned char )0x17), ((unsigned char )0xc4), ((unsigned char )0xa7), ((unsigned char )0x7e), ((unsigned char )0x3d), ((unsigned char )0x64), ((unsigned char )0x5d), ((unsigned char )0x19), ((unsigned char )0x73), ((unsigned char )0x60), ((unsigned char )0x81), ((unsigned char )0x4f), ((unsigned char )0xdc), ((unsigned char )0x22), ((unsigned char )0x2a), ((unsigned char )0x90), ((unsigned char )0x88), ((unsigned char )0x46), ((unsigned char )0xee), ((unsigned char )0xb8), ((unsigned char )0x14), ((unsigned char )0xde), ((unsigned char )0x5e), ((unsigned char )0x0b), ((unsigned char )0xdb), ((unsigned char )0xe0), ((unsigned char )0x32), ((unsigned char )0x3a), ((unsigned char )0x0a), ((unsigned char )0x49), ((unsigned char )0x06), ((unsigned char )0x24), ((unsigned char )0x5c), ((unsigned char )0xc2), ((unsigned char )0xd3), ((unsigned char )0xac), ((unsigned char )0x62), ((unsigned char )0x91), ((unsigned char )0x95), ((unsigned char )0xe4), ((unsigned char )0x79), ((unsigned char )0xe7), ((unsigned char )0xc8), ((unsigned char )0x37), ((unsigned char )0x6d), ((unsigned char )0x8d), ((unsigned char )0xd5), ((unsigned char )0x4e), ((unsigned char )0xa9), ((unsigned char )0x6c), ((unsigned char )0x56), ((unsigned char )0xf4), ((unsigned char )0xea), ((unsigned char )0x65), ((unsigned char )0x7a), ((unsigned char )0xae), ((unsigned char )0x08), ((unsigned char )0xba), ((unsigned char )0x78), ((unsigned char )0x25), ((unsigned char )0x2e), ((unsigned char )0x1c), ((unsigned char )0xa6), ((unsigned char )0xb4), ((unsigned char )0xc6), ((unsigned char )0xe8), ((unsigned char )0xdd), ((unsigned char )0x74), ((unsigned char )0x1f), ((unsigned char )0x4b), ((unsigned char )0xbd), ((unsigned char )0x8b), ((unsigned char )0x8a), ((unsigned char )0x70), ((unsigned char )0x3e), ((unsigned char )0xb5), ((unsigned char )0x66), ((unsigned char )0x48), ((unsigned char )0x03), ((unsigned char )0xf6), ((unsigned char )0x0e), ((unsigned char )0x61), ((unsigned char )0x35), ((unsigned char )0x57), ((unsigned char )0xb9), ((unsigned char )0x86), ((unsigned char )0xc1), ((unsigned char )0x1d), ((unsigned char )0x9e), ((unsigned char )0xe1), ((unsigned char )0xf8), ((unsigned char )0x98), ((unsigned char )0x11), ((unsigned char )0x69), ((unsigned char )0xd9), ((unsigned char )0x8e), ((unsigned char )0x94), ((unsigned char )0x9b), ((unsigned char )0x1e), ((unsigned char )0x87), ((unsigned char )0xe9), ((unsigned char )0xce), ((unsigned char )0x55), ((unsigned char )0x28), ((unsigned char )0xdf), ((unsigned char )0x8c), ((unsigned char )0xa1), ((unsigned char )0x89), ((unsigned char )0x0d), ((unsigned char )0xbf), ((unsigned char )0xe6), ((unsigned char )0x42), ((unsigned char )0x68), ((unsigned char )0x41), ((unsigned char )0x99), ((unsigned char )0x2d), ((unsigned char )0x0f), ((unsigned char )0xb0), ((unsigned char )0x54), ((unsigned char )0xbb), ((unsigned char )0x16)};
  return sbox[t];
}

static void aes_expandEncKey_1(unsigned char *k,unsigned char *rc)
{
  __merlin_access_range(k,0,31UL);
  k[0] ^= ((int )(rj_sbox((int )k[29]))) ^ ((int )( *rc));
  k[1] ^= ((int )(rj_sbox((int )k[30])));
  k[2] ^= ((int )(rj_sbox((int )k[31])));
  k[3] ^= ((int )(rj_sbox((int )k[28])));
   *rc = ((unsigned char )(((int )( *rc)) << 1 ^ (((int )( *rc)) >> 7 & 1) * 0x1b));
  int _s_i_0;
  exp1:
// Canonicalized from: for(_s_i_0 =((unsigned char )4);((int )_s_i_0) < 16;_s_i_0 += 4) {...}
// Standardize from: for(_s_i_0 =((unsigned char )4);_s_i_0 <= 15;_s_i_0 += 4) {...}
  for (_s_i_0 = 0; _s_i_0 <= 2; _s_i_0++) {
    int _in_s_i_0 = 4 + ((int )4LL) * _s_i_0;
    (((k[_in_s_i_0] ^= ((int )k[((int )_in_s_i_0) - 4]) , k[((int )_in_s_i_0) + 1] ^= ((int )k[((int )_in_s_i_0) - 3])) , k[((int )_in_s_i_0) + 2] ^= ((int )k[((int )_in_s_i_0) - 2])) , k[((int )_in_s_i_0) + 3] ^= ((int )k[((int )_in_s_i_0) - 1]));
  }
  _s_i_0 = 12 + ((int )4LL);
  k[16] ^= ((int )(rj_sbox((int )k[12])));
  k[17] ^= ((int )(rj_sbox((int )k[13])));
  k[18] ^= ((int )(rj_sbox((int )k[14])));
  k[19] ^= ((int )(rj_sbox((int )k[15])));
  int _s_i;
  exp2:
// Canonicalized from: for(_s_i =((unsigned char )20);((int )_s_i) < 32;_s_i += 4) {...}
// Standardize from: for(_s_i =((unsigned char )20);_s_i <= 31;_s_i += 4) {...}
  for (_s_i = 0; _s_i <= 2; _s_i++) {
    int _in_s_i = 20 + ((int )4LL) * _s_i;
    (((k[_in_s_i] ^= ((int )k[((int )_in_s_i) - 4]) , k[((int )_in_s_i) + 1] ^= ((int )k[((int )_in_s_i) - 3])) , k[((int )_in_s_i) + 2] ^= ((int )k[((int )_in_s_i) - 2])) , k[((int )_in_s_i) + 3] ^= ((int )k[((int )_in_s_i) - 1]));
  }
  _s_i = 28 + ((int )4LL);
/* aes_expandEncKey */
}

static void aes_expandEncKey_2(unsigned char *k,unsigned char *rc)
{
  __merlin_access_range(k,0,31UL);
  k[0] ^= ((int )(rj_sbox((int )k[29]))) ^ ((int )( *rc));
  k[1] ^= ((int )(rj_sbox((int )k[30])));
  k[2] ^= ((int )(rj_sbox((int )k[31])));
  k[3] ^= ((int )(rj_sbox((int )k[28])));
   *rc = ((unsigned char )(((int )( *rc)) << 1 ^ (((int )( *rc)) >> 7 & 1) * 0x1b));
  int _s_i_0;
  exp1:
// Canonicalized from: for(_s_i_0 =((unsigned char )4);((int )_s_i_0) < 16;_s_i_0 += 4) {...}
// Standardize from: for(_s_i_0 =((unsigned char )4);_s_i_0 <= 15;_s_i_0 += 4) {...}
  for (_s_i_0 = 0; _s_i_0 <= 2; _s_i_0++) {
    int _in_s_i_0 = 4 + ((int )4LL) * _s_i_0;
    (((k[_in_s_i_0] ^= ((int )k[((int )_in_s_i_0) - 4]) , k[((int )_in_s_i_0) + 1] ^= ((int )k[((int )_in_s_i_0) - 3])) , k[((int )_in_s_i_0) + 2] ^= ((int )k[((int )_in_s_i_0) - 2])) , k[((int )_in_s_i_0) + 3] ^= ((int )k[((int )_in_s_i_0) - 1]));
  }
  _s_i_0 = 12 + ((int )4LL);
  k[16] ^= ((int )(rj_sbox((int )k[12])));
  k[17] ^= ((int )(rj_sbox((int )k[13])));
  k[18] ^= ((int )(rj_sbox((int )k[14])));
  k[19] ^= ((int )(rj_sbox((int )k[15])));
  int _s_i;
  exp2:
// Canonicalized from: for(_s_i =((unsigned char )20);((int )_s_i) < 32;_s_i += 4) {...}
// Standardize from: for(_s_i =((unsigned char )20);_s_i <= 31;_s_i += 4) {...}
  for (_s_i = 0; _s_i <= 2; _s_i++) {
    int _in_s_i = 20 + ((int )4LL) * _s_i;
    (((k[_in_s_i] ^= ((int )k[((int )_in_s_i) - 4]) , k[((int )_in_s_i) + 1] ^= ((int )k[((int )_in_s_i) - 3])) , k[((int )_in_s_i) + 2] ^= ((int )k[((int )_in_s_i) - 2])) , k[((int )_in_s_i) + 3] ^= ((int )k[((int )_in_s_i) - 1]));
  }
  _s_i = 28 + ((int )4LL);
/* aes_expandEncKey */
}

static void aes_expandEncKey(unsigned char *k,unsigned char *rc)
{
  __merlin_access_range(k,0,31UL);
  k[0] ^= ((int )(rj_sbox((int )k[29]))) ^ ((int )( *rc));
  k[1] ^= ((int )(rj_sbox((int )k[30])));
  k[2] ^= ((int )(rj_sbox((int )k[31])));
  k[3] ^= ((int )(rj_sbox((int )k[28])));
   *rc = ((unsigned char )(((int )( *rc)) << 1 ^ (((int )( *rc)) >> 7 & 1) * 0x1b));
  int _s_i_0;
  exp1:
// Canonicalized from: for(_s_i_0 =((unsigned char )4);((int )_s_i_0) < 16;_s_i_0 += 4) {...}
// Standardize from: for(_s_i_0 =((unsigned char )4);_s_i_0 <= 15;_s_i_0 += 4) {...}
  for (_s_i_0 = 0; _s_i_0 <= 2; _s_i_0++) {
    int _in_s_i_0 = 4 + ((int )4LL) * _s_i_0;
    (((k[_in_s_i_0] ^= ((int )k[((int )_in_s_i_0) - 4]) , k[((int )_in_s_i_0) + 1] ^= ((int )k[((int )_in_s_i_0) - 3])) , k[((int )_in_s_i_0) + 2] ^= ((int )k[((int )_in_s_i_0) - 2])) , k[((int )_in_s_i_0) + 3] ^= ((int )k[((int )_in_s_i_0) - 1]));
  }
  _s_i_0 = 12 + ((int )4LL);
  k[16] ^= ((int )(rj_sbox((int )k[12])));
  k[17] ^= ((int )(rj_sbox((int )k[13])));
  k[18] ^= ((int )(rj_sbox((int )k[14])));
  k[19] ^= ((int )(rj_sbox((int )k[15])));
  int _s_i;
  exp2:
// Canonicalized from: for(_s_i =((unsigned char )20);((int )_s_i) < 32;_s_i += 4) {...}
// Standardize from: for(_s_i =((unsigned char )20);_s_i <= 31;_s_i += 4) {...}
  for (_s_i = 0; _s_i <= 2; _s_i++) {
    int _in_s_i = 20 + ((int )4LL) * _s_i;
    (((k[_in_s_i] ^= ((int )k[((int )_in_s_i) - 4]) , k[((int )_in_s_i) + 1] ^= ((int )k[((int )_in_s_i) - 3])) , k[((int )_in_s_i) + 2] ^= ((int )k[((int )_in_s_i) - 2])) , k[((int )_in_s_i) + 3] ^= ((int )k[((int )_in_s_i) - 1]));
  }
  _s_i = 28 + ((int )4LL);
/* aes_expandEncKey */
}

static void aes_addRoundKey_cpy(unsigned char *buf,unsigned char *key,unsigned char *cpk)
{
  __merlin_access_range(cpk,0,31UL);
  __merlin_access_range(key,0,31UL);
  __merlin_access_range(buf,0,15UL);
//register uint8_t i = 16;
  int i = 16;
  cpkey:
// Standardize from: for(i = 15;i >= 0;i--) {...}
  for (i = 0; i <= 15; i++) {
    int _in_i = 15 + ((int )(- 1)) * i;
    (buf[_in_i] ^= ((int )(cpk[_in_i] = key[_in_i])) , cpk[16 + _in_i] = key[16 + _in_i]);
  }
  i = 0 + ((int )(- 1));
/* aes_addRoundKey_cpy */
}

static void aes_subBytes(unsigned char *buf)
{
  __merlin_access_range(buf,0,15UL);
//register uint8_t i = 16;
  int i = 16;
//sub : while (i--) buf[i] = rj_sbox(buf[i]);
  sub:
// Standardize from: for(i = 15;i >= 0;i--) {...}
  for (i = 0; i <= 15; i++) {
    int _in_i = 15 + ((int )(- 1)) * i;
    buf[_in_i] = rj_sbox((int )buf[_in_i]);
  }
  i = 0 + ((int )(- 1));
/* aes_subBytes */
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
  if ((bool )(((int )x) & 0x80)) {
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
  int _s_i;
  mix:
// Canonicalized from: for(_s_i =((unsigned char )0);((int )_s_i) < 16;_s_i += 4) {...}
// Standardize from: for(_s_i =((unsigned char )0);_s_i <= 15;_s_i += 4) {...}
  for (_s_i = 0; _s_i <= 3; _s_i++) {
    int _in_s_i = 0 + ((int )4LL) * _s_i;
    a = buf[_in_s_i];
    b = buf[((int )_in_s_i) + 1];
    c = buf[((int )_in_s_i) + 2];
    d = buf[((int )_in_s_i) + 3];
    e = ((unsigned char )(((int )a) ^ ((int )b) ^ ((int )c) ^ ((int )d)));
    buf[_in_s_i] ^= ((int )e) ^ ((int )(rj_xtime((unsigned char )(((int )a) ^ ((int )b)))));
    buf[((int )_in_s_i) + 1] ^= ((int )e) ^ ((int )(rj_xtime((unsigned char )(((int )b) ^ ((int )c)))));
    buf[((int )_in_s_i) + 2] ^= ((int )e) ^ ((int )(rj_xtime((unsigned char )(((int )c) ^ ((int )d)))));
    buf[((int )_in_s_i) + 3] ^= ((int )e) ^ ((int )(rj_xtime((unsigned char )(((int )d) ^ ((int )a)))));
  }
  _s_i = 12 + ((int )4LL);
/* aes_mixColumns */
}

static void aes_addRoundKey(unsigned char *buf,unsigned char *key)
{
  __merlin_access_range(buf,0,15UL);
//register uint8_t i = 16;
  int i = 16;
//addkey : while (i--) buf[i] ^= key[i];
  addkey:
// Standardize from: for(i = 15;i >= 0;i--) {...}
  for (i = 0; i <= 15; i++) {
    int _in_i = 15 + ((int )(- 1)) * i;
    buf[_in_i] ^= ((int )key[_in_i]);
  }
  i = 0 + ((int )(- 1));
/* aes_addRoundKey */
}

static void aes256_encrypt_ecb(unsigned char k[32],unsigned char buf[16])
{
  __merlin_access_range(buf,0,15UL);
  __merlin_access_range(k,0,31UL);
  unsigned char ctx_deckey[32];
  unsigned char ctx_enckey[32];
  unsigned char ctx_key[32];
//INIT
  unsigned char rcon = (unsigned char )1;
  int _s_i_1;
  ecb1:
// Canonicalized from: for(_s_i_1 =((unsigned char )0);((unsigned long )_s_i_1) < sizeof(ctx . key);_s_i_1++) {...}
  for (_s_i_1 = ((unsigned char )0); _s_i_1 <= 31; ++_s_i_1) {
    ctx_enckey[_s_i_1] = ctx_deckey[_s_i_1] = k[_s_i_1];
  }
  int _s_i_0;
  ecb2:
// Canonicalized from: for(_s_i_0 =((unsigned char )8);((int )_s_i_0) > 1;--_s_i_0) {...}
// Standardize from: for(_s_i_0 =((unsigned char )8);_s_i_0 >= 2;--_s_i_0) {...}
  for (_s_i_0 = 0; _s_i_0 <= 6; _s_i_0++) {
    aes_expandEncKey(ctx_deckey,&rcon);
  }
  _s_i_0 = 2 + ((int )(- 1));
//DEC
  aes_addRoundKey_cpy(buf,ctx_enckey,ctx_key);
  int _s_i;
  rcon = ((unsigned char )1);
  ecb3:
// Canonicalized from: for((_s_i =((unsigned char )1) , rcon =((unsigned char )1));((int )_s_i) < 14;++_s_i) {...}
  for (_s_i = ((unsigned char )1); _s_i <= 13; ++_s_i) {
    aes_subBytes(buf);
    aes_shiftRows(buf);
    aes_mixColumns(buf);
    if ((bool )(((int )_s_i) & 1)) {
      aes_addRoundKey(buf,&ctx_key[16]);
    }
     else {
      aes_expandEncKey_1(ctx_key,&rcon);
      aes_addRoundKey(buf,ctx_key);
    }
  }
  aes_subBytes(buf);
  aes_shiftRows(buf);
  aes_expandEncKey_2(ctx_key,&rcon);
  aes_addRoundKey(buf,ctx_key);
/* aes256_encrypt */
}
static void __merlin_dummy_kernel_pragma();
// Original: #pragma ACCEL kernel

void aes256_encrypt_ecb_kernel(unsigned char key[32],unsigned char data[16 * (16 * 1024 * 1024)])
{
  __merlin_access_range(data,0,268435455UL);
  __merlin_access_range(key,0,31UL);
  
#pragma ACCEL interface variable=data  max_depth=268435456 depth=268435456
  
#pragma ACCEL interface variable=key  max_depth=32 depth=32
  
#pragma omp parallel for num_threads(16)
  for (int i1 = 0; i1 < 16 * 1024 * 1024 / (1024 * 4); i1++) {
    for (int i2 = 0; i2 < 1024 * 4; i2++) 
// Original: #pragma ACCEL false_dependence variable=data
{
      
#pragma ACCEL FALSE_DEPENDENCE VARIABLE=data
      int i = i1 * (1024 * 4) + i2;
      aes256_encrypt_ecb(key,data + i * 16);
    }
  }
}
