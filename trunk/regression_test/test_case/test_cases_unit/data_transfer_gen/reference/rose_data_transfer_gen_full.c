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
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <stdio.h> 
#include <stdlib.h> 
extern int __merlin_include__GB_stdlib_h_8;
extern int __merlin_include__GB_stdio_h_7;
extern int __merlin_include__GB_string_h_6;
extern int __merlin_include__GB_stdlib_h_5;
extern int __merlin_include__GB_stdio_h_4;
extern int __merlin_include__GB_string_h_3;
extern int __merlin_include__GB_stdlib_h_2;
extern int __merlin_include__GB_stdio_h_1;
extern int __merlin_include__GB_string_h_0;
struct t1 {
float a[4];}gvar0;
#pragma ACCEL kernel

void f0(float gvar0_a[4])
{
  __merlin_access_range(gvar0_a,0,3);
  
#pragma ACCEL INTERFACE DEPTH=1,4 VARIABLE=gvar0_a max_depth=1,4
  int i_1_0;
  for (i_1_0 = 0; i_1_0 < 4UL; ++i_1_0) {
    gvar0_a[i_1_0] = gvar0_a[i_1_0];
  }
}

void __merlinwrapper_f0()
{
  int _i0;
{
    
#pragma ACCEL wrapper VARIABLE=gvar0->a port=gvar0_a data_type=float depth=1,4 max_depth=1,4 io=RW copy=true
  }
  static float __m_gvar0_a[4];
  for (_i0 = 0; _i0 < 1; ++_i0) {
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "gvar0_a", "4 * sizeof(float )");
    
#pragma ACCEL debug fflush(stdout);
    if (gvar0 . a != 0) {
      memcpy((void *)(__m_gvar0_a + _i0 * 4),(const void *)gvar0 . a[_i0],4 * sizeof(float ));
    }
     else {
      printf("Error! Detected null pointer 'gvar0->a' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "gvar0_a");
    
#pragma ACCEL debug fflush(stdout);
  }
  
#pragma ACCEL wrapper VARIABLE=gvar0 port=gvar0 data_type="struct t1"
  f0(__m_gvar0_a);
  for (_i0 = 0; _i0 < 1; ++_i0) {
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "gvar0_a", "4 * sizeof(float )");
    
#pragma ACCEL debug fflush(stdout);
    if (gvar0 . a != 0) {
      memcpy((void *)gvar0 . a[_i0],(const void *)(__m_gvar0_a + _i0 * 4),4 * sizeof(float ));
    }
     else {
      printf("Error! Detected null pointer 'gvar0->a' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "gvar0_a");
    
#pragma ACCEL debug fflush(stdout);
  }
}
int gvar1;
#pragma ACCEL kernel

void f1(int *gvar1)
{
  __merlin_access_range(gvar1,0,0UL);
  
#pragma ACCEL interface variable=gvar1 depth=1 max_depth=1
   *gvar1 =  *gvar1;
}

void __merlinwrapper_f1()
{
  int _i0;
{
    
#pragma ACCEL wrapper VARIABLE=gvar1 port=gvar1 data_type=int depth=1 max_depth=1 io=RW copy=true
  }
  static int __m_gvar1[1];
  static int *__ti_gvar1;
  posix_memalign((void **)(&__ti_gvar1),64,sizeof(int ) * 1);
  for (_i0 = 0; _i0 < 1; ++_i0) {
    __ti_gvar1[_i0 * 1] = gvar1;
  }
  
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "gvar1", "1 * 1 * sizeof(int )");
  
#pragma ACCEL debug fflush(stdout);
  memcpy((void *)__m_gvar1,(const void *)__ti_gvar1,1 * 1 * sizeof(int ));
  
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "gvar1");
  
#pragma ACCEL debug fflush(stdout);
  
#pragma ACCEL wrapper VARIABLE=gvar1 port=gvar1 data_type=int depth=1 max_depth=1 io=RW copy=true
  f1(__m_gvar1);
  static int *__to_gvar1;
  posix_memalign((void **)(&__to_gvar1),64,sizeof(int ) * 1);
  
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "gvar1", "1 * 1 * sizeof(int )");
  
#pragma ACCEL debug fflush(stdout);
  memcpy((void *)__to_gvar1,(const void *)__m_gvar1,1 * 1 * sizeof(int ));
  
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "gvar1");
  
#pragma ACCEL debug fflush(stdout);
  for (_i0 = 0; _i0 < 1; ++_i0) {
    gvar1 = __to_gvar1[_i0 * 1];
  }
}
#pragma ACCEL kernel

void f2(float gvar0_a[4])
{
  __merlin_access_range(gvar0_a,0,3);
  
#pragma ACCEL INTERFACE DEPTH=1,4 VARIABLE=gvar0_a max_depth=1,4
  int i_1_0;
  for (i_1_0 = 0; i_1_0 < 4UL; ++i_1_0) {
    gvar0_a[i_1_0] = gvar0_a[i_1_0];
  }
}

void __merlinwrapper_f2(struct t1 gvar0)
{
  int _i0;
{
    
#pragma ACCEL wrapper VARIABLE=gvar0->a port=gvar0_a data_type=float depth=1,4 max_depth=1,4 io=RW copy=true
  }
  static float __m_gvar0_a[4];
  for (_i0 = 0; _i0 < 1; ++_i0) {
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "gvar0_a", "4 * sizeof(float )");
    
#pragma ACCEL debug fflush(stdout);
    if (gvar0 . a != 0) {
      memcpy((void *)(__m_gvar0_a + _i0 * 4),(const void *)gvar0 . a[_i0],4 * sizeof(float ));
    }
     else {
      printf("Error! Detected null pointer 'gvar0->a' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "gvar0_a");
    
#pragma ACCEL debug fflush(stdout);
  }
  
#pragma ACCEL wrapper VARIABLE=gvar0 port=gvar0 data_type="struct t1"
  f2(__m_gvar0_a);
  for (_i0 = 0; _i0 < 1; ++_i0) {
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "gvar0_a", "4 * sizeof(float )");
    
#pragma ACCEL debug fflush(stdout);
    if (gvar0 . a != 0) {
      memcpy((void *)gvar0 . a[_i0],(const void *)(__m_gvar0_a + _i0 * 4),4 * sizeof(float ));
    }
     else {
      printf("Error! Detected null pointer 'gvar0->a' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "gvar0_a");
    
#pragma ACCEL debug fflush(stdout);
  }
}
