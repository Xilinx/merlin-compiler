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
#include "cmost.h"
#include "merlin_type_define.h"

__kernel void vec_add_kernel(int *a,int *b,int *c,int inc)
{
  
#pragma HLS INTERFACE m_axi port=a offset=slave depth=1 bundle=merlin_gmem32
  
#pragma HLS INTERFACE m_axi port=b offset=slave depth=1 bundle=merlin_gmem32
  
#pragma HLS INTERFACE m_axi port=c offset=slave depth=1 bundle=merlin_gmem32
  
#pragma HLS INTERFACE s_axilite port=a bundle=control
  
#pragma HLS INTERFACE s_axilite port=b bundle=control
  
#pragma HLS INTERFACE s_axilite port=c bundle=control
  
#pragma HLS INTERFACE s_axilite port=inc bundle=control
  
#pragma HLS INTERFACE s_axilite port=return bundle=control
  
#pragma HLS interface port=a
  
#pragma HLS interface port=b
  
#pragma HLS interface port=c
  
#pragma HLS INTERFACE port=a
  
#pragma HLS INTERFACE port=b
  
#pragma HLS INTERFACE port=c
  int a_buf_0[10000];
  int b_buf_0[10000];
  int c_buf_0[10000];
  int j;
  merlinL0:
  for (j = 0; j < 10000; j++) {
    c_buf_0[j] = a_buf_0[j] + b_buf_0[j] + inc;
  }
}
