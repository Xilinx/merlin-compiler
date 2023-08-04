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


#define I 32
#define O 16
#define R 64
#define C 2
#define K 4

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



#define TS_O 4
#define OO div_ceil(O, TS_O)      

#define TS_R 4
#define RR div_ceil(R, TS_R)      

#define TS_I 1                    // PE_DATA_SIZE
#define II div_ceil(I, TS_I)      

#pragma ACCEL kernel 
void cnn_kernel(int in[R1][C1][I], int weight[O][K][K][I], int out[O][R][C])
{
  #pragma ACCEL systolic
  for (int o0 = 0; o0 < OO; o0++)
  for (int c = 0; c < C; c++) {
  
    #pragma ACCEL data_reuse scope 
    for (int i0 = 0; i0 < II; i0++)
    for (int p = 0; p < K; p++)
    for (int q = 0; q < K; q++) 
    for (int r0 = 0; r0 < RR; r0++) {
  
      #pragma ACCEL parallel scope
      for (int o1 = 0; o1 < TS_O; o1++)
      for (int r1 = 0; r1 < TS_R; r1++)
      for (int i1 = 0; i1 < TS_I; i1++)
      {
          int o = o1 * OO + o0;
          int r = r1 * RR + r0;
          int i = i0 * TS_I + i1;

          out[o][r][c] += weight[o][p][q][i] * in[r+p][c+q][i];
      }
    }
  }
}


