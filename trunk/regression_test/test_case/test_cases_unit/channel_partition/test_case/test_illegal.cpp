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
#include "merlin_stream.h" 
#pragma ACCEL STREAM_CHANNEL CHANNEL_DEPTH=200 NODE=__msm_node_0 PORT=__msm_port_a_buf_0___msm_node_0_0 
merlin_stream ch_a_buf_0_2;
#pragma ACCEL STREAM_CHANNEL CHANNEL_DEPTH=200 NODE=__msm_node_0 PORT=__msm_port_c_buf_0___msm_node_0_0 
#pragma HLS array_partition type=channel variable=ch_b_buf_0_2 dim=1 factor=5
merlin_stream ch_b_buf_0_2;
#include <string.h> 
#pragma ACCEL kernel
void top(int *a,int *b,int *c, int N)
{
    for (int jj = 0; jj < N; ++jj) {
#pragma ACCEL parallel
        merlin_stream_init(&ch_a_buf_0_2,10000,4);
    }
}
#pragma ACCEL kernel
void top1(int *a,int *b,int *c)
{
    for (int jj = 0; jj < 10; ++jj) {
#pragma ACCEL parallel
        merlin_stream_init(&ch_b_buf_0_2,10000,4);
    }
}

