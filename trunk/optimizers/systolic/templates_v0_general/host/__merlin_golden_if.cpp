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



//; my $cfg_root = param_tree_define("dummy", "systolic_cfg.xml", "item_for_array");

//; my $expr_ub_h = $cfg_root->{Host_Params}->{IN_UB};
//; my $expr_ub_v = $cfg_root->{Host_Params}->{WT_UB};
//; my $expr_ub_o = $cfg_root->{Host_Params}->{OUT_UB};

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "block_wise_reformat.h"
//#include "cnn_golden.h"
//#include "matmul_golden.h"
#include "golden.h"


static void cnn(
		data_precision* out,
		data_precision* in,
		data_precision* weight
	   )
{
//#if (TEST_CNN == 1)
	conv(out, in, weight);
//#else
//	matmul(out, in, weight);
//#endif
}

static void cnn_block_wise(
		data_precision *out_block_wise,
		data_precision *out,
		data_precision *in,
		data_precision *weight
		)
{
	int	num_elem_input_fm_bw = `$expr_ub_h`;
	int	num_elem_weight_bw = `$expr_ub_v`;

	data_precision *in_block_wise = (data_precision *)malloc(sizeof(data_precision) * num_elem_input_fm_bw);
	data_precision *weight_block_wise = (data_precision *)malloc(sizeof(data_precision) * num_elem_weight_bw);

	block_wise_reformat_input(in, in_block_wise);
	block_wise_reformat_weight(weight, weight_block_wise);

//#if (TEST_CNN == 1)
	conv_block_wise(out_block_wise, in_block_wise, weight_block_wise);
//#else
//	matmul_block_wise(out_block_wise, in_block_wise, weight_block_wise);
//#endif

	block_wise_reformat_output(out_block_wise, out);

	free(in_block_wise);
	free(weight_block_wise);
}

void cnn_gold(
       data_precision *input_fm,
       data_precision *weight,
       data_precision *output_fm,
       data_precision *output_fm_block_wise,
	   int num_elem_output_fm
		)
{
    printf("CNN golden ... \n");
    fflush(stdout);

    printf("number of output is %d\n", num_elem_output_fm);

	data_precision *output_fm_golden = (data_precision *)malloc(sizeof(data_precision) * num_elem_output_fm);

    printf("Original CNN begins ... \n");
    fflush(stdout);

	cnn(output_fm_golden, input_fm, weight);
    for (int i = 0; i < 10; ++i) {
        printf("cnn golden result = %f\n", *(output_fm_golden + i));
    }

#if (TEST_GOLDEN_BW == 1)
	// for golden comparison
    printf("Block-wise CNN begins ... \n");
    fflush(stdout);

	cnn_block_wise(output_fm_block_wise, output_fm, input_fm, weight);

	int err = 0;
	for (int i = 0; i < num_elem_output_fm; i++) {
		if (fabs(output_fm_golden[i] - output_fm[i]) > 0.01) {
			if (err < 10) printf("output_fm_golden[%d]=%f, output_fm[%d]=%f\n", i, output_fm_golden[i], i, output_fm[i]);
			else if (err == 10) printf("...\n");
			err++;
		}
	}
#endif

	// for final golden comparison
	memcpy(output_fm, output_fm_golden, sizeof(data_precision) * num_elem_output_fm);

	free(output_fm_golden);

	printf("CNN golden finished \n");
}
