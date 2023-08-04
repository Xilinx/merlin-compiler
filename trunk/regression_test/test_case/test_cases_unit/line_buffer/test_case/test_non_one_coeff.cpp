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
#define HS 256
#define WS 1024
#define FW 5
#define FH 2

#pragma ACCEL kernel
void conv1d(float in[HS][WS], float filter[FH][FW], float out[HS-FH+1][WS-FW+1]) {
    for (int i = 0; i< HS-FH+1; i++)
        for (int j = 0; j< WS-FW+1; j++) {
#pragma ACCEL line_buffer variable=in
            float tmp = 0.0f;
//#pragma ACCEL parallel
            for (int p = 0; p < FH / 2; p++)
            for (int q = 0; q < FW; q++)
                tmp += in[i+2 * p][j+q] * filter[2 * p][q]  + in [i + 2 *p + 1][j + q] * filter[ 2 *p + 1][q];
            out[i][j] = tmp;
        }
}

#pragma ACCEL kernel
void func_top_0(int *c) {
#pragma ACCEL INTERFACE VARIABLE=c DEPTH=1000
  int sum=0;
  int k;
  int i;
  int j;
  for (i = 0; i < 50; i++) {
    for (j = 0; j < 100; j++) {
#pragma ACCEL line_buffer variable=c
      sum += c[i] + c[2*i*100 + j];
    }
  }
}
