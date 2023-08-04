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

#pragma ACCEL kernel
void func_top_0() {
  int a[100][100];
  int b[100][100];
  int k;
  int i;
  int j;
  for (i = 0; i < 10; i++) {
    for (k = 0; k < 99; k++) {
#pragma ACCEL parallel
      for (j = 0; j < 10; j++) {
#pragma ACCEL pipeline
        a[i][j] = b[i * k][j] + b[i * k + 1][j + 1];
      }
    }
  }
}

#pragma ACCEL kernel
void func_top_1(int out[32]) {
  int yy[32][32];
  int sum[32][32];
  int sumi[32][32];
  int beta[32];
  int r[32];
  int i, k;
  int y[32][32];

  y[0][0] = out[0];
  beta[0] = 1;
  int *alpha = out;
  for (k = 1; k < 31; k++) {
    beta[k] = beta[k - 1] - alpha[k - 1] * alpha[k - 1] * beta[k - 1];
    sum[0][k] = r[k];

    for (i = 0; i <= k - 1; i++) {
#pragma ACCEL pipeline II = 1
      sumi[i + 1][k] = sum[i][k] + y[i][k - 1];
    }

    alpha[k] = -sumi[k][k] * beta[k];
    // bound check array 'y' read_range=[-29..30 inexact]
//    for (i = 0; i <= k - 1; i += 2) {
//#pragma ACCEL pipeline
//      yy[i][k] = y[i][k - 1] + alpha[k] * y[k - i - 1][k - 1];
//      yy[i][k + 1] = y[i][k] + alpha[k + 1] * y[k - i][k];
//    }

    yy[k][k] = alpha[k];
  }
  for (i = 0; i < 32; i++) {
    out[i] = yy[i][32 - 1];
  }
}

#pragma ACCEL kernel
void func_top_2(float u_out[64][64][64]) {
  float sigma = 0.05f;
  float DT = 5.0f;
  float lambda = 0.065f;
  float EPSILON = 5e-5;
  float sigma2 = (sigma * sigma);
  float gamma1 = (lambda / sigma2);
  int p, n, m;
  float u_1, u_2, u_3, u_4, u_5, u_6, u_7;
  float g_1, g_2, g_3, g_4, g_5, g_6, g_7;
  float u[64][64][64];
  int q = 0;

  for (p = 1; p < IMG_P + 1; p++)
    for (n = 1; n < IMG_N + 1; n++)
      for (m = 1; m < IMG_M + 1; m++) {
#pragma ACCEL pipeline
        u_1 = u[p][n][m];
        u_2 = u[p][n][m - 1];
        u_3 = u[p][n][m + 1];
        u_4 = u[p][n - 1][m];
        u_5 = u[p][n + 1][m];
        u_6 = u[p - 1][n][m];
        u_7 = u[p + 1][n][m];
        u_out[p][n][m] =
            (1.0f / sqrtf(((((((EPSILON + ((u_1 - u_2) * (u_1 - u_2))) +
                               ((u_1 - u_3) * (u_1 - u_3))) +
                              ((u_1 - u_4) * (u_1 - u_4))) +
                             ((u_1 - u_5) * (u_1 - u_5))) +
                            ((u_1 - u_6) * (u_1 - u_6))) +
                           ((u_1 - u_7) * (u_1 - u_7)))));
      }
}

#pragma ACCEL kernel
void func_top_3(int CP, float c1, float c2) {
  float optVal[257];
  float optValBuf[257];
  int k;
  int i;
  int j;

  for (i = 0; i < 100; i++) {
#pragma ACCEL PIPELINE II = 26
    for (j = 0; j < 256 + 1; ++j) {
#pragma ACCEL PARALLEL COMPLETE
      optValBuf[j] = optVal[j];
    }
    for (j = 0; j <= 256 - 1; j++) {
#pragma ACCEL PARALLEL COMPLETE
      if (j <= i) {
        optVal[j] =
            ((float)(fmax((double)(c1 * optValBuf[j] + c2 * optValBuf[1 + j]),
                          (double)(((float)CP)))));
      }
    }
  }
}

static void mm_store_si15(unsigned char dst_data[16],unsigned char a_data[4])
{
  unsigned char dst_data1[16];
  dst_data1[0] = a_data[0];
  dst_data1[5] = a_data[1];
  dst_data1[10] = a_data[2];
  dst_data1[15] = a_data[3];

  dst_data[0]  = dst_data1[0] ;
  dst_data[5]  = dst_data1[5] + 1;
  dst_data[10] = dst_data1[10] + 2;
  dst_data[15] = dst_data1[15] + 3;

}
//test for MER-304
#pragma ACCEL kernel
void func_top_4() {
  unsigned char vScore5_data[10][6][16];
  unsigned char vf_n_data1[3][4];
  const int rff_ = 1035;

  for (unsigned long i = 0; i < rff_; i++) {
    int ping_pong = (int )(i % ((unsigned long )2));
    for (int k = 0; k < 10; k++) {
#pragma ACCEL PIPELINE
      for (int j = 0; j < 3; j++) {
#pragma ACCEL PARALLEL COMPLETE
        mm_store_si15(vScore5_data[k][3 * (1 - ping_pong) + j],vf_n_data1[j]);
      }
    }
  }
}
int main() {
  float u_out[64][64][64];
  int out[32];
  func_top_0();
  func_top_1(out);
  func_top_2(u_out);
}
