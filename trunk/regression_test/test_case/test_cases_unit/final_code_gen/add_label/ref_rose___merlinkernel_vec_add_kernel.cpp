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

void sub(int *a)
{
  merlinL2:
  for (int i = 0; i < 10; ++i) {
    merlinuL20:
    for (int j = 0; j < 10; ++j) {
      a[i * 10 + j] = i * j;
    }
  }
  merlinL1:
  for (int i = 0; i < 10; ++i) {
    int j = 0;
    merlinuL10:
    while(j < 10){
      a[i * 10 + j] = i * j;
      j++;
    }
  }
  int i = 0;
  merlinuL30:
  do {
    int j = 0;
    merlinL0:
    while(j < 10){
      a[i * 10 + j] = i;
      j++;
    }
    i++;
  }while (i < 10);
  i = 0;
  L0:
  do {
    a[i]++;
  }while (i < 10);
}
static int top_dummy_pos;
#pragma ACCEL kernel
extern "C" { 

__kernel void top(int *a)
{
  
#pragma HLS INTERFACE m_axi port=a offset=slave  bundle=merlin_gmem_top_32_a
  
#pragma HLS INTERFACE s_axilite port=a bundle=control
  
#pragma HLS INTERFACE s_axilite port=return bundle=control
  merlinL9:
  for (int i = 0; i < 10; ++i) {
    merlinL8:
    for (int j = 0; j < 10; ++j) {
      a[i * 10 + j] = i * j;
    }
  }
  merlinL7:
  for (int i = 0; i < 10; ++i) {
    int j = 0;
    L00:
    while(j < 10){
      a[i * 10 + j] = i * j;
      j++;
      goto L00;
    }
  }
  int i = 0;
  merlinL6:
  do {
    int j = 0;
    merlinL5:
    while(j < 10){
      a[i * 10 + j] = i;
      j++;
    }
    i++;
  }while (i < 10);
  sub(a);
  switch(a[0]){
    case 1:
    merlinL4:
    for (int i = 0; i < 10; ++i) {
    }
    break; 
    default:
    merlinL3:
    for (int i = 0; i < 10; ++i) {
    }
    break; 
  }
}
}

void __merlinwrapper_top(int *a)
{
  top(a);
}
