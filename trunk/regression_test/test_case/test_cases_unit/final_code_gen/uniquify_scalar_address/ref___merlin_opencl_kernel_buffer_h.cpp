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
#include "__merlin_opencl_if.h"
int opencl_init_kernel_buffer();
int __merlin_release_kernel_buffer();
extern opencl_kernel __top_kernel;
extern cl_event __event_top;
#ifdef __cplusplus
extern "C" {
#endif
int __merlin_init_top();
int __merlin_release_top();
int __merlin_wait_kernel_top();
int __merlin_wait_write_top();
int __merlin_wait_read_top();
#ifdef __cplusplus
}
#endif
extern opencl_mem __top__a_buffer;
extern cl_event __event___top__a_buffer;
extern opencl_kernel __top2_kernel;
extern cl_event __event_top2;
#ifdef __cplusplus
extern "C" {
#endif
int __merlin_init_top2();
int __merlin_release_top2();
int __merlin_wait_kernel_top2();
int __merlin_wait_write_top2();
int __merlin_wait_read_top2();
#ifdef __cplusplus
}
#endif
extern opencl_mem __top2__a_buffer;
extern cl_event __event___top2__a_buffer;
