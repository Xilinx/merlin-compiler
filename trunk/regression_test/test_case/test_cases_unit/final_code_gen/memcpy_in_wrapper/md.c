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
#include "md.h"
#define MIN(x,y) ( (x)<(y) ? (x) : (y) )
#define MAX(x,y) ( (x)>(y) ? (x) : (y) )
static void __merlin_dummy_kernel_pragma();

void md_kernel(int n_points[4][4][4],dvector_t force[4][4][4][10],dvector_t position[4][4][4][10])
{
  
#pragma ACCEL interface variable=position depth=4,4,4,10
  
#pragma ACCEL interface variable=force depth=4,4,4,10
  
#pragma ACCEL interface variable=n_points depth=4,4,4
// b0 is the current block, b1 is b0 or a neighboring block
  ivector_t b0;
  ivector_t b1;
// p is a point in b0, q is a point in either b0 or b1
  dvector_t p;
  dvector_t q;
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
  for (b0 . x = 0; b0 . x < 4; b0 . x++) {
    loop_grid0_y:
    for (b0 . y = 0; b0 . y < 4; b0 . y++) {
      loop_grid0_z:
      for (b0 . z = 0; b0 . z < 4; b0 . z++) {
// Iterate over the 3x3x3 (modulo boundary conditions) cube of blocks around b0
        loop_grid1_x:
        for (b1 . x = (0 > b0 . x - 1?0 : b0 . x - 1); b1 . x < ((4 < b0 . x + 2?4 : b0 . x + 2)); b1 . x++) {
          loop_grid1_y:
          for (b1 . y = (0 > b0 . y - 1?0 : b0 . y - 1); b1 . y < ((4 < b0 . y + 2?4 : b0 . y + 2)); b1 . y++) {
            loop_grid1_z:
            for (b1 . z = (0 > b0 . z - 1?0 : b0 . z - 1); b1 . z < ((4 < b0 . z + 2?4 : b0 . z + 2)); b1 . z++) {
// For all points in b0
              dvector_t *base_q = position[b1 . x][b1 . y][b1 . z];
              int q_idx_range = n_points[b1 . x][b1 . y][b1 . z];
              loop_p:
              for (p_idx = 0; p_idx < n_points[b0 . x][b0 . y][b0 . z]; p_idx++) {
                p = position[b0 . x][b0 . y][b0 . z][p_idx];
                double sum_x = force[b0 . x][b0 . y][b0 . z][p_idx] . x;
                double sum_y = force[b0 . x][b0 . y][b0 . z][p_idx] . y;
                double sum_z = force[b0 . x][b0 . y][b0 . z][p_idx] . z;
// For all points in b1
                loop_q:
                for (q_idx = 0; q_idx < q_idx_range; q_idx++) {
                  q =  *(base_q + q_idx);
// Don't compute our own
                  if (q . x != p . x || q . y != p . y || q . z != p . z) {
// Compute the LJ-potential
                    dx = p . x - q . x;
                    dy = p . y - q . y;
                    dz = p . z - q . z;
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
                force[b0 . x][b0 . y][b0 . z][p_idx] . x = sum_x;
                force[b0 . x][b0 . y][b0 . z][p_idx] . y = sum_y;
                force[b0 . x][b0 . y][b0 . z][p_idx] . z = sum_z;
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
}
