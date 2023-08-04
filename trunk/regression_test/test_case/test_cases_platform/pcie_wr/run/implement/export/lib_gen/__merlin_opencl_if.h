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
/************************************************************************************
 *  (c) Copyright 2014-2015 Xilinx, Inc. All rights reserved.
 *
 ************************************************************************************/


#ifndef __OPENCL_IF_H_INCLUDED__
#define __OPENCL_IF_H_INCLUDED__

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "__merlin_parameter.h"

#include <CL/opencl.h>

int opencl_init_test_kernel(const char * bitstream);
//int opencl_create_kernel (char * kernel_name);
int opencl_create_kernel (cl_kernel * kernel, char * kernel_name);
int opencl_create_buffer (cl_mem * cl_buffer, size_t size, int type);
int opencl_write_buffer  (cl_mem cl_buffer, cl_command_queue commands, size_t  offset, void * host_buffer, size_t  size);
int opencl_read_buffer   (cl_mem cl_buffer, cl_command_queue commands, size_t  offset, void * host_buffer, size_t  size);
int opencl_set_kernel_arg(cl_kernel kernel, int index, size_t size, void * content);
//int opencl_enqueue_kernel(cl_kernel kernel, int dim, size_t global[]);
int opencl_enqueue_kernel(cl_kernel kernel, cl_command_queue commands, int dim, size_t global_in[], cl_event *event_out);
int opencl_enqueue_task(cl_kernel kernel);
int opencl_flush();

extern cl_command_queue commands[KERNEL_NUM];

typedef cl_mem    opencl_mem;
typedef cl_kernel opencl_kernel;


#endif //__OPENCL_IF_H_INCLUDED__

