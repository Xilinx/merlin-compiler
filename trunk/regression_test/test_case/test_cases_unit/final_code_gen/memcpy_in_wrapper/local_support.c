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
#include <string.h>
int INPUT_SIZE = (sizeof(struct bench_args_t ));
void md_kernel(int n_points[4][4][4],dvector_t force[4][4][4][10],dvector_t position[4][4][4][10]);
extern int __merlin_include__LB___merlinhead_kernel_top_h_0;
#include "__merlinhead_kernel_top.h"
extern int __merlin_include__LE___merlinhead_kernel_top_h_0;
extern int __merlin_include__LB___merlintask_top_h_1;
#include "__merlintask_top.h"
extern int __merlin_include__LE___merlintask_top_h_1;

void run_benchmark(void *vargs)
{
  struct bench_args_t *args = (struct bench_args_t *)vargs;
  
#pragma ACCEL task name="md"
{
    __merlin_init("kernel_top.aocx");
    md(args);
  }
}
/* Input format:
%% Section 1
int32_t[blockSide^3]: grid populations
%% Section 2
TYPE[blockSide^3*densityFactor]: positions
*/

void input_to_data(int fd,void *vdata)
{
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  char *p;
  char *s;
// Zero-out everything.
  memset(vdata,0,sizeof(struct bench_args_t ));
// Load input string
  p = readfile(fd);
  s = find_section_start(p,1);
  parse_int32_t_array(s,((int *)(data -> n_points)),4 * 4 * 4);
  s = find_section_start(p,2);
  parse_double_array(s,((double *)(data -> position)),3 * 4 * 4 * 4 * 10);
}

void data_to_input(int fd,void *vdata)
{
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  write_section_header(fd);
  write_int32_t_array(fd,((int *)(data -> n_points)),4 * 4 * 4);
  write_section_header(fd);
  write_double_array(fd,((double *)(data -> position)),3 * 4 * 4 * 4 * 10);
}
/* Output format:
%% Section 1
TYPE[blockSide^3*densityFactor]: force
*/

void output_to_data(int fd,void *vdata)
{
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  char *p;
  char *s;
// Zero-out everything.
  memset(vdata,0,sizeof(struct bench_args_t ));
// Load input string
  p = readfile(fd);
  s = find_section_start(p,1);
  parse_double_array(s,((double *)(data -> force)),3 * 4 * 4 * 4 * 10);
}

void data_to_output(int fd,void *vdata)
{
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  write_section_header(fd);
  write_double_array(fd,((double *)(data -> force)),3 * 4 * 4 * 4 * 10);
}

int check_data(void *vdata,void *vref)
{
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  struct bench_args_t *ref = (struct bench_args_t *)vref;
  int has_errors = 0;
  int i;
  int j;
  int k;
  int d;
  double diff_x;
  double diff_y;
  double diff_z;
  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      for (k = 0; k < 4; k++) {
        for (d = 0; d < 10; d++) {
          diff_x = data -> force[i][j][k][d] . x - ref -> force[i][j][k][d] . x;
          diff_y = data -> force[i][j][k][d] . y - ref -> force[i][j][k][d] . y;
          diff_z = data -> force[i][j][k][d] . z - ref -> force[i][j][k][d] . z;
          has_errors |= diff_x < -((double )1.0e-6) || ((double )1.0e-6) < diff_x;
          has_errors |= diff_y < -((double )1.0e-6) || ((double )1.0e-6) < diff_y;
          has_errors |= diff_z < -((double )1.0e-6) || ((double )1.0e-6) < diff_z;
        }
      }
    }
  }
// Return true if it's correct.
  return !has_errors;
}
