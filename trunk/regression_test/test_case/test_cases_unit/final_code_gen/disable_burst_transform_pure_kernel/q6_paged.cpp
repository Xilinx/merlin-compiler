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
struct ::TupParser ;
static void __merlin_dummy_SgClassDeclaration_class_TupP_();

struct ::TupParser 
{
  int off;
  int pgOff;
}
;

void p_ZN9TupParserC2Ev(struct TupParser *this_)
{
  this_ -> off = 0;
  this_ -> pgOff = 2;
}

void p_ZN9TupParser8resetBufEv(struct TupParser *this_)
{
  this_ -> pgOff = 2;
}

void p_ZN9TupParser8startTupEv(struct TupParser *this_)
{
  this_ -> off = 0;
}

unsigned char p_ZN9TupParser7getByteER6streamIhE(struct TupParser *this_,hls::stream< unsigned char  > &stream)
{
  ++this_ -> pgOff;
  return stream . read();
}

int p_ZN9TupParser12getHeaderIntER6streamIhE(struct TupParser *this_,hls::stream< unsigned char  > &stream)
{
  unsigned char ch1;
  unsigned char ch2;
  ch1 = p_ZN9TupParser7getByteER6streamIhE(this_,stream);
  ch2 = p_ZN9TupParser7getByteER6streamIhE(this_,stream);
  int nTups = ((int )ch2) << 8 | ((int )ch1);
  return nTups;
}

unsigned int p_ZN9TupParser11getFieldLenEjR6streamIhE(struct TupParser *this_,unsigned int fPtr,hls::stream< unsigned char  > &stream)
{
  unsigned char ch;
  unsigned int fLen = (unsigned int )0;
  if (attrLens[fPtr] != - 1) {
    fLen = ((unsigned int )attrLens[fPtr]);
    if (align[fPtr]) {
      int aoff = (int )(((unsigned int )(this_ -> off)) + ((unsigned int )(4 - 1)) & ~((unsigned int )(4 - 1)));
      if (aoff != this_ -> off) {
        for (int i = this_ -> off; i < aoff; ++i) {
          
#pragma HLS LOOP_TRIPCOUNT min=0 max=1 avg=0
          ch = p_ZN9TupParser7getByteER6streamIhE(this_,stream);
          ++this_ -> off;
        }
      }
    }
    return fLen;
  }
  ch = p_ZN9TupParser7getByteER6streamIhE(this_,stream);
  ++this_ -> off;
  if ((bool )(((int )ch) & 0x01)) {
    fLen = ((unsigned int )(((int )ch) >> 1));
    return fLen - ((unsigned int )1);
  }
   else {
    return (unsigned int )0;
  }
}

bool p_ZN9TupParser8getFieldEPhjR6streamIhE(struct TupParser *this_,unsigned char fData[16],unsigned int fLen,hls::stream< unsigned char  > &stream)
{
  
#pragma HLS ARRAY_PARTITION variable=fData complete dim=0
  unsigned char ch;
  for (int i = 0; ((unsigned int )i) < fLen; ++i) {
    
#pragma HLS LOOP_TRIPCOUNT min=4 max=16 avg=8
    ch = p_ZN9TupParser7getByteER6streamIhE(this_,stream);
    fData[i] = ch;
  }
  this_ -> off += fLen;
  return true;
}

bool p_ZN9TupParser9skipBytesEiR6streamIhE(struct TupParser *this_,int fLen,hls::stream< unsigned char  > &stream)
{
  this_ -> off += fLen;
  for (int i = 0; i < fLen; ++i) {
    
#pragma HLS LOOP_TRIPCOUNT min=4 max=16 avg=8
    unsigned char ch;
    ch = p_ZN9TupParser7getByteER6streamIhE(this_,stream);
  }
  return true;
}

void p_ZN9TupParser13skipToPageEndER6streamIhE(struct TupParser *this_,hls::stream< unsigned char  > &stream)
{
  for (int i = this_ -> pgOff; i < 8192; ++i) {
    
#pragma HLS LOOP_TRIPCOUNT min=4 max=16 avg=8
    unsigned char ch;
    ch = p_ZN9TupParser7getByteER6streamIhE(this_,stream);
  }
}
static void __merlin_dummy_SgClassDeclaration_class_TupP_();
//----------------------------------------------------------------------------
// xql6 kernel
//
// 2017-06-16 18:49:58 parik 
//----------------------------------------------------------------------------
//#include <iostream>
//#include <cstdio>
//#include <string>
//#include <time.h> 
#include "q6_paged.h"
/*
 * Converts Postgres numeric/decimal to fixed-decimal (2 digits) long. 
 * fData is numeric field from tuple.
 */

long snumToLong3(unsigned char fData[16])
{
  int fsz = fData[16 - 1];
  bool bShort = (fData[1] & 0xC0) == 0x80;
  bool bSign = (fData[1] & 0x20);
  unsigned int scale = ((fData[1] & 0x1F) << 1 + (fData[0] & 0x80));
  char weight = (fData[0] & 0x7F);
  weight = (weight | (weight & 0x40) << 1);
  long res = 0;
// 2 bytes for short-hdr. 2 bytes per btdig
  int nTotDigs = (fsz - 2) / 2;
  int nWhlDigs = weight >= 0?weight + 1 : 0;
  int nFracDigs = (scale?scale / 4 + 1 : 0);
  int nZer0Digs = scale?0 : weight;
// 1 Frac digit
  int nDigs = nWhlDigs + 1;
  int fPtr = 2;
  for (int i = nDigs - 1; i >= 0; (--i , fPtr += 2)) {
    
#pragma HLS LOOP_TRIPCOUNT min=1 max=8 avg=6
    short dig = 0;
    if (fPtr < fsz) {
      dig = (fData[fPtr + 1] << 8 | fData[fPtr]);
    }
    if (i == 0) {
      res = res * 100 + (dig / 100);
    }
     else {
      res = res * 10000 + dig;
    }
  }
  if (bSign) 
    return (- 1) * res;
   else 
    return res;
}

int getJdate(unsigned char fData[16])
{
  int j = 0;
  for (int i = 4 - 1; i >= 0; --i) {
    j = j << 8 | fData[i];
  }
  return j;
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// dataflow module to process the page header and generate nTups for all
// dataflow modules.

void getHeaderInt2(hls::stream< unsigned char  > &stream,hls::stream< short  > nTupsStrm[6],hls::stream< unsigned char  > &outStream)
{
  unsigned char ch1;
  unsigned char ch2;
  ch1 = stream . read();
  ch2 = stream . read();
//pgOff +=2;
//if (!b1 || !b2) std::cout << "getHeaderInt failed b1 " << b1 << " b2 " << b2 << std::endl;
  short nTups = (ch2 << 8 | ch1);
  for (int scnt = 0; scnt < 6; ++scnt) {
    nTupsStrm[scnt] . write(nTups);
  }
  for (int bcnt = 2; bcnt < 8192; ++bcnt) {
    
#pragma HLS PIPELINE
    unsigned char ch = stream . read();
    outStream . write(ch);
  }
}
// dataflow module to convert wide data to byte stream

void pageToByte(hls::stream< mywide_t  > &win,hls::stream< unsigned char  > &out)
{
  for (int i = 0; i < 2 * 1024 * 1024 / 16 / (8192 / 16) / 32; ++i) {
    for (int i = 0; i < 8192 / 16; ++i) {
      mywide_t tmp = win . read();
      for (int j = 0; j < 16; ++j) {
        
#pragma HLS PIPELINE
        out . write(tmp . data[j]);
      }
    }
  }
}
// dataflow module to convert numerics to native format (long)

void toLong(hls::stream< short  > &nTupsStrm,hls::stream< fdata_t  > &fieldStrm,hls::stream< long  > &longStrm)
{
  short nTups = nTupsStrm . read();
  for (short i = 0; i < nTups; ++i) {
    fdata_t tmp = fieldStrm . read();
    long l = snumToLong3(tmp . data);
    longStrm . write(l);
  }
}
// dataflow module to convert date field to int

void toDate(hls::stream< short  > &nTupsStrm,hls::stream< fdata_t  > &fieldStrm,hls::stream< int  > &dateStrm)
{
  short nTups = nTupsStrm . read();
  for (short i = 0; i < nTups; ++i) {
    
#pragma HLS PIPELINE
    fdata_t tmp = fieldStrm . read();
    int d = getJdate(tmp . data);
    dateStrm . write(d);
  }
}
// dataflow module to consume the native format streams and aggregate

long agg(hls::stream< short  > &nTupsStrm,hls::stream< long  > &qtyStrm,hls::stream< long  > &extStrm,hls::stream< long  > &dscStrm,hls::stream< int  > &shpStrm,hls::stream< bool  > &filterDone)
{
  short nTups = nTupsStrm . read();
  long sum = 0;
  for (short i = 0; i < nTups; ++i) {
    
#pragma HLS PIPELINE
    long quantity = qtyStrm . read();
    long extPrice = extStrm . read();
    long discount = dscStrm . read();
    int shipDate = shpStrm . read();
    if (shipDate >= - 2191 && shipDate < - 1826 && discount >= 5 && discount <= 7 && quantity < 2400) {
      sum += extPrice * discount;
//std::cout << "Matched. sum " << sum << std::endl;
//std::cout << "Tup [" << nTups << "] " << quantity <<"|" << extPrice 
//  << "|" << discount << "|" << shipDate << std::endl;
    }
  }
//  bool eop = filterDone. read ();
  return sum;
}
// dataflow module to parse tuples and stream fields of interest for query-6

void q6_filter(hls::stream< short  > &nTupsStrm,hls::stream< unsigned char  > &byteStream,hls::stream< fdata_t  > &qtyFieldStrm,hls::stream< fdata_t  > &extFieldStrm,hls::stream< fdata_t  > &dscFieldStrm,hls::stream< fdata_t  > &shpFieldStrm,hls::stream< bool  > &filterDone)
{
  struct TupParser prsr;
  p_ZN9TupParserC2Ev(&prsr);
  short nTups = nTupsStrm . read();
  for (short tcnt = 0; tcnt < nTups; ++tcnt) {
    
#pragma HLS LOOP_TRIPCOUNT min=50 max=70 avg=60
    p_ZN9TupParser8startTupEv(&prsr);
    for (unsigned char fPtr = 0; fPtr < nAttr; ++fPtr) {
      unsigned char fLen = (p_ZN9TupParser11getFieldLenEjR6streamIhE(&prsr,fPtr,byteStream));
//std::cout << "Tup [" << nTups << "] Field [" << fPtr << "] Len " << fLen << std::endl;
//bool b1 = prsr. getField (fLen, byteStream);
//if (!b1) std::cout << "Bad EOF at fPtr " << fPtr << " " << tup << " " << nTups << std::endl;
//      Dbg2 std::cout << "Tup [" << tcnt << "] F [" << fPtr << "] " << fLen << std::endl;
      if (fPtr == 4 || fPtr == 5 || fPtr == 6 || fPtr == 10) {
        fdata_t tmp;
        p_ZN9TupParser8getFieldEPhjR6streamIhE(&prsr,tmp . data,fLen,byteStream);
        tmp . data[16 - 1] = fLen;
        if (fPtr == 4) 
          qtyFieldStrm . write(tmp);
         else if (fPtr == 5) 
          extFieldStrm . write(tmp);
         else if (fPtr == 6) 
          dscFieldStrm . write(tmp);
         else 
          shpFieldStrm . write(tmp);
      }
       else {
        p_ZN9TupParser9skipBytesEiR6streamIhE(&prsr,fLen,byteStream);
      }
    }
  }
  p_ZN9TupParser13skipToPageEndER6streamIhE(&prsr,byteStream);
// write eof
//  filterDone. write (true);
}
// defines a processing-unit (PU) to compute query-6. Consumes a single page of
// tuples streamed byte-wise and generates aggregate

void q6_paged(hls::stream< unsigned char  > &byteStream,long *res)
{
  
#pragma HLS DATAFLOW
  hls::stream< fdata_t  > qtyFieldStrm;
  
#pragma HLS data_pack variable=qtyFieldStrm
  hls::stream< fdata_t  > extFieldStrm;
  
#pragma HLS data_pack variable=extFieldStrm
  hls::stream< fdata_t  > dscFieldStrm;
  
#pragma HLS data_pack variable=dscFieldStrm
  hls::stream< fdata_t  > shpFieldStrm;
  
#pragma HLS data_pack variable=shpFieldStrm
  hls::stream< long  > qtyStrm;
  hls::stream< long  > extStrm;
  hls::stream< long  > dscStrm;
  hls::stream< int  > shpStrm;
  hls::stream< unsigned char  > dataStream;
  hls::stream< short  > nTupsStrm[6];
  hls::stream< bool  > filterDone;
  getHeaderInt2(byteStream,nTupsStrm,dataStream);
  q6_filter(nTupsStrm[0],dataStream,qtyFieldStrm,extFieldStrm,dscFieldStrm,shpFieldStrm,filterDone);
  toLong(nTupsStrm[1],qtyFieldStrm,qtyStrm);
  toLong(nTupsStrm[2],extFieldStrm,extStrm);
  toLong(nTupsStrm[3],dscFieldStrm,dscStrm);
  toDate(nTupsStrm[4],shpFieldStrm,shpStrm);
   *res = agg(nTupsStrm[5],qtyStrm,extStrm,dscStrm,shpStrm,filterDone);
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// dataflow module for DDR Reader. Feeds each PU's input stream in a round-robin
// manner.

void read(mywide_t *inbuf,hls::stream< mywide_t  > inStream[32])
{
//16
  for (int i = 0; i < 2 * 1024 * 1024 / 16 / (8192 / 16) / 32; ++i) {
// 16
    for (int u = 0; u < 32; ++u) {
// 1024
      for (int j = 0; j < 8192 / 16; ++j) {
        
#pragma HLS PIPELINE
        mywide_t tmpData = inbuf[i * 32 * (8192 / 16) + u * (8192 / 16) + j];
        inStream[u] . write(tmpData);
        if (g_dbgLvl && j == 0) {
//          for (int k=0; k<WORD_SZ; ++k) printf ("%02x ", tmpData. data [k]);
//          std::cout << std::endl;
        }
      }
    }
  }
}
// wrapper for q6-PU

long q6(hls::stream< unsigned char  > &bStream)
{
//  std::cout << "q6 jd19940101 " << jd19940101 << " jd19950101 " << jd19950101
//   << " discLo " << discountLow << " discHi " << discountHi 
//   << " qtyHi " << quantityHi << std::endl;
  long sum = 0;
  for (int i = 0; i < 2 * 1024 * 1024 / 16 / (8192 / 16) / 32; ++i) {
    long s;
    q6_paged(bStream,&s);
    sum += s;
//    std::cout << "Page [" << i << "] sum " << sum << std::endl;
  }
  return sum;
}
// top level module

void flowWrap(mywide_t *inbuf,long *outbuf)
{
  
#pragma HLS DATAFLOW
  hls::stream< mywide_t  > inStream[32];
//hls::stream <mywide_t> inStream;
  
#pragma HLS STREAM variable=inStream depth=512
  
#pragma HLS data_pack variable=inStream
  hls::stream< unsigned char  > byteStream[32];
  read(inbuf,inStream);
  long sum = 0;
  for (int u = 0; u < 32; ++u) {
    
#pragma HLS UNROLL
    pageToByte(inStream[u],byteStream[u]);
    long pu_sum = q6(byteStream[u]);
    sum += pu_sum;
//    std::cout << "PU [" << u << "] pu_sum " << pu_sum << " sum " << sum << std::endl;
  }
   *outbuf = sum;
}
extern "C" {
static void __merlin_dummy_kernel_pragma();

void fpga_q6(mywide_t inbuf[2 * 1024 * 1024 / 16],long outbuf[1])
{
  
#pragma HLS data_pack variable=inbuf
  
#pragma HLS data_pack variable=outbuf
//#pragma HLS INTERFACE m_axi port=inbuf offset=slave depth=262144 bundle=gmem0
//#pragma HLS INTERFACE m_axi port=outbuf offset=slave depth=256 bundle=gmem1
  
#pragma HLS INTERFACE m_axi port=inbuf offset=slave bundle=gmem0
  
#pragma HLS INTERFACE m_axi port=outbuf offset=slave bundle=gmem1
  
#pragma HLS INTERFACE s_axilite port=inbuf bundle=control
  
#pragma HLS INTERFACE s_axilite port=outbuf bundle=control
  
#pragma HLS INTERFACE s_axilite port=return bundle=control
  
#pragma ACCEL INTERFACE variable=inbuf depth=262144
  
#pragma ACCEL INTERFACE variable=outbuf depth=256
  
#pragma HLS inline region off
  flowWrap(inbuf,outbuf);
}
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
