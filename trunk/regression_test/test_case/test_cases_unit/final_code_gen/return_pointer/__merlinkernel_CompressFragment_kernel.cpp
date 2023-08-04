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
#include <assert.h>
#include <string.h>
#include "merlin_type_define.h"
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();

static int p_ZN4Bits9Log2FloorEj(unsigned int n)
{
  if (n == ((unsigned int )0)) {
    return - 1;
  }
  int log = 0;
  unsigned int value = n;
// Standardize from: for(int i = 4;i >= 0;--i) {...}
  
#pragma unroll
  for (int i = 0; i <= 4; i++) {
    int _in_i = 4 + ((int )(- 1)) * i;
    int shift = 1 << _in_i;
    unsigned int x = value >> shift;
    if (x != ((unsigned int )0)) {
      value = x;
      log += shift;
    }
  }
  value == ((unsigned int )1)?((void )0) : __assert_fail("value == 1","CompressFragment_kernel.cpp",(unsigned int )241,__PRETTY_FUNCTION__);
  return log;
}

static unsigned int HashBytes(unsigned int bytes,int shift)
{
  unsigned int kMul = (unsigned int )0x1e35a7bd;
  return bytes * 506832829 >> shift;
}

static unsigned int Hash_1(const char *p,int shift)
{
  return HashBytes( *((unsigned int *)p),shift);
//return HashBytes(UNALIGNED_LOAD32(p), shift);
}

static unsigned int Hash_2(const char *p,int shift)
{
  return HashBytes( *((unsigned int *)p),shift);
//return HashBytes(UNALIGNED_LOAD32(p), shift);
}

static unsigned int Hash(const char *p,int shift)
{
  return HashBytes( *((unsigned int *)p),shift);
//return HashBytes(UNALIGNED_LOAD32(p), shift);
}

static char *EmitLiteral_1(char *op,const char *literal,int len,char allow_fast_path)
{
  char allow_fast_path_copy = 0;
// The vast majority of copies are below 16 bytes, for which a
// call to memcpy is overkill. This fast path can sometimes
// copy up to 15 bytes too much, but that is okay in the
// main loop, since we have a bit to go on for both sides:
//
//   - The input will always have kInputMarginBytes = 15 extra
//     available bytes, as long as we're in the main loop, and
//     if not, allow_fast_path = false.
//   - The output will always have 32 spare bytes (see
//     MaxCompressedLength).
// Zero-length literals are disallowed
  len > 0?((void )0) : __assert_fail("len > 0","CompressFragment_kernel.cpp",(unsigned int )567,__PRETTY_FUNCTION__);
  int n = len - 1;
  if (((bool )0) && len <= 16) {
// Fits in tag byte
     *(op++) = ((char )(((int )LITERAL) | n << 2));
// WORKAROUND: Merlin doesn't like this function
    long _memcpy_i_2;
    
#pragma ivdep array(op)
    
#pragma ivdep array(literal)
    for (_memcpy_i_2 = 0; _memcpy_i_2 < (((unsigned long )16) / 1); ++_memcpy_i_2) {
      long total_offset1 = (0 * 0 + 0);
      long total_offset2 = (0 * 0 + 0);
      op[total_offset1 + _memcpy_i_2] = literal[total_offset2 + _memcpy_i_2];
    }
//UnalignedCopy128(literal, op);
    return op + len;
  }
  if (n < 60) {
// Fits in tag byte
     *(op++) = ((char )(((int )LITERAL) | n << 2));
  }
   else {
// Encode in upcoming bytes
    char *base = op;
    int count = 0;
    op++;
    while(n > 0){
       *(op++) = ((char )(n & 0xff));
      n >>= 8;
      count++;
    }
    count >= 1?((void )0) : __assert_fail("count >= 1","CompressFragment_kernel.cpp",(unsigned int )592,__PRETTY_FUNCTION__);
    count <= 4?((void )0) : __assert_fail("count <= 4","CompressFragment_kernel.cpp",(unsigned int )593,__PRETTY_FUNCTION__);
     *base = ((char )(((int )LITERAL) | 59 + count << 2));
  }
  long _memcpy_i_3;
  
#pragma ivdep array(op)
  
#pragma ivdep array(literal)
  for (_memcpy_i_3 = 0; _memcpy_i_3 < (((unsigned long )len) / 1); ++_memcpy_i_3) {
    long total_offset1 = (0 * 0 + 0);
    long total_offset2 = (0 * 0 + 0);
    op[total_offset1 + _memcpy_i_3] = literal[total_offset2 + _memcpy_i_3];
  }
  return op + len;
}

static char *EmitLiteral(char *op,const char *literal,int len,char allow_fast_path)
{
  char allow_fast_path_copy = 1;
  int len_copy = 0;
// The vast majority of copies are below 16 bytes, for which a
// call to memcpy is overkill. This fast path can sometimes
// copy up to 15 bytes too much, but that is okay in the
// main loop, since we have a bit to go on for both sides:
//
//   - The input will always have kInputMarginBytes = 15 extra
//     available bytes, as long as we're in the main loop, and
//     if not, allow_fast_path = false.
//   - The output will always have 32 spare bytes (see
//     MaxCompressedLength).
// Zero-length literals are disallowed
  0 > 0?((void )0) : __assert_fail("len > 0","CompressFragment_kernel.cpp",(unsigned int )567,__PRETTY_FUNCTION__);
  int n = 0 - 1;
  if (((bool )1) && 0 <= 16) {
// Fits in tag byte
     *(op++) = ((char )(((int )LITERAL) | n << 2));
// WORKAROUND: Merlin doesn't like this function
    long _memcpy_i_0;
    
#pragma ivdep array(op)
    
#pragma ivdep array(literal)
    for (_memcpy_i_0 = 0; _memcpy_i_0 < (((unsigned long )16) / 1); ++_memcpy_i_0) {
      long total_offset1 = (0 * 0 + 0);
      long total_offset2 = (0 * 0 + 0);
      op[total_offset1 + _memcpy_i_0] = literal[total_offset2 + _memcpy_i_0];
    }
//UnalignedCopy128(literal, op);
    return op + 0;
  }
  if (n < 60) {
// Fits in tag byte
     *(op++) = ((char )(((int )LITERAL) | n << 2));
  }
   else {
// Encode in upcoming bytes
    char *base = op;
    int count = 0;
    op++;
    while(n > 0){
       *(op++) = ((char )(n & 0xff));
      n >>= 8;
      count++;
    }
    count >= 1?((void )0) : __assert_fail("count >= 1","CompressFragment_kernel.cpp",(unsigned int )592,__PRETTY_FUNCTION__);
    count <= 4?((void )0) : __assert_fail("count <= 4","CompressFragment_kernel.cpp",(unsigned int )593,__PRETTY_FUNCTION__);
     *base = ((char )(((int )LITERAL) | 59 + count << 2));
  }
  long _memcpy_i_1;
  
#pragma ivdep array(op)
  
#pragma ivdep array(literal)
  for (_memcpy_i_1 = 0; _memcpy_i_1 < (((unsigned long )0) / 1); ++_memcpy_i_1) {
    long total_offset1 = (0 * 0 + 0);
    long total_offset2 = (0 * 0 + 0);
    op[total_offset1 + _memcpy_i_1] = literal[total_offset2 + _memcpy_i_1];
  }
  return op + 0;
}

static bool IsLittleEndian()
{
  return true;
}

static int p_ZN4Bits17FindLSBSetNonZeroEj(unsigned int n)
{
  int rc = 31;
  int _l_i = 4;
  int _l_shift = 16;
// Canonicalized from: for(int i = 4, shift = 1 << 4;i >= 0;--i) {...}
// Standardize from: for(_l_i = 4;_l_i >= 0;--_l_i) {...}
  
#pragma unroll
  for (_l_i = 0; _l_i <= 4; _l_i++) {
    const unsigned int x = n << _l_shift;
    if (x != ((unsigned int )0)) {
      n = x;
      rc -= _l_shift;
    }
    _l_shift >>= 1;
  }
  _l_i = 0 + ((int )(- 1));
  return rc;
}

void merlin_assign_pair_1(unsigned long *lhs_first,char *lhs_second,unsigned long rhs_first,char rhs_second)
{
   *lhs_first = rhs_first;
   *lhs_second = ((bool )rhs_second);
}

static void FindMatchLength(unsigned long *ret_var_first,char *ret_var_second,const char *s1,const char *s2,const char *s2_limit)
{
// Implementation based on the x86-64 version, above.
  s2_limit >= s2?((void )0) : __assert_fail("s2_limit >= s2","CompressFragment_kernel.cpp",(unsigned int )341,__PRETTY_FUNCTION__);
  int matched = 0;
  while(s2 <= s2_limit - 4 &&  *((unsigned int *)s2) ==  *((unsigned int *)(s2 + matched))){
//         UNALIGNED_LOAD32(s2) == UNALIGNED_LOAD32(s1 + matched)) {
    s2 += 4;
    matched += 4;
  }
  if (IsLittleEndian() && s2 <= s2_limit - 4) {
    unsigned int x =  *((unsigned int *)s2) ^  *((unsigned int *)(s1 + matched));
//    uint32 x = UNALIGNED_LOAD32(s2) ^ UNALIGNED_LOAD32(s1 + matched);
    int matching_bits = p_ZN4Bits17FindLSBSetNonZeroEj(x);
    matched += matching_bits >> 3;
  }
   else {
    while(s2 < s2_limit && ((int )s1[matched]) == ((int )( *s2))){
      ++s2;
      ++matched;
    }
  }
  char ret_second;
  unsigned long ret_first;
  ret_first = ((unsigned long )matched);
  ret_second = (matched < 8);
  merlin_assign_pair_1(&( *ret_var_first),&( *ret_var_second),ret_first,((bool )ret_second));
  return ;
}

static int my_memcmp(const void *s1,const void *s2,unsigned long n)
{
  const unsigned char *p1 = (const unsigned char *)s1;
  const unsigned char *p2 = (const unsigned char *)s2;
  while((bool )(n--)){
    if (((int )( *p1)) != ((int )( *p2))) {
      return ((int )( *p1)) - ((int )( *p2));
    }
     else {
      (p1++ , p2++);
    }
  }
  return 0;
}

static unsigned int bswap_u32(unsigned int num)
{
  unsigned int swapped;
// move byte 3 to byte 0
  swapped = num >> 24 & ((unsigned int )0xff) | num << 8 & ((unsigned int )0xff0000) | num >> 8 & ((unsigned int )0xff00) | num << 24 & 0xff000000;
// move byte 1 to byte 2
// move byte 2 to byte 1
// byte 0 to byte 3
  return swapped;
}

static char *EmitCopyAtMost64_1(char *op,unsigned long offset,unsigned long len,char len_less_than_12)
{
  char len_less_than_12_copy = 0;
  unsigned long len_copy = 64;
  64 <= ((unsigned long )64)?((void )0) : __assert_fail("len <= 64","CompressFragment_kernel.cpp",(unsigned int )602,__PRETTY_FUNCTION__);
  64 >= ((unsigned long )4)?((void )0) : __assert_fail("len >= 4","CompressFragment_kernel.cpp",(unsigned int )603,__PRETTY_FUNCTION__);
  offset < ((unsigned long )65536)?((void )0) : __assert_fail("offset < 65536","CompressFragment_kernel.cpp",(unsigned int )604,__PRETTY_FUNCTION__);
  ((int )((bool )0)) == ((int )(64 < ((unsigned long )12)))?((void )0) : __assert_fail("len_less_than_12 == (len < 12)","CompressFragment_kernel.cpp",(unsigned int )605,__PRETTY_FUNCTION__);
  if (((bool )0) && offset < ((unsigned long )2048)) {
// offset fits in 11 bits.  The 3 highest go in the top of the first byte,
// and the rest go in the second byte.
     *(op++) = ((char )(((unsigned long )COPY_1_BYTE_OFFSET) + (64 - ((unsigned long )4) << 2) + (offset >> 3 & ((unsigned long )0xe0))));
     *(op++) = ((char )(offset & ((unsigned long )0xff)));
  }
   else {
// Write 4 bytes, though we only care about 3 of them.  The output buffer
// is required to have some slack, so the extra byte won't overrun it.
    unsigned int u = (unsigned int )(((unsigned long )COPY_2_BYTE_OFFSET) + (64 - ((unsigned long )1) << 2) + (offset << 8));
//LittleEndian le;
//le.Store32(op, u);
    if (IsLittleEndian()) {
       *((unsigned int *)op) = u;
//memcpy(op, &u, sizeof u);
    }
     else {
      u = bswap_u32(u);
       *((unsigned int *)op) = u;
//memcpy(op, &u, sizeof u);
    }
    op += 3;
  }
  return op;
}

static char *EmitCopyAtMost64_2(char *op,unsigned long offset,unsigned long len,char len_less_than_12)
{
  char len_less_than_12_copy = 0;
  unsigned long len_copy = 60;
  60 <= ((unsigned long )64)?((void )0) : __assert_fail("len <= 64","CompressFragment_kernel.cpp",(unsigned int )602,__PRETTY_FUNCTION__);
  60 >= ((unsigned long )4)?((void )0) : __assert_fail("len >= 4","CompressFragment_kernel.cpp",(unsigned int )603,__PRETTY_FUNCTION__);
  offset < ((unsigned long )65536)?((void )0) : __assert_fail("offset < 65536","CompressFragment_kernel.cpp",(unsigned int )604,__PRETTY_FUNCTION__);
  ((int )((bool )0)) == ((int )(60 < ((unsigned long )12)))?((void )0) : __assert_fail("len_less_than_12 == (len < 12)","CompressFragment_kernel.cpp",(unsigned int )605,__PRETTY_FUNCTION__);
  if (((bool )0) && offset < ((unsigned long )2048)) {
// offset fits in 11 bits.  The 3 highest go in the top of the first byte,
// and the rest go in the second byte.
     *(op++) = ((char )(((unsigned long )COPY_1_BYTE_OFFSET) + (60 - ((unsigned long )4) << 2) + (offset >> 3 & ((unsigned long )0xe0))));
     *(op++) = ((char )(offset & ((unsigned long )0xff)));
  }
   else {
// Write 4 bytes, though we only care about 3 of them.  The output buffer
// is required to have some slack, so the extra byte won't overrun it.
    unsigned int u = (unsigned int )(((unsigned long )COPY_2_BYTE_OFFSET) + (60 - ((unsigned long )1) << 2) + (offset << 8));
//LittleEndian le;
//le.Store32(op, u);
    if (IsLittleEndian()) {
       *((unsigned int *)op) = u;
//memcpy(op, &u, sizeof u);
    }
     else {
      u = bswap_u32(u);
       *((unsigned int *)op) = u;
//memcpy(op, &u, sizeof u);
    }
    op += 3;
  }
  return op;
}

static char *EmitCopyAtMost64_3(char *op,unsigned long offset,unsigned long len,char len_less_than_12)
{
  len <= ((unsigned long )64)?((void )0) : __assert_fail("len <= 64","CompressFragment_kernel.cpp",(unsigned int )602,__PRETTY_FUNCTION__);
  len >= ((unsigned long )4)?((void )0) : __assert_fail("len >= 4","CompressFragment_kernel.cpp",(unsigned int )603,__PRETTY_FUNCTION__);
  offset < ((unsigned long )65536)?((void )0) : __assert_fail("offset < 65536","CompressFragment_kernel.cpp",(unsigned int )604,__PRETTY_FUNCTION__);
  ((int )((bool )len_less_than_12)) == ((int )(len < ((unsigned long )12)))?((void )0) : __assert_fail("len_less_than_12 == (len < 12)","CompressFragment_kernel.cpp",(unsigned int )605,__PRETTY_FUNCTION__);
  if (((bool )len_less_than_12) && offset < ((unsigned long )2048)) {
// offset fits in 11 bits.  The 3 highest go in the top of the first byte,
// and the rest go in the second byte.
     *(op++) = ((char )(((unsigned long )COPY_1_BYTE_OFFSET) + (len - ((unsigned long )4) << 2) + (offset >> 3 & ((unsigned long )0xe0))));
     *(op++) = ((char )(offset & ((unsigned long )0xff)));
  }
   else {
// Write 4 bytes, though we only care about 3 of them.  The output buffer
// is required to have some slack, so the extra byte won't overrun it.
    unsigned int u = (unsigned int )(((unsigned long )COPY_2_BYTE_OFFSET) + (len - ((unsigned long )1) << 2) + (offset << 8));
//LittleEndian le;
//le.Store32(op, u);
    if (IsLittleEndian()) {
       *((unsigned int *)op) = u;
//memcpy(op, &u, sizeof u);
    }
     else {
      u = bswap_u32(u);
       *((unsigned int *)op) = u;
//memcpy(op, &u, sizeof u);
    }
    op += 3;
  }
  return op;
}

static char *EmitCopyAtMost64(char *op,unsigned long offset,unsigned long len,char len_less_than_12)
{
  char len_less_than_12_copy = 1;
  len <= ((unsigned long )64)?((void )0) : __assert_fail("len <= 64","CompressFragment_kernel.cpp",(unsigned int )602,__PRETTY_FUNCTION__);
  len >= ((unsigned long )4)?((void )0) : __assert_fail("len >= 4","CompressFragment_kernel.cpp",(unsigned int )603,__PRETTY_FUNCTION__);
  offset < ((unsigned long )65536)?((void )0) : __assert_fail("offset < 65536","CompressFragment_kernel.cpp",(unsigned int )604,__PRETTY_FUNCTION__);
  ((int )((bool )1)) == ((int )(len < ((unsigned long )12)))?((void )0) : __assert_fail("len_less_than_12 == (len < 12)","CompressFragment_kernel.cpp",(unsigned int )605,__PRETTY_FUNCTION__);
  if (((bool )1) && offset < ((unsigned long )2048)) {
// offset fits in 11 bits.  The 3 highest go in the top of the first byte,
// and the rest go in the second byte.
     *(op++) = ((char )(((unsigned long )COPY_1_BYTE_OFFSET) + (len - ((unsigned long )4) << 2) + (offset >> 3 & ((unsigned long )0xe0))));
     *(op++) = ((char )(offset & ((unsigned long )0xff)));
  }
   else {
// Write 4 bytes, though we only care about 3 of them.  The output buffer
// is required to have some slack, so the extra byte won't overrun it.
    unsigned int u = (unsigned int )(((unsigned long )COPY_2_BYTE_OFFSET) + (len - ((unsigned long )1) << 2) + (offset << 8));
//LittleEndian le;
//le.Store32(op, u);
    if (IsLittleEndian()) {
       *((unsigned int *)op) = u;
//memcpy(op, &u, sizeof u);
    }
     else {
      u = bswap_u32(u);
       *((unsigned int *)op) = u;
//memcpy(op, &u, sizeof u);
    }
    op += 3;
  }
  return op;
}

static char *EmitCopy(char *op,unsigned long offset,unsigned long len,char len_less_than_12)
{
  ((int )((bool )len_less_than_12)) == ((int )(len < ((unsigned long )12)))?((void )0) : __assert_fail("len_less_than_12 == (len < 12)","CompressFragment_kernel.cpp",(unsigned int )634,__PRETTY_FUNCTION__);
  if ((bool )len_less_than_12) {
    return EmitCopyAtMost64(op,offset,len,true);
  }
   else {
// A special case for len <= 64 might help, but so far measurements suggest
// it's in the noise.
// Emit 64 byte copies but make sure to keep at least four bytes reserved.
    while(len >= ((unsigned long )68)){
      op = EmitCopyAtMost64_1(op,offset,(unsigned long )64,false);
      len -= ((unsigned long )64);
    }
// One or two copies will now finish the job.
    if (len > ((unsigned long )64)) {
      op = EmitCopyAtMost64_2(op,offset,(unsigned long )60,false);
      len -= ((unsigned long )60);
    }
// Emit remainder.
    op = EmitCopyAtMost64_3(op,offset,len,(len < ((unsigned long )12)));
    return op;
  }
}

static const char *GetEightBytesAt(const char *ptr)
{
  return ptr;
}

static unsigned int GetUint32AtOffset_1(const char *v,int offset)
{
  int offset_copy = 1;
  1 >= 0?((void )0) : __assert_fail("offset >= 0","CompressFragment_kernel.cpp",(unsigned int )691,__PRETTY_FUNCTION__);
  1 <= 4?((void )0) : __assert_fail("offset <= 4","CompressFragment_kernel.cpp",(unsigned int )692,__PRETTY_FUNCTION__);
  return  *((unsigned int *)(v + 1));
//return UNALIGNED_LOAD32(v + offset);
}

static unsigned int GetUint32AtOffset_2(const char *v,int offset)
{
  int offset_copy = 1;
  1 >= 0?((void )0) : __assert_fail("offset >= 0","CompressFragment_kernel.cpp",(unsigned int )691,__PRETTY_FUNCTION__);
  1 <= 4?((void )0) : __assert_fail("offset <= 4","CompressFragment_kernel.cpp",(unsigned int )692,__PRETTY_FUNCTION__);
  return  *((unsigned int *)(v + 1));
//return UNALIGNED_LOAD32(v + offset);
}

static unsigned int GetUint32AtOffset_3(const char *v,int offset)
{
  int offset_copy = 2;
  2 >= 0?((void )0) : __assert_fail("offset >= 0","CompressFragment_kernel.cpp",(unsigned int )691,__PRETTY_FUNCTION__);
  2 <= 4?((void )0) : __assert_fail("offset <= 4","CompressFragment_kernel.cpp",(unsigned int )692,__PRETTY_FUNCTION__);
  return  *((unsigned int *)(v + 2));
//return UNALIGNED_LOAD32(v + offset);
}

static unsigned int GetUint32AtOffset(const char *v,int offset)
{
  int offset_copy = 0;
  0 >= 0?((void )0) : __assert_fail("offset >= 0","CompressFragment_kernel.cpp",(unsigned int )691,__PRETTY_FUNCTION__);
  0 <= 4?((void )0) : __assert_fail("offset <= 4","CompressFragment_kernel.cpp",(unsigned int )692,__PRETTY_FUNCTION__);
  return  *((unsigned int *)(v + 0));
//return UNALIGNED_LOAD32(v + offset);
}
static const unsigned long kBlockSize = (unsigned long )(1 << 16);
static const unsigned int kuint32max = (unsigned int )0xFFFFFFFF;
static void __merlin_dummy_kernel_pragma();
// Original: #pragma ACCEL kernel name=CompressFrangment_kernel

void merlin_assign_pair_0(unsigned long *lhs_first,char *lhs_second,unsigned long rhs_first,char rhs_second)
{
   *lhs_first = rhs_first;
   *lhs_second = ((bool )rhs_second);
}

void CompressFragment_kernel(const char *merlin_input,unsigned long input_size,char *op,unsigned short *table,const int table_size,int merlin_return_value[1])
{
  __merlin_access_range(merlin_return_value,0,0UL);
  __merlin_access_range(table,0,255UL);
  __merlin_access_range(op,0,65535UL);
  __merlin_access_range(merlin_input,0,65535UL);
  
#pragma ACCEL interface variable=merlin_return_value io=WO scope_type=return max_depth=1 depth=1
  
#pragma ACCEL interface max_depth=65536 depth=65536 variable=merlin_input
  
#pragma ACCEL interface variable=op max_depth=65536 depth=65536
  
#pragma ACCEL interface variable=table max_depth=256 depth=256
// WORKAROUND: since Merlin doesn't support assignment to interface pointers creating a local buffer
  char op_lcl[65536];
  char *op_lcl1 = op_lcl;
// "ip" is the input pointer, and "op" is the output pointer.
  const char *ip = merlin_input;
  input_size <= kBlockSize?((void )0) : __assert_fail("input_size <= kBlockSize","CompressFragment_kernel.cpp",(unsigned int )729,__PRETTY_FUNCTION__);
// table must be power of two
  (table_size & table_size - 1) == 0?((void )0) : __assert_fail("(table_size & (table_size - 1)) == 0","CompressFragment_kernel.cpp",(unsigned int )730,__PRETTY_FUNCTION__);
  const int shift = 32 - p_ZN4Bits9Log2FloorEj((unsigned int )table_size);
  ((int )(kuint32max >> shift)) == table_size - 1?((void )0) : __assert_fail("static_cast<int>(kuint32max >> shift) == table_size - 1","CompressFragment_kernel.cpp",(unsigned int )733,__PRETTY_FUNCTION__);
// Bytes in [next_emit, ip) will be emitted as literal bytes.  Or
// [next_emit, ip_end) after the main loop.
  const char *next_emit = merlin_input;
  const unsigned long kInputMarginBytes = (unsigned long )15;
  if (input_size >= kInputMarginBytes) {
    
#pragma ACCEL auto_register variable=table_buf_0
    unsigned short table_buf_0[256];
    long _memcpy_i_1;
    
#pragma ivdep array(table)
    
#pragma ivdep array(table_buf_0)
    
#pragma unroll 32
    for (_memcpy_i_1 = 0; _memcpy_i_1 < (sizeof(unsigned short ) * ((unsigned long )256) / 2); ++_memcpy_i_1) {
      long total_offset1 = (0 * 256 + 0);
      long total_offset2 = (0 * 0 + 0);
      table_buf_0[total_offset1 + _memcpy_i_1] = table[total_offset2 + _memcpy_i_1];
    }
    for (unsigned int next_hash = Hash(++ip,shift); ; ) {
      next_emit < ip?((void )0) : __assert_fail("next_emit < ip","CompressFragment_kernel.cpp",(unsigned int )745,__PRETTY_FUNCTION__);
// The body of this loop calls EmitLiteral once and then EmitCopy one or
// more times.  (The exception is that when we're close to exhausting
// the input we goto emit_remainder.)
//
// In the first iteration of this loop we're just starting, so
// there's nothing to copy, so calling EmitLiteral once is
// necessary.  And we only start a new iteration when the
// current iteration has determined that a call to EmitLiteral will
// precede the next call to EmitCopy (if any).
//
// Step 1: Scan forward in the input looking for a 4-byte-long match.
// If we get close to exhausting the input then goto emit_remainder.
//
// Heuristic match skipping: If 32 bytes are scanned with no matches
// found, start looking only at every other byte. If 32 more bytes are
// scanned (or skipped), look at every third byte, etc.. When a match is
// found, immediately go back to looking at every byte. This is a small
// loss (~5% performance, ~0.1% density) for compressible data due to more
// bookkeeping, but for non-compressible data (such as JPEG) it's a huge
// win since the compressor quickly "realizes" the data is incompressible
// and doesn't bother looking for matches everywhere.
//
// The "skip" variable keeps track of how many bytes there are since the
// last match; dividing it by 32 (ie. right-shifting by five) gives the
// number of bytes to move ahead for each iteration.
      unsigned int skip = (unsigned int )32;
      const char *next_ip = ip;
      const char *candidate;
      do {
        ip = next_ip;
        unsigned int hash = next_hash;
        hash == Hash_1(ip,shift)?((void )0) : __assert_fail("hash == Hash(ip, shift)","CompressFragment_kernel.cpp",(unsigned int )778,__PRETTY_FUNCTION__);
        unsigned int bytes_between_hash_lookups = skip >> 5;
        skip += bytes_between_hash_lookups;
        next_ip = ip + bytes_between_hash_lookups;
        if (next_ip > merlin_input + input_size - kInputMarginBytes) {
          goto emit_remainder;
        }
        next_hash = Hash_2(next_ip,shift);
        candidate = merlin_input + table_buf_0[hash];
        candidate >= merlin_input?((void )0) : __assert_fail("candidate >= base_ip","CompressFragment_kernel.cpp",(unsigned int )787,__PRETTY_FUNCTION__);
        candidate < ip?((void )0) : __assert_fail("candidate < ip","CompressFragment_kernel.cpp",(unsigned int )788,__PRETTY_FUNCTION__);
        table_buf_0[hash] = ((unsigned short )(ip - merlin_input));
      }while ( *((unsigned int *)ip) !=  *((unsigned int *)candidate));
// while (SNAPPY_PREDICT_TRUE(UNALIGNED_LOAD32(ip) !=
//                           UNALIGNED_LOAD32(candidate)));
// Step 2: A 4-byte match has been found.  We'll later see if more
// than 4 bytes match.  But, prior to the match, input
// bytes [next_emit, ip) are unmatched.  Emit them as "literal bytes."
      next_emit + 16 <= merlin_input + input_size?((void )0) : __assert_fail("next_emit + 16 <= ip_end","CompressFragment_kernel.cpp",(unsigned int )800,__PRETTY_FUNCTION__);
      op_lcl1 = EmitLiteral(op_lcl1,next_emit,(int )(ip - next_emit),true);
// Step 3: Call EmitCopy, and then see if another EmitCopy could
// be our next move.  Repeat until we find no match for the
// input immediately after what was consumed by the last EmitCopy call.
//
// If we exit this loop normally then we need to call EmitLiteral next,
// though we don't yet know how big the literal will be.  We handle that
// by proceeding to the next iteration of the main loop.  We also can exit
// this loop via goto if we get close to exhausting the input.
      const char *input_bytes;
      unsigned int candidate_bytes = (unsigned int )0;
      do {
        char ret_var_second;
        unsigned long ret_var_first;
// We have a 4-byte match at ip, and no need to emit any
// "literal bytes" prior to ip.
        const char *base = ip;
        FindMatchLength(&ret_var_first,&ret_var_second,candidate + 4,ip + 4,merlin_input + input_size);
        char p_second;
        unsigned long p_first;
        merlin_assign_pair_0(&p_first,&p_second,ret_var_first,((bool )ret_var_second));
        unsigned long matched = ((unsigned long )4) + p_first;
        ip += matched;
        unsigned long offset = (unsigned long )(base - candidate);
// WORKAROUND: Merlin doesn't support memcmp yet so using custom function 
        0 == my_memcmp((const void *)base,(const void *)candidate,matched)?((void )0) : __assert_fail("0 == my_memcmp(base, candidate, matched)","CompressFragment_kernel.cpp",(unsigned int )825,__PRETTY_FUNCTION__);
        op_lcl1 = EmitCopy(op_lcl1,offset,matched,((bool )p_second));
        next_emit = ip;
        if (ip >= merlin_input + input_size - kInputMarginBytes) {
          goto emit_remainder;
        }
// We are now looking for a 4-byte match again.  We read
// table[Hash(ip, shift)] for that.  To improve compression,
// we also update table[Hash(ip - 1, shift)] and table[Hash(ip, shift)].
        input_bytes = GetEightBytesAt(ip - 1);
        unsigned int prev_hash = HashBytes((GetUint32AtOffset(input_bytes,0)),shift);
        table_buf_0[prev_hash] = ((unsigned short )(ip - merlin_input - ((long )1)));
        unsigned int cur_hash = HashBytes((GetUint32AtOffset_1(input_bytes,1)),shift);
        candidate = merlin_input + table_buf_0[cur_hash];
        candidate_bytes =  *((unsigned int *)candidate);
//candidate_bytes = UNALIGNED_LOAD32(candidate);
        table_buf_0[cur_hash] = ((unsigned short )(ip - merlin_input));
      }while (GetUint32AtOffset_2(input_bytes,1) == candidate_bytes);
      next_hash = HashBytes((GetUint32AtOffset_3(input_bytes,2)),shift);
      ++ip;
    }
    long _memcpy_i_2;
    
#pragma ivdep array(table)
    
#pragma ivdep array(table_buf_0)
    
#pragma unroll 32
    for (_memcpy_i_2 = 0; _memcpy_i_2 < (sizeof(unsigned short ) * ((unsigned long )256) / 2); ++_memcpy_i_2) {
      long total_offset1 = (0 * 0 + 0);
      long total_offset2 = (0 * 256 + 0);
      table[total_offset1 + _memcpy_i_2] = table_buf_0[total_offset2 + _memcpy_i_2];
    }
  }
  emit_remainder:
// Emit the remaining bytes as a literal
  if (next_emit < merlin_input + input_size) {
    op_lcl1 = EmitLiteral_1(op_lcl1,next_emit,(int )(merlin_input + input_size - next_emit),false);
  }
  int offset = (int )(op_lcl1 - op);
  long _memcpy_i_0;
  
#pragma ivdep array(op)
  
#pragma ivdep array(op_lcl)
  
#pragma unroll 64
  for (_memcpy_i_0 = 0; _memcpy_i_0 < (((unsigned long )offset) / 1); ++_memcpy_i_0) {
    
#pragma HLS LOOP_TRIPCOUNT max=65536
    long total_offset1 = (0 * 0 + 0);
    long total_offset2 = (0 * 65536 + 0);
    op[total_offset1 + _memcpy_i_0] = op_lcl[total_offset2 + _memcpy_i_0];
  }
  merlin_return_value[0] = offset;
  return ;
}
