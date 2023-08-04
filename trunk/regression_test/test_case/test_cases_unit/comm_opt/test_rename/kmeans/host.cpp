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
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
//#include <fcntl.h>
//#include <string>
//#include <fstream>
#include <math.h>
#include "baseline.h"
void __merlinoldkernel_kmeans(int _num_samples,int _num_runs,int _num_clusters,int _vector_length,double *data_0,double *centers,double sums[784 * 10],int counts[10],double *output);
#include "__merlinhead_kmeans.h"

int main(int argc,char **argv)
{
  struct timeval t1;
  struct timeval t2;
  struct timeval tr;
// prepare data
  int feature_size = 784;
// for test set num_runs to be 1
  int num_runs = 1;
  int num_clusters = 10;
  int num_samples = 1024;
  if (argc > 1) {
    num_samples = atoi(argv[1]);
  }
  if (argc > 3) {
    num_clusters = atoi(argv[2]);
    feature_size = atoi(argv[3]);
  }
  int data_size = (feature_size + 1) * num_samples;
  int center_size = num_clusters * num_runs * (feature_size + 1);
  int output_size = num_clusters * num_runs * (feature_size + 3);
#define MAX_NUM_CLUSTERS 10
#define MAX_NUM_RUNS 1
#define MAX_VECTOR_LENGTH 1024
#define MAX_NUM_SAMPLES 10000
  data_size <= 10000 * (1024 + 1)?((void )0) : __assert_fail("data_size <= 10000*(1024+1)","host.cpp",45,__PRETTY_FUNCTION__);
  center_size <= 1 * 10 * (1024 + 1)?((void )0) : __assert_fail("center_size <= 1*10*(1024+1)","host.cpp",46,__PRETTY_FUNCTION__);
  output_size <= 1 * 10 * (1024 + 3)?((void )0) : __assert_fail("output_size <= 1*10*(1024+3)","host.cpp",47,__PRETTY_FUNCTION__);
  double *data_samples = new double [data_size];
  double *data_0 = new double [10000 * 1024];
  double *data_1 = new double [10000];
  double *centers = new double [center_size];
  double *centers_0 = new double [center_size];
  double *output_base = new double [output_size];
  double *output_test = new double [output_size];
// setup input with random data
  for (int i = 0; i < num_samples; i++) {
    double norm = 0.0;
    for (int j = 0; j < feature_size; j++) {
      double tmp;
      data_samples[i * (feature_size + 1) + j] = tmp = ((double )(rand())) / 2147483647;
      data_0[i * feature_size + j] = tmp;
      norm += data_samples[i * (feature_size + 1) + j] * data_samples[i * (feature_size + 1) + j];
    }
    norm = sqrt(norm);
    data_samples[i * (feature_size + 1) + feature_size] = norm;
    data_1[i] = norm;
  }
  for (int i = 0; i < num_clusters * num_runs; i++) {
    double norm = 0.0;
    for (int j = 0; j < feature_size; j++) {
      double tmp;
      centers[i * (feature_size + 1) + j] = tmp = ((double )(rand())) / 2147483647;
      centers_0[i * feature_size + j] = tmp;
      norm += centers[i * (feature_size + 1) + j] * centers[i * (feature_size + 1) + j];
    }
    centers[i * (feature_size + 1) + feature_size] = sqrt(norm);
  }
// compute baseline results
  kmeans_base(num_samples,num_runs,num_clusters,feature_size,data_samples,centers,output_base);
  double sums[784 * 10];
  int counts[10];
// compute test results
  
#pragma ACCEL string opencl_init_kmeans("kernel_top.aocx");
  __merlinwrapper_kmeans(num_samples,num_runs,num_clusters,feature_size,data_0,centers_0,sums,counts,output_test);
  for (int j = 0; j < 10; j++) {
    int offset = j * 787;
    output_test[offset + 0] = 0;
    output_test[offset + 1] = j;
    for (int k = 0; k < 784; k++) {
      output_test[offset + 2 + k] = sums[j * 784 + k];
    }
    output_test[offset + 2 + 784] = ((double )counts[j]);
  }
// compare results
  double diff_total = 0.0;
  double diff_ratio = 0.0;
  double max_diff = 0.0;
  for (int k = 0; k < output_size; k++) {
    double diff = abs(output_base[k] - output_test[k]);
    if (diff > max_diff) {
      max_diff = diff;
    }
    diff_total += diff;
    if (output_base[k] != 0) {
      diff_ratio += diff / abs(output_base[k]);
    }
    if (diff / abs(output_base[k]) > 1e-4) {
      printf("%d: %f|%f, ratio=%f\n",k,output_base[k],output_test[k],diff / abs(output_base[k]));
    }
  }
  printf("diff: %f max, %f/point, %f%/point\n",max_diff,diff_total / output_size,diff_ratio / output_size * 100.0);
  delete []output_base;
  delete []output_test;
  delete []data_samples;
  delete []centers;
  return 0;
}
