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

#pragma ACCEL kernel
void func_top_0(int b[100][100], int c[100][100]) {
#pragma ACCEL INTERFACE VARIABLE=b DEPTH=100,100
#pragma ACCEL INTERFACE VARIABLE=c DEPTH=100,100
  int i;
  int j;
  int sum[100][100];
  for (i = 1; i < 99; i++) {
    for (j = 1; j < 95; j++) {
#pragma ACCEL line_buffer variable=b
#pragma ACCEL line_buffer variable=c
      for (int h = 0; h < 5; h++) {
        sum[i][j] = b[i][j+h] + c[j][j+h];
      }
    }
  }
}
