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
#define n 100

#pragma ACCEL kernel
void func_top_0(int *c) {
#pragma ACCEL INTERFACE VARIABLE=c DEPTH=1000
  int sum=0;
  for (int i = 1; i < 45; i++) {
#pragma ACCEL line_buffer variable=c
    for (int j = 0; j < 50; j++) {
      for (int k = 0; k < 4; k++) {
        sum = c[i+j+k-1] + c[i+j+k+1];
      }
    }
  }
}
