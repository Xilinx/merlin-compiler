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
void test_kernel(int burst_length,int burst_number,int *a,int out);

void __merlinwrapper_test_kernel(int burst_length,int burst_number,int *a,int out)
{
  
#pragma ACCEL wrapper variable=burst_length port=burst_length
  
#pragma ACCEL wrapper variable=burst_number port=burst_number
  
#pragma ACCEL wrapper variable=a port=a depth=100000000 max_depth=100000000 data_type=int io=RO copy=false
  
#pragma ACCEL wrapper variable=out port=out
  static int __m_a[100000000];
{
    if (a != 0) {
      memcpy(((void *)(__m_a + 0)),((const void *)a),100000000 * sizeof(int ));
    }
     else {
      printf("Error! The external memory pointed by 'a' is invalid.\n");
      exit(1);
    }
  }
  
#pragma ACCEL task
  test_kernel(burst_length,burst_number,__m_a,out);
}
