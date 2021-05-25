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


#ifndef __CONFIG_H_
#define __CONFIG_H_

#include "systolic_params.h"

#define TEST_CONFIG 1

#if (TEST_CONFIG == 1)
const unsigned int cfg_in_num[] =	{2};
const unsigned int cfg_out_num[] =	{4};
const unsigned int cfg_img_row[] =	{4};
const unsigned int cfg_img_col[] =	{4};
const unsigned int cfg_k1[] =		{1};
const unsigned int cfg_k2[] =		{1};
const unsigned int cfg_conv_stride[] =	{1};
const unsigned int cfg_pooling_stride[] =	{1}; 
#else
const unsigned int cfg_in_num[] =	{3,		64,	64,	64,		64};
const unsigned int cfg_out_num[] =	{64,	64,	64,	256,	256};
const unsigned int cfg_img_row[] =	{224,	56,	56,	56,		56};
const unsigned int cfg_img_col[] =	{224,	56,	56,	56,		56};
const unsigned int cfg_k1[] =		{1,		1,	3,	1,		1};
const unsigned int cfg_k2[] =		{1,		1,	3,	1,		1};
// FIXME:
const unsigned int cfg_conv_stride[] =	{1,		1,	1,	1,		1};
// FIXME: we set the pooling stride for the 1st layer 4 to keep conv stride 1.
const unsigned int cfg_pooling_stride[] =	{4,		1,	1,	1,		1}; 
#endif

#if 0
const unsigned int cfg_BN_R[] = {1,	1,	1,	1,	1};
const unsigned int cfg_BN_C[] = {1,	1,	1,	1,	1};
const unsigned int cfg_BN_O[] = {1,	1,	1,	1,	1};
const unsigned int cfg_BN_I[] = {1,	1,	1,	1,	1};
#endif

typedef struct 
{
	int BN_R;
	int BN_C;
	int BN_O;
	int BN_I;
	int k1;
	int k2;
	int conv_stride; // conv stride
	int pooling_stride; // pooling stride
} OpCFG;

#if 0
const unsigned int cfg_BS_R[] = {BS_R,	BS_R,	BS_R,	BS_R,	BS_R};
const unsigned int cfg_BS_C[] = {BS_C,	BS_C,	BS_C,	BS_C,	BS_C};
const unsigned int cfg_BS_O[] = {BS_O,	BS_O,	BS_O,	BS_O,	BS_O};
const unsigned int cfg_BS_I[] = {BS_I,	BS_I,	BS_I,	BS_I,	BS_I};
#endif

#define BUILDING_BLOCK_NUM 1

const unsigned int building_block_content[] = {1, 4, 3, 3, 4, 3, 3, 3, 4, 3, 3, 3, 3, 3, 4, 3, 3};

#define MAX_ACTIVATION_BUF_LEN (56 * 56 * 256)

#endif
