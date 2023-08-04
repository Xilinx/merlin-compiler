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

//#include "baseline.h"

void gradient(int, int, int, double*, double*, double*);

int main(int argc, char** argv) {

  if (argc < 2) {
    printf("USAGE: %s <num_samples>\n", argv[0]);
    return -1;
  }
  struct	timeval t1, t2, tr;

  // prepare data
  int feature_size = 784;
  int num_labels = 10;
  int num_samples = atoi(argv[1]);
  assert(num_samples <= 32);
  int data_size = (feature_size+1) * num_samples;
  int weight_size = (num_labels-1) * feature_size;

  // input
  //double* data_samples = new double[data_size]; // ZP: 20151115: ROSE def-use has problem when the expression is in "new" expression
  double* data_samples =( double *) malloc(sizeof(double) * data_size);
  double* weights = new double[weight_size];

  // output
  double* output_base = new double[weight_size+1];
  double* output_test = new double[weight_size+1];

  // setup input with random data
  for (int i=0; i<num_samples; i++) {
    data_samples[i*(feature_size+1)] = rand()%num_labels; 
    for (int j=0; j<feature_size; j++) {
      data_samples[i*(feature_size+1)+1+j] = (double)rand()/RAND_MAX;
    }
  }
  for (int i=0; i<weight_size; i++) {
    weights[i] = (double)rand()/RAND_MAX;
  }

  // compute baseline results
  //gradient_base(num_samples, weights, data_samples, output_base);

  // compute test results

  gradient(num_samples, 10, 784, weights, data_samples, output_test);

  // compare results
  double diff_total = 0.0;
  double diff_ratio = 0.0;
  double max_diff = 0.0;
  for (int k=0; k<weight_size+1; k++) {
    double diff = abs(output_base[k] - output_test[k]); 
    if (diff > max_diff) {
      max_diff = diff;
    }

    diff_total += diff;
    if (output_base[k]!=0) {
      diff_ratio += diff / abs(output_base[k]);
    }

    if (diff / abs(output_base[k]) > 0.05) {
      printf("%d: %f|%f, ratio=%f\n", 
          k,
          output_base[k], 
          output_test[k],
          diff/abs(output_base[k]));
    }
  }
  printf("diff: %f max, %f/point, %f%/point\n",
      max_diff,
      diff_total/(weight_size+1),
      diff_ratio/(weight_size+1)*100.0);

  delete output_base;
  delete output_test;
  delete data_samples;
  delete weights;

  return 0;
}
