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



#include "systolic_params.h"

void systolic_golden(data_bitwidth *ifmap, data_bitwidth *weight, data_bitwidth *ofmap)
{
	for (int r0 = 0; r0 < BN_R; r0++)
	for (int o0 = 0; o0 < BN_O; o0++)
	for (int r1 = 0; r1 < BS_R; r1++)
	for (int c1 = 0; c1 < BS_C; c1++)
	for (int o1 = 0; o1 < BS_O; o1++)
	for (int r2 = 0; r2 < R; r2++)
	for (int c2 = 0; c2 < C; c2++)
	for (int o2 = 0; o2 < O; o2++)
	{
		// (r0,o0,r1,r2,c1,c2,o1,o2)
		int ofmap_idx = (r0 * BN_O + o0) * BS_R * R * BS_C * C * BS_O * O + ((r1 * R + r2) * BS_C * C + c1 * C + c2) * BS_O * O + o1 * O + o2;

		ofmap[ofmap_idx] = 0;
	}

	for (int r0 = 0; r0 < BN_R; r0++)
	for (int o0 = 0; o0 < BN_O; o0++)
	{
		for (int r1 = 0; r1 < BS_R; r1++)
		for (int c1 = 0; c1 < BS_C; c1++)
		for (int o1 = 0; o1 < BS_O; o1++)
		for (int i1 = 0; i1 < BS_I; i1++)
		{
			for (int r2 = 0; r2 < R; r2++)
			for (int c2 = 0; c2 < C; c2++)
			for (int o2 = 0; o2 < O; o2++)
			for (int p = 0; p < K1; p++)
			for (int q = 0; q < K2; q++)
			for (int i2 = 0; i2 < I; i2++)
			{
				// (r0,r1*R+r2+p,c1*C+c2+q,i1,i2)
				int ifmap_idx = r0 * ((BS_R * R + K1 - 1) * (BS_C * C + K2 - 1) * BS_I * I) + ((r1 * R + r2 + p) * (BS_C * C + K2 - 1) + (c1 * C + c2 + q)) * BS_I * I + i1 * I + i2;
				// (o0,o2,o1,p,q,i1,i2)
				int wt_idx = o0 * O * BS_O * K1 * K2 * BS_I * I + (((o1 * O + o2) * K1 + p) * K2 + q) * BS_I * I + i1 * I + i2;
	
				// (r0,o0,r1,r2,c1,c2,o1,o2)
				int ofmap_idx = (r0 * BN_O + o0) * BS_R * R * BS_C * C * BS_O * O + ((r1 * R + r2) * BS_C * C + c1 * C + c2) * BS_O * O + o1 * O + o2;
			
				ofmap[ofmap_idx] += ifmap[ifmap_idx] * weight[wt_idx];
			}
		}
	}
}
