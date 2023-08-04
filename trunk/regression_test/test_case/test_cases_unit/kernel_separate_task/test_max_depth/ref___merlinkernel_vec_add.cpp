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
#include "merlin_type_define.h"
extern int __merlin_include__GB_merlin_type_define_h_2;
void vec_add_kernel1(int *a,int *b,int *c,int inc);
void vec_add_kernel2(int *a,int *b,int *c,int inc);
#pragma ACCEL kernel

void vec_add(int *a,int *b,int *c,int inc)
{
  
#pragma ACCEL interface variable=a depth=100 max_depth=1000
  vec_add_kernel1(a,b,c,inc);
  vec_add_kernel2(a,b,c,inc);
}
