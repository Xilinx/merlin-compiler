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
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();

static void bfs(unsigned long nodes_edge_begin[256],unsigned long nodes_edge_end[256],unsigned long edges_dst[4096],unsigned long starting_node,signed char level[256],unsigned long level_counts[10])
{
  unsigned long n;
  unsigned long e;
  unsigned long cnt;
  level[starting_node] = ((signed char )0);
  level_counts[0] = ((unsigned long )1);
  loop_horizons:
{
  }
  int _s_horizon;
// Canonicalized from: for(_s_horizon =((signed char )0);((int )_s_horizon) < 10;_s_horizon++) {...}
  for (_s_horizon = ((int )((signed char )0)); _s_horizon <= 9; ++_s_horizon) {
    cnt = ((unsigned long )0);
// Add unmarked neighbors of the current horizon to the next horizon
    loop_nodes:
{
    }
    for (n = ((unsigned long )0); ((unsigned long )n) < ((unsigned long )(1L << 8)); n++) {
      if (((int )level[n]) == ((int )_s_horizon)) {
        unsigned long tmp_begin = nodes_edge_begin[n];
        unsigned long tmp_end = nodes_edge_end[n];
        loop_neighbors:
{
        }
        for (e = tmp_begin; e < tmp_end; e++) {
          
#pragma HLS loop_tripcount max=2
          unsigned long tmp_dst = edges_dst[e];
          signed char tmp_level = level[edges_dst[e]];
// Unmarked
          if (((int )tmp_level) == 127) {
            level[edges_dst[e]] = ((signed char )(((int )_s_horizon) + 1));
            ++cnt;
          }
        }
      }
    }
    if ((level_counts[((int )_s_horizon) + 1] = cnt) == ((unsigned long )0)) {
      break; 
    }
  }
}
static void __merlin_dummy_kernel_pragma();
// Original: #pragma ACCEL kernel

__kernel void bfs_kernel(__global unsigned long * restrict nodes_edge_begin,__global unsigned long * restrict nodes_edge_end,__global unsigned long * restrict edges_dst,__global unsigned long * restrict starting_node,__global signed char * restrict level,__global unsigned long * restrict level_counts)
{
  int _memcpy_i5_1;
  int _memcpy_i5_0;
  int _memcpy_i4_1;
  int _memcpy_i4_0;
  unsigned long starting_node_buf_0[1];
  starting_node_buf_0[0] = starting_node[0];
  unsigned long level_counts_buf_0[10000];
  long _memcpy_i_6;
  
#pragma unroll 8
  for (_memcpy_i_6 = 0; _memcpy_i_6 < sizeof(unsigned long ) * ((unsigned long )10000) / 8; ++_memcpy_i_6) {
    long total_offset1 = (0 * 10000 + 0);
    long total_offset2 = (0 * 0 + 0);
    level_counts_buf_0[total_offset1 + _memcpy_i_6] = level_counts[total_offset2 + _memcpy_i_6];
  }
  signed char level_buf_0[1000][256];
  for (_memcpy_i4_1 = 0; _memcpy_i4_1 < 1000; ++_memcpy_i4_1) {
    
#pragma HLS COALESCING_WIDTH=512
    
#pragma unroll 64
    for (_memcpy_i4_0 = 0; _memcpy_i4_0 < 256; ++_memcpy_i4_0) {
      level_buf_0[_memcpy_i4_1][_memcpy_i4_0] = level[0 + ((0 * 1000 + _memcpy_i4_1) * 256 + _memcpy_i4_0)];
    }
  }
  
#pragma ACCEL INTERFACE VARIABLE=edges_dst max_depth=4096000 DEPTH=4096000
  
#pragma ACCEL INTERFACE VARIABLE=nodes_edge_end max_depth=256000 DEPTH=256000
  
#pragma ACCEL INTERFACE VARIABLE=nodes_edge_begin max_depth=256000 DEPTH=256000
  
#pragma ACCEL interface variable=starting_node max_depth=1 depth=1
  
#pragma ACCEL interface variable=level max_depth=256000 depth=256000
  
#pragma ACCEL interface variable=level_counts max_depth=10000 depth=10000
  int num_jobs = 1000;
  for (int i = 0; i < 1000; i++) {
    unsigned long nodes_edge_end_buf_0[256];
    long _memcpy_i_1;
    
#pragma unroll 8
    for (_memcpy_i_1 = 0; _memcpy_i_1 < sizeof(unsigned long ) * ((unsigned long )256) / 8; ++_memcpy_i_1) {
      long total_offset1 = (0 * 256 + 0);
      long total_offset2 = (0 * 0 + i * 256);
      nodes_edge_end_buf_0[total_offset1 + _memcpy_i_1] = nodes_edge_end[total_offset2 + _memcpy_i_1];
    }
    unsigned long nodes_edge_begin_buf_0[256];
    long _memcpy_i_0;
    
#pragma unroll 8
    for (_memcpy_i_0 = 0; _memcpy_i_0 < sizeof(unsigned long ) * ((unsigned long )256) / 8; ++_memcpy_i_0) {
      long total_offset1 = (0 * 256 + 0);
      long total_offset2 = (0 * 0 + i * 256);
      nodes_edge_begin_buf_0[total_offset1 + _memcpy_i_0] = nodes_edge_begin[total_offset2 + _memcpy_i_0];
    }
    unsigned long edges_dst_buf_0[4096];
    long _memcpy_i_2;
    
#pragma unroll 8
    for (_memcpy_i_2 = 0; _memcpy_i_2 < sizeof(unsigned long ) * ((unsigned long )4096) / 8; ++_memcpy_i_2) {
      long total_offset1 = (0 * 4096 + 0);
      long total_offset2 = (0 * 0 + i * 4096);
      edges_dst_buf_0[total_offset1 + _memcpy_i_2] = edges_dst[total_offset2 + _memcpy_i_2];
    }
    bfs(nodes_edge_begin_buf_0,nodes_edge_end_buf_0,edges_dst_buf_0,starting_node_buf_0[0],level_buf_0[i],&level_counts_buf_0[i * 10]);
  }
  for (_memcpy_i5_1 = 0; _memcpy_i5_1 < 1000; ++_memcpy_i5_1) {
    
#pragma HLS COALESCING_WIDTH=512
    
#pragma unroll 64
    for (_memcpy_i5_0 = 0; _memcpy_i5_0 < 256; ++_memcpy_i5_0) {
      level[0 + ((0 * 1000 + _memcpy_i5_1) * 256 + _memcpy_i5_0)] = level_buf_0[_memcpy_i5_1][_memcpy_i5_0];
    }
  }
  long _memcpy_i_7;
  
#pragma unroll 8
  for (_memcpy_i_7 = 0; _memcpy_i_7 < sizeof(unsigned long ) * ((unsigned long )10000) / 8; ++_memcpy_i_7) {
    long total_offset1 = (0 * 0 + 0);
    long total_offset2 = (0 * 10000 + 0);
    level_counts[total_offset1 + _memcpy_i_7] = level_counts_buf_0[total_offset2 + _memcpy_i_7];
  }
  return ;
}
