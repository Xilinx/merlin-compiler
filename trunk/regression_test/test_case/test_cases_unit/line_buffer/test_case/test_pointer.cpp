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
void func_top_2(int *c) {
#pragma ACCEL INTERFACE VARIABLE=c DEPTH=1024
  int sum;
    for (int j = 1; j < 31; j++) {
      for (int k = 1; k < 31; k++) {
#pragma ACCEL line_buffer variable=c
        sum += *(c+(j+1)*32+k+1) + *(c+(j-1)*32+k);
    }
  }
}
