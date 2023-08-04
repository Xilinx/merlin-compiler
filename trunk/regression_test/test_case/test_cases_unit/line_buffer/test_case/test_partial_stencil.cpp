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

#define N 5
#define n 100

#pragma ACCEL kernel
void func_top_0(int b[5][100][100], int *c, int addr, int bb[100][100]) {
  int tmp;
  int k;
  int i;
  int j;
  for (i = 0; i < 99; i++) {
    for (j = 0; j < 99; j++) {
#pragma ACCEL line_buffer variable=b
      tmp += b[addr][i][j] + b[addr][i + 1][j + 1];
    }
  }

  for (j = 0; j < n; j++) {
    for (i = 0; i < 1; i++) {
      for (int h = 0; h < 99; h++) {
#pragma ACCEL line_buffer variable=bb
        tmp += bb[j*addr][h+1] + bb[i*j][h];
      }
    }
  }
}
