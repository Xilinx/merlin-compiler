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
#pragma ACCEL kernel
void func_top_1(int N)
{
	int a[10];
	int b[10];
	int i;
	int j;
	for (i = 0; i < 1; i++) {
#pragma HLS loop_tripcount max=1
		for (j = 0; j < 10; j++) 
		{
			a[j] = b[j];
		}
	}

	for (i = 0; i < 1; i++) {
        int var1=1;
		for (j = 0; j < 10; j++) 
		{
			a[j] = b[j]+var1;
		}
	}

	for (i = 0; i < 1; i++) {
#pragma ACCEL pipeline
		for (j = 0; j < 10; j++) 
		{
			a[j] = b[j];
		}
	}

	for (i = 0; i < N; i++) {
#pragma HLS loop_tripcount max=1
		for (j = 0; j < 10; j++) 
		{
			a[j] = b[j];
		}
	}
	
    for (i = 0; i < 1; i++) {
        if(N > 0) {
            for (j = 0; j < 10; j++) 
            {
                a[j] = b[j];
            }
        }
    }

}

#pragma ACCEL kernel
void func_top_4()
{
	int a[100][100];
	int b[100][100];
	int i;
	int j;
	for (i = 0; i < 1; i++) {
		for (j = 0; j < 8; j++) 
		{
			a[i][j] = b[i][j];
		}
		for (j = 0; j < 8; j++) 
		{
			a[i + 1][j] = b[i][j + 1];
		}
	}

    for (i = 0; i < 1; i++) {
        for (int k = 0; k < 1; k++) {
#pragma ACCEL parallel
            for (j = 0; j < 8; j++) 
            {
                a[i + 1][j] = b[i][j + 1];
            }
        }
    }
}

void sub_0(int a[100],int b[100][100])
{
	int i;
	int j;
	for (i = 0; i < 1; i++) {
		for (j = 0; j < 8; j++) 
		{
			a[i] = b[i][j];
		}
	}
}
#pragma ACCEL kernel
void func_top_9()
{
	int a[100][100];
	int b[100][100];
	for (int i = 0; i < 2; i++) {
	    sub_0(a[i],b);
    }
}


