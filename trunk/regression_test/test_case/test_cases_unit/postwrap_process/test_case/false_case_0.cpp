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
struct test_struct {};
typedef struct {int *a;struct test_struct x;double *b;}struct_with_empty;
static void __merlin_dummy_439();
extern int __merlin_include__LB___merlinhead_kernel_top_h_0;
extern struct_with_empty __merlin_f_return_struct_with_empty();
extern int __merlin_include__LE___merlinhead_kernel_top_h_0;

static void __merlin_dummy_extern_int_merlin_include_G_();
static void __merlin_dummy_kernel_pragma();
struct_with_empty f_return_struct_with_empty()
{
  return {((int *)0), {}, ((double *)0)};
}
struct_with_empty __merlinwrapper_f_return_struct_with_empty()
{
  
#pragma ACCEL kernel name="f_return_struct_with_empty"
  struct_with_empty __merlinwrapper_return = f_return_struct_with_empty();
  return __merlinwrapper_return;
}

struct_with_empty __merlin_f_return_struct_with_empty()
{
  
#pragma ACCEL string __merlin_check_opencl();
  
#pragma ACCEL string __merlin_init_f_return_struct_with_empty();
  __merlinwrapper_f_return_struct_with_empty();
  
#pragma ACCEL string __merlin_release_f_return_struct_with_empty();
}

struct_with_empty __merlin_write_buffer_f_return_struct_with_empty()
{
}

struct_with_empty __merlin_read_buffer_f_return_struct_with_empty()
{
}

struct_with_empty __merlin_execute_f_return_struct_with_empty()
{
}
