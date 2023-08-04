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

void convolve(float inFrame_r[2073600],float inFrame_g[2073600],float inFrame_b[2073600],float *outFrame_r,float *outFrame_g,float *outFrame_b,const float *coef,int coef_size,int w,int h)
{
  __merlin_access_range(coef,0,360UL);
  __merlin_access_range(outFrame_b,0,2073599UL);
  __merlin_access_range(outFrame_g,0,2073599UL);
  __merlin_access_range(outFrame_r,0,2073599UL);
  __merlin_access_range(inFrame_b,0,2073599);
  __merlin_access_range(inFrame_g,0,2073599);
  __merlin_access_range(inFrame_r,0,2073599);

#pragma ACCEL INTERFACE DEPTH=2073600 MAX_DEPTH=2073600 VARIABLE=outFrame_b

#pragma ACCEL INTERFACE DEPTH=2073600 MAX_DEPTH=2073600 VARIABLE=outFrame_g

#pragma ACCEL INTERFACE DEPTH=2073600 MAX_DEPTH=2073600 VARIABLE=outFrame_r

#pragma ACCEL INTERFACE BURST_OFF DEPTH=2304,900 MAX_DEPTH=2304,900 VARIABLE=inFrame_b

#pragma ACCEL INTERFACE BURST_OFF DEPTH=2304,900 MAX_DEPTH=2304,900 VARIABLE=inFrame_g

#pragma ACCEL INTERFACE BURST_OFF DEPTH=2304,900 MAX_DEPTH=2304,900 VARIABLE=inFrame_r

#pragma ACCEL interface variable=coef depth=361 max_depth=361
  coef_size = 19;
  h = 2304;
  w = 900;
  for (int i = (int )(- 9L); ((long )i) < 2313L; i++) 
  {

#pragma ACCEL PIPELINE 
    for (int j = (int )(- 9L); ((long )j) < 909L; j++) 
    {
#pragma ACCEL PARALLEL COMPLETE 

      {
        if (((long )i) + - 9L + - 9L + 18L < 2304L && ((long )i) + - 9L + - 9L + 18L >= 0L && ((long )j) + - 9L + - 9L + 18L < 900L && ((long )j) + - 9L + - 9L + 18L >= 0L) {
          inFrame_r[i * 900L + j];
          inFrame_g[i * 900L + j];
          inFrame_b[i * 900L + j];
        }
      }
    }
  }
}
