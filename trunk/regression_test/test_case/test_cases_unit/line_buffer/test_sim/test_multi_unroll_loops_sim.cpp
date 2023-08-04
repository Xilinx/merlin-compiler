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

#pragma ACCEL kernel
void func_top_1(float *merlin_input) {
 #pragma ACCEL interface variable=merlin_input depth=4,100,100 max_depth=4,100,100
    conv:
  for (int j = 0; j < 4; ++j) {
loop_h:
    for (int h = 0; h < 80 ; ++h) {
loop_w:
      for (int w = 0; w < 96; ++w) 
      {
#pragma ACCEL LINE_BUFFER VARIABLE=merlin_input

#pragma ACCEL PIPELINE
loop_p:
        for (int iter = 0; iter < 16; iter++) {

#pragma ACCEL PARALLEL COMPLETE
          float tmp = (float )0;
          for (int p = 0; p < 5; ++p) {

#pragma ACCEL PARALLEL COMPLETE
Loop_q:
            for (int q = 0; q < 5; ++q) {

#pragma ACCEL PARALLEL COMPLETE
              tmp = merlin_input[h * 100L + iter * 100L + j * 10000L + p * 100L + q + w];
              printf("%d, %d, %d, %d, %d, %d: %f\n", j, h, w, iter, p, q, tmp );
            }
          }
        }
      }
    }
  }
}

float c[40000];
int main() {

  for (size_t i = 0; i < 40000; i++)
  {
    c[i]=i;
  }

  func_top_1(c);
  return 0;
}
