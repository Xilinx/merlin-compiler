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
#include"header.h"
#define uu(p, n, m) (c[(p)*16*16 + (n)*16 + (m)])

#pragma ACCEL kernel
void func_top_1(int* c) {
#pragma ACCEL interface variable=c  max_depth=4096 depth=4096
  for (int p = 1; p < 15; p++) {
    for (int n = 1; n < 15; n++) {
      for (int m = 1; m < 15; m++) {
#pragma ACCEL pipeline
#pragma ACCEL line_buffer variable=c
          int u_center = uu(p, n, m);
          int u_1      = uu(p, n, m - 1);
          int u_2      = uu(p, n, m + 1);
          int u_3      = uu(p, n - 1, m);
          int u_4      = uu(p, n + 1, m);
          int u_5      = uu(p - 1, n, m);
          int u_6      = uu(p + 1, n, m);
          printf("%d, %d, %d: %d,%d,%d,%d,%d,%d,%d\n", p, n, m, u_center, u_1, u_2, u_3, u_4, u_5, u_6);
      }
    }
  }
}

int c[4096];
int main() {

  for (int i = 0; i < 4096; i++)
  {
    c[i]=i;
  }

  func_top_1(c);
  return 0;
}
