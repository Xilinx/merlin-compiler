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
//#include <stdlib.h>
//#include <math.h>
//#include "__merlin_math.h"
//#include <stdexcept>
//#include <string>
//#include <vector>
//#include <algorithm>
//#include <sstream>
//#include <limits>

#define TS_1 16
#define TS 16
#define CHUNK_NUM (60000/TS_1)

#pragma ACCEL kernel
void svm_kernel(int num_samples, double * data_feature, double * data_label, double * weights, double * output)
{
    int kk;
    int j0, jj, j;
    int feature_length = 784;
    int weight_length = 784; 
#pragma ACCEL pipeline 
    for (int k0 = 0; k0 < num_samples/TS_1; k0++ ) {
#pragma HLS loop_tripcount max=3750

        double dotProduct[TS]  = {0.0};

        int element_offset = feature_length + 1;

        for(j0 = 0; j0 < feature_length/TS; j0++ ) {
#pragma ACCEL pipeline flatten
            for(jj = 0; jj < TS; jj++ ) {
                for (kk = 0; kk < TS_1; kk++ ) {

                    int k = k0 * TS_1 + kk;
                    double  label = data_label[k];
                    double labelScaled = 2*label - 1.0;

                    j = j0 * TS + jj;
                    double mult=weights[j] * data_feature[k*feature_length+j];
                    dotProduct[kk] += mult;
                }}
        }

        for (kk = 0; kk < TS_1; kk++ ) {
            int k = k0 * TS_1 + kk;

            double  label = data_label[k];
            double labelScaled = 2*label - 1.0;

            if (1.0 > labelScaled * dotProduct[kk]) {
#pragma ACCEL pipeline flatten
                for(j0 = 0; j0 < feature_length/TS; j0++ ) {
                    for(jj = 0; jj < TS; jj++ ) {
                        j = j0 * TS + jj;
                        double mult= -labelScaled * data_feature[k*feature_length+j];
                        output[j] += mult;
                    }
                }
                output[weight_length] += 1.0 - labelScaled * dotProduct[kk];
            }
        }
    }
    output[weight_length+1] = (double)num_samples;
}




