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
static void __merlin_dummy_extern_int_merlin_include_G_();
const int sizex = 504;
const int sizey = 504;

static int coord(int x,int y,int z)
{
  return z * sizex * sizey + y * sizex + x;
}
const int sizez = 504;
static void __merlin_dummy_kernel_pragma();
#pragma ACCEL kernel

void fd3d_kernel(int time_steps,float h_coeff[128024064],float h_dataA[128024064],float h_dataB[128024064])
{
  __merlin_access_range(h_dataB,0,128024063UL);
  __merlin_access_range(h_dataA,0,128024063UL);
  __merlin_access_range(h_coeff,0,128024063UL);
  
#pragma ACCEL interface variable=h_dataB max_depth=128024064 depth=128024064
  
#pragma ACCEL interface variable=h_dataA max_depth=128024064 depth=128024064
  
#pragma ACCEL interface variable=h_coeff max_depth=128024064 depth=128024064
  float *from;
  float *to;
  int i = 0;
{
    if ((bool )(0 % 2)) {
      from = h_dataB;
    }
     else {
      from = h_dataA;
    }
    if ((bool )(0 % 2)) {
      to = h_dataA;
    }
     else {
      to = h_dataB;
    }
    for (int z = 4; z < sizez - 4; z++) {
      for (int y = 4; y < sizey - 4; y++) {
        for (int x = 4; x < sizex - 4; x++) {
          float result = h_coeff[0] * from[coord(x,y,z)];
          for (int r = 1; r <= 4; r++) {
            
#pragma ACCEL PARALLEL COMPLETE
            result += h_coeff[r] * from[coord(x - r,y,z)];
            result += h_coeff[r] * from[coord(x + r,y,z)];
            result += h_coeff[r] * from[coord(x,y - r,z)];
            result += h_coeff[r] * from[coord(x,y + r,z)];
            result += h_coeff[r] * from[coord(x,y,z - r)];
            result += h_coeff[r] * from[coord(x,y,z + r)];
          }
          to[coord(x,y,z)] = result;
        }
      }
    }
    for (int z = 0; z < sizez; z++) {
      for (int y = 0; y < sizey; y++) {
        for (int x = 0; x < 4; x++) {
          
#pragma ACCEL PARALLEL COMPLETE
          to[coord(x,y,z)] = from[coord(x,y,z)];
        }
      }
    }
    for (int z = 0; z < sizez; z++) {
      for (int y = 0; y < sizey; y++) {
        for (int x = 500; x < sizex; x++) {
          
#pragma ACCEL PARALLEL COMPLETE
          to[coord(x,y,z)] = from[coord(x,y,z)];
        }
      }
    }
    for (int z = 0; z < sizez; z++) {
      for (int y = 0; y < 4; y++) {
        for (int x = 0; x < sizex; x++) {
          to[coord(x,y,z)] = from[coord(x,y,z)];
        }
      }
    }
    for (int z = 0; z < sizez; z++) {
      for (int y = 500; y < sizey; y++) {
        for (int x = 0; x < sizex; x++) {
          to[coord(x,y,z)] = from[coord(x,y,z)];
        }
      }
    }
    for (int z = 0; z < 4; z++) {
      for (int y = 0; y < sizey; y++) {
        for (int x = 0; x < sizex; x++) {
          to[coord(x,y,z)] = from[coord(x,y,z)];
        }
      }
    }
    for (int z = 500; z < sizez; z++) {
      for (int y = 0; y < sizey; y++) {
        for (int x = 0; x < sizex; x++) {
          to[coord(x,y,z)] = from[coord(x,y,z)];
        }
      }
    }
  }
}
