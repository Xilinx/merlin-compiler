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
extern opencl_kernel __md_kernel_kernel;
extern cl_event __event_md_kernel;
#ifdef __cplusplus
extern "C" {
#endif
int __merlin_init_md_kernel();
int __merlin_release_md_kernel();
int __merlin_wait_kernel_md_kernel();
int __merlin_wait_write_md_kernel();
int __merlin_wait_read_md_kernel();
#ifdef __cplusplus
}
#endif
extern opencl_mem __md_kernel__force_x_buffer;
extern cl_event __event___md_kernel__force_x_buffer;
extern opencl_mem __md_kernel__force_y_buffer;
extern cl_event __event___md_kernel__force_y_buffer;
extern opencl_mem __md_kernel__force_z_buffer;
extern cl_event __event___md_kernel__force_z_buffer;
extern opencl_mem __md_kernel__n_points_buffer;
extern cl_event __event___md_kernel__n_points_buffer;
extern opencl_mem __md_kernel__position_x_buffer;
extern cl_event __event___md_kernel__position_x_buffer;
extern opencl_mem __md_kernel__position_y_buffer;
extern cl_event __event___md_kernel__position_y_buffer;
extern opencl_mem __md_kernel__position_z_buffer;
extern cl_event __event___md_kernel__position_z_buffer;
