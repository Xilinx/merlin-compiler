// (C) Copyright 2016-2021 Xilinx, Inc.
// All Rights Reserved.
//
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.




#define I 32
#define O 32
#define R 64
#define C 64
#define K 4

#define R1 67//(R+K-1)  // input dim with padding
#define C1 67//(C+K-1)


#pragma ACCEL kernel 
void cnn_kernel(int in[I][R1][C1], int w[I][O][K][K], int out[O][R][C])
{
    #pragma ACCEL systolic auto

    for (int o = 0; o < O; o++)
    for (int i = 0; i < I; i++)
    for (int r = 0; r < R; r++)
    for (int c = 0; c < C; c++)
    for (int p = 0; p < K; p++)
    for (int q = 0; q < K; q++) {
				out[o][r][c] += w[o][p][q][i] * in[r+p][c+q][i];
    }
}

