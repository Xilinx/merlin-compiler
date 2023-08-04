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

void foo(int a[10000], int b[10000], int c[10000]) {
    int n, p;
    for (n = 0; n < 511; n++) {
#pragma ACCEL parallel factor=auto{options: P0}
      for (p = 0; p < 687; p++) {
        c[n] = a[n] + b[p];
      }
    }
    return ;
}

void vec_add_kernel(int a[10000],int b[10000],int c[10000],int inc)
{
  __merlin_access_range(c,0,9999UL);
  __merlin_access_range(b,0,9999UL);
  __merlin_access_range(a,0,9999UL);
  
#pragma ACCEL interface variable=c max_depth=10000 depth=10000 bus_bitwidth=auto{ \
    options: B=[32,64,128,256,512]; default: 32}
#pragma ACCEL interface variable=b max_depth=10000 depth=10000
#pragma ACCEL interface variable=a max_depth=10000 depth=10000
  int i;
  int j;
  int k;
  int m;
  int n;
  int p;
  int q;
  for (i = 0; i < 10000; i++) {
#pragma ACCEL pipeline AUTO{OPTIONS: I=["off", "", "flatten"]; DEFAULT: "off"; \
                            ORDER: 0 if v!="flatten" else 1}
    for (j = 0; j < 512; j++) {
#pragma ACCEL parallel factor=auto{options:P0=[2**x for x in range(0, 10) if x==0 or I != "flatten"]; \
                                   default: 1} priority=logic
      foo(a, b, c);
    }

    foo(a, b, c);

    for (q = 0; q < 35; q++) {
      c[q] += q;
    }
  }

  for (i = 0; i < 10000; i++) {
#pragma ACCEL tiling factor=auto{options: T0=[2**x for x in range(0, 11) if T1 == 1]; default: 1; \
                                 order: 0 if v <= 256 else 1 if v <= 512 else 2}
    for (j = 0; j < 512; j++) {
#pragma ACCEL tiling factor=auto{options:T1 = [2**x for x in range(0, 10) if T0 == 1]; default: 1}
      for (k = 0; k < 751; k++) {
#pragma ACCEL parallel factor=auto{options: P1=[str(2**x) for x in range(0, 10)]; default: "1"}
        if (k < 300) {
            for (m = 0; m < 127; m++) {
#pragma ACCEL parallel factor=auto{options: P2=range(1, 10); default: 1}
              c[i] = a[j] + b[k + m];
            }
        }
        else {
            for (n = 0; n < 63; n++) {
              c[i] = a[j] + b[k + n];
            }
        }
      }
    }
  }

}
