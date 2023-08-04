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
#define c(n, m) (c[(n)*100 + (m)])
//#define b(n, m) (b[(n)*100 + (m)])
#define bb(n, m) (bb[(n)*100 + (m)])
#pragma ACCEL kernel
void func_top_0(int b[100][100], int *c, int bb[10000]) {
#pragma ACCEL INTERFACE VARIABLE=c DEPTH=10000
#pragma ACCEL INTERFACE VARIABLE=b DEPTH=100,100
#pragma ACCEL INTERFACE VARIABLE=bb DEPTH=20000
  int sum=0;
    for (int j = 0; j < 31; j++) {
      for (int k = 0; k < 31; k++) {
#pragma ACCEL line_buffer variable=c
        sum += c(j,k) + c(j+1,k+1);
      }
    }

    for (int j = 0; j < 99; j++) {
      for (int k = 0; k < 99; k++) {
#pragma ACCEL line_buffer variable=b
        sum += b[j][k] + b[j+1][k+1];
      }
    }

    for (int j = 0; j < 99; j++) {
      for (int k = 0; k < 99; k++) {
#pragma ACCEL line_buffer variable=bb
        sum += bb(j,k) + bb(j+1,k+1);
      }
    }

}

