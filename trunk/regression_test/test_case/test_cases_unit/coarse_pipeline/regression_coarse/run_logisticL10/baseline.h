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
#include <string.h>
//#include <limits>
#include <math.h>

#define LABEL_SIZE		10
#define FEATURE_SIZE	784

void gradient_base( 
    int num_samples, 
    double* weights, 
    double* data, 
    double* output )
{
  const int L = LABEL_SIZE;
  const int D = FEATURE_SIZE;
  const int weight_size = (L-1)*D;

  memset(output, 0, sizeof(double)*(weight_size+1));

  double* margins = new double[L-1];

  for(int k = 0; k < num_samples; k++ ) {

    double marginY = 0.0;
    //double maxMargin = -std::numeric_limits<double>::infinity();
    double maxMargin = -1e120;
    int    maxMarginIndex = 0;

    double  label = data[k*(D+1)];
    double* feature = data + k*(D+1) + 1;

    for (int i=0; i<L-1; i++) {
      double margin = 0.0;
      for(int j = 0; j < D; j+=1 ) {
        margin += weights[i*D+j] * feature[j];
      }
      if (i == (int)label - 1) {
        marginY = margin;
      }
      if (margin > maxMargin) {
        maxMargin = margin;
        maxMarginIndex = i;
      }
      margins[i] = margin;
    }

    double sum = 0.0;
    for (int i=0; i<L-1; i++) {
      if (maxMargin > 0) {
        margins[i] -= maxMargin;
        if (i == maxMarginIndex) {
          sum += exp(-maxMargin);
        }
        else {
          sum += exp(margins[i]);
        }
      } 
      else {
        sum += exp(margins[i]);
      }
    }

    // update gradient
    for(int i = 0; i < L-1; i++ ) {
      double multiplier = exp(margins[i]) / (sum+1.0);
      if (label != 0.0 && label == i+1) {
        multiplier -= 1.0;
      }
      for(int j = 0; j < D; j++ ) {
        output[i*D+j] +=  multiplier*feature[j];
      }
    }

    // compute loss
    double loss = log(sum+1); // math.logip(sum)
    if (label > 0.0) {
      loss -= marginY;
    }
    if (maxMargin > 0) {
      loss += maxMargin;
    }
    output[weight_size] += loss;
  }
}

