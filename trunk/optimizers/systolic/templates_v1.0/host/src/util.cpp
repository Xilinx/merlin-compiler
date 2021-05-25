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


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "systolic_params.h"
#include "cnn.h"

void dump_float_into_file(float * data, size_t size, const char *sFilename)
{
	FILE * fp;
	if (!(fp=fopen(sFilename, "wb")))
	{
		printf("[ERROR] file %s failed to open for write.\n", sFilename);
		exit(0);
	}

	for (int i = 0; i < size/4; i++)
	{
		//fprintf(fp, "%d %.1lf\n", i, data[i]);
		fprintf(fp, "%d %lf\n", i, data[i]);
	}

	fclose(fp);
}


#define ACL_ALIGNMENT 64

void* acl_aligned_malloc (size_t size) {
	void *result = NULL;
	posix_memalign (&result, ACL_ALIGNMENT, size);
	return result;
}

void acl_aligned_free (void *ptr) {
	free (ptr);
}




void randomize_array(float* array, const int size)
{
	for (int i = 0; i < size; ++i) 
	{
		array[i] = (float)rand() / (float)RAND_MAX;
	}
}

void one_array(float* array, const int size)
{
	for (int i = 0; i < size; ++i) 
	{
	//	array[i] = (float)rand() / (float)RAND_MAX;
		array[i] = 1.0;
	}
}

void idx_array(float* array, const int size)
{
	//	printf("%d %s\n", __LINE__, __func__);
	for (int i = 0; i < size; ++i) 
	{
		//      array[i] = 0;
		array[i] = 1.0 * i;
	}
}

bool compare_L2_norm(const float* ref_array, const float* output_array, const unsigned int size, const float epsilon) 
{
	// compute the L^2-Norm of the difference between the output array and reference array 
	// and compare it against the L^2-Norm of the reference.
	float diff = 0.0f;
	float ref = 0.0f;
	for (int i = 0; i < size; ++i) {

		const float o = output_array[i];
		const float r = ref_array[i];
		const float d = o - r;
		diff += d * d;
		ref += r * r;
	}

	const float diff_l2norm = sqrtf(diff);
	const float ref_l2norm = sqrtf(ref);
	const float error = diff_l2norm / ref_l2norm;
	const bool pass = error < epsilon;

	return pass;
}



void printDiff(float *data1, float *data2, long int size, float fListTol)
{
	printf("Listing Differences > %.6f...\n", fListTol);
	int i,j,k;
	int error_count=0;
	for (i = 0; i < size; i++) 
	{
		float fDiff = fabs(data1[i] - data2[i]);
		if (fDiff > fListTol) 
		{                
			if (error_count < 300) {  // print only first 300 errors
				printf("Host[%d] = %.6f\tKernel[%d]=%.6f\tDiff=%.6f\n", i, data1[i], i, data2[i], fDiff);
			}

			error_count++;
		} else {
			if (error_count < 300) {  // print correct only within first 300 errors
				printf("Correct or nan? --> Host[%d] = %.6f\tKernel[%d]=%.6f\tDiff=%.6f\n", i, data1[i], i, data2[i], fDiff);
			}
		}
	}
	printf("\nTotal Errors = %d\n\n", error_count);
}

