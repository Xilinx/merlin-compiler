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

#include "header.h"

void denoise_0_kernel(float *u,float *g,float *f,float *u_out);
void denoise_1_kernel(float *u,float *g,float *f,float *u_out);

void denoise(float *u,float *g,float *f,float *u_out)
{
/* computation */
  float sigma = 0.05f;
  float DT = 5.0f;
  float lambda = 0.065f;
  float EPSILON = 5e-5;
  float sigma2 = (sigma * sigma);
  float gamma1 = (lambda / sigma2);
  

/* denoise */
#define MAXITER 1
{
    int i;
    for (i = 0; i < 1; i++) 
    {
	denoise_0_kernel(u, g, f ,u_out);
	denoise_1_kernel(u, g, f ,u_out);
    }
}
}
