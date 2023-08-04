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
// if the iteration is less than the pipeline depth, the dependence may be ignored wrongly
//#define ADDITIONAL_CYCLES 2    // for 150MHz
#define ADDITIONAL_CYCLES 5    // for 200MHz
#include <math.h>

void shift_left(float regs[50])
{
  int i;
  for (i = 0; i < 50 - 1; i++) {
    regs[i] = regs[i + 1];
  }
}

void pipeline(int n_samples,volatile float *global_data,float weights1[50][784 + 1],float bias2[10],float weights2[10][50],float regs[50],float regs2[50],float regs3[50])
{
  float data[20][784 + 10];
  float neuron1[20][50];
  float error2[20][10];
  float error1[20][50];
  int i;
  int j;
  int k;
  int p;
  int n_stage = n_samples / 20;
  for (i = 0; i < n_stage; i++) {
    memcpy(((void *)( *data)),((const void *)(global_data + i * 20 * (784 + 10))),(10 + 784) * 20 * 4);
  }
}

#pragma ACCEL kernel
void twonn_kernel(int n_samples,volatile float *global_weights,volatile float *global_data)
{
  float weights1[50][784 + 1];
  float bias2[10];
  float weights2[10][50];
  int i;
  int j;
  int k;
  float regs[50];
  float regs2[50];
  float regs3[50];
  float buffer1[784 + 1];
  for (i = 0; i < 50; i++) 
/* Original: #pragma ACCEL pipeline */
{
    
#pragma ACCEL PIPELINE II=1
    memcpy(((void *)buffer1),((const void *)(global_weights + i * (784 + 1))),(784 + 1) * 4);
    for (j = 0; j < 784 + 1; j++) 
/* Original: #pragma ACCEL pipeline */
{
      
#pragma ACCEL PIPELINE II=1
      for (k = 0; k < 50; k++) {
        regs[k] = weights1[k][j];
      }
      shift_left(regs);
      for (k = 0; k < 50; k++) {
        weights1[k][j] = regs[k];
      }
      weights1[50 - 1][j] = buffer1[j];
    }
  }
  int weights_offset = (784 + 1) * 50;
  float buffer[50 + 1];
  for (i = 0; i < 10; i++) 
/* Original: #pragma ACCEL pipeline */
{
    
#pragma ACCEL PIPELINE II=1
    memcpy(((void *)buffer),((const void *)(global_weights + weights_offset + (50 + 1) * i)),(50 + 1) * 4);
    bias2[i] = buffer[0];
    for (j = 0; j < 50; j++) {
      shift_left(regs);
      regs[50 - 1] = buffer[j + 1];
    }
    for (j = 0; j < 50; j++) {
      weights2[i][j] = regs[j];
    }
  }
  pipeline(n_samples,global_data,weights1,bias2,weights2,regs,regs2,regs3);
  for (i = 0; i < 50; i++) 
/* Original: #pragma ACCEL pipeline */
{
    
#pragma ACCEL PIPELINE II=1
    for (j = 0; j < 784 + 1; j++) {
      buffer1[j] = weights1[0][j];
      for (k = 0; k < 50; k++) {
        regs[k] = weights1[k][j];
      }
      shift_left(regs);
      for (k = 0; k < 50; k++) {
        weights1[k][j] = regs[k];
      }
    }
    memcpy(((void *)(global_weights + i * (784 + 1))),((const void *)buffer1),(784 + 1) * 4);
  }
  for (i = 0; i < 10; i++) 
/* Original: #pragma ACCEL pipeline */
{
    
#pragma ACCEL PIPELINE II=1
    for (j = 0; j < 50; j++) {
      regs[j] = weights2[i][j];
    }
    buffer[0] = bias2[i];
    for (j = 0; j < 50; j++) {
      buffer[j + 1] = regs[0];
      shift_left(regs);
    }
    memcpy(((void *)(global_weights + weights_offset + (50 + 1) * i)),((const void *)buffer),(50 + 1) * 4);
  }
}
