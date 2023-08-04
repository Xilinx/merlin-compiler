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
void func_top_1(float *merlin_in) {
#pragma ACCEL INTERFACE VARIABLE=merlin_in DEPTH=1000
  float tmp=0;
  int i;
  for (i = 0; i < 99; i++) {
    int j = 1;
    while( j < 5) {
#pragma ACCEL line_buffer variable=merlin_in
      tmp += (merlin_in[i] + merlin_in[i + 1])/j;
      printf("%d, %d : %f\n", i , j, tmp );
      j++;
    }
  }
}

float c[1000];
int main() {

  for (size_t i = 0; i < 1000; i++)
  {
    c[i]=i;
  }

  func_top_1(c);
  return 0;
}
