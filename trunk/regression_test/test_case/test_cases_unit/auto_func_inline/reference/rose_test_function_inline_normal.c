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
//#define FOCUS 0
#if !FOCUS

void sub1_1(int *sub1_1_a)
{
}
#pragma ACCEL kernel

void test1_1()
{
  int test1_1_a[16];
  
#pragma HLS array_partition variable=test1_1_a dim=1
  sub1_1(test1_1_a);
}

void sub1_2(int *sub1_2_a)
{
}
#pragma ACCEL kernel

void test1_2()
{
  int test1_2_a[16];
  
#pragma HLS array_partition variable=test1_2_a dim=1
  sub1_2((int *)test1_2_a);
}
#else
#endif 
