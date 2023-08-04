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
{
    
#pragma ACCEL wrapper variable=nodes->edge_begin port=nodes_edge_begin depth=256000 max_depth=256000 data_type="unsigned long" io=RO copy=false
    
#pragma ACCEL wrapper variable=nodes->edge_end port=nodes_edge_end depth=256000 max_depth=256000 data_type="unsigned long" io=RO copy=false
    
#pragma ACCEL wrapper variable=edges->dst port=edges_dst depth=4096000 max_depth=4096000 data_type="unsigned long" io=RO copy=false
    
#pragma ACCEL wrapper variable=starting_node port=starting_node depth=1 max_depth=1 data_type="unsigned long" io=RO copy=true
    
#pragma ACCEL wrapper variable=level port=level depth=256000 max_depth=256000 data_type="signed char" io=RW copy=false
    
#pragma ACCEL wrapper variable=level_counts port=level_counts depth=10000 max_depth=10000 data_type="unsigned long" io=RW copy=false
  }
  static unsigned long __m_nodes_edge_begin[256000];
  static unsigned long __m_nodes_edge_end[256000];
  static unsigned long __m_edges_dst[4096000];
  static unsigned long __m_starting_node[1];
  static signed char __m_level[256000];
  static unsigned long __m_level_counts[10000];
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
    posix_memalign(((void **)(&__ti_nodes_edge_begin)),64,sizeof(unsigned long ) * 256000);
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
    memcpy(((void *)__m_nodes_edge_begin),((const void *)__ti_nodes_edge_begin),(256000 * 1) * sizeof(unsigned long ));
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "nodes_edge_begin");
    
#pragma ACCEL debug fflush(stdout);
    static unsigned long *__ti_nodes_edge_end;
    posix_memalign(((void **)(&__ti_nodes_edge_end)),64,sizeof(unsigned long ) * 256000);
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
    memcpy(((void *)__m_nodes_edge_end),((const void *)__ti_nodes_edge_end),(256000 * 1) * sizeof(unsigned long ));
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "nodes_edge_end");
    
#pragma ACCEL debug fflush(stdout);
    static unsigned long *__ti_edges_dst;
    posix_memalign(((void **)(&__ti_edges_dst)),64,sizeof(unsigned long ) * 4096000);
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
    memcpy(((void *)__m_edges_dst),((const void *)__ti_edges_dst),(4096000 * 1) * sizeof(unsigned long ));
    
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "edges_dst");
    
#pragma ACCEL debug fflush(stdout);
{
      
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "starting_node", "1 * sizeof(unsigned long )");
      
#pragma ACCEL debug fflush(stdout);
      if (starting_node != 0) {
        memcpy(((void *)(__m_starting_node + 0)),((const void *)starting_node),1 * sizeof(unsigned long ));
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
        memcpy(((void *)(__m_level + 0)),((const void *)level),256000 * sizeof(signed char ));
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
        memcpy(((void *)(__m_level_counts + 0)),((const void *)level_counts),10000 * sizeof(unsigned long ));
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
  bfs_kernel(__m_nodes_edge_begin,__m_nodes_edge_end,__m_edges_dst,__m_starting_node,__m_level,__m_level_counts);
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
        memcpy(((void *)level),((const void *)(__m_level + 0)),256000 * sizeof(signed char ));
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
        memcpy(((void *)level_counts),((const void *)(__m_level_counts + 0)),10000 * sizeof(unsigned long ));
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
  posix_memalign(((void **)(&__ti_nodes_edge_begin)),64,sizeof(unsigned long ) * 256000);
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
  memcpy(((void *)__m_nodes_edge_begin),((const void *)__ti_nodes_edge_begin),(256000 * 1) * sizeof(unsigned long ));
  
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "nodes_edge_begin");
  
#pragma ACCEL debug fflush(stdout);
  static unsigned long *__ti_nodes_edge_end;
  posix_memalign(((void **)(&__ti_nodes_edge_end)),64,sizeof(unsigned long ) * 256000);
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
  memcpy(((void *)__m_nodes_edge_end),((const void *)__ti_nodes_edge_end),(256000 * 1) * sizeof(unsigned long ));
  
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "nodes_edge_end");
  
#pragma ACCEL debug fflush(stdout);
  static unsigned long *__ti_edges_dst;
  posix_memalign(((void **)(&__ti_edges_dst)),64,sizeof(unsigned long ) * 4096000);
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
  memcpy(((void *)__m_edges_dst),((const void *)__ti_edges_dst),(4096000 * 1) * sizeof(unsigned long ));
  
#pragma ACCEL debug printf("[Merlin Info] Finish %s data for %s.\n", "copy in", "edges_dst");
  
#pragma ACCEL debug fflush(stdout);
{
    
#pragma ACCEL debug printf("[Merlin Info] Start %s data for %s, data size = %s...\n", "copy in", "starting_node", "1 * sizeof(unsigned long )");
    
#pragma ACCEL debug fflush(stdout);
    if (starting_node != 0) {
      memcpy(((void *)(__m_starting_node + 0)),((const void *)starting_node),1 * sizeof(unsigned long ));
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
      memcpy(((void *)(__m_level + 0)),((const void *)level),256000 * sizeof(signed char ));
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
      memcpy(((void *)(__m_level_counts + 0)),((const void *)level_counts),10000 * sizeof(unsigned long ));
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
      memcpy(((void *)level),((const void *)(__m_level + 0)),256000 * sizeof(signed char ));
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
      memcpy(((void *)level_counts),((const void *)(__m_level_counts + 0)),10000 * sizeof(unsigned long ));
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
}
