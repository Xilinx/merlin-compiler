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
#include <string.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include "merlin_type_define.h"
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_0();
void vec_add_kernel(int a[200000],int b[200000],int c[200000],int d[200000],long e[200000],int f[200000],int g[200000],long h[200000],long i[200000]);
static void __merlin_dummy_extern_int_merlin_include_L_();
#include "__merlinhead_kernel_top.h"
static void __merlin_dummy_extern_int_merlin_include_L_();

void __merlinwrapper_vec_add_kernel(int a[200000],int b[200000],int c[200000],int inc,int d[200000],long e[200000],int f[200000],int g[200000],long h[200000],long i[200000])
{
{
    
#pragma ACCEL wrapper VARIABLE=a port=a data_type=int depth=200000 max_depth=200000 io=RO copy=false
    
#pragma ACCEL wrapper VARIABLE=b port=b data_type=int depth=200000 max_depth=200000 io=RW copy=false
    
#pragma ACCEL wrapper VARIABLE=c port=c data_type=int depth=200000 max_depth=200000 io=RW copy=false
    
#pragma ACCEL wrapper VARIABLE=d port=d data_type=int depth=200000 max_depth=200000 io=RW copy=false
    
#pragma ACCEL wrapper VARIABLE=e port=e data_type=long depth=200000 max_depth=200000 io=RW copy=false
    
#pragma ACCEL wrapper VARIABLE=f port=f data_type=int depth=200000 max_depth=200000 io=RO copy=false
    
#pragma ACCEL wrapper VARIABLE=g port=g data_type=int depth=200000 max_depth=200000 io=RW copy=false
    
#pragma ACCEL wrapper VARIABLE=h port=h data_type=long depth=200000 max_depth=200000 io=RW copy=false
    
#pragma ACCEL wrapper VARIABLE=i port=i data_type=long depth=200000 max_depth=200000 io=RO copy=false
  }
  static int __m_a[200000];
  static int __m_b[200000];
  static int __m_c[200000];
  static int __m_d[200000];
  static long __m_e[200000];
  static int __m_f[200000];
  static int __m_g[200000];
  static long __m_h[200000];
  static long __m_i[200000];
{
/*
    static int __m_a[200000];
    static int __m_b[200000];
    static int __m_c[200000];
    static int __m_d[200000];
    static long __m_e[200000];
    static int __m_f[200000];
    static int __m_g[200000];
    static long __m_h[200000];
    static long __m_i[200000];
*/
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "a", "200000 * sizeof(int )");
      
#pragma ACCEL debug fflush(stdout);
      if (a != 0) {
        memcpy((void *)(__m_a + 0),(const void *)a,200000 * sizeof(int ));
      }
       else {
        printf("Error! Detected null pointer 'a' for external memory.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "a");
      
#pragma ACCEL debug fflush(stdout);
    }
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "b", "200000 * sizeof(int )");
      
#pragma ACCEL debug fflush(stdout);
      if (b != 0) {
        memcpy((void *)(__m_b + 0),(const void *)b,200000 * sizeof(int ));
      }
       else {
        printf("Error! Detected null pointer 'b' for external memory.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "b");
      
#pragma ACCEL debug fflush(stdout);
    }
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "c", "200000 * sizeof(int )");
      
#pragma ACCEL debug fflush(stdout);
      if (c != 0) {
        memcpy((void *)(__m_c + 0),(const void *)c,200000 * sizeof(int ));
      }
       else {
        printf("Error! Detected null pointer 'c' for external memory.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "c");
      
#pragma ACCEL debug fflush(stdout);
    }
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "d", "200000 * sizeof(int )");
      
#pragma ACCEL debug fflush(stdout);
      if (d != 0) {
        memcpy((void *)(__m_d + 0),(const void *)d,200000 * sizeof(int ));
      }
       else {
        printf("Error! Detected null pointer 'd' for external memory.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "d");
      
#pragma ACCEL debug fflush(stdout);
    }
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "e", "200000 * sizeof(long )");
      
#pragma ACCEL debug fflush(stdout);
      if (e != 0) {
        memcpy((void *)(__m_e + 0),(const void *)e,200000 * sizeof(long ));
      }
       else {
        printf("Error! Detected null pointer 'e' for external memory.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "e");
      
#pragma ACCEL debug fflush(stdout);
    }
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "f", "200000 * sizeof(int )");
      
#pragma ACCEL debug fflush(stdout);
      if (f != 0) {
        memcpy((void *)(__m_f + 0),(const void *)f,200000 * sizeof(int ));
      }
       else {
        printf("Error! Detected null pointer 'f' for external memory.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "f");
      
#pragma ACCEL debug fflush(stdout);
    }
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "g", "200000 * sizeof(int )");
      
#pragma ACCEL debug fflush(stdout);
      if (g != 0) {
        memcpy((void *)(__m_g + 0),(const void *)g,200000 * sizeof(int ));
      }
       else {
        printf("Error! Detected null pointer 'g' for external memory.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "g");
      
#pragma ACCEL debug fflush(stdout);
    }
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "h", "200000 * sizeof(long )");
      
#pragma ACCEL debug fflush(stdout);
      if (h != 0) {
        memcpy((void *)(__m_h + 0),(const void *)h,200000 * sizeof(long ));
      }
       else {
        printf("Error! Detected null pointer 'h' for external memory.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "h");
      
#pragma ACCEL debug fflush(stdout);
    }
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "i", "200000 * sizeof(long )");
      
#pragma ACCEL debug fflush(stdout);
      if (i != 0) {
        memcpy((void *)(__m_i + 0),(const void *)i,200000 * sizeof(long ));
      }
       else {
        printf("Error! Detected null pointer 'i' for external memory.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "i");
      
#pragma ACCEL debug fflush(stdout);
    }
  }
  
#pragma ACCEL kernel
  vec_add_kernel(__m_a,__m_b,__m_c,__m_d,__m_e,__m_f,__m_g,__m_h,__m_i);
{
/*
    static int __m_a[200000];
    static int __m_b[200000];
    static int __m_c[200000];
    static int __m_d[200000];
    static long __m_e[200000];
    static int __m_f[200000];
    static int __m_g[200000];
    static long __m_h[200000];
    static long __m_i[200000];
*/
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "b", "200000 * sizeof(int )");
      
#pragma ACCEL debug fflush(stdout);
      if (b != 0) {
        memcpy((void *)b,(const void *)(__m_b + 0),200000 * sizeof(int ));
      }
       else {
        printf("Error! Detected null pointer 'b' for external memory.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "b");
      
#pragma ACCEL debug fflush(stdout);
    }
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "c", "200000 * sizeof(int )");
      
#pragma ACCEL debug fflush(stdout);
      if (c != 0) {
        memcpy((void *)c,(const void *)(__m_c + 0),200000 * sizeof(int ));
      }
       else {
        printf("Error! Detected null pointer 'c' for external memory.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "c");
      
#pragma ACCEL debug fflush(stdout);
    }
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "d", "200000 * sizeof(int )");
      
#pragma ACCEL debug fflush(stdout);
      if (d != 0) {
        memcpy((void *)d,(const void *)(__m_d + 0),200000 * sizeof(int ));
      }
       else {
        printf("Error! Detected null pointer 'd' for external memory.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "d");
      
#pragma ACCEL debug fflush(stdout);
    }
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "e", "200000 * sizeof(long )");
      
#pragma ACCEL debug fflush(stdout);
      if (e != 0) {
        memcpy((void *)e,(const void *)(__m_e + 0),200000 * sizeof(long ));
      }
       else {
        printf("Error! Detected null pointer 'e' for external memory.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "e");
      
#pragma ACCEL debug fflush(stdout);
    }
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "g", "200000 * sizeof(int )");
      
#pragma ACCEL debug fflush(stdout);
      if (g != 0) {
        memcpy((void *)g,(const void *)(__m_g + 0),200000 * sizeof(int ));
      }
       else {
        printf("Error! Detected null pointer 'g' for external memory.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "g");
      
#pragma ACCEL debug fflush(stdout);
    }
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "h", "200000 * sizeof(long )");
      
#pragma ACCEL debug fflush(stdout);
      if (h != 0) {
        memcpy((void *)h,(const void *)(__m_h + 0),200000 * sizeof(long ));
      }
       else {
        printf("Error! Detected null pointer 'h' for external memory.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "h");
      
#pragma ACCEL debug fflush(stdout);
    }
  }
}

void __merlin_vec_add_kernel(int a[200000],int b[200000],int c[200000],int inc,int d[200000],long e[200000],int f[200000],int g[200000],long h[200000],long i[200000])
{
  
#pragma ACCEL string __merlin_check_opencl();
  
#pragma ACCEL string __merlin_init_vec_add_kernel();
  __merlinwrapper_vec_add_kernel(a,b,c,inc,d,e,f,g,h,i);
  
#pragma ACCEL string __merlin_release_vec_add_kernel();
}

void __merlin_write_buffer_vec_add_kernel(int a[200000],int b[200000],int c[200000],int inc,int d[200000],long e[200000],int f[200000],int g[200000],long h[200000],long i[200000])
{
  static int __m_a[200000];
  static int __m_b[200000];
  static int __m_c[200000];
  static int __m_d[200000];
  static long __m_e[200000];
  static int __m_f[200000];
  static int __m_g[200000];
  static long __m_h[200000];
  static long __m_i[200000];
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "a", "200000 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (a != 0) {
      memcpy((void *)(__m_a + 0),(const void *)a,200000 * sizeof(int ));
    }
     else {
      printf("Error! Detected null pointer 'a' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "a");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "b", "200000 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (b != 0) {
      memcpy((void *)(__m_b + 0),(const void *)b,200000 * sizeof(int ));
    }
     else {
      printf("Error! Detected null pointer 'b' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "b");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "c", "200000 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (c != 0) {
      memcpy((void *)(__m_c + 0),(const void *)c,200000 * sizeof(int ));
    }
     else {
      printf("Error! Detected null pointer 'c' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "c");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "d", "200000 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (d != 0) {
      memcpy((void *)(__m_d + 0),(const void *)d,200000 * sizeof(int ));
    }
     else {
      printf("Error! Detected null pointer 'd' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "d");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "e", "200000 * sizeof(long )");
    
#pragma ACCEL debug fflush(stdout);
    if (e != 0) {
      memcpy((void *)(__m_e + 0),(const void *)e,200000 * sizeof(long ));
    }
     else {
      printf("Error! Detected null pointer 'e' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "e");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "f", "200000 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (f != 0) {
      memcpy((void *)(__m_f + 0),(const void *)f,200000 * sizeof(int ));
    }
     else {
      printf("Error! Detected null pointer 'f' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "f");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "g", "200000 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (g != 0) {
      memcpy((void *)(__m_g + 0),(const void *)g,200000 * sizeof(int ));
    }
     else {
      printf("Error! Detected null pointer 'g' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "g");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "h", "200000 * sizeof(long )");
    
#pragma ACCEL debug fflush(stdout);
    if (h != 0) {
      memcpy((void *)(__m_h + 0),(const void *)h,200000 * sizeof(long ));
    }
     else {
      printf("Error! Detected null pointer 'h' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "h");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "i", "200000 * sizeof(long )");
    
#pragma ACCEL debug fflush(stdout);
    if (i != 0) {
      memcpy((void *)(__m_i + 0),(const void *)i,200000 * sizeof(long ));
    }
     else {
      printf("Error! Detected null pointer 'i' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "i");
    
#pragma ACCEL debug fflush(stdout);
  }
}

void __merlin_read_buffer_vec_add_kernel(int a[200000],int b[200000],int c[200000],int inc,int d[200000],long e[200000],int f[200000],int g[200000],long h[200000],long i[200000])
{
  static int __m_a[200000];
  static int __m_b[200000];
  static int __m_c[200000];
  static int __m_d[200000];
  static long __m_e[200000];
  static int __m_f[200000];
  static int __m_g[200000];
  static long __m_h[200000];
  static long __m_i[200000];
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "b", "200000 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (b != 0) {
      memcpy((void *)b,(const void *)(__m_b + 0),200000 * sizeof(int ));
    }
     else {
      printf("Error! Detected null pointer 'b' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "b");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "c", "200000 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (c != 0) {
      memcpy((void *)c,(const void *)(__m_c + 0),200000 * sizeof(int ));
    }
     else {
      printf("Error! Detected null pointer 'c' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "c");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "d", "200000 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (d != 0) {
      memcpy((void *)d,(const void *)(__m_d + 0),200000 * sizeof(int ));
    }
     else {
      printf("Error! Detected null pointer 'd' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "d");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "e", "200000 * sizeof(long )");
    
#pragma ACCEL debug fflush(stdout);
    if (e != 0) {
      memcpy((void *)e,(const void *)(__m_e + 0),200000 * sizeof(long ));
    }
     else {
      printf("Error! Detected null pointer 'e' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "e");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "g", "200000 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (g != 0) {
      memcpy((void *)g,(const void *)(__m_g + 0),200000 * sizeof(int ));
    }
     else {
      printf("Error! Detected null pointer 'g' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "g");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "h", "200000 * sizeof(long )");
    
#pragma ACCEL debug fflush(stdout);
    if (h != 0) {
      memcpy((void *)h,(const void *)(__m_h + 0),200000 * sizeof(long ));
    }
     else {
      printf("Error! Detected null pointer 'h' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "h");
    
#pragma ACCEL debug fflush(stdout);
  }
}

void __merlin_execute_vec_add_kernel(int a[200000],int b[200000],int c[200000],int inc,int d[200000],long e[200000],int f[200000],int g[200000],long h[200000],long i[200000])
{
}
