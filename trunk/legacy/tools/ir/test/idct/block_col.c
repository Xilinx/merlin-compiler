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


#include "stdio.h"
#include "autopilot_tech.h"
//#include "AllInOne.h"

#include "tldm_if.h"
// IDCT constants

#define IMG_WIDTH 16

// const int cwidth = BITS_COEF;
// const int shift = BITS_SHIFT;
//
// const int inv_coeff[8][8]={{2048,2048,2048,2048,2048,2048,2048,2048},
// 	{2841,2408,1609,565,-565,-1609,-2408,-2841},
// 	{2676,1108,-1108,-2676,-2676,-1108,1108,2676},
// 	{2408,-565,-2841,-1609,1609,2841,565,-2408},
// 	{2048,-2048,-2048,2048,2048,-2048,-2048,2048},
// 	{1609,-2841,565,2408,-2408,-565,2841,-1609},
// 	{1108,-2676,2676,-1108,-1108,2676,-2676,1108},
// 	{565,-1609,2408,-2841,2841,-2408,1609,-565}
// };

void idctcol(int16 blk[64], char dc_scaler);

// void idctcol(int16 blk[64], char dc_scaler)
// {
// #pragma AP inline self
//
// 	int16 x[B_WIDTH];
// #pragma AP ARRAY_PARTITION variable=x complete dim=1
// #pragma AP ARRAY_PARTITION variable=inv_coeff complete dim=1
// 	//#pragma AP resource type=memory variable=inv_coeff core=SPROMD_COMB
// 	short negMaxval;
// 	int tmpInt, i, j;
//
// 	if (dc_scaler)
// 		negMaxval = 0;
// 	else
// 		negMaxval = -MAXVAL_PIXEL - 1;
// 	for (i = 0; i < B_WIDTH; i++)
// 	{
// //#pragma AP unroll complete
// 		x[i] = blk[i*B_WIDTH];
// 	}
//
// 	for (j = 0; j < B_WIDTH; j++)
// 	{
// #pragma AP  pipeline II=1
// 		tmpInt = 0;
// 		for (i = 0; i < B_WIDTH; i++) {
// //#pragma AP unroll complete//if outer loop pipelined, it will unroll
// automatically 			tmpInt += x[i]*inv_coeff[i][j];
// 		}
//
// 		tmpInt = (tmpInt + (1 << (cwidth+shift))) >> (cwidth+shift+1);
// 		blk[j*8] = (tmpInt < negMaxval) ? negMaxval : (tmpInt > MAXVAL_PIXEL) ?
// MAXVAL_PIXEL : tmpInt;
// 	}
// }//

void idctcol_design(tldm_array<int16> &in, tldm_array<int32> &out,
                    char dc_scaler, int bx, int by) {
  int i, j;
  int16 blk[64];

  for (i = 0; i < 8; i++) {
    for (j = 0; j < 8; j++) {
#pragma AP pipeline
      blk[i + j * 8] =
          in.read(by * 8 + j, bx * 8 + i);  //[bx*8+i + (by*8+j) * IMG_WIDTH];
    }
  }
  idctcol(blk, dc_scaler);
  for (i = 0; i < 8; i++) {
    for (j = 0; j < 8; j++) {
#pragma AP pipeline
      out.write(
          by * 8 + j, bx * 8 + i,
          blk[i + j * 8]);  //[bx*8+i + (by*8+j) * IMG_WIDTH] = blk[i+j*8];
    }
  }
}

// Block IDCT function
//

void idct_col(volatile int16 *row2col_fifo, volatile int32 *text_fifo) {
#pragma AP interface ap_ctrl_none port = return

#pragma AP interface ap_fifo port = row2col_fifo
#pragma AP interface ap_fifo port = text_fifo

#pragma AP resource core = FSL variable = row2col_fifo
#pragma AP resource core = FSL variable = text_fifo

  //#pragma AP resource core = AXI4Stream variable = row2col_fifo
  // port_map={{row2col_fifo TDATA}} #pragma AP resource core = AXI4Stream
  // variable = text_fifo port_map={{text_fifo TDATA}}

  unsigned char i, j;
  char dc_scaler;
  int16 out[64];
  //#pragma AP resource type=memory variable=out core=DPRAMD
  int k;
  /* Comments from Muhuan
   * divide latency by 10 to get the runtime of one iteration
   */
  // for(k = 0; k < 10; k++)
  // while(1)
  {
#pragma AP loop_tripcount min = 2 avg = 2 max = 2

    int16 tmpdc = *row2col_fifo;
    dc_scaler = tmpdc & 0xff;
    for (j = 0; j < 64; j++) {
#pragma AP pipeline
      out[j] = *row2col_fifo;
    }

    // 1-D IDCT on columns
    for (i = 0; i < 8; i++) {
      idctcol(out + i, dc_scaler);
    }

    for (i = 0; i < 32; i++) {
#pragma AP pipeline
      *text_fifo = ((out[i * 2 + 1] << 16) | out[i * 2]);
    }

    // if (((tmpdc>>11)&1) != ((tmpdc>>10)&1)) break;  // add by pengzhang to
    // make the while loop finite
  }
}  // BlockIDCT

int block_cnt = 4;

int test_tldm(int *input_all, int *output_all) {
  int i;

  tldm_fifo<int16> *row2col_fifo;
  tldm_fifo<int32> *text_fifo;

  for (i = 0; i < block_cnt; i++) {
    // idct_col(row2col_fifo, text_fifo);
  }

  return 1;
}

int test_ref(tldm_array<int16> &input_all, tldm_array<int32> &output_all) {
  int i, j;
  for (i = 0; i < 2; i++)
    for (j = 0; j < 2; j++) {
      idctcol_design(input_all, output_all, 1, i, j);
    }
}

void idct_opencl_kernel_wrapper(int16 *in, int32 *out, char dc_scaler);

void test_opencl(tldm_array<int16> &input_all, tldm_array<int32> &output_all) {
  int i;
  int16 in[16 * 16];
  int32 out[16 * 16];

  for (i = 0; i < 16 * 16; i++)
    in[i] = input_all.read(i / 16, i % 16);
  idct_opencl_kernel_wrapper(in, out, 1);
  for (i = 0; i < 16 * 16; i++)
    output_all.write(i / 16, i % 16, out[i]);
}

int main() {
  int i;
  tldm_array<int16> input_all(2, 16, 16);
  tldm_array<int32> output_all(2, 16, 16);

  for (i = 0; i < 16 * 16; i++) {
    int val = i;
    input_all.write(i / 16, i % 16, val);
  }

  // test_ref(input_all, output_all);
  test_opencl(input_all, output_all);
  // test_tldm();

  for (i = 0; i < 16 * 16; i++) {
    printf("%d ", output_all.read(i / 16, i % 16));
    if (i % 8 == 7)
      printf("\n");
    if (i % 64 == 63)
      printf("\n");
  }
}
