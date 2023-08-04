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
#include <assert.h>
#include <cfloat>
static void __merlin_dummy_441();
void kmeans(double *data_0,double *centers,double sums[784 * 10],int counts[10]);

void __merlinwrapper_kmeans(int _num_samples,int _num_runs,int _num_clusters,int _vector_length,double *data_0,double *centers,double sums[784 * 10],int counts[10],double *output)
{
  
#pragma ACCEL wrapper variable=data_0 port=data_0 depth=10240000 max_depth=10240000 data_type=double io=RO copy=false
  
#pragma ACCEL wrapper variable=centers port=centers depth=10250 max_depth=10250 data_type=double io=RO copy=false
  
#pragma ACCEL wrapper variable=sums port=sums depth=7840 max_depth=7840 data_type=double io=RW copy=false
  
#pragma ACCEL wrapper variable=counts port=counts depth=10 max_depth=10 data_type=int io=RW copy=true
  static double __m_data_0[10240000];
  static double __m_centers[10250];
  static double __m_sums[7840];
  static int __m_counts[10];
{
    memcpy((__m_data_0 + 0),data_0,10240000 * sizeof(double ));
  }
{
    memcpy((__m_centers + 0),centers,10250 * sizeof(double ));
  }
{
    memcpy((__m_sums + 0),sums,7840 * sizeof(double ));
  }
{
    memcpy((__m_counts + 0),counts,10 * sizeof(int ));
  }
  
#pragma ACCEL kernel
  kmeans(__m_data_0,__m_centers,__m_sums,__m_counts);
{
    memcpy(sums,(__m_sums + 0),7840 * sizeof(double ));
  }
{
    memcpy(counts,(__m_counts + 0),10 * sizeof(int ));
  }
}
