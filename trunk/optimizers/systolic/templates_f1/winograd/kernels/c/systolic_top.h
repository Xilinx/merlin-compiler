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


#ifndef __SYSTOLIC_TOP_H_
#define __SYSTOLIC_TOP_H_

#include "systolic_params.h"

#define DEBUG_EXCEPTION 0

#define DEBUG_KERNEL_ORDER 1

//#define DEBUG_SHIFT_REG_PARTITION 1

#define DRAIN_TREE 0



extern "C" {

	void systolic_top(vec_input_t *ifmap, vec_input_t *weight, vec_output_t *ofmap);
//	void systolic_top(vec_input_t *ifmap, vec_input_t *weight, data_bitwidth *ofmap);
//	void systolic_top(data_bitwidth *ifmap, data_bitwidth *weight, data_bitwidth *ofmap);
}

#endif
