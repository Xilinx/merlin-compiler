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


#define VEC_SIZE 100
#define VEC_SIZE_2 10000


void smithwaterman_kernel(int * string_1, int * string_2, int * score);

int main ( ) {
	int inc = 0;
	int * string_1; cmost_malloc_1d( &string_1, "+" , 4, VEC_SIZE);
	int * string_2; cmost_malloc_1d( &string_2, "0", 4, VEC_SIZE);
	int * score;   cmost_malloc_1d( &score, "0"  , 4, VEC_SIZE_2);
	if (!(string_1 && string_2 && score)) while(1);

    smithwaterman_kernel(string_1, string_2, score);


	cmost_dump_1d(score, "score_out.dat");
	cmost_free_1d(string_1);
	cmost_free_1d(string_2);
	cmost_free_1d(score);
	
	return 0;
}

