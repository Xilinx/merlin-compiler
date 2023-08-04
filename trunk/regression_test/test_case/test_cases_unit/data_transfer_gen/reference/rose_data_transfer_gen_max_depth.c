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
extern int __merlin_include__GB_stdlib_h_2;
extern int __merlin_include__GB_stdio_h_1;
extern int __merlin_include__GB_string_h_0;

struct t1 
{
  int *a;
}
;
#pragma ACCEL kernel

void f0(int a_a[4])
{
  __merlin_access_range(a_a,0,3);
  
#pragma ACCEL INTERFACE MAX_DEPTH=1,4 VARIABLE=a_a depth=1,4
  int i;
  for (i = 0; i < 4; ++i) {
    a_a[i] = 1;
  }
}

void __merlinwrapper_f0(struct t1 *a)
{
  int _i0;
{
    
#pragma ACCEL wrapper VARIABLE=a->a port=a_a data_type=int depth=1,4 max_depth=1,4 io=RW copy=true
  }
  static int __m_a_a[4];
  for (_i0 = 0; _i0 < 1; ++_i0) {
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "a_a", "4 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (a != 0 && a[_i0] . a != 0) {
      memcpy((void *)(__m_a_a + _i0 * 4),(const void *)a[_i0] . a,4 * sizeof(int ));
    }
     else {
      printf("Error! Detected null pointer 'a->a' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "a_a");
    
#pragma ACCEL debug fflush(stdout);
  }
  
#pragma ACCEL wrapper VARIABLE=a port=a data_type="struct t1"
  f0(__m_a_a);
  for (_i0 = 0; _i0 < 1; ++_i0) {
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "a_a", "4 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (a != 0 && a[_i0] . a != 0) {
      memcpy((void *)a[_i0] . a,(const void *)(__m_a_a + _i0 * 4),4 * sizeof(int ));
    }
     else {
      printf("Error! Detected null pointer 'a->a' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "a_a");
    
#pragma ACCEL debug fflush(stdout);
  }
}
