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

#include "cmost.h"
#include "cmost.h"
#include <string.h>
#include <string.h> 
#include <stdio.h> 
#include <stdlib.h> 
void test_kernel(int burst_length,int burst_number,int *a);

void __merlinwrapper_test_kernel(int burst_length,int burst_number,int *a)
{
//   commands;
  
#pragma ACCEL wrapper variable=burst_length port=burst_length
  
#pragma ACCEL wrapper variable=burst_number port=burst_number
  
#pragma ACCEL wrapper variable=a port=a depth=10000 max_depth=10000 data_type=int io=RO copy=false
//  static  __m_a;
{
    if (a != 0) {
//       __test_kernel__a_buffer;
//      opencl_write_buffer(__test_kernel__a_buffer,commands[0],0 * sizeof(int ),((int *)a),((unsigned long )10000) * sizeof(int ));
        int i;
        for(i=0; i<burst_number; i++) {
            opencl_write_buffer(__test_kernel__a_buffer,commands[0],0 * sizeof(int ),((int *)a), burst_length * sizeof(int ));
        }

    }
     else {
      printf("Error! The external memory pointed by 'a' is invalid.\n");
      exit(1);
    }
  }
  
#pragma tldm graph_block name="unmaned_graph0"
{
    
#pragma tldm task_block name="unmaned_task1"
{




      
      
    opencl_set_kernel_arg(__test_kernel_kernel, 0, sizeof(int), &burst_length);
    opencl_set_kernel_arg(__test_kernel_kernel, 1, sizeof(int), &burst_number);
    opencl_set_kernel_arg(__test_kernel_kernel, 2, sizeof(opencl_mem), &__test_kernel__a_buffer);


    size_t __gid_unmaned_task1[1];
    __gid_unmaned_task1[0] = 1;

    opencl_enqueue_kernel(__test_kernel_kernel,commands[0], 1, __gid_unmaned_task1, &__event_unmaned_task1);
    clWaitForEvents(1, &__event_unmaned_task1);    

      
// #pragma ACCEL task
    }
  }
}
// #pragma ACCEL task
