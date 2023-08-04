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
#include <stdio.h> 
#include <stdlib.h> 
#include "merlin_type_define.h"
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_439();
void bfs_kernel(unsigned long *nodes_edge_begin,unsigned long *nodes_edge_end,unsigned long *edges_dst,unsigned long *starting_node,signed char *level,unsigned long *level_counts);
static void __merlin_dummy_extern_int_merlin_include_L_();
#include "__merlinhead_kernel_top.h"
static void __merlin_dummy_extern_int_merlin_include_L_();

void __merlinwrapper_bfs_kernel(struct node_t_struct *nodes,struct edge_t_struct *edges,unsigned long *starting_node,signed char *level,unsigned long *level_counts)
{
  __CMOST_COMMENT__ commands;
{
    
#pragma ACCEL wrapper variable=nodes->edge_begin port=nodes_edge_begin depth=256000 max_depth=256000 data_type="unsigned long" io=RO copy=false
    
#pragma ACCEL wrapper variable=nodes->edge_end port=nodes_edge_end depth=256000 max_depth=256000 data_type="unsigned long" io=RO copy=false
    
#pragma ACCEL wrapper variable=edges->dst port=edges_dst depth=4096000 max_depth=4096000 data_type="unsigned long" io=RO copy=false
    
#pragma ACCEL wrapper variable=starting_node port=starting_node depth=1 max_depth=1 data_type="unsigned long" io=RO copy=true
    
#pragma ACCEL wrapper variable=level port=level depth=256000 max_depth=256000 data_type="signed char" io=RW copy=false
    
#pragma ACCEL wrapper variable=level_counts port=level_counts depth=10000 max_depth=10000 data_type="unsigned long" io=RW copy=false
  }
  static __CMOST_COMMENT__ __m_nodes_edge_begin;
  static __CMOST_COMMENT__ __m_nodes_edge_end;
  static __CMOST_COMMENT__ __m_edges_dst;
  static __CMOST_COMMENT__ __m_starting_node;
  static __CMOST_COMMENT__ __m_level;
  static __CMOST_COMMENT__ __m_level_counts;
{
    int _i0;
/*
    static unsigned long __m_nodes_edge_begin[256000];
    static unsigned long __m_nodes_edge_end[256000];
    static unsigned long __m_edges_dst[4096000];
    static unsigned long __m_starting_node[1];
    static signed char __m_level[256000];
    static unsigned long __m_level_counts[10000];
*/
    static unsigned long *__ti_nodes_edge_begin;
    posix_memalign((void **)(&__ti_nodes_edge_begin),64,sizeof(unsigned long ) * 256000);
    for (_i0 = 0; _i0 < 256000; ++_i0) {
      if (nodes != 0) {
        __ti_nodes_edge_begin[_i0 * 1] = nodes[_i0] . edge_begin;
      }
       else {
        printf("Error! The external memory pointed by 'nodes[_i0] . edge_begin' is invalid.\n");
        exit(1);
      }
    }
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "nodes_edge_begin", "256000 * 1 * sizeof(unsigned long )");
    
#pragma ACCEL debug fflush(stdout);
    __CMOST_COMMENT__ __bfs_kernel__nodes_edge_begin_buffer;
    opencl_write_buffer(__bfs_kernel__nodes_edge_begin_buffer,commands[0],0 * sizeof(unsigned long ),(unsigned long *)__ti_nodes_edge_begin,((unsigned long )(256000 * 1)) * sizeof(unsigned long ));
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "nodes_edge_begin");
    
#pragma ACCEL debug fflush(stdout);
    static unsigned long *__ti_nodes_edge_end;
    posix_memalign((void **)(&__ti_nodes_edge_end),64,sizeof(unsigned long ) * 256000);
    for (_i0 = 0; _i0 < 256000; ++_i0) {
      if (nodes != 0) {
        __ti_nodes_edge_end[_i0 * 1] = nodes[_i0] . edge_end;
      }
       else {
        printf("Error! The external memory pointed by 'nodes[_i0] . edge_end' is invalid.\n");
        exit(1);
      }
    }
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "nodes_edge_end", "256000 * 1 * sizeof(unsigned long )");
    
#pragma ACCEL debug fflush(stdout);
    __CMOST_COMMENT__ __bfs_kernel__nodes_edge_end_buffer;
    opencl_write_buffer(__bfs_kernel__nodes_edge_end_buffer,commands[0],0 * sizeof(unsigned long ),(unsigned long *)__ti_nodes_edge_end,((unsigned long )(256000 * 1)) * sizeof(unsigned long ));
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "nodes_edge_end");
    
#pragma ACCEL debug fflush(stdout);
    static unsigned long *__ti_edges_dst;
    posix_memalign((void **)(&__ti_edges_dst),64,sizeof(unsigned long ) * 4096000);
    for (_i0 = 0; _i0 < 4096000; ++_i0) {
      if (edges != 0) {
        __ti_edges_dst[_i0 * 1] = edges[_i0] . dst;
      }
       else {
        printf("Error! The external memory pointed by 'edges[_i0] . dst' is invalid.\n");
        exit(1);
      }
    }
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "edges_dst", "4096000 * 1 * sizeof(unsigned long )");
    
#pragma ACCEL debug fflush(stdout);
    __CMOST_COMMENT__ __bfs_kernel__edges_dst_buffer;
    opencl_write_buffer(__bfs_kernel__edges_dst_buffer,commands[0],0 * sizeof(unsigned long ),(unsigned long *)__ti_edges_dst,((unsigned long )(4096000 * 1)) * sizeof(unsigned long ));
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "edges_dst");
    
#pragma ACCEL debug fflush(stdout);
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "starting_node", "1 * sizeof(unsigned long )");
      
#pragma ACCEL debug fflush(stdout);
      if (starting_node != 0) {
        __CMOST_COMMENT__ __bfs_kernel__starting_node_buffer;
        opencl_write_buffer(__bfs_kernel__starting_node_buffer,commands[0],0 * sizeof(unsigned long ),(unsigned long *)starting_node,((unsigned long )1) * sizeof(unsigned long ));
      }
       else {
        printf("Error! The external memory pointed by 'starting_node' is invalid.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "starting_node");
      
#pragma ACCEL debug fflush(stdout);
    }
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "level", "256000 * sizeof(signed char )");
      
#pragma ACCEL debug fflush(stdout);
      if (level != 0) {
        __CMOST_COMMENT__ __bfs_kernel__level_buffer;
        opencl_write_buffer(__bfs_kernel__level_buffer,commands[0],0 * sizeof(signed char ),(signed char *)level,((unsigned long )256000) * sizeof(signed char ));
      }
       else {
        printf("Error! The external memory pointed by 'level' is invalid.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "level");
      
#pragma ACCEL debug fflush(stdout);
    }
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "level_counts", "10000 * sizeof(unsigned long )");
      
#pragma ACCEL debug fflush(stdout);
      if (level_counts != 0) {
        __CMOST_COMMENT__ __bfs_kernel__level_counts_buffer;
        opencl_write_buffer(__bfs_kernel__level_counts_buffer,commands[0],0 * sizeof(unsigned long ),(unsigned long *)level_counts,((unsigned long )10000) * sizeof(unsigned long ));
      }
       else {
        printf("Error! The external memory pointed by 'level_counts' is invalid.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "level_counts");
      
#pragma ACCEL debug fflush(stdout);
    }
  }
  
#pragma ACCEL kernel
{
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "bfs_kernel");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__bfs_kernel_kernel, 0, sizeof(opencl_mem), &__bfs_kernel__nodes_edge_begin_buffer);
	opencl_set_kernel_arg(__bfs_kernel_kernel, 1, sizeof(opencl_mem), &__bfs_kernel__nodes_edge_end_buffer);
	opencl_set_kernel_arg(__bfs_kernel_kernel, 2, sizeof(opencl_mem), &__bfs_kernel__edges_dst_buffer);
	opencl_set_kernel_arg(__bfs_kernel_kernel, 3, sizeof(opencl_mem), &__bfs_kernel__starting_node_buffer);
	opencl_set_kernel_arg(__bfs_kernel_kernel, 4, sizeof(opencl_mem), &__bfs_kernel__level_buffer);
	opencl_set_kernel_arg(__bfs_kernel_kernel, 5, sizeof(opencl_mem), &__bfs_kernel__level_counts_buffer);
	#pragma ACCEL debug printf("[Merlin Info] Finish set kernel %s argument....\n", "bfs_kernel");
	#pragma ACCEL debug fflush(stdout);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start execute kernel %s....\n", "bfs_kernel");
	#pragma ACCEL debug fflush(stdout);
	size_t __gid[1];
	__gid[0] = 1;
	opencl_enqueue_kernel(__bfs_kernel_kernel, commands[0], 1, __gid, &__event_bfs_kernel);
}
{
	clWaitForEvents(1, &__event_bfs_kernel);
	if(__event_bfs_kernel) {
		opencl_release_event(__event_bfs_kernel);
	}
	#pragma ACCEL debug printf("[Merlin Info] Finish execute kernel %s....\n", "bfs_kernel");
	#pragma ACCEL debug fflush(stdout);
}
{
/*
    static unsigned long __m_nodes_edge_begin[256000];
    static unsigned long __m_nodes_edge_end[256000];
    static unsigned long __m_edges_dst[4096000];
    static unsigned long __m_starting_node[1];
    static signed char __m_level[256000];
    static unsigned long __m_level_counts[10000];
*/
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "level", "256000 * sizeof(signed char )");
      
#pragma ACCEL debug fflush(stdout);
      if (level != 0) {
        __CMOST_COMMENT__ __bfs_kernel__level_buffer;
        opencl_read_buffer(__bfs_kernel__level_buffer,commands[0],0 * sizeof(signed char ),(signed char *)level,((unsigned long )256000) * sizeof(signed char ));
      }
       else {
        printf("Error! The external memory pointed by 'level' is invalid.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "level");
      
#pragma ACCEL debug fflush(stdout);
    }
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "level_counts", "10000 * sizeof(unsigned long )");
      
#pragma ACCEL debug fflush(stdout);
      if (level_counts != 0) {
        __CMOST_COMMENT__ __bfs_kernel__level_counts_buffer;
        opencl_read_buffer(__bfs_kernel__level_counts_buffer,commands[0],0 * sizeof(unsigned long ),(unsigned long *)level_counts,((unsigned long )10000) * sizeof(unsigned long ));
      }
       else {
        printf("Error! The external memory pointed by 'level_counts' is invalid.\n");
        exit(1);
      }
      
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "level_counts");
      
#pragma ACCEL debug fflush(stdout);
    }
  }
}

void __merlin_bfs_kernel(struct node_t_struct *nodes,struct edge_t_struct *edges,unsigned long *starting_node,signed char *level,unsigned long *level_counts)
{
  
#pragma ACCEL string __merlin_check_opencl();
  
#pragma ACCEL string __merlin_init_bfs_kernel();
  __merlinwrapper_bfs_kernel(nodes,edges,starting_node,level,level_counts);
  
#pragma ACCEL string __merlin_release_bfs_kernel();
}

void __merlin_write_buffer_bfs_kernel(struct node_t_struct *nodes,struct edge_t_struct *edges,unsigned long *starting_node,signed char *level,unsigned long *level_counts)
{
  int _i0;
  static unsigned long __m_nodes_edge_begin[256000];
  static unsigned long __m_nodes_edge_end[256000];
  static unsigned long __m_edges_dst[4096000];
  static unsigned long __m_starting_node[1];
  static signed char __m_level[256000];
  static unsigned long __m_level_counts[10000];
  static unsigned long *__ti_nodes_edge_begin;
  posix_memalign((void **)(&__ti_nodes_edge_begin),64,sizeof(unsigned long ) * 256000);
  for (_i0 = 0; _i0 < 256000; ++_i0) {
    if (nodes != 0) {
      __ti_nodes_edge_begin[_i0 * 1] = nodes[_i0] . edge_begin;
    }
     else {
      printf("Error! The external memory pointed by 'nodes[_i0] . edge_begin' is invalid.\n");
      exit(1);
    }
  }
  
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "nodes_edge_begin", "256000 * 1 * sizeof(unsigned long )");
  
#pragma ACCEL debug fflush(stdout);
  __CMOST_COMMENT__ __bfs_kernel__nodes_edge_begin_buffer;
  __CMOST_COMMENT__ __event___bfs_kernel__nodes_edge_begin_buffer;
// __MERLIN_EVENT_WRITE__ __event___bfs_kernel__nodes_edge_begin_buffer
  int __MERLIN_EVENT_WRITE____event___bfs_kernel__nodes_edge_begin_buffer;
  opencl_write_buffer_nb(__bfs_kernel__nodes_edge_begin_buffer,commands[0],0 * sizeof(unsigned long ),(unsigned long *)__ti_nodes_edge_begin,((unsigned long )(256000 * 1)) * sizeof(unsigned long ),&__event___bfs_kernel__nodes_edge_begin_buffer);
  
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "nodes_edge_begin");
  
#pragma ACCEL debug fflush(stdout);
  static unsigned long *__ti_nodes_edge_end;
  posix_memalign((void **)(&__ti_nodes_edge_end),64,sizeof(unsigned long ) * 256000);
  for (_i0 = 0; _i0 < 256000; ++_i0) {
    if (nodes != 0) {
      __ti_nodes_edge_end[_i0 * 1] = nodes[_i0] . edge_end;
    }
     else {
      printf("Error! The external memory pointed by 'nodes[_i0] . edge_end' is invalid.\n");
      exit(1);
    }
  }
  
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "nodes_edge_end", "256000 * 1 * sizeof(unsigned long )");
  
#pragma ACCEL debug fflush(stdout);
  __CMOST_COMMENT__ __bfs_kernel__nodes_edge_end_buffer;
  __CMOST_COMMENT__ __event___bfs_kernel__nodes_edge_end_buffer;
// __MERLIN_EVENT_WRITE__ __event___bfs_kernel__nodes_edge_end_buffer
  int __MERLIN_EVENT_WRITE____event___bfs_kernel__nodes_edge_end_buffer;
  opencl_write_buffer_nb(__bfs_kernel__nodes_edge_end_buffer,commands[0],0 * sizeof(unsigned long ),(unsigned long *)__ti_nodes_edge_end,((unsigned long )(256000 * 1)) * sizeof(unsigned long ),&__event___bfs_kernel__nodes_edge_end_buffer);
  
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "nodes_edge_end");
  
#pragma ACCEL debug fflush(stdout);
  static unsigned long *__ti_edges_dst;
  posix_memalign((void **)(&__ti_edges_dst),64,sizeof(unsigned long ) * 4096000);
  for (_i0 = 0; _i0 < 4096000; ++_i0) {
    if (edges != 0) {
      __ti_edges_dst[_i0 * 1] = edges[_i0] . dst;
    }
     else {
      printf("Error! The external memory pointed by 'edges[_i0] . dst' is invalid.\n");
      exit(1);
    }
  }
  
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "edges_dst", "4096000 * 1 * sizeof(unsigned long )");
  
#pragma ACCEL debug fflush(stdout);
  __CMOST_COMMENT__ __bfs_kernel__edges_dst_buffer;
  __CMOST_COMMENT__ __event___bfs_kernel__edges_dst_buffer;
// __MERLIN_EVENT_WRITE__ __event___bfs_kernel__edges_dst_buffer
  int __MERLIN_EVENT_WRITE____event___bfs_kernel__edges_dst_buffer;
  opencl_write_buffer_nb(__bfs_kernel__edges_dst_buffer,commands[0],0 * sizeof(unsigned long ),(unsigned long *)__ti_edges_dst,((unsigned long )(4096000 * 1)) * sizeof(unsigned long ),&__event___bfs_kernel__edges_dst_buffer);
  
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "edges_dst");
  
#pragma ACCEL debug fflush(stdout);
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "starting_node", "1 * sizeof(unsigned long )");
    
#pragma ACCEL debug fflush(stdout);
    if (starting_node != 0) {
      __CMOST_COMMENT__ __bfs_kernel__starting_node_buffer;
      __CMOST_COMMENT__ __event___bfs_kernel__starting_node_buffer;
// __MERLIN_EVENT_WRITE__ __event___bfs_kernel__starting_node_buffer
      int __MERLIN_EVENT_WRITE____event___bfs_kernel__starting_node_buffer;
      opencl_write_buffer_nb(__bfs_kernel__starting_node_buffer,commands[0],0 * sizeof(unsigned long ),(unsigned long *)starting_node,((unsigned long )1) * sizeof(unsigned long ),&__event___bfs_kernel__starting_node_buffer);
    }
     else {
      printf("Error! The external memory pointed by 'starting_node' is invalid.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "starting_node");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "level", "256000 * sizeof(signed char )");
    
#pragma ACCEL debug fflush(stdout);
    if (level != 0) {
      __CMOST_COMMENT__ __bfs_kernel__level_buffer;
      __CMOST_COMMENT__ __event___bfs_kernel__level_buffer;
// __MERLIN_EVENT_WRITE__ __event___bfs_kernel__level_buffer
      int __MERLIN_EVENT_WRITE____event___bfs_kernel__level_buffer;
      opencl_write_buffer_nb(__bfs_kernel__level_buffer,commands[0],0 * sizeof(signed char ),(signed char *)level,((unsigned long )256000) * sizeof(signed char ),&__event___bfs_kernel__level_buffer);
    }
     else {
      printf("Error! The external memory pointed by 'level' is invalid.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "level");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "level_counts", "10000 * sizeof(unsigned long )");
    
#pragma ACCEL debug fflush(stdout);
    if (level_counts != 0) {
      __CMOST_COMMENT__ __bfs_kernel__level_counts_buffer;
      __CMOST_COMMENT__ __event___bfs_kernel__level_counts_buffer;
// __MERLIN_EVENT_WRITE__ __event___bfs_kernel__level_counts_buffer
      int __MERLIN_EVENT_WRITE____event___bfs_kernel__level_counts_buffer;
      opencl_write_buffer_nb(__bfs_kernel__level_counts_buffer,commands[0],0 * sizeof(unsigned long ),(unsigned long *)level_counts,((unsigned long )10000) * sizeof(unsigned long ),&__event___bfs_kernel__level_counts_buffer);
    }
     else {
      printf("Error! The external memory pointed by 'level_counts' is invalid.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "level_counts");
    
#pragma ACCEL debug fflush(stdout);
  }
}

void __merlin_read_buffer_bfs_kernel(struct node_t_struct *nodes,struct edge_t_struct *edges,unsigned long *starting_node,signed char *level,unsigned long *level_counts)
{
  static unsigned long __m_nodes_edge_begin[256000];
  static unsigned long __m_nodes_edge_end[256000];
  static unsigned long __m_edges_dst[4096000];
  static unsigned long __m_starting_node[1];
  static signed char __m_level[256000];
  static unsigned long __m_level_counts[10000];
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "level", "256000 * sizeof(signed char )");
    
#pragma ACCEL debug fflush(stdout);
    if (level != 0) {
      __CMOST_COMMENT__ __bfs_kernel__level_buffer;
      __CMOST_COMMENT__ __event___bfs_kernel__level_buffer;
// __MERLIN_EVENT_READ__ __event___bfs_kernel__level_buffer
      int __MERLIN_EVENT_READ____event___bfs_kernel__level_buffer;
      opencl_read_buffer_nb(__bfs_kernel__level_buffer,commands[0],0 * sizeof(signed char ),(signed char *)level,((unsigned long )256000) * sizeof(signed char ),&__event___bfs_kernel__level_buffer);
    }
     else {
      printf("Error! The external memory pointed by 'level' is invalid.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "level");
    
#pragma ACCEL debug fflush(stdout);
  }
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy out", "level_counts", "10000 * sizeof(unsigned long )");
    
#pragma ACCEL debug fflush(stdout);
    if (level_counts != 0) {
      __CMOST_COMMENT__ __bfs_kernel__level_counts_buffer;
      __CMOST_COMMENT__ __event___bfs_kernel__level_counts_buffer;
// __MERLIN_EVENT_READ__ __event___bfs_kernel__level_counts_buffer
      int __MERLIN_EVENT_READ____event___bfs_kernel__level_counts_buffer;
      opencl_read_buffer_nb(__bfs_kernel__level_counts_buffer,commands[0],0 * sizeof(unsigned long ),(unsigned long *)level_counts,((unsigned long )10000) * sizeof(unsigned long ),&__event___bfs_kernel__level_counts_buffer);
    }
     else {
      printf("Error! The external memory pointed by 'level_counts' is invalid.\n");
      exit(1);
    }
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy out", "level_counts");
    
#pragma ACCEL debug fflush(stdout);
  }
}

void __merlin_execute_bfs_kernel(struct node_t_struct *nodes,struct edge_t_struct *edges,unsigned long *starting_node,signed char *level,unsigned long *level_counts)
{
  int i;
{
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "bfs_kernel");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__bfs_kernel_kernel, 0, sizeof(opencl_mem), &__bfs_kernel__nodes_edge_begin_buffer);
	opencl_set_kernel_arg(__bfs_kernel_kernel, 1, sizeof(opencl_mem), &__bfs_kernel__nodes_edge_end_buffer);
	opencl_set_kernel_arg(__bfs_kernel_kernel, 2, sizeof(opencl_mem), &__bfs_kernel__edges_dst_buffer);
	opencl_set_kernel_arg(__bfs_kernel_kernel, 3, sizeof(opencl_mem), &__bfs_kernel__starting_node_buffer);
	opencl_set_kernel_arg(__bfs_kernel_kernel, 4, sizeof(opencl_mem), &__bfs_kernel__level_buffer);
	opencl_set_kernel_arg(__bfs_kernel_kernel, 5, sizeof(opencl_mem), &__bfs_kernel__level_counts_buffer);
	#pragma ACCEL debug printf("[Merlin Info] Finish set kernel %s argument....\n", "bfs_kernel");
	#pragma ACCEL debug fflush(stdout);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start execute kernel %s....\n", "bfs_kernel");
	#pragma ACCEL debug fflush(stdout);
	size_t __gid[1];
	__gid[0] = 1;
	opencl_enqueue_kernel(__bfs_kernel_kernel, commands[0], 1, __gid, &__event_bfs_kernel);
}
}
