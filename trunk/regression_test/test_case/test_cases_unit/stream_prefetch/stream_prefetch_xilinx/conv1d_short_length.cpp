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
#include "cmost.h"
#pragma ACCEL kernel

void conv1d(float *merlin_in,float filter[5],float *merlin_out)
{
  __merlin_access_range(merlin_out,0,1044479);
  __merlin_access_range(filter,0,4UL);
  __merlin_access_range(merlin_in,0,1048575);
  
#pragma ACCEL interface variable=merlin_in burst_off max_depth=1024,1024 depth=1024,1024 streaming_access=0,1048576
  
#pragma ACCEL interface variable=filter burst_off max_depth=5 depth=5
  
#pragma ACCEL interface variable=merlin_out burst_off max_depth=1024,1020 depth=1024,1020
  __merlin_streaming_range(merlin_out,0,1044480L);
  for (int i = 0; i < 1024; i++) {
    float slide_wind_0[5];
    __merlin_streaming_range(merlin_in,(0L + i * 1L) * 1024L,1024L);
    for (int j = -4; j < 1024 - 5 + 1; j++) 
// Original: #pragma ACCEL line_buffer variable=in
// Original: #pragma ACCEL LINE_BUFFER VARIABLE=merlin_in
{
      
#pragma HLS dependence variable=slide_wind_0 array inter false
      
#pragma ACCEL PIPELINE AUTO
      if (j + 4L + 0L < 1024L) {
        float tmp_wind_0;
        int j_0;
        for (j_0 = 1L; j_0 <= 4L; ++j_0) {
          
#pragma ACCEL PARALLEL COMPLETE
          slide_wind_0[j_0 + -1] = slide_wind_0[j_0];
        }
        tmp_wind_0 = merlin_in[(0L + i * 1L) * 1024L + (j + 4L + 0L) * 1L];
        slide_wind_0[4L] = tmp_wind_0;
      }
      if (j >= 0L && j <= 1019L) {
        float tmp = 0.0f;
        for (int q = 0; q < 5; q++) 
// Original: #pragma ACCEL parallel
// Original: #pragma ACCEL PARALLEL COMPLETE
{
          
#pragma ACCEL PARALLEL COMPLETE
          tmp += slide_wind_0[q + 0L] * filter[q];
        }
        merlin_out[((long )i) * 1020L + ((long )j)] = tmp;
      }
    }
  }
}
