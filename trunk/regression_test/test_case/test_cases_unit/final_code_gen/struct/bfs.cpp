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
/*
Implementations based on:
Harish and Narayanan. "Accelerating large graph algorithms on the GPU using CUDA." HiPC, 2007.
Hong, Oguntebi, Olukotun. "Efficient Parallel Graph Exploration on Multi-Core CPU and GPU." PACT, 2011.
*/
#include <assert.h>
#include "bfs.h"

void bfs(struct node_t_struct nodes[1L << 8],struct edge_t_struct edges[(1L << 8) * 16],unsigned long starting_node,signed char level[1L << 8],unsigned long level_counts[10])
{
  unsigned long n;
  unsigned long e;
  signed char horizon;
  unsigned long cnt;
  level[starting_node] = 0;
  level_counts[0] = 1;
  loop_horizons:
  for (horizon = 0; horizon < 10; horizon++) {
    cnt = 0;
// Add unmarked neighbors of the current horizon to the next horizon
    loop_nodes:
    for (n = 0; n < (1L << 8); n++) {
      if (level[n] == horizon) {
        unsigned long tmp_begin = nodes[n] . edge_begin;
        unsigned long tmp_end = nodes[n] . edge_end;
        loop_neighbors:
        for (e = tmp_begin; e < tmp_end; e++) {
          
#pragma HLS loop_tripcount max=2
          unsigned long tmp_dst = edges[e] . dst;
          signed char tmp_level = level[tmp_dst];
// Unmarked
          if (tmp_level == 127) {
            level[tmp_dst] = (horizon + 1);
            ++cnt;
          }
        }
      }
    }
    if ((level_counts[horizon + 1] = cnt) == 0) 
      break; 
  }
}
static void __merlin_dummy_kernel_pragma();

void bfs_kernel(struct node_t_struct *nodes,struct edge_t_struct *edges,unsigned long *starting_node,signed char *level,unsigned long *level_counts)
{
  
#pragma ACCEL interface variable=nodes depth=256000
  
#pragma ACCEL interface variable=edges depth=4096000
  
#pragma ACCEL interface variable=starting_node depth=1
  
#pragma ACCEL interface variable=level depth=256000
  
#pragma ACCEL interface variable=level_counts depth=10000
  int num_jobs = 1000;
  for (int i = 0; i < num_jobs; i++) {
    bfs(nodes + i * (1L << 8),edges + i * ((1L << 8) * 16), *starting_node,level + i * (1L << 8),level_counts + i * 10);
  }
  return ;
}
