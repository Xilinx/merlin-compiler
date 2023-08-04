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
#define SIZE_1 16
#include "memcpy_512_2d.h"
#undef SIZE_1
#include <string.h> 

#include "merlin_type_define.h"
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_kernel_pragma();
/* Original: #pragma ACCEL kernel */

__kernel void vec_add_kernel(merlin_uint_512 *a,merlin_uint_512 *b,merlin_uint_512 *c)
{
  
#pragma HLS INTERFACE m_axi port=a offset=slave depth=625
  
#pragma HLS INTERFACE m_axi port=b offset=slave depth=625
  
#pragma HLS INTERFACE m_axi port=c offset=slave depth=625
  
#pragma HLS INTERFACE s_axilite port=a bundle=control
  
#pragma HLS INTERFACE s_axilite port=b bundle=control
  
#pragma HLS INTERFACE s_axilite port=c bundle=control
  
#pragma HLS INTERFACE s_axilite port=return bundle=control
  
#pragma HLS DATA_PACK VARIABLE=c
  
#pragma HLS DATA_PACK VARIABLE=b
  
#pragma HLS DATA_PACK VARIABLE=a
  int b_buf[625][16];
  
#pragma HLS array_partition variable=b_buf complete dim=2
  int a_buf[625][16];
  
#pragma HLS array_partition variable=a_buf complete dim=2
  int c_buf[625][16];
  
#pragma HLS array_partition variable=c_buf complete dim=2
  
#pragma ACCEL interface variable=c depth=10000 max_depth=10000
  
#pragma ACCEL interface variable=b depth=10000 max_depth=10000
  
#pragma ACCEL interface variable=a depth=10000 max_depth=10000
/* Existing HLS partition: #pragma HLS array_partition variable=a_buf cyclic factor = 16 dim=2 */
  memcpy_wide_bus_read_int_2d_16_512(a_buf,0,0,(merlin_uint_512 *)a,(0 * 4),sizeof(int ) * ((unsigned long )10000),10000L);
/* Existing HLS partition: #pragma HLS array_partition variable=b_buf cyclic factor = 16 dim=2 */
  memcpy_wide_bus_read_int_2d_16_512(b_buf,0,0,(merlin_uint_512 *)b,(0 * 4),sizeof(int ) * ((unsigned long )10000),10000L);
/* Original pragma:  ACCEL PARALLEL FACTOR=16  */
  merlinL1:
/* Original pragma:  ACCEL PARALLEL FACTOR=16  */
  for (int i = 0; i < 625; i++) 
/* Original: #pragma ACCEL parallel factor=16 */
/* Original: #pragma ACCEL PARALLEL FACTOR=16 */
/* Original: #pragma ACCEL PIPELINE AUTO */
{
    
#pragma HLS pipeline
    merlinL0:
    for (int i_sub = 0; i_sub < 16; ++i_sub) 
/* Original: #pragma ACCEL parallel factor=16 */
/* Original: #pragma ACCEL PARALLEL FACTOR=16 */
/* Original: #pragma ACCEL PARALLEL */
{
      
#pragma HLS unroll
      c_buf[i][i_sub] = a_buf[i][i_sub] + b_buf[i][i_sub];
    }
  }
/* Existing HLS partition: #pragma HLS array_partition variable=c_buf cyclic factor = 16 dim=2 */
  memcpy_wide_bus_write_int_2d_16_512((merlin_uint_512 *)c,c_buf,0,0,(4 * 0),sizeof(int ) * ((unsigned long )10000),10000L);
}
