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
enum em {ONE=0,TWO=1} ;

struct t1 
{
  enum em a[4];
  int size;
}
;
#pragma ACCEL kernel

void f0(enum em a_a[40],int *a_size)
{
  __merlin_access_range(a_size,0,9UL);
  __merlin_access_range(a_a,0,39);
  
#pragma ACCEL INTERFACE DEPTH=a_size MAX_DEPTH=10 VARIABLE=a_size 
  
#pragma ACCEL INTERFACE DEPTH=a_size,4 MAX_DEPTH=10,4 VARIABLE=a_a 
  int i;
  int j;
  for (j = 0; j <  *a_size; ++j) {
    for (i = 0; i < 4; ++i) {
      a_a[j * 4L + i] = ONE;
    }
  }
}

void __merlinwrapper_f0(struct t1 *a)
{
  int _i0;
{
    
#pragma ACCEL wrapper VARIABLE=a->a port=a_a data_type="enum em" depth=a_size,4 max_depth=10,4 io=RW copy=true
    
#pragma ACCEL wrapper VARIABLE=a->size port=a_size data_type=int depth=a_size max_depth=10 io=RO copy=true
  }
  static enum em __m_a_a[40];
  static int __m_a_size[10];
  for (_i0 = 0; _i0 < a -> size; ++_i0) {
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "a_a", "4 * sizeof(enum em )");
    
#pragma ACCEL debug fflush(stdout);
    if (a != 0 && a[_i0] . a != 0) {
      memcpy((void *)(__m_a_a + _i0 * 4),(const void *)a[_i0] . a,4 * sizeof(enum em ));
    }
     else {
      printf("Error! Detected null pointer 'a->a' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "a_a");
    
#pragma ACCEL debug fflush(stdout);
  }
  static int *__ti_a_size;
  posix_memalign((void **)(&__ti_a_size),64,sizeof(int ) * 10);
  for (_i0 = 0; _i0 < a -> size; ++_i0) {
    if (a != 0) {
      __ti_a_size[_i0 * 1] = a[_i0] . size;
    }
     else {
      printf("Error! Detected null pointer 'a->size' for external memory.\n");
      exit(1);
    }
  }
  
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "a_size", "a -> size * 1 * sizeof(int )");
  
#pragma ACCEL debug fflush(stdout);
  memcpy((void *)__m_a_size,(const void *)__ti_a_size,a -> size * 1 * sizeof(int ));
  
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "a_size");
  
#pragma ACCEL debug fflush(stdout);
  
#pragma ACCEL wrapper VARIABLE=a port=a data_type="struct t1"
  f0(__m_a_a,__m_a_size);
  for (_i0 = 0; _i0 < a -> size; ++_i0) {
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "a_a", "4 * sizeof(enum em )");
    
#pragma ACCEL debug fflush(stdout);
    if (a != 0 && a[_i0] . a != 0) {
      memcpy((void *)a[_i0] . a,(const void *)(__m_a_a + _i0 * 4),4 * sizeof(enum em ));
    }
     else {
      printf("Error! Detected null pointer 'a->a' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "a_a");
    
#pragma ACCEL debug fflush(stdout);
  }
}
