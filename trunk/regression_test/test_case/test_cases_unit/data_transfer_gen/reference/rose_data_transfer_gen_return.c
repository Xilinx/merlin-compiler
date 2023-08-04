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

void f0(int *merlin_return)
{
  __merlin_access_range(merlin_return,0,0UL);
  
#pragma ACCEL interface variable=merlin_return depth=1 max_depth=1
   *merlin_return = 0;
}

void __merlinwrapper_f0(int *merlin_return)
{
{
    
#pragma ACCEL wrapper VARIABLE=merlin_return port=merlin_return data_type=int depth=1 max_depth=1 io=WO copy=true
  }
  static int __m_merlin_return[1];
  
#pragma ACCEL wrapper VARIABLE=merlin_return port=merlin_return data_type=int depth=1 max_depth=1 io=WO copy=true
  f0(__m_merlin_return);
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "merlin_return", "1 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (merlin_return != 0) {
      memcpy((void *)merlin_return,(const void *)(__m_merlin_return + 0),1 * sizeof(int ));
    }
     else {
      printf("Error! Detected null pointer 'merlin_return' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "merlin_return");
    
#pragma ACCEL debug fflush(stdout);
  }
}
