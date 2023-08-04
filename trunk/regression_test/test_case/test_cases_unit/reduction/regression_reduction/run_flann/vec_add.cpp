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
#include "ap_int.h"
#include "cmost.h"
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "config.h"
#include <ap_fixed.h>
//
//#define query_row_num 9665
#define query_row_num 9600
#define query_col_num 3
#define idx_row_num 9665
#define idx_col_num 1
#define dist_row_num 9665
#define dist_col_num 1
//#define target_row_num  18213
#define target_row_num 18176
#define target_col_num 3
#define MAX_VISITED_NODES 100
typedef class ap_fixed< 32 , 16 , AP_TRN , AP_SAT , 0 > in_t;
void vec_add_kernel(ap_uint<512> *target_x,ap_uint<512> *target_y,ap_uint<512> *target_z,ap_uint<512> *query_x,ap_uint<512> *query_y,ap_uint<512> *query_z,ap_uint<512> *index);
// inputs

void sw_search_kdtree(int root,float x,float y,float z,float *min_dist,int *min_idx,int *tree_idx,int *tree_depth,float *tree_x,float *tree_y,float *tree_z,int *tree_left_idx,int *tree_right_idx,int *nodes_to_visit_stack,int *num_nodes_to_visit)
{
  float local_dist;
  local_dist = (pow((x - tree_x[root]),2) + pow((y - tree_y[root]),2) + pow((z - tree_z[root]),2));
  local_dist = (sqrt(local_dist));
  int num_visited_nodes_local = 0;
// it will not be more than tree heigh: 15
  int visited_nodes_local[100];
  if (local_dist <  *min_dist) {
     *min_dist = local_dist;
     *min_idx = root;
  }
  int node_idx = root;
  while(1){
// printf ("node idx is %d\n", node_idx);
    visited_nodes_local[num_visited_nodes_local] = node_idx;
    num_visited_nodes_local++;
// x
    if (tree_depth[node_idx] % 3 == 0) {
      if (x < tree_x[node_idx]) {
        node_idx = tree_left_idx[node_idx];
        if (node_idx == - 1) {
          break; 
        }
        local_dist = (pow((x - tree_x[node_idx]),2) + pow((y - tree_y[node_idx]),2) + pow((z - tree_z[node_idx]),2));
        local_dist = (sqrt(local_dist));
        if (local_dist <  *min_dist) {
           *min_dist = local_dist;
           *min_idx = node_idx;
        }
      }
       else {
        node_idx = tree_right_idx[node_idx];
        if (node_idx == - 1) {
          break; 
        }
        local_dist = (pow((x - tree_x[node_idx]),2) + pow((y - tree_y[node_idx]),2) + pow((z - tree_z[node_idx]),2));
        local_dist = (sqrt(local_dist));
        if (local_dist <  *min_dist) {
           *min_dist = local_dist;
           *min_idx = node_idx;
        }
      }
// y
    }
     else {
      if (tree_depth[node_idx] % 3 == 1) {
        if (y < tree_y[node_idx]) {
          node_idx = tree_left_idx[node_idx];
          if (node_idx == - 1) {
            break; 
          }
          local_dist = (pow((x - tree_x[node_idx]),2) + pow((y - tree_y[node_idx]),2) + pow((z - tree_z[node_idx]),2));
          local_dist = (sqrt(local_dist));
          if (local_dist <  *min_dist) {
             *min_dist = local_dist;
             *min_idx = node_idx;
          }
        }
         else {
          node_idx = tree_right_idx[node_idx];
          if (node_idx == - 1) {
            break; 
          }
          local_dist = (pow((x - tree_x[node_idx]),2) + pow((y - tree_y[node_idx]),2) + pow((z - tree_z[node_idx]),2));
          local_dist = (sqrt(local_dist));
          if (local_dist <  *min_dist) {
             *min_dist = local_dist;
             *min_idx = node_idx;
          }
        }
// z
      }
       else {
        if (tree_depth[node_idx] % 3 == 2) {
          if (z < tree_z[node_idx]) {
            node_idx = tree_left_idx[node_idx];
            if (node_idx == - 1) {
              break; 
            }
            local_dist = (pow((x - tree_x[node_idx]),2) + pow((y - tree_y[node_idx]),2) + pow((z - tree_z[node_idx]),2));
            local_dist = (sqrt(local_dist));
            if (local_dist <  *min_dist) {
               *min_dist = local_dist;
               *min_idx = node_idx;
            }
          }
           else {
            node_idx = tree_right_idx[node_idx];
            if (node_idx == - 1) {
              break; 
            }
            local_dist = (pow((x - tree_x[node_idx]),2) + pow((y - tree_y[node_idx]),2) + pow((z - tree_z[node_idx]),2));
            local_dist = (sqrt(local_dist));
            if (local_dist <  *min_dist) {
               *min_dist = local_dist;
               *min_idx = node_idx;
            }
          }
        }
      }
    }
// end while(1) loop
  }
// now, find the nodes that need to be visited
  int i;
  for (i = 0; i < num_visited_nodes_local; i++) {
    int node_idx = visited_nodes_local[i];
    float diff;
// x
    if (tree_depth[node_idx] % 3 == 0) {
      diff = x - tree_x[node_idx];
      if (diff < 0) {
        diff = -diff;
      }
      if ( *min_dist >= diff) {
        if (x < tree_x[node_idx]) {
          int idx_to_add = tree_right_idx[node_idx];
          if (idx_to_add != - 1) {
            nodes_to_visit_stack[ *num_nodes_to_visit] = idx_to_add;
            ( *num_nodes_to_visit)++;
          }
        }
         else {
          if (x >= tree_x[node_idx]) {
            int idx_to_add = tree_left_idx[node_idx];
            if (idx_to_add != - 1) {
              nodes_to_visit_stack[ *num_nodes_to_visit] = idx_to_add;
              ( *num_nodes_to_visit)++;
            }
          }
        }
      }
// y
    }
     else {
      if (tree_depth[node_idx] % 3 == 1) {
        diff = y - tree_y[node_idx];
        if (diff < 0) {
          diff = -diff;
        }
        if ( *min_dist >= diff) {
          if (y < tree_y[node_idx]) {
            int idx_to_add = tree_right_idx[node_idx];
            if (idx_to_add != - 1) {
              nodes_to_visit_stack[ *num_nodes_to_visit] = idx_to_add;
              ( *num_nodes_to_visit)++;
            }
          }
           else {
            if (y >= tree_y[node_idx]) {
              int idx_to_add = tree_left_idx[node_idx];
              if (idx_to_add != - 1) {
                nodes_to_visit_stack[ *num_nodes_to_visit] = idx_to_add;
                ( *num_nodes_to_visit)++;
              }
            }
          }
        }
// z
      }
       else {
        if (tree_depth[node_idx] % 3 == 2) {
          diff = z - tree_z[node_idx];
          if (diff < 0) {
            diff = -diff;
          }
          if ( *min_dist >= diff) {
            if (z < tree_z[node_idx]) {
              int idx_to_add = tree_right_idx[node_idx];
              if (idx_to_add != - 1) {
                nodes_to_visit_stack[ *num_nodes_to_visit] = idx_to_add;
                ( *num_nodes_to_visit)++;
              }
            }
             else {
              if (z >= tree_z[node_idx]) {
                int idx_to_add = tree_left_idx[node_idx];
                if (idx_to_add != - 1) {
                  nodes_to_visit_stack[ *num_nodes_to_visit] = idx_to_add;
                  ( *num_nodes_to_visit)++;
                }
              }
            }
          }
        }
      }
    }
// end for loop
  }
  return ;
}

void sw_NNSearch(
// inputs
float *query,float *target,int *tree_idx,int *tree_depth,float *tree_x,float *tree_y,float *tree_z,int *tree_left_idx,int *tree_right_idx,
// outputs
int *idx)
{
// create some local buffers
// for inputs
  float query_local[9600 * 3];
  float target_local[18176 * 3];
  int tree_idx_local[18176];
  int tree_depth_local[18176];
  float tree_x_local[18176];
  float tree_y_local[18176];
  float tree_z_local[18176];
  int tree_left_idx_local[18176];
  int tree_right_idx_local[18176];
// for outputs
  int output_idx_local[9600 * 1];
// memcopy to local storage
  memcpy(query_local,query,(9600 * 3) * sizeof(float ));
  memcpy(target_local,target,(18176 * 3) * sizeof(float ));
// memcopy KDTree data into FPGA BRAM
  memcpy(tree_idx_local,tree_idx,18176 * sizeof(int ));
  memcpy(tree_depth_local,tree_depth,18176 * sizeof(int ));
  memcpy(tree_x_local,tree_x,18176 * sizeof(float ));
  memcpy(tree_y_local,tree_y,18176 * sizeof(float ));
  memcpy(tree_z_local,tree_z,18176 * sizeof(float ));
  memcpy(tree_left_idx_local,tree_left_idx,18176 * sizeof(int ));
  memcpy(tree_right_idx_local,tree_right_idx,18176 * sizeof(int ));
  int i;
// for (i = 0; i < query_row_num; i++)
  for (i = 0; i < 9600; i++) {
    float x = query_local[3 * i + 0];
    float y = query_local[3 * i + 1];
    float z = query_local[3 * i + 2];
    int min_idx = - 1;
    float min_dist = 999999999999;
// printf ("Calling search_kdtree now ...\n");
// root of the total KDTree
    int root_idx = 1819;
    int nodes_to_visit_stack[18176];
    int num_nodes_to_visit = 1;
    nodes_to_visit_stack[0] = root_idx;
    int node_idx;
    int total_nodes_to_visit = 0;
    while(num_nodes_to_visit > 0){
      total_nodes_to_visit++;
      node_idx = nodes_to_visit_stack[num_nodes_to_visit - 1];
      num_nodes_to_visit--;
      sw_search_kdtree(node_idx,x,y,z,&min_dist,&min_idx,tree_idx_local,tree_depth_local,tree_x_local,tree_y_local,tree_z_local,tree_left_idx_local,tree_right_idx_local,nodes_to_visit_stack,&num_nodes_to_visit);
    }
    output_idx_local[i] = min_idx;
// printf ("Total nodes to visit: %d\n", total_nodes_to_visit);
  }
// memcpy to DRAM
  memcpy(idx,output_idx_local,9600 * sizeof(int ));
  return ;
}

int main()
{
// allocate storage for inputs
  float *query;
  cmost_malloc(((void **)(&query)),9600 * 3 * 4);
  float *target;
  cmost_malloc(((void **)(&target)),18176 * 3 * 4);
  int *query_x_hw;
  cmost_malloc(((void **)(&query_x_hw)),9600 * 3 / 3 * 4);
  int *query_y_hw;
  cmost_malloc(((void **)(&query_y_hw)),9600 * 3 / 3 * 4);
  int *query_z_hw;
  cmost_malloc(((void **)(&query_z_hw)),9600 * 3 / 3 * 4);
  int *target_x_hw;
  cmost_malloc(((void **)(&target_x_hw)),18176 * 3 / 3 * 4);
  int *target_y_hw;
  cmost_malloc(((void **)(&target_y_hw)),18176 * 3 / 3 * 4);
  int *target_z_hw;
  cmost_malloc(((void **)(&target_z_hw)),18176 * 3 / 3 * 4);
// allocate storage for outputs
  int *idx;
  cmost_malloc(((void **)(&idx)),9665 * 1 * 4);
// allocate storage for SW version
  int *tree_idx;
  cmost_malloc(((void **)(&tree_idx)),18176 * 4);
  int *tree_depth;
  cmost_malloc(((void **)(&tree_depth)),18176 * 4);
  int *tree_left_idx;
  cmost_malloc(((void **)(&tree_left_idx)),18176 * 4);
  int *tree_right_idx;
  cmost_malloc(((void **)(&tree_right_idx)),18176 * 4);
  float *tree_x;
  cmost_malloc(((void **)(&tree_x)),18176 * 4);
  float *tree_y;
  cmost_malloc(((void **)(&tree_y)),18176 * 4);
  float *tree_z;
  cmost_malloc(((void **)(&tree_z)),18176 * 4);
  int *sw_idx = (int *)(malloc(9665 * sizeof(int )));
  query && target && tree_idx && tree_depth && tree_left_idx && tree_right_idx && tree_x && tree_y && tree_z && idx && sw_idx?((void )0) : __assert_fail("query && target && tree_idx && tree_depth && tree_left_idx && tree_right_idx && tree_x && tree_y && tree_z && idx && sw_idx","vec_add.cpp",350,__PRETTY_FUNCTION__);
// read from input.dat files
  cmost_load_file(query,"query.dat",9600 * 3 * 4);
// cmost_load_file(target, "target.dat" , target_row_num * target_col_num*4);
  cmost_load_file(target,"target.dat",18176 * 3 * 4);
  cmost_load_file(tree_idx,"tree_index.dat",18176 * 4);
  cmost_load_file(tree_depth,"tree_depth.dat",18176 * 4);
  cmost_load_file(tree_left_idx,"tree_left_index.dat",18176 * 4);
  cmost_load_file(tree_right_idx,"tree_right_index.dat",18176 * 4);
  cmost_load_file(tree_x,"tree_x.dat",18176 * 4);
  cmost_load_file(tree_y,"tree_y.dat",18176 * 4);
  cmost_load_file(tree_z,"tree_z.dat",18176 * 4);
// convert floating data into fixed data
  for (int i = 0; i != 18176 * 3 / 3; ++i) {
    in_t target_fixed(target[i * 3 + 0]);
    target_x_hw[i] = target_fixed . range();
    target_fixed = target[i * 3 + 1];
    target_y_hw[i] = target_fixed . range();
    target_fixed = target[i * 3 + 2];
    target_z_hw[i] = target_fixed . range();
  }
  for (int i = 0; i != 9600 * 3 / 3; ++i) {
    in_t query_fixed(query[i * 3 + 0]);
    query_x_hw[i] = query_fixed . range();
    query_fixed = query[i * 3 + 1];
    query_y_hw[i] = query_fixed . range();
    query_fixed = query[i * 3 + 2];
    query_z_hw[i] = query_fixed . range();
  }
  printf("Calling FLANN FPGA version now ...\n");
  

  vec_add_kernel(((ap_uint<512> *)target_x_hw),((ap_uint<512> *)target_y_hw),((ap_uint<512> *)target_z_hw),((ap_uint<512> *)query_x_hw),((ap_uint<512> *)query_y_hw),((ap_uint<512> *)query_z_hw),((ap_uint<512> *)idx));
// inputs
// outputs
  cmost_write_file(idx,"idx_out.dat",4 * 9665 * 1);
  cmost_dump(idx,"idx_out.dat");
// calling the software version now ...
  printf("Calling FLANN software version now ...\n");
#ifdef PRINT_TIMER
#endif
  sw_NNSearch(query,target,tree_idx,tree_depth,tree_x,tree_y,tree_z,tree_left_idx,tree_right_idx,sw_idx);
// inputs
// outputs
#ifdef PRINT_TIMER
#endif
// comparing the data now ...
#if 1
  int i;
  int err_cnt = 0;
// for (i = 0; i < query_row_num; i++)
  for (i = 0; i < 9600; i++) {
    if (sw_idx[i] != idx[i] && err_cnt < 10) {
      printf("Error here @ idx: %d\tFPGA output is %d\tSW output is %d\n",i,idx[i],sw_idx[i]);
      err_cnt++;
    }
    if (sw_idx[i] != idx[i] && err_cnt == 10) {
      printf("...\n");
      err_cnt++;
    }
  }
  if (err_cnt == 0) {
    printf("\nPassed.\n\n");
  }
#endif
  cmost_free(target);
  cmost_free(query);
  cmost_free(target_x_hw);
  cmost_free(target_y_hw);
  cmost_free(target_z_hw);
  cmost_free(query_x_hw);
  cmost_free(query_y_hw);
  cmost_free(query_z_hw);
  cmost_free(tree_idx);
  cmost_free(tree_depth);
  cmost_free(tree_left_idx);
  cmost_free(tree_right_idx);
  cmost_free(tree_x);
  cmost_free(tree_y);
  cmost_free(tree_z);
  cmost_free(idx);
  free(sw_idx);
  return 0;
}
