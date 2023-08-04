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
#include <string.h> 
#include <stdio.h> 
#include <stdlib.h> 
void ellpack_kernel(int m);
#include "__merlinhead_kernel_top.h"

void __merlinwrapper_ellpack_kernel(int m)
{
  
#pragma ACCEL kernel
{
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "ellpack_kernel");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__ellpack_kernel_kernel, 0, sizeof(int), &m);
	#pragma ACCEL debug printf("[Merlin Info] Finish set kernel %s argument....\n", "ellpack_kernel");
	#pragma ACCEL debug fflush(stdout);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start execute kernel %s....\n", "ellpack_kernel");
	#pragma ACCEL debug fflush(stdout);
	size_t __gid[1];
	__gid[0] = 1;
	opencl_enqueue_kernel(__ellpack_kernel_kernel, commands[0], 1, __gid, &__event_ellpack_kernel);
}
{
	clWaitForEvents(1, &__event_ellpack_kernel);
	#pragma ACCEL debug printf("[Merlin Info] Finish execute kernel %s....\n", "ellpack_kernel");
	#pragma ACCEL debug fflush(stdout);
}
}

void __merlin_ellpack_kernel(int m)
{
  __merlinwrapper_ellpack_kernel(m);
}

void __merlin_write_buffer_ellpack_kernel(int m)
{
}

void __merlin_read_buffer_ellpack_kernel()
{
}

void __merlin_execute_ellpack_kernel(double nzval[2097152],int cols[2097152],double vec[4096],double out[4096])
{
  int i;
{
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "ellpack_kernel");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__ellpack_kernel_kernel, 0, sizeof(int), &m);
	#pragma ACCEL debug printf("[Merlin Info] Finish set kernel %s argument....\n", "ellpack_kernel");
	#pragma ACCEL debug fflush(stdout);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start execute kernel %s....\n", "ellpack_kernel");
	#pragma ACCEL debug fflush(stdout);
	size_t __gid[1];
	__gid[0] = 1;
	opencl_enqueue_kernel(__ellpack_kernel_kernel, commands[0], 1, __gid, &__event_ellpack_kernel);
}
}
