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

#include "systolic_params.h"

#include "cnn.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void cnn_gold(float* out, const float* in, const float* weight) {
    
//    printf("CNN golden ... \n");
//    fflush(stdout);
    // Initialize output
    // TODO: Initialize with bias
    for (int i = 0; i < IMG_IN_NUM; i++) {
        for (int j = 0; j < IMG_ROW; j++) {
            for (int k = 0; k < IMG_COL; k++) {
                out[i * (IMG_ROW * IMG_COL) + j * IMG_COL + k] = 0;
            }
        }
    }
    
    for (int o = 0; o < IMG_OUT_NUM; o++)
    for (int i = 0; i < IMG_IN_NUM; i++)
    for (int r = 0; r < IMG_ROW; r++)
    for (int c = 0; c < IMG_COL; c++)
    for (int p = 0; p < KERNEL; p++)
    for (int q = 0; q < KERNEL; q++) {
        out[o * (IMG_ROW * IMG_COL) + r * IMG_COL + c] += 
            weight[o * (IMG_OUT_NUM * KERNEL * KERNEL) + i * (KERNEL * KERNEL) + p * KERNEL + q] * 
            in[i * (IMG_IN_ROW * IMG_IN_COL) + (r + p) * IMG_IN_COL + (c + q)];
    }
//    printf("CNN golden finished \n");
    return ;
}


void cnn_kernel_sw_v1(
    unsigned int num_elem_input,
    unsigned int num_elem_weight,
    unsigned int num_elem_output,
    float *in,
    float *weight,
    float *out
    ) {
 
    assert(num_elem_input == IMG_IN_NUM * IMG_IN_ROW * IMG_IN_COL);
    assert(num_elem_weight == IMG_IN_NUM * IMG_OUT_NUM * KERNEL * KERNEL);
    assert(num_elem_output == IMG_OUT_NUM * IMG_ROW * IMG_COL);
    
    memset(out, 0, sizeof(float) * num_elem_output);

    //int TILE_IN = ;

    assert(0 == IMG_OUT_NUM % SYS_ARRAY_NUM_ROWS);
    assert(0 == IMG_IN_NUM % (BSIZE_K * PE_DSIZE));
    //assert(0 == TILE_IN % DOT_PROD_VECTOR_SIZE);
    assert(0 == IMG_ROW % SYS_ARRAY_NUM_COLS);
       
    for (int o0 = 0; o0 < IMG_OUT_NUM / SYS_ARRAY_NUM_ROWS; o0++) {
    for (int c = 0; c < IMG_COL; c++) {
//    for (int ib = 0; ib < IMG_IN_NUM / BSIZE_K / PE_DSIZE ; ib++) 
    {
        for (int i0 = 0; i0 < BSIZE_K; i0++) {
        for (int p = 0; p < KERNEL; p++) {
        for (int q = 0; q < KERNEL; q++) {
        for (int r0 = 0; r0 < IMG_ROW / SYS_ARRAY_NUM_COLS; r0++) {
            for (int oo = 0; oo < SYS_ARRAY_NUM_ROWS; oo++) {
            for (int rr = 0; rr < SYS_ARRAY_NUM_COLS; rr++) {
            for (int ii = 0; ii < DOT_PROD_VECTOR_SIZE; ii++) {
                
                int ib = 0;

                int o = o0 * SYS_ARRAY_NUM_ROWS + oo;
                int r = r0 * SYS_ARRAY_NUM_COLS + rr;
                int i = ib * BSIZE_K * PE_DSIZE + i0 * PE_DSIZE + ii;
                
                out[o * (IMG_ROW * IMG_COL) + r * IMG_COL + c] += 
                weight[o * (IMG_OUT_NUM * KERNEL * KERNEL) + i * (KERNEL * KERNEL) + p * KERNEL + q] * 
                in[i * (IMG_IN_ROW * IMG_IN_COL) + (r + p) * IMG_IN_COL + (c + q)];

#if 0
                int weight_data = weight[o * (IMG_OUT_NUM * KERNEL * KERNEL) + i * (KERNEL * KERNEL) + p * KERNEL + q] ; 
                int in_data = in[i * (IMG_IN_ROW * IMG_IN_COL) + (r + p) * IMG_IN_COL + (c + q)];
                int out_data = out[o * (IMG_ROW * IMG_COL) + r * IMG_COL + c] ;

                printf("[%d,%d,%d] in=%d, weight=%d -> out=%d \n",
                        o, r, c, in_data, weight_data, out_data);
#endif
            }}}
        }}}}
    }}}
    
    return ;
}

void cnn_kernel_sw(
    unsigned int num_elem_input,
    unsigned int num_elem_weight,
    unsigned int num_elem_output,
    float *in,
    float *weight,
    float *out
    ) {
 
    assert(num_elem_input == IMG_IN_NUM * IMG_IN_ROW * IMG_IN_COL);
    assert(num_elem_weight == IMG_IN_NUM * IMG_OUT_NUM * KERNEL * KERNEL);
    assert(num_elem_output == IMG_OUT_NUM * IMG_ROW * IMG_COL);
    
    memset(out, 0, sizeof(float) * num_elem_output);

    //int TILE_IN = ;

    assert(0 == IMG_OUT_NUM % SYS_ARRAY_NUM_ROWS);
    assert(0 == IMG_IN_NUM % (BSIZE_K * PE_DSIZE));
    //assert(0 == TILE_IN % DOT_PROD_VECTOR_SIZE);
    assert(0 == IMG_ROW % SYS_ARRAY_NUM_COLS);


    assert(IMG_IN_NUM == BSIZE_K * PE_DSIZE);
       
    for (int o0 = 0; o0 < IMG_OUT_NUM / SYS_ARRAY_NUM_ROWS; o0++) {
    for (int c = 0; c < IMG_COL; c++) {
//    for (int ib = 0; ib < IMG_IN_NUM / BSIZE_K / PE_DSIZE ; ib++) 
    {
        int ib = 0;
        float w_buf[IMG_IN_NUM][PE_ROWS][KERNEL][KERNEL]; // i, oo, p, q
        float in_buf[IMG_IN_NUM][IMG_ROW + KERNEL-1][KERNEL]; // i, r+p, c+q
        float out_buf[PE_ROWS][IMG_ROW]; // oo, r
        for (int i0 = 0; i0 < BSIZE_K; i0++) {
        for (int p = 0; p < KERNEL; p++) {
        for (int q = 0; q < KERNEL; q++) {
        for (int r0 = 0; r0 < IMG_ROW / SYS_ARRAY_NUM_COLS; r0++) {
            for (int oo = 0; oo < SYS_ARRAY_NUM_ROWS; oo++) {
            for (int rr = 0; rr < SYS_ARRAY_NUM_COLS; rr++) {
            for (int ii = 0; ii < DOT_PROD_VECTOR_SIZE; ii++) {
                int o = o0 * SYS_ARRAY_NUM_ROWS + oo;
                int r = r0 * SYS_ARRAY_NUM_COLS + rr;
                int i = ib * BSIZE_K * PE_DSIZE + i0 * PE_DSIZE + ii;
                w_buf[i][oo][p][q] = 
                    weight[o * (IMG_OUT_NUM * KERNEL * KERNEL) + i * (KERNEL * KERNEL) + p * KERNEL + q] ; 

                in_buf[i][r+p][q] = 
                    in[i * (IMG_IN_ROW * IMG_IN_COL) + (r + p) * IMG_IN_COL + (c + q)];

                out_buf[oo][r] = out[o * (IMG_ROW * IMG_COL) + r * IMG_COL + c] ;

            }}}
        }}}}

        
        for (int i0 = 0; i0 < BSIZE_K; i0++) {
        for (int p = 0; p < KERNEL; p++) {
        for (int q = 0; q < KERNEL; q++) {
        for (int r0 = 0; r0 < IMG_ROW / SYS_ARRAY_NUM_COLS; r0++) {
            for (int oo = 0; oo < SYS_ARRAY_NUM_ROWS; oo++) {
            for (int rr = 0; rr < SYS_ARRAY_NUM_COLS; rr++) {
            for (int ii = 0; ii < DOT_PROD_VECTOR_SIZE; ii++) {

                int o = o0 * SYS_ARRAY_NUM_ROWS + oo;
                int r = r0 * SYS_ARRAY_NUM_COLS + rr;
                int i = ib * BSIZE_K * PE_DSIZE + i0 * PE_DSIZE + ii;
                
                out_buf[oo][r] += //out[o * (IMG_ROW * IMG_COL) + r * IMG_COL + c] += 
                w_buf[i][oo][p][q] * //weight[o * (IMG_OUT_NUM * KERNEL * KERNEL) + i * (KERNEL * KERNEL) + p * KERNEL + q] * 
                in_buf[i][r+p][q]; //in[i * (IMG_IN_ROW * IMG_IN_COL) + (r + p) * IMG_IN_COL + (c + q)];

#if 0
                int weight_data = weight[o * (IMG_OUT_NUM * KERNEL * KERNEL) + i * (KERNEL * KERNEL) + p * KERNEL + q] ; 
                int in_data = in[i * (IMG_IN_ROW * IMG_IN_COL) + (r + p) * IMG_IN_COL + (c + q)];
                int out_data = out[o * (IMG_ROW * IMG_COL) + r * IMG_COL + c] ;

                printf("[%d,%d,%d] in=%d, weight=%d -> out=%d \n",
                        o, r, c, in_data, weight_data, out_data);
#endif
            }}}
        }}}}

        for (int i0 = 0; i0 < BSIZE_K; i0++) {
        for (int p = 0; p < KERNEL; p++) {
        for (int q = 0; q < KERNEL; q++) {
        for (int r0 = 0; r0 < IMG_ROW / SYS_ARRAY_NUM_COLS; r0++) {
            for (int oo = 0; oo < SYS_ARRAY_NUM_ROWS; oo++) {
            for (int rr = 0; rr < SYS_ARRAY_NUM_COLS; rr++) {
            for (int ii = 0; ii < DOT_PROD_VECTOR_SIZE; ii++) {
                int o = o0 * SYS_ARRAY_NUM_ROWS + oo;
                int r = r0 * SYS_ARRAY_NUM_COLS + rr;
                int i = ib * BSIZE_K * PE_DSIZE + i0 * PE_DSIZE + ii;
                w_buf[i][oo][p][q] = 
                    weight[o * (IMG_OUT_NUM * KERNEL * KERNEL) + i * (KERNEL * KERNEL) + p * KERNEL + q] ; 

                in_buf[i][r+p][q] = 
                    in[i * (IMG_IN_ROW * IMG_IN_COL) + (r + p) * IMG_IN_COL + (c + q)];

                out[o * (IMG_ROW * IMG_COL) + r * IMG_COL + c] = 
                    out_buf[oo][r];
            }}}
        }}}}


    }}}
    
    return ;
}

