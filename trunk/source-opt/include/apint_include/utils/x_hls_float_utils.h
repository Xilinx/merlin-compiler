// (C) Copyright 2016-2021 Xilinx, Inc.
// All Rights Reserved.
//
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

// (C) Copyright 2016-2021 Xilinx, Inc.
// All Rights Reserved.
//
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.


#ifndef X_HLS_FLOAT_UTILS_H
#define X_HLS_FLOAT_UTILS_H
#include <assert.h>

static const ap_uint<100> ref_cordic("9b74eda8435e5a67f5f9092c6",16);

//Q115.397
static const ap_uint<512> ref_4oPi_512("000000000000000000000000000028BE60DB9391054A7F09D5F47D4D377036D8A5664F10E4107F9458EAF7AEF1586DC91B8E909374B801924BBA827464873FBF",16);

/*
* Double-precision calculations
* hi: 400-1-2+( 53-e1+1)    = 451-e1    = 399 (e1=52)
* lo: 400-1-2+(-53-e1-1-p1) = 343-e1-p1 = 281 (e1=52,p1=10)
*
* left-justified alignment
* e1	hi	lo	i	<<
* ----	---	---	---	-------
* 52	399	281	1	0
* 53	398	281	1	1
* 61			1	9
* 179			1	127
* 180			2	0
*
*-------------------------------------------------------------------
* Need to select 119-bits from ~1024 bits.
* 256 bits allows 128-bit selection shifted 128 bits.
* 128-bits is bigger than 119 which we need in the double-precision
* range reduction payne-hanek.
* 128-bits * 10 = 1280 which is > 1024 bits
*-------------------------------------------------------------------
*/
static const ap_uint<256> ref_4oPi_table_256[10] = {
    "0x0000000000000000000000000000000028be60db9391054a7f09d5f47d4d3770",
    "0x28be60db9391054a7f09d5f47d4d377036d8a5664f10e4107f9458eaf7aef158",
    "0x36d8a5664f10e4107f9458eaf7aef1586dc91b8e909374b801924bba82746487",
    "0x6dc91b8e909374b801924bba827464873f877ac72c4a69cfba208d7d4baed121",
    "0x3f877ac72c4a69cfba208d7d4baed1213a671c09ad17df904e64758e60d4ce7d",
    "0x3a671c09ad17df904e64758e60d4ce7d272117e2ef7e4a0ec7fe25fff7816603",
    "0x272117e2ef7e4a0ec7fe25fff7816603fbcbc462d6829b47db4d9fb3c9f2c26d",
    "0xfbcbc462d6829b47db4d9fb3c9f2c26dd3d18fd9a797fa8b5d49eeb1faf97c5e",
    "0xd3d18fd9a797fa8b5d49eeb1faf97c5ecf41ce7de294a4ba9afed7ec47e35742",
    "0xcf41ce7de294a4ba9afed7ec47e357421580cc11bf1edaeafc33ef0826bd0d87",
};

static const ap_uint<100> ref_4oPi_table_100[13] = {
    "0x000000000000000000000028B", // -4
    "0x00000000000000000028BE60D", // -3
    "0x0000000000000028BE60DB939", // -2
    "0x000000000028BE60DB9391054", // -1
    "0x00000028BE60DB9391054A7F0", // 0
    "0x0028BE60DB9391054A7F09D5F", // 1
    "0xBE60DB9391054A7F09D5F47D4", // 2
    "0xDB9391054A7F09D5F47D4D377", // 3
    "0x91054A7F09D5F47D4D377036D", // 4
    "0x4A7F09D5F47D4D377036D8A56", // 5
    "0x09D5F47D4D377036D8A5664F1", // 6
    "0xF47D4D377036D8A5664F10E41", // 7
    "0x4D377036D8A5664F10E417F94", // 8
};

static const ap_uint<80> ref_4oPi_table_80[9] = {
    "0x00000028BE60DB939105", // 0
    "0x0028BE60DB9391054A7F", // 1
    "0xBE60DB9391054A7F09D5", // 2
    "0xDB9391054A7F09D5F47D", // 3
    "0x91054A7F09D5F47D4D37", // 4
    "0x4A7F09D5F47D4D377036", // 5
    "0x09D5F47D4D377036D8A5", // 6
    "0xF47D4D377036D8A5664F", // 7
    "0x4D377036D8A5664F10E4", // 8
};


// Q2.398
static const ap_uint<400> ref_Pio4_400("3243F6A8885A308D313198A2E03707344A4093822299F31D0082EFA98EC4E6C89452821E638D01377BE5466CF34E90C6CC0A",16);

static const ap_uint<256> ref_Pio4_table[10] = {
    "0x000000000000000000000000000000003243f6a8885a308d313198a2e0370734",
    "0x3243f6a8885a308d313198a2e03707344a4093822299f31d0082efa98ec4e6c8",
    "0x4a4093822299f31d0082efa98ec4e6c89452821e638d01377be5466cf34e90c6",
    "0x9452821e638d01377be5466cf34e90c6cc0ac29b7c97c50dd3f84d5b5b547091",
    "0xcc0ac29b7c97c50dd3f84d5b5b54709179216d5d98979fb1bd1310ba698dfb5a",
    "0x79216d5d98979fb1bd1310ba698dfb5ac2ffd72dbd01adfb7b8e1afed6a267e9",
    "0xc2ffd72dbd01adfb7b8e1afed6a267e96ba7c9045f12c7f9924a19947b3916cf",
    "0x6ba7c9045f12c7f9924a19947b3916cf70801f2e2858efc16636920d871574e6",
    "0x70801f2e2858efc16636920d871574e69a458fea3f4933d7e0d95748f728eb65",
    "0x9a458fea3f4933d7e0d95748f728eb658718bcd5882154aee7b54a41dc25a59b",
};


template <int W, int I>
void scaled_fixed2ieee(ap_fixed<W, I> in, half &result, int prescale)
;

template <int W, int I>
void scaled_fixed2ieee(ap_ufixed<W, I> in, half &result, int prescale)
;

#define __hls_parts ((W+15)/16)
#define __hls_rem (W - (__hls_parts-1)*16)
template <int W, int I>
void scaled_fixed2ieee(ap_fixed<W, I> in, float &result, int prescale)
;


template <int W, int I>
void scaled_fixed2ieee(ap_ufixed<W, I> in, float &result, int prescale)
;

template <int W, int I>
void scaled_fixed2ieee(ap_fixed<W, I> in, double &result, int prescale)
;

template <int W, int I>
void scaled_fixed2ieee(ap_ufixed<W, I> in, double &result, int prescale)
;
#undef __hls_parts
#undef __hls_rem

#endif



