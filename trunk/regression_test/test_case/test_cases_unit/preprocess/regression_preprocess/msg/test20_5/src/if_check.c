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
#include<stdlib.h>
#include<string.h>

const int DIM11 = 2;
const int DIM22 = 4;
#pragma ACCEL kernel
void top_kernel(int *aa,int *bb,int *c, int val[1]) {
	int i;
	int j;
	int a_buf[4];
	for (j = 0; j < DIM11; ++j) 
	{
		for (i = 0; i < DIM11; ++i) {
			a_buf[j + i] = 0;
		}
		for (i = 0; i < DIM11; ++i) {
			if(i < val[0])
				a_buf[j + i] = aa[j]+ bb[DIM11*j+i];
			c[i] += a_buf[i];
		}
	}

	return;
}

