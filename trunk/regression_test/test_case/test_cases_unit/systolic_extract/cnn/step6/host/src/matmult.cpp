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

#define CPU_PE_DEBUG PE_DEBUG

#if 0
void matmul_kernel_sw_bak(
       unsigned int num_elem_A,
       unsigned int num_elem_B,
       unsigned int num_elem_C,
//       int BNUM_I, int BNUM_J, int BNUM_K,
       float * a,
       float * b,
       float * c ,
         int * t_idx_idx,
         int * t_idx_i,
         int * t_idx_j
       ) 
{
    assert(num_elem_A == HA * WA);
    assert(num_elem_B == HB * WB);
    assert(num_elem_C == HC * WC);
    float c_tmp[HC * WC];
    memset(c_tmp, 0, sizeof(float)*HC*WC);

//#define PE_ROW 4
//#define PE_COL 4
//#define D_SIZE 8

//#define BSIZE_I (128 / PE_ROW)
//#define BSIZE_J (128 / PE_COL)
//#define BSIZE_K (128 / D_SIZE)

//#define BNUM_I 8 //1024/128)
//#define BNUM_J 1 //1024/128)
//#define BNUM_K 1 //(1024/128)

    int cnt = 0;


    for (int i0 = 0; i0 < BNUM_I; i0++)
    for (int j0 = 0; j0 < BNUM_J; j0++)
    for (int k0 = 0; k0 < BNUM_K; k0++)
    {
        for (int k1 = 0; k1 < BSIZE_K; k1++)
      //  for (int i1 = 0; i1 < BSIZE_I; i1++)
        for (int p = 0; p < K; p++)
        for (int q = 0; q < K; q++)
        for (int j1 = 0; j1 < BSIZE_J; j1++)
        for (int i2 = 0; i2 < PE_ROWS; i2++)
        for (int j2 = 0; j2 < PE_COLS; j2++)
        for (int k2 = 0; k2 < PE_DSIZE; k2++)
        {
    		int i1 = p * K + q;
    
            int block_a = i0*BNUM_K + k0;
            int block_b = j0*BNUM_K + k0;
            int block_c = i0*BNUM_J + j0;
            int ii = i1 * PE_ROWS + i2;
            int jj = j1 * PE_COLS + j2;
            int kk = k1 * PE_DSIZE + k2;
    
            int addr_c = block_c * MAT_C_BLOCK_HEIGHT * MAT_C_BLOCK_WIDTH + 
                ii * MAT_C_BLOCK_WIDTH + jj;
            int addr_b = block_b * MAT_B_BLOCK_HEIGHT * MAT_B_BLOCK_WIDTH + 
                jj * MAT_B_BLOCK_HEIGHT + kk;
            int addr_a = block_a * MAT_A_BLOCK_HEIGHT * MAT_A_BLOCK_WIDTH + 
                ii * MAT_A_BLOCK_WIDTH + kk;
    
            {
                int i = k0 * BSIZE_K*PE_DSIZE + k1 * PE_DSIZE * k2;
                int o = k0 * BSIZE_K*PE_DSIZE + k1 * PE_DSIZE * k2;
                int addr_a1 = get_addr_a(i0, j0, k0, k1, p, q, j1, i2, j2, k2);
                assert(addr_a == addr_a1);
            }

            c_tmp[addr_c] += a[addr_a] * b[addr_b];
    
        }
    }

    //reorder_within_blocks(c_tmp, c, HC, WC, SYS_ARRAY_NUM_COLS, MAT_C_BLOCK_WIDTH);
    
    int mat_height = HC;
    int mat_width = WC;
    
    int num_sys_arr_columns = SYS_ARRAY_NUM_COLS;
    int block_width = MAT_C_BLOCK_WIDTH;
    {
        int num_elems = HC * WC;
        int column_interleaving = COLUMNS_INTERLEAVED; //block_width / SYS_ARRAY_NUM_COLS ;
        int word_id = 0;
        for(int i=0; i < num_elems; i+=block_width) {       // stride = block_width = 128

            // ZP: 20161005: interchange in every row in the block (scope = 128, 4x32 => 32x4)
            for(int j=0; j < COLUMNS_INTERLEAVED; j++) {    // block interleave = 32
                for(int k=0; k < SYS_ARRAY_NUM_COLS; k++) { // PE col = 4
                    c[word_id] = c_tmp[i+j+k*COLUMNS_INTERLEAVED];  // interchaning k and j

                    int old_idx = i+j+k*COLUMNS_INTERLEAVED;
                    int block_C_size = MAT_C_BLOCK_HEIGHT * MAT_C_BLOCK_WIDTH;
                    int pixel_ij = old_idx % block_C_size;
                    int block_ij = old_idx / block_C_size;
                    int _i0 = (block_ij / BNUM_J);
                    int _j0 = (block_ij % BNUM_J);
                    int _i1 = (pixel_ij / MAT_C_BLOCK_WIDTH);
                    int _j1 = (pixel_ij % MAT_C_BLOCK_WIDTH);

                    int idx_i = _i0 *MAT_C_BLOCK_HEIGHT  + _i1; 
                    int idx_j = _j0 *MAT_C_BLOCK_WIDTH  + _j1; 

                    t_idx_idx[word_id] = old_idx;
                    t_idx_i[word_id] = idx_i;
                    t_idx_j[word_id] = idx_j;

                    word_id++;
                    
                }
            }
        }
    }
}


// C-Model
void matmul_kernel_sw_v1(
       unsigned int num_elem_A,
       unsigned int num_elem_B,
       unsigned int num_elem_C,
//       int BNUM_I, int BNUM_J, int BNUM_K,
       float * a,
       float * b,
       float * c ,
         int * t_idx_idx,
         int * t_idx_i,
         int * t_idx_j
       ) 
{
    assert(num_elem_A == HA * WA);
    assert(num_elem_B == HB * WB);
    assert(num_elem_C == HC * WC);
    float c_tmp[HC * WC];
    memset(c_tmp, 0, sizeof(float)*HC*WC);

    int cnt = 0;

    assert(BNUM_I == IMG_OUT_NUM / PE_ROWS);
    assert(BNUM_J == IMG_COL);
    assert(BNUM_K == 1);
    assert(BSIZE_K == IMG_IN_NUM / PE_DSIZE);
    assert(BSIZE_I == K * K);


    for (int o0 = 0; o0 < BNUM_I; o0++)  // i0
    for (int c = 0; c < BNUM_J; c++)  // j0
//    for (int k0 = 0; k0 < BNUM_K; k0++) // NULL
    {
        for (int in0 = 0; in0 < BSIZE_K; in0++) // k1
        for (int p = 0; p < K; p++) // i1
        for (int q = 0; q < K; q++) // i1
        for (int r0 = 0; r0 < BSIZE_J; r0++) // j1

        for (int oo = 0; oo < PE_ROWS; oo++) //i2
        for (int rr = 0; rr < PE_COLS; rr++) //j2
        for (int inn = 0; inn < PE_DSIZE; inn++) //k2
        {
    		int i1 = p * K + q;
            int j0 = c;
            int i0 = o0;
            int k0 = 0;
            int k1 = in0;
            int j1 = r0;
            int i2 = oo;
            int j2 = rr;
            int k2 = inn;
    
            int block_a = o0*BNUM_K + k0;
            int block_b = j0*BNUM_K + k0;
            int block_c = o0*BNUM_J + j0;
            int ii = i1 * PE_ROWS + i2;
            int jj = j1 * PE_COLS + j2;
            int kk = k1 * PE_DSIZE + k2;
    
            int addr_c = block_c * MAT_C_BLOCK_HEIGHT * MAT_C_BLOCK_WIDTH + 
                ii * MAT_C_BLOCK_WIDTH + jj;
            int addr_b = block_b * MAT_B_BLOCK_HEIGHT * MAT_B_BLOCK_WIDTH + 
                jj * MAT_B_BLOCK_HEIGHT + kk;
            int addr_a = block_a * MAT_A_BLOCK_HEIGHT * MAT_A_BLOCK_WIDTH + 
                ii * MAT_A_BLOCK_WIDTH + kk;
    
            //{
            //    int i = k0 * BSIZE_K*PE_DSIZE + k1 * PE_DSIZE * k2;
            //    int o = k0 * BSIZE_K*PE_DSIZE + k1 * PE_DSIZE * k2;
            //    int addr_a1 = get_addr_a(i0, j0, k0, k1, p, q, j1, i2, j2, k2);
            //    assert(addr_a == addr_a1);
            //}

            c_tmp[addr_c] += a[addr_a] * b[addr_b];
    
        }
    }

    //reorder_within_blocks(c_tmp, c, HC, WC, SYS_ARRAY_NUM_COLS, MAT_C_BLOCK_WIDTH);
    
    int mat_height = HC;
    int mat_width = WC;
    
    int num_sys_arr_columns = SYS_ARRAY_NUM_COLS;
    int block_width = MAT_C_BLOCK_WIDTH;
    {
        int num_elems = HC * WC;
        int column_interleaving = COLUMNS_INTERLEAVED; //block_width / SYS_ARRAY_NUM_COLS ;
        int word_id = 0;
        for(int i=0; i < num_elems; i+=block_width) {       // stride = block_width = 128

            // ZP: 20161005: interchange in every row in the block (scope = 128, 4x32 => 32x4)
            for(int j=0; j < COLUMNS_INTERLEAVED; j++) {            // block interleave = 32
                for(int k=0; k < SYS_ARRAY_NUM_COLS; k++) {         // PE col = 4
                    c[word_id] = c_tmp[i+j+k*COLUMNS_INTERLEAVED];  // interchaning k and j

                    int old_idx = i+j+k*COLUMNS_INTERLEAVED;
                    int block_C_size = MAT_C_BLOCK_HEIGHT * MAT_C_BLOCK_WIDTH;
                    int pixel_ij = old_idx % block_C_size;
                    int block_ij = old_idx / block_C_size;
                    int _i0 = (block_ij / BNUM_J);
                    int _j0 = (block_ij % BNUM_J);
                    int _i1 = (pixel_ij / MAT_C_BLOCK_WIDTH);
                    int _j1 = (pixel_ij % MAT_C_BLOCK_WIDTH);

                    int idx_i = _i0 *MAT_C_BLOCK_HEIGHT  + _i1; 
                    int idx_j = _j0 *MAT_C_BLOCK_WIDTH  + _j1; 

                    t_idx_idx[word_id] = old_idx;
                    t_idx_i[word_id] = idx_i;
                    t_idx_j[word_id] = idx_j;

                    word_id++;
                    
                }
            }
        }
    }
}
#endif


void matmul_kernel_sw(
       unsigned int num_elem_A,
       unsigned int num_elem_B,
       unsigned int num_elem_C,
//       int BNUM_I, int BNUM_J, int BNUM_K,
       float * a,
       float * b,
       float * c ,
         int * t_idx_addr,
         int * t_idx_idx,
         int * t_idx_i,
         int * t_idx_j
       ) 
{
    assert(num_elem_A == HA * WA);
    assert(num_elem_B == HB * WB);
    assert(num_elem_C == HC * WC);
    float c_tmp[HC * WC];
    memset(c_tmp, 0, sizeof(float)*HC*WC);

    int cnt = 0;

    assert(BNUM_I == IMG_OUT_NUM / PE_ROWS);
    assert(BNUM_J == IMG_COL);
    assert(BNUM_K == 1);
    assert(BSIZE_K == IMG_IN_NUM / PE_DSIZE);
    assert(BSIZE_I == K * K);

	/*
	printf("%d %d %d\n", IMG_IN_NUM, IMG_IN_ROW, IMG_IN_COL);

	for (int i = 0; i < IMG_IN_NUM; i++)
		for (int r = 0; r < IMG_IN_ROW; r++)
			for (int c = 0; c < IMG_IN_COL; c++)
			{
				int idx = i * IMG_IN_ROW * IMG_IN_COL + r * IMG_IN_COL + c;
				printf("%d: %f\n", idx, b[idx]);
			}
			*/

    for (int o0 = 0; o0 < BNUM_I; o0++)  // i0
    for (int c = 0; c < BNUM_J; c++)  // j0
//    for (int k0 = 0; k0 < BNUM_K; k0++) // NULL
    {
        int k0 = 0;
        int ib = k0;

        float w_buf[IMG_IN_NUM][PE_ROWS][KERNEL][KERNEL]; // i, oo, p, q
        float in_buf[IMG_IN_NUM][IMG_ROW + KERNEL-1][KERNEL]; // i, r+p, c+q
        //float out_buf[PE_ROWS][IMG_ROW]; // oo, r
        float out_buf[IMG_ROW][KERNEL][KERNEL][PE_ROWS]; // r, pq, oo
        for (int in0 = 0; in0 < BSIZE_K; in0++) {
        for (int p = 0; p < KERNEL; p++) {
        for (int q = 0; q < KERNEL; q++) {
        for (int r0 = 0; r0 < IMG_ROW / SYS_ARRAY_NUM_COLS; r0++) {
            for (int oo = 0; oo < SYS_ARRAY_NUM_ROWS; oo++) {
            for (int rr = 0; rr < SYS_ARRAY_NUM_COLS; rr++) {
            for (int inn = 0; inn < DOT_PROD_VECTOR_SIZE; inn++) {
                int addr_a, addr_b, addr_c;
                {
                    int i1 = p * K + q;
                    int j0 = c;
                    int i0 = o0;
                    int k1 = in0;
                    int j1 = r0;
                    int i2 = oo;
                    int j2 = rr;
                    int k2 = inn;

                    int block_a = o0*BNUM_K + k0;
                    int block_b = j0*BNUM_K + k0;
                    int block_c = o0*BNUM_J + j0;
                    int ii = i1 * PE_ROWS + i2;
                    int jj = j1 * PE_COLS + j2;
                    int kk = k1 * PE_DSIZE + k2;

                    //addr_a = block_a * MAT_A_BLOCK_HEIGHT * MAT_A_BLOCK_WIDTH + 
                    //  ii * MAT_A_BLOCK_WIDTH + kk;

                    addr_a = block_a * MAT_A_BLOCK_HEIGHT * MAT_A_BLOCK_WIDTH + 
                        ((oo *K  + p) * K + q)* BSIZE_K + kk;

                    addr_c = block_c * MAT_C_BLOCK_HEIGHT * MAT_C_BLOCK_WIDTH + 
                    ii * MAT_C_BLOCK_WIDTH + jj;
                    addr_b = block_b * MAT_B_BLOCK_HEIGHT * MAT_B_BLOCK_WIDTH + 
                        jj * MAT_B_BLOCK_HEIGHT + kk;

                    int o = o0 * SYS_ARRAY_NUM_ROWS + oo;
                    int r = r0 * SYS_ARRAY_NUM_COLS + rr;
                    //int r = rr*BSIZE_J + r0;   // the actual access to r is [rr,r0]
                    int i = ib * BSIZE_K * PE_DSIZE + in0 * PE_DSIZE + inn;
                    addr_b = i * (IMG_IN_ROW * IMG_IN_COL) + (r + p) * IMG_IN_COL + 
                        (c + q);
                }

                {
                    int o = o0 * SYS_ARRAY_NUM_ROWS + oo;
                    int r = r0 * SYS_ARRAY_NUM_COLS + rr;
                    //int r = rr*BSIZE_J + r0;   // the actual access to r is [rr,r0]
                    int i = ib * BSIZE_K * PE_DSIZE + in0 * PE_DSIZE + inn;
                    w_buf[i][oo][p][q] = a[addr_a];
                        //a[o * (IMG_OUT_NUM * KERNEL * KERNEL) + i * (KERNEL * KERNEL) + p * KERNEL + q] ; 

                    in_buf[i][r+p][q] = b[addr_b];
					if (0 == i && 0 == q) {
					//printf("CPU: in_buf[%d][%d+%d][%d]=%.3f, ref=%d\n", i, r, p, q, in_buf[i][r+p][q], addr_b);
					}

                    //in_buf[i][r][0] = b[addr_b];
                       // b[i * (IMG_IN_ROW * IMG_IN_COL) + (r + p) * IMG_IN_COL + (c + q)];
                       //

                    int i1 = p * K + q;
                    int out_addr = i1 * IMG_OUT_NUM * IMG_ROW * IMG_COL + 
                        o * IMG_ROW * IMG_COL + r * IMG_COL + c;

                    //out_buf[r][p][q][oo]= c_tmp[addr_c];
                    out_buf[r][p][q][oo]= c_tmp[out_addr];
                }

            }}}
        }}}}

        for (int in0 = 0; in0 < BSIZE_K; in0++) // k1
        for (int p = 0; p < K; p++) // i1
        for (int q = 0; q < K; q++) // i1
        for (int r0 = 0; r0 < BSIZE_J; r0++) // j1

        for (int oo = 0; oo < PE_ROWS; oo++) //i2
        for (int rr = 0; rr < PE_COLS; rr++) //j2
        for (int inn = 0; inn < PE_DSIZE; inn++) //k2
        {
    		int i1 = p * K + q;
            int j0 = c;
            int i0 = o0;
            int k1 = in0;
            int j1 = r0;
            int i2 = oo;
            int j2 = rr;
            int k2 = inn;
    
            int block_a = o0*BNUM_K + k0;
            int block_b = j0*BNUM_K + k0;
            int block_c = o0*BNUM_J + j0;
            int ii = i1 * PE_ROWS + i2;
            int jj = j1 * PE_COLS + j2;
            int kk = k1 * PE_DSIZE + k2;

            int addr_c = block_c * MAT_C_BLOCK_HEIGHT * MAT_C_BLOCK_WIDTH + 
                ii * MAT_C_BLOCK_WIDTH + jj;
            int addr_b = block_b * MAT_B_BLOCK_HEIGHT * MAT_B_BLOCK_WIDTH + 
                jj * MAT_B_BLOCK_HEIGHT + kk;
            //int addr_a = block_a * MAT_A_BLOCK_HEIGHT * MAT_A_BLOCK_WIDTH + 
            //    ii * MAT_A_BLOCK_WIDTH + kk;
    
            int r = r0 * SYS_ARRAY_NUM_COLS + rr;
            //int r = rr*BSIZE_J + r0;   // the actual access to r is [rr,r0]
            int i = in0 * PE_DSIZE + inn;

            // c_tmp[addr_c] += a[addr_a] * b[addr_b];
            //out_buf[r][p][q][oo] += w_buf[i][oo][p][q] * in_buf[i][r][0];
            out_buf[r][p][q][oo] += w_buf[i][oo][p][q] * in_buf[i][r+p][q];

#if CPU_PE_DEBUG
                    int rr_fpga = r / BSIZE_J;
                    int r0_fpga = r % BSIZE_J;
                    int pe_row = oo; // 0;
                    int pe_col = rr_fpga; // 1;

                    //int pe_idx = o0 * BNUM_J * BSIZE_I * BSIZE_J + 
                    //    c * BSIZE_I * BSIZE_J + i1 * BSIZE_J + j1;
                    int pe_idx = o0 * BNUM_J * BSIZE_I * BSIZE_J + 
                        c * BSIZE_I * BSIZE_J + i1 * BSIZE_J + r0_fpga;

                    int o = o0 * SYS_ARRAY_NUM_ROWS + oo;
                    int out_addr = i1 * IMG_OUT_NUM * IMG_ROW * IMG_COL + 
                                   o * IMG_ROW * IMG_COL + r * IMG_COL + c;

                    if (pe_row == CPU_PE_DEBUG_I &&
                        pe_col == CPU_PE_DEBUG_J &&
                        pe_idx == CPU_PE_DEBUG_IDX )
                    //if (out_addr == 10273)
                    {
                        printf("CPU PE[%d,%d]@%d addr=%d %.2lf <+ {", 
                                pe_row,pe_col,pe_idx, out_addr , out_buf[r][p][q][oo]);

                        for(int d=0; d < DOT_PROD_VECTOR_SIZE; ++d) {

#if DOT_PROD_VECTOR_SIZE==1
                            printf(" %.1lf", w_buf[i][oo][p][q]);
#else
                            printf(" %.1lf",  w_buf[i][oo][p][q][d]);
#endif
                        }
                        printf("}.*{");
                        for(int d=0; d < DOT_PROD_VECTOR_SIZE; ++d) {
#if DOT_PROD_VECTOR_SIZE==1
                            printf(" %.1lf",  in_buf[i][r+p][q]);
#else
                            printf(" %.1lf", in_buf[i][r+p][q][d]);
#endif
                        }
                        printf("} in=%d r0=%d rr=%d p=%d c=%d q=%d\n", 
                                i, r0_fpga, rr_fpga,p, c,q);
                    }
#endif
        }

        for (int in0 = 0; in0 < BSIZE_K; in0++) {
        for (int p = 0; p < KERNEL; p++) {
        for (int q = 0; q < KERNEL; q++) {
        for (int r0 = 0; r0 < IMG_ROW / SYS_ARRAY_NUM_COLS; r0++) {
            for (int oo = 0; oo < SYS_ARRAY_NUM_ROWS; oo++) {
            for (int rr = 0; rr < SYS_ARRAY_NUM_COLS; rr++) {
            for (int inn = 0; inn < DOT_PROD_VECTOR_SIZE; inn++) {
                int addr_a, addr_b, addr_c;
                int i1 = p * K + q;
                {
                    int j0 = c;
                    int i0 = o0;
                    int k1 = in0;
                    int j1 = r0;
                    int i2 = oo;
                    int j2 = rr;
                    int k2 = inn;

                    int block_a = o0*BNUM_K + k0;
                    int block_b = j0*BNUM_K + k0;
                    int block_c = o0*BNUM_J + j0;
                    int ii = i1 * PE_ROWS + i2;
                    int jj = j1 * PE_COLS + j2;
                    int kk = k1 * PE_DSIZE + k2;

                    addr_a = block_a * MAT_A_BLOCK_HEIGHT * MAT_A_BLOCK_WIDTH + 
                    ii * MAT_A_BLOCK_WIDTH + kk;
                    addr_c = block_c * MAT_C_BLOCK_HEIGHT * MAT_C_BLOCK_WIDTH + 
                    ii * MAT_C_BLOCK_WIDTH + jj;
                    addr_b = block_b * MAT_B_BLOCK_HEIGHT * MAT_B_BLOCK_WIDTH + 
                        jj * MAT_B_BLOCK_HEIGHT + kk;
                }

                {
                    int o = o0 * SYS_ARRAY_NUM_ROWS + oo;
                    int r = r0 * SYS_ARRAY_NUM_COLS + rr;
                   // int r = rr*BSIZE_J + r0;   // the actual access to r is [rr,r0]
        
                    int i = ib * BSIZE_K * PE_DSIZE + in0 * PE_DSIZE + inn;

                    int out_addr = i1 * IMG_OUT_NUM * IMG_ROW * IMG_COL + 
                        o * IMG_ROW * IMG_COL + r * IMG_COL + c;


                    //c_tmp[addr_c] = out_buf[r][p][q][oo]; //out_buf[oo][r];
                    c_tmp[out_addr] = out_buf[r][p][q][oo]; //out_buf[oo][r];
                }

            }}}
        }}}}

    }

    //reorder_within_blocks(c_tmp, c, HC, WC, SYS_ARRAY_NUM_COLS, MAT_C_BLOCK_WIDTH);
    //

#if 1
    // Now the interleave happens in r dimension which is the second from the right
    // c[o][r][c]
    {
        int word_id = 0;
        for (int o0 = 0; o0 < BNUM_I; o0++)
        for (int c_ = 0; c_ < BNUM_J; c_++)
        for (int oo = 0; oo < PE_ROWS; oo++)
        for (int p = 0; p < KERNEL; p++)
        for (int q = 0; q < KERNEL; q++) 
        for (int r0 = 0; r0 < BSIZE_J; r0++)
        for (int rr = 0; rr < PE_COLS; rr++)
            {
                int i1 = p * K + q;
                int o = o0 * PE_ROWS + oo;
                //int r = r0 * BSIZE_J + rr;

                int out_addr = i1 * IMG_OUT_NUM * IMG_ROW * IMG_COL + 
                               o * IMG_ROW * IMG_COL + (rr * BSIZE_J+r0) * IMG_COL + c_;

                c[word_id] = c_tmp[out_addr];  

                int row = ((word_id / PE_COLS) / (BSIZE_I * BSIZE_J) ) % PE_ROWS;
                int col = word_id % PE_COLS;
                int pe_idx = 
                    (word_id / PE_COLS) % (BSIZE_I * BSIZE_J) + 
                    (BSIZE_I * BSIZE_J)  * ((word_id / PE_COLS) / (BSIZE_I * BSIZE_J)  / PE_ROWS );

//                if (word_id == 4417)
//                {
//                    printf("HERE 2: addr=%d, p=%d,q=%d o=%d, rr=%d, r0=%d, c_=%d\n",
//                            out_addr, p,q, o, rr, r0, c_);
//                }
//
                t_idx_addr[word_id] = out_addr;
                t_idx_idx[word_id] = pe_idx;
                t_idx_i[word_id]   = row; //idx_i;
                t_idx_j[word_id]   = col; //idx_j;

                word_id++;
            }
    }

#else
    
    int mat_height = HC;
    int mat_width = WC;
    
    int num_sys_arr_columns = SYS_ARRAY_NUM_COLS;
    int block_width = MAT_C_BLOCK_WIDTH;
    {
        int num_elems = HC * WC;
        int column_interleaving = COLUMNS_INTERLEAVED; 
        int word_id = 0;
        for(int i=0; i < num_elems; i+=block_width) {       // stride = block_width = 128

            // ZP: 20161005: interchange in every row in the block (scope = 128, 4x32 => 32x4)
            for(int j=0; j < COLUMNS_INTERLEAVED; j++) {            // block interleave = 32
                for(int k=0; k < SYS_ARRAY_NUM_COLS; k++) {         // PE col = 4
                    //int org_idx = i+j+k*COLUMNS_INTERLEAVED;

                    int org_idx = i+j+k*COLUMNS_INTERLEAVED;

                    c[word_id] = c_tmp[org_idx];  // interchaning k and j

                    int block_C_size = MAT_C_BLOCK_HEIGHT * MAT_C_BLOCK_WIDTH;
                    int pixel_ij = org_idx % block_C_size;
                    int block_ij = org_idx / block_C_size;
                    int _i0 = (block_ij / BNUM_J);
                    int _j0 = (block_ij % BNUM_J);
                    int _i1 = (pixel_ij / MAT_C_BLOCK_WIDTH);
                    int _j1 = (pixel_ij % MAT_C_BLOCK_WIDTH);

                    int idx_i = _i0 *MAT_C_BLOCK_HEIGHT  + _i1; 
                    int idx_j = _j0 *MAT_C_BLOCK_WIDTH  + _j1; 

                    int row = ((word_id / PE_COLS) / (BSIZE_I * BSIZE_J) ) % PE_ROWS;
                    int col = word_id % PE_COLS;
                    int pe_idx = 
                        (word_id / PE_COLS) % (BSIZE_I * BSIZE_J) + 
                        (BSIZE_I * BSIZE_J)  * ((word_id / PE_COLS) / (BSIZE_I * BSIZE_J)  / PE_ROWS );

                    t_idx_idx[word_id] = pe_idx;
                    t_idx_i[word_id]   = row; //idx_i;
                    t_idx_j[word_id]   = col; //idx_j;

                    word_id++;
                    
                }
            }
        }
    }
#endif
}
