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
#include<math.h>

#pragma ACCEL kernel
void func_top(float a[10][10], float b[10][10]) {

  int i, j;
  float c_buf[10][10];
  float val3=0;
  val3 = a[1][5];

  for (i = 0; i < 10; i++) {
#pragma ACCEL pipeline
    int val2 = 1;
    for (j = 0; j < 10; j++) {
      c_buf[i][j] = a[i][j] + val2 + val3;
    }
    const int val = 5;
    int val1 = val3;
    for (j = 0; j < 10; j++) {
      b[i][j] = c_buf[i][j] + val + val1*10 + val2;
    }
  }
}

void func_top_sw(float a[10][10], float b[10][10]) {

  int i, j;
  float c_buf[10][10];
  float val3=0;
  val3 = a[1][5];

  for (i = 0; i < 10; i++) {
#pragma ACCEL pipeline
    int val2 = 1;
    for (j = 0; j < 10; j++) {
      c_buf[i][j] = a[i][j] + val2 + val3;
    }
    const int val = 5;
    int val1 = val3;
    for (j = 0; j < 10; j++) {
      b[i][j] = c_buf[i][j] + val + val1*10 + val2;
    }
  }
}

int main() {

  float a[10][10];
  for (int i = 0; i < 10; i++)
  for (int i0 = 0; i0 < 10; i0++)
  {
    a[i][i0]=i+i0;
  }
  float b[10][10];
  float b_sw[10][10];
  func_top(a, b);
  func_top_sw(a, b_sw);
  for (int i = 0; i < 10; i++)
  for (int i0 = 0; i0 < 10; i0++)
  {
      float larger = b[i][i0]-b_sw[i][i0]; 
      if (fabs(larger) > 1e-5) {
          printf("[ERROR] out=%f exp=%f\n", b[i][i0], b_sw[i][i0]);
          printf("Error.\n");
          return 1 ;
      }
  }   

  printf("Success.\n");
  return 0;
}
