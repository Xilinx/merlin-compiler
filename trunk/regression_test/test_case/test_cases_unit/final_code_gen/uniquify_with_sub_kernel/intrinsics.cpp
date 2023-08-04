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
#include <assert.h>
#include "intrinsics.h"
// Enable this flag when checking functional correctness of SSE C models
//#define CHECK_INTRIN

struct m128i mm_setzero_si128()
{
  struct m128i x;
  for (int i = 0; i < 16; i++) {
    x . data[i] = 0;
  }
  return x;
}

struct m128i mm_insert_epi16(struct m128i a,const int i,const int imm8)
{
  struct m128i dst;
  imm8 >= 0 && imm8 <= 7?(static_cast < void  >  (0)) : __assert_fail("imm8 >= 0 && imm8 <= 7","intrinsics.cpp",32,__PRETTY_FUNCTION__);
  for (int j = 0; j < 16; j++) {
    dst . data[j] = a . data[j];
  }
  dst . data[imm8 * 2] = (i & 0xff);
  dst . data[imm8 * 2 + 1] = (i >> 8 & 0xff);
  return dst;
}

struct m128i mm_shufflelo_epi16(struct m128i a,int loc)
{
  loc == 0?(static_cast < void  >  (0)) : __assert_fail("loc == 0","intrinsics.cpp",56,__PRETTY_FUNCTION__);
  struct m128i dst;
// The logic for this function is actually a little bit annoying to implement.
// In bowtie2 alignNucleotides, it is only called with loc = 0.
// So, only the code for loc=0 is specified here.
  unsigned int x0 = a . data[0];
  unsigned int x1 = a . data[1];
  for (int i = 0; i < 8; i += 2) {
    dst . data[i] = x0;
    dst . data[i + 1] = x1;
  }
  for (int i = 8; i < 16; i++) {
    dst . data[i] = a . data[i];
  }
  return dst;
}

struct m128i mm_shuffle_epi32(struct m128i a,int loc)
{
  loc == 0?(static_cast < void  >  (0)) : __assert_fail("loc == 0","intrinsics.cpp",89,__PRETTY_FUNCTION__);
  struct m128i dst;
// This is also a cut-down intrinsic similar to shufflelo_epi16
  unsigned char x0 = a . data[0];
  unsigned char x1 = a . data[1];
  unsigned char x2 = a . data[2];
  unsigned char x3 = a . data[3];
  for (int i = 0; i < 16; i += 4) {
    dst . data[i] = x0;
    dst . data[i + 1] = x1;
    dst . data[i + 2] = x2;
    dst . data[i + 3] = x3;
  }
  return dst;
}

struct m128i mm_cmpeq_epi16(struct m128i a,struct m128i b)
{
  struct m128i dst;
  for (int i = 0; i < 16; i += 2) {
    unsigned char a_lo = a . data[i];
    unsigned char a_hi = a . data[i + 1];
    unsigned char b_lo = b . data[i];
    unsigned char b_hi = b . data[i + 1];
    unsigned short a_i = (a_lo | a_hi << 8);
    unsigned short b_i = (b_lo | b_hi << 8);
    if (a_i == b_i) {
      dst . data[i] = 0xff;
      dst . data[i + 1] = 0xff;
    }
     else {
      dst . data[i] = 0x00;
      dst . data[i + 1] = 0x00;
    }
  }
  return dst;
}

struct m128i mm_cmpgt_epi8(struct m128i a,struct m128i b)
{
  struct m128i dst;
  for (int i = 0; i < 16; i++) {
    unsigned char a_i = a . data[i];
    unsigned char b_i = b . data[i];
    dst . data[i] = ((a_i > b_i?0xff : 0));
  }
  return dst;
}

struct m128i mm_cmpgt_epi16(struct m128i a,struct m128i b)
{
  struct m128i dst;
  for (int i = 0; i < 16; i += 2) {
    unsigned short a_i = (a . data[i] | a . data[i + 1] << 8);
    unsigned short b_i = (b . data[i] | b . data[i + 1] << 8);
    if (a_i > b_i) {
      dst . data[i] = 0xff;
      dst . data[i + 1] = 0xff;
    }
     else {
      dst . data[i] = 0;
      dst . data[i + 1] = 0;
    }
  }
  return dst;
}

struct m128i mm_cmpeq_epi8(struct m128i a,struct m128i b)
{
  struct m128i dst;
  for (int i = 0; i < 16; i++) {
    if (a . data[i] == b . data[i]) 
      dst . data[i] = 0xff;
     else 
      dst . data[i] = 0x00;
  }
  return dst;
}

struct m128i mm_xor_si128(struct m128i a,struct m128i b)
{
  struct m128i dst;
  for (int i = 0; i < 16; i++) {
    dst . data[i] = (a . data[i] ^ b . data[i]);
  }
  return dst;
}

struct m128i mm_srli_si128(struct m128i a,int imm8)
{
  struct m128i dst;
  int tmp = imm8 & 0xff;
  if (tmp > 15) 
    tmp = 16;
  tmp >= 0 && tmp <= 16?(static_cast < void  >  (0)) : __assert_fail("tmp >= 0 && tmp <= 16","intrinsics.cpp",272,__PRETTY_FUNCTION__);
// fill in the bottom bits
  for (int i = 0; i < 16 - tmp; i++) {
    dst . data[i] = a . data[i + tmp];
  }
// fill in the top bits
  for (int i = 16 - tmp; i < 16; i++) {
    dst . data[i] = 0;
  }
  return dst;
}

void mm_store_si128(struct m128i *dst,struct m128i a)
{
  for (int i = 0; i < 16; i++) {
    dst -> data[i] = a . data[i];
  }
}

struct m128i mm_load_si128(struct m128i *src)
{
  struct m128i dst;
  for (int i = 0; i < 16; i++) {
    dst . data[i] = src -> data[i];
  }
  return dst;
}

struct m128i mm_slli_si128(struct m128i a,int imm8)
{
  struct m128i dst;
  int tmp = imm8 & 0xff;
  if (tmp > 15) 
    tmp = 16;
  tmp >= 0 && tmp <= 16?(static_cast < void  >  (0)) : __assert_fail("tmp >= 0 && tmp <= 16","intrinsics.cpp",345,__PRETTY_FUNCTION__);
// fill in the top bits
  for (int i = 0; i < 16 - tmp; i++) {
    dst . data[i + tmp] = a . data[i];
  }
// fill in the bottom bits
  for (int i = 0; i < tmp; i++) {
    dst . data[i] = 0;
  }
  return dst;
}

struct m128i mm_or_si128(struct m128i a,struct m128i b)
{
  struct m128i dst;
  for (int i = 0; i < 16; i++) {
    dst . data[i] = (a . data[i] | b . data[i]);
  }
  return dst;
}

struct m128i mm_subs_epu8(struct m128i a,struct m128i b)
{
  struct m128i dst;
  for (int i = 0; i < 16; i++) {
    unsigned char a_i = a . data[i];
    unsigned char b_i = b . data[i];
    unsigned char result = (a_i - b_i);
    if (result > a_i) 
      result = 0x00;
    dst . data[i] = result;
  }
  return dst;
}

struct m128i mm_subs_epi16(struct m128i a,struct m128i b)
{
  struct m128i dst;
  for (int i = 0; i < 16; i += 2) {
    short a_i = (a . data[i] | a . data[i + 1] << 8);
    short b_i = (b . data[i] | b . data[i + 1] << 8);
    short result = (a_i - b_i);
    if (result > a_i) 
      result = 0x0000;
    dst . data[i] = (result & 0xff);
    dst . data[i + 1] = (result >> 8 & 0xff);
  }
  return dst;
}

struct m128i mm_adds_epi16(struct m128i a,struct m128i b)
{
  struct m128i dst;
  for (int i = 0; i < 16; i += 2) {
    short a_i = (a . data[i] | a . data[i + 1] << 8);
    short b_i = (b . data[i] | b . data[i + 1] << 8);
    short dst_i = (a_i + b_i);
// saturation
    if (dst_i < a_i) 
      dst_i = 0xffff;
    dst . data[i] = (dst_i & 0xff);
    dst . data[i + 1] = (dst_i >> 8 & 0xff);
  }
  return dst;
}

struct m128i mm_adds_epu8(struct m128i a,struct m128i b)
{
  struct m128i dst;
  for (int i = 0; i < 16; i++) {
    unsigned char result = (a . data[i] + b . data[i]);
    if (result < a . data[i]) {
      result = 0xff;
    }
    dst . data[i] = result;
  }
  return dst;
}

struct m128i mm_max_epu8(struct m128i a,struct m128i b)
{
  struct m128i dst;
  for (int i = 0; i < 16; i++) {
    unsigned char a_i = a . data[i];
    unsigned char b_i = b . data[i];
    dst . data[i] = (a_i > b_i?a_i : b_i);
  }
  return dst;
}

struct m128i mm_max_epi16(struct m128i a,struct m128i b)
{
  struct m128i dst;
  for (int i = 0; i < 16; i += 2) {
    short a_i = (a . data[i] | a . data[i + 1] << 8);
    short b_i = (b . data[i] | b . data[i + 1] << 8);
    short result = a_i > b_i?a_i : b_i;
    dst . data[i] = (result & 0xff);
    dst . data[i + 1] = (result >> 8 & 0xff);
  }
  return dst;
}

unsigned int mm_movemask_epi8(struct m128i a)
{
  unsigned char mask[16];
  for (int i = 0; i < 16; i++) {
    mask[i] = (a . data[i] >> 7 & 0x1);
  }
  unsigned int mask_new = 0x0000;
  for (int i = 0; i < 16; i++) {
    mask_new |= (mask[i] << i);
  }
  return mask_new;
}
