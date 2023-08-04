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
#include "merlin_type_define.h"
extern "C"{
#include "header.h"
#define TYPE2 int
}
typedef struct xxx {
int yyy;}zzz;
#define DATA 100
static void __merlin_dummy_1();
static void __merlin_dummy_3();
extern "C"{
#undef TYPE2
}
void vec_add_kernel(int *a,int *b,int *c,int inc);
extern int __merlin_include__LB___merlinhead_kernel_top_h_0;
#include "__merlinhead_kernel_top.h"
extern int __merlin_include__LE___merlinhead_kernel_top_h_0;

void __merlinwrapper_vec_add_kernel(int *a,int *b,int *c,int inc)
{
  
#pragma ACCEL kernel
  vec_add_kernel(a,b,c,inc);
}

void __merlin_vec_add_kernel(int *a,int *b,int *c,int inc)
{
  
#pragma ACCEL string __merlin_check_opencl();
  
#pragma ACCEL string __merlin_init_vec_add_kernel();
  __merlinwrapper_vec_add_kernel(a,b,c,inc);
  
#pragma ACCEL string __merlin_release_vec_add_kernel();
}

void __merlin_write_buffer_vec_add_kernel(int *a,int *b,int *c,int inc)
{
}

void __merlin_read_buffer_vec_add_kernel(int *a,int *b,int *c,int inc)
{
}

void __merlin_execute_vec_add_kernel(int *a,int *b,int *c,int inc)
{
}
