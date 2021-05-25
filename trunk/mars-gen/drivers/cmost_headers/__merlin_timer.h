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


#ifndef CMOST_TIME_H
#define CMOST_TIME_H
#ifdef __cplusplus
#define _Bool bool
extern "C" {
#endif

void start_timer();
double read_timer();
void acc_pcie_timer();
void acc_comp_timer();
void report_timer(const char *kernel_name);
void timer_begin(int index);
void timer_comp_end(int index);
void timer_total(int index);
void wrapper_start();
void wrapper_end();
void print_time();
#ifdef __cplusplus
}
#endif
#endif
