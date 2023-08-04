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
const int val[100] = {0};

#pragma ACCEL kernel
void func_top_0() {
  int a[100][100];
  int b[100][101];
  int k;
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    for (k = 0; k < 99; k++) {
      for (j = 0; j < 99; j++) {
#pragma ACCEL parallel complete
        a[i][j] = b[k][j + 1] + b[k + 1][j] + val[j];
      }
    }
  }
  for (j = 0; j < 100; j++) {
    for (k = 0; k < 20; k++) {
#pragma ACCEL parallel factor = 5 priority = logic
      a[i][j] = b[4 * k][j] + b[4 * k + 1][j + 1];
    }
  }

  for (j = 0; j < 100; j++) {
    for (k = 0; k < 20; k++) {
#pragma ACCEL parallel factor = 5 priority = bram
      a[i][j] = b[4 * k][j] + b[4 * k + 1][j + 1];
    }
  }
}

