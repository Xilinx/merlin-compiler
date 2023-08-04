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
#include<stdio.h>
#include<string.h>

#define HS 256
#define WS 64
#define FH 2
#define FW 3

#pragma ACCEL kernel
void conv1d(float in[HS][WS], float filter[FH][FW], float out[HS-FH+1][WS-FW+1]) {
    for (int i = FH-1; i< HS; i++)
        for (int j = FW-1; j< WS; j++) {
#pragma ACCEL line_buffer variable=in
            float tmp = 0.0f;
            for (int p = 0; p < FH; p++)
            for (int q = 0; q < FW; q++) {
                tmp += in[i-p][j-q] * filter[p][q];
                printf("%d, %d, %d, %d : %f\n", i, j, p, q, tmp );
            }
            out[i-FH+1][j-FW+1] = tmp;
        }
}

float in[HS][WS];
float filter[FH][FW];
float out[HS-FH+1][WS-FW+1];

int main() {
  for (int i = 0; i< HS; i++)
    for (int j = 0; j< WS; j++) {
      in[i][j]=(j+1)*(i+1);
    }
  for (int i = 0; i< FH; i++)
    for (int j = 0; j< FW; j++) {
      filter[i][j]=1;
    }
    for (int i = 0; i< HS-FH+1; i++)
        for (int j = 0; j< WS-FW+1; j++) {
          out[i][j]=0;
    }
  conv1d(in, filter, out);
  return 0;
}
