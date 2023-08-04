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
#define __constant
#define __kernel
#define __global
#include "memcpy_512_1d.h"
#define SIZE_1 4
#include "memcpy_512_2d.h"
#undef SIZE_1
#include <string.h> 

#include "merlin_type_define.h"
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_kernel_pragma();
// Original: #pragma ACCEL kernel
static int vec_add_kernel_dummy_pos;
extern "C" { 

__kernel void vec_add_kernel(merlin_uint_512 *a,merlin_uint_512 *b,merlin_uint_512 *c,int inc,int inc_comp)
{
  
#pragma HLS INTERFACE m_axi port=a offset=slave depth=63
  
#pragma HLS INTERFACE m_axi port=b offset=slave depth=63
  
#pragma HLS INTERFACE m_axi port=c offset=slave depth=63
  
#pragma HLS INTERFACE s_axilite port=a bundle=control
  
#pragma HLS INTERFACE s_axilite port=b bundle=control
  
#pragma HLS INTERFACE s_axilite port=c bundle=control
  
#pragma HLS INTERFACE s_axilite port=inc bundle=control
  
#pragma HLS INTERFACE s_axilite port=inc_comp bundle=control
  
#pragma HLS INTERFACE s_axilite port=return bundle=control
  
#pragma HLS DATA_PACK VARIABLE=c
  
#pragma HLS DATA_PACK VARIABLE=b
  
#pragma HLS DATA_PACK VARIABLE=a
  int b_buf_2[1000];
  
#pragma HLS array_partition variable=b_buf_2 cyclic factor=8 dim=1
  int a_buf_2[1000];
  
#pragma HLS array_partition variable=a_buf_2 cyclic factor=8 dim=1
  int c_buf_2[1000];
  
#pragma HLS array_partition variable=c_buf_2 cyclic factor=8 dim=1
  int b_buf_1[1000];
  
#pragma HLS array_partition variable=b_buf_1 cyclic factor=8 dim=1
  int a_buf_1[1000];
  
#pragma HLS array_partition variable=a_buf_1 cyclic factor=8 dim=1
  int c_buf_1[1000];
  
#pragma HLS array_partition variable=c_buf_1 cyclic factor=8 dim=1
  int b_buf_0[250][4];
  
#pragma HLS array_partition variable=b_buf_0 complete dim=2
  
#pragma HLS array_partition variable=b_buf_0 cyclic factor=4 dim=1
  int a_buf_0[250][4];
  
#pragma HLS array_partition variable=a_buf_0 complete dim=2
  
#pragma HLS array_partition variable=a_buf_0 cyclic factor=4 dim=1
  int c_buf_0[250][4];
  
#pragma HLS array_partition variable=c_buf_0 complete dim=2
  
#pragma HLS array_partition variable=c_buf_0 cyclic factor=4 dim=1
// Existing HLS partition: #pragma HLS array_partition variable=c_buf_0 complete dim=2
// Existing HLS partition: #pragma HLS array_partition variable=c_buf_0 cyclic factor = 4 dim=1
  memcpy_wide_bus_read_int_2d_4_512(c_buf_0,0,0,(merlin_uint_512 *)c,(0 * 4),sizeof(int ) * ((unsigned long )1000),1000LL);
// Existing HLS partition: #pragma HLS array_partition variable=a_buf_0 complete dim=2
// Existing HLS partition: #pragma HLS array_partition variable=a_buf_0 cyclic factor = 4 dim=1
  memcpy_wide_bus_read_int_2d_4_512(a_buf_0,0,0,(merlin_uint_512 *)a,(0 * 4),sizeof(int ) * ((unsigned long )1000),1000LL);
// Existing HLS partition: #pragma HLS array_partition variable=b_buf_0 complete dim=2
// Existing HLS partition: #pragma HLS array_partition variable=b_buf_0 cyclic factor = 4 dim=1
  memcpy_wide_bus_read_int_2d_4_512(b_buf_0,0,0,(merlin_uint_512 *)b,(0 * 4),sizeof(int ) * ((unsigned long )1000),1000LL);
  
#pragma ACCEL interface variable=a max_depth=1000 depth=1000
  
#pragma ACCEL interface variable=b max_depth=1000 depth=1000
  
#pragma ACCEL interface variable=c max_depth=1000 depth=1000
// Original pramga:  ACCEL PARALLEL FACTOR=4 
  for (int j = 0; j < 250; j++) 
// Original: #pragma ACCEL parallel factor=4
// Original: #pragma ACCEL PARALLEL FACTOR=4
// Original: #pragma ACCEL PIPELINE AUTO
{
    
#pragma HLS pipeline
    for (int j_sub_0 = 0; j_sub_0 < 4; ++j_sub_0) 
// Original: #pragma ACCEL parallel factor=4
// Original: #pragma ACCEL PARALLEL FACTOR=4
// Original: #pragma ACCEL PARALLEL
{
      
#pragma HLS unroll
      if (inc > 100) {
        c_buf_0[j][j_sub_0] = a_buf_0[j][j_sub_0] + b_buf_0[j][j_sub_0];
      }
    }
  }
// Existing HLS partition: #pragma HLS array_partition variable=c_buf_0 complete dim=2
// Existing HLS partition: #pragma HLS array_partition variable=c_buf_0 cyclic factor = 4 dim=1
  memcpy_wide_bus_write_int_2d_4_512((merlin_uint_512 *)c,c_buf_0,0,0,(4 * 0),sizeof(int ) * ((unsigned long )1000),1000LL);
// Original pramga:  ACCEL PARALLEL FACTOR=4 
  for (int j = 0; j < 250 + -inc / 4; j++) 
// Original: #pragma ACCEL pipeline
// Original: #pragma ACCEL parallel factor=4
// Original: #pragma ACCEL PIPELINE
// Original: #pragma ACCEL PARALLEL FACTOR=4
// Original: #pragma ACCEL PIPELINE
{
    
#pragma HLS pipeline
    for (int j_sub = 0; j_sub < 4; ++j_sub) 
// Original: #pragma ACCEL pipeline
// Original: #pragma ACCEL parallel factor=4
// Original: #pragma ACCEL PIPELINE
// Original: #pragma ACCEL PARALLEL FACTOR=4
// Original: #pragma ACCEL PARALLEL
{
      
#pragma HLS unroll
      if (inc < 100) {
        int tmp_6;
        int tmp_3;
        int tmp_0;
        tmp_0 = memcpy_wide_bus_single_read_int_512((merlin_uint_512 *)a,((j * 4 + j_sub) * 4));
        tmp_3 = memcpy_wide_bus_single_read_int_512((merlin_uint_512 *)b,((j * 4 + j_sub) * 4));
        tmp_6 = tmp_0 + tmp_3;
        memcpy_wide_bus_single_write_int_512((merlin_uint_512 *)c,tmp_6,(4 * (j * 4 + j_sub)));
      }
    }
  }
{
// Original pramga:  ACCEL PARALLEL FACTOR=4 
    for (int j = (1000 + -inc) / 4 * 4; j < 1000 - inc; j++) 
// Original: #pragma ACCEL pipeline
// Original: #pragma ACCEL parallel factor=4
// Original: #pragma ACCEL PIPELINE
// Original: #pragma ACCEL PARALLEL FACTOR=4
// Original: #pragma ACCEL PIPELINE
{
      
#pragma HLS pipeline
      
#pragma HLS LOOP_TRIPCOUNT max=3
      if (inc < 100) {
        int tmp_7;
        int tmp_4;
        int tmp_1;
        tmp_1 = memcpy_wide_bus_single_read_int_512((merlin_uint_512 *)a,(j * 4));
        tmp_4 = memcpy_wide_bus_single_read_int_512((merlin_uint_512 *)b,(j * 4));
        tmp_7 = tmp_1 + tmp_4;
        memcpy_wide_bus_single_write_int_512((merlin_uint_512 *)c,tmp_7,(4 * j));
      }
    }
  }
// Existing HLS partition: #pragma HLS array_partition variable=c_buf_2 cyclic factor = 16 dim=1
  memcpy_wide_bus_read_int_512(&c_buf_2[0],(merlin_uint_512 *)c,(0 * 4),sizeof(int ) * ((unsigned long )1000),1000LL);
// Existing HLS partition: #pragma HLS array_partition variable=a_buf_2 cyclic factor = 16 dim=1
  memcpy_wide_bus_read_int_512(&a_buf_2[0],(merlin_uint_512 *)a,(0 * 4),sizeof(int ) * ((unsigned long )1000),1000LL);
// Existing HLS partition: #pragma HLS array_partition variable=b_buf_2 cyclic factor = 16 dim=1
  memcpy_wide_bus_read_int_512(&b_buf_2[0],(merlin_uint_512 *)b,(0 * 4),sizeof(int ) * ((unsigned long )1000),1000LL);
  for (int j = 0; j < 1000; j++) 
// Original: #pragma ACCEL pipeline
// Original: #pragma ACCEL PIPELINE
// Original: #pragma ACCEL PIPELINE
{
    
#pragma HLS pipeline
    if (inc < inc_comp) {
      c_buf_2[j] = a_buf_2[j] + b_buf_2[j];
    }
  }
// Existing HLS partition: #pragma HLS array_partition variable=c_buf_2 cyclic factor = 16 dim=1
  memcpy_wide_bus_write_int_512((merlin_uint_512 *)c,&c_buf_2[0],(4 * 0),sizeof(int ) * ((unsigned long )1000),1000LL);
// Existing HLS partition: #pragma HLS array_partition variable=c_buf_1 cyclic factor = 16 dim=1
  memcpy_wide_bus_read_int_512(&c_buf_1[0],(merlin_uint_512 *)c,(0 * 4),sizeof(int ) * ((unsigned long )1000),1000LL);
// Existing HLS partition: #pragma HLS array_partition variable=a_buf_1 cyclic factor = 16 dim=1
  memcpy_wide_bus_read_int_512(&a_buf_1[0],(merlin_uint_512 *)a,(0 * 4),sizeof(int ) * ((unsigned long )1000),1000LL);
// Existing HLS partition: #pragma HLS array_partition variable=b_buf_1 cyclic factor = 16 dim=1
  memcpy_wide_bus_read_int_512(&b_buf_1[0],(merlin_uint_512 *)b,(0 * 4),sizeof(int ) * ((unsigned long )1000),1000LL);
  for (int j = 0; j < 1000; j++) 
// Original: #pragma ACCEL pipeline
// Original: #pragma ACCEL PIPELINE
// Original: #pragma ACCEL PIPELINE
{
    
#pragma HLS pipeline
    if (inc < c_buf_1[0]) {
      c_buf_1[j] = a_buf_1[j] + b_buf_1[j];
    }
  }
// Existing HLS partition: #pragma HLS array_partition variable=c_buf_1 cyclic factor = 16 dim=1
  memcpy_wide_bus_write_int_512((merlin_uint_512 *)c,&c_buf_1[0],(4 * 0),sizeof(int ) * ((unsigned long )1000),1000LL);
  for (int j = 0; j < 1000 - inc; j++) 
// Original: #pragma ACCEL pipeline
// Original: #pragma ACCEL PIPELINE
// Original: #pragma ACCEL PIPELINE
{
    
#pragma HLS pipeline
    int tmp_8;
    int tmp_5;
    int tmp_2;
    tmp_2 = memcpy_wide_bus_single_read_int_512((merlin_uint_512 *)a,(j * 4));
    tmp_5 = memcpy_wide_bus_single_read_int_512((merlin_uint_512 *)b,(j * 4));
    tmp_8 = memcpy_wide_bus_single_read_int_512((merlin_uint_512 *)c,(j * 4));
    tmp_8 += tmp_2 + tmp_5;
    memcpy_wide_bus_single_write_int_512((merlin_uint_512 *)c,tmp_8,(4 * j));
  }
}
}
