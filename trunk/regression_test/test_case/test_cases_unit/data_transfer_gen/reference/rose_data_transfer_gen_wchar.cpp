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
#pragma ACCEL kernel

void f0(wchar_t *a)
{
  __merlin_access_range(a,0,3UL);
  
#pragma ACCEL interface variable=a depth=4 max_depth=4
  int i;
  for (i = 0; i < 4; ++i) {
    a[i] = 10;
  }
}

void __merlinwrapper_f0(wchar_t *a)
{
{
    
#pragma ACCEL wrapper VARIABLE=a port=a data_type=wchar_t depth=4 max_depth=4 io=WO copy=true
  }
  static wchar_t __m_a[4];
  
#pragma ACCEL wrapper VARIABLE=a port=a data_type=wchar_t depth=4 max_depth=4 io=WO copy=true
  f0(__m_a);
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "a", "4 * sizeof(wchar_t )");
    
#pragma ACCEL debug fflush(stdout);
    if (a != 0) {
      memcpy((void *)a,(const void *)(__m_a + 0),4 * sizeof(wchar_t ));
    }
     else {
      printf("Error! Detected null pointer 'a' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "a");
    
#pragma ACCEL debug fflush(stdout);
  }
}
