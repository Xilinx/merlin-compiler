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
static void __merlin_dummy_0();
static void __merlin_dummy_kernel_pragma();
/* Original: #pragma ACCEL kernel name="vec_add" */

void test_kernel(int burst_length,int burst_number,int *a,int out)
{
  
#pragma ACCEL interface variable=a max_depth=100000000 depth=100000000
  int i;
  int j;
  int temp_arr[1];
  int temp;
  for (i = 0; i < burst_number; i++) {
    for (j = 0; j < burst_length; j++) {
      temp_arr[0] = burst_number;
      temp_arr[0] = a[j];
    }
  }
  out = temp_arr[0];
}
