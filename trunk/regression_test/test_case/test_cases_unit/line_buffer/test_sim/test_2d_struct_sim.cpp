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

struct RGB {float r;float g;float b;};

#pragma ACCEL kernel
void func_top_1(const struct RGB inFrame[100][100]) {
//#pragma ACCEL interface variable=c  max_depth=1024 depth=1024

  int h=100;
  int w=100;
  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++)  {
#pragma ACCEL LINE_BUFFER VARIABLE=inFrame
      for (int m = 0; m < 19; m++) {
#pragma ACCEL PARALLEL COMPLETE
        for (int n = 0; n < 19; n++) {
#pragma ACCEL PARALLEL COMPLETE
          int ii = i + m - 9;
          int jj = j + n - 9;
          if (ii >= 0 && ii < h && jj >= 0 && jj < w) {
            float sum_r = inFrame[ii][jj] . r;
            float sum_g = inFrame[ii][jj] . g;
            float sum_b = inFrame[ii][jj] . b;
            printf("%d, %d: %f, %f, %f\n", ii, jj, sum_r, sum_g, sum_b);
          }
        }
      }
    }
  }
}

RGB c[100][100];
int main() {

  for (int i = 0; i < 100; i++) {
    for (int j = 0; j < 100; j++)  {
      c[i][j].r=i+j+1;
      c[i][j].g=i+j+0.5;
      c[i][j].b=i+j+0.2;
    }
  }

  func_top_1(c);
  return 0;
}
