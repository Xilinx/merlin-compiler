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


//#define I 128
//#define O 80
//#define R 64
//#define C 4
//#define K 3

#define O 10
#define R 16
#define I 8

#define BS_O 8
#define BS_R 4
#define BS_I 8
#define BS_C 4
#define K 3

#define BNUM_O 1
#define BNUM_C 1
#define BNUM_I 2

#define IMG_IN_NUM (I * BS_I * BNUM_I)
#define IMG_OUT_NUM (O * BS_O * BNUM_O)
#define IMG_ROW (R * BS_R)
#define IMG_COL (BS_C * BNUM_C)
#define KERNEL K

//#define R1 (R + K - 1)  // input dim with padding
//#define C1 (C + K - 1)

#define IMG_IN_ROW (IMG_ROW + KERNEL - 1)  // input dim with padding
#define IMG_IN_COL (IMG_COL + KERNEL - 1)


#pragma ACCEL kernel 
void cnn_kernel(int in[IMG_IN_ROW][IMG_IN_COL][IMG_IN_NUM], int weight[IMG_OUT_NUM][K][K][IMG_IN_NUM], int out[IMG_OUT_NUM][IMG_ROW][IMG_COL])
{
  #pragma ACCEL systolic
  for (int o0 = 0; o0 < BNUM_O; o0++) // BNUM_O=1
  for (int c0 = 0; c0 < BNUM_C; c0++) // BNUM_C=1
  for (int i0 = 0; i0 < BNUM_I; i0++) { // BNUM_I=2
  
    #pragma ACCEL data_reuse scope 
    for (int i1 = 0; i1 < BS_I; i1++) // BS_I=8
    for (int p = 0; p < K; p++) // K=3
    for (int q = 0; q < K; q++) // K=3
    for (int o1 = 0; o1 < BS_O; o1++) // BS_O=8
    for (int c1 = 0; c1 < BS_C; c1++) // BS_C=4
    for (int r1 = 0; r1 < BS_R; r1++) { // BS_R=4
  
      #pragma ACCEL parallel scope
      for (int o2 = 0; o2 < O; o2++) // O=10
      for (int r2 = 0; r2 < R; r2++) // R=16
      for (int i2 = 0; i2 < I; i2++) // I=8
      {
          int o = o0 * BS_O * O + o2 * BS_O + o1;
          int r = r2 * BS_R + r1;
          int i = i0 * BS_I * I + i2 * BS_I + i1;
		  int c = c0 * BS_C + c1;

          out[o][r][c] += weight[o][p][q][i] * in[r+p][c+q][i];
      }
    }
  }
}


