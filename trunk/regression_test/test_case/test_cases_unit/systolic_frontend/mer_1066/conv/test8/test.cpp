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
#include "test.h"

#pragma ACCEL kernel
void matmul_kernel(float ***out, const float ***in, const float ****weight)
{
#pragma ACCEL interface variable=in depth=112,112,168
#pragma ACCEL interface variable=weight depth=192,1,1,168
#pragma ACCEL interface variable=out depth=112,112,192

#pragma ACCEL systolic auto=auto
	for (int o = 0; o < IMG_OUT_NUM; o++)
	for (int i = 0; i < IMG_IN_NUM; i++)
	for (int r = 0; r < IMG_ROW; r++)
	for (int c = 0; c < IMG_COL; c++)
	for (int p = 0; p < K; p++)
	for (int q = 0; q < K; q++)
	{
		out[r][c][o] += in[r + p][c + q][i] * weight[o][p][q][i];
	}

	return ;
}

