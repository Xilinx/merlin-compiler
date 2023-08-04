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


////#define VEC_SIZE 10000
////
////void vec_add_kernel(int *a, int *b, int*c, int inc)
////{
////    int j;
////    for (j = 0 ;j < VEC_SIZE; j++) c[j] = a[j]+b[j]+inc;
////}


#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <hls_stream.h>
#include <ap_utils.h>

#define query_row_num  9665
#define query_col_num  3

#define index_row_num  9665
#define index_col_num  1

#define dist_row_num  9665
#define dist_col_num  1

#define target_row_num  18213
#define target_col_num  3

#define MAX_VISITED_NODES 100


void fun_a (
		hls::stream<int>	&output_a,
		hls::stream<int>	&input_a
)
{
	int i;
	int data[10] = {0};
	int dummy = 0;
	for (i = 0; i < 10; i++)
	{
		output_a.write(data[i] + dummy);
		printf (" A	writing done.\n");
		//while (input_a.empty()) {}
//        ap_wait();
//		dummy = input_a.read();
//		printf (" A	reading done.\n");
	}
	return;
}

void fun_b (
		hls::stream<int>	&input_b,
		hls::stream<int>	&output_b
)
{
	int i;
	for (i = 0; i < 10; i++)
	{
		//while (input_b.empty()) {}
		int dummy = input_b.read();
		printf (" B	reading done.\n");
//        ap_wait();
//		output_b.write(dummy);
//		printf (" B	writing done.\n");
	}
	return;
}


extern "C" {
void vec_add_kernel(int *a, int *b, int*c, int inc)
{
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// define wires here
	hls::stream<int> 	a_to_b;
	hls::stream<int> 	b_to_a;
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	
	{
	#pragma HLS dataflow
		fun_a(a_to_b, b_to_a);
		fun_b(a_to_b, b_to_a);
		
	}

    c[0] = 0;

	return;
}
}

