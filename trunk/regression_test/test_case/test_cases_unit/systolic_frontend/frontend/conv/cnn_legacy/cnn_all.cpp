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
#include <iostream>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include "merlin_cnn.h"

//#define LAYER_NUM 13
#define LAYER_NUM 1

// VGG
//const unsigned int in_num[] = {16, 64, 64, 128, 128, 256, 256, 256, 512, 512, 512, 512, 512};
//const unsigned int out_num[] = {64, 64, 128, 128, 256, 256, 256, 512, 512, 512, 512, 512, 512};
//const unsigned int img_row[] = {224, 224, 112, 112, 56, 56, 56, 28, 28, 28, 14, 14, 14};
//const unsigned int img_col[] = {224, 224, 112, 112, 56, 56, 56, 28, 28, 28, 14, 14, 14};
//const unsigned int kernel[] = {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3};
//const unsigned int scale[] = {0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0};
//const unsigned int pooling[] = {0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0};

// AlexNet
const unsigned int in_num[] = {3, 48, 256, 192, 192};
const unsigned int out_num[] = {48, 128, 192, 192, 128};
const unsigned int img_row[] = {55, 27, 13, 13, 13};
const unsigned int img_col[] = {55, 27, 13, 13, 13};
const unsigned int kernel[] = {11, 5, 3, 3, 3};
const unsigned int scale[] = {1, 1, 1, 1, 1};
const unsigned int pooling[] = {1, 1, 0, 0, 0};


#pragma ACCEL kernel 
void cnn_kernel(float ***out, float ***in, float *****w)
{
	for (int l = 0; l < LAYER_NUM; l++) {
		if (scale[l]) {
			merlin_cnn_scale(in, 1.27);
		}

   // #pragma ACCEL systolic auto
    #pragma ACCEL systolic auto=auto
    for (int o = 0; o < out_num[l]; o++)
    for (int i = 0; i < in_num[l]; i++)
    for (int r = 0; r < img_row[l]; r++)
    for (int c = 0; c < img_col[l]; c++)
    for (int p = 0; p < kernel[l]; p++)
    for (int q = 0; q < kernel[l]; q++) {
				out[o][r][c] += w[l][o][p][q][i] * in[i][r+p][c+q];
    }

		if (pooling[l]) {
			merlin_cnn_pooling(out, 2, AVERAGE);
		}
	}

}

