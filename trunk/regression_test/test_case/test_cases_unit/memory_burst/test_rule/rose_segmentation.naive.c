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
#define epsilon 10e-5

void initial(float phi[64 + 2][64 + 2][64 + 2])
{
  int i;
  int j;
  int k;
  for (i = 0; i < 64 + 2; i++) {
    for (j = 0; j < 64 + 2; j++) {
      for (k = 0; k < 64 + 2; k++) {
        
#pragma ACCEL pipeline
        phi[i][j][k] = (i + j + k);
      }
    }
  }
}

void finalloop(float phi[64 + 2][64 + 2][64 + 2],float curvature_motion_part_g[64 + 2][64 + 2][64 + 2],float dt)
{
  int i;
  int j;
  int k;
  int ti;
  int tj;
  int tk;
  int pi;
  int pj;
  int pk;
  for (ti = 0; ti < 2; ti++) {
    for (tj = 0; tj < 2; tj++) {
      for (tk = 0; tk < 2; tk++) {
        for (pi = 0; pi < 32; pi++) {
          for (pj = 0; pj < 32; pj++) {
            for (pk = 0; pk < 32; pk++) {
//#pragma HLS pipeline
              i = ti * 32 + pi;
              j = tj * 32 + pj;
              k = tk * 32 + pk;
              
#pragma ACCEL pipeline
              phi[i][j][k] += curvature_motion_part_g[i][j][k] * dt;
            }
          }
        }
      }
    }
  }
}

void kernel_segmentation(float interpt[64 + 2][64 + 2][64 + 2],float curvature_motion_part_g[64 + 2][64 + 2][64 + 2])
{
  float lambda1 = 1;
  float lambda2 = 1;
  float mu = 0.18f * 255.0f * 255.0f;
  float dt = 0.15f / mu;
  float c1 = 1;
  float c2 = 1;
  int i;
  int j;
  int k;
  int ti;
  int tj;
  int tk;
  int pi;
  int pj;
  int pk;
  float Grad;
  float K;
  float Dx_p;
  float Dx_m;
  float Dy_p;
  float Dy_m;
  float Dz_p;
  float Dz_m;
  float Dx_0;
  float Dy_0;
  float Dz_0;
  float r0;
  float r1;
  float r2;
  float r3;
  float r4;
  float r5;
  float r6;
  float r7;
  float r8;
  float r9;
  float r10;
  float r11;
  float r12;
  float r13;
  float r14;
  float r15;
  float r16;
  float r17;
  float r18;
  float Dxx;
  float Dyy;
  float Dzz;
  float Dxy;
  float Dxz;
  float Dyz;
  float phi[64 + 2][64 + 2][64 + 2];
  initial(phi);
  
#pragma scop
  for (ti = 0; ti < 2; ti++) 
    for (tj = 0; tj < 2; tj++) 
      for (tk = 0; tk < 2; tk++) 
        for (pi = 0; pi < 32; pi++) {
          for (pj = 0; pj < 32; pj++) {
            for (pk = 0; pk < 32; pk++) {
              i = ti * 32 + pi;
              j = tj * 32 + pj;
              k = tk * 32 + pk;
              
#pragma ACCEL pipeline
//#pragma ACCEL pipeline
//#pragma HLS pipeline
//#pragma HLS array_partition variable=phi cyclic factor=3 dim=1
//#pragma HLS array_partition variable=phi cyclic factor=3 dim=2
//#pragma HLS array_partition variable=phi cyclic factor=3 dim=3
              r0 = phi[i][j][k];
              r1 = phi[i - 1][j][k];
              r2 = phi[i + 1][j][k];
              r3 = phi[i][j - 1][k];
              r4 = phi[i][j + 1][k];
              r5 = phi[i][j][k - 1];
              r6 = phi[i][j][k + 1];
              r7 = phi[i + 1][j][k + 1];
              r8 = phi[i - 1][j][k + 1];
              r9 = phi[i - 1][j][k - 1];
              r10 = phi[i + 1][j][k - 1];
              r11 = phi[i - 1][j - 1][k];
              r12 = phi[i + 1][j - 1][k];
              r13 = phi[i + 1][j + 1][k];
              r14 = phi[i - 1][j + 1][k];
              r15 = phi[i][j + 1][k + 1];
              r16 = phi[i][j + 1][k - 1];
              r17 = phi[i][j - 1][k - 1];
              r18 = phi[i][j - 1][k + 1];
              Dx_p = r2 - r0;
              Dx_m = r0 - r1;
              Dy_p = r4 - r0;
              Dy_m = r0 - r3;
              Dz_p = r6 - r0;
              Dz_m = r0 - r5;
              Dx_0 = (r2 - r1) / 2;
              Dy_0 = (r4 - r3) / 2;
              Dz_0 = (r6 - r5) / 2;
              Dxx = Dx_p - Dx_m;
              Dyy = Dy_p - Dy_m;
              Dzz = Dz_p - Dz_m;
              Dxy = (r13 - r12 - r14 + r11) / 4.0f;
              Dxz = (r7 - r10 - r8 + r9) / 4.0f;
              Dyz = (r15 - r16 - r18 + r17) / 4.0f;
              Grad = Dx_0 * Dx_0 + Dy_0 * Dy_0 + Dz_0 * Dz_0;
              K = ((Dx_0 * Dx_0 * Dyy - 2 * Dx_0 * Dy_0 * Dxy + Dy_0 * Dy_0 * Dxx + Dx_0 * Dx_0 * Dzz - 2 * Dx_0 * Dz_0 * Dxz + Dz_0 * Dz_0 * Dxx + Dy_0 * Dy_0 * Dzz - 2 * Dy_0 * Dz_0 * Dyz + Dz_0 * Dz_0 * Dyy) / ((Grad * Grad * Grad) + 10e-5));
              curvature_motion_part_g[i][j][k] = Grad * (mu * K + lambda1 * (interpt[i][j][k] - c1) * (interpt[i][j][k] - c1) - lambda2 * (interpt[i][j][k] - c2) * (interpt[i][j][k] - c2));
            }
          }
        }
  finalloop(phi,curvature_motion_part_g,dt);
  
#pragma endscop
}

int main()
{
  float interpt[64 + 2][64 + 2][64 + 2];
  float curvature_motion_part_g[64 + 2][64 + 2][64 + 2];
//kernel_denoise(u, g, f);
  int i;
  int j;
  int k;
  for (i = 1; i < 64; i++) {
    for (j = 1; j < 64; j++) {
      for (k = 1; k < 64; k++) {
        interpt[i][j][k] = (i + j + k);
      }
    }
  }
  kernel_segmentation(interpt,curvature_motion_part_g);
  for (i = 1; i < 64; i++) {
    for (j = 1; j < 64; j++) {
      for (k = 1; k < 64; k++) {
        printf("Output is %f \n",curvature_motion_part_g[i][j][k]);
      }
    }
  }
  return 0;
}
