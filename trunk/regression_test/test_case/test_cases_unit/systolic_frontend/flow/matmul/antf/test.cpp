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

#define M 4
#define N 128
#define K 64

#pragma ACCEL kernel
void matmul_kernel(float **C, const float **A, const float **B)
{
#pragma ACCEL interface variable=A depth=4,64
#pragma ACCEL interface variable=B depth=128,64
#pragma ACCEL interface variable=C depth=4,128

#pragma ACCEL systolic auto=auto
	for (int j = 0; j < N; j++)
	for (int i = 0; i < M; i++)
	for (int k = 0; k < K; k++) 
	{
		C[i][j] += A[i][k] * B[j][k];
	}

	return ;
}

