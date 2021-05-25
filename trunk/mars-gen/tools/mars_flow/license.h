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



#ifndef TRUNK_MARS_GEN_TOOLS_MARS_FLOW_LICENSE_H_
#define TRUNK_MARS_GEN_TOOLS_MARS_FLOW_LICENSE_H_
typedef enum {
  FALCON_XILINX = 0,
  FALCON_ALTERA,
  FALCON_CUSTOM,
  FALCON_RT,
  FALCON_DNA,
  FALCON_INT,
  FALCON_FCZIP,
  FALCON_EVAL
} Feature;

#ifdef __cplusplus
extern "C" {
#endif
// initialize licensing. only need to call once
int fc_license_init();
// cleanup licensing. only need to call once
int fc_license_cleanup();
// Check out a feature. It will error out if checkout fails
// If verbose != 0, will generate a message
int fc_license_checkout(int feature, int verbose);
// Check in a feature.
int fc_license_checkin(int feature);
// not defined yet.
int fc_license_exist(int feature);
#ifdef __cplusplus
};
#endif
#endif  // TRUNK_MARS_GEN_TOOLS_MARS_FLOW_LICENSE_H_
