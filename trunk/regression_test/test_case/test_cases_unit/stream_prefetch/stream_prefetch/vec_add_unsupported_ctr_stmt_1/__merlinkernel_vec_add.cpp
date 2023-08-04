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

void vec_add(int s,int a1[1024],int a2[1024],int a3[1024],int a4[1024],int a5[1024],int a6[1024],int a7[1024],int a8[1024],int a9[1024],int a10[1024],int merlin_return_value[1])
{
  __merlin_access_range(merlin_return_value,0,0UL);
  __merlin_access_range(a10,0,1023UL);
  __merlin_access_range(a9,0,1023UL);
  __merlin_access_range(a8,0,1023UL);
  __merlin_access_range(a7,0,1023UL);
  __merlin_access_range(a6,0,1023UL);
  __merlin_access_range(a5,0,1023UL);
  __merlin_access_range(a4,0,1023UL);
  __merlin_access_range(a3,0,1023UL);
  __merlin_access_range(a2,0,1023UL);
  __merlin_access_range(a1,0,1023UL);
  
#pragma ACCEL interface variable=merlin_return_value io=WO scope_type=return max_depth=1 depth=1
  
#pragma ACCEL interface variable=a1 stream_prefetch=on max_depth=1024 depth=1024
  
#pragma ACCEL interface variable=a2 max_depth=1024 depth=1024
  
#pragma ACCEL interface variable=a3 max_depth=1024 depth=1024
  
#pragma ACCEL interface variable=a4 max_depth=1024 depth=1024
  
#pragma ACCEL interface variable=a5 max_depth=1024 depth=1024
  
#pragma ACCEL interface variable=a6 max_depth=1024 depth=1024
  
#pragma ACCEL interface variable=a7 max_depth=1024 depth=1024
  
#pragma ACCEL interface variable=a8 max_depth=1024 depth=1024
  
#pragma ACCEL interface variable=a9 max_depth=1024 depth=1024
  
#pragma ACCEL interface variable=a10 max_depth=1024 depth=1024
  int i = 0;
  int ret = 0;
// multiple access
  ret += a1[0] + a1[0];
// while loop
  while(i < 100){
    ret += a2[i];
  }
// non-canonical loop
  for (i = 0; i < 100 && i > - 100; ++i) {
    ret += a3[i];
  }
// assign iterator in loop
  for (i = 0; i < 100; ++i) {
    ret += a4[i];
    i = 200;
  }
// goto stmt
  // for (i = 0; i < 100; ++i) {
  //   ret += a5[i];
  //   goto A;
  // }
  // A:
// break stmt
  for (i = 0; i < 100; ++i) {
    ret += a6[i];
    break; 
  }
// break stmt
  for (i = 0; i < 100; ++i) {
    continue; 
    ret += a7[i];
  }
// switch stmt
  switch(2){
    case 0:
    ret += a8[0];
    break; 
  }
// use iterator outside of a loop
  ret += a9[8];
  ret += a10[168];
  merlin_return_value[0] = ret;
}
