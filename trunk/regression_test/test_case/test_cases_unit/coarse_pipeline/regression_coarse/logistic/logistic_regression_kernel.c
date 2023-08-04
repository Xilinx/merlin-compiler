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
#include "logistic.h"
#include <math.h>

void shift_left(float regs[LABEL_SIZE])
{
#pragma HLS INLINE
    int i;
    for(i=0; i<LABEL_SIZE-1; i++)
    {
#pragma HLS UNROLL
        regs[i]=regs[i+1];
    }
}

//void recv_data( volatile float *global_data, int i, float data[CHUNK_SIZE][FEATURE_SIZE+LABEL_SIZE] )
void recv_data( volatile float *global_data, unsigned int offset, float data[CHUNK_SIZE][FEATURE_SIZE+LABEL_SIZE] )
{
#pragma HLS INLINE OFF
    {
	//	unsigned int offset=i * CHUNK_SIZE * (FEATURE_SIZE+LABEL_SIZE);
        memcpy( *data, global_data + offset, (LABEL_SIZE+FEATURE_SIZE)*CHUNK_SIZE*4 );
    }
}

void compute1( float data[CHUNK_SIZE][FEATURE_SIZE+LABEL_SIZE], float weights[LABEL_SIZE][FEATURE_SIZE+1], float result1[CHUNK_SIZE][LABEL_SIZE] )
{
#pragma HLS INLINE OFF
    {
        int i, j, k;
        for( j = 0; j < CHUNK_SIZE; j++ )
        {
#pragma HLS PIPELINE
            for( k = 0; k < LABEL_SIZE; k++ )
            {
#pragma HLS UNROLL
                result1[j][k] = weights[k][0];
            }
        }
STAGE1_D: for( i = 0; i < FEATURE_SIZE; i++ )
          {
STAGE1_n: for( j = 0; j < CHUNK_SIZE; j++ )
          {
#pragma HLS PIPELINE
STAGE1_L: for( k = 0; k < LABEL_SIZE; k++ )
          {
              result1[j][k] += weights[k][i+1] * data[j][i+LABEL_SIZE];
          }
          }
          }
    }
}

void compute2(float result1[CHUNK_SIZE][LABEL_SIZE], float data[CHUNK_SIZE][FEATURE_SIZE+LABEL_SIZE], float result2[CHUNK_SIZE][LABEL_SIZE] )
{
#pragma HLS INLINE OFF
    {
        int j, k;
STAGE2_L: for( k = 0; k < LABEL_SIZE; k++ )
          {
STAGE2_n: for( j = 0; j < CHUNK_SIZE; j++ )
          {
#pragma HLS pipeline
              float temp = 1+result1[j][k]*result1[j][k];
              result2[j][k] = result1[j][k]/temp-(2.f*data[j][k]-1.f)/(temp);
              //result2[j][k] = 1.f/(1.f+expf(-result1[j][k]))-data[j][k];
          }
          }
    }
}

void compute3(float result2[CHUNK_SIZE][LABEL_SIZE], float data[CHUNK_SIZE][FEATURE_SIZE+LABEL_SIZE], float gradient[LABEL_SIZE][FEATURE_SIZE+1])
{
#pragma HLS INLINE OFF
    {
        int i, j, k;
STAGE3_n: for( j = 0; j < CHUNK_SIZE; j++ )
          {
STAGE3_D: for( i = 0; i <FEATURE_SIZE+1; i++ )
          {
#pragma HLS PIPELINE
STAGE3_L: for( k = 0; k < LABEL_SIZE; k++ )
          {
#pragma HLS UNROLL
              gradient[k][i] += result2[j][k] * (i==0 ? 1.f : data[j][i-1+LABEL_SIZE]);
          }
          }
          }
    }
}

void pipeline( int n_samples, volatile float *global_data, float weights[LABEL_SIZE][FEATURE_SIZE+1], float gradient[LABEL_SIZE][FEATURE_SIZE+1] )
{
    int i;
   // int n_stage = n_samples / CHUNK_SIZE;
    int n_stage = TOTAL_SIZE / CHUNK_SIZE;
		
TOP_LOOP: for(i = 0; i < n_stage; i++ )
          {
				#pragma ACCEL pipeline
		  		float data[CHUNK_SIZE][FEATURE_SIZE+LABEL_SIZE];
					float result1[CHUNK_SIZE][LABEL_SIZE];
					float result2[CHUNK_SIZE][LABEL_SIZE];


						// recv_data( global_data, i, data );
  		  recv_data( global_data, i * CHUNK_SIZE * (FEATURE_SIZE+LABEL_SIZE), data );
		    compute1( data, weights, result1 );
		    compute2( result1, data, result2 );
		    compute3( result2, data, gradient );

		  }
}

#pragma ACCEL kernel
void logistic_regression_kernel( int n_samples, float global_weights[WEIGHTS_SIZE], float global_data[DATA_SIZE], float global_gradient[WEIGHTS_SIZE] )
{
#ifdef ADD_IF
#pragma HLS INTERFACE m_axi port=global_data offset=slave
#pragma HLS INTERFACE m_axi port=global_weights offset=slave
#pragma HLS INTERFACE m_axi port=global_gradient offset=slave
#pragma HLS INTERFACE s_axilite port=n_samples
#pragma HLS INTERFACE s_axilite port=return
#endif
    float weights[LABEL_SIZE][FEATURE_SIZE+1];
    float gradient[LABEL_SIZE][FEATURE_SIZE+1];
    int i,j,k;
    float regs[LABEL_SIZE];
//#pragma HLS ARRAY_PARTITION variable="regs" complete dim="1"
    float buffer[FEATURE_SIZE+1];
//#pragma HLS ARRAY_PARTITION variable="weights" complete dim="1"
//#pragma HLS ARRAY_PARTITION variable="gradient" complete dim="1"

    n_samples = TOTAL_SIZE;

#if 0
    for(i=0; i < LABEL_SIZE; i++)
    {
#pragma HLS UNROLL
        memcpy((void*)(weights[i]), (const void*)(global_weights+(FEATURE_SIZE+1)*i), (FEATURE_SIZE+1)*4);
        memcpy((void*)(gradient[i]), (const void*)(global_gradient+(FEATURE_SIZE+1)*i), (FEATURE_SIZE+1)*4);
    }
#else
    for(i=0; i < LABEL_SIZE; i++)
    {
#pragma HLS UNROLL
        int j;
        for(j = 0; j < FEATURE_SIZE+1; j++) {
            gradient[i][j] = 0.f;
        }
    }
    for(i=0; i < LABEL_SIZE; i++)
    {
        memcpy((void*)buffer, (const void*)(global_weights+(FEATURE_SIZE+1)*i), (FEATURE_SIZE+1)*4);
        for(j=0; j < FEATURE_SIZE+1; j++)
        {
            for(k=0; k < LABEL_SIZE; k++)
            {
#pragma HLS UNROLL
                regs[k]=weights[k][j];
            }
            shift_left(regs);
            for(k=0; k < LABEL_SIZE; k++)
            {
#pragma HLS UNROLL
                weights[k][j]=regs[k];
            }
            weights[LABEL_SIZE-1][j]=buffer[j];
        }
    }
#endif
    pipeline( n_samples, global_data, weights, gradient);
#if 0
    for(i=0; i < LABEL_SIZE; i++)
    {
#pragma HLS UNROLL
        memcpy((void*)(global_gradient+(FEATURE_SIZE+1)*i), (const void*)(gradient[i]), (FEATURE_SIZE+1)*4);
    }
#else
    for(i=0; i < LABEL_SIZE; i++)
    {
        for(j=0; j < FEATURE_SIZE+1; j++)
        {
            buffer[j]=gradient[0][j];
            for(k=0; k < LABEL_SIZE; k++)
            {
#pragma HLS UNROLL
                regs[k]=gradient[k][j];
            }
            shift_left(regs);
            for(k=0; k < LABEL_SIZE; k++)
            {
#pragma HLS UNROLL
                gradient[k][j]=regs[k];
            }
        }
        memcpy((void*)(global_gradient+i*(FEATURE_SIZE+1)), (const void*)buffer,(FEATURE_SIZE+1)*4);
    }
#endif
}


int main()
{   

    int inc1 = 0, inc2 = 0;
    int n_samples = TOTAL_SIZE;
    float * global_weights; cmost_malloc_1d( &global_weights, "0" , 4, WEIGHTS_SIZE);
    float * global_data; cmost_malloc_1d( &global_data, "global.dat", 4, DATA_SIZE);
    float * global_gradient; cmost_malloc_1d( &global_gradient, "0"  , 4, WEIGHTS_SIZE);
    float * global_gradient_gold; cmost_malloc_1d( &global_gradient_gold, "gradient_gold.dat"  , 4, WEIGHTS_SIZE);
	if (!(global_weights && global_data && global_gradient)) while (1);

    logistic_regression_kernel( n_samples, global_weights, global_data, global_gradient );

    int i;
    int err=0;
    double epsilon = 1.0e-16;

    for (i = 0 ; i < WEIGHTS_SIZE; i++) {
        if (fabs(global_gradient[i] - global_gradient_gold[i]) > epsilon) {
            err++;
            printf("Test FAIL at %d: %f (expected %f)\n", i, global_gradient[i], global_gradient_gold[i]);
        }
    }

    cmost_write_file(global_gradient, "gradient.dat", 4*WEIGHTS_SIZE); // added
    cmost_dump(global_gradient, "out.dat");
    cmost_free_1d(global_weights);
    cmost_free_1d(global_data);
    cmost_free_1d(global_gradient);
    cmost_free_1d(global_gradient_gold);

    if (err != 0) return 1;
    return 0;
}

