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


#include <stdio.h>
#include "systolic_params.h"

void conv_gold(data_bitwidth *ifmap_buf, data_bitwidth *filter_buf, data_bitwidth *ofmap_buf)
{
	for (int c2 = 0; c2 < C; c2++)
	for (int i2 = 0; i2 < I; i2++)
	{
		int ofmap_buf_idx = c2 * I + i2;

		ofmap_buf[ofmap_buf_idx] = 0;
	}

	for (int i2 = 0; i2 < I; i2++)
	for (int c2 = 0; c2 < C; c2++)
	for (int q = 0; q < K2; q++)
	{
		int ifmap_buf_idx = (c2 + q) * I + i2;
		int ofmap_buf_idx = c2 * I + i2;
		int filter_buf_idx = q * I + i2;

		ofmap_buf[ofmap_buf_idx] += ifmap_buf[ifmap_buf_idx] * filter_buf[filter_buf_idx];

	//	printf("ofmap_buf[%d] += ifmap_buf[%d] * filter_buf[%d], %d, %d, %d\n", ofmap_buf_idx, ifmap_buf_idx, filter_buf_idx, ofmap_buf[ofmap_buf_idx], ifmap_buf[ifmap_buf_idx], filter_buf[filter_buf_idx]);
	//	printf("ofmap_buf[%d] += ifmap_buf[%d] * filter_buf[%d], %d, %d, %d, %d\n", ofmap_buf_idx, ifmap_buf_idx, filter_buf_idx, ofmap_buf[ofmap_buf_idx], ifmap_buf[ifmap_buf_idx], filter_buf[filter_buf_idx], &(filter_buf[filter_buf_idx]));
	}

}

void winograd_4_3(data_bitwidth g[3], data_bitwidth d[6], data_bitwidth Y[4])
{
	data_bitwidth Gg[6];
	data_bitwidth BTd[6];

	/*
	 * AT=
	 * |1 1 1 1 1 0|
	 * |0 1 -1 2 -2 0|
	 * |0 1 1 4 4 0|
	 * |0 1 -1 8 -8 1|
	 * */

	/*
	 * G=
	 * |1/4 0 0|
	 * |-1/6 -1/6 -1/6|
	 * |-1/6 1/6 -1/6|
	 * |1/24 1/12 -1/6|
	 * |1/24 -1/12 1/6|
	 * |0 0 1|
	 * */

	/*
	 * BT=
	 * |4 0 -5 0 1 0|
	 * |0 -4 -4 1 1 0|
	 * |0 4 -4 -1 1 0|
	 * |0 -2 -1 2 1 0|
	 * |0 2 -1 -2 1 0|
	 * |0 4 0 -5 0 1|
	 * */

	/* Y = AT*((G*g)(BT*d)) = 
	 * |d[0]g[0] + d[1]g[1] + d[2]g[2]|
	 * |d[1]g[0] + d[2]g[1] + d[3]g[2]|
	 * |d[2]g[0] + d[3]g[1] + d[4]g[2]|
	 * |d[3]g[0] + d[4]g[1] + d[5]g[2]|
	 * */

	// Gg
	Gg[0] = g[0] / 4;
	Gg[1] = -g[0] / 6 - g[1] / 6 - g[2] / 6;
	Gg[2] = -g[0] / 6 + g[1] / 6 - g[2] / 6;
	Gg[3] = g[0] / 24 + g[1] / 12 + g[2] / 6;
	Gg[4] = g[0] / 24 - g[1] / 12 + g[2] / 6;
	Gg[5] =	g[2] / 6;

//	Gg[0] = 1;
//	Gg[1] = 1;
//	Gg[2] = 1;
//	Gg[3] = 1;
//	Gg[4] = 1;
//	Gg[5] =	1;

	// BTd
	BTd[0] = 4 * d[0] - 5 * d[2] + d[4];
	BTd[1] = -4 * d[1] - 4 * d[2] + d[3] + d[4];
	BTd[2] = 4 * d[1] - 4 * d[2] - d[3] + d[4];
	BTd[3] = -2 * d[1] - 1 * d[2] + 2 * d[3] + d[4];
	BTd[4] = 2 * d[1] - 1 * d[2] - 2 * d[3] + d[4];
	BTd[5] = 4 * d[2] - 5 * d[3] + d[5];

//	printf("%d\n", BTd[0]);
//	printf("%d\n", BTd[1]);
//	printf("%d\n", BTd[2]);
//	printf("%d\n", BTd[3]);
//	printf("%d\n", BTd[4]);
//	printf("%d\n", BTd[5]);

//	BTd[0] = 1;
//	BTd[1] = 1;
//	BTd[2] = 1;
//	BTd[3] = 1;
//	BTd[4] = 1;
//	BTd[5] = 1;

	// Y = AT*((G*g)(BT*d))
	Y[0] = Gg[0] * BTd[0] + Gg[1] * BTd[1] + Gg[2] * BTd[2] + Gg[3] * BTd[3] + Gg[4] * BTd[4];
	Y[1] = Gg[1] * BTd[1] - Gg[2] * BTd[2] + 2 * Gg[3] * BTd[3] - 2 * Gg[4] * BTd[4];
	Y[2] = Gg[1] * BTd[1] + Gg[2] * BTd[2] + 4 * Gg[3] * BTd[3] + 4 * Gg[4] * BTd[4];
	Y[3] = Gg[1] * BTd[1] - Gg[2] * BTd[2] + 8 * Gg[3] * BTd[3] - 8 * Gg[4] * BTd[4] + Gg[5] * BTd[5];

//	Y[0] = 1;
//	Y[1] = 1;
//	Y[2] = 1;
//	Y[3] = 1;
}

void conv_winograd(data_bitwidth *ifmap_buf, data_bitwidth *filter_buf, data_bitwidth *ofmap_buf)
{
	data_bitwidth g[3];
	data_bitwidth d[6];
	data_bitwidth Y[4];

	for (int i2 = 0; i2 < I; i2++)
	{
		for (int c2_q = 0; c2_q < (C + K2 - 1); c2_q++)
		{
			d[c2_q] = ifmap_buf[c2_q * I + i2];

		}
		for (int q = 0; q < K2; q++)
		{
			g[q] = filter_buf[q * I + i2];
		}

		winograd_4_3(g, d, Y);

		for (int c2 = 0; c2 < C; c2++)
		{
			ofmap_buf[c2 * I + i2] = Y[c2];
		}
	}
}

void c_model(data_bitwidth *ifmap, data_bitwidth *weight, data_bitwidth *ofmap)
{
	data_bitwidth ifmap_buf[(C + K2 - 1) * I];
	data_bitwidth filter_buf[K2 * I];
	data_bitwidth ofmap_buf[C * I];
//	data_bitwidth ofmap_buf[C];

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
		for (int p = 0; p < K1; p++)
		{
			for (int r2 = 0; r2 < R; r2++)
			for (int o2 = 0; o2 < O; o2++)
			{
			//	for (int c2 = 0; c2 < C; c2++)
			//	for (int q = 0; q < K2; q++)
			//	for (int i2 = 0; i2 < I; i2++)
			//	{
			//	}
				// write ifmap_buf
				for (int c2_q = 0; c2_q < (C + K2 - 1); c2_q++)
				for (int i2 = 0; i2 < I; i2++)
				{
					// (r0,r1*R+r2+p,c1*C+c2_q,i1,i2)
					int ifmap_idx = r0 * ((BS_R * R + K1 - 1) * (BS_C * C + K2 - 1) * BS_I * I) + ((r1 * R + r2 + p) * (BS_C * C + K2 - 1) + (c1 * C + c2_q)) * BS_I * I + i1 * I + i2;
					int ifmap_buf_idx = c2_q * I + i2;

					ifmap_buf[ifmap_buf_idx] = ifmap[ifmap_idx];
				//	printf("ifmap[%d]=%d\n", ifmap_idx, ifmap[ifmap_idx]);
				}
				for (int q = 0; q < K2; q++)
				for (int i2 = 0; i2 < I; i2++)
				{
					// (o0,o2,o1,p,q,i1,i2)
					int wt_idx = o0 * O * BS_O * K1 * K2 * BS_I * I + (((o1 * O + o2) * K1 + p) * K2 + q) * BS_I * I + i1 * I + i2;
					int filter_buf_idx = q * I + i2;

					filter_buf[filter_buf_idx] = weight[wt_idx];
				//	printf("weight[%d]=%d, filter_buf[%d]=%d, %d\n", wt_idx, weight[wt_idx], filter_buf_idx, filter_buf[filter_buf_idx], &(filter_buf[filter_buf_idx]));
				}

#if (WINOGRAD == 1)
				conv_winograd(ifmap_buf, filter_buf, ofmap_buf);
#else
				conv_gold(ifmap_buf, filter_buf, ofmap_buf);
#endif

				for (int c2 = 0; c2 < C; c2++)
				for (int i2 = 0; i2 < I; i2++)
				{
					// (r0,o0,r1,r2,c1,c2,o1,o2)
					int ofmap_idx = (r0 * BN_O + o0) * BS_R * R * BS_C * C * BS_O * O + ((r1 * R + r2) * BS_C * C + c1 * C + c2) * BS_O * O + o1 * O + o2;
					int ofmap_buf_idx = c2 * I + i2;

					ofmap[ofmap_idx] += ofmap_buf[ofmap_buf_idx];
				}
			}
		}
	}
}
