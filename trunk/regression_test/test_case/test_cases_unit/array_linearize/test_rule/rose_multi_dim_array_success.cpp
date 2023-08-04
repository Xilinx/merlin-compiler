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
//
#pragma ACCEL kernel

void test_1(char *a)
{
  
#pragma ACCEL interface variable=a depth=1 max_depth=1
   *a = 'a';
}
#pragma ACCEL kernel

void test_2(char a[2])
{
  
#pragma ACCEL interface variable=a depth=1,2 max_depth=1,2
  a[0] = 'a';
}
#pragma ACCEL kernel

void test_3(char *a)
{
  
#pragma ACCEL interface variable=a depth=1 max_depth=1
  char b = 'b';
   *a = b;
}
#pragma ACCEL kernel

void test_7(char a[24])
{
  
#pragma ACCEL interface variable=a depth=1,2,3,4 max_depth=1,2,3,4
  a[0] = 'a';
}
