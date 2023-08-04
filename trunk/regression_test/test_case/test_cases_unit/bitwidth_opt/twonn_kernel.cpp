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



#include<string.h>
#define DUMP 0
#define learning_rate 0.01f
#define LABEL_SIZE		10
#define HIDDEN_SIZE     50
#define FEATURE_SIZE	784
#define CHUNK_SIZE 20
#define lambda 0.0001f
#define two_lambda 0.0002f
#define two_lambda_learning_rate 0.000002f
#define learning_rate_CHUNK_SIZE 0.0005f
#define WEIGHTS_SIZE 238510 //(FEATURE_SIZE+1)*HIDDEN_SIZE+(HIDDEN_SIZE+1)*LABEL_SIZE
#define TOTAL_SIZE 60000
#define DATA_SIZE 47640000

#include "cmost.h"
// ZP: Please note this is to work around the bug in HLS
// if the iteration is less than the pipeline depth, the dependence may be ignored wrongly
//#define ADDITIONAL_CYCLES 2    // for 150MHz
#define ADDITIONAL_CYCLES 5    // for 200MHz

#include <math.h>

void shift_left(float regs[HIDDEN_SIZE])
{
  __merlin_access_range(regs,0,62UL);
    int i;
    for(i=0; i<HIDDEN_SIZE-1; i++)
    {
#pragma ACCEL parallel complete
        regs[i]=regs[i+1];
    }
}

void pipeline(int n_samples, volatile float *global_data, 
              float weights1[HIDDEN_SIZE][FEATURE_SIZE+1], 
              float bias2[LABEL_SIZE], 
              float weights2[LABEL_SIZE][HIDDEN_SIZE],
              float regs[HIDDEN_SIZE],
              float regs2[HIDDEN_SIZE],
              float regs3[HIDDEN_SIZE]
)
{
  __merlin_access_range(regs3,0,62UL);
  __merlin_access_range(regs2,0,62UL);
  __merlin_access_range(regs,0,62UL);
  __merlin_access_range(weights2,0,9UL,0,62UL);
  __merlin_access_range(bias2,0,9UL);
  __merlin_access_range(weights1,0,62UL,0,784UL);
  __merlin_access_range(global_data,0,47639999UL);

    float data[CHUNK_SIZE][FEATURE_SIZE+LABEL_SIZE];
    float neuron1[CHUNK_SIZE][HIDDEN_SIZE];
    float error2[CHUNK_SIZE][LABEL_SIZE];
    float error1[CHUNK_SIZE][HIDDEN_SIZE];
    int i,j,k,p;
    int n_stage = n_samples / CHUNK_SIZE;
    for(i = 0; i < n_stage; i++)
    {
#pragma HLS loop_tripcount max=3000
        {
            memcpy( (void*)*data, (const void*)(global_data + i * CHUNK_SIZE * (FEATURE_SIZE+LABEL_SIZE)), (LABEL_SIZE+FEATURE_SIZE)*CHUNK_SIZE*4 );
        }
        for(j=0; j < CHUNK_SIZE; j++)
        {
#pragma ACCEL pipeline
            for(k=0; k < HIDDEN_SIZE; k++)
            {
#pragma ACCEL parallel complete
                neuron1[j][k]=weights1[k][0];
            }
        }
        for(p=0; p < FEATURE_SIZE; p++)
        {
            for(j=0; j < CHUNK_SIZE; j++)
            {
#pragma ACCEL pipeline
                float temp = data[j][LABEL_SIZE+p];
                for(k=0; k < HIDDEN_SIZE; k++)
                {
#pragma ACCEL parallel complete
                    neuron1[j][k]+=temp*weights1[k][p+1];
                }
            }
        }
        for(j=0; j < CHUNK_SIZE; j++)
        {
            for(k=0; k < HIDDEN_SIZE; k++)
            {
#pragma ACCEL parallel complete
                regs[k]=neuron1[j][k];
            }
            for(k=0; k < HIDDEN_SIZE; k++)
            {
#pragma ACCEL pipeline
                float a=regs[0];
                shift_left(regs);
                float b=(a/sqrtf(1.f+a*a)+1.f)/2.f;
                shift_left(regs3);
                regs3[HIDDEN_SIZE-1]=b;
            }
            for(k=0; k < HIDDEN_SIZE; k++)
            {
#pragma ACCEL parallel complete
                neuron1[j][k]=regs3[k];
            }
        }

        for(j=0; j < CHUNK_SIZE; j++)
        {
            for(k=0; k < LABEL_SIZE; k++)
            {
#pragma ACCEL pipeline
                error2[j][k]=bias2[k];
            }
        }
        for(j=0; j < CHUNK_SIZE; j++)
        {
            for(k=0; k < LABEL_SIZE+ADDITIONAL_CYCLES; k++)
            {
#pragma ACCEL pipeline
                for(p=0; p < HIDDEN_SIZE; p++)
                {
#pragma ACCEL parallel complete
                    if (k < LABEL_SIZE) error2[j][k]+=neuron1[j][p]*weights2[k][p];
                }
            }
        }
        for(j=0; j < CHUNK_SIZE; j++)
        {
            for(k=0; k < LABEL_SIZE; k++)
            {
#pragma ACCEL pipeline
//#pragma ACCEL pipeline
//#pragma HLS DEPENDENCE variable="error2" inter false
                error2[j][k] = (error2[j][k]/sqrtf(1.f+error2[j][k]*error2[j][k])+1.f)/2.f-data[j][k];
            }
        }
        for(j=0; j < CHUNK_SIZE; j++)
        {
#pragma ACCEL pipeline
            for(p=0; p < HIDDEN_SIZE; p++)
            {
#pragma ACCEL parallel complete
                    if (k < LABEL_SIZE) error2[j][k]+=neuron1[j][p]*weights2[k][p];
                error1[j][p] = 0.f;
            }
        }
        for(k=0; k < LABEL_SIZE; k++)
        {
            for(j=0; j < CHUNK_SIZE; j++)
            {
#pragma ACCEL pipeline
                float temp = error2[j][k];
                for(p=0; p < HIDDEN_SIZE; p++)
                {
#pragma ACCEL parallel complete
                    error1[j][p]+=temp*weights2[k][p];
                }
            }
        }
        for(j=0; j < CHUNK_SIZE; j++)
        {
            for(p=0; p < HIDDEN_SIZE; p++)
            {
#pragma ACCEL parallel complete
                regs[p]=error1[j][p];
                regs2[p]=neuron1[j][p];
            }
            for(p=0; p < HIDDEN_SIZE; p++)
            {
#pragma ACCEL pipeline
                float a = regs[0];
                shift_left(regs);
                float b = regs2[0];
                shift_left(regs2);
                float c = a * b * (1.f - b) * learning_rate_CHUNK_SIZE;
                shift_left(regs3);
                regs3[HIDDEN_SIZE-1]=c;
            }
            for(p=0; p < HIDDEN_SIZE; p++)
            {
#pragma ACCEL parallel complete
                error1[j][p]=regs3[p];
            }
        }
        for(j=0; j < CHUNK_SIZE; j++)
        {
            for(k=0; k < LABEL_SIZE; k++)
            {
#pragma ACCEL pipeline
                error2[j][k] *= learning_rate_CHUNK_SIZE;
            }
        }
        for(k=0; k < LABEL_SIZE+ADDITIONAL_CYCLES; k++)
        {
#pragma ACCEL pipeline
            for(p=0; p < HIDDEN_SIZE; p++)
            {
#pragma ACCEL parallel complete
                if (k < LABEL_SIZE) weights2[k][p] -= weights2[k][p] * two_lambda_learning_rate;
            }
        }
        for(j = 0; j < CHUNK_SIZE; j++)
        {
            for(k = 0; k < LABEL_SIZE+ADDITIONAL_CYCLES; k++)
            {
#pragma ACCEL pipeline
                if (k < LABEL_SIZE) bias2[k] -= error2[j][k];
            }
        }
        for(j = 0; j < CHUNK_SIZE; j++)
        {
            for(k = 0; k < LABEL_SIZE+ADDITIONAL_CYCLES; k++)
            {
#pragma ACCEL pipeline
                float temp = error2[j][k];
                for(p=0; p < HIDDEN_SIZE; p++)
                {
#pragma ACCEL parallel complete
                    if (k < LABEL_SIZE) weights2[k][p] -= temp * neuron1[j][p];
                }
            }
        }
        for( p = 1; p < FEATURE_SIZE+1; p++)
        {
#pragma ACCEL pipeline
            for(k=0; k < HIDDEN_SIZE; k++)
            {
#pragma ACCEL parallel complete
                weights1[k][p] -= weights1[k][p] * two_lambda_learning_rate;
            }
        }
        for(j = 0; j < CHUNK_SIZE; j++)
        {
            for( p = 0; p < FEATURE_SIZE+1; p++)
            {
#pragma ACCEL pipeline
                float temp = data[j][LABEL_SIZE+p-1];
                for(k = 0; k < HIDDEN_SIZE; k++)
                {
#pragma ACCEL parallel complete
                    weights1[k][p] -= error1[j][k] * (p==0 ? 1.f : temp);
                }
            }
        }
    }
} 


#pragma ACCEL kernel
void twonn_kernel( int n_samples, volatile float* global_weights, volatile float* global_data )
{
  __merlin_access_range(global_data,0,47639999UL);
  __merlin_access_range(global_weights,0,238509UL);
// #pragma HLS INTERFACE ap_bus port=global_weights depth=397510
// #pragma HLS RESOURCE variable=global_weights core=AXI4M
// #pragma HLS INTERFACE ap_bus port=global_data depth=476400
// #pragma HLS RESOURCE variable=global_data core=AXI4M
// #pragma HLS RESOURCE variable=return core=AXI4LiteS metadata="-bus_bundle control"

    float weights1[HIDDEN_SIZE][FEATURE_SIZE+1];
    float bias2[LABEL_SIZE];
    float weights2[LABEL_SIZE][HIDDEN_SIZE];
    int i,j,k;
    float regs[HIDDEN_SIZE];
    float regs2[HIDDEN_SIZE];
    float regs3[HIDDEN_SIZE];
    float buffer1[FEATURE_SIZE+1];
    for(i=0; i < HIDDEN_SIZE; i++)
    {
        memcpy((void*)buffer1, (const void*)(global_weights+i*(FEATURE_SIZE+1)),(FEATURE_SIZE+1)*4);
        for(j=0; j < FEATURE_SIZE+1; j++)
        {
#pragma ACCEL pipeline 
            for(k=0; k < HIDDEN_SIZE; k++)
            {
#pragma ACCEL parallel complete
                regs[k]=weights1[k][j];
            }
            shift_left(regs);
            for(k=0; k < HIDDEN_SIZE; k++)
            {
#pragma ACCEL parallel complete
                weights1[k][j]=regs[k];
            }
            weights1[HIDDEN_SIZE-1][j]=buffer1[j];
        }
    }
    int weights_offset = (FEATURE_SIZE+1)*HIDDEN_SIZE;
    float buffer[HIDDEN_SIZE+1];
    for(i=0; i < LABEL_SIZE; i++)
    {
        memcpy((void*)buffer, (const void*)(global_weights+weights_offset+(HIDDEN_SIZE+1)*i), (HIDDEN_SIZE+1)*4);
        bias2[i]=buffer[0];
        for(j=0; j < HIDDEN_SIZE; j++)
        {
#pragma ACCEL pipeline
            shift_left(regs);
            regs[HIDDEN_SIZE-1]=buffer[j+1];
        }
        for(j=0; j < HIDDEN_SIZE; j++)
        {
#pragma ACCEL parallel complete
            weights2[i][j]=regs[j];
        }
    }
    pipeline( n_samples, global_data, weights1, bias2, weights2, regs, regs2, regs3);
    for(i=0; i < HIDDEN_SIZE; i++)
    {
        for(j=0; j < FEATURE_SIZE+1; j++)
        {
#pragma ACCEL parallel pipeline
            buffer1[j]=weights1[0][j];
            for(k=0; k < HIDDEN_SIZE; k++)
            {
#pragma ACCEL parallel complete
                regs[k]=weights1[k][j];
            }
            shift_left(regs);
            for(k=0; k < HIDDEN_SIZE; k++)
            {
#pragma ACCEL parallel complete
                weights1[k][j]=regs[k];
            }
        }
        memcpy((void*)(global_weights+i*(FEATURE_SIZE+1)), (const void*)buffer1,(FEATURE_SIZE+1)*4);
    }
    for(i=0; i < LABEL_SIZE; i++)
    {
        for(j=0; j < HIDDEN_SIZE; j++)
        {
#pragma ACCEL parallel complete
            regs[j]=weights2[i][j];
        }
        buffer[0]=bias2[i];
        for(j=0; j < HIDDEN_SIZE; j++)
        {
#pragma ACCEL pipeline
            buffer[j+1]=regs[0];
            shift_left(regs);
        }
        memcpy((void*)(global_weights+weights_offset+(HIDDEN_SIZE+1)*i), (const void*)buffer, (HIDDEN_SIZE+1)*4);
    }
}



