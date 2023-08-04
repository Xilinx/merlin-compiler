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
char A[8] = "abcdefgh";
int test();
extern int __merlin_include__LB___merlinhead_kernel_top_h_0;
#include "__merlinhead_kernel_top.h"
extern int __merlin_include__LE___merlinhead_kernel_top_h_0;

int __merlinwrapper_test()
{
  
#pragma ACCEL kernel name="test"
  int __merlinwrapper_return = test();
  return __merlinwrapper_return;
}

int __merlin_test()
{
  
#pragma ACCEL string __merlin_check_opencl();
  
#pragma ACCEL string __merlin_init_test();
  int __merlin_return = __merlinwrapper_test();
  
#pragma ACCEL string __merlin_release_test();
  return __merlin_return;
}

int __merlin_write_buffer_test()
{
}

int __merlin_read_buffer_test()
{
}

int __merlin_execute_test()
{
}
