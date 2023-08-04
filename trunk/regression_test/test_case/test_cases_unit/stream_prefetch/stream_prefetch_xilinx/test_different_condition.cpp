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

void stencil3d_kernel(int C[2],int orig[16385024],int sol[16385024])
{
  __merlin_access_range(sol,0,16385023UL);
  __merlin_access_range(orig,0,16385023UL);
  __merlin_access_range(C,0,1UL);
  int i;
  int j;
  int k;
  int sum0;
  int sum1;
  int mul0;
  int mul1;
  
#pragma ACCEL interface variable=C depth=2 max_depth=2
  
#pragma ACCEL interface variable=orig depth=16385024 max_depth=16385024
  
#pragma ACCEL interface variable=sol depth=16385024 max_depth=16385024
  int shift_square_0[32][16];
  int shift_square_1[32][16];
  int shift_square_2[32][16];
  int sol_temp[32][16];
  __merlin_streaming_range(orig,0,16385024);
  __merlin_streaming_range(sol,0,16384000);
  for (i = 0; i < 16001; i++) 
  {
    for (int i_sub = 0; i_sub < 2; ++i_sub) 
    {
#pragma ACCEL PIPELINE AUTO
#pragma ACCEL TILED_TAG
      for (j = 0; j < 32 + 2; j++) 
      {
#pragma ACCEL PARALLEL COMPLETE 
        if (j < 32) {
          for (k = 0; k < 16; k++) 
          {
#pragma ACCEL PARALLEL COMPLETE 
            shift_square_2[j][k] = orig[k + 16 * (j + 32 * (i * 2 + i_sub))] + 1;
          }
        }
        if (j >= 1 && j <= 32 && i * 2 + i_sub >= 1 && i * 2 + i_sub <= 32000) {
          for (k = 0; k < 16; k++) 
          {
#pragma ACCEL PARALLEL COMPLETE 
            sol[k + 16 * (j - 1 + 32 * (i * 2 + i_sub - 1))] = sol_temp[j - 1][k] + 1;
          }
        }
      }
    }
  }
}
