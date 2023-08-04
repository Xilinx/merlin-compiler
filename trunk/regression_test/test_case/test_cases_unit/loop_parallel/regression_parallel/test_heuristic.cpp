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
#include <ap_int.h>

typedef class ap_uint< 24 > StreamType_9_name;
typedef class ap_uint< 24 > PixelType_15_name;

#pragma ACCEL kernel
void func_top_1(int width, unsigned char out[1]) {
  unsigned char lut[3][256];
  StreamType_9_name val_src;
  int k;

colLoop:
  for (int j = 0; j<width; j ++ ) {
#pragma ACCEL PIPELINE AUTO
#pragma HLS LOOP_TRIPCOUNT min=128 max=128
    unsigned char l = (unsigned char )0;
    int c = 0;
procLoop:
    for (k = 0; k<=2L; k ++ ) {
#pragma ACCEL PARALLEL COMPLETE
      class ap_uint< 13 > _in_k = 0L+8L*k;
      PixelType_15_name p;
      p = val_src . range((_in_k+(8 - 1)),_in_k);
      out[0] += ((unsigned long )lut[l][(long )p]);
      l++;
    }
    k = 16L + 8L;
  }
}
