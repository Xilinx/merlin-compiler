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
static void __merlin_dummy_431();
void ellpack_kernel(double nzval[(1 << 12) * (1 << 9)],int cols[(1 << 12) * (1 << 9)],double vec[1 << 12],double out[1 << 12]);
static void __merlin_dummy_extern_int_merlin_include_L_();
#include "__merlinhead_kernel_top.h"
static void __merlin_dummy_extern_int_merlin_include_L_();

void __merlinwrapper_ellpack_kernel(double nzval[(1 << 12) * (1 << 9)],int cols[(1 << 12) * (1 << 9)],double vec[1 << 12],double out[1 << 12])
{
{
    
#pragma ACCEL wrapper variable=nzval port=nzval depth=2097152 max_depth=2097152 data_type=double io=RO copy=false
    
#pragma ACCEL wrapper variable=cols port=cols depth=2097152 max_depth=2097152 data_type=int io=RO copy=false
    
#pragma ACCEL wrapper variable=vec port=vec depth=4096 max_depth=4096 data_type=double io=RO copy=false
    
#pragma ACCEL wrapper variable=out depth=4096 max_depth=4096 data_type=double io=RW copy=false port=merlin_out
  }
  static double __m_nzval[2097152];
  static int __m_cols[2097152];
  static double __m_vec[4096];
  static double __m_merlin_out[4096];
{
/*
    static double __m_nzval[2097152];
    static int __m_cols[2097152];
    static double __m_vec[4096];
    static double __m_merlin_out[4096];
*/
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "nzval", "2097152 * sizeof(double )");
      
#pragma ACCEL debug fflush(stdout);
      if (nzval != 0) {
        memcpy((void *)(__m_nzval + 0),(const void *)nzval,2097152 * sizeof(double ));
      }
       else {
        printf("Error! The external memory pointed by 'nzval' is invalid.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "nzval");
      
#pragma ACCEL debug fflush(stdout);
    }
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "cols", "2097152 * sizeof(int )");
      
#pragma ACCEL debug fflush(stdout);
      if (cols != 0) {
        memcpy((void *)(__m_cols + 0),(const void *)cols,2097152 * sizeof(int ));
      }
       else {
        printf("Error! The external memory pointed by 'cols' is invalid.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "cols");
      
#pragma ACCEL debug fflush(stdout);
    }
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "vec", "4096 * sizeof(double )");
      
#pragma ACCEL debug fflush(stdout);
      if (vec != 0) {
        memcpy((void *)(__m_vec + 0),(const void *)vec,4096 * sizeof(double ));
      }
       else {
        printf("Error! The external memory pointed by 'vec' is invalid.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "vec");
      
#pragma ACCEL debug fflush(stdout);
    }
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "merlin_out", "4096 * sizeof(double )");
      
#pragma ACCEL debug fflush(stdout);
      if (out != 0) {
        memcpy((void *)(__m_merlin_out + 0),(const void *)out,4096 * sizeof(double ));
      }
       else {
        printf("Error! The external memory pointed by 'out' is invalid.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "merlin_out");
      
#pragma ACCEL debug fflush(stdout);
    }
  }
  
#pragma ACCEL kernel
  ellpack_kernel(__m_nzval,__m_cols,__m_vec,__m_merlin_out);
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "merlin_out", "4096 * sizeof(double )");
    
#pragma ACCEL debug fflush(stdout);
    if (out != 0) {
      memcpy((void *)out,(const void *)(__m_merlin_out + 0),4096 * sizeof(double ));
    }
     else {
      printf("Error! The external memory pointed by 'out' is invalid.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "merlin_out");
    
#pragma ACCEL debug fflush(stdout);
  }
}

void __merlin_ellpack_kernel(double nzval[(1 << 12) * (1 << 9)],int cols[(1 << 12) * (1 << 9)],double vec[1 << 12],double out[1 << 12])
{
  
#pragma ACCEL string __merlin_check_opencl();
  
#pragma ACCEL string __merlin_init_ellpack_kernel();
  __merlinwrapper_ellpack_kernel(nzval,cols,vec,out);
  
#pragma ACCEL string __merlin_release_ellpack_kernel();
}

void __merlin_write_buffer_ellpack_kernel(double nzval[(1 << 12) * (1 << 9)],int cols[(1 << 12) * (1 << 9)],double vec[1 << 12],double out[1 << 12])
{
  static double __m_nzval[2097152];
  static int __m_cols[2097152];
  static double __m_vec[4096];
  static double __m_merlin_out[4096];
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "nzval", "2097152 * sizeof(double )");
    
#pragma ACCEL debug fflush(stdout);
    if (nzval != 0) {
      memcpy((void *)(__m_nzval + 0),(const void *)nzval,2097152 * sizeof(double ));
    }
     else {
      printf("Error! The external memory pointed by 'nzval' is invalid.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "nzval");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "cols", "2097152 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (cols != 0) {
      memcpy((void *)(__m_cols + 0),(const void *)cols,2097152 * sizeof(int ));
    }
     else {
      printf("Error! The external memory pointed by 'cols' is invalid.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "cols");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "vec", "4096 * sizeof(double )");
    
#pragma ACCEL debug fflush(stdout);
    if (vec != 0) {
      memcpy((void *)(__m_vec + 0),(const void *)vec,4096 * sizeof(double ));
    }
     else {
      printf("Error! The external memory pointed by 'vec' is invalid.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "vec");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "merlin_out", "4096 * sizeof(double )");
    
#pragma ACCEL debug fflush(stdout);
    if (out != 0) {
      memcpy((void *)(__m_merlin_out + 0),(const void *)out,4096 * sizeof(double ));
    }
     else {
      printf("Error! The external memory pointed by 'out' is invalid.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "merlin_out");
    
#pragma ACCEL debug fflush(stdout);
  }
}

void __merlin_read_buffer_ellpack_kernel(double nzval[(1 << 12) * (1 << 9)],int cols[(1 << 12) * (1 << 9)],double vec[1 << 12],double out[1 << 12])
{
  static double __m_nzval[2097152];
  static int __m_cols[2097152];
  static double __m_vec[4096];
  static double __m_merlin_out[4096];
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "merlin_out", "4096 * sizeof(double )");
    
#pragma ACCEL debug fflush(stdout);
    if (out != 0) {
      memcpy((void *)out,(const void *)(__m_merlin_out + 0),4096 * sizeof(double ));
    }
     else {
      printf("Error! The external memory pointed by 'out' is invalid.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "merlin_out");
    
#pragma ACCEL debug fflush(stdout);
  }
}

void __merlin_execute_ellpack_kernel(double nzval[(1 << 12) * (1 << 9)],int cols[(1 << 12) * (1 << 9)],double vec[1 << 12],double out[1 << 12])
{
}
