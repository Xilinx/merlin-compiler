// (C) Copyright 2016-2021 Xilinx, Inc.
// All Rights Reserved.
//
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.



#ifndef __OPENCL_IF_H_INCLUDED__
#define __OPENCL_IF_H_INCLUDED__

#include <stdio.h>
#include "__merlin_parameter.h"

#include <CL/opencl.h>
#include <CL/cl_ext.h>

#define __MERLIN_VENDOR
#ifdef __MERLIN_SDACCEL
#include <CL/cl_ext_xilinx.h>
#endif

//#include <vector>

#ifdef __cplusplus
extern "C" {
#endif
int opencl_init(const char *bitstream);
int __merlin_init(const char *bitstream);
int __merlin_init_opencl(const char *bitstream);
int __merlin_release();
int __merlin_release_opencl();
int __merlin_check_opencl();
#ifdef __cplusplus
}
#endif
// int opencl_create_kernel (char * kernel_name);
int opencl_create_kernel(cl_kernel *kernel, char *kernel_name);
int opencl_create_buffer(cl_mem *cl_buffer, long long size, int type, void * migrate_data);
#ifdef __MERLIN_SDACCEL
int opencl_create_ext_buffer(cl_mem *cl_buffer, cl_mem_ext_ptr_t *cl_buffer_ext,
                             long long size, int type);
#endif
int opencl_write_buffer(cl_mem cl_buffer, cl_command_queue commands,
                        long long offset, void *host_buffer, long long size);
int opencl_read_buffer(cl_mem cl_buffer, cl_command_queue commands,
                       long long offset, void *host_buffer, long long size);
int opencl_migrate_buffer(int num_bufs, cl_mem* cl_bufs, cl_command_queue commands, int direction);

int opencl_write_buffer_nb(cl_mem cl_buffer, cl_command_queue commands,
                           long long offset, void *host_buffer, long long size,
                           cl_event *event);
int opencl_read_buffer_nb(cl_mem cl_buffer, cl_command_queue commands,
                          long long offset, void *host_buffer, long long size,
                          cl_event *event);
int opencl_set_kernel_arg(cl_kernel kernel, int index, size_t size,
                          const void *content);
// int opencl_enqueue_kernel(cl_kernel kernel, int dim, size_t global[]);
int opencl_enqueue_kernel(cl_kernel kernel, cl_command_queue commands, int dim,
                          size_t global_in[], cl_event *event_out);
int opencl_enqueue_task(cl_kernel kernel, cl_command_queue commands);
int opencl_flush(cl_command_queue commands);

int opencl_wait_event(cl_event event);
int opencl_release_event(cl_event event);
int opencl_release_mem_object(cl_mem mem);
int opencl_release_kernel(cl_kernel kernel);

extern cl_command_queue commands[KERNEL_NUM];

typedef cl_mem opencl_mem;
typedef cl_kernel opencl_kernel;
#ifdef __MERLIN_SDACCEL
typedef cl_mem_ext_ptr_t opencl_mem_ext;
#endif

#endif //__OPENCL_IF_H_INCLUDED__
