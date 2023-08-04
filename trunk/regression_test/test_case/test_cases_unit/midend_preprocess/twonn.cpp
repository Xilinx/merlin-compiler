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
//#include <stdlib.h>
//#include <string.h>
////#include <hls_video.h>
//#include <my_window.h>
#include <math.h>

//#include "cmost.h"

#define DUMP 0
#define learning_rate 0.01f
#define LABEL_SIZE		10
#define HIDDEN_SIZE     100
#define FEATURE_SIZE	784
#define CHUNK_SIZE 20
#define lambda 0.0001f
#define two_lambda 0.0002f
#define two_lambda_learning_rate 0.000002f
#define learning_rate_CHUNK_SIZE 0.0005f
#define WEIGHTS_SIZE 238510 //(FEATURE_SIZE+1)*HIDDEN_SIZE+(HIDDEN_SIZE+1)*LABEL_SIZE
#define TOTAL_SIZE 60000
#define DATA_SIZE 47640000
#pragma ACCEL kernel
void pipeline(int n_samples,   
        float weights1[HIDDEN_SIZE* (FEATURE_SIZE+1)], 
        float bias2[LABEL_SIZE], 
        float weights2[LABEL_SIZE *HIDDEN_SIZE]
//        hls::Window<1,HIDDEN_SIZE,float>& regs,
//        hls::Window<1,HIDDEN_SIZE,float>& regs2,
//        hls::Window<1,HIDDEN_SIZE,float>& regs3
        )
{
	float data[CHUNK_SIZE][FEATURE_SIZE+LABEL_SIZE];
	float neuron1[CHUNK_SIZE][HIDDEN_SIZE];
//#pragma HLS ARRAY_PARTITION variable="neuron1" complete dim="2"
	float error2[CHUNK_SIZE][LABEL_SIZE];
	float error1[CHUNK_SIZE][HIDDEN_SIZE];
//#pragma HLS ARRAY_PARTITION variable="error1" complete dim="2"
    int i,j,k,p;
    int n_stage = n_samples / CHUNK_SIZE;
    for(i = 0; i < n_stage; i++)
    {
        {
       //     memcpy( (void*)*data, (const void*)(global_data + i * CHUNK_SIZE * (FEATURE_SIZE+LABEL_SIZE)), (LABEL_SIZE+FEATURE_SIZE)*CHUNK_SIZE*4 );
        }
        for(j=0; j < CHUNK_SIZE; j++)
        {
//#pragma HLS PIPELINE
            for(k=0; k < HIDDEN_SIZE; k++)
            {
//#pragma HLS UNROLL
                neuron1[j][k]=weights1[(k) * (FEATURE_SIZE + 1) + 0];
            }
        }
        for(p=0; p < FEATURE_SIZE; p++)
        {
            for(j=0; j < CHUNK_SIZE; j++)
            {
//#pragma HLS PIPELINE
                for(k=0; k < HIDDEN_SIZE; k++)
                {
//#pragma HLS UNROLL
                    neuron1[j][k]+=data[j][LABEL_SIZE+p]*weights1[(k) * (FEATURE_SIZE + 1) + p+1];
                }
            }
        }
		/*
        for(j=0; j < CHUNK_SIZE; j++)
        {
            for(k=0; k < HIDDEN_SIZE; k++)
            {
#pragma HLS UNROLL
                regs.insert(neuron1[j][k],0,k);
            }
            for(k=0; k < HIDDEN_SIZE; k++)
            {
#pragma HLS PIPELINE
                float a=regs.getval(0,0);
                regs.shift_left();
                float b= (a/sqrtf(1.f+a*a)+1.f)/2.f;
                regs3.shift_left();
                regs3.insert(b,0,HIDDEN_SIZE-1);
            }
            for(k=0; k < HIDDEN_SIZE; k++)
            {
#pragma HLS UNROLL
                neuron1[j][k]=regs3.getval(0,k);
            }
        }*/
#if DUMP
        FILE* fn1 = fopen("neuron1.log","w");
        for(j=0; j < CHUNK_SIZE; j++)
        {
            for(k=0; k < HIDDEN_SIZE; k++)
            {
                fprintf(fn1, "%f\n", neuron1[j][k]);
            }
        }
        fclose(fn1);
#endif
        for(j=0; j < CHUNK_SIZE; j++)
        {
            for(k=0; k < LABEL_SIZE; k++)
            {
                error2[j][k]=bias2[k];
            }
        }
        for(j=0; j < CHUNK_SIZE; j++)
        {
            for(k=0; k < LABEL_SIZE; k++)
            {
//#pragma HLS PIPELINE
                for(p=0; p < HIDDEN_SIZE; p++)
                {
//#pragma HLS UNROLL
                    error2[j][k]+=neuron1[j][p]*weights2[(k) * HIDDEN_SIZE + p];
                }
            }
        }
        for(j=0; j < CHUNK_SIZE; j++)
        {
            for(k=0; k < LABEL_SIZE; k++)
            {
//#pragma HLS PIPELINE
#pragma HLS DEPENDENCE variable="error2" inter false
                error2[j][k] = (error2[j][k]/sqrtf(1.f+error2[j][k]*error2[j][k])+1.f)/2.f-data[j][k];
                //error2[j][k] = 1.f / ( 1.f + exp(-pre_neuron2[j][k]) )-data[j][k];
            }
        }
#if DUMP
        FILE* fn2 = fopen("neuron2.log","w");
        for(j=0; j < CHUNK_SIZE; j++)
        {
            for(k=0; k < LABEL_SIZE; k++)
            {
                fprintf(fn2, "%f\n", error2[j][k]+data[j][k]);
            }
        }
        fclose(fn2);
#endif
        for(j=0; j < CHUNK_SIZE; j++)
        {
//#pragma HLS PIPELINE
            for(p=0; p < HIDDEN_SIZE; p++)
            {
//#pragma HLS UNROLL
                error1[j][p] = 0.f;
            }
        }
        for(k=0; k < LABEL_SIZE; k++)
        {
            for(j=0; j < CHUNK_SIZE; j++)
            {
//#pragma HLS PIPELINE
                for(p=0; p < HIDDEN_SIZE; p++)
                {
//#pragma HLS UNROLL
                    error1[j][p]+=error2[j][k]*weights2[(k) * HIDDEN_SIZE + p];
                }
            }
        }
		/*
        for(j=0; j < CHUNK_SIZE; j++)
        {
            for(p=0; p < HIDDEN_SIZE; p++)
            {
#pragma HLS UNROLL
                regs.insert(error1[j][p],0,p);
                regs2.insert(neuron1[j][p],0,p);
            }
            for(p=0; p < HIDDEN_SIZE; p++)
            {
#pragma HLS PIPELINE
                float a = regs.getval(0,0);
                regs.shift_left();
                float b = regs2.getval(0,0);
                regs2.shift_left();
                float c = a * b * (1.f - b) * learning_rate_CHUNK_SIZE;
                regs3.shift_left();
                regs3.insert(c,0,HIDDEN_SIZE-1);
            }
            for(p=0; p < HIDDEN_SIZE; p++)
            {
#pragma HLS UNROLL
                error1[j][p]=regs3.getval(0,p);
            }
        } */
#if DUMP
        FILE* fe1 = fopen("error1.log","w");
        for(j=0; j < CHUNK_SIZE; j++)
        {
            for(p=0; p < HIDDEN_SIZE; p++)
            {
                fprintf(fe1, "%f\n", error1[j][p] / learning_rate_CHUNK_SIZE);
            }
        }
        fclose(fe1);
#endif
        for(j=0; j < CHUNK_SIZE; j++)
        {
            for(k=0; k < LABEL_SIZE; k++)
            {
                error2[j][k] *= learning_rate_CHUNK_SIZE;
            }
        }
        for(k=0; k < LABEL_SIZE; k++)
        {
//#pragma HLS PIPELINE
            for(p=0; p < HIDDEN_SIZE; p++)
            {
//#pragma HLS UNROLL
                weights2[(k) * HIDDEN_SIZE + p] -= weights2[(k) * HIDDEN_SIZE + p] * two_lambda_learning_rate;
            }
        }
        for(j = 0; j < CHUNK_SIZE; j++)
        {
            for(k = 0; k < LABEL_SIZE; k++)
            {
                bias2[k] -= error2[j][k];
            }
        }
        for(j = 0; j < CHUNK_SIZE; j++)
        {
            for(k = 0; k < LABEL_SIZE; k++)
            {
//#pragma HLS PIPELINE
                    for(p=0; p < HIDDEN_SIZE; p++)
                    {
//#pragma HLS UNROLL
                        weights2[(k) * HIDDEN_SIZE + p] -= error2[j][k] * neuron1[j][p];
                    }
            }
        }
#if DUMP
        FILE* fw2 = fopen("weights2.log","w");
        for(k = 0; k < LABEL_SIZE; k++)
        {
            fprintf(fw2, "%f\n", bias2[k]);
            for(p=0; p < HIDDEN_SIZE; p++)
            {
                fprintf(fw2, "%f\n", weights2[(k) * HIDDEN_SIZE + p]);
            }
        }
        fclose(fw2);
#endif
        for( p = 1; p < FEATURE_SIZE+1; p++)
        {
//#pragma HLS PIPELINE
                for(k=0; k < HIDDEN_SIZE; k++)
                {
//#pragma HLS UNROLL
                    weights1[(k) * (FEATURE_SIZE + 1) + p] -= weights1[(k) * (FEATURE_SIZE + 1) + p] * two_lambda_learning_rate;
                }
        }
        for(j = 0; j < CHUNK_SIZE; j++)
        {
                for( p = 0; p < FEATURE_SIZE+1; p++)
                {
//#pragma HLS PIPELINE
                    for(k = 0; k < HIDDEN_SIZE; k++)
                    {
//#pragma HLS UNROLL
                        weights1[(k) * (FEATURE_SIZE + 1) + p] -= error1[j][k] * (p==0 ? 1.f : data[j][LABEL_SIZE+p-1]);
                    }
                }
        }
#if DUMP
        FILE* fw1 = fopen("weights1.log","w");
        for(k = 0; k < HIDDEN_SIZE; k++)
        {
                for( p = 0; p < FEATURE_SIZE+1; p++)
                {
                    fprintf(fw1, "%f\n", weights1[(k) * (FEATURE_SIZE + 1) + p]);
                }
        }
        fclose(fw1);
#endif
    }
} 

