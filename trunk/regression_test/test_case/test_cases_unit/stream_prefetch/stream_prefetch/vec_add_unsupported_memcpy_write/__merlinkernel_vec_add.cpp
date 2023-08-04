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
#include <string.h>
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_kernel_pragma();
// Original: #pragma ACCEL kernel

void vec_add(int s,int a1[1024],int a2[1024])
{
  __merlin_access_range(a2,0,1023UL);
  __merlin_access_range(a1,0,1023UL);
  
#pragma ACCEL interface variable=a1 max_depth=1024 depth=1024
  
#pragma ACCEL interface variable=a2 max_depth=1024 depth=1024
  int a1_buf[1024];
  int a2_buf[1024];
  memcpy((void *)a1_buf,(const void *)a1,4 * ((unsigned long )1024));
  memcpy((void *)a2_buf, a2, 4 * 1024);
  for (int i = 0; i < 1024; ++i) {
    a2_buf[i] = a1_buf[1023 - i] + a1[i] + a2[i];
  }
  memcpy(a2, a2_buf, 4 * 1024);

}
