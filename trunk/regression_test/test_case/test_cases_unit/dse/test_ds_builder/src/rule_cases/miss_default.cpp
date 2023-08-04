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
  
#pragma ACCEL interface variable=c max_depth=10000 depth=10000
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
    for (j = 0; j < 512; j++) {
      foo(a, b, c);
    }

    foo(a, b, c);

    for (q = 0; q < 35; q++) {
      c[q] += q;
    }
  }

#pragma ACCEL parallel factor=auto{options: P=range(10)}
  for (i = 0; i < 10000; i++) {
    for (j = 0; j < 512; j++) {
      for (k = 0; k < 751; k++) {
        if (k < 300) {
            for (m = 0; m < 127; m++) {
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
