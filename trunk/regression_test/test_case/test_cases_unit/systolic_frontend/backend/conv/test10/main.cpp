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

#include <stdio.h>
#include "__merlinhead_kernel_top.h"
//#include "__merlin_systolic_util.h"
//#include "./src/test1.h"

int main(int argc, char **argv)
{
	char *bin_file = argv[1];
	__merlin_init(bin_file);

	// input/output data containers
	float *a;
	float *b;
	float *c;
	unsigned int num_elem_a = (16 + 3 - 1) * (10 + 3 - 1) * 32;
	unsigned int num_elem_b = 137 * 3 * 3 * 32;
	unsigned int num_elem_c = 16 * 8 * 137;

	// memory allocation
	if ((a = (float *)acl_aligned_malloc(num_elem_a * sizeof(float))) == NULL)
	{
		perror("Failed malloc of a");
	}
	if ((b = (float *)acl_aligned_malloc(num_elem_b * sizeof(float))) == NULL)
	{
		perror("Failed malloc of b");
	}
	if ((c = (float *)acl_aligned_malloc(num_elem_c * sizeof(float))) == NULL)
	{
		perror("Failed malloc of c");
	}

	randomize_array(a, num_elem_a);
//	idx_array(a, num_elem_a);
//	one_array(a, num_elem_a);
	randomize_array(b, num_elem_b);
//	one_array(b, num_elem_b);
	randomize_array(c, num_elem_c);
	
	// init
	// kernel call
	// FIXME: argv[1] will not be needed in the future
	__merlin_systolic_kernel_top(argv[1],
			a, num_elem_a, 
			b, num_elem_b, 
			c, num_elem_c);

	// release
	acl_aligned_free(a);
	acl_aligned_free(b);
	acl_aligned_free(c);

	__merlin_release();
}
