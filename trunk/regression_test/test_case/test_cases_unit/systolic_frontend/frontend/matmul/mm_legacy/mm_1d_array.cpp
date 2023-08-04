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

#define M 64
#define N 64
#define K 64

#pragma ACCEL kernel
void compute_gold(float* C, const float* A, const float* B) {

#pragma ACCEL systolic auto=auto
	for (int r = 0; r < M; r++)
	for (int o = 0; o < N; o++)
	for (int i = 0; i < K; i++) 
	{
	//	C[r * N + o] += B[i * N + o] * A[r * K + i];
		C[r * N + o] += B[o * K + i] * A[r * K + i];
	//	C[r][o] += B[i][o] * A[r][i];
	}

	return ;
}

