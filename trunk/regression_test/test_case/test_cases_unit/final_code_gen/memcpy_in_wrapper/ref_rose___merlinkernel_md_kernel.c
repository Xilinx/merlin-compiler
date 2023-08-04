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
static void __merlin_dummy_kernel_pragma();
/* Original: #pragma ACCEL kernel */

void merlin_assign_dvector_t_0(double *lhs_x,double *lhs_y,double *lhs_z,double rhs_x,double rhs_y,double rhs_z)
{
   *lhs_x = rhs_x;
   *lhs_y = rhs_y;
   *lhs_z = rhs_z;
}

void merlin_assign_dvector_t_1(double *lhs_x,double *lhs_y,double *lhs_z,double rhs_x,double rhs_y,double rhs_z)
{
   *lhs_x = rhs_x;
   *lhs_y = rhs_y;
   *lhs_z = rhs_z;
}

__kernel void md_kernel(__global int * restrict n_points,__global double * restrict force_x,__global double * restrict force_y,__global double * restrict force_z,__global double * restrict position_x,__global double * restrict position_y,__global double * restrict position_z)
{
  int n_points_buf_0[64];
  long _memcpy_i_0;
  
#pragma unroll 16
  for (_memcpy_i_0 = 0; _memcpy_i_0 < sizeof(int ) * ((unsigned long )64) / 4; ++_memcpy_i_0) {
    long total_offset1 = (0 * 64 + 0);
    long total_offset2 = (0 * 0 + 0);
    n_points_buf_0[total_offset1 + _memcpy_i_0] = n_points[total_offset2 + _memcpy_i_0];
  }
  double force_z_buf_0[640];
  long _memcpy_i_5;
  
#pragma unroll 8
  for (_memcpy_i_5 = 0; _memcpy_i_5 < sizeof(double ) * ((unsigned long )640) / 8; ++_memcpy_i_5) {
    long total_offset1 = (0 * 640 + 0);
    long total_offset2 = (0 * 0 + 0);
    force_z_buf_0[total_offset1 + _memcpy_i_5] = force_z[total_offset2 + _memcpy_i_5];
  }
  double force_y_buf_0[640];
  long _memcpy_i_3;
  
#pragma unroll 8
  for (_memcpy_i_3 = 0; _memcpy_i_3 < sizeof(double ) * ((unsigned long )640) / 8; ++_memcpy_i_3) {
    long total_offset1 = (0 * 640 + 0);
    long total_offset2 = (0 * 0 + 0);
    force_y_buf_0[total_offset1 + _memcpy_i_3] = force_y[total_offset2 + _memcpy_i_3];
  }
  double force_x_buf_0[640];
  long _memcpy_i_1;
  
#pragma unroll 8
  for (_memcpy_i_1 = 0; _memcpy_i_1 < sizeof(double ) * ((unsigned long )640) / 8; ++_memcpy_i_1) {
    long total_offset1 = (0 * 640 + 0);
    long total_offset2 = (0 * 0 + 0);
    force_x_buf_0[total_offset1 + _memcpy_i_1] = force_x[total_offset2 + _memcpy_i_1];
  }
  double position_z_buf_0[640];
  long _memcpy_i_9;
  
#pragma unroll 8
  for (_memcpy_i_9 = 0; _memcpy_i_9 < sizeof(double ) * ((unsigned long )640) / 8; ++_memcpy_i_9) {
    long total_offset1 = (0 * 640 + 0);
    long total_offset2 = (0 * 0 + 0);
    position_z_buf_0[total_offset1 + _memcpy_i_9] = position_z[total_offset2 + _memcpy_i_9];
  }
  double position_y_buf_0[640];
  long _memcpy_i_8;
  
#pragma unroll 8
  for (_memcpy_i_8 = 0; _memcpy_i_8 < sizeof(double ) * ((unsigned long )640) / 8; ++_memcpy_i_8) {
    long total_offset1 = (0 * 640 + 0);
    long total_offset2 = (0 * 0 + 0);
    position_y_buf_0[total_offset1 + _memcpy_i_8] = position_y[total_offset2 + _memcpy_i_8];
  }
  double position_x_buf_0[640];
  long _memcpy_i_7;
  
#pragma unroll 8
  for (_memcpy_i_7 = 0; _memcpy_i_7 < sizeof(double ) * ((unsigned long )640) / 8; ++_memcpy_i_7) {
    long total_offset1 = (0 * 640 + 0);
    long total_offset2 = (0 * 0 + 0);
    position_x_buf_0[total_offset1 + _memcpy_i_7] = position_x[total_offset2 + _memcpy_i_7];
  }
  
#pragma ACCEL INTERFACE VARIABLE=position_z max_depth=4,4,4,10 DEPTH=4,4,4,10
  
#pragma ACCEL INTERFACE VARIABLE=position_y max_depth=4,4,4,10 DEPTH=4,4,4,10
  
#pragma ACCEL INTERFACE VARIABLE=position_x max_depth=4,4,4,10 DEPTH=4,4,4,10
  
#pragma ACCEL INTERFACE VARIABLE=force_z max_depth=4,4,4,10 DEPTH=4,4,4,10
  
#pragma ACCEL INTERFACE VARIABLE=force_y max_depth=4,4,4,10 DEPTH=4,4,4,10
  
#pragma ACCEL INTERFACE VARIABLE=force_x max_depth=4,4,4,10 DEPTH=4,4,4,10
  
#pragma ACCEL interface variable=n_points max_depth=4,4,4 depth=4,4,4
  int b0_z;
  int b0_y;
  int b0_x;
  int b1_z;
  int b1_y;
  int b1_x;
  double p_z;
  double p_y;
  double p_x;
  double q_z;
  double q_y;
  double q_x;
  int p_idx;
  int q_idx;
  double dx;
  double dy;
  double dz;
  double r2inv;
  double r6inv;
  double potential;
  double f;
// Iterate over the grid, block by block
  loop_grid0_x:
{
  }
  for (b0_x = 0; b0_x < 4; b0_x++) {
    loop_grid0_y:
{
    }
    for (b0_y = 0; b0_y < 4; b0_y++) {
      loop_grid0_z:
{
      }
      for (b0_z = 0; b0_z < 4; b0_z++) {
// Iterate over the 3x3x3 (modulo boundary conditions) cube of blocks around b0
        loop_grid1_x:
{
        }
        for (b1_x = (0 > b0_x - 1?0 : b0_x - 1); b1_x < ((4 < b0_x + 2?4 : b0_x + 2)); b1_x++) {
          loop_grid1_y:
{
          }
          for (b1_y = (0 > b0_y - 1?0 : b0_y - 1); b1_y < ((4 < b0_y + 2?4 : b0_y + 2)); b1_y++) {
            loop_grid1_z:
{
            }
            for (b1_z = (0 > b0_z - 1?0 : b0_z - 1); b1_z < ((4 < b0_z + 2?4 : b0_z + 2)); b1_z++) {
              int q_idx_range = n_points_buf_0[b1_x * 16 + b1_y * 4 + b1_z];
              loop_p:
{
              }
              for (p_idx = 0; p_idx < n_points_buf_0[b0_x * 16 + b0_y * 4 + b0_z]; p_idx++) {
                merlin_assign_dvector_t_0(&p_x,&p_y,&p_z,position_x_buf_0[b0_x * 160 + b0_y * 40 + b0_z * 10 + p_idx],position_y_buf_0[b0_x * 160 + b0_y * 40 + b0_z * 10 + p_idx],position_z_buf_0[b0_x * 160 + b0_y * 40 + b0_z * 10 + p_idx]);
                double sum_x = force_x_buf_0[b0_x * 160 + b0_y * 40 + b0_z * 10 + p_idx];
                double sum_y = force_y_buf_0[b0_x * 160 + b0_y * 40 + b0_z * 10 + p_idx];
                double sum_z = force_z_buf_0[b0_x * 160 + b0_y * 40 + b0_z * 10 + p_idx];
// For all points in b1
                loop_q:
{
                }
                for (q_idx = 0; q_idx < q_idx_range; q_idx++) {
                  merlin_assign_dvector_t_1(&q_x,&q_y,&q_z,position_x_buf_0[b1_x * 160 + b1_y * 40 + b1_z * 10 + q_idx],position_y_buf_0[b1_x * 160 + b1_y * 40 + b1_z * 10 + q_idx],position_z_buf_0[b1_x * 160 + b1_y * 40 + b1_z * 10 + q_idx]);
// Don't compute our own
                  if (q_x != p_x || q_y != p_y || q_z != p_z) {
// Compute the LJ-potential
                    dx = p_x - q_x;
                    dy = p_y - q_y;
                    dz = p_z - q_z;
                    r2inv = 1.0 / (dx * dx + dy * dy + dz * dz);
                    r6inv = r2inv * r2inv * r2inv;
                    potential = r6inv * (1.5 * r6inv - 2.0);
// Update forces
                    f = r2inv * potential;
                    sum_x += f * dx;
                    sum_y += f * dy;
                    sum_z += f * dz;
                  }
// loop_q
                }
                force_x_buf_0[b0_x * 160 + b0_y * 40 + b0_z * 10 + p_idx] = sum_x;
                force_y_buf_0[b0_x * 160 + b0_y * 40 + b0_z * 10 + p_idx] = sum_y;
                force_z_buf_0[b0_x * 160 + b0_y * 40 + b0_z * 10 + p_idx] = sum_z;
// loop_p
              }
// loop_grid1_*
            }
          }
        }
// loop_grid0_*
      }
    }
  }
  long _memcpy_i_2;
  
#pragma unroll 8
  for (_memcpy_i_2 = 0; _memcpy_i_2 < sizeof(double ) * ((unsigned long )640) / 8; ++_memcpy_i_2) {
    long total_offset1 = (0 * 0 + 0);
    long total_offset2 = (0 * 640 + 0);
    force_x[total_offset1 + _memcpy_i_2] = force_x_buf_0[total_offset2 + _memcpy_i_2];
  }
  long _memcpy_i_4;
  
#pragma unroll 8
  for (_memcpy_i_4 = 0; _memcpy_i_4 < sizeof(double ) * ((unsigned long )640) / 8; ++_memcpy_i_4) {
    long total_offset1 = (0 * 0 + 0);
    long total_offset2 = (0 * 640 + 0);
    force_y[total_offset1 + _memcpy_i_4] = force_y_buf_0[total_offset2 + _memcpy_i_4];
  }
  long _memcpy_i_6;
  
#pragma unroll 8
  for (_memcpy_i_6 = 0; _memcpy_i_6 < sizeof(double ) * ((unsigned long )640) / 8; ++_memcpy_i_6) {
    long total_offset1 = (0 * 0 + 0);
    long total_offset2 = (0 * 640 + 0);
    force_z[total_offset1 + _memcpy_i_6] = force_z_buf_0[total_offset2 + _memcpy_i_6];
  }
}
