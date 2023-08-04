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
#include "ap_int.h"
#include "xf_params.h"
#include "xf_structs.h"
#include "xf_types.h"

void sub_func(StreamType< 5> ::name &P0) {
    class ap_uint< 8 > STEP(8);
    P0 = P0 |P0<<2*STEP;;
}
#pragma ACCEL kernel
void func_top_7(float a[100][100], float b[100][100]) {

  int i, j;
  int img_width = 100;
  float c_buf[100][100];
  StreamType< 5> ::name P0;
  class ap_uint< 13 > col;
  for (col = 0; col<((int )img_width); col ++ ) {
#pragma ACCEL pipeline
#pragma HLS LOOP_TRIPCOUNT min=100 max=100
    P0 = 1;
    sub_func(P0);
    for (j = 0; j < 100; j++) {
      c_buf[(long )col][j] = a[(long )col][j];
    }
  }
}


