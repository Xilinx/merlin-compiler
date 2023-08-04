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
#include "merlin_type_define.h"
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_kernel_pragma();
// Original: #pragma ACCEL kernel

void ellpack_kernel(double nzval[(1 << 12) * (1 << 9)],int cols[(1 << 12) * (1 << 9)],double vec[1 << 12],double merlin_out[1 << 12])
{
  __merlin_access_range(merlin_out,0,4095UL);
  __merlin_access_range(vec,0,4095UL);
  __merlin_access_range(cols,0,2097151UL);
  __merlin_access_range(nzval,0,2097151UL);
//TYPE Si;
  
#pragma ACCEL interface variable=nzval max_depth=2097152 depth=2097152
  
#pragma ACCEL interface variable=cols max_depth=2097152 depth=2097152
  
#pragma ACCEL interface variable=vec max_depth=4096 depth=4096
  
#pragma ACCEL interface max_depth=4096 depth=4096 variable=merlin_out
#define TS 16
#define TS1 (8)
#define TS2 (32)
  ellpack_1:
//#pragma ACCEL pipeline
  for (int i0 = 0; i0 < (1 << 12) / 32; i0++) {
    for (int iii = 0; iii < 32; iii++) {
      int i = i0 * 32 + iii;
      int j0;
      int jj;
// = out[i];
      double sum = (double )0;
      ellpack_2:
      for (j0 = 0; j0 < (1 << 9) / 16; j0++) {
        for (jj = 0; jj < 16; jj++) {
          int j = j0 * 16 + jj;
          double Si = nzval[j + i * (1 << 9)] * vec[cols[j + i * (1 << 9)]];
          sum += Si;
        }
      }
      merlin_out[i] += sum;
    }
  }
}
