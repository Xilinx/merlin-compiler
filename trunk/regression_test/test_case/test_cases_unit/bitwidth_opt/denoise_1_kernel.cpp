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
#include "math.h"
#define uu(p, n, m) (u[(p)*DEFINE_N_PITCH*DEFINE_M_PITCH + (n)*DEFINE_M_PITCH + (m)])
#define gg(p, n, m) (g[(p)*DEFINE_N_PITCH*DEFINE_M_PITCH + (n)*DEFINE_M_PITCH + (m)])
#define ff(p, n, m) (f[(p)*DEFINE_N_PITCH*DEFINE_M_PITCH + (n)*DEFINE_M_PITCH + (m)])
#define uu_out(p, n, m) (u_out[(p)*DEFINE_N_PITCH*DEFINE_M_PITCH + (n)*DEFINE_M_PITCH + (m)])

#pragma ACCEL kernel
void denoise_1_kernel(float *u,float *g,float *f,float *u_out)
{
  float u_out_0_0_buf[512];
  float u_0_6_buf[512];
  float u_0_5_buf[512];
  float u_0_4_buf[512];
  float u_0_3_buf[512];
  float u_0_2_buf[512];
  float u_0_1_buf[512];
  float u_0_0_buf[512];
  float g_0_5_buf[512];
  float g_0_4_buf[512];
  float g_0_3_buf[512];
  float g_0_2_buf[512];
  float g_0_1_buf[512];
  float g_0_0_buf[512];
  float f_0_0_buf[512];
  float sigma = 0.05f;
  float DT = 5.0f;
  float lambda = 0.065f;
  float EPSILON = 5e-5;
  float sigma2 = sigma * sigma;
  float gamma1 = lambda / sigma2;
  int p;
  int n;
  int m;
  for (p = 1; p < 8 + 1; p++) {
    for (n = 1; n < 512 + 1; n++) {
      memcpy(&f_0_0_buf[0],&f[264196 * p + 514 * n + 1],sizeof(float ) * 512);
      memcpy(&g_0_0_buf[0],&g[264196 * p + 514 * n - 1 + 1],sizeof(float ) * 512);
      memcpy(&g_0_1_buf[0],&g[264196 * p + 514 * n + 1 + 1],sizeof(float ) * 512);
      memcpy(&g_0_2_buf[0],&g[264196 * p + 514 * n - 514 + 1],sizeof(float ) * 512);
      memcpy(&g_0_3_buf[0],&g[264196 * p + 514 * n + 514 + 1],sizeof(float ) * 512);
      memcpy(&g_0_4_buf[0],&g[264196 * p + 514 * n - 264196 + 1],sizeof(float ) * 512);
      memcpy(&g_0_5_buf[0],&g[264196 * p + 514 * n + 264196 + 1],sizeof(float ) * 512);
      memcpy(&u_0_0_buf[0],&u[264196 * p + 514 * n + 1],sizeof(float ) * 512);
      memcpy(&u_0_1_buf[0],&u[264196 * p + 514 * n - 1 + 1],sizeof(float ) * 512);
      memcpy(&u_0_2_buf[0],&u[264196 * p + 514 * n + 1 + 1],sizeof(float ) * 512);
      memcpy(&u_0_3_buf[0],&u[264196 * p + 514 * n - 514 + 1],sizeof(float ) * 512);
      memcpy(&u_0_4_buf[0],&u[264196 * p + 514 * n + 514 + 1],sizeof(float ) * 512);
      memcpy(&u_0_5_buf[0],&u[264196 * p + 514 * n - 264196 + 1],sizeof(float ) * 512);
      memcpy(&u_0_6_buf[0],&u[264196 * p + 514 * n + 264196 + 1],sizeof(float ) * 512);
      for (m = 1; m < 512 + 1; m++) {
        
#pragma ACCEL pipeline
        float u_center = u_0_0_buf[m - 1];
        float f_center = f_0_0_buf[m - 1];
        float r;
        r = u_center * f_center / sigma2;
        r = r * (2.38944f + r * (0.950037f + r)) / (4.65314f + r * (2.57541f + r * (1.48937f + r)));
        float u_1 = u_0_1_buf[m - 1];
        float u_2 = u_0_2_buf[m - 1];
        float u_3 = u_0_3_buf[m - 1];
        float u_4 = u_0_4_buf[m - 1];
        float u_5 = u_0_5_buf[m - 1];
        float u_6 = u_0_6_buf[m - 1];
        float g_1 = g_0_0_buf[m - 1];
        float g_2 = g_0_1_buf[m - 1];
        float g_3 = g_0_2_buf[m - 1];
        float g_4 = g_0_3_buf[m - 1];
        float g_5 = g_0_4_buf[m - 1];
        float g_6 = g_0_5_buf[m - 1];
        u_out_0_0_buf[m - 1] = (u_center + DT * (u_1 * g_1 + u_2 * g_2 + u_3 * g_3 + u_4 * g_4 + u_5 * g_5 + u_6 * g_6 + gamma1 * f_center * r)) / (1.0f + DT * (g_1 + g_2 + g_3 + g_4 + g_5 + g_6 + gamma1));
      }
      memcpy(&u_out[264196 * p + 514 * n + 1],&u_out_0_0_buf[0],sizeof(float ) * 512);
    }
  }
}
