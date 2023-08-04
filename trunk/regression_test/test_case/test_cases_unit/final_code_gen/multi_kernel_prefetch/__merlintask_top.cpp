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
#include "__merlinhead_kernel_top.h"
#include "__merlintask_top.h"

void __merlin_vec_add_top(int *a,int *b,int *d,int *c1,int *c2,int inc)
{
  __merlin_init_vec_add_kernel2();
  __merlin_init_vec_add_kernel1();
  __merlin_write_buffer_vec_add_kernel1(a,b,c1,inc);
  __merlin_wait_write_vec_add_kernel1();
  __merlin_write_buffer_vec_add_kernel2(d,b,c2,inc);
  __merlin_wait_write_vec_add_kernel2();
  __merlin_execute_vec_add_kernel1(a,b,c1,inc);
  __merlin_execute_vec_add_kernel2(d,b,c2,inc);
  __merlin_wait_kernel_vec_add_kernel1();
  __merlin_wait_kernel_vec_add_kernel2();
  __merlin_read_buffer_vec_add_kernel1(a,b,c1,inc);
  __merlin_wait_read_vec_add_kernel1();
  __merlin_read_buffer_vec_add_kernel2(d,b,c2,inc);
  __merlin_wait_read_vec_add_kernel2();
  __merlin_release_vec_add_kernel1();
  __merlin_release_vec_add_kernel2();
}
