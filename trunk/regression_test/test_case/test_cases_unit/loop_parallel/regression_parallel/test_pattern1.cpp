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
void func_top_0(int offset[100], int b[100]) {
  int a[7][100];
  int c[36];
  int ca[100];
  int cc[512];
  int k;
  int i;
  int j;
  for (i = 0; i < 36; i++) {
#pragma ACCEL pipeline
        c[offset[i]] = b[i] + 1;
        c[offset[i+1]] = b[i+1] + c[offset[i]] + c[offset[i+2]];
  }
  
  int ii=0;
    // bound check array 'ca' read_range=[0..136 inexact]
//  for (j = 0; j < 36; j++) {
//#pragma ACCEL pipeline
//      ii = j/2;
//      ca[offset[ii]] = b[ii] + 1;
//      ca[offset[ii+1]] = b[j+1] + ca[j] + ca[ii << 2 +1];
//  }
 
  for (i = 0; i < 36; i++) {
#pragma ACCEL pipeline
        cc[offset[i]] = c[i];
        cc[offset[i+1]] = c[offset[i]] + cc[offset[i+2]];
  }

  for (i = 0; i < 100; i++) {
#pragma ACCEL pipeline
      for (j = 0; j < 7; j++) {
#pragma ACCEL parallel
        a[offset[j*4]][i] = b[i] + 1;
      }
    }
}

