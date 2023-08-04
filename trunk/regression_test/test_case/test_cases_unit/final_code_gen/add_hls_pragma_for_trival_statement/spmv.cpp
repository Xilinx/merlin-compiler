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
/* Based on algorithm described here: http://www.cs.berkeley.edu/~mhoemmen/matrix-seminar/slides/UCB_sparse_tutorial_1.pdf
*/
#include "spmv.h"
#include <string.h>
static void __merlin_dummy_kernel_pragma();

void ellpack_kernel(double nzval[(1 << 12) * (1 << 9)],int cols[(1 << 12) * (1 << 9)],double vec[1 << 12],double out[1 << 12])
{
//TYPE Si;
  
#pragma ACCEL interface variable=nzval depth=2097152
  
#pragma ACCEL interface variable=cols depth=2097152
  
#pragma ACCEL interface variable=vec depth=4096
  
#pragma ACCEL interface variable=out depth=4096
#define TS 16
#define TS1 (8)
#define TS2 (32)
  ellpack_1:
//#pragma ACCEL pipeline
  for (int i0 = 0; i0 < (1 << 12) / 32; i0++) 
    for (int iii = 0; iii < 32; iii++) {
      int i = i0 * 32 + iii;
      int j0;
      int jj;
// = out[i];
      double sum = 0;
      ellpack_2:
      for (j0 = 0; j0 < (1 << 9) / 16; j0++) {
        for (jj = 0; jj < 16; jj++) {
          int j = j0 * 16 + jj;
          double Si = nzval[j + i * (1 << 9)] * vec[cols[j + i * (1 << 9)]];
          sum += Si;
        }
      }
      out[i] += sum;
    }
}
