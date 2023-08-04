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
void func_top_1(int input[2], int var) {
  int b[2][16];
  int write_idx = 0;
  int read_idx = var/8;
  int t = 16;

  while(t > 0) {
#pragma ACCEL pipeline
    b[0][write_idx % 16] = input[0];
    b[1][write_idx % 16] = input[1];
    input[0] = b[0][(read_idx + 0) % 16];
    input[1] = b[1][(read_idx + 1) % 16];
    write_idx++;
    t--;
  }
}
