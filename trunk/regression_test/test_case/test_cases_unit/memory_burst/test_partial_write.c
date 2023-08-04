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
#pragma ACCEL kernel

void func_top_0(int *a,int *b)
{
  int i;
  int j;
  for (i = 0; i < 10; ++i) {
    
#pragma ACCEL parallel complete
    for (j = 0; j < 10; j++) {
      
#pragma ACCEL parallel complete priority = logic
      a[i * 10 + j] = b[j];
    }
  }
}
#pragma ACCEL kernel

void func_top_1(int *a,int *b)
{
  int i;
  int j;
  for (i = 0; i < 10; ++i) {
    
#pragma ACCEL parallel complete
    for (j = 0; j < 10; j++) {
      
#pragma ACCEL parallel complete priority = logic
      a[i * 11 + j] = b[j];
    }
  }
}
#pragma ACCEL kernel

void func_top_2(int *a,int *b)
{
  int i;
  int j;
  for (i = 0; i < 10; ++i) {
    
#pragma ACCEL parallel complete
    for (j = 0; j < 10; j++) {
      
#pragma ACCEL parallel complete priority = logic
      a[i * 9 + j] = b[j];
    }
  }
}
