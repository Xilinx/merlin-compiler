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
#include<ap_int.h>
struct st ;

struct st 
{
  class ap_int< 10 > a;
  int b;
}
;
#pragma ACCEL kernel

void top(class ap_int< 10 > *a_a,int *a_b,int i)
{
  
#pragma ACCEL INTERFACE DEPTH=10 VARIABLE=a_b max_depth=10
  
#pragma ACCEL INTERFACE DEPTH=10 VARIABLE=a_a max_depth=10
  int arr_b[3] = {(20), (20), (20)};
  class ap_int< 10 > arr_a[3] = {(10), (10), (10)};
  int arr2_b[10];
  class ap_int< 10 > arr2_a[10];
  a_a[i] = arr_a[i]+arr2_a[i];
  a_b[i] = arr_b[i] + arr2_b[i];
}
