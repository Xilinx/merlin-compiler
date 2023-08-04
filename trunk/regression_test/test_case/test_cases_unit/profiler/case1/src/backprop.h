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
#ifndef _BACKPROP_H_
#define _BACKPROP_H_

#include "support.h"
#define BIGRND 0x7fffffff


#define ETA 0.3       //eta value
#define MOMENTUM 0.3  //momentum value
#define NUM_THREAD 8 //OpenMP threads

void workload(float delta[17], float ly[65537], float w[65537 * 17], float oldw[65537 * 17]);

struct bench_args_t {
  float delta[17];
  float ly[65537];
  float w[65537 * 17];
  float oldw[65537 * 17];
};


#endif
