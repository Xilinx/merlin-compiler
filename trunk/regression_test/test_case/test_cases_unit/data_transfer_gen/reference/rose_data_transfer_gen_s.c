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
  float a[4];
}
;

struct t2 
{
  float a;
  float b[4];
  float c[8];
}
;
struct t1 gvar0;
struct t1 gvar1[100];
double gvar2;
double gvar3[4];
struct t2 gvar4;
struct t2 gvar5[4];
#pragma ACCEL kernel

void f0(float gvar0_a[4],float gvar1_a[400],double *gvar2,double gvar3[4],float *gvar4_a,float gvar4_b[4],float gvar4_c[8],float gvar5_a[4],float gvar5_b[16],float gvar5_c[32])
{
  __merlin_access_range(gvar5_c,0,31);
  __merlin_access_range(gvar5_b,0,15);
  __merlin_access_range(gvar5_a,0,3);
  __merlin_access_range(gvar4_c,0,7);
  __merlin_access_range(gvar4_b,0,3);
  __merlin_access_range(gvar4_a,0,0UL);
  __merlin_access_range(gvar3,0,3);
  __merlin_access_range(gvar2,0,0UL);
  __merlin_access_range(gvar1_a,0,399);
  __merlin_access_range(gvar0_a,0,3);
  
#pragma ACCEL INTERFACE DEPTH=1,4,8 SCOPE_TYPE=global VARIABLE=gvar5_c max_depth=1,4,8
  
#pragma ACCEL INTERFACE DEPTH=1,4,4 SCOPE_TYPE=global VARIABLE=gvar5_b max_depth=1,4,4
  
#pragma ACCEL INTERFACE DEPTH=1,4 SCOPE_TYPE=global VARIABLE=gvar5_a max_depth=1,4
  
#pragma ACCEL INTERFACE DEPTH=1,8 HAS_ADDROP=1 SCOPE_TYPE=global VARIABLE=gvar4_c max_depth=1,8
  
#pragma ACCEL INTERFACE DEPTH=1,4 HAS_ADDROP=1 SCOPE_TYPE=global VARIABLE=gvar4_b max_depth=1,4
  
#pragma ACCEL INTERFACE DEPTH=1 HAS_ADDROP=1 SCOPE_TYPE=global VARIABLE=gvar4_a max_depth=1
  
#pragma ACCEL INTERFACE DEPTH=1,100,4 SCOPE_TYPE=global VARIABLE=gvar1_a max_depth=1,100,4
  
#pragma ACCEL INTERFACE DEPTH=1,4 HAS_ADDROP=1 SCOPE_TYPE=global VARIABLE=gvar0_a max_depth=1,4
  int i_3_0;
  int i_2_0;
  int i_1_0;
  
#pragma ACCEL interface variable=gvar2 has_addrop=1 scope_type=global depth=1 max_depth=1
  
#pragma ACCEL interface variable=gvar3 scope_type=global depth=1,4 max_depth=1,4
  int i;
  for (i_1_0 = 0; i_1_0 < 4UL; ++i_1_0) {
    gvar0_a[i_1_0] = gvar0_a[i_1_0];
  }
  for (i = 0; i < 100; ++i) {
    int i_1_0_0;
    for (i_1_0_0 = 0; i_1_0_0 < 4UL; ++i_1_0_0) {
      gvar1_a[i * 4L + i_1_0_0] = gvar1_a[i * 4L + i_1_0_0];
    }
  }
   *gvar2 =  *gvar2;
  for (i = 0; i < 4; ++i) 
    gvar3[i] = gvar3[i];
   *gvar4_a =  *gvar4_a;
  for (i_2_0 = 0; i_2_0 < 4UL; ++i_2_0) {
    gvar4_b[i_2_0] = gvar4_b[i_2_0];
  }
  for (i_3_0 = 0; i_3_0 < 8UL; ++i_3_0) {
    gvar4_c[i_3_0] = gvar4_c[i_3_0];
  }
  for (i = 0; i < 4; ++i) {
    int i_3_0_0;
    int i_2_0_0;
    gvar5_a[i] = gvar5_a[i];
    for (i_2_0_0 = 0; i_2_0_0 < 4UL; ++i_2_0_0) {
      gvar5_b[i * 4L + i_2_0_0] = gvar5_b[i * 4L + i_2_0_0];
    }
    for (i_3_0_0 = 0; i_3_0_0 < 8UL; ++i_3_0_0) {
      gvar5_c[i * 8L + i_3_0_0] = gvar5_c[i * 8L + i_3_0_0];
    }
  }
}

void __merlinwrapper_f0()
{
  int _i0;
{
    
#pragma ACCEL wrapper VARIABLE=gvar0->a port=gvar0_a data_type=float depth=4 max_depth=4 io=RW copy=true scope_type=global
    
#pragma ACCEL wrapper VARIABLE=gvar1->a port=gvar1_a data_type=float depth=100,4 max_depth=100,4 io=RW copy=true scope_type=global
    
#pragma ACCEL wrapper VARIABLE=gvar2 port=gvar2 data_type=double depth=1 max_depth=1 io=RW copy=true scope_type=global
    
#pragma ACCEL wrapper VARIABLE=gvar3 port=gvar3 data_type=double depth=4 max_depth=4 io=RW copy=true scope_type=global
    
#pragma ACCEL wrapper VARIABLE=gvar4->a port=gvar4_a data_type=float depth=1 max_depth=1 io=RW copy=true scope_type=global
    
#pragma ACCEL wrapper VARIABLE=gvar4->b port=gvar4_b data_type=float depth=4 max_depth=4 io=RW copy=true scope_type=global
    
#pragma ACCEL wrapper VARIABLE=gvar4->c port=gvar4_c data_type=float depth=8 max_depth=8 io=RW copy=true scope_type=global
    
#pragma ACCEL wrapper VARIABLE=gvar5->a port=gvar5_a data_type=float depth=4 max_depth=4 io=RW copy=true scope_type=global
    
#pragma ACCEL wrapper VARIABLE=gvar5->b port=gvar5_b data_type=float depth=4,4 max_depth=4,4 io=RW copy=true scope_type=global
    
#pragma ACCEL wrapper VARIABLE=gvar5->c port=gvar5_c data_type=float depth=4,8 max_depth=4,8 io=RW copy=true scope_type=global
  }
  static float __m_gvar0_a[4];
  static float __m_gvar1_a[400];
  static double __m_gvar2[1];
  static double __m_gvar3[4];
  static float __m_gvar4_a[1];
  static float __m_gvar4_b[4];
  static float __m_gvar4_c[8];
  static float __m_gvar5_a[4];
  static float __m_gvar5_b[16];
  static float __m_gvar5_c[32];
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "gvar0_a", "4 * sizeof(float )");
    
#pragma ACCEL debug fflush(stdout);
    if (gvar0 . a != 0) {
      memcpy((void *)(__m_gvar0_a + 0),(const void *)gvar0 . a,4 * sizeof(float ));
    }
     else {
      printf("Error! Detected null pointer 'gvar0->a' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "gvar0_a");
    
#pragma ACCEL debug fflush(stdout);
  }
  for (_i0 = 0; _i0 < 100; ++_i0) {
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "gvar1_a", "4 * sizeof(float )");
    
#pragma ACCEL debug fflush(stdout);
    if (gvar1 != 0 && gvar1[_i0] . a != 0) {
      memcpy((void *)(__m_gvar1_a + _i0 * 4),(const void *)gvar1[_i0] . a,4 * sizeof(float ));
    }
     else {
      printf("Error! Detected null pointer 'gvar1->a' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "gvar1_a");
    
#pragma ACCEL debug fflush(stdout);
  }
  static double *__ti_gvar2;
  posix_memalign((void **)(&__ti_gvar2),64,sizeof(double ) * 1);
  for (_i0 = 0; _i0 < 1; ++_i0) {
    __ti_gvar2[_i0 * 1] = gvar2;
  }
  
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "gvar2", "1 * 1 * sizeof(double )");
  
#pragma ACCEL debug fflush(stdout);
  memcpy((void *)__m_gvar2,(const void *)__ti_gvar2,1 * 1 * sizeof(double ));
  
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "gvar2");
  
#pragma ACCEL debug fflush(stdout);
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "gvar3", "4 * sizeof(double )");
    
#pragma ACCEL debug fflush(stdout);
    if (gvar3 != 0) {
      memcpy((void *)(__m_gvar3 + 0),(const void *)gvar3,4 * sizeof(double ));
    }
     else {
      printf("Error! Detected null pointer 'gvar3' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "gvar3");
    
#pragma ACCEL debug fflush(stdout);
  }
  static float *__ti_gvar4_a;
  posix_memalign((void **)(&__ti_gvar4_a),64,sizeof(float ) * 1);
  for (_i0 = 0; _i0 < 1; ++_i0) {
    __ti_gvar4_a[_i0 * 1] = gvar4 . a;
  }
  
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "gvar4_a", "1 * 1 * sizeof(float )");
  
#pragma ACCEL debug fflush(stdout);
  memcpy((void *)__m_gvar4_a,(const void *)__ti_gvar4_a,1 * 1 * sizeof(float ));
  
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "gvar4_a");
  
#pragma ACCEL debug fflush(stdout);
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "gvar4_b", "4 * sizeof(float )");
    
#pragma ACCEL debug fflush(stdout);
    if (gvar4 . b != 0) {
      memcpy((void *)(__m_gvar4_b + 0),(const void *)gvar4 . b,4 * sizeof(float ));
    }
     else {
      printf("Error! Detected null pointer 'gvar4->b' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "gvar4_b");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "gvar4_c", "8 * sizeof(float )");
    
#pragma ACCEL debug fflush(stdout);
    if (gvar4 . c != 0) {
      memcpy((void *)(__m_gvar4_c + 0),(const void *)gvar4 . c,8 * sizeof(float ));
    }
     else {
      printf("Error! Detected null pointer 'gvar4->c' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "gvar4_c");
    
#pragma ACCEL debug fflush(stdout);
  }
  static float *__ti_gvar5_a;
  posix_memalign((void **)(&__ti_gvar5_a),64,sizeof(float ) * 4);
  for (_i0 = 0; _i0 < 4; ++_i0) {
    if (gvar5 != 0) {
      __ti_gvar5_a[_i0 * 1] = gvar5[_i0] . a;
    }
     else {
      printf("Error! Detected null pointer 'gvar5->a' for external memory.\n");
      exit(1);
    }
  }
  
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "gvar5_a", "4 * 1 * sizeof(float )");
  
#pragma ACCEL debug fflush(stdout);
  memcpy((void *)__m_gvar5_a,(const void *)__ti_gvar5_a,4 * 1 * sizeof(float ));
  
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "gvar5_a");
  
#pragma ACCEL debug fflush(stdout);
  for (_i0 = 0; _i0 < 4; ++_i0) {
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "gvar5_b", "4 * sizeof(float )");
    
#pragma ACCEL debug fflush(stdout);
    if (gvar5 != 0 && gvar5[_i0] . b != 0) {
      memcpy((void *)(__m_gvar5_b + _i0 * 4),(const void *)gvar5[_i0] . b,4 * sizeof(float ));
    }
     else {
      printf("Error! Detected null pointer 'gvar5->b' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "gvar5_b");
    
#pragma ACCEL debug fflush(stdout);
  }
  for (_i0 = 0; _i0 < 4; ++_i0) {
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "gvar5_c", "8 * sizeof(float )");
    
#pragma ACCEL debug fflush(stdout);
    if (gvar5 != 0 && gvar5[_i0] . c != 0) {
      memcpy((void *)(__m_gvar5_c + _i0 * 8),(const void *)gvar5[_i0] . c,8 * sizeof(float ));
    }
     else {
      printf("Error! Detected null pointer 'gvar5->c' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "gvar5_c");
    
#pragma ACCEL debug fflush(stdout);
  }
  
#pragma ACCEL wrapper VARIABLE=gvar5 port=gvar5 data_type="struct t2"
  f0(__m_gvar0_a,__m_gvar1_a,__m_gvar2,__m_gvar3,__m_gvar4_a,__m_gvar4_b,__m_gvar4_c,__m_gvar5_a,__m_gvar5_b,__m_gvar5_c);
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "gvar0_a", "4 * sizeof(float )");
    
#pragma ACCEL debug fflush(stdout);
    if (gvar0 . a != 0) {
      memcpy((void *)gvar0 . a,(const void *)(__m_gvar0_a + 0),4 * sizeof(float ));
    }
     else {
      printf("Error! Detected null pointer 'gvar0->a' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "gvar0_a");
    
#pragma ACCEL debug fflush(stdout);
  }
  for (_i0 = 0; _i0 < 100; ++_i0) {
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "gvar1_a", "4 * sizeof(float )");
    
#pragma ACCEL debug fflush(stdout);
    if (gvar1 != 0 && gvar1[_i0] . a != 0) {
      memcpy((void *)gvar1[_i0] . a,(const void *)(__m_gvar1_a + _i0 * 4),4 * sizeof(float ));
    }
     else {
      printf("Error! Detected null pointer 'gvar1->a' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "gvar1_a");
    
#pragma ACCEL debug fflush(stdout);
  }
  static double *__to_gvar2;
  posix_memalign((void **)(&__to_gvar2),64,sizeof(double ) * 1);
  
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "gvar2", "1 * 1 * sizeof(double )");
  
#pragma ACCEL debug fflush(stdout);
  memcpy((void *)__to_gvar2,(const void *)__m_gvar2,1 * 1 * sizeof(double ));
  
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "gvar2");
  
#pragma ACCEL debug fflush(stdout);
  for (_i0 = 0; _i0 < 1; ++_i0) {
    gvar2 = __to_gvar2[_i0 * 1];
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "gvar3", "4 * sizeof(double )");
    
#pragma ACCEL debug fflush(stdout);
    if (gvar3 != 0) {
      memcpy((void *)gvar3,(const void *)(__m_gvar3 + 0),4 * sizeof(double ));
    }
     else {
      printf("Error! Detected null pointer 'gvar3' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "gvar3");
    
#pragma ACCEL debug fflush(stdout);
  }
  static float *__to_gvar4_a;
  posix_memalign((void **)(&__to_gvar4_a),64,sizeof(float ) * 1);
  
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "gvar4_a", "1 * 1 * sizeof(float )");
  
#pragma ACCEL debug fflush(stdout);
  memcpy((void *)__to_gvar4_a,(const void *)__m_gvar4_a,1 * 1 * sizeof(float ));
  
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "gvar4_a");
  
#pragma ACCEL debug fflush(stdout);
  for (_i0 = 0; _i0 < 1; ++_i0) {
    gvar4 . a = __to_gvar4_a[_i0 * 1];
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "gvar4_b", "4 * sizeof(float )");
    
#pragma ACCEL debug fflush(stdout);
    if (gvar4 . b != 0) {
      memcpy((void *)gvar4 . b,(const void *)(__m_gvar4_b + 0),4 * sizeof(float ));
    }
     else {
      printf("Error! Detected null pointer 'gvar4->b' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "gvar4_b");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "gvar4_c", "8 * sizeof(float )");
    
#pragma ACCEL debug fflush(stdout);
    if (gvar4 . c != 0) {
      memcpy((void *)gvar4 . c,(const void *)(__m_gvar4_c + 0),8 * sizeof(float ));
    }
     else {
      printf("Error! Detected null pointer 'gvar4->c' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "gvar4_c");
    
#pragma ACCEL debug fflush(stdout);
  }
  static float *__to_gvar5_a;
  posix_memalign((void **)(&__to_gvar5_a),64,sizeof(float ) * 4);
  
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "gvar5_a", "4 * 1 * sizeof(float )");
  
#pragma ACCEL debug fflush(stdout);
  memcpy((void *)__to_gvar5_a,(const void *)__m_gvar5_a,4 * 1 * sizeof(float ));
  
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "gvar5_a");
  
#pragma ACCEL debug fflush(stdout);
  for (_i0 = 0; _i0 < 4; ++_i0) {
    if (gvar5 != 0) {
      gvar5[_i0] . a = __to_gvar5_a[_i0 * 1];
    }
     else {
      printf("Error! Detected null pointer 'gvar5->a' for external memory.\n");
      exit(1);
    }
  }
  for (_i0 = 0; _i0 < 4; ++_i0) {
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "gvar5_b", "4 * sizeof(float )");
    
#pragma ACCEL debug fflush(stdout);
    if (gvar5 != 0 && gvar5[_i0] . b != 0) {
      memcpy((void *)gvar5[_i0] . b,(const void *)(__m_gvar5_b + _i0 * 4),4 * sizeof(float ));
    }
     else {
      printf("Error! Detected null pointer 'gvar5->b' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "gvar5_b");
    
#pragma ACCEL debug fflush(stdout);
  }
  for (_i0 = 0; _i0 < 4; ++_i0) {
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "gvar5_c", "8 * sizeof(float )");
    
#pragma ACCEL debug fflush(stdout);
    if (gvar5 != 0 && gvar5[_i0] . c != 0) {
      memcpy((void *)gvar5[_i0] . c,(const void *)(__m_gvar5_c + _i0 * 8),8 * sizeof(float ));
    }
     else {
      printf("Error! Detected null pointer 'gvar5->c' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "gvar5_c");
    
#pragma ACCEL debug fflush(stdout);
  }
}
