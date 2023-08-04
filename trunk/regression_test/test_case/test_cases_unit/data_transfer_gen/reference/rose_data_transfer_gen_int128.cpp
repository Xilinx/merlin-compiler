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
void foo(unsigned __int128 merlin_return_value[1],__int128 a[10],__int128 b[10]);

unsigned __int128 __merlinwrapper_foo(__int128 a[10],__int128 b[10])
{
  unsigned __int128 merlin_return_value[1];
{
    
#pragma ACCEL wrapper VARIABLE=merlin_return_value port=merlin_return_value data_type="unsigned __int128" depth=1 max_depth=1 io=WO copy=true scope_type=return
    
#pragma ACCEL wrapper VARIABLE=a port=a data_type=__int128 depth=10 max_depth=10 io=RO copy=true
    
#pragma ACCEL wrapper VARIABLE=b port=b data_type=__int128 depth=10 max_depth=10 io=RO copy=true
  }
  static unsigned __int128 __m_merlin_return_value[1];
  static __int128 __m_a[10];
  static __int128 __m_b[10];
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "a", "10 * sizeof(__int128 )");
    
#pragma ACCEL debug fflush(stdout);
    if (a != 0) {
      memcpy((void *)(__m_a + 0),(const void *)a,10 * sizeof(__int128 ));
    }
     else {
      printf("Error! Detected null pointer 'a' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "a");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "b", "10 * sizeof(__int128 )");
    
#pragma ACCEL debug fflush(stdout);
    if (b != 0) {
      memcpy((void *)(__m_b + 0),(const void *)b,10 * sizeof(__int128 ));
    }
     else {
      printf("Error! Detected null pointer 'b' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "b");
    
#pragma ACCEL debug fflush(stdout);
  }
  
#pragma ACCEL kernel name="foo"
  foo(__m_merlin_return_value,__m_a,__m_b);
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "merlin_return_value", "1 * sizeof(unsigned __int128 )");
    
#pragma ACCEL debug fflush(stdout);
    if (merlin_return_value != 0) {
      memcpy((void *)merlin_return_value,(const void *)(__m_merlin_return_value + 0),1 * sizeof(unsigned __int128 ));
    }
     else {
      printf("Error! Detected null pointer 'merlin_return_value' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "merlin_return_value");
    
#pragma ACCEL debug fflush(stdout);
  }
  return  *merlin_return_value;
}
static const unsigned __int128 m126 = 18446744073709551615ULL;

void foo(unsigned __int128 merlin_return_value[1],__int128 a[10],__int128 b[10])
{
  
#pragma ACCEL interface variable=b depth=10 max_depth=10
  
#pragma ACCEL interface variable=a depth=10 max_depth=10
  __merlin_access_range(b,0,9UL);
  __merlin_access_range(a,0,9UL);
  __merlin_access_range(merlin_return_value,0,0UL);
  
#pragma ACCEL interface variable=merlin_return_value io=WO scope_type=return max_depth=1 depth=1
  merlin_return_value[0] = m126 + a[2] + b[2];
  return ;
}
