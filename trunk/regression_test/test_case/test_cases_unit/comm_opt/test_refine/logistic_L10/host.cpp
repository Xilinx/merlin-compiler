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
typedef double BUS_TYPE;
void gradient(int ,int ,int ,double *,BUS_TYPE *,double *,double *,double *,double *);

int main(int argc,char **argv)
{
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
//double* data_samples = new double[data_size]; // ZP: 20151115: ROSE def-use has problem when the expression is in "new" expression
  double *data_samples = (double *)(malloc(sizeof(double ) * data_size));
  double *feature_samples = (double *)(malloc(sizeof(double ) * feature_size * num_samples));
  double *label_samples = (double *)(malloc(sizeof(double ) * num_samples));
  double *weights = new double [weight_size];
// output
  double *output_base = new double [weight_size + 1];
  double *output_test = new double [weight_size];
  double *output_last_element = new double [1];
  double tt;
// setup input with random data
  for (int i = 0; i < num_samples; i++) {
    tt = data_samples[i * (feature_size + 1)] = (rand() % num_labels);
    label_samples[i] = tt;
    for (int j = 0; j < feature_size; j++) {
      tt = data_samples[i * (feature_size + 1) + 1 + j] = ((double )(rand())) / 2147483647;
      feature_samples[i * feature_size + j] = tt;
    }
  }
  for (int i = 0; i < weight_size; i++) {
    weights[i] = ((double )(rand())) / 2147483647;
    output_test[i] = 0;
  }
  output_last_element[0] = 0;
// compute baseline results
  gradient_base(num_samples,weights,data_samples,output_base);
// compute test results
  
  gradient(num_samples,10,784,weights,data_samples,output_test,output_last_element,feature_samples,label_samples);
// compare results
  double diff_total = 0.0;
  double diff_ratio = 0.0;
  double max_diff = 0.0;
  for (int k = 0; k < weight_size + 1; k++) {
    double diff = abs(output_base[k] - ((k == weight_size? *output_last_element : output_test[k])));
    if (diff > max_diff) {
      max_diff = diff;
    }
    diff_total += diff;
    if (output_base[k] != 0) {
      diff_ratio += diff / abs(output_base[k]);
    }
    if (diff / abs(output_base[k]) > 0.05) {
      printf("%d: %f|%f, ratio=%f\n",k,output_base[k],(k == weight_size?output_last_element[0] : output_test[k]),diff / abs(output_base[k]));
    }
  }
  printf("diff: %f max, %f/point, %f%/point\n",max_diff,diff_total / (weight_size + 1),diff_ratio / (weight_size + 1) * 100.0);
  delete []output_base;
  delete []output_test;
  free(data_samples);
  free(feature_samples);
  free(label_samples);
  delete []weights;
  return 0;
}
