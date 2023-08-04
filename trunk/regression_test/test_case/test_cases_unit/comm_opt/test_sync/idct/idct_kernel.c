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
#include "AllInOne.h"
#include <math.h>
#define TOTAL_MEM_SIZE 600000 //(3*352*288)
#define mb_height  18
#define mb_width   22
#define IMG_NUM    2
#define img 0
const int cwidth = 13;
const int shift = 3;
const int inv_coeff[8][8] = {{(2048), (2048), (2048), (2048), (2048), (2048), (2048), (2048)}, {(1841), (2408), (1609), (565), (- 565), (- 1609), (- 2408), (- 1841)}, {(2676), (1108), (- 1108), (- 2676), (- 2676), (- 1108), (1108), (2676)}, {(2408), (- 565), (- 1841), (- 1609), (1609), (1841), (565), (- 2408)}, {(2048), (- 2048), (- 2048), (2048), (2048), (- 2048), (- 2048), (2048)}, {(1609), (- 1841), (565), (2408), (- 2408), (- 565), (1841), (- 1609)}, {(1108), (- 2676), (2676), (- 1108), (- 1108), (2676), (- 2676), (1108)}, {(565), (- 1609), (2408), (- 1841), (1841), (- 2408), (1609), (- 565)}};
void idct_row(int blk,int mbx,int mby,int *dequant_data,int *row2col_data,int *row2col_dc);
/*       int img,
        int mb_width,
        int mb_height,*/
void idct_col(int blk,int mbx,int mby,int *row2col_data,int *row2col_dc,int *text_data);
/*       int img,
        int mb_width,
        int mb_height,*/
static void __merlin_dummy_0();
/* Original: #pragma ACCEL kernel name="idct" */

#pragma ACCEL kernel
void idct_kernel(
/*       int img,
        int mb_width,
        int mb_height,*/
int *dequant_data,int *row2col_data,int *text_data)
{
  int _memcpy_i1_3;
  int _memcpy_i1_2;
  int _memcpy_i1_1;
  int _memcpy_i1_0;
  int _memcpy_i0_4;
  int _memcpy_i0_3;
  int _memcpy_i0_2;
  int _memcpy_i0_1;
  int _memcpy_i0_0;
  int dequant_data_buf_0[18][22][6][8][8];
  int blk_0_0;
  int mbx_0_0;
  int mby_0_0;
  int row2col_data1_0_0[72][22][6][8][8];
  int row2col_dc_0_0;
  int text_data_buf_0_0[22][6][8][8];
  int mby__idct_row_0;
  int mbx__idct_row_0;
  int blk__idct_row_0;
  int i__idct_row_0;
  int j__idct_row_0;
  int mb_idx__idct_row_0;
  int blk_idx0_0__idct_row_0;
  int blk_idx1_0__idct_row_0;
  int blk_idx2_0__idct_row_0;
  int blk_idx3_0__idct_row_0;
  int dc_value_0__idct_row_0;
  int blk_idx_0__idct_row_0;
  int tmpval_0__idct_row_0;
  int tmpval2_0__idct_row_0;
  int x_0__idct_row_0;
  int ii_0__idct_row_0;
  int j_0__idct_row_0;
  int idx2_0__idct_row_0;
  int idx_0__idct_row_0;
  int mby__idct_col_0;
  int mbx__idct_col_0;
  int blk__idct_col_0;
  int i__idct_col_0;
  int j__idct_col_0;
  char dc_scaler__idct_col_0;
  int out__idct_col_0[64];
  int tmpval__idct_col_0;
  int k__idct_col_0;
  int blk_idx0__idct_col_0;
  int blk_idx1_0__idct_col_0;
  int blk_idx2_0__idct_col_0;
  int blk_idx3_0__idct_col_0;
  int mb_idx_0__idct_col_0;
  int blk_idx_0__idct_col_0;
  int idx_0__idct_col_0;
  int idx2_0__idct_col_0;
  int tmpdc_0__idct_col_0;
  int x_0__idct_col_0;
  short negMaxval_0__idct_col_0;
  int tmpInt_0__idct_col_0;
  int ii_0__idct_col_0;
  int j_0__idct_col_0;
  int tmp_0__idct_col_0;
  for (_memcpy_i0_4 = 0; _memcpy_i0_4 < 18; ++_memcpy_i0_4) {
    for (_memcpy_i0_3 = 0; _memcpy_i0_3 < 22; ++_memcpy_i0_3) {
      for (_memcpy_i0_2 = 0; _memcpy_i0_2 < 6; ++_memcpy_i0_2) {
        for (_memcpy_i0_1 = 0; _memcpy_i0_1 < 8; ++_memcpy_i0_1) {
          for (_memcpy_i0_0 = 0; _memcpy_i0_0 < 8; ++_memcpy_i0_0) {
            dequant_data_buf_0[_memcpy_i0_4][_memcpy_i0_3][_memcpy_i0_2][_memcpy_i0_1][_memcpy_i0_0] = dequant_data[0 + (((((0 * 18 + _memcpy_i0_4) * 22 + _memcpy_i0_3) * 6 + _memcpy_i0_2) * 8 + _memcpy_i0_1) * 8 + _memcpy_i0_0)];
          }
        }
      }
    }
  }
  
#pragma ACCEL interface variable=text_data depth=600000 max_depth=600000
  
#pragma ACCEL interface variable=row2col_data depth=600000 max_depth=600000
  
#pragma ACCEL interface variable=dequant_data depth=600000 max_depth=600000
  for (mby_0_0 = 0; mby_0_0 < 18; mby_0_0++) {
    
#pragma HLS loop_tripcount max=18
    for (mbx_0_0 = 0; mbx_0_0 < 22; mbx_0_0++) {
      
#pragma HLS loop_tripcount max=22
//#pragma ACCEL pipeline
      for (blk_0_0 = 0; blk_0_0 < 6; blk_0_0++) {{
// 1-D IDCT on rows
          for (i__idct_row_0 = 0; i__idct_row_0 < 8; i__idct_row_0++) {
            for (j_0__idct_row_0 = 0; j_0__idct_row_0 < 8; j_0__idct_row_0++) 
/* Original: #pragma ACCEL pipeline flatten */
{
              
#pragma ACCEL PIPELINE II=1
              tmpval_0__idct_row_0 = 0;
              for (ii_0__idct_row_0 = 0; ii_0__idct_row_0 < 8; ii_0__idct_row_0++) {
                
#pragma ACCEL PARALLEL COMPLETE
                x_0__idct_row_0 = dequant_data_buf_0[mby_0_0][mbx_0_0][blk_0_0][j_0__idct_row_0][ii_0__idct_row_0];
                tmpval_0__idct_row_0 += x_0__idct_row_0 * inv_coeff[ii_0__idct_row_0][j_0__idct_row_0];
              }
              tmpval2_0__idct_row_0 = tmpval_0__idct_row_0 + (1 << cwidth - shift - 3) >> cwidth - shift - 2;
              row2col_data1_0_0[mby_0_0][mbx_0_0][blk_0_0][i__idct_row_0][j_0__idct_row_0] = tmpval2_0__idct_row_0;
              if (i__idct_row_0 == 0 && j_0__idct_row_0 == 0) {
                dc_value_0__idct_row_0 = tmpval2_0__idct_row_0;
              }
            }
          }
/*[blk_idx]*/
// BlockIDCT
        }
{
// 1-D IDCT on columns
          for (i__idct_col_0 = 0; i__idct_col_0 < 8; i__idct_col_0++) {
            if ((char )(dc_value_0__idct_row_0 & 0xff)) {
              negMaxval_0__idct_col_0 = ((short )0);
            }
             else {
              negMaxval_0__idct_col_0 = ((short )(- 255 - 1));
            }
            for (j_0__idct_col_0 = 0; j_0__idct_col_0 < 8; j_0__idct_col_0++) 
/* Original: #pragma ACCEL pipeline FLATTEN */
{
              
#pragma ACCEL PIPELINE II=1
              tmpInt_0__idct_col_0 = 0;
              for (ii_0__idct_col_0 = 0; ii_0__idct_col_0 < 8; ii_0__idct_col_0++) {
                
#pragma ACCEL PARALLEL COMPLETE
                x_0__idct_col_0 = row2col_data1_0_0[mby_0_0][mbx_0_0][blk_0_0][j_0__idct_col_0][ii_0__idct_col_0];
                tmpInt_0__idct_col_0 += x_0__idct_col_0 * inv_coeff[ii_0__idct_col_0][j_0__idct_col_0];
              }
              tmpInt_0__idct_col_0 = tmpInt_0__idct_col_0 + (1 << cwidth + shift) >> cwidth + shift + 1;
              tmp_0__idct_col_0 = (tmpInt_0__idct_col_0 < ((int )negMaxval_0__idct_col_0)?((int )negMaxval_0__idct_col_0) : ((tmpInt_0__idct_col_0 > 255?255 : tmpInt_0__idct_col_0)));
              text_data_buf_0_0[mbx_0_0][blk_0_0][j_0__idct_col_0][i__idct_col_0] = tmp_0__idct_col_0;
            }
//idx2++;
          }
// BlockIDCT
        }
      }
/*img, mb_width, mb_height, */
/*img, mb_width, mb_height, */
//#pragma ACCEL parallel complete
    }
    for (_memcpy_i1_3 = 0; _memcpy_i1_3 < 22; ++_memcpy_i1_3) {
      for (_memcpy_i1_2 = 0; _memcpy_i1_2 < 6; ++_memcpy_i1_2) {
        for (_memcpy_i1_1 = 0; _memcpy_i1_1 < 8; ++_memcpy_i1_1) {
          for (_memcpy_i1_0 = 0; _memcpy_i1_0 < 8; ++_memcpy_i1_0) {
            text_data[mby_0_0 * 8448 + ((((0 * 22 + _memcpy_i1_3) * 6 + _memcpy_i1_2) * 8 + _memcpy_i1_1) * 8 + _memcpy_i1_0)] = text_data_buf_0_0[_memcpy_i1_3][_memcpy_i1_2][_memcpy_i1_1][_memcpy_i1_0];
          }
        }
      }
    }
  }
}
static void __merlin_dummy_idct_row();
static void __merlin_dummy_idct_col();
