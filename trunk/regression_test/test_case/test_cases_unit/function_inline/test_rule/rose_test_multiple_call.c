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
static void __merlin_dummy_sub();
static void __merlin_dummy_test();
static void __merlin_dummy_test2();
static void __merlin_dummy_test3();
#pragma ACCEL kernel

void top(int *a)
{
{
    a[0] = 0;
  }
{
    (a + 10)[0] = 0;
  }
  int return_5_test;
{
    int a_test_0 = a[0];
    return_5_test = a_test_0 > 0;
  }
  if (return_5_test) {
  }
   else {
    int return_4_test;
{
      int a_test = a[1];
      return_4_test = a_test > 0;
    }
    if (return_4_test) {
    }
  }
  for (int i = 0; 1; ) {
    int return_3_test2;
{
      int b_test2_0 = a[1];
      return_3_test2 = b_test2_0 > 10;
    }
    if (!(i < return_3_test2)) {
      break; 
    }
    int return_2_test2;
{
      int b_test2 = a[3];
      return_2_test2 = b_test2 > 10;
    }
    i += return_2_test2;
  }
  while(1){
    int return_0_test3;
{
      int c_test3 = a[4];
      return_0_test3 = c_test3 < 10;
    }
    int return_1_test3;
{
      int c_test3_0 = a[2];
      return_1_test3 = c_test3_0 < 10;
    }
    if (!(return_1_test3 && return_0_test3)) {
      break; 
    }
  }
}
