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
void func_top_0() {
  int a[100][100];
  int b[100][100];
  int c[3][2048];
  int c1[2000];
  int k;
  int i;
  int j;
  for (i = 0; i < 100; i++) {
#pragma ACCEL pipeline
    for (k = 0; k < 99; k++)
#pragma ACCEL parallel complete
      for (j = 0; j < 99; j++) {
#pragma ACCEL parallel complete
        a[i][j] = b[k][j] + b[k + 1][j + 1];
      }
  }
  for (i = 0; i < 32; i++) {
#pragma ACCEL pipeline
    for (j = 0; j < 1025; j++) {
#pragma ACCEL parallel complete
      c1[j] = j;
    }
  }

  for (j = 0; j < 2000; j++) {
#pragma ACCEL parallel complete
    if(j <= 1500)
      c1[j] = j;
  }

  for (i = 0; i < 3; i++) {
#pragma ACCEL pipeline
    for (int t = 0; t < 32; t++) {
#pragma ACCEL parallel
      for (j = 0; j < 32; j++) {
#pragma ACCEL parallel
        c[i][t*32+j] = j;
        c[i][t*32+j+1] = j;
        c[i][t*32+j+2] = j;
        c[i][t*32+j+3] = j;
        c[i][t*32+j+4] = j;
      }
    }
  }
}


