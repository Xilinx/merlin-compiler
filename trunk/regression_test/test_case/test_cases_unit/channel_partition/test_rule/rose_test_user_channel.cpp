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
// Original: #pragma ACCEL array_partition type=channel variable=ch_c_buf_0_0_p dim=2 factor=10
// Original: #pragma ACCEL array_partition type=channel variable=ch_c_buf_0_0_p dim=1 factor=5
#include "merlin_stream.h" 
#pragma ACCEL STREAM_CHANNEL CHANNEL_DEPTH=200 NODE=__msm_node_0 PORT=__msm_port_c_buf_0___msm_node_0_0
::merlin_stream ch_c_buf_0_0_p[5][10];
#include <string.h> 
#pragma ACCEL kernel

void top1(int *a,int *b,int *c)
{
  for (int jj = 0; jj < 5; ++jj) {
    
#pragma ACCEL parallel
    for (int _kk = 0; _kk < 10; ++_kk) {
      
#pragma ACCEL parallel
      merlin_stream_init(&ch_c_buf_0_0_p[jj][_kk],10000,4);
    }
  }
}
