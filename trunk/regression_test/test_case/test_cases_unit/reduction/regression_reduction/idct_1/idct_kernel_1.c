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

const int cwidth = BITS_COEF;
const int shift = BITS_SHIFT;

int idx2;
int idx;
extern int kid;

const int inv_coeff[8][8]={{2048,2048,2048,2048,2048,2048,2048,2048},
    {1841,2408,1609,565,-565,-1609,-2408,-1841},
    {2676,1108,-1108,-2676,-2676,-1108,1108,2676},
    {2408,-565,-1841,-1609,1609,1841,565,-2408},
    {2048,-2048,-2048,2048,2048,-2048,-2048,2048},
    {1609,-1841,565,2408,-2408,-565,1841,-1609},
    {1108,-2676,2676,-1108,-1108,2676,-2676,1108},
    {565,-1609,2408,-1841,1841,-2408,1609,-565}
};

int idx_base0;
int idx_base1;
int idx_base2;
int idx_base3;

void idct_row(
        int blk,
        int mbx,
        int mby, 
        int img,
        int mb_width,
        int mb_height,
        int * dequant_data, int * row2col_data, int * row2col_dc);

void idct_col(
        int blk,
        int mbx,
        int mby, 
        int img,
        int mb_width,
        int mb_height,
        int * row2col_data, int * row2col_dc, int * text_data);


#pragma ACCEL kernel
void idct_kernel(
        int img,
        int mb_width,
        int mb_height,
        int * dequant_data, int * row2col_data, int * text_data)
{
    int blk;
    int mbx;
    int mby; 
    for (mby = 0; mby < mb_height; mby++)
#pragma HLS loop_tripcount max=18
        for (mbx = 0; mbx < mb_width; mbx++)
        {
#pragma HLS loop_tripcount max=22
//#pragma ACCEL pipeline
            for (blk = 0; blk < 6; blk++)
            {
//#pragma ACCEL parallel complete
                int row2col_dc;
                idct_row(blk, mbx, mby, img, mb_width, mb_height, dequant_data, row2col_data, &row2col_dc);

                idct_col(blk, mbx, mby, img, mb_width, mb_height, row2col_data, &row2col_dc, text_data);
            }
        }
}


void idct_row(
        int blk,
        int mbx,
        int mby, 
        int img,
        int mb_width,
        int mb_height,
        int * dequant_data, int * row2col_data, int * row2col_dc)
{
    //unsigned char i;
    int i;
    int j;

    int mb_idx = mbx + mb_width*mby + mb_width*mb_height*img;

    idx_base0= img; //get_global_id(0);
    idx_base1= mby; //get_global_id(1);
    idx_base2= mbx; //get_global_id(2);
    idx_base3= blk; //get_global_id(3);

    // 1-D IDCT on rows
    int dc_value = 0;
    int blk_idx = mb_idx * 6 + blk;
    for (i=0; i<8; i++)
    {
        int tmpval;
        int tmpval2;
        int x;
        int ii, j;

        int idx2 = blk_idx*64 + 8*i;
        idx = blk_idx*64;

        for (j = 0; j < B_WIDTH; j++)
        {
//#pragma ACCEL pipeline
            tmpval = 0;
            for (ii = 0; ii < B_WIDTH; ii++) {
//#pragma ACCEL parallel complete
#pragma ACCEL pipeline
                x = dequant_data[idx+8*j+ii];
                tmpval += x*inv_coeff[ii][j];
            }
            tmpval2 = (tmpval + (1 << (cwidth-shift-3))) >> (cwidth-shift-2);
            row2col_data[idx2+j] = tmpval2;

            if (i ==0 && j == 0) dc_value = tmpval2;
        }
    }
    *row2col_dc /*[blk_idx]*/ = dc_value;
} // BlockIDCT


void idct_col(
        int blk,
        int mbx,
        int mby, 
        int img,
        int mb_width,
        int mb_height,
        int * row2col_data, int* row2col_dc, int * text_data)
{
    //unsigned char i,j;
    int i , j;
    char dc_scaler;
    int out[64];
    int tmpval;
    int k;
    int idx_base0= img;
    int idx_base1= mby;
    int idx_base2= mbx;
    int idx_base3= blk;

    int mb_idx = mbx + mb_width*mby + mb_width*mb_height*img;
    int idx_base = idx_base3 + mb_idx*6;

    int idx = idx_base*64;
    int idx2 = idx_base*64;
    int tmpdc = *row2col_dc; //[idx_base];
    dc_scaler = tmpdc&0xff;

    // 1-D IDCT on columns
    for (i=0; i<8; i++)
    {
        int x;
        short negMaxval;
        int tmpInt, ii, j;

        if (dc_scaler)
            negMaxval = 0;
        else
            negMaxval = -MAXVAL_PIXEL - 1;

        for (j = 0; j < B_WIDTH; j++)
        {
//#pragma ACCEL pipeline
            tmpInt = 0;
            for (ii = 0; ii < B_WIDTH; ii++) {
//#pragma ACCEL parallel complete
#pragma ACCEL pipeline
                x = row2col_data[idx+ii+8*j];
                tmpInt += x*inv_coeff[ii][j];
            }

            tmpInt = (tmpInt + (1 << (cwidth+shift))) >> (cwidth+shift+1);
            int tmp = (tmpInt < negMaxval) ? negMaxval : (tmpInt > MAXVAL_PIXEL) ? MAXVAL_PIXEL : tmpInt;
            text_data[idx2+i+8*j] = tmp;
        }
        //idx2++;
    }
} // BlockIDCT

