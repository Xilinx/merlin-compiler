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
void matmul_kernel(int *a, int *b, int *c_in, int *c_out) {
  int i;
  int j;
  int k;
  int c_buf[256][256];
  for (k = 0; k < 256; k++) {
    for (j = 0; j < 256; j++) {
      c_buf[k][j] = c_in[k * 256 + j];
    }
  }
  for (k = 0; k < 256; k++) {
    
#pragma ACCEL spawn
    for (i = 0; i < 256; i++) {
      for (j = 0; j < 256; j++) {
        c_buf[i][j] += a[k * 256 + i] * b[k * 256 + j];
      }
    }
  }
  for (k = 0; k < 256; k++) {
    for (j = 0; j < 256; j++) {
      c_out[k * 256 + j] = c_buf[k][j];
    }
  }
}
