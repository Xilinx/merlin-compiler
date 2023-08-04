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
extern opencl_kernel __vec_add_kernel;
extern cl_event __event_vec_add;
#ifdef __cplusplus
extern "C" {
#endif
int __merlin_init_vec_add();
int __merlin_release_vec_add();
int __merlin_wait_kernel_vec_add();
int __merlin_wait_write_vec_add();
int __merlin_wait_read_vec_add();
#ifdef __cplusplus
}
#endif
extern opencl_mem __vec_add__a_buffer;
extern cl_event __event___vec_add__a_buffer;
extern opencl_mem __vec_add__b_buffer;
extern cl_event __event___vec_add__b_buffer;
extern opencl_mem __vec_add__c_buffer;
extern cl_event __event___vec_add__c_buffer;
extern opencl_kernel __msm_port_c_vec_add_0_kernel;
extern cl_event __event_msm_port_c_vec_add_0;
