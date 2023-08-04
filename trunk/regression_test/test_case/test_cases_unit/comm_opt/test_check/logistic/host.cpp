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
#include<stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
//#include <string>
//#include <fstream>
#include <math.h>
#include <assert.h>
#include "baseline.h"
typedef float BUS_TYPE;
void __merlinoldkernel_gradient(int n_samples,int label_size,int feature_size,float *weights,float *data,float *output,float *output_last_element,float *feature_samples,float *label_samples);
#include "__merlinhead_gradient.h"

int main(int argc,char **argv)
{
  
#pragma ACCEL string opencl_init_gradient(bitstream);
//int num_samples = 60000; 
//int num_samples = 5120;  // CPU emulation fails because of large buffer
  int num_samples = 2560;
//if (argc < 2) {
//  printf("USAGE: %s <num_samples>\n", argv[0]);
////    return -1;    
//    num_samples = 512;
//}
//else 
//{
//    num_samples = atoi(argv[1]);
//}
  struct timeval t1;
  struct timeval t2;
  struct timeval tr;
// prepare data
  int feature_size = 784;
  int num_labels = 10;
//assert(num_samples <= 64);
  num_samples <= 2560?((void )0) : __assert_fail("num_samples <= 2560","host.cpp",36,__PRETTY_FUNCTION__);
  int data_size = (feature_size + 1) * num_samples;
  int weight_size = (num_labels - 1) * feature_size;
// input
//float* data_samples = new float[data_size]; // ZP: 20151115: ROSE def-use has problem when the expression is in "new" expression
  float *data_samples = (float *)(malloc(((size_t )4) * data_size));
  float *feature_samples = (float *)(malloc(((size_t )4) * feature_size * num_samples));
  float *label_samples = (float *)(malloc(((size_t )4) * num_samples));
  float *weights = new float [weight_size];
  float *weights_hw = new float [weight_size];
// output
  float *output_base = new float [weight_size + 1];
  float *output_test = new float [weight_size];
  float *output_last_element = new float [1];
  float tt;
// setup input with random data
  for (int i = 0; i < num_samples; i++) {
    tt = data_samples[i * (feature_size + 1)] = (rand() % num_labels);
    label_samples[i] = tt;
    for (int j = 0; j < feature_size; j++) {
      tt = data_samples[i * (feature_size + 1) + 1 + j] = ((float )(rand())) / 2147483647;
      feature_samples[i * feature_size + j] = tt;
    }
  }
  for (int i = 0; i < num_labels - 1; i++) {
    for (int j = 0; j < feature_size; ++j) {
      float tmp = ((float )(rand())) / 2147483647;
      weights[i * feature_size + j] = tmp;
      weights_hw[j * (num_labels - 1) + i] = tmp;
      output_test[i * feature_size + j] = 0;
    }
  }
  output_last_element[0] = 0;
// compute baseline results
  gradient_base(num_samples,weights,data_samples,output_base);
// compute test results
  __merlinwrapper_gradient(num_samples,10,784,weights_hw,data_samples,output_test,output_last_element,feature_samples,label_samples);
// compare results
  float diff_total = 0.0;
  float diff_ratio = 0.0;
  float max_diff = 0.0;
  for (int k = 0; k < weight_size + 1; k++) {
    float rose_temp__1;
    if (k == weight_size) {
      rose_temp__1 =  *output_last_element;
    }
     else {
      rose_temp__1 = output_test[k];
    }
    float diff = abs(output_base[k] - rose_temp__1);
    if (diff > max_diff) {
      max_diff = diff;
    }
    diff_total += diff;
    if (output_base[k] != 0) {
      diff_ratio += diff / abs(output_base[k]);
    }
    if ((diff / abs(output_base[k])) > 0.05) {
      float rose_temp__2;
      if (k == weight_size) {
        rose_temp__2 = output_last_element[0];
      }
       else {
        rose_temp__2 = output_test[k];
      }
      printf("%d: %f|%f, ratio=%f\n",k,output_base[k],rose_temp__2,(diff / abs(output_base[k])));
    }
  }
  printf("diff: %f max, %f/point, %f%/point\n",max_diff,(diff_total / (weight_size + 1)),(diff_ratio / (weight_size + 1)) * 100.0);
  delete []output_base;
  delete []output_test;
  free(data_samples);
  free(feature_samples);
  free(label_samples);
  delete []weights;
  delete []weights_hw;
  return 0;
}
