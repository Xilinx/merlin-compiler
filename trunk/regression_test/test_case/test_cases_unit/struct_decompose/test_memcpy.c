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
#include <string.h>

#define N 100
struct st {
  int alpha[N];
  int beta[N];
};

#pragma ACCEL kernel
void top(struct st *a, int *res) {
#pragma ACCEL interface variable=a depth=100
	int i;

	memcpy(res, a->alpha, 4 * N);

  for (i = 0; i < N; ++i) {
	res[i] = (res[i] + a->beta[i]) * 2;
  }
  return;
}

int main() {
	int i;
  struct st a;
  int arr[N];

  for (i = 0; i < N; ++i) {
    a.alpha[i] = i;
    a.beta[i] = N - i;
  }

  top(&a, arr);

  return 0;
}
