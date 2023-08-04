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
extern int __merlin_include__GB_stdlib_h_2;
extern int __merlin_include__GB_stdio_h_1;
extern int __merlin_include__GB_string_h_0;
static void __merlin_dummy_431();
void workload(float *feature,int *membership,float *clusters);
static void __merlin_dummy_extern_int_merlin_include_L_();
#include "__merlinhead_kernel_top.h"
static void __merlin_dummy_extern_int_merlin_include_L_();

void __merlinwrapper_workload(float *feature,int *membership,float *clusters)
{
{
    
#pragma ACCEL wrapper variable=feature  depth=26214400 max_depth=26214400 data_type=float io=RO copy=false port=merlin_feature
    
#pragma ACCEL wrapper variable=membership port=membership depth=819200 max_depth=819200 data_type=int io=WO copy=false
    
#pragma ACCEL wrapper variable=clusters port=clusters depth=160 max_depth=160 data_type=float io=RO copy=true
  }
  static float __m_merlin_feature[26214400];
  static int __m_membership[819200];
  static float __m_clusters[160];
{
/*
    static float __m_merlin_feature[26214400];
    static int __m_membership[819200];
    static float __m_clusters[160];
*/
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "merlin_feature", "26214400 * sizeof(float )");
      
#pragma ACCEL debug fflush(stdout);
      if (feature != 0) {
        memcpy((void *)(__m_merlin_feature + 0),(const void *)feature,26214400 * sizeof(float ));
      }
       else {
        printf("Error! Detected null pointer 'feature' for external memory.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "merlin_feature");
      
#pragma ACCEL debug fflush(stdout);
    }
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "clusters", "160 * sizeof(float )");
      
#pragma ACCEL debug fflush(stdout);
      if (clusters != 0) {
        memcpy((void *)(__m_clusters + 0),(const void *)clusters,160 * sizeof(float ));
      }
       else {
        printf("Error! Detected null pointer 'clusters' for external memory.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "clusters");
      
#pragma ACCEL debug fflush(stdout);
    }
  }
  
#pragma ACCEL kernel
  workload(__m_merlin_feature,__m_membership,__m_clusters);
{
/*
    static float __m_merlin_feature[26214400];
    static int __m_membership[819200];
    static float __m_clusters[160];
*/
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "membership", "819200 * sizeof(int )");
      
#pragma ACCEL debug fflush(stdout);
      if (membership != 0) {
        memcpy((void *)membership,(const void *)(__m_membership + 0),819200 * sizeof(int ));
      }
       else {
        printf("Error! Detected null pointer 'membership' for external memory.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "membership");
      
#pragma ACCEL debug fflush(stdout);
    }
  }
}

void __merlin_workload(float *feature,int *membership,float *clusters)
{
  
#pragma ACCEL string __merlin_check_opencl();
  
#pragma ACCEL string __merlin_init_workload();
  __merlinwrapper_workload(feature,membership,clusters);
  
#pragma ACCEL string __merlin_release_workload();
}

void __merlin_write_buffer_workload(float *feature,int *membership,float *clusters)
{
  static float __m_merlin_feature[26214400];
  static int __m_membership[819200];
  static float __m_clusters[160];
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "merlin_feature", "26214400 * sizeof(float )");
    
#pragma ACCEL debug fflush(stdout);
    if (feature != 0) {
      memcpy((void *)(__m_merlin_feature + 0),(const void *)feature,26214400 * sizeof(float ));
    }
     else {
      printf("Error! Detected null pointer 'feature' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "merlin_feature");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "clusters", "160 * sizeof(float )");
    
#pragma ACCEL debug fflush(stdout);
    if (clusters != 0) {
      memcpy((void *)(__m_clusters + 0),(const void *)clusters,160 * sizeof(float ));
    }
     else {
      printf("Error! Detected null pointer 'clusters' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "clusters");
    
#pragma ACCEL debug fflush(stdout);
  }
}

void __merlin_read_buffer_workload(float *feature,int *membership,float *clusters)
{
  static float __m_merlin_feature[26214400];
  static int __m_membership[819200];
  static float __m_clusters[160];
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "membership", "819200 * sizeof(int )");
    
#pragma ACCEL debug fflush(stdout);
    if (membership != 0) {
      memcpy((void *)membership,(const void *)(__m_membership + 0),819200 * sizeof(int ));
    }
     else {
      printf("Error! Detected null pointer 'membership' for external memory.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "membership");
    
#pragma ACCEL debug fflush(stdout);
  }
}

void __merlin_execute_workload(float *feature,int *membership,float *clusters)
{
}
