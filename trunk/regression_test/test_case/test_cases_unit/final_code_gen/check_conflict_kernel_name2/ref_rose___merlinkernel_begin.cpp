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
#include "memcpy_512_1d.h"

void begin_merlinkernel0()
{
}
static int beg___in_dummy_pos;
extern "C" { 

__kernel void beg_in(::merlin_uint_512 *a,::merlin_uint_512 *b,::merlin_uint_512 *c,int inc)
{
  
#pragma HLS INTERFACE m_axi port=a offset=slave  bundle=merlin_gmem_beg___in
  
#pragma HLS INTERFACE m_axi port=b offset=slave  bundle=merlin_gmem_beg___in
  
#pragma HLS INTERFACE m_axi port=c offset=slave  bundle=merlin_gmem_beg___in
  
#pragma HLS INTERFACE s_axilite port=a bundle=control
  
#pragma HLS INTERFACE s_axilite port=b bundle=control
  
#pragma HLS INTERFACE s_axilite port=c bundle=control
  
#pragma HLS INTERFACE s_axilite port=inc bundle=control
  
#pragma HLS INTERFACE s_axilite port=return bundle=control
  begin_merlinkernel0();
}
}
