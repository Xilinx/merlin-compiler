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
#define NUM 128
#define NUM1 256
#define IMROW 64
#define IMCOL 64
#define INIMROW 66
#define INIMCOL 66
#define KERNEL 3


#pragma ACCEL kernel
void cnn_kernel1(float Cout1[IMROW][IMCOL][NUM1], 
        float Cin[INIMROW][INIMCOL][NUM], 
        float weight[KERNEL][KERNEL][NUM1][NUM]) {
  float Cout[IMROW][IMCOL][NUM1]; 
  for (int c = 0; c < IMCOL; c++) {
    for (int r = 0; r < IMROW; r++) {
      for (int p = 0; p < KERNEL; p++) { 
        for (int o = 0; o < NUM1; o++) {
          for (int q = 0; q < KERNEL; q++)  {
            for (int ii = 0; ii < NUM/32; ii++)
            {
#pragma ACCEL pipeline
#pragma ACCEL reduction variable=Cout SCHEME=auto
              for (int i_sub = 0; i_sub < 32; i_sub++) {
#pragma ACCEL parallel
                int i = ii*32 + i_sub;
                float one_sub= weight[p][q][o][i] * Cin[r+p][c+q][i];

                Cout[r][c][o] += one_sub;
              }
            }
          }}}}}

}


#pragma ACCEL kernel
void cnn_kernel2(float Cout1[IMROW][IMCOL][NUM1], 
        float Cin[INIMROW][INIMCOL][NUM], 
        float weight[KERNEL][KERNEL][NUM1][NUM]) {
  float Cout[IMROW][IMCOL][NUM1];
  for (int r = 0; r < IMROW; r++) {
    for (int c = 0; c < IMCOL; c++) {
      for (int p = 0; p < KERNEL; p++) { 
        for (int q = 0; q < KERNEL; q++)  {
            for (int ii = 0; ii < 8; ii++) {
          for (int oo = 0; oo < 8; oo++) {
#pragma ACCEL pipeline
#pragma ACCEL reduction variable=Cout
              for (int o_sub = 0; o_sub < 32; ++o_sub) {
#pragma ACCEL parallel
                for (int i_sub = 0; i_sub < 16; i_sub++)
                {
#pragma ACCEL parallel
                  int o = oo*32+o_sub;
                  int i = ii*16+i_sub;
                  float one_sub= weight[p][q][o][i] * Cin[r+p][c+q][i];

                  Cout[r][c][o] += one_sub;
                }
              }}}}}}}

}

#pragma ACCEL kernel
void cnn_kernel3(float Cout1[IMROW][IMCOL][NUM1], 
        float Cin[INIMROW][INIMCOL][NUM], 
        float weight[KERNEL][KERNEL][NUM1][NUM]) {
  float Cout[IMROW][IMCOL][NUM1];
  for (int r = 0; r < IMROW; r++) {
    for (int c = 0; c < IMCOL; c++) {
      for (int p = 0; p < KERNEL; p++) { 
        for (int q = 0; q < KERNEL; q++)  {
            for (int ii = 0; ii < 8; ii++) {
          for (int oo = 0; oo < 8; oo++) {
#pragma ACCEL pipeline
#pragma ACCEL reduction variable=Cout SCHEME=block
              for (int o_sub = 0; o_sub < 32; ++o_sub) {
#pragma ACCEL parallel
                for (int i_sub = 0; i_sub < 16; i_sub++)
                {
#pragma ACCEL parallel
                  int o = oo*32+o_sub;
                  int i = ii*16+i_sub;
                  float one_sub= weight[p][q][o][i] * Cin[r+p][c+q][i];

                  Cout[r][c][o] += one_sub;
                }
              }}}}}}}

}
