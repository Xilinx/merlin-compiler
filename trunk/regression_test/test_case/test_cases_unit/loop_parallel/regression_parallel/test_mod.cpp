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
void func_top_37_0(int N) {
  int a[100][100];
  int b[100][100];
  int i;
  int j;
  for (i = 0; i < 100; i++) {
#pragma ACCEL PIPELINE II = 1
    for (j = 0; j < 100; j++) {

#pragma ACCEL PARALLEL COMPLETE
      a[i % 50][j % 50] = b[i][j % N];
    }
  }
}
#pragma ACCEL kernel
void func_top_37() {
  int a[100][100];
  int b[100][100];
  int i;
  int j;
  for (i = 0; i < 100; i++) {
#pragma ACCEL PIPELINE II = 1
    for (j = 0; j < 100; j++) {

#pragma ACCEL PARALLEL COMPLETE
      a[i][j % 100] = b[i][j];
    }
  }
}
#pragma ACCEL kernel
void func_top_38() {
  int a[100][100];
  int b[100][100];
  int i;
  int j;
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 100; j++) {

#pragma ACCEL PARALLEL factor = 4
      a[i][j % 99] = b[i][(j + 1) % 100] + b[i][(j + 3) % 100];
    }
  }
}

#pragma ACCEL kernel
void func_top_39(int x, int y, int z, int sum[1]) {
  float sub_force_x[4] = {(0.0), (0.0), (0.0), (0.0)};
  float sub_force_y[4] = {(0.0), (0.0), (0.0), (0.0)};
  float sub_force_z[4] = {(0.0), (0.0), (0.0), (0.0)};
  int j, j0;
  for (j = 0; j < 25; j++) {
#pragma ACCEL PIPELINE
    for (j0 = 0; j0 < 4; j0++) {
#pragma ACCEL PARALLEL
      int jj = j * 4 + j0;
      sub_force_x[jj % 4] += x;
      sub_force_y[jj % 4] += y;
      sub_force_z[jj % 4] += z;
    }
  }
  sum[0] = sub_force_x[0] + sub_force_y[0] + sub_force_z[0];
}

