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
#include <math.h>
#include "merlin_type_define.h"
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_kernel_pragma();
// Original: #pragma ACCEL kernel

void vec_add(int s,int a[1024],int b[1024],int c[1024])
{
  int i_sub;
  __merlin_access_range(c,0,1023UL);
  __merlin_access_range(b,0,1023UL);
  __merlin_access_range(a,0,1023UL);
  
#pragma ACCEL interface variable=b max_depth=1024 depth=1024
  
#pragma ACCEL interface variable=a stream_prefetch=on max_depth=1024 depth=1024
  
#pragma ACCEL interface variable=c stream_prefetch=on max_depth=1024 depth=1024
  
#pragma ACCEL interface variable=s stream_prefetch=on
// Original pramga:  ACCEL PARALLEL FACTOR=4 
  for (int i = 0; i < 256; i++) 
// Original: #pragma ACCEL parallel factor=4
// Original: #pragma ACCEL PARALLEL FACTOR=4
{
    
#pragma ACCEL PARALLEL FACTOR=1
    for (i_sub = 0; i_sub < 4; ++i_sub) 
// Original: #pragma ACCEL parallel factor=4
// Original: #pragma ACCEL PARALLEL FACTOR=4
{
      
#pragma ACCEL PARALLEL 
      int a0 = a[i * 4 + i_sub];
      int b0 = b[i * 4 + i_sub];
      int c0 = 0;
      for (int j = 0; j < 1024 * 10; j++) {
        c0 += ((int )(pow((double )(a0 * b0 + j),1.5))) % 7 + s;
      }
      c[i * 4 + i_sub] = c0;
    }
  }
}
