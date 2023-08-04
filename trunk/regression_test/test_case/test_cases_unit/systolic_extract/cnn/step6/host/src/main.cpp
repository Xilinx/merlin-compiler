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
// System includes 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include <sys/time.h>

#include <unistd.h>

#pragma message ("* Compiling for ALTERA CL")
#include "CL/opencl.h"

#include "cnn.h"

void cnn_gold(float* out, const float* in, const float* weight) ;
void cnn_kernel_sw(
    unsigned int num_elem_input,
    unsigned int num_elem_weight,
    unsigned int num_elem_output,
    float *in,
    float *weight,
    float *out);
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
       ) ;

void dump_float_into_file(float * data, size_t size, const char *sFilename)
{
	FILE * fp;
	if (!(fp=fopen(sFilename, "wb")))
	{
		printf("[ERROR] file %s failed to open for write.\n", sFilename);
		exit(0);
	}

    for (int i = 0; i < size/4; i++)
    {
        //fprintf(fp, "%d %.1lf\n", i, data[i]);
        fprintf(fp, "%d %lf\n", i, data[i]);
    }

	fclose(fp);
}


#define ACL_ALIGNMENT 64

#include <stdlib.h>
void* acl_aligned_malloc (size_t size) {
	void *result = NULL;
	posix_memalign (&result, ACL_ALIGNMENT, size);
	return result;
}
void acl_aligned_free (void *ptr) {
	free (ptr);
  }


//#define EMULATOR
//#define COMPUTE_GOLDEN_BLOCKED
#define COMPUTE_GOLDEN



  #include "systolic_params.h"

#ifndef EMULATOR
  #define AOCX_FILE "systolic_array_4x4_dot8.aocx"
#else
  #define AOCX_FILE "kernel_top.sim.aocx"
#endif

#include "opencl_if.h"


void randomize_array(float* array, const int size)
{
    for (int i = 0; i < size; ++i) 
    {
        array[i] = (float)rand() / (float)RAND_MAX;
    }
}

void one_array(float* array, const int size)
{
    for (int i = 0; i < size; ++i) 
    {
        array[i] = (float)rand() / (float)RAND_MAX;
    //    array[i] = 1.0;
    }
}

void idx_array(float* array, const int size)
{
//	printf("%d %s\n", __LINE__, __func__);
    for (int i = 0; i < size; ++i) 
    {
  //      array[i] = 0;
        array[i] = 1.0 * i;
    }
}

void pad_B(float *input, float *input_pad)
{
//	for (int i = 0; i < IMG_IN_NUM; i++) {
//		for (int r = 0; r < IMG_IN_ROW; r++) {
//			for (int c = 0; c < IMG_COL; c++) {
//				for (int q = 0; q < K; q++) {
//					input_pad[i * IMG_IN_ROW * IMG_IN_COL + r * IMG_IN_COL + c * K + q] = 
//					input[i * IMG_IN_ROW * IMG_IN_COL + r * IMG_IN_COL + c + q];
//				}
//			}
//		}
//	}

#if 1
    for (int o0 = 0; o0 < IMG_OUT_NUM / SYS_ARRAY_NUM_ROWS; o0++) {
    for (int c = 0; c < IMG_COL; c++) {
//    for (int ib = 0; ib < IMG_IN_NUM / BSIZE_K / PE_DSIZE ; ib++) 
    {
        int ib = 0;
        float w_buf[IMG_IN_NUM][PE_ROWS][KERNEL][KERNEL]; // i, oo, p, q
        float in_buf[IMG_IN_NUM][IMG_ROW + KERNEL-1][KERNEL]; // i, r+p, c+q
        float out_buf[PE_ROWS][IMG_ROW]; // oo, r

        //int block_idx = o0*IMG_COL + c; 
        int block_idx = c; 
        int BLOCK_SIZE_WITH_PADDING = (BSIZE_J*PE_COLS+K-1) * K * BSIZE_K * PE_DSIZE;
        
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

                // r+p  : BSIZE_J + K-1
                // q    : K
                // in   : BSIZE_K
                int inner_address = (r+p)*K * BSIZE_K + 
                    q * BSIZE_K + i0;

                input_pad[block_idx * BLOCK_SIZE_WITH_PADDING + inner_address] = 
                    input[i * (IMG_IN_ROW * IMG_IN_COL) + (r + p) * IMG_IN_COL + (c + q)];

            }}}
        }}}}
    }}}
#endif

//    int cnt = 8575;
//    printf("HELLLO [%d] = %lf\n", cnt, input_pad[cnt]);


}

void one_array_w_panding(float *array, const int size, const int row_size) {
    // Add 0s at the first and last column
    
    assert (size % row_size == 0);
    for (int i = 0; i < size; i += row_size) {
        array[i] = 0;
        array[i + row_size - 1] = 0;
        for (int j = 1; j < row_size - 1; j++) {
            array[i + j] = (float) rand() / (float) RAND_MAX;
        }
    }
    return ;
}

bool compare_L2_norm(const float* ref_array, const float* output_array, const unsigned int size, const float epsilon) 
{
  // compute the L^2-Norm of the difference between the output array and reference array 
  // and compare it against the L^2-Norm of the reference.
  float diff = 0.0f;
  float ref = 0.0f;
  for (int i = 0; i < size; ++i) {

    const float o = output_array[i];
    const float r = ref_array[i];
    const float d = o - r;
    diff += d * d;
    ref += r * r;
  }

  const float diff_l2norm = sqrtf(diff);
  const float ref_l2norm = sqrtf(ref);
  const float error = diff_l2norm / ref_l2norm;
  const bool pass = error < epsilon;

  return pass;
}


// using the original B here, not the transposed version
void compute_gold(float* C, const float* A, const float* B, unsigned int hA, unsigned int wA, unsigned int wB)
{
    for (unsigned int i = 0; i < hA; ++i)
        for (unsigned int j = 0; j < wB; ++j) {
            double sum = 0;
            for (unsigned int k = 0; k < wA; ++k) {
                double a = A[i * wA + k];
                double b = B[k * wB + j];
                sum += a * b;
            }
            C[i * wB + j] = (float)sum;
        }
}

/*
#define min(a,b) ((a<b) ? (a):(b))

#define COMPUTE_GOLD_BLOCK_SIZE 64


// takes transposed version of B
void compute_gold_blocked(float* C, const float* A, const float* B, unsigned int hA, unsigned int wA, unsigned int wB, unsigned int hB) 
{
   const int block_size = COMPUTE_GOLD_BLOCK_SIZE;

    for(int i0 = 0; i0 < hA ; i0 += block_size) {
        for(int j0 = 0; j0 < wB; j0 += block_size) {
            
            for(int k0=0; k0 < wA ; k0 += block_size ) {

              for(int i = i0; i < min(hA, i0+block_size); i++) {
                  for(int j = j0; j < min(wB, j0+block_size); j++) {
                      double sum = 0;
                      for(int k = k0; k < min(wA, k0+block_size); k++) {
                        double a = A[i * wA + k];
                        //double b = B[k * wB + j];
                        double b = B[j * hB + k]; // B is transposed
                        sum += a * b;
                      }
                      C[i * wB + j] += (float)sum;
                  }
              }
            }
        }
    }
}
*/

void transpose_matrix( float * B_orig, float * B_transposed, int hB, int wB) 
{
  for(int i=0; i < wB; ++i) {
    for(int j=0; j < hB; ++j) {
      B_transposed[i*hB + j] = B_orig[j*wB + i];
    }
  }
}

void block_wise_reformat_matrix( float * A_orig, float * A_block_wise, int mat_height, int mat_width, int block_height, int block_width) 
{
  int word_id = 0;
  for(int i=0; i < mat_height; i+=block_height) {
    for(int j=0; j < mat_width; j+=block_width) {
      for(int k=0; k < block_height; k++) {
        for(int l=0; l < block_width; l++) {
          A_block_wise[word_id] = A_orig[(i+k)*mat_width + (j+l)];
          word_id++;
        }
      }
    }
  }
}

void reorder_within_blocks( float * C_block_wise, float * C_reordered_within_blocks, int mat_height, int mat_width, int num_sys_arr_columns, int block_width) 
{
  int num_elems = mat_height*mat_width;
  int column_interleaving = block_width / num_sys_arr_columns;
  int word_id = 0;
  for(int i=0; i < num_elems; i+=block_width) {
    for(int j=0; j < column_interleaving; j++) {
      for(int k=0; k < num_sys_arr_columns ; k++) {
        C_reordered_within_blocks[word_id] = C_block_wise[i+j+k*column_interleaving];
        word_id++;
      }
    }
  }
}

void print_matrix(float * A, int hA, int wA) 
{
  for(int i=0; i < hA; ++i) {
    for(int j=0; j < wA; ++j) {
      printf("%.5f\t", A[i*wA + j]);
    }
    printf("\n");
  }
}


void printDiff(float *data1, float *data2, long int size, float fListTol)
{
    printf("Listing Differences > %.6f...\n", fListTol);
    int i,j,k;
    int error_count=0;
    for (i = 0; i < size; i++) 
    {
      float fDiff = fabs(data1[i] - data2[i]);
      if (fDiff > fListTol) 
        {                
            if (error_count < 300) {  // print only first 300 errors
              printf("Host[%d] = %.6f\tKernel[%d]=%.6f\tDiff=%.6f\n", i, data1[i], i, data2[i], fDiff);
            }

            error_count++;
        } else {
          if (error_count < 300) {  // print correct only within first 300 errors
            printf("Correct or nan? --> Host[%d] = %.6f\tKernel[%d]=%.6f\tDiff=%.6f\n", i, data1[i], i, data2[i], fDiff);
          }
        }
    }
    printf("\nTotal Errors = %d\n\n", error_count);
}



int get_addr_a(int i0, int j0, int k0, int k1, int p, int q, int j1, int i2, int j2, int k2)
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
    return addr_a;
}


void matmul_kernel_gold(
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


    for (int i0 = 0; i0 < BNUM_I; i0++)
    for (int j0 = 0; j0 < BNUM_J; j0++)
    for (int k0 = 0; k0 < BNUM_K; k0++)
    {
        for (int k1 = 0; k1 < BSIZE_K; k1++)
        for (int i1 = 0; i1 < BSIZE_I; i1++)
        for (int j1 = 0; j1 < BSIZE_J; j1++)
        for (int i2 = 0; i2 < PE_ROWS; i2++)
        for (int j2 = 0; j2 < PE_COLS; j2++)
        for (int k2 = 0; k2 < PE_DSIZE; k2++)
        {

        }
    }

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


void matmul_kernel_top(cl_int & status,
       cl_command_queue cmdQueue[NUM_QUEUES_TO_CREATE+1],
   	   cl_program &program,
       cl_context &context,
       unsigned int num_elem_A,
       unsigned int num_elem_B,
       unsigned int num_elem_C,
//       int BNUM_I, int BNUM_J, int BNUM_K,
       float * matrix_mul_inputA_block_wise,
       float * matrix_mul_inputB_block_wise,
       float * matrix_mul_outputC,
	double k_start_time[NUM_QUEUES_TO_FINISH],
	double k_end_time[NUM_QUEUES_TO_FINISH],
	double k_exec_time[NUM_QUEUES_TO_FINISH]
       ) ;



int main(int argc, const char** argv) {

        printf("%s Starting...\n\n", argv[0]); 

	unsigned int elements;
	FILE * file;
	long int fstart, fend;
	unsigned int i;

	std::streampos filesize;
	FILE *f_out = stdout;

	////////////////////////////////////////
	// Check and print out the parameters //
	////////////////////////////////////////
        printf("\n===== Host-CPU checking the systolic array matrix multiplication parameters ======\n\n");
	
	printf("HA: \t\t%d\n", HA);
	printf("WA: \t\t%d\n\n", WA);

	printf("HB: \t\t%d\n", HB);
	printf("WB: \t\t%d\n\n", WB);

	printf("HC: \t\t%d\n", HC);				
	printf("WC: \t\t%d\n\n", WC);

	printf("SYS_ARRAY_NUM_ROWS: \t\t%d\n", SYS_ARRAY_NUM_ROWS);
	if (SYS_ARRAY_NUM_ROWS<1) {
		printf("--->ERROR, SYS_ARRAY_NUM_ROWS must be larger than 0\n");
	}
	printf("SYS_ARRAY_NUM_COLS: \t\t%d\n", SYS_ARRAY_NUM_COLS);
	if (SYS_ARRAY_NUM_COLS<1l) {
		printf("--->ERROR, SYS_ARRAY_NUM_COLS must be larger than 0\n");
		exit(1);
	}
	printf("DOT_PROD_VECTOR_SIZE: \t\t%d\n", DOT_PROD_VECTOR_SIZE);
	if (	DOT_PROD_VECTOR_SIZE!=1 && 
			DOT_PROD_VECTOR_SIZE!=2 && 
			DOT_PROD_VECTOR_SIZE!=4 && 
		DOT_PROD_VECTOR_SIZE!=8 && 
		DOT_PROD_VECTOR_SIZE!=16) {
		printf("Illegal DOT_PROD_VECTOR_SIZE, supported: 4,8,16\n");
		exit(1);
	}
	printf("\n");

	printf("ACCUM_SHIFT_REG_SIZE: \t\t%d\n", ACCUM_SHIFT_REG_SIZE);

	printf("\n");

	printf("ROWS_INTERLEAVED: \t\t%d\n",    ROWS_INTERLEAVED);
	printf("COLUMNS_INTERLEAVED: \t\t%d\n", COLUMNS_INTERLEAVED);
	printf("\n");

	printf("MAT_A_BLOCK_HEIGHT: \t\t%d\n",  MAT_A_BLOCK_HEIGHT);
	printf("MAT_A_BLOCK_WIDTH: \t\t%d\n",   MAT_A_BLOCK_WIDTH);
	printf("MAT_A_BLOCK_SIZE: \t\t%d\n",    MAT_A_BLOCK_SIZE);

        //printf("MAT_A_BLOCK_NUM_VECTORS: \t\t%d\n",   MAT_A_BLOCK_NUM_VECTORS);
        //if (MAT_A_BLOCK_SIZE % DOT_PROD_VECTOR_SIZE) {
        //  printf("MAT_A_BLOCK_SIZE must be a multiple of DOT_PROD_VECTOR_SIZE\b");
        //}
        //printf("MAT_A_NUM_BLOCKS_IN_ROW: \t\t%d\n",   MAT_A_NUM_BLOCKS_IN_ROW);
        //if (WA % MAT_A_BLOCK_WIDTH) {
        //  printf("WA must be a multiple of MAT_A_BLOCK_WIDTH\n");
        //}
        //printf("MAT_A_NUM_BLOCKS_IN_COL: \t\t%d\n",   MAT_A_NUM_BLOCKS_IN_COL);
        //if (HA % MAT_A_BLOCK_HEIGHT) {
        //  printf("HA must be a multiple of MAT_A_BLOCK_HEIGHT\n");
        //}
        //printf("MAT_A_NUM_VECTORS_IN_ROW_OF_BLOCKS: \t\t%d\n",   MAT_A_NUM_VECTORS_IN_ROW_OF_BLOCKS);
	printf("\n");
        
	printf("MAT_B_BLOCK_HEIGHT: \t\t%d\n",  MAT_B_BLOCK_HEIGHT);
	printf("MAT_B_BLOCK_WIDTH: \t\t%d\n",   MAT_B_BLOCK_WIDTH);
	printf("MAT_B_BLOCK_SIZE: \t\t%d\n",    MAT_B_BLOCK_SIZE);

//        printf("MAT_B_BLOCK_NUM_VECTORS: \t\t%d\n",   MAT_B_BLOCK_NUM_VECTORS);
//        if (MAT_B_BLOCK_SIZE % DOT_PROD_VECTOR_SIZE) {
//          printf("MAT_B_BLOCK_SIZE must be a multiple of DOT_PROD_VECTOR_SIZE\b");
//        }
//        printf("MAT_B_NUM_BLOCKS_IN_ROW: \t\t%d\n",   MAT_B_NUM_BLOCKS_IN_ROW);
//        if (WB % MAT_B_BLOCK_WIDTH) {
//          printf("WB must be a multiple of MAT_B_BLOCK_WIDTH\n");
//        }
//        printf("MAT_B_NUM_BLOCKS_IN_COL: \t\t%d\n",   MAT_B_NUM_BLOCKS_IN_COL);
//        if (HB % MAT_B_BLOCK_HEIGHT) {
//          printf("HB must be a multiple of MAT_B_BLOCK_HEIGHT\n");
//        }
//        printf("MAT_B_NUM_VECTORS_IN_COL_OF_BLOCKS: \t\t%d\n",  MAT_B_NUM_VECTORS_IN_COL_OF_BLOCKS);
//        printf("MAT_B_NUM_VECTORS_IN_MATRIX: \t\t%d\n",         MAT_B_NUM_VECTORS_IN_MATRIX);
//	printf("\n");

	printf("MAT_C_BLOCK_HEIGHT: \t\t%d\n",  MAT_C_BLOCK_HEIGHT);
	printf("MAT_C_BLOCK_WIDTH: \t\t%d\n",   MAT_C_BLOCK_WIDTH);

	/*

        if (HA % COMPUTE_GOLD_BLOCK_SIZE) {
          printf("COMPUTE_GOLD_BLOCK_SIZE must evenly divide HA for gold matrix mult computation!\n");
          exit(1);
        }
        if (WB % COMPUTE_GOLD_BLOCK_SIZE) {
          printf("COMPUTE_GOLD_BLOCK_SIZE must evenly divide WB for gold matrix mult computation!\n");
          exit(1);
        }
        if (WA % COMPUTE_GOLD_BLOCK_SIZE) {
          printf("COMPUTE_GOLD_BLOCK_SIZE must evenly divide WA for gold matrix mult computation!\n");
          exit(1);
        }
		*/

	float* matrix_mul_inputA;
	float* matrix_mul_inputA_block_wise;

	float* matrix_mul_inputB; // transposed
	float* matrix_mul_inputB_transposed; // non-transposed
	float* matrix_mul_inputB_block_wise; // non-transposed

	float* matrix_mul_outputC;
	float* matrix_mul_outputC_1;
	float* matrix_mul_outputC_2;
	float* cnn_out;
	float* cnn_golden;

        float* golden_output;
        float* golden_output_computed_by_blocking;
        float* golden_output_block_wise;
        float* golden_output_block_wise_and_reordered;

         int num_elem_input = IMG_IN_NUM * IMG_IN_ROW * IMG_IN_COL;
         int num_elem_weight = IMG_IN_NUM * IMG_OUT_NUM * KERNEL * KERNEL;
	unsigned int num_elem_A = HA*WA;
	unsigned int num_elem_B = HB*WB;
	unsigned int num_elem_C = HC*WC;


        printf("\n===== Host-CPU preparing A,B matrices and computing golden reference for matrix C ======\n\n");

    float * weight = (float*) acl_aligned_malloc(num_elem_weight * sizeof (float));
    if (!weight) {
        perror("Failed malloc of weights");
    }
    one_array(weight, num_elem_weight);
    float * input = (float*) acl_aligned_malloc(num_elem_input * sizeof (float));
    if (!input) {
        perror("Failed malloc of input feature maps");
    }
    one_array_w_panding(input, num_elem_input, IMG_IN_ROW);
        
        // matrix A
        ////////////  
	if((matrix_mul_inputA = (float*)acl_aligned_malloc(num_elem_A*sizeof(float))) == NULL) {
		perror("Failed malloc of matrix_mul_inputA");
	}
	//randomize_array(matrix_mul_inputA, num_elem_A);
	one_array(matrix_mul_inputA, num_elem_A);

	if((matrix_mul_inputA_block_wise = (float*)acl_aligned_malloc(num_elem_A*sizeof(float))) == NULL) {
		perror("Failed malloc of matrix_mul_inputA_block_wise");
	}

        // matrix B
        ///////////  
	if((matrix_mul_inputB = (float*)acl_aligned_malloc(num_elem_B*sizeof(float))) == NULL) {
		perror("Failed malloc of matrix_mul_inputB");
	}
  	//randomize_array(matrix_mul_inputB, num_elem_B);
  	one_array(matrix_mul_inputB, num_elem_B);
  	
	if((matrix_mul_inputB_transposed = (float*)acl_aligned_malloc(num_elem_B*sizeof(float))) == NULL) {
		perror("Failed malloc of matrix_mul_inputB_transposed");
	}

	if((matrix_mul_inputB_block_wise = (float*)acl_aligned_malloc(num_elem_B*sizeof(float))) == NULL) {
		perror("Failed malloc of matrix_mul_inputB_block_wise");
	}

        ////////////  
	if((matrix_mul_outputC = (float*)acl_aligned_malloc(num_elem_C*sizeof(float))) == NULL) {
		perror("Failed malloc of matrix_mul_outputC");
	}
	memset(matrix_mul_outputC, 0, num_elem_C*sizeof(float));
	if((matrix_mul_outputC_1 = (float*)acl_aligned_malloc(num_elem_C*sizeof(float))) == NULL) {
		perror("Failed malloc of matrix_mul_outputC_1");
	}
	memset(matrix_mul_outputC_1, 0, num_elem_C*sizeof(float));
	if((matrix_mul_outputC_2 = (float*)acl_aligned_malloc(num_elem_C*sizeof(float))) == NULL) {
		perror("Failed malloc of matrix_mul_outputC_2");
	}
	memset(matrix_mul_outputC_2, 0, num_elem_C*sizeof(float));

    int num_elem_output = IMG_OUT_NUM * IMG_ROW * IMG_COL;
	if((cnn_golden= (float*)acl_aligned_malloc(num_elem_output*sizeof(float))) == NULL) {
		perror("Failed malloc of cnn_golden");
	}
	memset(cnn_golden, 0, num_elem_output*sizeof(float));
	if((cnn_out= (float*)acl_aligned_malloc(num_elem_output*sizeof(float))) == NULL) {
		perror("Failed malloc of cnn_out");
	}
	memset(cnn_out, 0, num_elem_output*sizeof(float));

        ////////////  
        
	if((golden_output = (float*)acl_aligned_malloc(num_elem_C*sizeof(float))) == NULL) {
		perror("Failed malloc of golden_output");
	}
	if((golden_output_computed_by_blocking = (float*)acl_aligned_malloc(num_elem_C*sizeof(float))) == NULL) {
		perror("Failed malloc of golden_output compute by blocking");
	}
	memset(golden_output_computed_by_blocking, 0, num_elem_C*sizeof(float));

	if((golden_output_block_wise = (float*)acl_aligned_malloc(num_elem_C*sizeof(float))) == NULL) {
		perror("Failed malloc of golden_output_block_wise");
	}
	if((golden_output_block_wise_and_reordered = (float*)acl_aligned_malloc(num_elem_C*sizeof(float))) == NULL) {
		perror("Failed malloc of golden_output_block_wise_and_reordered\n");
	}

        printf("Allocated memory for host-side matrices!\n");
        printf("Transposing and re-formatting of matrices!\n");

        int HA_trim = 0; 
        int WB_trim = 0; 
        int num_elem_C_gold = 0;
        int num_elem_C_gold_first_section = 0;
        int num_elem_C_gold_last_section = 0;
        int C_gold_first_section_offset = 0;
        int C_gold_last_section_offset = 0;

#ifdef COMPUTE_GOLDEN
		/*
        HA_trim = 2 * MAT_A_BLOCK_HEIGHT;
        WB_trim = WB; // this one cannot be trimmed, compute_gold requires changes for this to work
        num_elem_C_gold = HA_trim * WB_trim;
          
        printf(" *** Computing golden reference of the result C matrix (only a section of the C matrix), HC(section)=%d, WC(section)=%d!\n",HA_trim, WB_trim);
        printf(" *** This takes several minutes...\n");
      //    void compute_gold(float* C, const float* A, const float* B, unsigned int hA, unsigned int wA, unsigned int wB)
        compute_gold(golden_output, matrix_mul_inputA, matrix_mul_inputB, HA_trim, WA, WB_trim);
		*/
        num_elem_C_gold = HC * WC;
        printf(" *** Computing golden reference of the result C matrix, HC=%d, WC=%d!\n",HC, WC);
        compute_gold(golden_output, matrix_mul_inputA, matrix_mul_inputB, HA, WA, WB);
#endif

	printf("Block-wise reformatting of matrix A!\n");
    	block_wise_reformat_matrix(matrix_mul_inputA, matrix_mul_inputA_block_wise, HA, WA, MAT_A_BLOCK_HEIGHT, MAT_A_BLOCK_WIDTH);
        
	printf("Transposing of matrix B!\n");
    	transpose_matrix(matrix_mul_inputB, matrix_mul_inputB_transposed, HB, WB);



#ifdef COMPUTE_GOLDEN_BLOCKED
        printf(" *** Computing golden reference of the result C matrix (computing two sections of matrix C)\n");
        printf(" *** This takes several minutes...\n");

        // first two "rows of blocks"
        HA_trim = 2 * MAT_A_BLOCK_HEIGHT;
        WB_trim = WB; // this one cannot be trimmed, compute_gold_blocked requires changes for this to work
        num_elem_C_gold_first_section = HA_trim * WB_trim;
        C_gold_first_section_offset = 0; 

        printf(" *** Computing the first section of the golden C reference, HC(section)=%d, WC(section)=%d!\n",HA_trim, WB_trim);
        compute_gold_blocked(golden_output_computed_by_blocking, matrix_mul_inputA, matrix_mul_inputB_transposed, HA_trim, WA, WB_trim, HB);

        // last "row of blocks"
        HA_trim = MAT_A_BLOCK_HEIGHT;
        num_elem_C_gold_last_section = HA_trim * WB_trim;
        C_gold_last_section_offset = (HC-HA_trim)*WC;

        printf(" *** Computing the last section of the golden C reference, HC(section)=%d, WC(section)=%d!\n",HA_trim, WB_trim);
        compute_gold_blocked(golden_output_computed_by_blocking + C_gold_last_section_offset, matrix_mul_inputA + (HA-HA_trim)*WA, matrix_mul_inputB_transposed, HA_trim, WA, WB_trim, HB);

//        printf("Comparing golden_output_computed_by_blocking to default golden_output!\n");     
//        //bool gold_by_blocking_ok = compare_L2_norm(golden_output, golden_output_computed_by_blocking, num_elem_C, 0.0f);
//        bool gold_by_blocking_ok = compare_L2_norm(golden_output, golden_output_computed_by_blocking, num_elem_C, 1.0e-6f);

      // for sanity checks and debugging
      // if (gold_by_blocking_ok != true) {
       //   printf("--> golden_output_computed_by_blocking and golden_output DIFFER!\n");     
       // //printDiff(matrix_mul_inputB, matrix_mul_inputB_block_wise, num_elem_B, 1.0e-6f);
       // } else {
       //   printf("--> golden_output_computed_by_blocking and golden_output are the SAME!\n");     
       // }

        golden_output = golden_output_computed_by_blocking;
#endif


	printf("Block-wise reformatting of matrix B!\n");
    	block_wise_reformat_matrix(matrix_mul_inputB_transposed, matrix_mul_inputB_block_wise, WB, HB, MAT_B_BLOCK_WIDTH, MAT_B_BLOCK_HEIGHT);


	printf("Block-wise reformatting of golden output matrix C!\n");
    	block_wise_reformat_matrix(golden_output, golden_output_block_wise, HC, WC, MAT_C_BLOCK_HEIGHT, MAT_C_BLOCK_WIDTH);

	printf("Reordering within blocks of block-wise golden output matrix C!\n");
        reorder_within_blocks(golden_output_block_wise, golden_output_block_wise_and_reordered, HC, WC, SYS_ARRAY_NUM_COLS, MAT_C_BLOCK_WIDTH);
	// printf("Matrix A\n");
	// // print_matrix(matrix_mul_inputA, HA, WA);
	// printf("\n");

	// printf("Matrix B (original)\n");
	// print_matrix(matrix_mul_inputB, HB, WB);
	// printf("\n");
	
	// printf("Matrix B (transposed)\n");
	// // print_matrix(matrix_mul_inputB_transposed, WB, HB);
	// printf("\n");

	// printf("Matrix C (gold)\n");
	// print_matrix(golden_output, HC, WC);
	// printf("\n");

    printf("\n===== Host-CPU setting up the OpenCL platform and device ======\n\n");

	// Use this to check the output of each API call
	cl_int status;

	//----------------------------------------------
	// Discover and initialize the platforms
	//----------------------------------------------
	cl_uint numPlatforms = 0;
	cl_platform_id* platforms = NULL;

	// Use clGetPlatformIDs() to retrieve the
	// number of platforms
	status = clGetPlatformIDs(0, NULL, &numPlatforms);
        fprintf(stdout,"Number of platforms = %d\n", numPlatforms);

	// Allocate enough space for each platform
	platforms = (cl_platform_id*) acl_aligned_malloc (numPlatforms * sizeof(cl_platform_id));
        printf("Allocated space for Platform\n");

	// Fill in platforms with clGetPlatformIDs()
	status = clGetPlatformIDs(numPlatforms, platforms, NULL); CHECK(status);
        printf("Filled in platforms\n");    

	//----------------------------------------------
	// Discover and initialize the devices 
	//----------------------------------------------

	cl_uint numDevices = 0;
	cl_device_id* devices = NULL;

	// Device info
	char buffer[4096];
	unsigned int buf_uint;
	int device_found = 0;

        printf("Initializing IDs\n");    
	for (i=0; i<numPlatforms; i++) {
		status = clGetDeviceIDs(platforms[i],
				CL_DEVICE_TYPE_ALL,
				0,
				NULL,
				&numDevices); 

		if(status == CL_SUCCESS){
			clGetPlatformInfo(platforms[i], 
					CL_PLATFORM_VENDOR,
					4096,
					buffer,
					NULL);
			if(strstr(buffer, "Altera") != NULL){
				device_found = 1;
			}
			printf("%s\n", buffer);

			if(device_found){
				// Allocate enough space for each device
				devices = (cl_device_id*)
					acl_aligned_malloc (numDevices * sizeof(cl_device_id));

				// Fill in devices with clGetDeviceIDs()
				status = clGetDeviceIDs(platforms[i],
						CL_DEVICE_TYPE_ALL,
						numDevices,
						devices,
						NULL);
				break;
			}
		}
	}

	if(!device_found) {
		printf("failed to find a OpenCL device\n");
		exit(-1);
	}

	for (i = 0; i < numDevices; i++) {
		clGetDeviceInfo(devices[i],
				CL_DEVICE_NAME,
				4096,
				buffer,
				NULL);
		fprintf(f_out, "\nDevice Name: %s\n", buffer);

		clGetDeviceInfo(devices[i],
				CL_DEVICE_VENDOR,
				4096,
				buffer,
				NULL);
		fprintf(f_out, "Device Vendor: %s\n", buffer);

		clGetDeviceInfo(devices[i],
				CL_DEVICE_MAX_COMPUTE_UNITS,
				sizeof(buf_uint),
				&buf_uint,
				NULL);
		fprintf(f_out, "Device Computing Units: %u\n", buf_uint);

		clGetDeviceInfo(devices[i],
				CL_DEVICE_GLOBAL_MEM_SIZE,
				sizeof(unsigned long),
				&buffer,
				NULL);
		fprintf(f_out, "Global Memory Size: %i\n", *((unsigned long*)buffer));

		clGetDeviceInfo(devices[i],
				CL_DEVICE_MAX_MEM_ALLOC_SIZE,
				sizeof(unsigned long),
				&buffer,
				NULL);
		fprintf(f_out, "Global Memory Allocation Size: %i\n\n", *((unsigned long*)buffer));
	}



	//----------------------------------------------
	// Create a context 
	//----------------------------------------------

    printf("\n===== Host-CPU setting up the OpenCL command queues ======\n\n");

	cl_context context = NULL;

	// Create a context using clCreateContext() and
	// associate it with the device

	context = clCreateContext(
			NULL,
			1,
			devices,
			NULL,
			NULL,
			&status); CHECK(status);

	//----------------------------------------------
	// Create command queues
	//---------------------------------------------

	cl_command_queue cmdQueue[NUM_QUEUES_TO_CREATE+1]; // extra queue for reading buffer C
        
	// Create a command queue using clCreateCommandQueue(),
	// and associate it with the device you want to execute on
	for(i=0; i<NUM_QUEUES_TO_CREATE; i++) {
            //fprintf(stdout,"cmdQueue i = %d, kernel name = %s\n", i, kernel_name[i]);
            cmdQueue[i] = clCreateCommandQueue(
				context,
				devices[0],
				CL_QUEUE_PROFILING_ENABLE,
				&status); CHECK(status);
	}

        //fprintf(stdout,"cmdQueue i = %d, a queue for reading the C buffer\n", i);
        cmdQueue[i] = clCreateCommandQueue(
                            context,
                            devices[0],
                            CL_QUEUE_PROFILING_ENABLE,
                            &status); CHECK(status);

	//----------------------------------------------
	// Create device buffers
	//----------------------------------------------

	//----------------------------------------------
	// Write host data to device buffers
	//----------------------------------------------


	//----------------------------------------------
	// Create the program from binaries
	//----------------------------------------------
        printf("\n===== Host-CPU setting up OpenCL program and kernels ======\n\n");

   	cl_program program;

	size_t binary_length;
	const unsigned char *binary;

        printf("\nAOCX file: %s\n\n", AOCX_FILE);
        // create the program using binary already compiled offline using aoc (i.e. the .aocx file)
	FILE *fp = fopen(AOCX_FILE, "rb");

	if (fp == NULL) {
		printf("Failed to open the AOCX file (fopen).\n");
		return -1;
	}

	fseek(fp, 0, SEEK_END);
	binary_length = ftell(fp);
	binary = (unsigned char*) malloc(sizeof(unsigned char) * binary_length);
	assert(binary && "Malloc failed");
	rewind(fp);

	if (fread((void*)binary, binary_length, 1, fp) == 0) {
		printf("Failed to read from the AOCX file (fread).\n");
		return -1;
	}
	fclose(fp);

	// Create a program using clCreateProgramWithBinary()
	program = clCreateProgramWithBinary(
			context,
			1,
			devices,
			&binary_length,
			(const unsigned char **)&binary,
			&status,
			NULL); CHECK(status);


	//----------------------------------------------
	// Create the kernel
	//----------------------------------------------

	status = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	if(status != CL_SUCCESS) {
		char log[128*1024] = {0};
		clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, 128*1024, log, NULL);
		printf("%s\n", log);
		CHECK(status);
	}

	double k_start_time[NUM_QUEUES_TO_FINISH];
	double k_end_time[NUM_QUEUES_TO_FINISH];
	double k_exec_time[NUM_QUEUES_TO_FINISH];

    printf("\n ---- Start FPGA from host ---- \n\n");
    fflush(stdout);

    //////////////////////////////////
    // PARAMETER: BNUM_I, BNUM_J, BNUM_K
	/*
    int BNUM_I = 1;  
    int BNUM_J = 1; 
    int BNUM_K = 1; 
	*/
	/*
    int BNUM_I = 8;  
    int BNUM_J = 8; 
    int BNUM_K = 8; 
	*/
    assert(BNUM_I * MAT_A_BLOCK_HEIGHT <= HA);
    assert(BNUM_J * MAT_B_BLOCK_WIDTH  <= WB);
    assert(BNUM_K * MAT_A_BLOCK_WIDTH  <= WA);

//	printf("IN_SIZE=%d\n", BNUM_J * BNUM_K * MAT_B_BLOCK_SIZE);
	idx_array(matrix_mul_inputB_block_wise, BNUM_J * BNUM_K * MAT_B_BLOCK_SIZE);

	float* matrix_mul_inputB_block_wise_padded;

    const int BLOCK_SIZE_WITH_PADDING = (BSIZE_J*PE_COLS+K-1) * K * BSIZE_K * PE_DSIZE;
    const int BLOCK_B_PADDED_BLOCKED_SIZE = BNUM_J * BNUM_K * BLOCK_SIZE_WITH_PADDING;
	if((matrix_mul_inputB_block_wise_padded = (float*)acl_aligned_malloc(BLOCK_B_PADDED_BLOCKED_SIZE* sizeof(float))) == NULL) {
		perror("Failed malloc of matrix_mul_inputB_block_wise_padded");
	}
	memset(matrix_mul_inputB_block_wise_padded, 0, BLOCK_B_PADDED_BLOCKED_SIZE*sizeof(float));

	pad_B(matrix_mul_inputB_block_wise, matrix_mul_inputB_block_wise_padded);

     matmul_kernel_top(status,
       cmdQueue,
       program,
       context,
       num_elem_A,   // HA * WA
       num_elem_B,   // HB * WB
       num_elem_C,   // HC * WC
//       BNUM_I, BNUM_J, BNUM_K,
       matrix_mul_inputA_block_wise,
     //  matrix_mul_inputB_block_wise,
	   matrix_mul_inputB_block_wise_padded,
       matrix_mul_outputC,
       k_start_time,
       k_end_time,
       k_exec_time
       ) ;

    printf("\n ---- Finish FPGA from host ---- \n\n");
    printf("*** Configuration *** \n");
    printf("BNUM_I=%d\n", BNUM_I);
    printf("BNUM_J=%d\n", BNUM_J);
    printf("BNUM_K=%d\n", BNUM_K);
    printf("BSIZE_I=%d\n", ROWS_INTERLEAVED);
    printf("BSIZE_J=%d\n", COLUMNS_INTERLEAVED);
    printf("BSIZE_K=%d\n", BSIZE_K);
    printf("PE_ROWS=%d\n",  PE_ROWS);
    printf("PE_COLS=%d\n",  PE_COLS);
    printf("PE_DSIZE=%d\n", PE_DSIZE);
    printf("\n");
    fflush(stdout);

     {
         int * t_idx_addr = new int [num_elem_C];
         int * t_idx_idx = new int [num_elem_C];
         int * t_idx_i = new int [num_elem_C];
         int * t_idx_j = new int [num_elem_C];

             
         matmul_kernel_sw(
                 num_elem_A,   // HA * WA
                 num_elem_B,   // HB * WB
                 num_elem_C,   // HC * WC
 //                BNUM_I, BNUM_J, BNUM_K,
                 matrix_mul_inputA_block_wise,
                 matrix_mul_inputB_block_wise,
                 matrix_mul_outputC_1,
                 t_idx_addr, t_idx_idx, t_idx_i, t_idx_j
                 ) ;


         cnn_gold(cnn_golden, input, weight);
#if 1

         int num_elem_input = IMG_IN_NUM * IMG_IN_ROW * IMG_IN_COL;
         int num_elem_weight = IMG_IN_NUM * IMG_OUT_NUM * KERNEL * KERNEL;
         int num_elem_output = IMG_OUT_NUM * IMG_ROW * IMG_COL;

         
         cnn_kernel_sw(
                num_elem_input ,
                num_elem_weight,
                num_elem_output,
                 input, weight, cnn_out);


         {

                printf("******************************************\n");
                int err = 0;
                for (int t = 0; t < num_elem_output; t++) {
                    if (fabs(cnn_out[t] - cnn_golden[t]) > 0.01) {
                        if (err < 10) printf("ERROR: idx=%d, out=%lf, exp=%lf\n",
                            t, cnn_out[t], cnn_golden[t]);
                        else if (err == 10) printf("...\n");
                        err++;
                    }
                }
                if (err) printf("[CNN C-model Verification] %d errors\n", err);
                else printf("[CNN C-model Verification] PASSED!\n");      

         }
#endif


         int err = 0;
         printf("******************************************\n");
         for (int t = 0; t  < num_elem_C; t++)
         {
             int t_i = t_idx_i[t];
             int t_j = t_idx_j[t];
             int t_idx = t_idx_idx[t];
             int t_addr = t_idx_addr[t];
            if (fabs(matrix_mul_outputC[t] - matrix_mul_outputC_1[t]) > 0.01)
            {
                if (err < 10) printf("ERROR: idx=%d addr=%d pe(i,j)=(%d,%d)@%d out=%lf, exp=%lf\n",
                        t, t_addr, t_i,t_j,t_idx, matrix_mul_outputC[t], matrix_mul_outputC_1[t]);
                else if (err == 10) printf("...\n");
                err ++;
            }
         }
         if (err) printf("[MM C-model Verification] %d errors\n", err);
         else     printf("[MM C-model Verification] PASSED!\n");
         printf("******************************************\n");

         dump_float_into_file(matrix_mul_outputC, num_elem_C , "c_out.dat");
         dump_float_into_file(matrix_mul_outputC_1, num_elem_C , "c_ref.dat");
     }



    bool res;

// Some sanity checks, this is for debugging only
/*
    printf("\nSanity checks...\n"); 
    printf("Comparing matrix_mul_inputA and matrix_mul_inputA_block_wise!\n");     
    res = compare_L2_norm(matrix_mul_inputA, matrix_mul_inputA_block_wise, num_elem_A, 1.0e-6f);
    if (res != true) {
      printf("--> GOOD, matrix_mul_inputA and matrix_mul_inputA_block_wise differ!\n");     
      // printDiff(matrix_mul_inputB, matrix_mul_inputB_block_wise, num_elem_B, 1.0e-6f);
    } 

    printf("Comparing matrix_mul_inputB and matrix_mul_inputB_block_wise!\n");     
    res = compare_L2_norm(matrix_mul_inputB, matrix_mul_inputB_block_wise, num_elem_B, 1.0e-6f);
    if (res != true) {
      printf("--> GOOD, matrix_mul_inputB and matrix_mul_inputB_block_wise differ!\n");     
      // printDiff(matrix_mul_inputB, matrix_mul_inputB_block_wise, num_elem_B, 1.0e-6f);
    } 

    printf("Comparing golden_output and golden_output_block_wise!\n");     
    res = compare_L2_norm(golden_output_block_wise, golden_output, num_elem_C, 1.0e-6f);
    if (res != true) {
      printf("--> GOOD, golden_output and golden_output_block_wise differ!\n");     
	  // printDiff(golden_output_block_wise, golden_output, num_elem_C, 1.0e-1f);
    } 
    printf("\n\n");
*/

#if 0
    printf("\n===== Comparing FPGA results to golden reference ======\n\n");
    float epsilon = 1.0e-5f;
    printf("Tolerance epsilon for L2-norm: 1.0e-5f = %f\n", epsilon);

    printf("Comparing FPGA results to golden reference (the first section of matrix C)\n");
    res = compare_L2_norm(golden_output_block_wise_and_reordered + C_gold_first_section_offset, matrix_mul_outputC + C_gold_first_section_offset, num_elem_C_gold_first_section, epsilon);
    if (res != true) {
               printDiff(golden_output_block_wise_and_reordered + C_gold_first_section_offset, matrix_mul_outputC + C_gold_first_section_offset, num_elem_C_gold_first_section, epsilon);
    } else { // res == shrTRUE
      printf("Comparing FPGA results to golden reference (the last section of matrix C)\n");
      res = compare_L2_norm(golden_output_block_wise_and_reordered + C_gold_last_section_offset, matrix_mul_outputC + C_gold_last_section_offset, num_elem_C_gold_last_section, epsilon);
      if (res != true) {
                 printDiff(golden_output_block_wise_and_reordered + C_gold_last_section_offset, matrix_mul_outputC + C_gold_last_section_offset, num_elem_C_gold_last_section, epsilon);
      } 
    }
#endif
		
    printf("\n===== Reporting measured throughput ======\n\n");
    double k_earliest_start_time = k_start_time[0];
    double k_latest_end_time     = k_end_time[0];	
	
    for (i=1; i<NUM_QUEUES_TO_FINISH; i++) {

      if (k_start_time[i] < k_earliest_start_time) 	
        k_earliest_start_time 	= k_start_time[i];

      if (k_end_time[i]   > k_latest_end_time) 		
        k_latest_end_time 		= k_end_time[i];
    } 

    // IMPORTANT: we care about the finish time of drain_C, once data is drained we are done
    k_latest_end_time 		= k_end_time[KID_DRAIN_MAT_C];


//    for(i=0; i<NUM_QUEUES_TO_FINISH; i++) {
//      printf("  Kernel execution time on FPGA: %s, \n   \t\t\t\t\t\t\t\t\texec time = %.5f s, start=%.5f s, end=%.5f s\n", kernel_name[i], k_exec_time[i], k_start_time[i], k_end_time[i]);     
//    } 
 
    double k_overall_exec_time = k_latest_end_time - k_earliest_start_time;
	
    printf("\n");
    printf("  Loader kernels start time\t\t= %.5f s\n", k_earliest_start_time);     
    printf("  Drainer kernels end time\t\t= %.5f s\n", k_latest_end_time);     
    printf("  FPGA MatMult exec time\t\t= %.5f s\n", k_overall_exec_time);     

 	// multiplied by 1.0e-9 to get G-FLOPs
    printf("\n");

    //double num_operations = (double)2.0 * WA * HC * WC;
    //double num_operations = (double)2.0 * 
    //    BNUM_I * MAT_A_BLOCK_HEIGHT *
    //    BNUM_J * MAT_B_BLOCK_WIDTH *
    //    BNUM_K * MAT_A_BLOCK_WIDTH ;
    double num_operations = (double)2.0 * 
        BNUM_I * BSIZE_I * PE_COLS * 
        BNUM_J * BSIZE_J * PE_ROWS *
        BNUM_K * BSIZE_K * PE_DSIZE;

    printf("  # operations = %.0f\n", num_operations );     
    printf("  Throughput: %.5f GFLOPS\n", (double)1.0e-9 * num_operations / k_overall_exec_time);     

	printf("\n");
	printf("DONE\n");
//        printf("%s\n\n", (res == true ? "PASSED" : "FAILED"));

        FILE *fp_status;
        fp_status=fopen("matrixMult.txt", "w");
        fprintf(fp_status,"%s\n\n", (res == true ? "PASSED" : "FAILED"));
        fclose(fp_status);

	//----------------------------------------------
	// Release the OpenCL resources
	//----------------------------------------------

	// Free resources
	//for(i=0; i<NUM_KERNELS; i++) {

	for(i=0; i<NUM_QUEUES_TO_CREATE; i++) {
          clReleaseCommandQueue(cmdQueue[i]);
        }
	
	acl_aligned_free(matrix_mul_inputA);
	acl_aligned_free(matrix_mul_inputB);
	acl_aligned_free(matrix_mul_outputC);

	clReleaseProgram(program);
	clReleaseContext(context);

	acl_aligned_free(platforms);
	acl_aligned_free(devices);

}


