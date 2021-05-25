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


#include "memcpy_128.h"
#define LARGE_BUS 128
#ifdef __cplusplus
#define MARS_WIDE_BUS_TYPE ap_uint<LARGE_BUS> 
#define c_get_range(tmp, x, y) tmp((y + x) * 8 - 1, y * 8)
#define c_set_range(tmp, x, y, val) tmp((y + x) * 8 - 1, y * 8) = val
#else
#define MARS_WIDE_BUS_TYPE merlin_uint_128
#define c_get_range(tmp, x, y) merlin_get_range_128(&tmp, y, x)
#define c_set_range(tmp, x, y, val) merlin_set_range_128(&tmp, y, x, val)
#endif
#include "mars_wide_bus_2d.h"
#undef LARGE_BUS
#undef MARS_WIDE_BUS_TYPE
#undef c_get_range
#undef c_set_range
