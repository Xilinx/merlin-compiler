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
#define B_MAX A_MAX*(6/3+1-1)
#define C_MAX B_MAX*2
#define HALF 1024
#pragma ACCEL kernel
void gemm_cpu(int *A, int *B) {
#pragma ACCEL interface variable = A depth = 2048 max_depth = A_MAX
// Need better tokenizer if I want to write C_MAX * 2 * (3 + 12) or data structure
// This is too fine-grain and no impact. 
#pragma ACCEL interface variable = B depth = 2048 max_depth = C_MAX*2*(4+12)
#pragma ACCEL tiling factor=F parallel_factor=PF 
  for (int i = 0; i < 2048; i++) {
#pragma HLS LOOP_TRIPCOUNT min=HALF*2 max=HALF+HALF
    A[i] = B[i] + 10;
  }
}
