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


#ifndef __MERLIN_SYSTOLIC_ARRAY_DRIVER_H_
#define __MERLIN_SYSTOLIC_ARRAY_DRIVER_H_

#include <cstddef>

typedef float data_precision;
//typedef int data_precision;
//typedef char data_precision;

//void merlin_systolic_array_driver(const char *case_name);
void __merlin_systolic_kernel_top(const char *case_name, 
		data_precision *a, unsigned int num_elem_a,
		data_precision *b, unsigned int num_elem_b,
		data_precision *o_opencl, unsigned int num_elem_o);

// __merlin_systolic_util.h
void dump_data_precision_into_file(data_precision * data, size_t size, const char *sFilename);

void *acl_aligned_malloc(size_t size);
void acl_aligned_free(void *ptr);

void randomize_array(data_precision* array, const int size);
void one_array(data_precision* array, const int size);
void idx_array(data_precision* array, const int size);

bool compare_L2_norm(const data_precision* ref_array, const data_precision* output_array, const unsigned int size, const data_precision epsilon);

void printDiff(data_precision *data1, data_precision *data2, long int size, data_precision fListTol);

// __merlin_opencl_if.h
int __merlin_init(const char *bitstream);
int __merlin_release();

#endif
