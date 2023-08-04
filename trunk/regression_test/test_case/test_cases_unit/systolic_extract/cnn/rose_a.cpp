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
#define I 16
#define O 16
#define R 224
#define C 224
#define K 3
#define R1 (R+K-1)  // input dim with padding
#define C1 (C+K-1)
//#pragma ACCEL kernel 
//void cnn_kernel(int in[I][R1][C1], int b[I][O][K][K], int c[O][R][C])
//{
//    #pragma ACCEL systolic_array auto
//    for (int o = 0; o < O; o++)
//    for (int i = 0; i < I; i++)
//    for (int r = 0; r < R; r++)
//    for (int c = 0; c < C; c++)
//    for (int p = 0; p < K; p++)
//    for (int q = 0; q < K; q++) {
//        out[o][r][c] += in[i][r][c] * weight[i][o][p][q];
//    }
//}
#define div_ceil(x,y) (((x) + (y)-1) / (y))
#define TS_O 10                   // PE_ROWS
#define OO div_ceil(O, TS_O)      
#define TS_R 16                   // PE_COLS
#define RR div_ceil(R, TS_R)      
#define TS_I 8                    // PE_DATA_SIZE
#define II div_ceil(I, TS_I)      
#pragma ACCEL kernel

void cnn_kernel(int in[16][224 + 3 - 1][224 + 3 - 1],int weight[16][16][3][3],int out[16][224][224])
{
  
#pragma ACCEL systolic
  for (int o0 = 0; o0 < (16 + 10 - 1) / 10; o0++) 
    for (int c = 0; c < 224; c++) {
      
#pragma ACCEL data_reuse scope
      for (int i0 = 0; i0 < (16 + 8 - 1) / 8; i0++) 
        for (int p = 0; p < 3; p++) 
          for (int q = 0; q < 3; q++) 
            for (int r0 = 0; r0 < (224 + 16 - 1) / 16; r0++) {
              
#pragma ACCEL parallel scope
              for (int o1 = 0; o1 < 10; o1++) 
                for (int r1 = 0; r1 < 16; r1++) 
                  for (int i1 = 0; i1 < 8; i1++) {
                    int i = i0 * 8 + i1;
                    int o = o0 * 10 + o1;
                    int r = r0 * 16 + r1;
                    out[o][r][c] += weight[i][o][p][q] * in[i][r + p][c + q];
                  }
            }
    }
}
