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
#include "AllInOne.h"
#include <math.h>
const int cwidth = 13;
const int shift = 3;
const int inv_coeff[8][8] = {{(2048), (2048), (2048), (2048), (2048), (2048), (2048), (2048)}, {(1841), (2408), (1609), (565), (- 565), (- 1609), (- 2408), (- 1841)}, {(2676), (1108), (- 1108), (- 2676), (- 2676), (- 1108), (1108), (2676)}, {(2408), (- 565), (- 1841), (- 1609), (1609), (1841), (565), (- 2408)}, {(2048), (- 2048), (- 2048), (2048), (2048), (- 2048), (- 2048), (2048)}, {(1609), (- 1841), (565), (2408), (- 2408), (- 565), (1841), (- 1609)}, {(1108), (- 2676), (2676), (- 1108), (- 1108), (2676), (- 2676), (1108)}, {(565), (- 1609), (2408), (- 1841), (1841), (- 2408), (1609), (- 565)}};
void idct_row(int blk,int mbx,int mby,int img,int mb_width,int mb_height,int *dequant_data,int *row2col_data,int *row2col_dc);
void idct_col(int blk,int mbx,int mby,int img,int mb_width,int mb_height,int *row2col_data,int *row2col_dc,int *text_data);
#pragma ACCEL kernel
void idct_kernel(int img,int mb_width,int mb_height,int *dequant_data,int *row2col_data,int *text_data)
{
  int blk;
  int mbx;
  int mby;
  int row2col_dc_0;
  int mb_height__idct_row_0;
  int mb_width__idct_row_0;
  int img__idct_row_0;
  int mby__idct_row_0;
  int mbx__idct_row_0;
  int blk__idct_row_0;
  int i__idct_row_0;
  int j__idct_row_0;
  int mb_idx__idct_row_0;
  int idx_base0_0__idct_row_0;
  int idx_base1_0__idct_row_0;
  int idx_base2_0__idct_row_0;
  int idx_base3_0__idct_row_0;
  int dc_value_0__idct_row_0;
  int blk_idx_0__idct_row_0;
  int tmpval_0__idct_row_0;
  int tmpval2_0__idct_row_0;
  int x_0__idct_row_0;
  int ii_0__idct_row_0;
  int j_0__idct_row_0;
  int idx2_0__idct_row_0;
  int idx_0__idct_row_0;
  int mb_height__idct_col_0;
  int mb_width__idct_col_0;
  int img__idct_col_0;
  int mby__idct_col_0;
  int mbx__idct_col_0;
  int blk__idct_col_0;
  int i__idct_col_0;
  int j__idct_col_0;
  char dc_scaler__idct_col_0;
  int out__idct_col_0[64];
  int tmpval__idct_col_0;
  int k__idct_col_0;
  int idx_base0__idct_col_0;
  int idx_base1_0__idct_col_0;
  int idx_base2_0__idct_col_0;
  int idx_base3_0__idct_col_0;
  int mb_idx_0__idct_col_0;
  int idx_base_0__idct_col_0;
  int idx_0__idct_col_0;
  int idx2_0__idct_col_0;
  int tmpdc_0__idct_col_0;
  int x_0__idct_col_0;
  short negMaxval_0__idct_col_0;
  int tmpInt_0__idct_col_0;
  int ii_0__idct_col_0;
  int j_0__idct_col_0;
  int tmp_0__idct_col_0;
  for (mby = 0; mby < mb_height; mby++) {
    
#pragma HLS loop_tripcount max=18
    for (mbx = 0; mbx < mb_width; mbx++) {
      
#pragma HLS loop_tripcount max=22
//#pragma ACCEL pipeline
      for (blk = 0; blk < 6; blk++) {{
          mb_height__idct_row_0 = mb_height;
          mb_width__idct_row_0 = mb_width;
          img__idct_row_0 = img;
          mby__idct_row_0 = mby;
          mbx__idct_row_0 = mbx;
          blk__idct_row_0 = blk;
          mb_idx__idct_row_0 = mbx + mb_width * mby + mb_width * mb_height * img;
          idx_base0_0__idct_row_0 = img;
          idx_base1_0__idct_row_0 = mby;
          idx_base2_0__idct_row_0 = mbx;
          idx_base3_0__idct_row_0 = blk;
          dc_value_0__idct_row_0 = 0;
          blk_idx_0__idct_row_0 = (mbx + mb_width * mby + mb_width * mb_height * img) * 6 + blk;
          for (i__idct_row_0 = 0; i__idct_row_0 < 8; i__idct_row_0++) {
            idx2_0__idct_row_0 = ((mbx + mb_width * mby + mb_width * mb_height * img) * 6 + blk) * 64 + 8 * i__idct_row_0;
            idx_0__idct_row_0 = ((mbx + mb_width * mby + mb_width * mb_height * img) * 6 + blk) * 64;
            for (j_0__idct_row_0 = 0; j_0__idct_row_0 < 8; j_0__idct_row_0++) 
/* Original: #pragma ACCEL pipeline flatten */
{
              
#pragma ACCEL PIPELINE II=1
              tmpval_0__idct_row_0 = 0;
              for (ii_0__idct_row_0 = 0; ii_0__idct_row_0 < 8; ii_0__idct_row_0++) {
                
#pragma ACCEL PARALLEL COMPLETE
                x_0__idct_row_0 = dequant_data[((mbx + mb_width * mby + mb_width * mb_height * img) * 6 + blk) * 64 + 8 * j_0__idct_row_0 + ii_0__idct_row_0];
                tmpval_0__idct_row_0 += x_0__idct_row_0 * inv_coeff[ii_0__idct_row_0][j_0__idct_row_0];
              }
              tmpval2_0__idct_row_0 = tmpval_0__idct_row_0 + (1 << cwidth - shift - 3) >> cwidth - shift - 2;
              row2col_data[((mbx + mb_width * mby + mb_width * mb_height * img) * 6 + blk) * 64 + 8 * i__idct_row_0 + j_0__idct_row_0] = tmpval2_0__idct_row_0;
              if (i__idct_row_0 == 0 && j_0__idct_row_0 == 0) {
                dc_value_0__idct_row_0 = tmpval2_0__idct_row_0;
              }
            }
          }
/*[blk_idx]*/
           *(&row2col_dc_0) = dc_value_0__idct_row_0;
// BlockIDCT
        }
{
          mb_height__idct_col_0 = mb_height;
          mb_width__idct_col_0 = mb_width;
          img__idct_col_0 = img;
          mby__idct_col_0 = mby;
          mbx__idct_col_0 = mbx;
          blk__idct_col_0 = blk;
          idx_base0__idct_col_0 = img;
          idx_base1_0__idct_col_0 = mby;
          idx_base2_0__idct_col_0 = mbx;
          idx_base3_0__idct_col_0 = blk;
          mb_idx_0__idct_col_0 = mbx + mb_width * mby + mb_width * mb_height * img;
          idx_base_0__idct_col_0 = blk + (mbx + mb_width * mby + mb_width * mb_height * img) * 6;
          idx_0__idct_col_0 = (blk + (mbx + mb_width * mby + mb_width * mb_height * img) * 6) * 64;
          idx2_0__idct_col_0 = (blk + (mbx + mb_width * mby + mb_width * mb_height * img) * 6) * 64;
          tmpdc_0__idct_col_0 =  *(&row2col_dc_0);
          dc_scaler__idct_col_0 = ((char )( *(&row2col_dc_0) & 0xff));
// 1-D IDCT on columns
          for (i__idct_col_0 = 0; i__idct_col_0 < 8; i__idct_col_0++) {
            if ((char )( *(&row2col_dc_0) & 0xff)) {
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
                x_0__idct_col_0 = row2col_data[(blk + (mbx + mb_width * mby + mb_width * mb_height * img) * 6) * 64 + ii_0__idct_col_0 + 8 * j_0__idct_col_0];
                tmpInt_0__idct_col_0 += x_0__idct_col_0 * inv_coeff[ii_0__idct_col_0][j_0__idct_col_0];
              }
              tmpInt_0__idct_col_0 = tmpInt_0__idct_col_0 + (1 << cwidth + shift) >> cwidth + shift + 1;
              tmp_0__idct_col_0 = (tmpInt_0__idct_col_0 < ((int )negMaxval_0__idct_col_0)?((int )negMaxval_0__idct_col_0) : ((tmpInt_0__idct_col_0 > 255?255 : tmpInt_0__idct_col_0)));
              text_data[(blk + (mbx + mb_width * mby + mb_width * mb_height * img) * 6) * 64 + i__idct_col_0 + 8 * j_0__idct_col_0] = tmp_0__idct_col_0;
            }
//idx2++;
          }
// BlockIDCT
        }
      }
//#pragma ACCEL parallel complete
    }
  }
}

void idct_row(int blk,int mbx,int mby,int img,int mb_width,int mb_height,int *dequant_data,int *row2col_data,int *row2col_dc)
{
//unsigned char i;
  int i;
  int j;
  int mb_idx;
//get_global_id(0);
//get_global_id(1);
//get_global_id(2);
//get_global_id(3);
// 1-D IDCT on rows
  int idx_base0_0;
  int idx_base1_0;
  int idx_base2_0;
  int idx_base3_0;
  int dc_value_0;
  int blk_idx_0;
  int tmpval_0;
  int tmpval2_0;
  int x_0;
  int ii_0;
  int j_0;
  int idx2_0;
  int idx_0;
  mb_idx = mbx + mb_width * mby + mb_width * mb_height * img;
  idx_base0_0 = img;
  idx_base1_0 = mby;
  idx_base2_0 = mbx;
  idx_base3_0 = blk;
  dc_value_0 = 0;
  blk_idx_0 = mb_idx * 6 + blk;
  for (i = 0; i < 8; i++) {
    idx2_0 = blk_idx_0 * 64 + 8 * i;
    idx_0 = blk_idx_0 * 64;
    for (j_0 = 0; j_0 < 8; j_0++) 
/* Original: #pragma ACCEL pipeline flatten */
{
      
#pragma ACCEL PIPELINE II=1
      tmpval_0 = 0;
      for (ii_0 = 0; ii_0 < 8; ii_0++) {
        
#pragma ACCEL PARALLEL COMPLETE
        x_0 = dequant_data[idx_0 + 8 * j_0 + ii_0];
        tmpval_0 += x_0 * inv_coeff[ii_0][j_0];
      }
      tmpval2_0 = tmpval_0 + (1 << cwidth - shift - 3) >> cwidth - shift - 2;
      row2col_data[idx2_0 + j_0] = tmpval2_0;
      if (i == 0 && j_0 == 0) {
        dc_value_0 = tmpval2_0;
      }
    }
  }
/*[blk_idx]*/
   *row2col_dc = dc_value_0;
// BlockIDCT
}

void idct_col(int blk,int mbx,int mby,int img,int mb_width,int mb_height,int *row2col_data,int *row2col_dc,int *text_data)
{
//unsigned char i,j;
  int i;
  int j;
  char dc_scaler;
  int out[64];
  int tmpval;
  int k;
  int idx_base0;
//[idx_base];
  int idx_base1_0;
  int idx_base2_0;
  int idx_base3_0;
  int mb_idx_0;
  int idx_base_0;
  int idx_0;
  int idx2_0;
  int tmpdc_0;
  int x_0;
  short negMaxval_0;
  int tmpInt_0;
  int ii_0;
  int j_0;
  int tmp_0;
  idx_base0 = img;
  idx_base1_0 = mby;
  idx_base2_0 = mbx;
  idx_base3_0 = blk;
  mb_idx_0 = mbx + mb_width * mby + mb_width * mb_height * img;
  idx_base_0 = idx_base3_0 + mb_idx_0 * 6;
  idx_0 = idx_base_0 * 64;
  idx2_0 = idx_base_0 * 64;
  tmpdc_0 =  *row2col_dc;
  dc_scaler = (tmpdc_0 & 0xff);
// 1-D IDCT on columns
  for (i = 0; i < 8; i++) {
    if ((char )(tmpdc_0 & 0xff)) {
      negMaxval_0 = 0;
    }
     else {
      negMaxval_0 = (- 255 - 1);
    }
    for (j_0 = 0; j_0 < 8; j_0++) 
/* Original: #pragma ACCEL pipeline FLATTEN */
{
      
#pragma ACCEL PIPELINE II=1
      tmpInt_0 = 0;
      for (ii_0 = 0; ii_0 < 8; ii_0++) {
        
#pragma ACCEL PARALLEL COMPLETE
        x_0 = row2col_data[idx_0 + ii_0 + 8 * j_0];
        tmpInt_0 += x_0 * inv_coeff[ii_0][j_0];
      }
      tmpInt_0 = tmpInt_0 + (1 << cwidth + shift) >> cwidth + shift + 1;
      tmp_0 = (tmpInt_0 < ((int )negMaxval_0)?((int )negMaxval_0) : ((tmpInt_0 > 255?255 : tmpInt_0)));
      text_data[idx2_0 + i + 8 * j_0] = tmp_0;
    }
//idx2++;
  }
// BlockIDCT
}
