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
void func_top_1(int input[10]) {
  int fully_a[10];
#pragma HLS RESOURCE variable=fully_a core=RAM_T2P_URAM
  int fully_b[10];
#pragma HLS RESOURCE variable=fully_b core=RAM_2P_URAM
  int c0[2];
#pragma HLS RESOURCE variable=c0 core=RAM_1P_LUTRAM
  int c1[2];
#pragma HLS RESOURCE variable=c1 core=ROM_1P_LUTRAM

  for (int i = 0; i < 10; i++) {
#pragma ACCEL pipeline
    fully_a[i] = input[i];
    fully_b[i] = input[i];
    if (i%2) {
      fully_a[i+1] = input[i]+1; 
      fully_b[i+1] = input[i]+1; 
    }
  }

  for (int i = 0; i < 2; i++) {
#pragma ACCEL parallel
    c0[i] = c1[i];
  }
}
