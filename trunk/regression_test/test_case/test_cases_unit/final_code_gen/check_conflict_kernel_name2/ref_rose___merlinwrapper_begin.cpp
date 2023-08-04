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
#include "memcpy_512.h"
#include <string.h> 
#include <stdio.h> 
#include <stdlib.h> 
void beg___in(::merlin_uint_512 *a,::merlin_uint_512 *b,::merlin_uint_512 *c,int inc);
#include "__merlinhead_kernel_top.h"

void __merlinwrapper_beg___in(int a[4096],int b[4096],int c[4096],int inc)
{
  static int __m_a[4096];
  static int __m_b[4096];
  static int __m_c[4096];
  
#pragma ACCEL kernel
{
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "beg___in");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__beg___in_kernel, 0, sizeof(opencl_mem), &__beg___in__a_buffer);
	opencl_set_kernel_arg(__beg___in_kernel, 1, sizeof(opencl_mem), &__beg___in__b_buffer);
	opencl_set_kernel_arg(__beg___in_kernel, 2, sizeof(opencl_mem), &__beg___in__c_buffer);
	opencl_set_kernel_arg(__beg___in_kernel, 3, sizeof(int), &inc);
	#pragma ACCEL debug printf("[Merlin Info] Finish set kernel %s argument....\n", "beg___in");
	#pragma ACCEL debug fflush(stdout);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start execute kernel %s....\n", "beg___in");
	#pragma ACCEL debug fflush(stdout);
	size_t __gid[1];
	__gid[0] = 1;
	opencl_enqueue_kernel(__beg___in_kernel, commands[0], 1, __gid, &__event_beg___in);
}
{
	clWaitForEvents(1, &__event_beg___in);
	#pragma ACCEL debug printf("[Merlin Info] Finish execute kernel %s....\n", "beg___in");
	#pragma ACCEL debug fflush(stdout);
}
}

void __merlin_beg___in(int a[4096],int b[4096],int c[4096],int inc)
{
  __merlinwrapper_beg___in(a,b,c,inc);
}

void __merlin_write_buffer_beg___in(int a[4096],int b[4096],int c[4096],int inc)
{
}

void __merlin_read_buffer_beg___in(int a[4096],int b[4096],int c[4096],int inc)
{
}

void __merlin_execute_beg___in(int a[4096],int b[4096],int c[4096],int inc)
{
  int i;
{
	#pragma ACCEL debug printf("[Merlin Info] Start set kernel %s argument....\n", "beg___in");
	#pragma ACCEL debug fflush(stdout);
	opencl_set_kernel_arg(__beg___in_kernel, 0, sizeof(opencl_mem), &__beg___in__a_buffer);
	opencl_set_kernel_arg(__beg___in_kernel, 1, sizeof(opencl_mem), &__beg___in__b_buffer);
	opencl_set_kernel_arg(__beg___in_kernel, 2, sizeof(opencl_mem), &__beg___in__c_buffer);
	opencl_set_kernel_arg(__beg___in_kernel, 3, sizeof(int), &inc);
	#pragma ACCEL debug printf("[Merlin Info] Finish set kernel %s argument....\n", "beg___in");
	#pragma ACCEL debug fflush(stdout);
}
{
	#pragma ACCEL debug printf("[Merlin Info] Start execute kernel %s....\n", "beg___in");
	#pragma ACCEL debug fflush(stdout);
	size_t __gid[1];
	__gid[0] = 1;
	opencl_enqueue_kernel(__beg___in_kernel, commands[0], 1, __gid, &__event_beg___in);
}
}
