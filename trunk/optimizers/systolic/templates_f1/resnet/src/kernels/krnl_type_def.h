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


#ifndef __KRNL_TYPE_DEF_H_
#define __KRNL_TYPE_DEF_H_

#include <ap_int.h>
#include "systolic_params.h"
#include "type_def.h"

// FIXME: NUM_PACKED_DATA must be divided by IN_PACK_SIZE
//#define BW_BITWIDTH 512 
#define BW_BITWIDTH (IN_PACK_SIZE * DATA_BITWIDTH) 
#define NUM_PACKED_DATA (BW_BITWIDTH / DATA_BITWIDTH)

typedef ap_int<BW_BITWIDTH> dram_trans_t;

typedef ap_int<DATA_BITWIDTH * IN_PACK_SIZE> ap_in_t;

typedef ap_int<DATA_BITWIDTH * (C + MK2 - 1) * IN_PACK_SIZE> ap_ifmap_t;
typedef ap_int<DATA_BITWIDTH * IN_PACK_SIZE> ap_filter_t;
typedef ap_int<DATA_BITWIDTH * C> ap_pe_out_t;
//typedef ap_int<DATA_BITWIDTH * OUT_PACK_SIZE> ap_out_t;
typedef ap_int<DATA_BITWIDTH * IN_PACK_SIZE> ap_out_t;

#endif
