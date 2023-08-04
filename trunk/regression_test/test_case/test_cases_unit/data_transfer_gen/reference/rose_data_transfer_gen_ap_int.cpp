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
#include<ap_int.h>
extern int __merlin_include__GB_stdlib_h_2;
extern int __merlin_include__GB_stdio_h_1;
extern int __merlin_include__GB_string_h_0;
struct t1 ;

struct t1 
{
  class ap_int< 10 > a[4];
  class ap_fixed< 10 , 2 , AP_TRN , AP_WRAP , 0 > b[4];
}
;
#pragma ACCEL kernel

void f0(class ap_int< 10 > a_a[4],class ap_fixed< 10 , 2 , AP_TRN , AP_WRAP , 0 > a_b[4])
{
  __merlin_access_range(a_b,0,3);
  __merlin_access_range(a_a,0,3);
  
#pragma ACCEL INTERFACE DEPTH=1,4 VARIABLE=a_b max_depth=1,4
  
#pragma ACCEL INTERFACE DEPTH=1,4 VARIABLE=a_a max_depth=1,4
  int i;
  for (i = 0; i < 4; ++i) {
    a_a[i] = 1;
    a_b[i] = 0.1;
  }
}

void __merlinwrapper_f0(struct t1 *a)
{
  int _i0;
{
    
#pragma ACCEL wrapper VARIABLE=a->a port=a_a data_type="class ap_int < 10 > " depth=1,4 max_depth=1,4 io=RW copy=true
    
#pragma ACCEL wrapper VARIABLE=a->b port=a_b data_type="class ap_fixed < 10 , 2 , AP_TRN , AP_WRAP , 0 > " depth=1,4 max_depth=1,4 io=RW copy=true
  }
  static class ap_int< 10 > __m_a_a[4];
  static class ap_fixed< 10 , 2 , AP_TRN , AP_WRAP , 0 > __m_a_b[4];
  for (_i0 = 0; _i0 < 1; ++_i0) {
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "a_a", "4 * sizeof(class ap_int< 10 > )");
    
#pragma ACCEL debug fflush(stdout);
    if (a != 0 && a[_i0] . a != 0) {
      memcpy((void *)(__m_a_a + _i0 * 4),(const void *)a[_i0] . a,4 * sizeof(class ap_int< 10 > ));
    }
     else {
      printf("Error! Detected null pointer 'a->a' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "a_a");
    
#pragma ACCEL debug fflush(stdout);
  }
  for (_i0 = 0; _i0 < 1; ++_i0) {
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "a_b", "4 * sizeof(class ap_fixed< 10 , 2 , AP_TRN , AP_WRAP , 0 > )");
    
#pragma ACCEL debug fflush(stdout);
    if (a != 0 && a[_i0] . b != 0) {
      memcpy((void *)(__m_a_b + _i0 * 4),(const void *)a[_i0] . b,4 * sizeof(class ap_fixed< 10 , 2 , AP_TRN , AP_WRAP , 0 > ));
    }
     else {
      printf("Error! Detected null pointer 'a->b' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "a_b");
    
#pragma ACCEL debug fflush(stdout);
  }
  
#pragma ACCEL wrapper VARIABLE=a port=a data_type="struct t1"
  f0(__m_a_a,__m_a_b);
  for (_i0 = 0; _i0 < 1; ++_i0) {
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "a_a", "4 * sizeof(class ap_int< 10 > )");
    
#pragma ACCEL debug fflush(stdout);
    if (a != 0 && a[_i0] . a != 0) {
      memcpy((void *)a[_i0] . a,(const void *)(__m_a_a + _i0 * 4),4 * sizeof(class ap_int< 10 > ));
    }
     else {
      printf("Error! Detected null pointer 'a->a' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "a_a");
    
#pragma ACCEL debug fflush(stdout);
  }
  for (_i0 = 0; _i0 < 1; ++_i0) {
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "a_b", "4 * sizeof(class ap_fixed< 10 , 2 , AP_TRN , AP_WRAP , 0 > )");
    
#pragma ACCEL debug fflush(stdout);
    if (a != 0 && a[_i0] . b != 0) {
      memcpy((void *)a[_i0] . b,(const void *)(__m_a_b + _i0 * 4),4 * sizeof(class ap_fixed< 10 , 2 , AP_TRN , AP_WRAP , 0 > ));
    }
     else {
      printf("Error! Detected null pointer 'a->b' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "a_b");
    
#pragma ACCEL debug fflush(stdout);
  }
}
