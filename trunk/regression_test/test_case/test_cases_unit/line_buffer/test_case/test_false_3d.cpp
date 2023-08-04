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
#define c(p, n, m) (c[(p)*32*32 + (n)*32 + (m)])



#pragma ACCEL kernel
void func_top_3(int c[32][32][32]) {
#pragma ACCEL interface variable=c  max_depth=32,32,32 depth=32,32,32
  int sum;
  for (int i = 0; i < 32; i++) {
    for (int j = 0; j < 31; j++) {
      for (int k = 2; k < 30; k++) {
#pragma ACCEL pipeline
#pragma ACCEL line_buffer variable=c
        sum += c[i+1][j][k - 1] + c[i+1][j][k+1];
      }
    }
  }
}


