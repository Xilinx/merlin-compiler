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




#include "cl_kernel.h"
#include "autopilot_tech.h"
#include "AllInOne.h"

const int cwidth = BITS_COEF;
const int shift = BITS_SHIFT;

const int inv_coeff[8][8] = {
    {2048, 2048, 2048, 2048, 2048, 2048, 2048, 2048},
    {2841, 2408, 1609, 565, -565, -1609, -2408, -2841},
    {2676, 1108, -1108, -2676, -2676, -1108, 1108, 2676},
    {2408, -565, -2841, -1609, 1609, 2841, 565, -2408},
    {2048, -2048, -2048, 2048, 2048, -2048, -2048, 2048},
    {1609, -2841, 565, 2408, -2408, -565, 2841, -1609},
    {1108, -2676, 2676, -1108, -1108, 2676, -2676, 1108},
    {565, -1609, 2408, -2841, 2841, -2408, 1609, -565}};

#define IMG_WIDTH 16
void idctcol(int16 blk[64], char dc_scaler) {
#pragma AP inline self

  int16 x[B_WIDTH];
#pragma AP ARRAY_PARTITION variable = x complete dim = 1
#pragma AP ARRAY_PARTITION variable = inv_coeff complete dim = 1
  //#pragma AP resource type=memory variable=inv_coeff core=SPROMD_COMB
  short negMaxval;
  int tmpInt, i, j;

  if (dc_scaler)
    negMaxval = 0;
  else
    negMaxval = -MAXVAL_PIXEL - 1;
  for (i = 0; i < B_WIDTH; i++) {
    //#pragma AP unroll complete
    x[i] = blk[i * B_WIDTH];
  }

  for (j = 0; j < B_WIDTH; j++) {
#pragma AP pipeline II = 1
    tmpInt = 0;
    for (i = 0; i < B_WIDTH; i++) {
      //#pragma AP unroll complete//if outer loop pipelined, it will unroll
      // automatically
      tmpInt += x[i] * inv_coeff[i][j];
    }

    tmpInt = (tmpInt + (1 << (cwidth + shift))) >> (cwidth + shift + 1);
    blk[j * 8] = (tmpInt < negMaxval)
                     ? negMaxval
                     : (tmpInt > MAXVAL_PIXEL) ? MAXVAL_PIXEL : tmpInt;
  }
}  //

void idct_col_kernel(int16 *in, int32 *out, char dc_scaler) {
#pragma AP interface ap_ctrl_none port = return

#pragma tldm iteration_domain iterator = "bx, by" max_range = "2, 2"
//#pragma tldm tuning iterator="bx, by" order="by, bx" tile_size="2, 2"
// additional = "val0"
#pragma tldm tuning iterator = "bx, by" order =                                  \
    "i, j, ii, jj : 0<=ii; ii<2; 0<=jj; jj<2; ii=bx-2*i; jj=by-2*j" additional = \
        "val1"
#pragma tldm access_pattern name = "in_ref" port = "in" dir = "r" iterator =   \
    "bx, by" pattern =                                                         \
        "bx*8+i + (by*8+j) * 16 : 0<=i; i<8; 0<=j; j<8" additional =           \
            "val2"  // derivable
#pragma tldm access_pattern name = "out_ref" port = "out" dir = "w" iterator = \
    "bx, by" pattern =                                                         \
        "bx*8+i + (by*8+j) * 16 : 0<=i; i<8; 0<=j; j<8"  // derivable

  //#pragma tldm address_map port=in  iterator="bx, by" parameter="i, j"
  // map="bx*8+i + (by*8+j) * 16 : i + j*8" #pragma tldm address_map port=out
  // iterator="bx, by" parameter="i, j" map="bx*8+i + (by*8+j) * 16 : i + j*8"

  int i, j;
  int bx = get_global_id(0);
  int by = get_global_id(1);

  int16 blk[64];

  for (i = 0; i < 8; i++) {
    for (j = 0; j < 8; j++) {
      blk[i + j * 8] = in[bx * 8 + i + (by * 8 + j) * IMG_WIDTH];
    }
  }
  idctcol(blk, dc_scaler);
  for (i = 0; i < 8; i++) {
    for (j = 0; j < 8; j++) {
      out[bx * 8 + i + (by * 8 + j) * IMG_WIDTH] = blk[i + j * 8];
    }
  }
}

void idct_opencl_kernel_wrapper(int16 *in, int32 *out, char dc_scaler) {
  int i, j;
  for (j = 0; j < 2; j++) {
    for (i = 0; i < 2; i++) {
      set_global_id(0, i);
      set_global_id(1, j);
      idct_opencl_kernel(in, out, dc_scaler);
    }
  }
}
