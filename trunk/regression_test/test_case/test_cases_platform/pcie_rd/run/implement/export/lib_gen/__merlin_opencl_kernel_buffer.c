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



#include "__merlin_opencl_kernel_buffer.h"

//


//opencl_mem ____test_kernel__a_buffer_buffer;
opencl_mem __test_kernel__a_buffer;


cl_event __event_unmaned_task1;
opencl_kernel __test_kernel_kernel;



void opencl_init_kernel_buffer()
{

    opencl_create_kernel(&__test_kernel_kernel, (char *)"test_kernel");
//    opencl_create_buffer(&____test_kernel__a_buffer_buffer, 1L*(40000), 2);
    opencl_create_buffer(&__test_kernel__a_buffer, 1L*(5000000000), 2);



}

