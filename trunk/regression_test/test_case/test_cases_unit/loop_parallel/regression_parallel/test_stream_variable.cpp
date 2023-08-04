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
#include <hls_stream.h>
#include<ap_int.h>

void sub_func(class hls::stream< ap_uint< 32 >  > *c1) {
}
#pragma ACCEL kernel
void func_top_1() {

  class hls::stream< ap_uint< 32 >  > strm_arry_c1[8]; 
  #pragma HLS stream variable = strm_arry_c1 depth = 8 
  #pragma HLS resource variable = strm_arry_c1 core = FIFO_SRL 
  class hls::stream< ap_uint< 32 >  > c0[8]; 
  #pragma HLS stream variable = c0 depth = 8 
  #pragma HLS RESOURCE variable=c0 core=RAM_T2P_URAM
  int c1[8]; 
  #pragma HLS RESOURCE variable=c1 core=RAM_T2P_URAM


  for (int i = 0; i < 8; i++) {
#pragma ACCEL parallel
    sub_func(&strm_arry_c1[i]);
    c0[i].write(c1[i]);
  }
}
