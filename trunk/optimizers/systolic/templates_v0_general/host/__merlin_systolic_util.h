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


#ifndef __UTIL_H_
#define __UTIL_H_

#include <cstddef>

#include "__merlinhead_kernel_top.h"

void dump_data_precision_into_file(data_precision * data, size_t size, const char *sFilename);

void *acl_aligned_malloc(size_t size);
void acl_aligned_free(void *ptr);

void randomize_array(data_precision* array, const int size);
void one_array(data_precision* array, const int size);
void idx_array(data_precision* array, const int size);

bool compare_L2_norm(const data_precision* ref_array, const data_precision* output_array, const unsigned int size, const data_precision epsilon);


void printDiff(data_precision *data1, data_precision *data2, long int size, data_precision fListTol);

#endif // __UTIL_H_

