// Copyright 2005 Google Inc. All Rights Reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHAL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//#include "snappy.h"
//#include "snappy-internal.h"
//#include "snappy-sinksource.h"
//#ifndef SNAPPY_HAVE_SSE2
//#if defined(__SSE2__) || defined(_M_X64) ||     (defined(_M_IX86_FP) && _M_IX86_FP >= 2)
//#define SNAPPY_HAVE_SSE2 1
//#else
#define SNAPPY_HAVE_SSE2 0
//#endif
//#endif
#include <stdio.h>
#include <string.h>
#include <assert.h>
//#include <algorithm>
//#include <string>
//#include <vector>
//#include "CompressFragment_kernel.h"
#include <stdint.h>
typedef int8_t int8;
typedef uint8_t uint8;
typedef int16_t int16;
typedef uint16_t uint16;
typedef int32_t int32;
typedef uint32_t uint32;
typedef int64_t int64;
typedef uint64_t uint64;
// struct to replace std::pair
typedef struct pair {
unsigned long first;
bool second;}pair;

int my_memcmp(const void *s1,const void *s2,unsigned long n)
{
  const unsigned char *p1 = (const unsigned char *)s1;
  const unsigned char *p2 = (const unsigned char *)s2;
  while((n--))
    if (( *p1) != ( *p2)) 
      return ( *p1) - ( *p2);
     else 
      (p1++ , p2++);
  return 0;
}
static const unsigned int kuint32max = (unsigned int )0xFFFFFFFF;
//static const int64 kint64max = static_cast<int64>(0x7FFFFFFFFFFFFFFFL);
// The size of a compression block. Note that many parts of the compression
// code assumes that kBlockSize <= 65536; in particular, the hash table
// can only store 16-bit offsets, and EmitCopy() also assumes the offset
// is 65535 bytes or less. Note also that if you change this, it will
// affect the framing format (see framing_format.txt).
//
// Note that there might be older data around that is compressed with larger
// block sizes, so the decompression code should not rely on the
// non-existence of long backreferences.
static const int kBlockLog = 16;
static const unsigned long kBlockSize = (1 << 16);

bool IsLittleEndian()
{
  return true;
}

unsigned int bswap_u32(unsigned int num)
{
  unsigned int swapped;
// move byte 3 to byte 0
  swapped = num >> 24 & 0xff | num << 8 & 0xff0000 | num >> 8 & 0xff00 | num << 24 & 0xff000000;
// move byte 1 to byte 2
// move byte 2 to byte 1
// byte 0 to byte 3
  return swapped;
}
// WORKAROUND: Removed all references to these functions to simplify code.
//// These functions are provided for architectures that don't support
//// unaligned loads and stores.
//
//inline uint16 UNALIGNED_LOAD16(const void *p) {
//  uint16 t;
//  memcpy(&t, p, sizeof t);
//  return t;
//}
//
//inline uint32 UNALIGNED_LOAD32(const void *p) {
//  uint32 t;
//  memcpy(&t, p, sizeof t);
//  return t;
//}
//
//inline uint64 UNALIGNED_LOAD64(const void *p) {
//  uint64 t;
//  memcpy(&t, p, sizeof t);
//  return t;
//}
//
//inline void UNALIGNED_STORE16(void *p, uint16 v) {
//  memcpy(p, &v, sizeof v);
//}
//
//inline void UNALIGNED_STORE32(void *p, uint32 v) {
//  memcpy(p, &v, sizeof v);
//}
//
//inline void UNALIGNED_STORE64(void *p, uint64 v) {
//  memcpy(p, &v, sizeof v);
//}
//
//// Convert to little-endian storage, opposite of network format.
//// Convert x from host to little endian: x = LittleEndian.FromHost(x);
//// convert x from little endian to host: x = LittleEndian.ToHost(x);
////
////  Store values into unaligned memory converting to little endian order:
////    LittleEndian.Store16(p, x);
////
////  Load unaligned values stored in little endian converting to host order:
////    x = LittleEndian.Load16(p);
//class LittleEndian {
// public:
////   LittleEndian();
////   ~LittleEndian();
//     // Conversion functions.
//#if defined(SNAPPY_IS_BIG_ENDIAN)
//
//  /*static*/ uint16 FromHost16(uint16 x) { return bswap_16(x); }
//  /*static*/ uint16 ToHost16(uint16 x) { return bswap_16(x); }
//
//  /*static*/ uint32 FromHost32(uint32 x) { return bswap_32(x); }
//  /*static*/ uint32 ToHost32(uint32 x) { return bswap_32(x); }
//
//  /*static*/ bool IsLittleEndian() { return false; }
//
//#else  // !defined(SNAPPY_IS_BIG_ENDIAN)
//
//  /*static*/ uint16 FromHost16(uint16 x) { return x; }
//  /*static*/ uint16 ToHost16(uint16 x) { return x; }
//
//  /*static*/ uint32 FromHost32(uint32 x) { return x; }
//  /*static*/ uint32 ToHost32(uint32 x) { return x; }
//
//  /*static*/ bool IsLittleEndian() { return true; }
//
//#endif  // !defined(SNAPPY_IS_BIG_ENDIAN)
//
//  // Functions to do unaligned loads and stores in little-endian order.
//  /*static*/ uint16 Load16(const void *p) {
//    return ToHost16(UNALIGNED_LOAD16(p));
//  }
//
//  /*static*/ void Store16(void *p, uint16 v) {
//    UNALIGNED_STORE16(p, FromHost16(v));
//  }
//
//  /*static*/ uint32 Load32(const void *p) {
//    return ToHost32(UNALIGNED_LOAD32(p));
//  }
//
//  /*static*/ void Store32(void *p, uint32 v) {
//    UNALIGNED_STORE32(p, FromHost32(v));
//  }
//};
// Some bit-manipulation functions.

struct ::Bits 
{
}
;
int p_ZN4Bits9Log2FloorEj(struct Bits *this_,unsigned int n);
int p_ZN4Bits17FindLSBSetNonZeroEj(struct Bits *this_,unsigned int n);
static void __merlin_dummy_SgClassDeclaration_class_Bits_();

int p_ZN4Bits9Log2FloorEj(struct Bits *this_,unsigned int n)
{
  if (n == ((unsigned int )0)) 
    return - 1;
  int log = 0;
  unsigned int value = n;
  for (int i = 4; i >= 0; --i) {
    int shift = 1 << i;
    unsigned int x = value >> shift;
    if (x != ((unsigned int )0)) {
      value = x;
      log += shift;
    }
  }
  value == ((unsigned int )1)?((void )0) : __assert_fail("value == 1","CompressFragment_kernel.cpp",(unsigned int )241,__PRETTY_FUNCTION__);
  return log;
}
static void __merlin_dummy_SgMemberFunctionDeclaration_int_Log2Fl_();

int p_ZN4Bits17FindLSBSetNonZeroEj(struct Bits *this_,unsigned int n)
{
  int rc = 31;
  for (int i = 4, shift = 1 << 4; i >= 0; --i) {
    const unsigned int x = n << shift;
    if (x != ((unsigned int )0)) {
      n = x;
      rc -= shift;
    }
    shift >>= 1;
  }
  return rc;
}
static void __merlin_dummy_SgMemberFunctionDeclaration_int_FindLS_();
// Variable-length integer encoding.

class Varint 
{
//   Varint();
//   ~Varint();
// Maximum lengths of varint encoding of uint32.
  public: static const int kMax32 = 5;
// Attempts to parse a varint32 from a prefix of the bytes in [ptr,limit-1].
// Never reads a character at or beyond limit.  If a valid/terminated varint32
// was found in the range, stores it in *OUTPUT and returns a pointer just
// past the last byte of the varint32. Else returns NULL.  On success,
// "result <= limit".
/*static*/
  inline const char *Parse32WithLimit(const char *ptr,const char *limit,uint32 *OUTPUT);
// REQUIRES   "ptr" points to a buffer of length sufficient to hold "v".
// EFFECTS    Encodes "v" into "ptr" and returns a pointer to the
//            byte just past the last encoded byte.
/*static*/
  inline char *Encode32(char *ptr,uint32 v);
// EFFECTS    Appends the varint representation of "value" to "*s".
//static void Append32(string* s, uint32 value);
}
;

inline const char *Varint::Parse32WithLimit(const char *p,const char *l,uint32 *OUTPUT)
{
  const unsigned char *ptr = reinterpret_cast < const unsigned char * >  (p);
  const unsigned char *limit = reinterpret_cast < const unsigned char * >  (l);
  uint32 b;
  uint32 result;
  if (ptr >= limit) 
    return 0L;
  b = ( *(ptr++));
  result = b & 127;
  if (b < 128) 
    goto done;
  if (ptr >= limit) 
    return 0L;
  b = ( *(ptr++));
  result |= (b & 127) << 7;
  if (b < 128) 
    goto done;
  if (ptr >= limit) 
    return 0L;
  b = ( *(ptr++));
  result |= (b & 127) << 14;
  if (b < 128) 
    goto done;
  if (ptr >= limit) 
    return 0L;
  b = ( *(ptr++));
  result |= (b & 127) << 21;
  if (b < 128) 
    goto done;
  if (ptr >= limit) 
    return 0L;
  b = ( *(ptr++));
  result |= (b & 127) << 28;
  if (b < 16) 
    goto done;
// Value is too long to be a varint32
  return 0L;
  done:
   *OUTPUT = result;
  return reinterpret_cast < const char * >  (ptr);
}

inline char *Varint::Encode32(char *sptr,uint32 v)
{
// Operate on characters as unsigneds
  unsigned char *ptr = reinterpret_cast < unsigned char * >  (sptr);
  static const int B = 128;
  if (v < (1 << 7)) {
     *(ptr++) = v;
  }
   else if (v < (1 << 14)) {
     *(ptr++) = (v | B);
     *(ptr++) = (v >> 7);
  }
   else if (v < (1 << 21)) {
     *(ptr++) = (v | B);
     *(ptr++) = (v >> 7 | B);
     *(ptr++) = (v >> 14);
  }
   else if (v < (1 << 28)) {
     *(ptr++) = (v | B);
     *(ptr++) = (v >> 7 | B);
     *(ptr++) = (v >> 14 | B);
     *(ptr++) = (v >> 21);
  }
   else {
     *(ptr++) = (v | B);
     *(ptr++) = (v >> 7 | B);
     *(ptr++) = (v >> 14 | B);
     *(ptr++) = (v >> 21 | B);
     *(ptr++) = (v >> 28);
  }
  return reinterpret_cast < char * >  (ptr);
}
#define SNAPPY_PREDICT_FALSE(x) x
#define SNAPPY_PREDICT_TRUE(x) x

inline static struct pair FindMatchLength(const char *s1,const char *s2,const char *s2_limit)
{
// Implementation based on the x86-64 version, above.
  s2_limit >= s2?((void )0) : __assert_fail("s2_limit >= s2","CompressFragment_kernel.cpp",341,__PRETTY_FUNCTION__);
  int matched = 0;
  while(s2 <= s2_limit - 4 &&  *((unsigned int *)s2) ==  *((unsigned int *)(s2 + matched))){
//         UNALIGNED_LOAD32(s2) == UNALIGNED_LOAD32(s1 + matched)) {
    s2 += 4;
    matched += 4;
  }
//LittleEndian le;
  struct Bits bits;
  if (IsLittleEndian() && s2 <= s2_limit - 4) {
    unsigned int x =  *((unsigned int *)s2) ^  *((unsigned int *)(s1 + matched));
//    uint32 x = UNALIGNED_LOAD32(s2) ^ UNALIGNED_LOAD32(s1 + matched);
    int matching_bits = p_ZN4Bits17FindLSBSetNonZeroEj(&bits,x);
    matched += matching_bits >> 3;
  }
   else {
    while(s2 < s2_limit && s1[matched] == ( *s2)){
      ++s2;
      ++matched;
    }
  }
  struct pair ret;
  ret . first = matched;
  ret . second = matched < 8;
  return ret;
}
enum __anonymous_0x1cc1210 {LITERAL=0,
// 3 bit length + 3 bits of offset in opcode
COPY_1_BYTE_OFFSET=1,COPY_2_BYTE_OFFSET=2,COPY_4_BYTE_OFFSET=3} ;
//namespace snappy {
//using internal::COPY_1_BYTE_OFFSET;
//using internal::COPY_2_BYTE_OFFSET;
//using internal::LITERAL;
//using internal::char_table;
//using internal::kMaximumTagLength;
// Any hash function will produce a valid compressed bitstream, but a good
// hash function reduces the number of collisions and thus yields better
// compression for compressible input, and more speed for incompressible
// input. Of course, it doesn't hurt if the hash function is reasonably fast
// either, as it gets called a lot.

inline static unsigned int HashBytes(unsigned int bytes,int shift)
{
  unsigned int kMul = 0x1e35a7bd;
  return bytes * kMul >> shift;
}

inline static unsigned int Hash(const char *p,int shift)
{
  return HashBytes( *((unsigned int *)p),shift);
//return HashBytes(UNALIGNED_LOAD32(p), shift);
}

unsigned long MaxCompressedLength(unsigned long source_len)
{
// Compressed data can be defined as:
//    compressed := item* literal*
//    item       := literal* copy
//
// The trailing literal sequence has a space blowup of at most 62/60
// since a literal of length 60 needs one tag byte + one extra byte
// for length information.
//
// Item blowup is trickier to measure.  Suppose the "copy" op copies
// 4 bytes of data.  Because of a special check in the encoding code,
// we produce a 4-byte copy only if the offset is < 65536.  Therefore
// the copy op takes 3 bytes to encode, and this type of item leads
// to at most the 62/60 blowup for representing literals.
//
// Suppose the "copy" op copies 5 bytes of data.  If the offset is big
// enough, it will take 5 bytes to encode the copy op.  Therefore the
// worst case here is a one-byte literal followed by a five-byte copy.
// I.e., 6 bytes of input turn into 7 bytes of "compressed" data.
//
// This last factor dominates the blowup, so the final estimate is:
  return 32 + source_len + source_len / 6;
}
//namespace {
// WORKAROUND: Replaced with single memcpy
//void UnalignedCopy64(const void* src, void* dst) {
//  char tmp[8];
//  memcpy(tmp, src, 8);
//  memcpy(dst, tmp, 8);
//}
//void UnalignedCopy128(const void* src, void* dst) {
//  // TODO(alkis): Remove this when we upgrade to a recent compiler that emits
//  // SSE2 moves for memcpy(dst, src, 16).
//#if SNAPPY_HAVE_SSE2
//  __m128i x = _mm_loadu_si128(static_cast<const __m128i*>(src));
//  _mm_storeu_si128(static_cast<__m128i*>(dst), x);
//#else
//  char tmp[16];
//  memcpy(tmp, src, 16);
//  memcpy(dst, tmp, 16);
//#endif
//}
// Copy [src, src+(op_limit-op)) to [op, (op_limit-op)) a byte at a time. Used
// for handling COPY operations where the input and output regions may overlap.
// For example, suppose:
//    src       == "ab"
//    op        == src + 2
//    op_limit  == op + 20
// After IncrementalCopySlow(src, op, op_limit), the result will have eleven
// copies of "ab"
//    ababababababababababab
// Note that this does not match the semantics of either memcpy() or memmove().

inline char *IncrementalCopySlow(const char *src,char *op,char *const op_limit)
{
  while(op < op_limit){
     *(op++) =  *(src++);
  }
  return op_limit;
}
// Copy [src, src+(op_limit-op)) to [op, (op_limit-op)) but faster than
// IncrementalCopySlow. buf_limit is the address past the end of the writable
// region of the buffer.

inline char *IncrementalCopy(const char *src,char *op,char *const op_limit,char *const buf_limit)
{
// Terminology:
//
// slop = buf_limit - op
// pat  = op - src
// len  = limit - op
  src < op?(static_cast < void  >  (0)) : __assert_fail("src < op","CompressFragment_kernel.cpp",471,__PRETTY_FUNCTION__);
  op_limit <= buf_limit?(static_cast < void  >  (0)) : __assert_fail("op_limit <= buf_limit","CompressFragment_kernel.cpp",472,__PRETTY_FUNCTION__);
// NOTE: The compressor always emits 4 <= len <= 64. It is ok to assume that
// to optimize this function but we have to also handle these cases in case
// the input does not satisfy these conditions.
  unsigned long pattern_size = (op - src);
// The cases are split into different branches to allow the branch predictor,
// FDO, and static prediction hints to work better. For each input we list the
// ratio of invocations that match each condition.
//
// input        slop < 16   pat < 8  len > 16
// ------------------------------------------
// html|html4|cp   0%         1.01%    27.73%
// urls            0%         0.88%    14.79%
// jpg             0%        64.29%     7.14%
// pdf             0%         2.56%    58.06%
// txt[1-4]        0%         0.23%     0.97%
// pb              0%         0.96%    13.88%
// bin             0.01%     22.27%    41.17%
//
// It is very rare that we don't have enough slop for doing block copies. It
// is also rare that we need to expand a pattern. Small patterns are common
// for incompressible formats and for those we are plenty fast already.
// Lengths are normally not greater than 16 but they vary depending on the
// input. In general if we always predict len <= 16 it would be an ok
// prediction.
//
// In order to be fast we want a pattern >= 8 bytes and an unrolled loop
// copying 2x 8 bytes at a time.
// Handle the uncommon case where pattern is less than 8 bytes.
  if (pattern_size < 8) {
// Expand pattern to at least 8 bytes. The worse case scenario in terms of
// buffer usage is when the pattern is size 3. ^ is the original position
// of op. x are irrelevant bytes copied by the last UnalignedCopy64.
//
// abc
// abcabcxxxxx
// abcabcabcabcxxxxx
//    ^
// The last x is 14 bytes after ^.
    if (op <= buf_limit - 14) {
      while(pattern_size < 8){
        memcpy(op,src,8);
//UnalignedCopy64(src, op);
        op += pattern_size;
        pattern_size *= 2;
      }
      if (op >= op_limit) 
        return op_limit;
    }
     else {
      return IncrementalCopySlow(src,op,op_limit);
    }
  }
  pattern_size >= 8?(static_cast < void  >  (0)) : __assert_fail("pattern_size >= 8","CompressFragment_kernel.cpp",525,__PRETTY_FUNCTION__);
// Copy 2x 8 bytes at a time. Because op - src can be < 16, a single
// UnalignedCopy128 might overwrite data in op. UnalignedCopy64 is safe
// because expanding the pattern to at least 8 bytes guarantees that
// op - src >= 8.
  while(op <= buf_limit - 16){
    memcpy(op,src,8);
//UnalignedCopy64(src, op);
    memcpy((op + 8),(src + 8),8);
//UnalignedCopy64(src + 8, op + 8);
    src += 16;
    op += 16;
    if (op >= op_limit) 
      return op_limit;
  }
// We only take this branch if we didn't have enough slop and we can do a
// single 8 byte copy.
  if (op <= buf_limit - 8) {
    memcpy(op,src,8);
//UnalignedCopy64(src, op);
    src += 8;
    op += 8;
  }
  return IncrementalCopySlow(src,op,op_limit);
}
//}  // namespace

inline static char *EmitLiteral(char *op,const char *literal,int len,bool allow_fast_path)
{
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
  len > 0?((void )0) : __assert_fail("len > 0","CompressFragment_kernel.cpp",567,__PRETTY_FUNCTION__);
  int n = len - 1;
  if (allow_fast_path && len <= 16) {
// Fits in tag byte
     *(op++) = (LITERAL | n << 2);
// WORKAROUND: Merlin doesn't like this function
    memcpy(op,literal,16);
//UnalignedCopy128(literal, op);
    return op + len;
  }
  if (n < 60) {
// Fits in tag byte
     *(op++) = (LITERAL | n << 2);
  }
   else {
// Encode in upcoming bytes
    char *base = op;
    int count = 0;
    op++;
    while(n > 0){
       *(op++) = (n & 0xff);
      n >>= 8;
      count++;
    }
    count >= 1?((void )0) : __assert_fail("count >= 1","CompressFragment_kernel.cpp",592,__PRETTY_FUNCTION__);
    count <= 4?((void )0) : __assert_fail("count <= 4","CompressFragment_kernel.cpp",593,__PRETTY_FUNCTION__);
     *base = (LITERAL | 59 + count << 2);
  }
  memcpy(op,literal,len);
  return op + len;
}

inline static char *EmitCopyAtMost64(char *op,unsigned long offset,unsigned long len,bool len_less_than_12)
{
  len <= 64?((void )0) : __assert_fail("len <= 64","CompressFragment_kernel.cpp",602,__PRETTY_FUNCTION__);
  len >= 4?((void )0) : __assert_fail("len >= 4","CompressFragment_kernel.cpp",603,__PRETTY_FUNCTION__);
  offset < 65536?((void )0) : __assert_fail("offset < 65536","CompressFragment_kernel.cpp",604,__PRETTY_FUNCTION__);
  len_less_than_12 == (len < 12)?((void )0) : __assert_fail("len_less_than_12 == (len < 12)","CompressFragment_kernel.cpp",605,__PRETTY_FUNCTION__);
  if (len_less_than_12 && offset < 2048) {
// offset fits in 11 bits.  The 3 highest go in the top of the first byte,
// and the rest go in the second byte.
     *(op++) = (COPY_1_BYTE_OFFSET + (len - 4 << 2) + (offset >> 3 & 0xe0));
     *(op++) = (offset & 0xff);
  }
   else {
// Write 4 bytes, though we only care about 3 of them.  The output buffer
// is required to have some slack, so the extra byte won't overrun it.
    unsigned int u = (COPY_2_BYTE_OFFSET + (len - 1 << 2) + (offset << 8));
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

inline static char *EmitCopy(char *op,unsigned long offset,unsigned long len,bool len_less_than_12)
{
  len_less_than_12 == (len < 12)?((void )0) : __assert_fail("len_less_than_12 == (len < 12)","CompressFragment_kernel.cpp",634,__PRETTY_FUNCTION__);
  if (len_less_than_12) {
    return EmitCopyAtMost64(op,offset,len,true);
  }
   else {
// A special case for len <= 64 might help, but so far measurements suggest
// it's in the noise.
// Emit 64 byte copies but make sure to keep at least four bytes reserved.
    while(len >= 68){
      op = EmitCopyAtMost64(op,offset,64,false);
      len -= 64;
    }
// One or two copies will now finish the job.
    if (len > 64) {
      op = EmitCopyAtMost64(op,offset,60,false);
      len -= 60;
    }
// Emit remainder.
    op = EmitCopyAtMost64(op,offset,len,len < 12);
    return op;
  }
}

bool GetUncompressedLength(const char *start,unsigned long n,unsigned long *result)
{
  unsigned int v = 0;
  const char *limit = start + n;
  class Varint vint;
  if (vint .  Parse32WithLimit (start,limit,(&v)) != 0L) {
     *result = v;
    return true;
  }
   else {
    return false;
  }
}
// For 0 <= offset <= 4, GetUint32AtOffset(GetEightBytesAt(p), offset) will
// equal UNALIGNED_LOAD32(p + offset).  Motivation: On x86-64 hardware we have
// empirically found that overlapping loads such as
//  UNALIGNED_LOAD32(p) ... UNALIGNED_LOAD32(p+1) ... UNALIGNED_LOAD32(p+2)
// are slower than UNALIGNED_LOAD64(p) followed by shifts and casts to uint32.
//
// We have different versions for 64- and 32-bit; ideally we would avoid the
// two functions and just inline the UNALIGNED_LOAD64 call into
// GetUint32AtOffset, but GCC (at least not as of 4.6) is seemingly not clever
// enough to avoid loading the value multiple times then. For 64-bit, the load
// is done when GetEightBytesAt() is called, whereas for 32-bit, the load is
// done at GetUint32AtOffset() time.
typedef const char *EightBytesReference;

inline static const char *GetEightBytesAt(const char *ptr)
{
  return ptr;
}

inline static unsigned int GetUint32AtOffset(const char *v,int offset)
{
  offset >= 0?((void )0) : __assert_fail("offset >= 0","CompressFragment_kernel.cpp",691,__PRETTY_FUNCTION__);
  offset <= 4?((void )0) : __assert_fail("offset <= 4","CompressFragment_kernel.cpp",692,__PRETTY_FUNCTION__);
  return  *((unsigned int *)(v + offset));
//return UNALIGNED_LOAD32(v + offset);
}
// Flat array compression that does not emit the "uncompressed length"
// prefix. Compresses "input" string to the "*op" buffer.
//
// REQUIRES: "input" is at most "kBlockSize" bytes long.
// REQUIRES: "op" points to an array of memory that is at least
// "MaxCompressedLength(input.size())" in size.
// REQUIRES: All elements in "table[0..table_size-1]" are initialized to zero.
// REQUIRES: "table_size" is a power of two
//
// Returns an "end" pointer into "op" buffer.
// "end - op" is the compressed size of "input".
//namespace internal {
static void __merlin_dummy_kernel_pragma();

int CompressFragment_kernel(const char *input,unsigned long input_size,char *op,unsigned short *table,const int table_size)
{
  
#pragma ACCEL interface variable=input depth=65536
  
#pragma ACCEL interface variable=op depth=65536
  
#pragma ACCEL interface variable=table depth=256
// WORKAROUND: since Merlin doesn't support assignment to interface pointers creating a local buffer
  char op_lcl[65536];
  char *op_lcl1 = op_lcl;
// "ip" is the input pointer, and "op" is the output pointer.
  const char *ip = input;
  input_size <= kBlockSize?((void )0) : __assert_fail("input_size <= kBlockSize","CompressFragment_kernel.cpp",729,__PRETTY_FUNCTION__);
// table must be power of two
  (table_size & table_size - 1) == 0?((void )0) : __assert_fail("(table_size & (table_size - 1)) == 0","CompressFragment_kernel.cpp",730,__PRETTY_FUNCTION__);
  struct Bits bits;
  const int shift = 32 - p_ZN4Bits9Log2FloorEj(&bits,table_size);
  ((int )(kuint32max >> shift)) == table_size - 1?((void )0) : __assert_fail("static_cast<int>(kuint32max >> shift) == table_size - 1","CompressFragment_kernel.cpp",733,__PRETTY_FUNCTION__);
  const char *ip_end = input + input_size;
  const char *base_ip = ip;
// Bytes in [next_emit, ip) will be emitted as literal bytes.  Or
// [next_emit, ip_end) after the main loop.
  const char *next_emit = ip;
  const unsigned long kInputMarginBytes = 15;
  if (input_size >= kInputMarginBytes) {
    const char *ip_limit = input + input_size - kInputMarginBytes;
    for (unsigned int next_hash = Hash(++ip,shift); ; ) {
      next_emit < ip?((void )0) : __assert_fail("next_emit < ip","CompressFragment_kernel.cpp",745,__PRETTY_FUNCTION__);
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
      unsigned int skip = 32;
      const char *next_ip = ip;
      const char *candidate;
      do {
        ip = next_ip;
        unsigned int hash = next_hash;
        hash == Hash(ip,shift)?((void )0) : __assert_fail("hash == Hash(ip, shift)","CompressFragment_kernel.cpp",778,__PRETTY_FUNCTION__);
        unsigned int bytes_between_hash_lookups = skip >> 5;
        skip += bytes_between_hash_lookups;
        next_ip = ip + bytes_between_hash_lookups;
        if (next_ip > ip_limit) {
          goto emit_remainder;
        }
        next_hash = Hash(next_ip,shift);
        candidate = base_ip + table[hash];
        candidate >= base_ip?((void )0) : __assert_fail("candidate >= base_ip","CompressFragment_kernel.cpp",787,__PRETTY_FUNCTION__);
        candidate < ip?((void )0) : __assert_fail("candidate < ip","CompressFragment_kernel.cpp",788,__PRETTY_FUNCTION__);
        table[hash] = (ip - base_ip);
      }while ( *((unsigned int *)ip) !=  *((unsigned int *)candidate));
// while (SNAPPY_PREDICT_TRUE(UNALIGNED_LOAD32(ip) !=
//                           UNALIGNED_LOAD32(candidate)));
// Step 2: A 4-byte match has been found.  We'll later see if more
// than 4 bytes match.  But, prior to the match, input
// bytes [next_emit, ip) are unmatched.  Emit them as "literal bytes."
      next_emit + 16 <= ip_end?((void )0) : __assert_fail("next_emit + 16 <= ip_end","CompressFragment_kernel.cpp",800,__PRETTY_FUNCTION__);
      op_lcl1 = EmitLiteral(op_lcl1,next_emit,(ip - next_emit),true);
// Step 3: Call EmitCopy, and then see if another EmitCopy could
// be our next move.  Repeat until we find no match for the
// input immediately after what was consumed by the last EmitCopy call.
//
// If we exit this loop normally then we need to call EmitLiteral next,
// though we don't yet know how big the literal will be.  We handle that
// by proceeding to the next iteration of the main loop.  We also can exit
// this loop via goto if we get close to exhausting the input.
      const char *input_bytes;
      unsigned int candidate_bytes = 0;
      do {
// We have a 4-byte match at ip, and no need to emit any
// "literal bytes" prior to ip.
        const char *base = ip;
        struct pair p = FindMatchLength(candidate + 4,ip + 4,ip_end);
        unsigned long matched = 4 + p . first;
        ip += matched;
        unsigned long offset = (base - candidate);
// WORKAROUND: Merlin doesn't support memcmp yet so using custom function 
        0 == my_memcmp(base,candidate,matched)?((void )0) : __assert_fail("0 == my_memcmp(base, candidate, matched)","CompressFragment_kernel.cpp",825,__PRETTY_FUNCTION__);
        op_lcl1 = EmitCopy(op_lcl1,offset,matched,p . second);
        next_emit = ip;
        if (ip >= ip_limit) {
          goto emit_remainder;
        }
// We are now looking for a 4-byte match again.  We read
// table[Hash(ip, shift)] for that.  To improve compression,
// we also update table[Hash(ip - 1, shift)] and table[Hash(ip, shift)].
        input_bytes = GetEightBytesAt(ip - 1);
        unsigned int prev_hash = HashBytes((GetUint32AtOffset(input_bytes,0)),shift);
        table[prev_hash] = (ip - base_ip - 1);
        unsigned int cur_hash = HashBytes((GetUint32AtOffset(input_bytes,1)),shift);
        candidate = base_ip + table[cur_hash];
        candidate_bytes =  *((unsigned int *)candidate);
//candidate_bytes = UNALIGNED_LOAD32(candidate);
        table[cur_hash] = (ip - base_ip);
      }while (GetUint32AtOffset(input_bytes,1) == candidate_bytes);
      next_hash = HashBytes((GetUint32AtOffset(input_bytes,2)),shift);
      ++ip;
    }
  }
  emit_remainder:
// Emit the remaining bytes as a literal
  if (next_emit < ip_end) {
    op_lcl1 = EmitLiteral(op_lcl1,next_emit,(ip_end - next_emit),false);
  }
  int offset = (op_lcl1 - op);
  memcpy(op,op_lcl,offset);
  return offset;
}
//}  // end namespace internal
//} // end namespace snappy
