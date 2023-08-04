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
#include "cmost.h"

struct st 
{
  int data;
  char d;
}
;
static void __merlin_dummy_a();
const char a_d[10];
const int a_data[10];
#pragma ACCEL kernel

void top(int b_data[10],char b_d[10])
{
  
#pragma ACCEL interface variable=b_d depth=10 max_depth=10
  
#pragma ACCEL interface variable=b_data depth=10 max_depth=10
  __merlin_access_range(b_d,0,9UL);
  __merlin_access_range(b_data,0,9UL);
  char *ptr_d;
  int *ptr_data;
  ptr_data = ((int *)(&a_data[1]));
  ptr_d = ((char *)(&a_d[1]));
   *((int *)(&a_data[1])) = 10;
  b_data[1] = a_data[1];
  b_d[1] = a_d[1];
}
