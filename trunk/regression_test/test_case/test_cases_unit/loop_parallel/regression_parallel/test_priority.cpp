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
void func_test() {
  int a[64][64];
  int b[64][64];
  int i;
  int j;
  /*Array Access*/
  for (j = 1; j < 63; j++) {
    for (i = 1; i < 63; i++) {

#pragma ACCEL PIPELINE II = 1 PRIORITY = logic
      b[j][i] = (a[j + 1][i] + a[j - 1][i] + a[j][i + 1] + a[j][i - 1]) / 4;
    }
  }
  for (j = 1; j < 63; j++) {
    for (i = 1; i < 8; i++) {

#pragma ACCEL PARALLEL PRIORITY = logic
      b[j][i] = (a[j + 1][i] + a[j - 1][i] + a[j][i + 1] + a[j][i - 1]) / 4;
    }
  }
}

#pragma ACCEL kernel
void func_org() {
  int a[64][64];
  int b[64][64];
  int i;
  int j;
  /*Array Access*/
  for (j = 1; j < 63; j++) {
    for (i = 1; i < 63; i++) {

#pragma ACCEL PIPELINE II = 1
      b[j][i] = (a[j + 1][i] + a[j - 1][i] + a[j][i + 1] + a[j][i - 1]) / 4;
    }
  }
  for (j = 1; j < 63; j++) {
    for (i = 1; i < 8; i++) {

#pragma ACCEL PARALLEL
      b[j][i] = (a[j + 1][i] + a[j - 1][i] + a[j][i + 1] + a[j][i - 1]) / 4;
    }
  }
}
