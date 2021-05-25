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

#include "type_def.h"
#include "systolic_params.h"
#include "config.h"

static void conv2d(data_bitwidth *ifmap,
		data_bitwidth *weights,
		data_bitwidth *ofmap, 
		OpCFG &cfg, 
		int wt_offset)
{
}

static void kernel_sw_full(data_bitwidth *ifmap,
		data_bitwidth *weights,
		data_bitwidth *ofmap,
		OpCFG *cfg)
{
	int wt_offset = 0;

	conv2d(ifmap, weights, ofmap, cfg[0], wt_offset);
}

void c_model(data_bitwidth *ifmap,
		data_bitwidth *weights,
		data_bitwidth *ofmap, 
		OpCFG *cfg)
{
	kernel_sw_full(ifmap, weights, ofmap, cfg);
}
