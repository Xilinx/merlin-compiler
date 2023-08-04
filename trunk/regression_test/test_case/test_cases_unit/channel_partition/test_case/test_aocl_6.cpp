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
#include "merlin_stream.h" 
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
#pragma ACCEL STREAM_CHANNEL CHANNEL_DEPTH=32 NODE=align_kernel_fpga PORT=msm_port_profbuf_vec_data_align_kernel_fpga_0
merlin_stream ch_profbuf_vec_data_3;

static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static const unsigned long NWORDS_PER_REG = 16;
static const unsigned long NBYTES_PER_REG = 16;
static const unsigned long NBYTES_PER_WORD = 1;


void merlin_assign_m128i_0(unsigned char (*lhs_data)[16])
{
  int i_0;
  for (i_0 = 0; i_0 < 16UL; ++i_0) {
    unsigned char profbuf_vec_data_sn_tmp_0;
    
#pragma ACCEL PARALLEL COMPLETE
    merlin_stream_read(&ch_profbuf_vec_data_3,(&profbuf_vec_data_sn_tmp_0));
    ( *lhs_data)[i_0] = profbuf_vec_data_sn_tmp_0;
  }
}

void msm_port_profbuf_vec_data_align_kernel_fpga_0(unsigned char *profbuf_vec_data)
{
  int _i_sub_4;
  int _i_0_5;
  for (int _i_l23 = 0; _i_l23 < 2500; _i_l23++) 
{   
#pragma ACCEL PARALLEL FACTOR=1
    for (_i_sub_4 = 0; _i_sub_4 < 4; ++_i_sub_4) 
{     
#pragma ACCEL PARALLEL
      for (int _j_l24 = 0; _j_l24 < 128; _j_l24++) {
        for (_i_0_5 = 0; ((unsigned long )_i_0_5) < 16UL; ++_i_0_5) {
          unsigned char profbuf_vec_data_sp_tmp_0;
          
#pragma ACCEL PARALLEL COMPLETE
          profbuf_vec_data_sp_tmp_0 = (&profbuf_vec_data[(_i_l23 * 4 + _i_sub_4) * 2048 + _j_l24 * 16])[_i_0_5];
          merlin_stream_write(&ch_profbuf_vec_data_3,(&profbuf_vec_data_sp_tmp_0));
        }
      }
    }
  }
}

#pragma ACCEL kernel

void align_kernel_fpga(char rf_vec[2048 * 10000],int rf_len_vec[10000],int profbuf_size_vec[10000],unsigned char *profbuf_vec_data,long results[10000],int N)
{
  merlin_stream_init(&ch_profbuf_vec_data_3,0,1);
  
#pragma ACCEL STREAM_PORT ACCESS_TYPE=read DIM_NUM=1 NODE=align_kernel_fpga VARIABLE=profbuf_vec_data
  msm_port_profbuf_vec_data_align_kernel_fpga_0(profbuf_vec_data);
 
  int i_sub;
  
#pragma ACCEL INTERFACE STREAM_PREFETCH=on VARIABLE=profbuf_vec_data max_depth=1280000,16 DEPTH=1280000,16 memory_burst=off
  
#pragma ACCEL interface variable=rf_vec stream_prefetch=on max_depth=20480000 depth=20480000 memory_burst=off
  
#pragma ACCEL interface variable=rf_len_vec stream_prefetch=on max_depth=10000 depth=10000 memory_burst=off
  
#pragma ACCEL interface variable=profbuf_size_vec stream_prefetch=on max_depth=10000 depth=10000 memory_burst=off
  
#pragma ACCEL interface variable=results stream_prefetch=on max_depth=10000 depth=10000 memory_burst=off

  for (int _i_l23 = 0; _i_l23 < 2500; _i_l23++) {
#pragma ACCEL PARALLEL FACTOR=1
    for (i_sub = 0; i_sub < 4; ++i_sub) {
#pragma ACCEL PARALLEL
      unsigned char profbuf_data[128][16];
      for (int _j_l24 = 0; _j_l24 < 128; _j_l24++) {
        merlin_assign_m128i_0(&profbuf_data[_j_l24]);
      }
    }
  }
  merlin_stream_reset(&ch_profbuf_vec_data_3);
}
