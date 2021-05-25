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


#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <CL/opencl.h>

#include <iostream>

#include "systolic_params.h"
#include "opencl_if.h"
#include "cnn.h"
#include "c_model.h"
#include "util.h"

char *xclbin_filename;

////////////////////////////////////////////////////////////////////////////////

data_bitwidth ifmap[BN_R * BN_I * (R * BS_R + K1 - 1) * (C * BS_C + K2 - 1) * BS_I * I];
data_bitwidth weight[BN_O * BN_I * O * BS_O * K1 * K2 * BS_I * I];
data_bitwidth ofmap_opencl[BN_R * BN_O * R * BS_R * C * BS_C * O * BS_O];

data_bitwidth ofmap_sw[BN_R * BN_O * R * BS_R * C * BS_C * O * BS_O];
data_bitwidth ofmap_c[BN_R * BN_O * R * BS_R * C * BS_C * O * BS_O];

int main(int argc, char **argv)
{
	int ifmap_length = BN_R * BN_I * (R * BS_R + K1 - 1) * (C * BS_C + K2 - 1) * BS_I * I;
	int wt_length = BN_O * BN_I * O * BS_O * K1 * K2 * BS_I * I;
	int ofmap_length = BN_R * BN_O * R * BS_R * C * BS_C * O * BS_O;

	unsigned int correct;               // number of correct results returned

	if (argc != 2)
	{
		printf("%s <inputfile>\n", argv[0]);
		return EXIT_FAILURE;
	}

	xclbin_filename = argv[1];

	// Fill our data sets with pattern
	//
	prepare_data(ifmap, ifmap_length);
	prepare_data(weight, wt_length);


	// Validate our results
	//
	systolic_golden(ifmap, weight, ofmap_sw);
	c_model(ifmap, weight, ofmap_c);

//	opencl_model(weight, ifmap, ofmap_opencl);

    int match = 0;
    for (int i = 0 ; i < ofmap_length; i++)
	{
#if (DATA_BITWIDTH == 32)
		if (fabs(ofmap_c[i] - ofmap_sw[i]) > 1e-3)
#else
			if (ofmap_c[i] != ofmap_sw[i])
#endif
		{
			std::cout << "Error: Result mismatch" << std::endl;
#if (DATA_BITWIDTH == 32)
			printf("ofmap_sw[%d]=%.3f. ofmap_c[%d]=%.3f\n", i, ofmap_sw[i], i, ofmap_c[i]);
#else
			printf("ofmap_sw[%d]=%d. ofmap_c[%d]=%d\n", i, ofmap_sw[i], i, ofmap_c[i]);
#endif
			match = 1;
			break;
		}
    }

    std::cout << "TEST " << (match ? "FAILED" : "PASSED") << std::endl; 

}
