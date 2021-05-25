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


#ifndef TRUNK_LEGACY_TOOLS_IR_TEST_IDCT_AUTOPILOT_TECH_H_
#define TRUNK_LEGACY_TOOLS_IR_TEST_IDCT_AUTOPILOT_TECH_H_

typedef unsigned int uint1;
typedef unsigned int uint2;
typedef unsigned int uint3;
typedef unsigned int uint4;
typedef unsigned int uint5;
typedef unsigned int uint6;
typedef unsigned int uint7;
typedef unsigned int uint8;
typedef unsigned int uint9;
typedef unsigned int uint10;
typedef unsigned int uint11;
typedef unsigned int uint12;
typedef unsigned int uint13;
typedef unsigned int uint14;
typedef unsigned int uint15;
typedef unsigned int uint16;
typedef unsigned int uint17;
typedef unsigned int uint18;
typedef unsigned int uint19;
typedef unsigned int uint20;
typedef unsigned int uint21;
typedef unsigned int uint32;

typedef int int1;
typedef int int2;
typedef int int3;
typedef int int4;
typedef int int5;
typedef int int6;
typedef int int7;
typedef int int8;
typedef int int9;
typedef int int10;
typedef int int11;
typedef int int12;
typedef int int13;
typedef int int14;
typedef int int15;
typedef int int16;
typedef int int17;
typedef int int18;
typedef int int19;
typedef int int20;
typedef int int21;
typedef int int32;

#define apint_get_range(a, h, l) (((a) >> (l)) & ((1 << ((h) - (l) + 1)) - 1))

#endif  // TRUNK_LEGACY_TOOLS_IR_TEST_IDCT_AUTOPILOT_TECH_H_
