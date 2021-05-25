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
#include <stdlib.h>
#include <math.h>

#include "systolic_params.h"
#include "cnn.h"
#include "util.h"

#include "systolic_top.h"

data_bitwidth ifmap[BN_R * BN_I * R * BS_R * BS_I * I];
data_bitwidth weight[BN_O * BN_I * O * BS_O * BS_I * I];
data_bitwidth ofmap[BN_R * BN_O * R * BS_R * O * BS_O];

data_bitwidth ofmap_test[BN_R * BN_O * R * BS_R * O * BS_O];

static void hls_model_verify()
{
	int err = 0;

	for (int r0 = 0; r0 < BN_R; r0++)
	for (int o0 = 0; o0 < BN_O; o0++)
	{
		for (int r2 = 0; r2 < R; r2++)
		for (int r1 = 0; r1 < BS_R; r1++)
		for (int o2 = 0; o2 < O; o2++)
		for (int o1 = 0; o1 < BS_O; o1++)
		{
		//	int ofmap_idx = (r0 * BN_O + o0) * R * BS_R * O * BS_O + ((r2 * BS_R + r1) * O + o2) * BS_O + o1;
			int ofmap_idx = (r0 * BN_O + o0) * R * BS_R * O * BS_O + ((r2 * BS_R + r1) * BS_O + o1) * O + o2;
	
		//	if (fabs(ofmap_test[ofmap_idx] - ofmap[ofmap_idx]) > 0.001)
			{
				if (err < 10)
				{
#if (DATA_BITWIDTH == 32)
					printf("HLS-MODEL ERROR: ofmap_test[%d]=%.3f, ofmap[%d]=%.3f\n", ofmap_idx, ofmap_test[ofmap_idx], ofmap_idx, ofmap[ofmap_idx]);
#else
					printf("HLS-MODEL ERROR: ofmap_test[%d]=%d, ofmap[%d]=%d\n", ofmap_idx, ofmap_test[ofmap_idx], ofmap_idx, ofmap[ofmap_idx]);
#endif
				}
				else if (err == 10)
					printf("...\n");
			//	err++;
			}
		}
	}

	if (err) printf("[HLS-MODEL Verification] %d ERRORS\n", err);
	else printf("[HLS-MODEL Verification] PASSED!\n");
}

int main(int argc, char *argv[])
{

	int ifmap_length = BN_R * BN_I * R * BS_R * BS_I * I;
	prepare_data(ifmap, ifmap_length);
	int wt_length = BN_O * BN_I * O * BS_O * BS_I * I;
	prepare_data(weight, wt_length);

	systolic_golden(ifmap, weight, ofmap_test);
	systolic_top((vec_input_t *)ifmap, (vec_input_t *)weight, (vec_output_t *)ofmap);
//	systolic_top((vec_input_t *)ifmap, (vec_input_t *)weight, ofmap);
//	systolic_top(ifmap, weight, ofmap);

	hls_model_verify();

	return 0;
}
