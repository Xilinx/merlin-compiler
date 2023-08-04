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
#include <math.h>
#define IMG_P 62 
#define IMG_N 62 
#define IMG_M 62 

void initial(float u[64 * 64 * 64 + 64 * 64 + 64])
{
  int p;
  int n;
  int m;
  for (p = 0; p < 62 + 2; p++) {
    for (n = 0; n < 62 + 2; n++) {
      for (m = 0; m < 62 + 2; m++) {
        u[p * 64 * 64 + n * 64 + m] = (p + n + m);
      }
    }
  }
}

void calculate(float u[64 * 64 * 64 + 64 * 64 + 64],float g[64 * 64 * 64 + 64 * 64 + 64],float f[64 * 64 * 64 + 64 * 64 + 64],float u_out[64 * 64 * 64 + 64 * 64 + 64])
{
  float sigma = 0.05f;
  float DT = 5.0f;
  float lambda = 0.065f;
  float EPSILON = 5e-5;
  float sigma2 = sigma * sigma;
  float gamma1 = lambda / sigma2;
  int p;
  int n;
  int m;
  float u_1;
  float u_2;
  float u_3;
  float u_4;
  float u_5;
  float u_6;
  float u_7;
  float r;
  float g_1;
  float g_2;
  float g_3;
  float g_4;
  float g_5;
  float g_6;
  float g_7;
  register short lbv;
  register short ubv;
  register short lb;
  register short ub;
  register short lb1;
  register short ub1;
  register short lb2;
  register short ub2;
  register short c1;
  register short c2;
  register short c3;
  register short c4;
  register short c5;
  register short c6;
  for (p = 1; p < 62 + 1; p++) {
    for (n = 1; n < 62 + 1; n++) {
      for (m = 1; m < 62 + 1; m++) {
        
#pragma ACCEL pipeline_parallel factor=4
        u_1 = u[p * 64 * 64 + n * 64 + m];
        u_2 = u[p * 64 * 64 + n * 64 + m - 1];
        u_3 = u[p * 64 * 64 + n * 64 + m + 1];
        u_4 = u[p * 64 * 64 + (n - 1) * 64 + m];
        u_5 = u[p * 64 * 64 + (n + 1) * 64 + m];
        u_6 = u[(p - 1) * 64 * 64 + n * 64 + m];
        u_7 = u[(p + 1) * 64 * 64 + n * 64 + m];
        g_2 = g[p * 64 * 64 + n * 64 + m - 1];
        g_3 = g[p * 64 * 64 + n * 64 + m + 1];
        g_4 = g[p * 64 * 64 + (n - 1) * 64 + m];
        g_5 = g[p * 64 * 64 + (n + 1) * 64 + m];
        g_6 = g[(p - 1) * 64 * 64 + n * 64 + m];
        g_7 = g[(p + 1) * 64 * 64 + n * 64 + m];
        r = u_1 * f[p * 64 * 64 + n * 64 + m] / sigma2;
        r = r * (2.38944f + r * (0.950037f + r)) / (4.65314f + r * (2.57541f + r * (1.48937f + r)));
        u_out[p * 64 * 64 + n * 64 + m] = (u_1 + DT * (u_2 * g_2 + u_3 * g_3 + u_4 * g_4 + u_5 * g_5 + u_6 * g_6 + u_7 * g_7 + gamma1 * f[p * 64 * 64 + n * 64 + m] * r)) / (1.0f + DT * (g_2 + g_3 + g_4 + g_5 + g_6 + g_7 + gamma1));
      }
    }
  }
}
/*float f[(64) * 64 * 64  + (64) * 64 + 64],*/
/*,int IMG_P,int IMG_n,int IMG_M*/
#pragma ACCEL kernel
void kernel_denoise(float u_out[64 * 64 * 64 + 64 * 64 + 64])
{
/* computation */
  float sigma = 0.05f;
  float DT = 5.0f;
  float lambda = 0.065f;
  float EPSILON = 5e-5;
  float sigma2 = sigma * sigma;
  float gamma1 = lambda / sigma2;
  int p;
  int n;
  int m;
//float u[(p) * 64 * 64  + (n) * 64 + m];
  float u_1;
  float u_2;
  float u_3;
  float u_4;
  float u_5;
  float u_6;
  float u_7;
  float r;
  float g_1;
  float g_2;
  float g_3;
  float g_4;
  float g_5;
  float g_6;
  float g_7;
  float u[266240 + 64];
  float g[266240 + 64];
  float f[266240 + 64];
  int q = 0;
  initial(u);
  for (p = 1; p < 62 + 1; p++) {
    for (n = 1; n < 62 + 1; n++) {
      for (m = 1; m < 62 + 1; m++) {
//#pragma HLS pipeline II = 1
//#pragma HLS unroll factor=9
//#pragma HLS array_partition variable=u cyclic factor=3 dim=1
//#pragma HLS array_partition variable=u cyclic factor=3 dim=2
//#pragma HLS array_partition variable=u cyclic factor=3 dim=3
        
#pragma ACCEL pipeline
        u_1 = u[p * 64 * 64 + n * 64 + m];
        u_2 = u[p * 64 * 64 + n * 64 + m - 1];
        u_3 = u[p * 64 * 64 + n * 64 + m + 1];
        u_4 = u[p * 64 * 64 + (n - 1) * 64 + m];
        u_5 = u[p * 64 * 64 + (n + 1) * 64 + m];
        u_6 = u[(p - 1) * 64 * 64 + n * 64 + m];
        u_7 = u[(p + 1) * 64 * 64 + n * 64 + m];
        g[p * 64 * 64 + n * 64 + m] = 1.0f / sqrtf(EPSILON + (u_1 - u_2) * (u_1 - u_2) + (u_1 - u_3) * (u_1 - u_3) + (u_1 - u_4) * (u_1 - u_4) + (u_1 - u_5) * (u_1 - u_5) + (u_1 - u_6) * (u_1 - u_6) + (u_1 - u_7) * (u_1 - u_7));
      }
    }
  }
  calculate(u,g,f,u_out);
}

int main()
{
  float u_out[274624 + 2];
//kernel_denoise(u, g, f);
  int i;
  int j;
  int k;
  kernel_denoise(u_out);
  for (i = 1; i < 64; i++) {
    for (j = 1; j < 64; j++) {
      for (k = 1; k < 64; k++) {
        printf("Output is %f \n",u_out[i * 64 * 64 + j * 64 + k]);
      }
    }
  }
  return 0;
}
