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
int test1_merlin_clone2 = 0;
static int test1_merlin_clone6(int a,int b,int c);

static int test1_merlin_clone6(int a,int b,int c)
{
  c = a + b;
}

__kernel void vec_add_kernel2_merlin_clone1(__global int * restrict d,__global int * restrict b,__global int * restrict c,int inc)
{
  test1_merlin_clone2 = 0;
  test1_merlin_clone6(1,2,3);
  double test1_merlin_clone3 = 0;
}
