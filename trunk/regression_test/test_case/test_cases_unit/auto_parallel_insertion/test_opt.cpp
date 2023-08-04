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
void func_top_18(int a[64][64], int b[64][64], int c[64][64]) {
  for (int i1 = 0; i1 < 32; i1++) {
    for (int i2 = 0; i2 < 32; i2++) {
#pragma ACCEL parallel reduction = c
      for (int i3 = 0; i3 < 32; i3++) {
        c[i1][i2] += a[i2][i3] * b[i2][i3];
      }
    }
  }
}


#pragma ACCEL kernel
void func_top_19(int a[64][64], int b[64][64], int c[64][64]) {
  for (int i1 = 0; i1 < 32; i1++) {
#pragma ACCEL tile factor=2
    for (int i2 = 0; i2 < 32; i2++) {
      for (int i3 = 0; i3 < 100; i3++) {
        c[i1][i2] += a[i2][i3] * b[i2][i3];
      }
    }
  }
}

#pragma ACCEL kernel
void func_top_20(int a[64][64], int b[64][64], int c[64][64]) {
  for (int i0 = 0; i0 < 32; i0++) {
    for (int i1 = 0; i1 < 32; i1++) {
#pragma ACCEL line_buffer variable=c
      for (int i2 = 0; i2 < 32; i2++) {
        for (int i3 = 0; i3 < 32; i3++) {
          c[i1][i2] += a[i2][i3] * b[i2][i3];
        }
      }
    }
  }
}


