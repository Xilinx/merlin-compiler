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



#include <stdlib.h>

#include "type_def.h"
//#include "systolic_params.h"
#include "config.h"

void randomize_array(data_bitwidth *array, const int size)
{
	for (int i = 0; i < size; ++i) 
	{
	//	array[i] = (data_bitwidth)rand() / (data_bitwidth)RAND_MAX;
		array[i] = (data_bitwidth)(rand() % 10);
	}
}

void one_array(data_bitwidth *array, const int size)
{
	for (int i = 0; i < size; ++i) 
	{
	//	array[i] = (data_bitwidth)rand() / (data_bitwidth)RAND_MAX;
		array[i] = 1;
	}
}

void idx_array(data_bitwidth *array, const int size)
{
	//	printf("%d %s\n", __LINE__, __func__);
	for (int i = 0; i < size; ++i) 
	{
		//      array[i] = 0;
		array[i] = 1 * i;
	}
}

int get_num_conv()
{
	int num_conv = 0;

	for (int i = 0; i < BUILDING_BLOCK_NUM; i++)
	{
		num_conv += building_block_content[i];
	}

	return num_conv;
}
