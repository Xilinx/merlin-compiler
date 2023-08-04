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
#include <hls_stream.h>
#include "merlin_type_define.h"
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
const int g_dbgLvl = 0;

static void read(mywide_t (*inbuf)[32][512],class hls::stream< mywide_t  > inStream[32])
{
//16
  merlinL3:
//16
  for (int i = 0; i < 2 * 1024 * 1024 / 16 / (8192 / 16) / 32; ++i) {
// 16
    merlinL2:
// 16
    for (int u = 0; u < 32; ++u) {
// 1024
      merlinL1:
// 1024
      for (int j = 0; j < 8192 / 16; ++j) {
        
#pragma HLS PIPELINE
        mywide_t tmpData = inbuf[i][u][j];
        inStream[u] . write(tmpData);
        if (((bool )g_dbgLvl) && j == 0) {
//          for (int k=0; k<WORD_SZ; ++k) printf ("%02x ", tmpData. data [k]);
//          std::cout << std::endl;
        }
      }
    }
  }
}

static void pageToByte(class hls::stream< mywide_t  > &win,class hls::stream< unsigned char  > &out)
{
  merlinL6:
  for (int i = 0; i < 2 * 1024 * 1024 / 16 / (8192 / 16) / 32; ++i) {
    merlinL5:
    for (int i = 0; i < 8192 / 16; ++i) {
      mywide_t tmp = win . read();
      merlinL4:
      for (int j = 0; j < 16; ++j) {
        
#pragma HLS PIPELINE
        out . write(tmp . data[j]);
      }
    }
  }
}

static void getHeaderInt2(class hls::stream< unsigned char  > &stream,class hls::stream< short  > nTupsStrm[6],class hls::stream< unsigned char  > &outStream)
{
  unsigned char ch1;
  unsigned char ch2;
  ch1 = stream . read();
  ch2 = stream . read();
//pgOff +=2;
//if (!b1 || !b2) std::cout << "getHeaderInt failed b1 " << b1 << " b2 " << b2 << std::endl;
  short nTups = (short )(((int )ch2) << 8 | ((int )ch1));
  merlinL9:
  for (int scnt = 0; scnt < 6; ++scnt) 
// Original: #pragma ACCEL PARALLEL COMPLETE
{
    
#pragma HLS unroll
    nTupsStrm[scnt] . write(nTups);
  }
  merlinL8:
  for (int bcnt = 2; bcnt < 8192; ++bcnt) {
    
#pragma HLS PIPELINE
    unsigned char ch = stream . read();
    outStream . write(ch);
  }
}

static void p_ZN9TupParserC2Ev(struct TupParser *this_)
{
  this_ -> off = 0;
  this_ -> pgOff = 2;
}

static void p_ZN9TupParser8startTupEv(struct TupParser *this_)
{
  this_ -> off = 0;
}

static unsigned char p_ZN9TupParser7getByteER6streamIhE_1(struct TupParser *this_,class hls::stream< unsigned char  > &stream)
{
  ++this_ -> pgOff;
  return stream . read();
}

static unsigned char p_ZN9TupParser7getByteER6streamIhE_2(struct TupParser *this_,class hls::stream< unsigned char  > &stream)
{
  ++this_ -> pgOff;
  return stream . read();
}

static unsigned char p_ZN9TupParser7getByteER6streamIhE_3(struct TupParser *this_,class hls::stream< unsigned char  > &stream)
{
  ++this_ -> pgOff;
  return stream . read();
}

static unsigned char p_ZN9TupParser7getByteER6streamIhE_4(struct TupParser *this_,class hls::stream< unsigned char  > &stream)
{
  ++this_ -> pgOff;
  return stream . read();
}

static unsigned char p_ZN9TupParser7getByteER6streamIhE(struct TupParser *this_,class hls::stream< unsigned char  > &stream)
{
  ++this_ -> pgOff;
  return stream . read();
}
const int attrLens[] = {(8), (8), (8), (4), (- 1), (- 1), (- 1), (- 1), (- 1), (- 1), (4), (4), (4), (- 1), (- 1), (- 1)};
const char align[16] = {('\0'), ('\0'), ('\0'), ('\0'), ('\0'), ('\0'), ('\0'), ('\0'), ('\0'), ('\0'), ('\1'), ('\1'), ('\1'), ('\0'), ('\0'), ('\0')};

static unsigned int p_ZN9TupParser11getFieldLenEjR6streamIhE(struct TupParser *this_,unsigned int fPtr,class hls::stream< unsigned char  > &stream)
{
  unsigned char ch;
  unsigned int fLen = (unsigned int )0;
  if (attrLens[fPtr] != - 1) {
    fLen = ((unsigned int )attrLens[fPtr]);
    if ((bool )align[fPtr]) {
      int aoff = (int )(((unsigned int )(this_ -> off)) + ((unsigned int )(4 - 1)) & ~((unsigned int )(4 - 1)));
      if (aoff != this_ -> off) {
        merlinL12:
        for (int i = this_ -> off; i < aoff; ++i) 
// Original: #pragma ACCEL PIPELINE AUTO
{
          
#pragma HLS pipeline
          
#pragma HLS LOOP_TRIPCOUNT min=0 max=1 avg=0
          ch = p_ZN9TupParser7getByteER6streamIhE(this_,stream);
          ++this_ -> off;
        }
      }
    }
    return fLen;
  }
  ch = p_ZN9TupParser7getByteER6streamIhE_1(this_,stream);
  ++this_ -> off;
  if ((bool )(((int )ch) & 0x01)) {
    fLen = ((unsigned int )(((int )ch) >> 1));
    return fLen - ((unsigned int )1);
  }
   else {
    return (unsigned int )0;
  }
}

static bool p_ZN9TupParser8getFieldEPhjR6streamIhE(struct TupParser *this_,unsigned char fData[16],unsigned int fLen,class hls::stream< unsigned char  > &stream)
{
  
#pragma HLS ARRAY_PARTITION variable=fData complete dim=0
  unsigned char ch;
  merlinL13:
  for (int i = 0; ((unsigned int )i) < fLen; ++i) 
// Original: #pragma ACCEL PIPELINE AUTO
{
    
#pragma HLS pipeline
    
#pragma HLS LOOP_TRIPCOUNT min=4 max=16 avg=8
    ch = p_ZN9TupParser7getByteER6streamIhE_2(this_,stream);
    fData[i] = ch;
  }
  this_ -> off += fLen;
  return true;
}

static bool p_ZN9TupParser9skipBytesEiR6streamIhE(struct TupParser *this_,int fLen,class hls::stream< unsigned char  > &stream)
{
  this_ -> off += fLen;
  merlinL14:
  for (int i = 0; i < fLen; ++i) 
// Original: #pragma ACCEL PIPELINE AUTO
{
    
#pragma HLS pipeline
    
#pragma HLS LOOP_TRIPCOUNT min=4 max=16 avg=8
    unsigned char ch;
    ch = p_ZN9TupParser7getByteER6streamIhE_3(this_,stream);
  }
  return true;
}

static void p_ZN9TupParser13skipToPageEndER6streamIhE(struct TupParser *this_,class hls::stream< unsigned char  > &stream)
{
  merlinL15:
  for (int i = this_ -> pgOff; i < 8192; ++i) 
// Original: #pragma ACCEL PIPELINE AUTO
{
    
#pragma HLS pipeline
    
#pragma HLS LOOP_TRIPCOUNT min=4 max=16 avg=8
    unsigned char ch;
    ch = p_ZN9TupParser7getByteER6streamIhE_4(this_,stream);
  }
}
const int nAttr = 16;

static void q6_filter(class hls::stream< short  > &nTupsStrm,class hls::stream< unsigned char  > &byteStream,class hls::stream< fdata_t  > &qtyFieldStrm,class hls::stream< fdata_t  > &extFieldStrm,class hls::stream< fdata_t  > &dscFieldStrm,class hls::stream< fdata_t  > &shpFieldStrm,class hls::stream< bool  > &filterDone)
{
  struct TupParser prsr;
  p_ZN9TupParserC2Ev(&prsr);
  short nTups = nTupsStrm . read();
  merlinL11:
  for (short tcnt = (short )0; ((int )tcnt) < ((int )nTups); ++tcnt) {
    
#pragma HLS LOOP_TRIPCOUNT min=50 max=70 avg=60
    p_ZN9TupParser8startTupEv(&prsr);
    int _s_fPtr = 0;
    unsigned char _l_fPtr = (unsigned char )0;
// Canonicalized from: for(unsigned char fPtr =(unsigned char )0;((int )_s_fPtr) < nAttr;++_s_fPtr) {...}
    merlinL10:
// Canonicalized from: for(unsigned char fPtr =(unsigned char )0;((int )_s_fPtr) < nAttr;++_s_fPtr) {...}
    for (_s_fPtr = ((int )((unsigned char )0)); _s_fPtr <= 15; ++_s_fPtr) {
      unsigned char fLen = (unsigned char )(p_ZN9TupParser11getFieldLenEjR6streamIhE(&prsr,(unsigned int )_s_fPtr,byteStream));
//std::cout << "Tup [" << nTups << "] Field [" << fPtr << "] Len " << fLen << std::endl;
//bool b1 = prsr. getField (fLen, byteStream);
//if (!b1) std::cout << "Bad EOF at fPtr " << fPtr << " " << tup << " " << nTups << std::endl;
//      Dbg2 std::cout << "Tup [" << tcnt << "] F [" << fPtr << "] " << fLen << std::endl;
      if (((int )_s_fPtr) == 4 || ((int )_s_fPtr) == 5 || ((int )_s_fPtr) == 6 || ((int )_s_fPtr) == 10) {
        fdata_t tmp;
        p_ZN9TupParser8getFieldEPhjR6streamIhE(&prsr,tmp . data,(unsigned int )fLen,byteStream);
        tmp . data[16 - 1] = fLen;
        if (((int )_s_fPtr) == 4) {
          qtyFieldStrm . write(tmp);
        }
         else {
          if (((int )_s_fPtr) == 5) {
            extFieldStrm . write(tmp);
          }
           else {
            if (((int )_s_fPtr) == 6) {
              dscFieldStrm . write(tmp);
            }
             else {
              shpFieldStrm . write(tmp);
            }
          }
        }
      }
       else {
        p_ZN9TupParser9skipBytesEiR6streamIhE(&prsr,(int )fLen,byteStream);
      }
    }
  }
  p_ZN9TupParser13skipToPageEndER6streamIhE(&prsr,byteStream);
// write eof
//  filterDone. write (true);
}

static long snumToLong3(unsigned char fData[16])
{
  int fsz = (int )fData[16 - 1];
  bool bShort = (((int )fData[1]) & 0xC0) == 0x80;
  char bSign = ((bool )(((int )fData[1]) & 0x20));
  unsigned int scale = (unsigned int )((((int )fData[1]) & 0x1F) << 1 + (((int )fData[0]) & 0x80));
  char weight = (char )(((int )fData[0]) & 0x7F);
  weight = ((char )(((int )weight) | (((int )weight) & 0x40) << 1));
  long res = (long )0;
// 2 bytes for short-hdr. 2 bytes per btdig
  int nTotDigs = (fsz - 2) / 2;
  int rose_temp;
  if (((int )weight) >= 0) {
    rose_temp = ((int )weight) + 1;
  }
   else {
    rose_temp = 0;
  }
  int nWhlDigs = rose_temp;
  unsigned int rose_temp_0;
  if ((bool )scale) {
    rose_temp_0 = scale / ((unsigned int )4) + ((unsigned int )1);
  }
   else {
    rose_temp_0 = ((unsigned int )0);
  }
  int nFracDigs = (int )rose_temp_0;
  int rose_temp_1;
  if ((bool )scale) {
    rose_temp_1 = 0;
  }
   else {
    rose_temp_1 = ((int )weight);
  }
  int nZer0Digs = rose_temp_1;
// 1 Frac digit
  int nDigs = nWhlDigs + 1;
  int fPtr = 2;
  int _l_i = nDigs - 1;
// Canonicalized from: for(int i = nDigs - 1;i >= 0;(--i , fPtr += 2)) {...}
  merlinL17:
// Canonicalized from: for(int i = nDigs - 1;i >= 0;(--i , fPtr += 2)) {...}
  for (_l_i = nDigs - 1; _l_i >= 0; --_l_i) 
// Original: #pragma ACCEL PIPELINE AUTO
{
    
#pragma HLS pipeline
    
#pragma HLS LOOP_TRIPCOUNT min=1 max=8 avg=6
    short dig = (short )0;
    if (fPtr < fsz) {
      dig = ((short )(((int )fData[fPtr + 1]) << 8 | ((int )fData[fPtr])));
    }
    if (_l_i == 0) {
      res = res * ((long )100) + ((long )(((int )dig) / 100));
    }
     else {
      res = res * ((long )10000) + ((long )dig);
    }
    fPtr += 2;
  }
  if ((bool )bSign) {
    return ((long )(- 1)) * res;
  }
   else {
    return res;
  }
}

static void toLong(class hls::stream< short  > &nTupsStrm,class hls::stream< fdata_t  > &fieldStrm,class hls::stream< long  > &longStrm)
{
  short nTups = nTupsStrm . read();
  merlinL16:
  for (short i = (short )0; ((int )i) < ((int )nTups); ++i) {
    fdata_t tmp = fieldStrm . read();
    long l = snumToLong3(tmp . data);
    longStrm . write(l);
  }
}

static int getJdate(unsigned char fData[16])
{
  int j = 0;
// Standardize from: for(int i = 4 - 1;i >= 0;--i) {...}
  merlinL19:
// Standardize from: for(int i = 4 - 1;i >= 0;--i) {...}
  for (int i = 0; i <= 3; i++) 
// Original: #pragma ACCEL PARALLEL COMPLETE
{
    
#pragma HLS unroll
    int _in_i = 3 + - 1 * i;
    j = j << 8 | ((int )fData[_in_i]);
  }
  return j;
}

static void toDate(class hls::stream< short  > &nTupsStrm,class hls::stream< fdata_t  > &fieldStrm,class hls::stream< int  > &dateStrm)
{
  short nTups = nTupsStrm . read();
  merlinL18:
  for (short i = (short )0; ((int )i) < ((int )nTups); ++i) {
    
#pragma HLS PIPELINE
    fdata_t tmp = fieldStrm . read();
    int d = getJdate(tmp . data);
    dateStrm . write(d);
  }
}

static long agg(class hls::stream< short  > &nTupsStrm,class hls::stream< long  > &qtyStrm,class hls::stream< long  > &extStrm,class hls::stream< long  > &dscStrm,class hls::stream< int  > &shpStrm,class hls::stream< bool  > &filterDone)
{
  short nTups = nTupsStrm . read();
  long sum = (long )0;
  merlinL20:
  for (short i = (short )0; ((int )i) < ((int )nTups); ++i) {
    
#pragma HLS PIPELINE
    long quantity = qtyStrm . read();
    long extPrice = extStrm . read();
    long discount = dscStrm . read();
    int shipDate = shpStrm . read();
    if (shipDate >= - 2191 && shipDate < - 1826 && discount >= ((long )5) && discount <= ((long )7) && quantity < ((long )2400)) {
      sum += extPrice * discount;
//std::cout << "Matched. sum " << sum << std::endl;
//std::cout << "Tup [" << nTups << "] " << quantity <<"|" << extPrice 
//  << "|" << discount << "|" << shipDate << std::endl;
    }
  }
//  bool eop = filterDone. read ();
  return sum;
}

static void q6_paged(class hls::stream< unsigned char  > &byteStream,long *res)
{
  
#pragma HLS DATAFLOW
  class hls::stream< fdata_t  > qtyFieldStrm;
  
#pragma HLS data_pack variable=qtyFieldStrm
  class hls::stream< fdata_t  > extFieldStrm;
  
#pragma HLS data_pack variable=extFieldStrm
  class hls::stream< fdata_t  > dscFieldStrm;
  
#pragma HLS data_pack variable=dscFieldStrm
  class hls::stream< fdata_t  > shpFieldStrm;
  
#pragma HLS data_pack variable=shpFieldStrm
  class hls::stream< long  > qtyStrm;
  class hls::stream< long  > extStrm;
  class hls::stream< long  > dscStrm;
  class hls::stream< int  > shpStrm;
  class hls::stream< unsigned char  > dataStream;
  class hls::stream< short  > nTupsStrm[6];
  
#pragma HLS array_partition variable=nTupsStrm complete dim=1
  class hls::stream< bool  > filterDone;
  getHeaderInt2(byteStream,nTupsStrm,dataStream);
  q6_filter(nTupsStrm[0],dataStream,qtyFieldStrm,extFieldStrm,dscFieldStrm,shpFieldStrm,filterDone);
  toLong(nTupsStrm[1],qtyFieldStrm,qtyStrm);
  toLong(nTupsStrm[2],extFieldStrm,extStrm);
  toLong(nTupsStrm[3],dscFieldStrm,dscStrm);
  toDate(nTupsStrm[4],shpFieldStrm,shpStrm);
   *res = agg(nTupsStrm[5],qtyStrm,extStrm,dscStrm,shpStrm,filterDone);
}

static long q6(class hls::stream< unsigned char  > &bStream)
{
//  std::cout << "q6 jd19940101 " << jd19940101 << " jd19950101 " << jd19950101
//   << " discLo " << discountLow << " discHi " << discountHi 
//   << " qtyHi " << quantityHi << std::endl;
  long sum = (long )0;
  merlinL7:
  for (int i = 0; i < 2 * 1024 * 1024 / 16 / (8192 / 16) / 32; ++i) {
    long s;
    q6_paged(bStream,&s);
    sum += s;
//    std::cout << "Page [" << i << "] sum " << sum << std::endl;
  }
  return sum;
}

static void flowWrap(mywide_t (*inbuf)[32][512],long *outbuf)
{
  
#pragma HLS DATAFLOW
  class hls::stream< mywide_t  > inStream[32];
//hls::stream <mywide_t> inStream;
  
#pragma HLS STREAM variable=inStream depth=512
  
#pragma HLS data_pack variable=inStream
  class hls::stream< unsigned char  > byteStream[32];
  read((mywide_t (*)[32][512])inbuf,inStream);
  long sum = (long )0;
  merlinL0:
  for (int u = 0; u < 32; ++u) {
    
#pragma HLS UNROLL
    pageToByte(inStream[u],byteStream[u]);
    long pu_sum = q6(byteStream[u]);
    sum += pu_sum;
//    std::cout << "PU [" << u << "] pu_sum " << pu_sum << " sum " << sum << std::endl;
  }
   *outbuf = sum;
}
static int fpga_q6_dummy_pos;
#pragma ACCEL kernel
extern "C" { 

__kernel void fpga_q6(mywide_t inbuf[131072],long outbuf[1])
{
  
#pragma HLS INTERFACE s_axilite port=return bundle=control
  struct __wide inbuf_buf_0[8][32][512];
  memcpy(inbuf_buf_0[0][0],(const void *)(&inbuf[0]),sizeof(struct __wide ) * 131072);
  
#pragma HLS data_pack variable=inbuf
  
#pragma HLS data_pack variable=outbuf
//#pragma HLS INTERFACE m_axi port=inbuf offset=slave depth=262144 bundle=gmem0
//#pragma HLS INTERFACE m_axi port=outbuf offset=slave depth=256 bundle=gmem1
  
#pragma HLS INTERFACE m_axi port=inbuf offset=slave bundle=gmem0
  
#pragma HLS INTERFACE m_axi port=outbuf offset=slave bundle=gmem1
  
#pragma HLS INTERFACE s_axilite port=inbuf bundle=control
  
#pragma HLS INTERFACE s_axilite port=outbuf bundle=control
  
#pragma ACCEL INTERFACE variable=inbuf max_depth=131072 depth=262144
  
#pragma ACCEL INTERFACE variable=outbuf max_depth=1 depth=256
  
#pragma HLS inline region off
  flowWrap((mywide_t (*)[32][512])inbuf_buf_0,outbuf);
}
}
