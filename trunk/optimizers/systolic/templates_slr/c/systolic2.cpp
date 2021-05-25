// (C) Copyright 2016-2021 Xilinx, Inc.
// All Rights Reserved.
//
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.



//#include "systolic.h"

typedef float data_bitwidth;

extern "C" {
void systolic_top2(data_bitwidth *ifmap, data_bitwidth *weight, data_bitwidth *ofmap)
{
#pragma HLS INTERFACE m_axi port=ifmap offset=slave bundle=ifmap
#pragma HLS INTERFACE m_axi port=weight offset=slave bundle=weight
#pragma HLS INTERFACE m_axi port=ofmap offset=slave bundle=ofmap
#pragma HLS INTERFACE s_axilite port=ifmap bundle=control
#pragma HLS INTERFACE s_axilite port=weight bundle=control
#pragma HLS INTERFACE s_axilite port=ofmap bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control
//	for (int o2 = 0; o2 < O; o2++)
//	for (int o1 = 0; o1 < BS_O; o1++)
//	for (int r1 = 0; r1 < BS_R; r1++)
//	{
//		int ofmap_idx = (o2 * BS_O + o1) * BS_R + r1;
//
//		ofmap[ofmap_idx] = 0;
//		for (int i1 = 0; i1 < BS_I; i1++)
//		{
//			int ifmap_idx = r1 * BS_I + i1;
//			int wt_idx = (o2 * BS_O + o1) * BS_I + i1;
//
//			ofmap[ofmap_idx] += weight[wt_idx] * ifmap[ifmap_idx];
//		}
//	}
	ofmap[0] += weight[0] * ifmap[0];
}
}
