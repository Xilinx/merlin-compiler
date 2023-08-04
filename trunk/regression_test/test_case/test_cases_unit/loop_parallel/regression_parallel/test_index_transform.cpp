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

typedef double Scalar;

#pragma ACCEL kernel
void top(int start, int j0, int step) {

  Scalar accu[9][1000];
  double data1_3_0_buf[90000];
  double data_3_0_buf[90000];
  int indices_3_0_buf[500][36];

  int sub_start;
  int i;

  for (i = 0; i < 18; ++i) {
    for (sub_start = 0; sub_start < 500; sub_start++) {

#pragma ACCEL PIPELINE II = 1
      int sub_start0 = sub_start * 2 + 0;
      int sub_start1 = sub_start * 2 + 1;
      int new_start0 = start * 10000 + sub_start0 + j0 * 1000;
      int new_start1 = start * 10000 + sub_start1 + j0 * 1000;
      int idx_step = indices_3_0_buf[sub_start][i] * step;
      int idx_step1 = indices_3_0_buf[sub_start][18 + i] * step;
      Scalar x = data_3_0_buf[0 + (idx_step + 0)];
      Scalar y = data_3_0_buf[0 + (idx_step + 1)];
      Scalar z = data_3_0_buf[0 + (idx_step + 2)];
      Scalar x1 = data1_3_0_buf[0 + (idx_step1 + 0)];
      Scalar y1 = data1_3_0_buf[0 + (idx_step1 + 1)];
      Scalar z1 = data1_3_0_buf[0 + (idx_step1 + 2)];

      accu[0][sub_start0] = x * x;
      accu[1][sub_start0] = x * y;
      accu[2][sub_start0] = x * z;
      accu[3][sub_start0] = y * y;
      accu[4][sub_start0] = y * z;
      accu[5][sub_start0] = z * z;
      accu[6][sub_start0] = x;
      accu[7][sub_start0] = y;
      accu[8][sub_start0] = z;
    }
  }
  return;
}
