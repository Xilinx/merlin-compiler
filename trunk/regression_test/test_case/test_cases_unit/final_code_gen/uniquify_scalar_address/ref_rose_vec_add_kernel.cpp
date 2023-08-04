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
//test whether function 'f' has only two versions

void f_1(int *a)
{
   *a = 1;
}

void f(__global int *a)
{
   *a = 1;
}
#pragma ACCEL kernel

__kernel void top(__global int * restrict a)
{
  f(&a[0]);
  f(&a[1]);
  int local_r;
  f_1(&local_r);
  int local2;
  f_1(&local2);
  int a_buf[2];
  f_1(&a_buf[0]);
  f_1(&a_buf[1]);
}

void f1(int *a)
{
   *a = 1;
}

void f2(int *a)
{
  f1(a);
  f1(a);
}
#pragma ACCEL kernel

__kernel void top2(__global int * restrict a)
{
  int local_r;
  f2(&local_r);
  int local2;
  f2(&local2);
  int a_buf[2];
  f2(&a_buf[0]);
}
