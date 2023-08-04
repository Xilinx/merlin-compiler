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
#include "ap_int.h"
#include <string.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <assert.h>
#include <string.h>
static void __merlin_dummy_518();
void kmeans(int num_samples,int num_runs_,int num_clusters_,int vector_length_,double centers[1024 * 10 * 1],double output[(1024 + 3) * 10 * 1],int data_size,int center_size,int output_size);

void __merlinwrapper_kmeans_kernel(int num_samples,int num_runs_,int num_clusters_,int vector_length_,double data[1024 * 10000],double centers[1024 * 10 * 1],double output[(1024 + 3) * 10 * 1],int data_size,int center_size,int output_size)
{
  
#pragma ACCEL wrapper variable=num_samples port=num_samples
  
#pragma ACCEL wrapper variable=num_runs_ port=num_runs_
  
#pragma ACCEL wrapper variable=num_clusters_ port=num_clusters_
  
#pragma ACCEL wrapper variable=vector_length_ port=vector_length_
  
#pragma ACCEL wrapper variable=data port=data depth=10240000 max_depth=10240000 data_type=double io=RO copy=false
  
#pragma ACCEL wrapper variable=centers port=centers depth=10240 max_depth=10240 data_type=double io=RO copy=false
  
#pragma ACCEL wrapper variable=output depth=10270 max_depth=10270 data_type=double io=RW copy=false port=merlin_output
  
#pragma ACCEL wrapper variable=data_size port=data_size
  
#pragma ACCEL wrapper variable=center_size port=center_size
  
#pragma ACCEL wrapper variable=output_size port=output_size
  static double __m_data[10240000];
  static double __m_centers[10240];
  static double __m_merlin_output[10270];
{
    if (data != 0) {
      memcpy(((void *)(__m_data + 0)),((const void *)data),10240000 * sizeof(double ));
    }
     else {
      printf("Error! The external memory pointed by 'data' is invalid.\n");
      exit(1);
    }
  }
{
    if (centers != 0) {
      memcpy(((void *)(__m_centers + 0)),((const void *)centers),10240 * sizeof(double ));
    }
     else {
      printf("Error! The external memory pointed by 'centers' is invalid.\n");
      exit(1);
    }
  }
{
    if (output != 0) {
      memcpy(((void *)(__m_merlin_output + 0)),((const void *)output),10270 * sizeof(double ));
    }
     else {
      printf("Error! The external memory pointed by 'output' is invalid.\n");
      exit(1);
    }
  }
  
  kmeans(num_samples,num_runs_,num_clusters_,vector_length_,__m_centers,__m_merlin_output,data_size,center_size,output_size);
{
    if (output != 0) {
      memcpy(((void *)output),((const void *)(__m_merlin_output + 0)),10270 * sizeof(double ));
    }
     else {
      printf("Error! The external memory pointed by 'output' is invalid.\n");
      exit(1);
    }
  }
}
