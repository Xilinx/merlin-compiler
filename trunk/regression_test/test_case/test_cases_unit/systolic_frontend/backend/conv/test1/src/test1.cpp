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


#include "test1.h"

#pragma ACCEL kernel 
void cnn_kernel(int ***in, int ****weight, int ***out)
{
  #pragma ACCEL systolic
  for (int r0 = 0; r0 < BN_R; r0++) // BN_R=12
  for (int c0 = 0; c0 < BN_C; c0++) // BN_C=16
  for (int o0 = 0; o0 < BN_O; o0++) // BN_O=2
  for (int i0 = 0; i0 < BN_I; i0++) { // BN_I=1
  
    #pragma ACCEL data_reuse scope 
    for (int p = 0; p < K; p++) // K=3
    for (int q = 0; q < K; q++) // K=3
    for (int i1 = 0; i1 < BS_I; i1++) // BS_I=4
    for (int r1 = 0; r1 < BS_R; r1++) // BS_R=4
    for (int c1 = 0; c1 < BS_C; c1++) // BS_C=14
    for (int o1 = 0; o1 < BS_O; o1++) // BS_O=1
  
      #pragma ACCEL parallel scope
      for (int r2 = 0; r2 < R; r2++) // R=5
      for (int o2 = 0; o2 < O; o2++) // O=32
      for (int i2 = 0; i2 < I; i2++) // I=8
      {
          int r = r0 * R * BS_R + r2 * BS_R + r1;
		  int c = c0 * BS_C + c1;
          int o = o0 * O * BS_O + o2 * BS_O + o1;
          int i = i0 * BS_I * I + i2 * BS_I + i1;

          out[r][c][o] += in[r+p][c+q][i] * weight[o][p][q][i];
		//  out[(r * (BN_C * BS_C) + c) * (BN_O * BS_O * O) + o] += 
		//	  in[((r+p) * (BN_C * BS_C + K) + (c+q)) * (BN_I * BS_I * I) + i] * 
		//	  weight[((o * K + p) * K + q) * (BN_I * BS_I * I) + i];
      }
  }
}
