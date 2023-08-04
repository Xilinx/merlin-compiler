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
struct DATA32{unsigned int data[32];};
struct DATA{DATA32 data;};
void sub_func(DATA array[10]) {
  #pragma HLS ARRAY_PARTITION variable=&array complete dim=0
  for (int i = 0; i < 10; i++) {
#pragma ACCEL unroll
    array[i];
  }
}
#pragma ACCEL kernel
void func_top_1(int *in) {
#pragma ACCEL interface variable=in depth=3300,16 max_depth=3300,16
#pragma HLS array partition variable=in dim=2 

  int a[100];
#pragma HLS resource variable=a core = ROM_nP_LUTRAM
  int b[100];
#pragma HLS array_partition variable=a,b cyclic factor=4
  int aa[100];
#pragma HLS array partition variable=aa cyclic factor=10
  int c[100];
#pragma HLS array partition variable=c cyclic factor=10
  DATA burst;
#pragma HLS array partition variable=burst->data->data cyclic factor=8
  DATA32 burst32;
#pragma HLS array partition variable=burst32->data cyclic factor=16
  int fully_a[10][10];
#pragma HLS array_partition variable=fully_a dim=1 complete 
#pragma HLS array_partition variable=fully_a dim=2 complete 
#pragma HLS RESOURCE variable=fully_a core=RAM_T2P_URAM latency=2
  int fully_b[10][10];
#pragma HLS array_partition variable=fully_b dim=0 complete 
#pragma HLS RESOURCE variable=fully_b core=RAM_T2P_URAM latency=2
  DATA burst_array[10];

  for (int i = 0; i < 10; i++) {
#pragma ACCEL parallel
    c[i] = fully_a[9][i];
    c[i] += fully_b[9][i];
  }

  for (int i = 0; i < 100; i++) {
#pragma ACCEL parallel
    c[i] ++;
    if(i<32)
      burst32.data[i] = 10;
  }
  for (int j = 0; j < 16; j++) {
    for (int i = 0; i < 100; i++) {
#pragma ACCEL PIPELINE
      a[i] = b[i]+c[i]- aa[i] + in[i*16+j];
    }
  }
  sub_func(burst_array);
}
