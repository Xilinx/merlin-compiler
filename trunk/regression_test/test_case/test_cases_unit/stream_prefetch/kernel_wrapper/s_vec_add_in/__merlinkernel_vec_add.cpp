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
#include <math.h>
#include "merlin_type_define.h"
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
#pragma ACCEL kernel

void vec_add(int s,int a[1024],int b[1024],int c[1024])
{
  
#pragma ACCEL interface variable=c depth=1024
  
#pragma ACCEL interface variable=b depth=1024
  
#pragma ACCEL interface variable=a stream_prefetch=on depth=1024
  
#pragma ACCEL interface variable=s stream_prefetch=on
  for (int i = 0; i < 1024; i++) 
// Original: #pragma ACCEL parallel factor=4
{
    
#pragma ACCEL PARALLEL FACTOR=4 
    int a0 = a[i];
    int b0 = b[i];
    int c0 = 0;
    for (int j = 0; j < 1024 * 10; j++) {
      c0 += ((int )(pow((double )(a0 * b0 + j),1.5))) % 7 + s;
    }
    c[i] = c0;
  }
}
