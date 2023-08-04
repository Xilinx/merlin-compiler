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

//#include <stdio.h>

#define LABEL_SIZE		10
#define FEATURE_SIZE	784
#define WEIGHT_SIZE 7840 //LABEL_SIZE*FEATURE_SIZE
#define TOTAL_SIZE 1344
#define DATA_SIZE 1067136 //TOTAL_SIZE*(LABEL_SIZE+FEATURE_SIZE)
//#define TOTAL_SIZE 32
#define CHUNK_SIZE 16
#define N_CHUNKS TOTAL_SIZE/CHUNK_SIZE
#define DUP 10

#define my_expf(a) (a)

void recv_data( int exec, volatile float *global_data, unsigned int offset, float data[CHUNK_SIZE][FEATURE_SIZE+LABEL_SIZE] )
{
#pragma HLS INLINE OFF
    if( exec )
    {
        memcpy( *data, global_data + offset, (LABEL_SIZE+FEATURE_SIZE)*CHUNK_SIZE*4 );
    }
}

void compute1( int exec, float data[CHUNK_SIZE][FEATURE_SIZE+LABEL_SIZE], float weights[LABEL_SIZE][FEATURE_SIZE], float result1[CHUNK_SIZE][LABEL_SIZE] )
{
#pragma HLS INLINE OFF
    if( exec )
    {
        int i, j, k;
        STAGE1_D: for( i = 0; i < FEATURE_SIZE; i++ )
        {
            STAGE1_n: for( j = 0; j < CHUNK_SIZE; j++ )
           {
//#pragma HLS PIPELINE
                STAGE1_L: for( k = 0; k < LABEL_SIZE; k++ )
                {
//#pragma ACCEL pipeline_parallel factor = 11
#pragma ACCEL parallel complete prior=logic
                  result1[j][k] += weights[k+1][i] * weights[k][i] * data[j][i+LABEL_SIZE];
                }
            }
        }
    }
}

void compute2( int exec, float result1[CHUNK_SIZE][LABEL_SIZE], float data[CHUNK_SIZE][FEATURE_SIZE+LABEL_SIZE], float result2[CHUNK_SIZE][LABEL_SIZE] )
{
#pragma HLS INLINE OFF
    if( exec )
    {
        int j, k;
        STAGE2_L: for( k = 0; k < LABEL_SIZE; k++ )
        {
            STAGE2_n: for( j = 0; j < CHUNK_SIZE; j++ )
            {
#pragma ACCEL pipeline
                result2[j][k] = (1.f/(1.f+my_expf(-data[j][k]*result1[j][k]))-1.f)*data[j][k];
            }
        }
    }
}

void compute3( int exec, float result2[CHUNK_SIZE][LABEL_SIZE], float data[CHUNK_SIZE][FEATURE_SIZE+LABEL_SIZE], float gradient[LABEL_SIZE][FEATURE_SIZE])
{
#pragma HLS INLINE OFF
    if( exec )
    {
        int i, j, k;
        STAGE3_n: for( j = 0; j < CHUNK_SIZE; j++ )
        {
            STAGE3_D: for( i = 0; i <FEATURE_SIZE; i++ )
            {
#pragma HLS PIPELINE
                STAGE3_L: for( k = 0; k < LABEL_SIZE; k++ )
                {
#pragma ACCEL pipeline_parallel factor = 10
                    gradient[k][i] += data[j][i+LABEL_SIZE] * result2[j][k];
                }
            }
        }
    }
}

void kernel( int i, int n_stage, volatile float *global_data, 
        float weights[LABEL_SIZE][FEATURE_SIZE], 
        float gradient[LABEL_SIZE][FEATURE_SIZE], 
        float data_recv[CHUNK_SIZE][FEATURE_SIZE+LABEL_SIZE], 
        float data_compute1[CHUNK_SIZE][FEATURE_SIZE+LABEL_SIZE], 
        float data_compute2[CHUNK_SIZE][FEATURE_SIZE+LABEL_SIZE], 
        float data_compute3[CHUNK_SIZE][FEATURE_SIZE+LABEL_SIZE], 
        float result1_prod[CHUNK_SIZE][LABEL_SIZE], 
        float result1_cons[CHUNK_SIZE][LABEL_SIZE], 
        float result2_prod[CHUNK_SIZE][LABEL_SIZE], 
        float result2_cons[CHUNK_SIZE][LABEL_SIZE] 
        )
{
#pragma HLS INLINE OFF
#pragma ACCEL pipeline
    recv_data( i < n_stage, global_data, i * CHUNK_SIZE * (FEATURE_SIZE+LABEL_SIZE), data_recv );
    compute1( i > 0 && i < n_stage + 1, data_compute1, weights, result1_prod );
    compute2( i > 1 && i < n_stage + 2, result1_cons, data_compute2, result2_prod );
    compute3( i > 2, result2_cons, data_compute3, gradient );
}

void pipeline( volatile float *global_data, float weights[LABEL_SIZE][FEATURE_SIZE], float gradient[LABEL_SIZE][FEATURE_SIZE] )
{
	float data[4][CHUNK_SIZE][FEATURE_SIZE+LABEL_SIZE];
#pragma HLS ARRAY_PARTITION variable=data complete dim=1
    float result1[2][CHUNK_SIZE][LABEL_SIZE];
#pragma HLS ARRAY_PARTITION variable=result1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=result1 complete dim=3
    float result2[2][CHUNK_SIZE][LABEL_SIZE];
#pragma HLS ARRAY_PARTITION variable=result2 complete dim=1
#pragma HLS ARRAY_PARTITION variable=result2 complete dim=3

    int i;
    int n_stage = TOTAL_SIZE / CHUNK_SIZE;
    int counter = 0;
    TOP_LOOP: for(i = 0; i < n_stage + 3; i++ )
    {
        if( counter == 0 )
        {
            kernel( i, n_stage, global_data, weights, gradient, 
                    data[0], data[1], data[2], data[3], 
                    result1[0], result1[1], result2[0], result2[1] );
        }
        else if ( counter == 1 )
        {
            kernel( i, n_stage, global_data, weights, gradient, 
                    data[3], data[0], data[1], data[2], 
                    result1[1], result1[0], result2[1], result2[0] );
        }
        else if ( counter == 2 )
        {
            kernel( i, n_stage, global_data, weights, gradient, 
                    data[2], data[3], data[0], data[1], 
                    result1[0], result1[1], result2[0], result2[1] );
        }
        else if ( counter == 3 )
        {
            kernel( i, n_stage, global_data, weights, gradient, 
                    data[1], data[2], data[3], data[0], 
                    result1[1], result1[0], result2[1], result2[0] );
        }
        counter++;
        if( counter >= 4 ) counter = 0;
    }
}
#pragma ACCEL kernel
void logistic_kernel( volatile float* global_weights, volatile float* global_data, volatile float* global_gradient )
{
	float weights[LABEL_SIZE][FEATURE_SIZE];
	float gradient[LABEL_SIZE][FEATURE_SIZE];
    memcpy(*gradient, global_gradient, LABEL_SIZE*FEATURE_SIZE*4);
    pipeline(global_data, weights, gradient);
    memcpy(global_gradient, *gradient, LABEL_SIZE*FEATURE_SIZE*4);
}

int main()
{   

    int inc1 = 0, inc2 = 0;
    float * global_weights; ACCEL_malloc_1d( &global_weights, "weights.dat" , 4, WEIGHT_SIZE);
    float * global_data; ACCEL_malloc_1d( &global_data, "data.dat", 4, DATA_SIZE);
    float * global_gradient; ACCEL_malloc_1d( &global_gradient, "zero.dat"  , 4, WEIGHT_SIZE);
	if (!(global_weights && global_data && global_gradient)) while (1);

    logistic_kernel( global_weights, global_data, global_gradient );

    //ACCEL_dump_1d(c, "c_out.dat"); // added
    ACCEL_dump_1d(global_gradient, "gradient.dat"); // added
    ACCEL_free_1d(global_weights);
    ACCEL_free_1d(global_data);
    ACCEL_free_1d(global_gradient);
    return 0;
}

