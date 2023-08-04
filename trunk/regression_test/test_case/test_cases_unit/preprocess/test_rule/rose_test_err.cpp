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
// 1. loop structure
// 2. function structure
// 3. access pattern
// 4. pragma modes
#pragma ACCEL kernel

void func_top_3(int &a,int *b)
{
  int k;
  int i;
  int j;
  for (j = 0; j < 100; ++j) {
    for (k = 0; k < 100; ++k) {
      for (i = 0; i < 100; i++) 
// Original: #pragma ACCEL pipeline
{
        
#pragma ACCEL PIPELINE 
        a += b[i + 100 * k + j];
      }
    }
  }
}
