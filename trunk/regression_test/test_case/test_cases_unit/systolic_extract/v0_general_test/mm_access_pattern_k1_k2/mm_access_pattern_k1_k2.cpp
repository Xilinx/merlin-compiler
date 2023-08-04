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
#define BN_I 1
#define BN_J 2
#define BN_K 2
//#define BS_I 16
//#define BS_J 8
//#define BS_K 32
//#define PE_ROWS 4
//#define PE_COLS 4
//#define PE_DSIZE 8

#define BS_I 2
#define BS_J 2
#define BS_K 32
#define PE_ROWS 2
#define PE_COLS 2
#define PE_DSIZE 8


#pragma ACCEL kernel 
void matmul_kernel(int **c, int **a, int **b)
{
#pragma ACCEL systolic
	for (int i0 = 0; i0 < BN_I; i0++)
	for (int j0 = 0; j0 < BN_J; j0++)
	for (int k0 = 0; k0 < BN_K; k0++)
	{
#pragma ACCEL data_reuse scope 
		for (int k1 = 0; k1 < BS_K; k1++)
		for (int i1 = 0; i1 < BS_I; i1++)
		for (int j1 = 0; j1 < BS_J; j1++)
		{
#pragma ACCEL parallel scope
			for (int i2 = 0; i2 < PE_ROWS; i2++)
			for (int j2 = 0; j2 < PE_COLS; j2++)
			for (int k2 = 0; k2 < PE_DSIZE; k2++)
			{
				int i = i0 * BS_I * PE_ROWS + i2 * BS_I + i1;
				int j = j0 * BS_J * PE_COLS + j2 * BS_J + j1;
			//	int k = k0 * BS_K * PE_DSIZE + k2 * BS_K + k1;
				int k = k0 * BS_K * PE_DSIZE + k1 * PE_DSIZE + k2;

				c[i][j] += a[i][k] * b[j][k];
			}
		}
	}
}
