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
extern opencl_kernel __CompressFragment_kernel_kernel;
extern cl_event __event_CompressFragment_kernel;
#ifdef __cplusplus
extern "C" {
#endif
int __merlin_init_CompressFragment_kernel();
int __merlin_release_CompressFragment_kernel();
int __merlin_wait_kernel_CompressFragment_kernel();
int __merlin_wait_write_CompressFragment_kernel();
int __merlin_wait_read_CompressFragment_kernel();
#ifdef __cplusplus
}
#endif
extern opencl_mem __CompressFragment_kernel__merlin_input_buffer;
extern cl_event __event___CompressFragment_kernel__merlin_input_buffer;
extern opencl_mem __CompressFragment_kernel__merlin_return_value_buffer;
extern cl_event __event___CompressFragment_kernel__merlin_return_value_buffer;
extern opencl_mem __CompressFragment_kernel__op_buffer;
extern cl_event __event___CompressFragment_kernel__op_buffer;
extern opencl_mem __CompressFragment_kernel__table_buffer;
extern cl_event __event___CompressFragment_kernel__table_buffer;
