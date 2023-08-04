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
#pragma ACCEL stream_channel
merlin_stream ch_c_buf_0_2_p[5][10];
#pragma ACCEL array_partition type=channel variable=ch_c_buf_0_2 dim=2 factor=10
#pragma ACCEL array_partition type=channel variable=ch_c_buf_0_2 dim=1 factor=5
#pragma ACCEL stream_channel
merlin_stream ch_a_buf_0_1_p[5][10];
#pragma ACCEL array_partition type=channel variable=ch_a_buf_0_1 dim=2 factor=10
#pragma ACCEL array_partition type=channel variable=ch_a_buf_0_1 dim=1 factor=5
#pragma ACCEL stream_channel
merlin_stream ch_b_buf_0_0_p[5][10];
#pragma ACCEL array_partition type=channel variable=ch_b_buf_0_0 dim=2 factor=10
#pragma ACCEL array_partition type=channel variable=ch_b_buf_0_0 dim=1 factor=5
#include <string.h> 
#include <string.h> 
#define TILE_FACTOR 100
#define UNROLL_FACTOR 5
static void __merlin_dummy___merlin_dummy_kernel_pragma();
// Original: #pragma ACCEL kernel

void __msm_node_0(int i,int b_buf_0[5][20][10][10],int a_buf_0[5][20][10][10],int c_buf_0[5][20][10][10],int jj)
// Original: #pragma ACCEL parallel
// Original: #pragma ACCEL PARALLEL COMPLETE
{
  int c_buf_0_sn_tmp;
  int a_buf_0_sn_tmp;
  int b_buf_0_sn_tmp;
  int j0;
  int k;
  int k0;
  int kk;
  for (j0 = 0; j0 < 20; ++j0) {
    for (k0 = 0; k0 < 10; ++k0) {
      
#pragma unroll
      for (kk = 0; kk < 10; ++kk) 
// Original: #pragma ACCEL parallel
// Original: #pragma ACCEL PARALLEL COMPLETE
{
        k = k0 * 10 + kk;
        int offset;
        offset = (i * 100 + jj * (100 / 5) + j0) * 100 + k;
        merlin_stream_read(&ch_b_buf_0_0_p[jj][kk],(&b_buf_0_sn_tmp));
        merlin_stream_read(&ch_a_buf_0_1_p[jj][kk],(&a_buf_0_sn_tmp));
        c_buf_0_sn_tmp = a_buf_0_sn_tmp + b_buf_0_sn_tmp;
        merlin_stream_write(&ch_c_buf_0_2_p[jj][kk],(&c_buf_0_sn_tmp));
      }
    }
  }
}

void __msm_port_i___msm_node_0_0(int i)
{
  int _i;
  _i = i;
  _i;
}

void __msm_port_b_buf_0___msm_node_0_0(int b_buf_0[5][20][10][10],int jj)
{
  int b_buf_0_sp_tmp;
  int _jj;
  _jj = jj;
  int _j0;
  int _k0;
  int _kk;
  for (_j0 = 0; _j0 < 20; ++_j0) {
    for (_k0 = 0; _k0 < 10; ++_k0) {
      
#pragma unroll
      for (_kk = 0; _kk < 10; ++_kk) 
// Original: #pragma ACCEL parallel
// Original: #pragma ACCEL PARALLEL COMPLETE
{
        b_buf_0_sp_tmp = b_buf_0[_jj][_j0][_k0][_kk];
        merlin_stream_write(&ch_b_buf_0_0_p[jj][_kk],(&b_buf_0_sp_tmp));
      }
    }
  }
}

void __msm_port_a_buf_0___msm_node_0_0(int a_buf_0[5][20][10][10],int jj)
{
  int a_buf_0_sp_tmp;
  int _jj;
  _jj = jj;
  int _j0;
  int _k0;
  int _kk;
  for (_j0 = 0; _j0 < 20; ++_j0) {
    for (_k0 = 0; _k0 < 10; ++_k0) {
      
#pragma unroll
      for (_kk = 0; _kk < 10; ++_kk) 
// Original: #pragma ACCEL parallel
// Original: #pragma ACCEL PARALLEL COMPLETE
{
        a_buf_0_sp_tmp = a_buf_0[_jj][_j0][_k0][_kk];
        merlin_stream_write(&ch_a_buf_0_1_p[jj][_kk],(&a_buf_0_sp_tmp));
      }
    }
  }
}

void __msm_port_c_buf_0___msm_node_0_0(int c_buf_0[5][20][10][10],int jj)
{
  int c_buf_0_sp_tmp;
  int _jj;
  _jj = jj;
  int _j0;
  int _k0;
  int _kk;
  for (_j0 = 0; _j0 < 20; ++_j0) {
    for (_k0 = 0; _k0 < 10; ++_k0) {
      
#pragma unroll
      for (_kk = 0; _kk < 10; ++_kk) 
// Original: #pragma ACCEL parallel
// Original: #pragma ACCEL PARALLEL COMPLETE
{
        merlin_stream_read(&ch_c_buf_0_2_p[jj][_kk],(&c_buf_0_sp_tmp));
        c_buf_0[_jj][_j0][_k0][_kk] = c_buf_0_sp_tmp;
      }
    }
  }
}

void __msm_port_jj___msm_node_0_0(int jj)
{
  int _jj;
  _jj = jj;
  _jj;
}

#pragma ACCEL kernel
void top(int *a,int *b,int *c)
{
  int _memcpy_i2_3;
  int _memcpy_i2_2;
  int _memcpy_i2_1;
  int _memcpy_i2_0;
  int _memcpy_i1_3;
  int _memcpy_i1_2;
  int _memcpy_i1_1;
  int _memcpy_i1_0;
  int _memcpy_i0_3;
  int _memcpy_i0_2;
  int _memcpy_i0_1;
  int _memcpy_i0_0;
  
#pragma accel interface variable=a max_depth=1000000 depth=1000000
  
#pragma accel interface variable=b max_depth=1000000 depth=1000000
  
#pragma accel interface variable=c max_depth=1000000 depth=1000000
  for (int i = 0; i < 100; ++i) {
    int b_buf_0[5][20][10][10];
    int a_buf_0[5][20][10][10];
    int c_buf_0[5][20][10][10];
{
      for (_memcpy_i0_3 = 0; _memcpy_i0_3 < 5UL; ++_memcpy_i0_3) {
        for (_memcpy_i0_2 = 0; _memcpy_i0_2 < 20UL; ++_memcpy_i0_2) {
          for (_memcpy_i0_1 = 0; _memcpy_i0_1 < 10UL; ++_memcpy_i0_1) {
            
#pragma unroll
            for (_memcpy_i0_0 = 0; _memcpy_i0_0 < 10UL; ++_memcpy_i0_0) {
              a_buf_0[_memcpy_i0_3][_memcpy_i0_2][_memcpy_i0_1][_memcpy_i0_0] = a[(i * 10000) + ((((0 * 5UL + _memcpy_i0_3) * 20UL + _memcpy_i0_2) * 10UL + _memcpy_i0_1) * 10UL + _memcpy_i0_0)];
            }
          }
        }
      }
    }
{
      for (_memcpy_i1_3 = 0; _memcpy_i1_3 < 5UL; ++_memcpy_i1_3) {
        for (_memcpy_i1_2 = 0; _memcpy_i1_2 < 20UL; ++_memcpy_i1_2) {
          for (_memcpy_i1_1 = 0; _memcpy_i1_1 < 10UL; ++_memcpy_i1_1) {
            
#pragma unroll
            for (_memcpy_i1_0 = 0; _memcpy_i1_0 < 10UL; ++_memcpy_i1_0) {
              b_buf_0[_memcpy_i1_3][_memcpy_i1_2][_memcpy_i1_1][_memcpy_i1_0] = b[(i * 10000) + ((((0 * 5UL + _memcpy_i1_3) * 20UL + _memcpy_i1_2) * 10UL + _memcpy_i1_1) * 10UL + _memcpy_i1_0)];
            }
          }
        }
      }
    }
    
#pragma unroll
    for (int jj = 0; jj < 5; ++jj) {
      
#pragma ACCEL stream_port access_type=read dim_num=0 node=__msm_node_0 variable=i
      __msm_port_i___msm_node_0_0(i);
      int kk_3;
      
#pragma unroll
      for (kk_3 = 0; kk_3 <= 9; ++kk_3) {
        merlin_stream_init(&ch_b_buf_0_0_p[jj][kk_3],((unsigned long )10000),((unsigned long )4));
      }
// Original pragma: ACCEL stream_port access_type=read dim_num=4 node=__msm_node_0 variable=b_buf_0
      
#pragma ACCEL stream_port access_type=read dim_num=4 node=__msm_node_0 parallel_iterator=1 variable=b_buf_0
      __msm_port_b_buf_0___msm_node_0_0(b_buf_0,jj);
      int kk_1;
      
#pragma unroll
      for (kk_1 = 0; kk_1 <= 9; ++kk_1) {
        merlin_stream_init(&ch_a_buf_0_1_p[jj][kk_1],((unsigned long )10000),((unsigned long )4));
      }
// Original pragma: ACCEL stream_port access_type=read dim_num=4 node=__msm_node_0 variable=a_buf_0
      
#pragma ACCEL stream_port access_type=read dim_num=4 node=__msm_node_0 parallel_iterator=1 variable=a_buf_0
      __msm_port_a_buf_0___msm_node_0_0(a_buf_0,jj);
// Original: #pragma ACCEL parallel
// Original: #pragma ACCEL PARALLEL COMPLETE
      int kk;
      
#pragma unroll
      for (kk = 0; kk <= 9; ++kk) {
        merlin_stream_init(&ch_c_buf_0_2_p[jj][kk],((unsigned long )10000),((unsigned long )4));
      }
      
#pragma ACCEL stream_port access_type=read dim_num=0 node=__msm_node_0 variable=jj
      __msm_port_jj___msm_node_0_0(jj);
// Original pragma: ACCEL stream_node 
// Original pragma: ACCEL stream_node parallel_factor=5
      
#pragma ACCEL stream_node parallel_factor=5 parallel_iterator=4
      __msm_node_0(i,b_buf_0,a_buf_0,c_buf_0,jj);
// Original pragma: ACCEL stream_port access_type=write dim_num=4 node=__msm_node_0 variable=c_buf_0
      
#pragma ACCEL stream_port access_type=write dim_num=4 node=__msm_node_0 parallel_iterator=1 variable=c_buf_0
      __msm_port_c_buf_0___msm_node_0_0(c_buf_0,jj);
      int kk_0;
      
#pragma unroll
      for (kk_0 = 0; kk_0 <= 9; ++kk_0) {
        merlin_stream_reset(&ch_c_buf_0_2_p[jj][kk_0]);
      }
      int kk_2;
      
#pragma unroll
      for (kk_2 = 0; kk_2 <= 9; ++kk_2) {
        merlin_stream_reset(&ch_a_buf_0_1_p[jj][kk_2]);
      }
      int kk_4;
      
#pragma unroll
      for (kk_4 = 0; kk_4 <= 9; ++kk_4) {
        merlin_stream_reset(&ch_b_buf_0_0_p[jj][kk_4]);
      }
    }
{
      for (_memcpy_i2_3 = 0; _memcpy_i2_3 < 5UL; ++_memcpy_i2_3) {
        for (_memcpy_i2_2 = 0; _memcpy_i2_2 < 20UL; ++_memcpy_i2_2) {
          for (_memcpy_i2_1 = 0; _memcpy_i2_1 < 10UL; ++_memcpy_i2_1) {
            
#pragma unroll
            for (_memcpy_i2_0 = 0; _memcpy_i2_0 < 10UL; ++_memcpy_i2_0) {
              c[(i * 10000) + ((((0 * 5UL + _memcpy_i2_3) * 20UL + _memcpy_i2_2) * 10UL + _memcpy_i2_1) * 10UL + _memcpy_i2_0)] = c_buf_0[_memcpy_i2_3][_memcpy_i2_2][_memcpy_i2_1][_memcpy_i2_0];
            }
          }
        }
      }
    }
  }
}
