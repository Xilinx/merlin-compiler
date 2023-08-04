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
/****************************************************************************/
// vec_add_kernel.cpp
// Copyright (C) 2017 Xilinx, Inc. - All rights reserved.
/****************************************************************************/
#include "vec_add.h"
#include "stdio.h"
static void __merlin_dummy_kernel_pragma();

void vec_add_kernel(int a[10000],int b[10000],int c[10000],int inc)
{
  
#pragma ACCEL interface variable=a depth=10000 max_depth=10000
  
#pragma ACCEL interface variable=b depth=10000 max_depth=10000
  
#pragma ACCEl interface variable=c depth=10000 max_depth=10000
  int j;
  
#pragma ACCEL parallel factor=16
  for (j = 0; j < 10000; j++) {
    c[j] = a[j] + b[j] + inc;
  }
}
