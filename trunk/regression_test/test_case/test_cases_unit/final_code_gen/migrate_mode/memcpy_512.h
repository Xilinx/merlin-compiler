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
#ifndef __MERLING_MEMCPY_512_INTERFACE_H_
#define __MERLING_MEMCPY_512_INTERFACE_H_
#ifdef __cplusplus
#include <ap_int.h>
#else
#include <assert.h>
#include <stdint.h>
typedef struct merlin_uint_512 {
  unsigned char data[512 / 8];
} merlin_uint_512;
static uint64_t merlin_get_range_512(merlin_uint_512 *tmp, int low, int len) {
#pragma HLS INLINE SELF
  uint64_t ret = 0;
  int i;
  int start = low;
  switch (len) {
  case 1:
    for (i = 0; i >= 0; --i) {
#pragma HLS unroll
      ret <<= 8;
      ret |= tmp->data[i + start];
    }
    break;
  case 2:
    for (i = 1; i >= 0; --i) {
#pragma HLS unroll
      ret <<= 8;
      ret |= tmp->data[i + start];
    }
    break;
  case 4:
    for (i = 3; i >= 0; --i) {
#pragma HLS unroll
      ret <<= 8;
      ret |= tmp->data[i + start];
    }
    break;
  case 8:
    for (i = 7; i >= 0; --i) {
#pragma HLS unroll
      ret <<= 8;
      ret |= tmp->data[i + start];
    }
    break;
  default:
    assert(0);
  }
  return ret;
}

static void merlin_set_range_512(merlin_uint_512 *tmp, int low, int len,
                                 uint64_t val) {
#pragma HLS INLINE SELF
  int i;
  int start = low;
  switch (len) {
  case 1:
    for (i = 0; i < 1; ++i) {
#pragma HLS unroll
      tmp->data[start + i] = val & 0xff;
      val >>= 8;
    }
    break;
  case 2:
    for (i = 0; i < 2; ++i) {
#pragma HLS unroll
      tmp->data[start + i] = val & 0xff;
      val >>= 8;
    }
    break;
  case 4:
    for (i = 0; i < 4; ++i) {
#pragma HLS unroll
      tmp->data[start + i] = val & 0xff;
      val >>= 8;
    }
    break;
  case 8:
    for (i = 0; i < 8; ++i) {
#pragma HLS unroll
      tmp->data[start + i] = val & 0xff;
      val >>= 8;
    }
    break;
  default:
    assert(0);
  }
}
#endif
#endif
