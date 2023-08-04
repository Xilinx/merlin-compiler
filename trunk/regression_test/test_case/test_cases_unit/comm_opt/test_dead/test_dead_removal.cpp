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
void kernel_top(int * a)
{
  int i;
  int j;
  int k;
  int m;
  int i1;
  int a0=10;
  int d[100];
  int b[100][100];
  int c[100][100][100];
  for (m = 1; m < 3; m++) {
	for (k = 0; k < 5; k++) {
	  for (j = 0; j < 10; j++) {
		for (i = 0; i < 100; i++){
		  c[i][j][k] += 0 + 1 + 2;
		  i1 = 10;
		}
	  }
	}
  }
  for (k = 0; k < 5; k++) {
	for (j = 0; j < 10; j++) {
	  for (i = 0; i < 100; i++){
		b[j][i] = 0 + 1 + 2 + a0 + i1;
	  }
	}
  }
  for (i = 0; i < 100; i++){
	d[i] += 0 + 1 + 2 + a[i] + i1;
  }
}

void main()
{
    int *a;
    kernel_top(a);
}
