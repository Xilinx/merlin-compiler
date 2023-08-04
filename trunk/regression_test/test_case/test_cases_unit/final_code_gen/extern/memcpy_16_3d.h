//(C) Copyright 2016-2021 Xilinx, Inc.
//All Rights Reserved.
//
//Licensed to the Apache Software Foundation (ASF) under one
//or more contributor license agreements.  See the NOTICE file
//distributed with this work for additional information
//regarding copyright ownership.  The ASF licenses this file
//to you under the Apache License, Version 2.0 (the
//"License"); you may not use this file except in compliance
//with the License.  You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
//Unless required by applicable law or agreed to in writing,
//software distributed under the License is distributed on an
//"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
//KIND, either express or implied.  See the License for the
//specific language governing permissions and limitations
//under the License. (edited)
#ifdef __cplusplus
#include "ap_int.h"
#else
#include "ap_cint.h"
#endif
#define LARGE_BUS 16
#ifdef __cplusplus
//typedef ap_uint<LARGE_BUS> MARS_WIDE_BUS_TYPE;
#define MARS_WIDE_BUS_TYPE ap_uint<16>
#else
//typedef uint16 MARS_WIDE_BUS_TYPE;
#define MARS_WIDE_BUS_TYPE uint16
#endif
#include "mars_wide_bus_3d.h"
#undef MARS_WIDE_BUS_TYPE
#undef LARGE_BUS
